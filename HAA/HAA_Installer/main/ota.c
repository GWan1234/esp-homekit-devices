/*
* Home Accessory Architect OTA Installer
*
* Copyright 2020-2024 José Antonio Jiménez Campos (@RavenSystem)
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef ESP_PLATFORM

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "errno.h"
#include "esp32_port.h"

#else   // ESP_OPEN_RTOS

#include <esp8266.h>
#include <sysparam.h>
#include <spiflash.h>
#include <rboot-api.h>

#endif


#include <lwip/api.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/etharp.h"

#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/sha512.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#include <ota.h>

#include "header.h"

#ifdef ESP_PLATFORM

#ifdef HAABOOT
    #define MAXFILESIZE         (0xc0000 - 16)      // From partitions.csv
#else
    #define MAXFILESIZE         (0x110000 - 16)     // From partitions.csv
#endif  //HAABOOT

#else   // ESP_OPEN_RTOS

#ifdef HAABOOT
    #define MAXFILESIZE         ((SPIFLASH_HOMEKIT_BASE_ADDR - BOOT1SECTOR) - 16)
#else
    #define MAXFILESIZE         ((BOOT1SECTOR - BOOT0SECTOR) - 16)
#endif  //HAABOOT

#endif


static ecc_key public_key;
static uint8_t file_first_byte[] = { 0xff };
static WOLFSSL_CTX* ctx = NULL;
static char last_host[HOST_LEN];
static char last_location[LOCATION_LEN];

#ifdef DEBUG_WOLFSSL
void MyLoggingCallback(const int logLevel, const char* const logMessage) {
    /*custom logging function*/
    INFO("loglevel: %i - %s", logLevel, logMessage);
}
#endif

#ifdef ESP_PLATFORM
esp_partition_t* partition_boot0 = NULL;
esp_partition_t* partition_boot1 = NULL;

void set_partitions() {
    if (!partition_boot0) {
        partition_boot0 = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_0, NULL);
    }
    
    if (!partition_boot1) {
        partition_boot1 = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, NULL);
        if (!partition_boot1) {
            partition_boot1 = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
        }
    }
}

esp_partition_t* get_partition(const unsigned int sector) {
    if (sector == BOOT0SECTOR) {
        return partition_boot0;
    }
    
    return partition_boot1;
}
#else

#define set_partitions()

#endif

static char *strstr_lc(char *full_string, const char *search) {
    const unsigned int search_len = strlen(search);
    for (unsigned int i = 0; i <= strlen(full_string) - search_len; i++) {
        if (tolower((unsigned char) full_string[i]) == tolower((unsigned char) search[0])) {
            for (unsigned int j = 0; j < search_len; j++) {
                if (tolower((unsigned char) full_string[i + j]) != tolower((unsigned char) search[j])) {
                    break;
                }
                
                if (j == search_len - 1) {
                    return full_string + i;
                }
            }
        }
    }
    
    return NULL;
}

static void ota_get_host(const char* repo) {
    memset(last_host, 0, HOST_LEN);
    const char *found = strchr(repo, '/');

    if (found) {
        memcpy(last_host, repo, found - repo);
    } else {
        memcpy(last_host, repo, strlen(repo));
    }
}

static void ota_get_location(const char* repo) {
    memset(last_location, 0, LOCATION_LEN);
    const char *found = strchr(repo, '/');

    if (found) {
        memcpy(last_location, found + 1, strlen(found) - 1);
    } else {
        last_location[0] = 0;
    }
}

void ota_init(char* repo, const bool is_ssl) {
    INFO("INIT");

    // ip_addr_t target_ip;
    
    set_partitions();
    
#ifndef ESP_PLATFORM
    rboot_config conf;
    conf = rboot_get_config();
    if (conf.count != 2 || conf.roms[0] != BOOT0SECTOR || conf.roms[1] != BOOT1SECTOR || conf.current_rom != 0) {
        conf.count = 2;
        conf.roms[0] = BOOT0SECTOR;
        conf.roms[1] = BOOT1SECTOR;
        conf.current_rom = 0;
        rboot_set_config(&conf);
    }
# endif
    
    if (is_ssl) {
#ifdef DEBUG_WOLFSSL
        if (wolfSSL_SetLoggingCb(MyLoggingCallback)) {
            ERROR("Setting debug callback");
        }
#endif
        wolfSSL_Init();

        ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
        
        //ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
        //wolfSSL_CTX_SetMinVersion(ctx, WOLFSSL_TLSV1_2);
        
        wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    }
    
    word32 idx = 0;
    wc_ecc_init(&public_key);
    
    // Public key to verify signatures and avoid malware
    const uint8_t raw_public_key[] = {
        0x30, 0x76, 0x30, 0x10, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce,
        0x3d, 0x02, 0x01, 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22,
        0x03, 0x62, 0x00, 0x04, 0x98, 0xe0, 0x54, 0xc4, 0x9b, 0x8a,
        0x41, 0x94, 0x76, 0xd1, 0x7f, 0xfd, 0xdf, 0x7b, 0xc4, 0xcc,
        0x8f, 0x97, 0x37, 0x89, 0x31, 0xd5, 0x17, 0x99, 0xe8, 0x0f,
        0x94, 0x3a, 0x85, 0x21, 0x09, 0xc6, 0xaa, 0xee, 0xb0, 0xee,
        0x58, 0x29, 0xb1, 0x14, 0x6d, 0x8f, 0x37, 0xcd, 0x01, 0x23,
        0x2f, 0xbf, 0x75, 0x3b, 0x70, 0xc2, 0xb9, 0x3f, 0x30, 0x4d,
        0x88, 0xf4, 0xc6, 0x5f, 0x8c, 0x24, 0x8a, 0x02, 0xd4, 0xce,
        0x65, 0x64, 0x24, 0xc2, 0x6d, 0xd2, 0x2c, 0x11, 0x95, 0x08,
        0x00, 0x5d, 0x4d, 0x9a, 0x9f, 0x1d, 0xab, 0x91, 0xf4, 0x04,
        0x66, 0x30, 0x94, 0x56, 0x3b, 0x4c, 0xb7, 0xba, 0xdb, 0x22
    };
    
    wc_EccPublicKeyDecode(raw_public_key, &idx, &public_key, sizeof(raw_public_key));
}

static int ota_connect(char* host, uint16_t port, int *socket, WOLFSSL** ssl, const bool is_ssl) {
    INFO_NNL("*** CONNECT\nDNS..");
    int ret;
    const struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
    };
    
    struct addrinfo* res = NULL;
    
    void print_error() {
        if (res) {
            freeaddrinfo(res);
        }
        INFO("!");
    }
    
    char port_s[6];
    itoa(port, port_s, 10);
    ret = getaddrinfo(host, port_s, &hints, &res);
    if (ret) {
        print_error();
        return -2;
    }
    
    INFO_NNL("OK\nSocket..");
    *socket = lwip_socket(res->ai_family, res->ai_socktype, 0);
    if (*socket < 0) {
        print_error();
        return -3;
    }

    INFO_NNL("OK\nConnect..");
    ret = connect(*socket, res->ai_addr, res->ai_addrlen);
    if (ret) {
        print_error();
        return -2;
    }
    INFO_NNL("OK\n");

    // SSL mode
    if (is_ssl) {
        INFO_NNL("SSL..");
        *ssl = wolfSSL_new(ctx);
        
        if (!*ssl) {
            print_error();
            return -2;
        }

        INFO_NNL("OK\nFD..");
        
        //wolfSSL_Debugging_ON();
        
        ret = wolfSSL_set_fd(*ssl, *socket);
        if (ret != SSL_SUCCESS) {
            freeaddrinfo(res);
            ERROR("%i", ret);
            ret = wolfSSL_get_error(*ssl, ret);
            ERROR("SSL %d", ret);
            return -1;
        }
        
        INFO_NNL("OK\nSNI..");
        ret = wolfSSL_UseSNI(*ssl, WOLFSSL_SNI_HOST_NAME, host, strlen(host));
        
        if (ret != SSL_SUCCESS) {
            freeaddrinfo(res);
            ERROR("%i", ret);
            ret = wolfSSL_get_error(*ssl, ret);
            ERROR("SSL %d", ret);
            return -1;
        }
        
        //wolfSSL_Debugging_OFF();
        
        INFO_NNL("OK\nSSLConnect..");
        
        vTaskDelay(50 / portTICK_PERIOD_MS);
        ret = wolfSSL_connect(*ssl);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        
        if (ret != SSL_SUCCESS) {
            freeaddrinfo(res);
            ERROR("%i", ret);
            ret = wolfSSL_get_error(*ssl, ret);
            ERROR("SSL %d", ret);
            return -1;
        }
        
        INFO("OK");
    }
    
    freeaddrinfo(res);
    
    return 0;
}

static int ota_get_final_location(char* repo, char* file, uint16_t port, const bool is_ssl) {
    int ota_conn_result = -1;
    int ret = 0;
    int error = 0;
    int slash;
    WOLFSSL* ssl;
    int socket;
    
    uint32_t buffer_len = 0;
    char* buffer = NULL;
    char* recv_buf = NULL;
    char* location = NULL;

    ota_get_host(repo);
    ota_get_location(repo);
    
    if (strlen(last_location) > 0) {
        strcat(strcat(last_location, "/"), file);
    } else {
        strcat(last_location, file);
    }
    
    int i = 0;
    while (i < MAX_302_JUMPS) {
        i++;
        INFO("*** FORWARDING %s:%i/%s", last_host, port, last_location);
        
        ota_conn_result = ota_connect(last_host, port, &socket, &ssl, is_ssl);  //release socket and ssl when ready
        
        const struct timeval rcvtimeout = { 2, 0 };
        setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &rcvtimeout, sizeof(rcvtimeout));
        
        recv_buf = malloc(RECV_BUF_LEN);
        memset(recv_buf, 0, RECV_BUF_LEN);
        
        strcat(strcat(strcat(strcat(strcat(strcat(strcpy(recv_buf,
                                            REQUESTHEAD),
                                            last_location),
                                            REQUESTTAIL),
                                            last_host),
                                            RANGE),
                                            "0-1"),
                                            CRLFCRLF);
        
        if (ota_conn_result == 0) {
            const unsigned int send_bytes = strlen(recv_buf);
            
            if (is_ssl) {
                ret = wolfSSL_write(ssl, recv_buf, send_bytes);
            } else {
                ret = lwip_write(socket, recv_buf, send_bytes);
            }
            
            INFO_NNL("Write..");
            if (ret > 0) {
                INFO("OK");
                
                //wolfSSL_shutdown(ssl); //by shutting down the connection before even reading, we reduce the payload to the minimum
                unsigned int all_ok = false;
                
                do {
                    memset(recv_buf, 0, RECV_BUF_LEN);
                    
                    if (is_ssl) {
                        ret = wolfSSL_read(ssl, recv_buf, RECV_BUF_LEN - 1);
                    } else {
                        ret = lwip_read(socket, recv_buf, RECV_BUF_LEN - 1);
                    }
                    
                    if (ret > 0) {
                        all_ok = true;
                        buffer_len += ret;
                        char* new_buffer = realloc(buffer, buffer_len + 1);
                        if (!new_buffer) {
                            ERROR("Buffer");
                            break;
                        }
                        buffer = new_buffer;
                        memcpy(buffer + buffer_len - ret, recv_buf, ret);
                    }
                } while (ret > 0 && buffer_len < HEADER_BUFFER_LEN);
                
                if (all_ok) {
                    ret = buffer_len;
                    buffer[ret] = 0;
                    
                    INFO("GOT %i Bytes\n%s\n", ret, buffer);
                    
                    location = strstr_lc(buffer, "http/1.1 ");
                    if (location) {
                        location += 9; // Flush "HTTP/1.1 "
                        slash = atoi(location);
                        INFO("HTTP returns %d\n", slash);
                        if (slash == 200 || slash == 206) {
                            i = MAX_302_JUMPS;
                            
                        } else if (slash == 302) {
                            location = strstr_lc(buffer, "\nlocation:");
                            if (location) {
                                strchr(location, '\r')[0] = 0;
                                if (location[10] == ' ') {
                                    location++;
                                }
                                
                                location = strstr_lc(location , "//");
                                location += 2; // Flush "//"

                                ota_get_host(location);
                                ota_get_location(location);
                                
                            } else {
                                i = MAX_302_JUMPS;
                                error = -1;
                            }
                            
                        } else {
                            i = MAX_302_JUMPS;
                            error = -2;
                        }

                    } else {
                        i = MAX_302_JUMPS;
                        error = -3;
                    }

                } else {
                    ERROR("Read %i", ret);
                    if (is_ssl) {
                        ret = wolfSSL_get_error(ssl, ret);
                        ERROR("SSL %d", ret);
                    }
                }
            } else {
                ERROR("%i", ret);
                if (is_ssl) {
                    ret = wolfSSL_get_error(ssl, ret);
                    ERROR("SSL %d", ret);
                }
            }
        }
        
        free(recv_buf);
        
        buffer_len = 0;
        if (buffer) {
            free(buffer);
            buffer = NULL;
        }

        switch (ota_conn_result) {
            case  0:
            case -1:
                if (is_ssl) {
                    wolfSSL_free(ssl);
                }
            case -2:
                lwip_close(socket);
            //case -3:
            default:
            ;
        }
        
        if (ota_conn_result < 0) {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }
    
    if (error < 0) {
        return error;
    }
    
    if (ota_conn_result < 0) {
        return ota_conn_result;
    }
    
    return ret;
}

static int ota_get_file_ex(char* repo, char* file, int sector, uint8_t* buffer, int bufsz, uint16_t port, const bool is_ssl, int* resume) {
    INFO("*** DOWNLOADING");
    
    int ota_conn_result, ret = 0;
    WOLFSSL* ssl;
    int socket;
    
    int recv_bytes = 0;
    int collected = 0;
    
    if (resume) {
        collected = *resume;
        INFO("Resuming %i", collected);
    }
    
    int length = collected + 1;
    int clength = 0;
    int last_collected;
    int writespace = 0;
    int left, header;

    if (sector == 0 && buffer == NULL) {
        return -5;      // Needs to be either a sector or a signature/version file
    }
    
    unsigned int connection_tries = 0;
    while ((ota_conn_result = ota_get_final_location(repo, file, port, is_ssl)) <= 0 && connection_tries < 3) {
        connection_tries++;
        ERROR("Tries %i", connection_tries);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    if (ota_conn_result <= 0) {
        ERROR("FINAL LOC");
        return -1;
    }
    
    INFO("*** FINAL %s:%i/%s\n", last_host, port, last_location);
    
    int new_connection() {
        unsigned int tries = 0;
        int result;
        while ((result = ota_connect(last_host, port, &socket, &ssl, is_ssl)) < 0 && tries < 3) {
            tries++;
            ERROR("Tries %i", tries);
            
            switch (result) {
                case 0:
                case -1:
                    if (is_ssl) {
                        wolfSSL_free(ssl);
                    }
                case -2:
                    lwip_close(socket);
                //case -3:
                default:
                ;
            }
            
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
        
        if (result >= 0) {
            const struct timeval rcvtimeout = { 60, 0 };
            setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &rcvtimeout, sizeof(rcvtimeout));
        }
        
        return result;
    }
    
    ota_conn_result = new_connection();
    
    if (ota_conn_result == 0) {
        char* location;
        char* recv_buf = malloc(RECV_BUF_LEN);
        
        connection_tries = 0;
        while (collected < length && connection_tries < MAX_DOWNLOAD_FILE_TRIES) {
            last_collected = collected;
            
            snprintf(recv_buf, RECV_BUF_LEN - 1, REQUESTHEAD"%s"REQUESTTAIL"%s"RANGE"%d-%d%s", last_location, last_host, collected, collected + 4095, CRLFCRLF);
            
            const unsigned int send_bytes = strlen(recv_buf);
            
            if (is_ssl) {
                ret = wolfSSL_write(ssl, recv_buf, send_bytes);
            } else {
                ret = lwip_write(socket, recv_buf, send_bytes);
            }
            
            recv_bytes = 0;
            
            if (ret > 0) {
                header = 1;
                memset(recv_buf, 0, RECV_BUF_LEN);
                //wolfSSL_Debugging_ON();
                do {
                    if (is_ssl) {
                        vTaskDelay(50 / portTICK_PERIOD_MS);
                        ret = wolfSSL_read(ssl, recv_buf, RECV_BUF_LEN - 1);
                        vTaskDelay(50 / portTICK_PERIOD_MS);
                    } else {
                        ret = lwip_read(socket, recv_buf, RECV_BUF_LEN - 1);
                    }

                    if (ret > 0) {
                        if (header) {
                            //INFO("--------\n%s\n-------- %d", recv_buf, ret);
                            // Parse Content-Length: xxxx
                            
                            void try_new_conn() {
                                INFO("--\n%s\n-- %d", recv_buf, ret);
                                free(recv_buf);
                                collected = last_collected;
                                connection_tries++;
                                if (is_ssl) {
                                    wolfSSL_free(ssl);
                                }
                                lwip_close(socket);
                                vTaskDelay(1000 / portTICK_PERIOD_MS);
                                ota_conn_result = new_connection();
                                recv_buf = malloc(RECV_BUF_LEN);
                            }
                            
                            location = strstr_lc(recv_buf, "\ncontent-length:");
                            if (!location) {
                                ERROR("No CL");
                                try_new_conn();
                                break;
                            }
                            strchr(location, '\r')[0] = 0;
                            location += 16; // Flush "Content-Length:"
                            clength = atoi(location);
                            location[strlen(location)] = '\r'; // In case the order changes
                            // Parse Content-Range: bytes xxxx-yyyy/zzzz
                            location = strstr_lc(recv_buf, "\ncontent-range:");
                            if (location) {
                                strchr(location,'\r')[0] = 0;
                                location += 15; // Flush "Content-Range: "
                                location = strstr_lc(recv_buf, "bytes ");
                                location += 6; // Flush "bytes "
                                
                                location = strstr_lc(location, "/");
                                location++; // Flush "/"
                                
                                length = atoi(location);
                                
                                location[strlen(location)] = '\r'; // Search the entire buffer again
                            } else if (buffer) {
                                length = clength;
                            } else {
                                ERROR("No CR");
                                try_new_conn();
                                break;
                            }
                            
                            location = strstr_lc(recv_buf, CRLFCRLF) + 4; // Go to end of header
                            if ((left = ret - (location - recv_buf))) {
                                header = 0; // We have body in the same IP packet as the header so we need to process it already
                                ret = left;
                                memmove(recv_buf, location, left); // Move this payload to the head of the recv_buf
                            }
                        }
                        
                        if (length > MAXFILESIZE) {
                            ERROR("TOO BIG %i/%i", length, MAXFILESIZE);
                            free(recv_buf);
                            return -10;
                        }
                        
                        if (!header) {
                            recv_bytes += ret;
                            if (sector) { // Write to flash
                                connection_tries = 0;
                                if (writespace < ret) {
#ifdef ESP_PLATFORM
                                    if (esp_partition_erase_range(get_partition(sector), collected, SPI_FLASH_SECTOR_SIZE) != ESP_OK) {
#else
                                    INFO_NNL("0x%02X ", (sector + collected) / SPI_FLASH_SECTOR_SIZE);
                                    if (!spiflash_erase_sector(sector + collected)) {
#endif
                                        free(recv_buf);
                                        return -6; // Erase error
                                    }
                                    writespace += SPI_FLASH_SECTOR_SIZE;
                                }
                                if (collected) {
#ifdef ESP_PLATFORM
                                    if (esp_partition_write(get_partition(sector), collected, (uint8_t*) recv_buf, ret) != ESP_OK) {
#else
                                    if (!spiflash_write(sector + collected, (uint8_t*) recv_buf, ret)) {
#endif
                                        free(recv_buf);
                                        return -7; // Write error
                                    }
                                } else { // At the very beginning, do not write the first byte yet but store it for later
                                    file_first_byte[0] = (uint8_t) recv_buf[0];
#ifdef ESP_PLATFORM
                                    if (esp_partition_write(get_partition(sector), 1, (uint8_t*) recv_buf + 1, ret - 1) != ESP_OK) {
#else
                                    if (!spiflash_write(sector + 1, (uint8_t*) recv_buf + 1, ret - 1)) {
#endif
                                        free(recv_buf);
                                        return -8; // Write error
                                    }
                                }
                                writespace -= ret;
                            } else { // Buffer
                                if (ret > bufsz) {
                                    free(recv_buf);
                                    return -9; // Too big
                                }
                                memcpy(buffer, recv_buf, ret);
                            }
                            collected += ret;
                        }
                        
                    } else {
                        ERROR("Read %i", ret);
                        if (is_ssl) {
                            ret = wolfSSL_get_error(ssl, ret);
                            ERROR("SSL %d", ret);
                        }
                        
                        if (!ret && collected < length) {
                            free(recv_buf);
                            if (is_ssl) {
                                wolfSSL_free(ssl);
                            }
                            lwip_close(socket);
                            vTaskDelay(1000 / portTICK_PERIOD_MS);
                            ota_conn_result = new_connection();
                            recv_buf = malloc(RECV_BUF_LEN);
                        }
                        
                        collected = last_collected;
                        break;
                    }
                    
                    header = 0; // Move to header section itself
                    
                    if (ota_conn_result != 0) {
                        collected = last_collected;
                        break;
                    }
                } while (recv_bytes < clength);
                
                INFO("- %d", collected);
                
            } else {
                ERROR("Write %i", ret);
                if (is_ssl) {
                    ret = wolfSSL_get_error(ssl, ret);
                    ERROR("SSL %d", ret);
                }
                
                if (ret == -308) {
                    free(recv_buf);
                    if (is_ssl) {
                        wolfSSL_free(ssl);
                    }
                    lwip_close(socket);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    ota_conn_result = new_connection();
                    recv_buf = malloc(RECV_BUF_LEN);
                } else {
                    connection_tries = MAX_DOWNLOAD_FILE_TRIES;
                    break;
                }
            }
            
            if (ota_conn_result != 0) {
                connection_tries = MAX_DOWNLOAD_FILE_TRIES;
                break;
            }
        }
        
        free(recv_buf);
    }
    
    switch (ota_conn_result) {
        case 0:
        case -1:
            if (is_ssl) {
                wolfSSL_free(ssl);
            }
        case -2:
            lwip_close(socket);
        //case -3:
        default:
        ;
    }
    
    if (resume) {
        *resume = collected;
    }
    
    if (connection_tries >= MAX_DOWNLOAD_FILE_TRIES) {
        return 1;
    }
    
    return 0;
}

int ota_get_file_part(char* repo, char* file, int sector, uint16_t port, const bool is_ssl, int* collected) {
    INFO(">>> File part from %s", repo);
    
    return ota_get_file_ex(repo, file, sector, NULL, 0, port, is_ssl, collected);
}

int ota_get_file(char* repo, char* file, int sector, uint16_t port, const bool is_ssl) {
    INFO(">>> File from %s", repo);
    
    return ota_get_file_ex(repo, file, sector, NULL, 0, port, is_ssl, NULL);
}

char* ota_get_version(char* repo, char* version_file, uint16_t port, const bool is_ssl) {
    INFO(">>> Version from %s", repo);

    uint8_t* version = malloc(VERSIONSTRINGLEN + 1);
    memset(version, 0, VERSIONSTRINGLEN + 1);

    if (ota_get_file_ex(repo, version_file, 0, version, VERSIONSTRINGLEN, port, is_ssl, NULL) == 0) {
        INFO("**** %s v%s", version_file, (char*) version);
    } else {
        free(version);
        version = NULL;
        INFO("ERROR");
    }
    
    return (char*) version;
}

int ota_get_sign(char* repo, char* file, uint8_t* signature, uint16_t port, bool is_ssl) {
    INFO(">>> Get sign");
    int ret;
    char* signame = malloc(strlen(file) + 5);
    strcpy(signame, file);
    strcat(signame, SIGNFILESUFIX);
    memset(signature, 0, SIGNSIZE);
    ret = ota_get_file_ex(repo, signame, 0, signature, SIGNSIZE, port, is_ssl, NULL);
    free(signame);
    
    return ret;
}

int ota_verify_sign(int start_sector, int filesize, uint8_t* signature) {
    INFO(">>> Verifying");
    
    int bytes;
    uint8_t hash[HASHSIZE];
    uint8_t* buffer = malloc(1024);
    Sha384 sha;
    
    wc_InitSha384(&sha);

    for (bytes = 0; bytes < filesize - 1024; bytes += 1024) {
#ifdef ESP_PLATFORM
        if (esp_partition_read(get_partition(start_sector), bytes, buffer, 1024) != ESP_OK) {
#else
        if (!spiflash_read(start_sector + bytes, buffer, 1024)) {
#endif
            ERROR("Reading flash");
            break;
        }
        
        if (bytes == 0) {
            buffer[0] = file_first_byte[0];
        }
        
        wc_Sha384Update(&sha, buffer, 1024);
    }
    
#ifdef ESP_PLATFORM
    if (esp_partition_read(get_partition(start_sector), bytes, buffer, filesize - bytes) != ESP_OK) {
#else
    if (!spiflash_read(start_sector + bytes, buffer, filesize - bytes)) {
#endif
        ERROR("Reading flash");
    }
    
    wc_Sha384Update(&sha, buffer, filesize - bytes);
    
    free(buffer);
    
    wc_Sha384Final(&sha, hash);
    
    int verify = 0;
    wc_ecc_verify_hash(signature, SIGNSIZE, hash, HASHSIZE, &verify, &public_key);
    
    INFO(">>> Result %s", verify == 1 ? "OK" : "ERROR");

    return verify - 1;
}

void ota_finalize_file(int sector) {
    INFO("*** FINISHING");
    
#ifdef ESP_PLATFORM
    if (esp_partition_write(get_partition(sector), 0, file_first_byte, 1) != ESP_OK) {
#else
    if (!spiflash_write(sector, file_first_byte, 1)) {
#endif
        
        ERROR("Writing flash");
    }
}

void ota_reboot(const uint8_t partition) {
    INFO("*** REBOOTING");
    
    set_partitions();
    
    if (partition == 1) {
#ifndef ESP_PLATFORM
        rboot_set_temp_rom(1);
#else
        esp_ota_set_boot_partition(partition_boot1);
    } else {
        esp_ota_set_boot_partition(partition_boot0);
#endif
    }
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    sdk_system_restart();
}
