#ifndef __HOMEKIT_TYPES_H__
#define __HOMEKIT_TYPES_H__

#include <stdbool.h>
#include <stdint.h>

#include <homekit/tlv.h>


typedef uint8_t homekit_format_t;                       // 4 bits
#define HOMEKIT_FORMAT_BOOL                            (0)
#define HOMEKIT_FORMAT_UINT8                           (1)
#define HOMEKIT_FORMAT_UINT16                          (2)
#define HOMEKIT_FORMAT_UINT32                          (3)
#define HOMEKIT_FORMAT_UINT64                          (4)
#define HOMEKIT_FORMAT_INT                             (5)
#define HOMEKIT_FORMAT_FLOAT                           (6)
#define HOMEKIT_FORMAT_STRING                          (7)
#define HOMEKIT_FORMAT_TLV                             (8)
#define HOMEKIT_FORMAT_DATA                            (9)

typedef uint8_t homekit_unit_t;                         // 3 bits
#define HOMEKIT_UNIT_NONE                              (0)
#define HOMEKIT_UNIT_CELSIUS                           (1)
#define HOMEKIT_UNIT_PERCENTAGE                        (2)
#define HOMEKIT_UNIT_ARCDEGREES                        (3)
#define HOMEKIT_UNIT_LUX                               (4)
#define HOMEKIT_UNIT_SECONDS                           (5)

typedef uint8_t homekit_permissions_t;                  // 6 bits
#define HOMEKIT_PERMISSIONS_PAIRED_READ                 (1)
#define HOMEKIT_PERMISSIONS_PAIRED_WRITE                (2)
#define HOMEKIT_PERMISSIONS_NOTIFY                      (4)
#define HOMEKIT_PERMISSIONS_ADDITIONAL_AUTHORIZATION    (8)
#define HOMEKIT_PERMISSIONS_TIMED_WRITE                 (16)
#define HOMEKIT_PERMISSIONS_HIDDEN                      (32)

typedef uint8_t homekit_device_category_t;              // 6 bits
#define HOMEKIT_DEVICE_CATEGORY_OTHER                   (1)
#define HOMEKIT_DEVICE_CATEGORY_BRIDGE                  (2)
#define HOMEKIT_DEVICE_CATEGORY_FAN                     (3)
#define HOMEKIT_DEVICE_CATEGORY_GARAGE                  (4)
#define HOMEKIT_DEVICE_CATEGORY_LIGHTBULB               (5)
#define HOMEKIT_DEVICE_CATEGORY_DOOR_LOCK               (6)
#define HOMEKIT_DEVICE_CATEGORY_OUTLET                  (7)
#define HOMEKIT_DEVICE_CATEGORY_SWITCH                  (8)
#define HOMEKIT_DEVICE_CATEGORY_THERSMOSTAT             (9)
#define HOMEKIT_DEVICE_CATEGORY_SENSOR                  (10)
#define HOMEKIT_DEVICE_CATEGORY_SECURITY_SYSTEM         (11)
#define HOMEKIT_DEVICE_CATEGORY_DOOR                    (12)
#define HOMEKIT_DEVICE_CATEGORY_WINDOW                  (13)
#define HOMEKIT_DEVICE_CATEGORY_WINDOW_COVERING         (14)
#define HOMEKIT_DEVICE_CATEGORY_PROGRAMMABLE_SWITCH     (15)
#define HOMEKIT_DEVICE_CATEGORY_RANGE_EXTENDER          (16)
#define HOMEKIT_DEVICE_CATEGORY_IP_CAMERA               (17)
#define HOMEKIT_DEVICE_CATEGORY_VIDEO_DOOR_BELL         (18)
#define HOMEKIT_DEVICE_CATEGORY_AIR_PURIFIER            (19)
#define HOMEKIT_DEVICE_CATEGORY_HEATER                  (20)
#define HOMEKIT_DEVICE_CATEGORY_AIR_CONDITIONER         (21)
#define HOMEKIT_DEVICE_CATEGORY_HUMIDIFIER              (22)
#define HOMEKIT_DEVICE_CATEGORY_DEHUMIDIFIER            (23)
#define HOMEKIT_DEVICE_CATEGORY_APPLE_TV                (24)
#define HOMEKIT_DEVICE_CATEGORY_HOMEPOD                 (25)
#define HOMEKIT_DEVICE_CATEGORY_SPEAKER                 (26)
#define HOMEKIT_DEVICE_CATEGORY_AIRPORT                 (27)
#define HOMEKIT_DEVICE_CATEGORY_SPRINKLER               (28)
#define HOMEKIT_DEVICE_CATEGORY_FAUCET                  (29)
#define HOMEKIT_DEVICE_CATEGORY_SHOWER_HEAD             (30)
#define HOMEKIT_DEVICE_CATEGORY_TELEVISION              (31)
#define HOMEKIT_DEVICE_CATEGORY_TARGET_CONTROLLER       (32)
#define HOMEKIT_DEVICE_CATEGORY_WIFI_ROUTER             (33)
#define HOMEKIT_DEVICE_CATEGORY_AUDIO_RECEIVER          (34)
#define HOMEKIT_DEVICE_CATEGORY_TV_SET_BOX              (35)
#define HOMEKIT_DEVICE_CATEGORY_TV_STREAMING_STICK      (36)

struct _homekit_accessory;
struct _homekit_service;
struct _homekit_characteristic;
typedef struct _homekit_accessory homekit_accessory_t;
typedef struct _homekit_service homekit_service_t;
typedef struct _homekit_characteristic homekit_characteristic_t;


typedef struct {
    bool is_null: 1;
    bool is_static: 1;
    homekit_format_t format: 4;
    uint32_t data_size: 26;
    union {
        bool bool_value;
        int int_value;
        float float_value;
        char* string_value;
        uint8_t* data_value;
        tlv_values_t* tlv_values;
    };
} homekit_value_t;

bool homekit_value_equal(homekit_value_t *a, homekit_value_t *b);
void homekit_value_copy(homekit_value_t *dst, homekit_value_t *src);
homekit_value_t *homekit_value_clone(homekit_value_t *value);
void homekit_value_destruct(homekit_value_t *value);
void homekit_value_free(homekit_value_t *value);


#define HOMEKIT_NULL_(...) \
    {.format=HOMEKIT_FORMAT_BOOL, .is_null=true, ##__VA_ARGS__}
#define HOMEKIT_NULL(...) (homekit_value_t) HOMEKIT_NULL_( __VA_ARGS__ )

#define HOMEKIT_BOOL_(value, ...) \
    {.format=HOMEKIT_FORMAT_BOOL, .bool_value=(value), ##__VA_ARGS__}
#define HOMEKIT_BOOL(value, ...) (homekit_value_t) HOMEKIT_BOOL_(value, __VA_ARGS__)

#define HOMEKIT_INT_(value, ...) \
    {.format=HOMEKIT_FORMAT_INT, .int_value=(value), ##__VA_ARGS__}
#define HOMEKIT_INT(value, ...) (homekit_value_t) HOMEKIT_INT_(value, ##__VA_ARGS__)

#define HOMEKIT_UINT8_(value, ...) \
    {.format=HOMEKIT_FORMAT_UINT8, .int_value=(value), ##__VA_ARGS__}
#define HOMEKIT_UINT8(value, ...) (homekit_value_t) HOMEKIT_UINT8_(value, ##__VA_ARGS__)

#define HOMEKIT_UINT16_(value, ...) \
    {.format=HOMEKIT_FORMAT_UINT16, .int_value=(value), ##__VA_ARGS__}
#define HOMEKIT_UINT16(value, ...) (homekit_value_t) HOMEKIT_UINT16_(value, ##__VA_ARGS__)

#define HOMEKIT_UINT32_(value, ...) \
    {.format=HOMEKIT_FORMAT_UINT32, .int_value=(value), ##__VA_ARGS__}
#define HOMEKIT_UINT32(value, ...) (homekit_value_t) HOMEKIT_UINT32_(value, ##__VA_ARGS__)

#define HOMEKIT_UINT64_(value, ...) \
    {.format=HOMEKIT_FORMAT_UINT64, .int_value=(value), ##__VA_ARGS__}
#define HOMEKIT_UINT64(value, ...) (homekit_value_t) HOMEKIT_UINT64_(value, ##__VA_ARGS__)

#define HOMEKIT_FLOAT_(value, ...) \
    {.format=HOMEKIT_FORMAT_FLOAT, .float_value=(value), ##__VA_ARGS__}
#define HOMEKIT_FLOAT(value, ...) (homekit_value_t) HOMEKIT_FLOAT_(value, ##__VA_ARGS__)

#define HOMEKIT_STRING_(value, ...) \
    {.format=HOMEKIT_FORMAT_STRING, .string_value=(value), ##__VA_ARGS__}
#define HOMEKIT_STRING(value, ...) (homekit_value_t) HOMEKIT_STRING_(value, ##__VA_ARGS__)

#define HOMEKIT_TLV_(value, ...) \
    {.format=HOMEKIT_FORMAT_TLV, .tlv_values=(value), ##__VA_ARGS__}
#define HOMEKIT_TLV(value, ...) (homekit_value_t) HOMEKIT_TLV_(value, ##__VA_ARGS__)

#define HOMEKIT_DATA_(value, size, ...) \
    {.format=HOMEKIT_FORMAT_DATA, .data_value=(value), .data_size=(size), ##__VA_ARGS__}
#define HOMEKIT_DATA(value, size, ...) (homekit_value_t) HOMEKIT_DATA_(value, size, ##__VA_ARGS__)


typedef struct {
    unsigned int count;
    uint8_t* values;
} homekit_valid_values_t;

#ifndef HOMEKIT_DISABLE_VALUE_RANGES
typedef struct {
    uint8_t start;
    uint8_t end;
} homekit_valid_values_range_t;

typedef struct {
    unsigned int count;
    homekit_valid_values_range_t *ranges;
} homekit_valid_values_ranges_t;
#endif //HOMEKIT_DISABLE_VALUE_RANGES

typedef struct _homekit_characteristic_subscription {
    void *context;
    struct _homekit_characteristic_subscription *next;
} homekit_characteristic_subscription_t;


struct _homekit_characteristic {
    homekit_service_t *service;
    const char *type;
    const char *description;
    
    uint16_t id;
    homekit_format_t format: 4;
    homekit_unit_t unit: 3;
    homekit_permissions_t permissions: 6;
    int _align: 3;
    
    homekit_value_t value;
    
    float *min_value;
    float *max_value;
    float *min_step;
    
#ifndef HOMEKIT_DISABLE_MAXLEN_CHECK
    int *max_len;
    int *max_data_len;
#endif //HOMEKIT_DISABLE_MAXLEN_CHECK

    homekit_valid_values_t valid_values;
    
#ifndef HOMEKIT_DISABLE_VALUE_RANGES
    homekit_valid_values_ranges_t valid_values_ranges;
#endif //HOMEKIT_DISABLE_VALUE_RANGES
    
    homekit_characteristic_subscription_t* subscriptions;
    
    homekit_value_t (*getter_ex)(const homekit_characteristic_t *ch);
    void (*setter_ex)(homekit_characteristic_t *ch, const homekit_value_t value);
};

struct _homekit_service {
    homekit_accessory_t *accessory;

    uint16_t id;
    bool hidden: 1;
    bool primary: 1;

    const char *type;

    homekit_service_t **linked;
    homekit_characteristic_t **characteristics;
};

struct _homekit_accessory {
    uint16_t id;
    
    homekit_service_t **services;
};

// Macro to define accessory
#define HOMEKIT_ACCESSORY(...) \
    &(homekit_accessory_t) { \
        ##__VA_ARGS__ \
    }

// Macro to define service inside accessory definition.
// Requires HOMEKIT_SERVICE_<name> define to expand to service type UUID string
#define HOMEKIT_SERVICE(_type, ...) \
    &(homekit_service_t) { .type=HOMEKIT_SERVICE_ ## _type, ##__VA_ARGS__ }

// Macro to define standalone service (outside of accessory definition)
// Requires HOMEKIT_SERVICE_<name> define to expand to service type UUID string
#define HOMEKIT_SERVICE_(_type, ...) \
    { .type=HOMEKIT_SERVICE_ ## _type, ##__VA_ARGS__ }

// Macro to define characteristic inside service definition
#define HOMEKIT_CHARACTERISTIC(name, ...) \
    &(homekit_characteristic_t) { \
        HOMEKIT_DECLARE_CHARACTERISTIC_ ## name( __VA_ARGS__ ) \
    }

// Macro to define standalone characteristic (outside of service definition)
// Requires HOMEKIT_DECLARE_CHARACTERISTIC_<name>() macro
#define HOMEKIT_CHARACTERISTIC_(name, ...) \
    { \
        HOMEKIT_DECLARE_CHARACTERISTIC_ ## name( __VA_ARGS__ ) \
    }

// Declaration macro to create a custom characteristic inplace without
// having to define HOMKIT_DECLARE_CHARACTERISTIC_<name>() macro.
//
// Useage:
//     homekit_characteristic_t custom_ch = HOMEKIT_CHARACTERISTIC_(
//         CUSTOM,
//         .type = "00000023-0000-1000-8000-0026BB765291",
//         .description = "My custom characteristic",
//         .format = homekit_format_string,
//         .permissions = HOMEKIT_PERMISSIONS_PAIRED_READ
//                      | HOMEKIT_PERMISSIONS_PAIRED_WRITE,
//         .value = HOMEKIT_STRING_("my value"),
//     );
#define HOMEKIT_DECLARE_CHARACTERISTIC_CUSTOM(...) \
    .format = HOMEKIT_FORMAT_UINT8, \
    .unit = HOMEKIT_UNIT_NONE, \
    .permissions = HOMEKIT_PERMISSIONS_PAIRED_READ, \
    ##__VA_ARGS__


// Allocate memory and copy given accessory
// Does not make copies of all accessory services, so make sure thay are
// either allocated on heap or in static memory (but not on stack).
homekit_accessory_t *homekit_accessory_clone(homekit_accessory_t *accessory);
// Allocate memory and copy given service.
// Does not make copies of all service characteristics, so make sure that are
// either allocated on heap or in static memory (but not on stack).
homekit_service_t *homekit_service_clone(homekit_service_t *service);
// Allocate memory and copy given characteristic.
homekit_characteristic_t *homekit_characteristic_clone(homekit_characteristic_t *characteristic);


// Macro to define an accessory in dynamic memory.
// Used to aid creating accessories definitions in runtime.
// Makes copy of all internal services/characteristics.
#define NEW_HOMEKIT_ACCESSORY(...) \
    homekit_accessory_clone(HOMEKIT_ACCESSORY(__VA_ARGS__))

// Macro to define an service in dynamic memory.
// Used to aid creating services definitions in runtime.
// Makes copy of all internal characteristics.
#define NEW_HOMEKIT_SERVICE(name, ...) \
    homekit_service_clone(HOMEKIT_SERVICE(name, ## __VA_ARGS__))

// Macro to define an characteristic in dynamic memory.
// Used to aid creating characteristics definitions in runtime.
#define NEW_HOMEKIT_CHARACTERISTIC(name, ...) \
    homekit_characteristic_clone(HOMEKIT_CHARACTERISTIC(name, ## __VA_ARGS__))


// Init accessories by automatically assigning IDs to all
// accessories/services/characteristics, normalizing internal data.
void homekit_accessories_init(homekit_accessory_t **accessories);

// Find accessory by ID. Returns NULL if not found
homekit_accessory_t *homekit_accessory_by_id(homekit_accessory_t **accessories, int aid);
// Find service inside accessory by service type. Returns NULL if not found
homekit_service_t *homekit_service_by_type(homekit_accessory_t *accessory, const char *type);
// Find characteristic inside service by type. Returns NULL if not found
homekit_characteristic_t *homekit_service_characteristic_by_type(homekit_service_t *service, const char *type);
// Find characteristic by accessory ID and characteristic ID. Returns NULL if not found
homekit_characteristic_t *homekit_characteristic_by_aid_and_iid(homekit_accessory_t **accessories, int aid, int iid);

void homekit_characteristic_notify(homekit_characteristic_t *ch);
void homekit_characteristic_add_notify_subscription(
    homekit_characteristic_t *ch,
    void *context
);
void homekit_characteristic_remove_notify_subscription(
    homekit_characteristic_t *ch,
    void *context
);
void homekit_accessories_clear_notify_subscriptions(
    homekit_accessory_t **accessories,
    void *context
);
bool homekit_characteristic_has_notify_subscription(
    const homekit_characteristic_t *ch,
    void *context
);


#endif // __HOMEKIT_TYPES_H__
