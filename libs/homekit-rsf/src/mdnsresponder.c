/*
 * Apple HomeKit Bonjour implementation
 * (c) José Antonio Jiménez Campos (@RavenSystem)
 */

/*
 * Basic multicast DNS responder
 * 
 * Advertises the IP address, port, and characteristics of a service to other
 * devices using multicast DNS on the same LAN, so they can find devices with
 * addresses dynamically allocated by DHCP. See avahi, Bonjour, etc. See
 * RFC6762, RFC6763
 *
 * This sample code is in the public domain.
 *
 * by M J A Hamel 2016
 */

#include <string.h>
#include <stdio.h>

#ifdef ESP_PLATFORM

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <semphr.h>
#include "esp_wifi.h"
#include "esp_netif.h"

#define LOCK_TCPIP_CORE()
#define UNLOCK_TCPIP_CORE()

#include "adv_logger.h"

#define HOMEKIT_MDNS_PRINTF(message, ...)       adv_logger_printf(message, ##__VA_ARGS__)

#else

#include <espressif/esp_common.h>
#include <espressif/esp_wifi.h>
#include <etstimer.h>
#include <esplibs/libmain.h>
#include <FreeRTOS.h>
#include <timers.h>
#include <task.h>
#include <semphr.h>

#define HOMEKIT_MDNS_PRINTF(message, ...)       printf(message, ##__VA_ARGS__)

#endif

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/dns.h>
#include <lwip/prot/dns.h>
#include <lwip/prot/iana.h>
#include <lwip/udp.h>
#include <lwip/igmp.h>
#include <lwip/netif.h>
#include <timers_helper.h>

#include "mdnsresponder.h"

#if !LWIP_IGMP
#error "LWIP_IGMP needs to be defined in lwipopts.h"
#endif

// #define qDebugLog             // Log activity generally
// #define qLogIncoming          // Log all arriving multicast packets
// #define qLogAllTraffic        // Log and decode all mDNS packets

//-------------------------------------------------------------------

#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif

PACK_STRUCT_BEGIN
/** DNS message header */
struct mdns_hdr {
    PACK_STRUCT_FIELD(u16_t id);
    PACK_STRUCT_FIELD(u8_t flags1);
    PACK_STRUCT_FIELD(u8_t flags2);
    PACK_STRUCT_FIELD(u16_t numquestions);
    PACK_STRUCT_FIELD(u16_t numanswers);
    PACK_STRUCT_FIELD(u16_t numauthrr);
    PACK_STRUCT_FIELD(u16_t numextrarr);
}PACK_STRUCT_STRUCT;
PACK_STRUCT_END

#define SIZEOF_DNS_HDR 12

PACK_STRUCT_BEGIN
/** MDNS query message structure */
struct mdns_query {
    /* MDNS query record starts with either a domain name or a pointer
     to a name already present somewhere in the packet. */
    PACK_STRUCT_FIELD(u16_t type);
    PACK_STRUCT_FIELD(u16_t class);
}PACK_STRUCT_STRUCT;
PACK_STRUCT_END

#define SIZEOF_DNS_QUERY 4

PACK_STRUCT_BEGIN
/** MDNS answer message structure */
struct mdns_answer {
    /* MDNS answer record starts with either a domain name or a pointer
     to a name already present somewhere in the packet. */
    PACK_STRUCT_FIELD(u16_t type);
    PACK_STRUCT_FIELD(u16_t class);
    PACK_STRUCT_FIELD(u32_t ttl);
    PACK_STRUCT_FIELD(u16_t len);
}PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#define SIZEOF_DNS_ANSWER 10

PACK_STRUCT_BEGIN
struct mdns_rr_srv {
    /* RR SRV  */
    PACK_STRUCT_FIELD(u16_t prio);
    PACK_STRUCT_FIELD(u16_t weight);
    PACK_STRUCT_FIELD(u16_t port);
}PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#define SIZEOF_DNS_RR_SRV 6


#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#define kDummyDataSize      8           // arbitrary, dynamically resized
#define kMaxNameSize        64
#define kMaxQStr            128         // max incoming question key handled

typedef struct mdns_rsrc {
    struct mdns_rsrc*    rNext;
    u16_t   rType;
    u32_t   rTTL;
    u16_t   rKeySize;
    u16_t   rDataSize;
    char    rData[kDummyDataSize];      // Key, as C str with . seperators, followed by data in network-ready form
                                        // at rData[rKeySize]
} mdns_rsrc;

static struct udp_pcb* gMDNS_pcb = NULL;
static const ip_addr_t gMulticastV4Addr = DNS_MQUERY_IPV4_GROUP_INIT;
#if LWIP_IPV6
#include "lwip/mld6.h"
static const ip_addr_t gMulticastV6Addr = DNS_MQUERY_IPV6_GROUP_INIT;
#endif
static SemaphoreHandle_t gDictMutex = NULL;
static mdns_rsrc*      gDictP = NULL;       // RR database, linked list

static u8_t* mdns_response = NULL;
static u16_t mdns_responder_reply_size = 0;

#define MDNS_TTL_MULTIPLIER_MS      (1000)  // Set to 1000 to use standard time
#define MDNS_TTL_SAFE_MARGIN        (7)
static uint32_t mdns_ttl = 4500;
static uint32_t mdns_ttl_period = 4500;

#define MDNS_STATUS_PROBING_1       (0)
#define MDNS_STATUS_PROBING_2       (1)
#define MDNS_STATUS_PROBE_OK        (2)
#define MDNS_STATUS_WORKING         (3)

static u8_t mdns_status = MDNS_STATUS_WORKING;

//---------------------- Debug/logging utilities -------------------------

    // DNS field TYPE used for "Resource Records", some additions
    #define DNS_RRTYPE_AAAA           28    /* IPv6 host address */
    #define DNS_RRTYPE_SRV            33    /* Service record */
    #define DNS_RRTYPE_OPT            41    /* EDNS0 OPT record */
    #define DNS_RRTYPE_NSEC           47    /* NSEC record */
    #define DNS_RRTYPE_TSIG           250   /* Transaction Signature */
    #define DNS_RRTYPE_ANY            255   /* Not a DNS type, but a DNS query type, meaning "all types"*/

    // DNS field CLASS used for "Resource Records" 
    #define DNS_RRCLASS_ANY           255  /* Any class (q) */

    #define DNS_FLAG1_RESP            0x80
    #define DNS_FLAG1_OPMASK          0x78
    #define DNS_FLAG1_AUTH            0x04
    #define DNS_FLAG1_TRUNC           0x02
    #define DNS_FLAG1_RD              0x01
    #define DNS_FLAG2_RA              0x80
    #define DNS_FLAG2_RESMASK         0x0F

#ifdef qDebugLog
    static char qstr[12];

    static char* mdns_qrtype(uint16_t typ)
    {
        switch(typ) {
            case DNS_RRTYPE_A    : return ("A");
            case DNS_RRTYPE_NS   : return ("NS");
            case DNS_RRTYPE_PTR  : return ("PTR");
            case DNS_RRTYPE_TXT  : return ("TXT ");
            case DNS_RRTYPE_AAAA : return ("AAAA");
            case DNS_RRTYPE_SRV  : return ("SRV ");
            case DNS_RRTYPE_NSEC : return ("NSEC ");
            case DNS_RRTYPE_ANY  : return ("ANY");
        }
        sprintf(qstr, "type %d", typ);
        return qstr;
    }

#ifdef qLogAllTraffic

        static void mdns_printhex(u8_t* p, int n)
        {
            int i;
            for (i=0; i<n; i++) {
                HOMEKIT_MDNS_PRINTF("%02X ",*p++);
                if ((i % 32) == 31) HOMEKIT_MDNS_PRINTF("\n");
            }
            HOMEKIT_MDNS_PRINTF("\n");
        }

        static void mdns_print_pstr(u8_t* p)
        {
            int i, n;
            char* cp;

            n = *p++;
            cp = (char*)p;
            for (i = 0; i < n; i++) {
                putchar(*cp++);
            }
        }

        static char cstr[16];

        static char* mdns_qclass(uint16_t cls)
        {
            switch(cls) {
                case DNS_RRCLASS_IN  : return ("In");
                case DNS_RRCLASS_ANY : return ("ANY");
            }
            sprintf(cstr,"class %d",cls);
            return cstr;
        }

        // Sequence of Pascal strings, terminated by zero-length string
        // Handles compression, returns ptr to next item
        static u8_t* mdns_print_name(u8_t* p, struct mdns_hdr* hp)
        {
            char* cp = (char*)p;
            int i, n;

            do {
                n = *cp++;
                if ((n & 0xC0) == 0xC0) {
                    n = (n & 0x3F) << 8;
                    n |= (u8_t)*cp++;
                    mdns_print_name((u8_t*)hp + n, hp);
                    n = 0;
                } else if (n & 0xC0) {
                    HOMEKIT_MDNS_PRINTF("<label $%X?>",n);
                    n = 0;
                } else {
                    for (i = 0; i < n; i++)
                        putchar(*cp++);
                    if (n != 0) putchar('.');
                }
            } while (n > 0);
            return (u8_t*)cp;
        }


        static u8_t* mdns_print_header(struct mdns_hdr* hdr)
        {
            if (hdr->flags1 & DNS_FLAG1_RESP) {
                HOMEKIT_MDNS_PRINTF("Response, ID $%X %s ", htons(hdr->id), (hdr->flags1 & DNS_FLAG1_AUTH) ? "Auth " : "Non-auth ");
                if (hdr->flags2 & DNS_FLAG2_RA) HOMEKIT_MDNS_PRINTF("RA ");
                if ((hdr->flags2 & DNS_FLAG2_RESMASK) == 0) HOMEKIT_MDNS_PRINTF("noerr");
                                       else HOMEKIT_MDNS_PRINTF("err %d", hdr->flags2 & DNS_FLAG2_RESMASK);
            } else {
                HOMEKIT_MDNS_PRINTF("Query, ID $%X op %d", htons(hdr->id), (hdr->flags1 >> 4) & 0x7 );
            }
            if (hdr->flags1 & DNS_FLAG1_RD) HOMEKIT_MDNS_PRINTF("RD ");
            if (hdr->flags1 & DNS_FLAG1_TRUNC) HOMEKIT_MDNS_PRINTF("[TRUNC] ");

            HOMEKIT_MDNS_PRINTF(": %d questions", htons(hdr->numquestions) );
            if (hdr->numanswers != 0)
                HOMEKIT_MDNS_PRINTF(", %d answers",htons(hdr->numanswers));
            if (hdr->numauthrr != 0)
                HOMEKIT_MDNS_PRINTF(", %d auth RR",htons(hdr->numauthrr));
            if (hdr->numextrarr != 0)
                HOMEKIT_MDNS_PRINTF(", %d extra RR",htons(hdr->numextrarr));
            putchar('\n');
            return (u8_t*)hdr + SIZEOF_DNS_HDR;
        }

        // Copy needed because it may be misaligned
        static u8_t* mdns_print_query(u8_t* p)
        {
            struct mdns_query q;
            uint16_t c;

            memcpy(&q, p, SIZEOF_DNS_QUERY);
            c = htons(q.class);
            HOMEKIT_MDNS_PRINTF(" %s %s", mdns_qrtype(htons(q.type)), mdns_qclass(c & 0x7FFF) );
            if (c & 0x8000) HOMEKIT_MDNS_PRINTF(" unicast-req");
            HOMEKIT_MDNS_PRINTF("\n");
            return p + SIZEOF_DNS_QUERY;
        }

        // Copy needed because it may be misaligned
        static u8_t* mdns_print_answer(u8_t* p, struct mdns_hdr* hp)
        {
            struct mdns_answer ans;
            u16_t rrlen, atype, rrClass;;

            memcpy(&ans,p,SIZEOF_DNS_ANSWER);
            atype = htons(ans.type);
            rrlen = htons(ans.len);
            rrClass = htons(ans.class);
            HOMEKIT_MDNS_PRINTF(" %s %s TTL %d ", mdns_qrtype(atype), mdns_qclass(rrClass & 0x7FFF), htonl(ans.ttl));
            if (rrClass & 0x8000) 
                HOMEKIT_MDNS_PRINTF("cache-flush ");
            if (rrlen > 0) {
                u8_t* rp = p + SIZEOF_DNS_ANSWER;
                if (atype == DNS_RRTYPE_A && rrlen == 4) {
                    HOMEKIT_MDNS_PRINTF("%d.%d.%d.%d\n",rp[0],rp[1],rp[2],rp[3]);
                } else if (atype == DNS_RRTYPE_PTR) {
                    mdns_print_name(rp, hp);
                    HOMEKIT_MDNS_PRINTF("\n");
                } else if (atype == DNS_RRTYPE_TXT) {
                    mdns_print_pstr(rp);
                    HOMEKIT_MDNS_PRINTF("\n");
                } else if (atype == DNS_RRTYPE_SRV && rrlen > SIZEOF_DNS_RR_SRV) {
                    struct mdns_rr_srv srvRR;
                    memcpy(&srvRR, rp, SIZEOF_DNS_RR_SRV);
                    HOMEKIT_MDNS_PRINTF("prio %d, weight %d, port %d, target ", srvRR.prio, srvRR.weight, ntohs(srvRR.port));
                    mdns_print_name(rp + SIZEOF_DNS_RR_SRV, hp);
                    HOMEKIT_MDNS_PRINTF("\n");
                } else {
                    HOMEKIT_MDNS_PRINTF("%db:", rrlen);
                    mdns_printhex(rp, rrlen);
                }
            } else
                HOMEKIT_MDNS_PRINTF("\n");
            return p + SIZEOF_DNS_ANSWER + rrlen;
        }

        static int mdns_print_msg(u8_t* msgP, int msgLen)
        {
            int i;
            u8_t *tp;
            u8_t *limP = msgP + msgLen;
            struct mdns_hdr* hdr;

            hdr = (struct mdns_hdr*) msgP;
            tp = mdns_print_header(hdr);
            for (i = 0; i < htons(hdr->numquestions); i++) {
                HOMEKIT_MDNS_PRINTF(" Q%d: ", i + 1);
                tp = mdns_print_name(tp, hdr);
                tp = mdns_print_query(tp);
                if (tp > limP) return 0;
            }

            for (i = 0; i < htons(hdr->numanswers); i++) {
                HOMEKIT_MDNS_PRINTF(" A%d: ", i + 1);
                tp = mdns_print_name(tp, hdr);
                tp = mdns_print_answer(tp, hdr);
                if (tp > limP) return 0;
            }

            for (i = 0; i < htons(hdr->numauthrr); i++) {
                HOMEKIT_MDNS_PRINTF(" AuRR%d: ", i + 1);
                tp = mdns_print_name(tp, hdr);
                tp = mdns_print_answer(tp, hdr);
                if (tp > limP) return 0;
            }

            for (i = 0; i < htons(hdr->numextrarr); i++) {
                HOMEKIT_MDNS_PRINTF(" ExRR%d: ", i + 1);
                tp = mdns_print_name(tp, hdr);
                tp = mdns_print_answer(tp, hdr);
                if (tp > limP) return 0;
            }
            return 1;
        }
#endif // qLogAllTraffic
#endif // qDebugLog

//---------------------------------------------------------------------------

// Convert a DNS domain name label sequence into C string with . seperators
// Handles compression
static u8_t* mdns_labels2str(u8_t* hdrP, u8_t* p, char* qStr)
{
    int i, n;

    do {
        n = *p++;
        if ((n & 0xC0) == 0xC0) {
            n = (n & 0x3F) << 8;
            n |= (u8_t)*p++;
            mdns_labels2str( hdrP, hdrP + n, qStr);
            return p;
        } else if (n & 0xC0) {
            //HOMEKIT_MDNS_PRINTF("mdns_labels2str,label $%X?",n);
            return p;
        } else {
            for (i = 0; i < n; i++)
                *qStr++ = *p++;
            if (n == 0) *qStr++ = 0;
                 else *qStr++ = '.';
        }
    } while (n>0);
    return p;
}

// Encode a <string>.<string>.<string> as a sequence of labels, return length
static int mdns_str2labels(const char* name, u8_t* lseq, int max)
{
    int i, n, sdx, idx = 0;
    int lc = 0;

    do {
        sdx = idx;
        while (name[idx] != '.' && name[idx] != 0) idx++;
        n = idx - sdx;
        if (lc + 1 + n > max) {
            HOMEKIT_MDNS_PRINTF("! mDNS size %d\n", lc + 1 + n);
            return 0;
        }
        *lseq++ = n;
        lc++;
        for (i = 0; i < n; i++)
            *lseq++ = name[sdx + i];
        lc += n;
        if (name[idx] == '.')
            idx++;
    } while (n > 0);
    return lc;
}

// Unpack a DNS question RR at qp, return pointer to next RR
static u8_t* mdns_get_question(u8_t* hdrP, u8_t* qp, char* qStr, uint16_t* qClass, uint16_t* qType, u8_t* qUnicast)
{
    struct mdns_query qr;
    uint16_t cls;

    qp = mdns_labels2str(hdrP, qp, qStr);
    memcpy(&qr, qp, SIZEOF_DNS_QUERY);
    *qType = htons(qr.type);
    cls = htons(qr.class);
    *qUnicast = cls >> 15;
    *qClass = cls & 0x7FFF;
    return qp + SIZEOF_DNS_QUERY;
}

//---------------------------------------------------------------------------
static void mdns_announce_netif(struct netif *netif, const ip_addr_t *addr);

static TimerHandle_t mdns_announce_timer = NULL;

int mdns_buffer_init(uint16_t new_size) {
    if (mdns_response == NULL) {
        if (new_size == 0) {
            new_size = MDNS_RESPONDER_REPLY_SIZE_DEFAULT;
        }
        
        mdns_response = malloc(new_size);
    }
    
    if (mdns_response == NULL) {
        HOMEKIT_MDNS_PRINTF("! mDNS alloc %d\n", mdns_responder_reply_size);
        return -1;
    }
    
    mdns_responder_reply_size = new_size;
    
    return 0;
}

void mdns_buffer_deinit() {
    if (mdns_response != NULL) {
        free(mdns_response);
        mdns_responder_reply_size = 0;
        mdns_response = NULL;
    }
}

void mdns_clear() {
    mdns_announce_stop();
    
    if (!xSemaphoreTake(gDictMutex, portMAX_DELAY))
        return;
    
    mdns_rsrc *rsrc = gDictP;
    gDictP = NULL;

    while (rsrc) {
        mdns_rsrc *next = rsrc->rNext;
        free(rsrc);
        rsrc = next;
    }

    mdns_buffer_deinit();
    
    xSemaphoreGive(gDictMutex);
}


void mdns_TXT_append(char* txt, size_t txt_size, const char* record, size_t record_size)
{
    size_t txt_len = strlen(txt);

    if (record_size > 255) {
        char *s = strndup(record, record_size);
        HOMEKIT_MDNS_PRINTF("! mDNS %s too long\n", s); // longer than 255
        free(s);
        return;
    }

    if (txt_len + record_size + 2 > txt_size) {  // extra 2 is for length and terminator
        char *s = strndup(record, record_size);
        HOMEKIT_MDNS_PRINTF("! mDNS adding TXT %s\n", s);
        free(s);
        return;
    }

    txt[txt_len] = record_size;
    memcpy(txt + txt_len + 1, record, record_size);
    txt[txt_len + record_size + 1] = 0;
}


// Add a record to the RR database list
static void mdns_add_response(const char* vKey, u16_t vType, u32_t ttl, const void* dataP, u16_t vDataSize)
{
    mdns_rsrc* rsrcP;
    int keyLen, recSize;

    keyLen = strlen(vKey) + 1;
    recSize = sizeof(mdns_rsrc) - kDummyDataSize + keyLen + vDataSize;
    rsrcP = (mdns_rsrc*)malloc(recSize);
    if (rsrcP == NULL) {
        HOMEKIT_MDNS_PRINTF("! mDNS alloc %d\n",recSize);
    } else {
        rsrcP->rType = vType;
        rsrcP->rTTL = ttl;
        rsrcP->rKeySize = keyLen;
        rsrcP->rDataSize = vDataSize;
        memcpy(rsrcP->rData, vKey, keyLen);
        memcpy(&rsrcP->rData[keyLen], dataP, vDataSize);

        if (xSemaphoreTake(gDictMutex, portMAX_DELAY)) {
            rsrcP->rNext = gDictP;
            gDictP = rsrcP;
            xSemaphoreGive(gDictMutex);
        }

#ifdef qDebugLog
        HOMEKIT_MDNS_PRINTF("mDNS added RR '%s' %s, %d bytes\n", vKey, mdns_qrtype(vType), vDataSize);
#endif
    }
}

void mdns_add_PTR(const char* rKey, u32_t ttl, const char* nmStr)
{
    size_t nl;
    u8_t lBuff[kMaxNameSize];

    nl = mdns_str2labels(nmStr, lBuff, sizeof(lBuff));
    if (nl > 0) {
        mdns_add_response(rKey, DNS_RRTYPE_PTR, ttl, lBuff, nl);
    }
}

void mdns_add_SRV(const char* rKey, u32_t ttl, u16_t rPort, const char* targName)
{
    typedef struct SrvRec  {
        struct mdns_rr_srv srvRR;
        u8_t lBuff[kMaxNameSize];
    } __attribute__((packed)) SrvRec;

    int     nl;
    SrvRec     temp;

    temp.srvRR.prio = 0;
    temp.srvRR.weight = 0;
    temp.srvRR.port = htons(rPort);
    nl = mdns_str2labels(targName, temp.lBuff, sizeof(temp.lBuff));
    if (nl > 0)
        mdns_add_response(rKey, DNS_RRTYPE_SRV, ttl, &temp, SIZEOF_DNS_RR_SRV + nl);
}

// Single TXT str, can be concatenated
void mdns_add_TXT(const char* rKey, u32_t ttl, const char* txStr)
{
    mdns_add_response(rKey, DNS_RRTYPE_TXT, ttl, txStr, strlen(txStr));
}

void mdns_add_A(const char* rKey, u32_t ttl, const ip4_addr_t *addr)
{
    mdns_add_response(rKey, DNS_RRTYPE_A, ttl, addr, sizeof(ip4_addr_t));
}

#if LWIP_IPV6
void mdns_add_AAAA(const char* rKey, u32_t ttl, const ip6_addr_t *addr)
{
    mdns_add_response(rKey, DNS_RRTYPE_AAAA, ttl, addr, sizeof(addr->addr));
}
#endif

static void mdns_announce() {
    if (mdns_status == MDNS_STATUS_WORKING) {
        HOMEKIT_MDNS_PRINTF("mDNS 1\n");
        if (rs_esp_timer_change_period(mdns_announce_timer, MDNS_TTL_SAFE_MARGIN * MDNS_TTL_MULTIPLIER_MS) == pdPASS) {
            mdns_status = MDNS_STATUS_PROBING_1;
        }
        
    } else if (mdns_status == MDNS_STATUS_PROBING_2) {
        HOMEKIT_MDNS_PRINTF("mDNS 2\n");
        mdns_status = MDNS_STATUS_PROBE_OK;
        
    } else if (mdns_status == MDNS_STATUS_PROBE_OK) {
#ifdef ESP_PLATFORM
        HOMEKIT_MDNS_PRINTF("mDNS TTL %li/%lis\n", mdns_ttl, mdns_ttl_period);
#else
        HOMEKIT_MDNS_PRINTF("mDNS TTL %i/%is\n", mdns_ttl, mdns_ttl_period);
#endif
        
        if (rs_esp_timer_change_period(mdns_announce_timer, (mdns_ttl_period - MDNS_TTL_SAFE_MARGIN) * MDNS_TTL_MULTIPLIER_MS) == pdPASS) {
            mdns_status = MDNS_STATUS_WORKING;
        }
    }
    
#ifdef ESP_PLATFORM
    struct netif* netif = netif_default;
#else
    struct netif* netif = sdk_system_get_netif(STATION_IF);
#endif
    
#if LWIP_IPV4
    mdns_announce_netif(netif, &gMulticastV4Addr);
#endif
#if LWIP_IPV6
    mdns_announce_netif(netif, &gMulticastV6Addr);
#endif
}

void mdns_announce_stop() {
    rs_esp_timer_stop_forced(mdns_announce_timer);
}

void mdns_announce_start() {
    rs_esp_timer_change_period_forced(mdns_announce_timer, MDNS_TTL_SAFE_MARGIN * MDNS_TTL_MULTIPLIER_MS);
    mdns_status = MDNS_STATUS_WORKING;
    mdns_announce();
}

void mdns_add_facility_work(const char* instanceName,   // Friendly name, need not be unique
                            const char* serviceName,    // Must be "_name", e.g. "_hap" or "_http"
                            const char* addText,        // Must be <key>=<value>
                            mdns_flags flags,           // TCP or UDP
                            u16_t onPort,               // port number
                            u32_t ttl,                  // seconds
                            u32_t ttl_period            // seconds
                           )
{
    if (mdns_buffer_init(0) != 0) {
        return;
    }
    
    mdns_ttl = ttl;
    mdns_ttl_period = ttl_period;
    
    size_t key_len = strlen(serviceName) + 12;
    char *key = malloc(key_len + 1);
    size_t full_name_len = strlen(instanceName) + 1 + key_len;
    char *fullName = malloc(full_name_len + 1);
    size_t dev_name_len = strlen(instanceName) + 7;
    char *devName = malloc(dev_name_len + 1);

#ifdef qDebugLog
    HOMEKIT_MDNS_PRINTF("\nmDNS advertising instance %s protocol %s", instanceName, serviceName);
    if (addText) {
        HOMEKIT_MDNS_PRINTF(" text %s", addText);
    }
    HOMEKIT_MDNS_PRINTF(" on port %d %s TTL %d secs\n", onPort, (flags & mdns_UDP) ? "UDP" : "TCP", ttl);
#endif

    snprintf(key, key_len + 1, "%s.%s.local.", serviceName, (flags & mdns_UDP) ? "_udp" :"_tcp");
    snprintf(fullName, full_name_len + 1, "%s.%s", instanceName, key);
    snprintf(devName, dev_name_len + 1, "%s.local.", instanceName);

    // Order has significance for extraRR feature
    if (addText) {
        mdns_add_TXT(fullName, ttl, addText);
    }

#if LWIP_IPV6
    const ip6_addr_t addr6 = { {0ul, 0ul, 0ul, 0ul} };
    mdns_add_AAAA(devName, ttl, &addr6);
#endif

    const ip4_addr_t addr4 = { 0 };
    mdns_add_A(devName, ttl, &addr4);

    mdns_add_SRV(fullName, ttl, onPort, devName);
    mdns_add_PTR(key, ttl, fullName);

    // Optional, makes us browsable
    if (flags & mdns_Browsable) {
        mdns_add_PTR("_services._dns-sd._udp.local.", ttl, key);
    }

    free(key);
    free(fullName);
    free(devName);

    mdns_announce_timer = rs_esp_timer_create((MDNS_TTL_SAFE_MARGIN * MDNS_TTL_MULTIPLIER_MS), pdTRUE, NULL, mdns_announce);
    
    mdns_announce_start();
}

void mdns_add_facility(const char* instanceName,   // Friendly name, need not be unique
                       const char* serviceName,    // Must be "_name", e.g. "_hap" or "_http"
                       const char* addText,        // Must be <key>=<value>
                       mdns_flags flags,           // TCP or UDP
                       u16_t onPort,               // port number
                       u32_t ttl,                  // seconds
                       u32_t ttl_period            // seconds
                      )
{
    
    /*
    while (sdk_wifi_station_get_connect_status() != STATION_GOT_IP) {
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
     
    vTaskDelay(400 / portTICK_PERIOD_MS);
    */
     
    if (strstr(addText, "sf=1") != NULL) {
        //printf("mDNS TTL 4500 for pairing\n");
        ttl = 4500;
        ttl_period = 4500;
    }
    
#ifdef ESP_PLATFORM
    ttl_period -= esp_random() % (ttl_period >> 4);
#else
    ttl_period -= hwrand() % (ttl_period >> 4);
#endif
    
    mdns_add_facility_work(instanceName, serviceName, addText, flags, onPort, ttl, ttl_period);
}

static mdns_rsrc* mdns_match(const char* qstr, u16_t qType)
{
    mdns_rsrc* rp = gDictP;
    while (rp != NULL) {
       if (rp->rType == qType || qType == DNS_RRTYPE_ANY) {
            if (strcasecmp(rp->rData, qstr) == 0) {
#ifdef qDebugLog
                HOMEKIT_MDNS_PRINTF(" - matched '%s' %s\n", qstr, mdns_qrtype(rp->rType));
#endif
                break;
            }
        }
        rp = rp->rNext;
    }
    return rp;
}

// Create answer RR and append to resp[respLen], return new length
static int mdns_add_to_answer(mdns_rsrc* rsrcP, u8_t* resp, int respLen)
{
    // Key is stored as C str, convert to labels
    size_t rem = mdns_responder_reply_size - respLen;
    size_t len = mdns_str2labels(rsrcP->rData, &resp[respLen], rem);
    if (len == 0) {
        // Overflow, skip this answer.
        return respLen;
    }
    if ((len + SIZEOF_DNS_ANSWER + rsrcP->rDataSize) > rem) {
        // Overflow, skip this answer.
        HOMEKIT_MDNS_PRINTF("! mDNS size %d\n", len + SIZEOF_DNS_ANSWER + rsrcP->rDataSize);
        return respLen;
    }
    respLen += len;

    // Answer fields: may be misaligned, so build and memcpy
    struct mdns_answer ans;
    ans.type  = htons(rsrcP->rType);
    ans.class = htons(DNS_RRCLASS_IN);
    ans.ttl   = htonl(rsrcP->rTTL);
    ans.len   = htons(rsrcP->rDataSize);
    memcpy(&resp[respLen], &ans, SIZEOF_DNS_ANSWER);
    respLen += SIZEOF_DNS_ANSWER;

    // Data for this key
    memcpy(&resp[respLen], &rsrcP->rData[rsrcP->rKeySize], rsrcP->rDataSize);
    respLen += rsrcP->rDataSize;

    return respLen;
}

//---------------------------------------------------------------------------

// Send UDP to multicast or unicast address
static void mdns_send_mcast(struct netif* netif, const ip_addr_t *addr, u8_t* msgP, int nBytes, const u8_t unicast)
{
    struct pbuf* p;
    err_t err;

#ifdef qLogAllTraffic
    mdns_print_msg(msgP, nBytes);
#endif

    p = pbuf_alloc(PBUF_TRANSPORT, nBytes, PBUF_RAM);
    if (p) {
        memcpy(p->payload, msgP, nBytes);
        const ip_addr_t *dest_addr;
        if (unicast) {
            dest_addr = addr;
        } else if (IP_IS_V6_VAL(*addr)) {
#if LWIP_IPV6
            dest_addr = &gMulticastV6Addr;
#endif
        } else {
            dest_addr = &gMulticastV4Addr;
        }
        
        //const uint32_t free_heap = xPortGetFreeHeapSize();
        
        LOCK_TCPIP_CORE();
        err = udp_sendto_if(gMDNS_pcb, p, dest_addr, LWIP_IANA_PORT_MDNS, netif);
        UNLOCK_TCPIP_CORE();
        
        pbuf_free(p);
        
        if (err == ERR_OK) {
#ifdef qDebugLog
            HOMEKIT_MDNS_PRINTF(" - responded to " IPSTR " with %d bytes err %d\n", IP2STR(dest_addr), nBytes, err);
#endif
            /*
            if (free_heap < 10240) {
                uint8_t count = 0;
                while (free_heap > xPortGetFreeHeapSize() && count < 2) {
                    count++;
                    vTaskDelay(1);
                }
            }
             */
        } else {
            mdns_status = MDNS_STATUS_WORKING;
            HOMEKIT_MDNS_PRINTF("! mDNS send (%d)\n", err);
        }
    } else {
        HOMEKIT_MDNS_PRINTF("! mDNS alloc %d\n", nBytes);
    }
}
    
// Message has passed tests, may want to send an answer
static void mdns_reply(const ip_addr_t *addr, struct mdns_hdr* hdrP)
{
    unsigned int i, nquestions, respLen;
    struct mdns_hdr* rHdr;
    mdns_rsrc* extra;
    u8_t* qBase = (u8_t*)hdrP;
    u8_t* qp;

    memset(mdns_response, 0, mdns_responder_reply_size);

#ifdef qDebugLog
    HOMEKIT_MDNS_PRINTF("mDNS_reply\n");
#endif
    
    // Build response header
    rHdr = (struct mdns_hdr*) mdns_response;
    rHdr->id = hdrP->id;
    rHdr->flags1 = DNS_FLAG1_RESP + DNS_FLAG1_AUTH;
    rHdr->flags2 = 0;
    rHdr->numquestions = 0;
    rHdr->numanswers = 0;
    rHdr->numauthrr = 0;
    rHdr->numextrarr = 0;
    respLen = SIZEOF_DNS_HDR;

    extra = NULL;
    qp = qBase + SIZEOF_DNS_HDR;
    nquestions = htons(hdrP->numquestions);
    u8_t unicast = 1;

    if (xSemaphoreTake(gDictMutex, portMAX_DELAY)) {

        for (i = 0; i < nquestions; i++) {
            char  qStr[kMaxQStr];
            u16_t qClass, qType;
            u8_t  qUnicast;
            mdns_rsrc* rsrcP;

            qp = mdns_get_question(qBase, qp, qStr, &qClass, &qType, &qUnicast);
            if (qClass == DNS_RRCLASS_IN || qClass == DNS_RRCLASS_ANY) {
                rsrcP = mdns_match(qStr, qType);
                if (rsrcP) {
                    if (mdns_status == MDNS_STATUS_PROBING_1) {
                        mdns_status = MDNS_STATUS_PROBING_2;
                    }
#if LWIP_IPV6
                    if (rsrcP->rType == DNS_RRTYPE_AAAA) {
                        // Emit an answer for each ipv6 address.
                        struct netif *netif = ip_current_input_netif();
                        for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                            if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i))) {
                                const ip6_addr_t *addr6 = netif_ip6_addr(netif, i);
#ifdef qDebugLog
                                char addr6_str[IP6ADDR_STRLEN_MAX];
                                ip6addr_ntoa_r(addr6, addr6_str, IP6ADDR_STRLEN_MAX);
                                HOMEKIT_MDNS_PRINTF("Updating AAAA record for '%s' to %s\n", rsrcP->rData, addr6_str);
#endif
                                memcpy(&rsrcP->rData[rsrcP->rKeySize], addr6, sizeof(addr6->addr));
                                size_t new_len = mdns_add_to_answer(rsrcP, mdns_response, respLen);
                                if (new_len > respLen) {
                                    rHdr->numanswers = htons(htons(rHdr->numanswers) + 1);
                                    respLen = new_len;
                                }
                            }
                        }
                        continue;
                    }
#endif

                    if (rsrcP->rType == DNS_RRTYPE_A) {
                        struct netif *netif = ip_current_input_netif();
#ifdef qDebugLog
                        char addr4_str[IP4ADDR_STRLEN_MAX];
                        ip4addr_ntoa_r(netif_ip4_addr(netif), addr4_str, IP4ADDR_STRLEN_MAX);
                        HOMEKIT_MDNS_PRINTF("Updating A record for '%s' to %s\n", rsrcP->rData, addr4_str);
#endif
                        memcpy(&rsrcP->rData[rsrcP->rKeySize], netif_ip4_addr(netif), sizeof(ip4_addr_t));
                    }

                    size_t new_len = mdns_add_to_answer(rsrcP, mdns_response, respLen);
                    if (new_len > respLen) {
                        rHdr->numanswers = htons(htons(rHdr->numanswers) + 1);
                        respLen = new_len;
                    }

                    // Extra RR logic: if SRV follows PTR, or A follows SRV, volunteer it in extraRR
                    // Not required, but could do more here, see RFC6763 s12
                    if (qType == DNS_RRTYPE_PTR) {
                        if (rsrcP->rNext && rsrcP->rNext->rType == DNS_RRTYPE_SRV)
                            extra = rsrcP->rNext;
                    } else if (qType == DNS_RRTYPE_SRV) {
                        if (rsrcP->rNext && rsrcP->rNext->rType == DNS_RRTYPE_A)
                            extra = rsrcP->rNext;
                    }
#ifdef qDebugLog
                    HOMEKIT_MDNS_PRINTF("qUnicast: %i\n", qUnicast);
#endif
                    if (!qUnicast) {
                        unicast = 0;
                    }
                }
            }
        } // for nQuestions

        xSemaphoreGive(gDictMutex);
    }

    if (respLen > SIZEOF_DNS_HDR) {
        if (extra) {
            if (extra->rType == DNS_RRTYPE_A) {
                struct netif *netif = ip_current_input_netif();
#ifdef qDebugLog
                char addr4_str[IP4ADDR_STRLEN_MAX];
                ip4addr_ntoa_r(netif_ip4_addr(netif), addr4_str, IP4ADDR_STRLEN_MAX);
                HOMEKIT_MDNS_PRINTF("Updating A record for '%s' to %s\n", extra->rData, addr4_str);
#endif
                memcpy(&extra->rData[extra->rKeySize], netif_ip4_addr(netif), sizeof(ip4_addr_t));
            }
            size_t new_len = mdns_add_to_answer(extra, mdns_response, respLen);
            if (new_len > respLen) {
                rHdr->numextrarr = htons(htons(rHdr->numextrarr) + 1);
                respLen = new_len;
            }
        }
#ifdef qDebugLog
        HOMEKIT_MDNS_PRINTF("*** Sending response (unicast: %i)...\n", unicast);
#endif
        
#ifdef ESP_PLATFORM
        struct netif* netif = netif_default;
#else
        struct netif* netif = sdk_system_get_netif(STATION_IF);
#endif
        
        mdns_send_mcast(netif, addr, mdns_response, respLen, unicast);
    }
}

// Announce all configured services
static void mdns_announce_netif(struct netif *netif, const ip_addr_t *addr)
{
    if (mdns_response == NULL) {
        return;
    }
    memset(mdns_response, 0, mdns_responder_reply_size);

    // Build response header
    struct mdns_hdr *rHdr = (struct mdns_hdr*) mdns_response;
    memset(rHdr, 0, sizeof(*rHdr));
    rHdr->flags1 = DNS_FLAG1_RESP + DNS_FLAG1_AUTH;

    size_t respLen = SIZEOF_DNS_HDR;

    if (xSemaphoreTake(gDictMutex, portMAX_DELAY)) {
        mdns_rsrc *rsrcP = gDictP;
        while (rsrcP) {
#if LWIP_IPV6
            if (rsrcP->rType == DNS_RRTYPE_AAAA) {
                // Emit an answer for each ipv6 address.
                for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i))) {
                        const ip6_addr_t *addr6 = netif_ip6_addr(netif, i);
#ifdef qDebugLog
                        char addr6_str[IP6ADDR_STRLEN_MAX];
                        ip6addr_ntoa_r(addr6, addr6_str, IP6ADDR_STRLEN_MAX);
                        HOMEKIT_MDNS_PRINTF("Updating AAAA record for '%s' to %s\n", rsrcP->rData, addr6_str);
#endif
                        memcpy(&rsrcP->rData[rsrcP->rKeySize], addr6, sizeof(addr6->addr));
                        size_t new_len = mdns_add_to_answer(rsrcP, mdns_response, respLen);
                        if (new_len > respLen) {
                            rHdr->numanswers = htons(htons(rHdr->numanswers) + 1);
                            respLen = new_len;
                        }
                    }
                }
                continue;
            }
#endif

            if (rsrcP->rType == DNS_RRTYPE_A) {
#ifdef qDebugLog
                char addr4_str[IP4ADDR_STRLEN_MAX];
                ip4addr_ntoa_r(netif_ip4_addr(netif), addr4_str, IP4ADDR_STRLEN_MAX);
                HOMEKIT_MDNS_PRINTF("Updating A record for '%s' to %s\n", rsrcP->rData, addr4_str);
#endif
                memcpy(&rsrcP->rData[rsrcP->rKeySize], netif_ip4_addr(netif), sizeof(ip4_addr_t));
            }

            size_t new_len = mdns_add_to_answer(rsrcP, mdns_response, respLen);
            if (new_len > respLen) {
                rHdr->numanswers = htons(htons(rHdr->numanswers) + 1);
                respLen = new_len;
            }

            rsrcP = rsrcP->rNext;
        }

        xSemaphoreGive(gDictMutex);
    }

    if (respLen > SIZEOF_DNS_HDR) {
        mdns_send_mcast(netif, addr, mdns_response, respLen, 0);
    }
}

// Callback from udp_recv
static void mdns_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
#ifdef qLogIncoming
    char addr_str[IPADDR_STRLEN_MAX];
    ipaddr_ntoa_r(addr, addr_str, IPADDR_STRLEN_MAX);
    HOMEKIT_MDNS_PRINTF("\n\nmDNS IPv%d got %db from %s\n", IP_IS_V6(addr) ? 6 : 4, p->tot_len, addr_str);
#endif

    // Sanity checks on size
    if (mdns_response) {
        if (p->tot_len > (mdns_responder_reply_size) ||
            p->tot_len < (SIZEOF_DNS_HDR + SIZEOF_DNS_QUERY + 1)) {
            char addr_str[IPADDR_STRLEN_MAX];
            ipaddr_ntoa_r(addr, addr_str, IPADDR_STRLEN_MAX);
            HOMEKIT_MDNS_PRINTF("! mDNS %ib from %s\n", p->tot_len, addr_str);
        } else {
            struct mdns_hdr* hdrP = (struct mdns_hdr*) p->payload;
    #ifdef qLogAllTraffic
            HOMEKIT_MDNS_PRINTF("mDNS_recv: payload size %i\n", p->tot_len);
            mdns_print_msg(p->payload, p->tot_len);
    #endif
            if ((hdrP->flags1 & (DNS_FLAG1_RESP + DNS_FLAG1_OPMASK + DNS_FLAG1_TRUNC)) == 0 &&
                hdrP->numquestions > 0) {
                mdns_reply(addr, hdrP);
            }
        }
    }
    
    pbuf_free(p);
}

// If we are in station mode and have an IP address, start a multicast UDP receive
void mdns_init()
{
#ifdef ESP_PLATFORM
    struct netif* netif = netif_default;
#else
    struct netif* netif = sdk_system_get_netif(STATION_IF);
#endif
    
    LOCK_TCPIP_CORE();
    
    // Start IGMP on the netif for our interface: this isn't done for us
    if (!(netif->flags & NETIF_FLAG_IGMP)) {
        netif->flags |= NETIF_FLAG_IGMP;
        igmp_start(netif);
    }
    
    gDictMutex = xSemaphoreCreateBinary();
    xSemaphoreGive(gDictMutex);
    
    gMDNS_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
    
    igmp_joingroup_netif(netif, ip_2_ip4(&gMulticastV4Addr));
    
#if LWIP_IPV6
    mld6_joingroup_netif(netif, ip_2_ip6(&gMulticastV6Addr));
#endif
    
    udp_bind(gMDNS_pcb, IP_ANY_TYPE, LWIP_IANA_PORT_MDNS);
    
    udp_bind_netif(gMDNS_pcb, netif);
    udp_recv(gMDNS_pcb, mdns_recv, NULL);
    
    UNLOCK_TCPIP_CORE();
}

/*
// If we are in station mode and have an IP address, start a multicast UDP receive
void mdns_init()
{
    err_t err;

#ifdef ESP_PLATFORM
    struct netif* netif = netif_default;
#else
    struct netif* netif = sdk_system_get_netif(STATION_IF);
#endif
    
    if (netif == NULL) {
        HOMEKIT_MDNS_PRINTF("! mDNS wifi opmode\n");
        return;
    }

    LOCK_TCPIP_CORE();
    
    // Start IGMP on the netif for our interface: this isn't done for us
    if (!(netif->flags & NETIF_FLAG_IGMP)) {
        netif->flags |= NETIF_FLAG_IGMP;
        err = igmp_start(netif);
        if (err != ERR_OK) {
            HOMEKIT_MDNS_PRINTF("! mDNS igmp_start on %c%c %d\n", netif->name[0], netif->name[1], err);
            UNLOCK_TCPIP_CORE();
            return;
        }
    }

    gDictMutex = xSemaphoreCreateBinary();
    if (!gDictMutex) {
        HOMEKIT_MDNS_PRINTF("! mDNS mutex\n");
        UNLOCK_TCPIP_CORE();
        return;
    }
    xSemaphoreGive(gDictMutex);
    
    gMDNS_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
    if (!gMDNS_pcb) {
        HOMEKIT_MDNS_PRINTF("! mDNS udp_new\n");
        UNLOCK_TCPIP_CORE();
        return;
    }

    if ((err = igmp_joingroup_netif(netif, ip_2_ip4(&gMulticastV4Addr))) != ERR_OK) {
        HOMEKIT_MDNS_PRINTF("! mDNS_init igmp_join %d\n", err);
        UNLOCK_TCPIP_CORE();
        return;
    }

#if LWIP_IPV6
    if ((err = mld6_joingroup_netif(netif, ip_2_ip6(&gMulticastV6Addr))) != ERR_OK) {
        HOMEKIT_MDNS_PRINTF("! mDNS_init igmp_join %d\n", err);
        UNLOCK_TCPIP_CORE();
        return;
    }
#endif

    if ((err = udp_bind(gMDNS_pcb, IP_ANY_TYPE, LWIP_IANA_PORT_MDNS)) != ERR_OK) {
        HOMEKIT_MDNS_PRINTF("! mDNS udp_bind %d\n", err);
        UNLOCK_TCPIP_CORE();
        return;
    }
    
    udp_bind_netif(gMDNS_pcb, netif);
    udp_recv(gMDNS_pcb, mdns_recv, NULL);
    
    UNLOCK_TCPIP_CORE();
}
*/
