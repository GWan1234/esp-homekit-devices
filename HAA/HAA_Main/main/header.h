/*
 * Home Accessory Architect
 *
 * Copyright 2019-2024 José Antonio Jiménez Campos (@RavenSystem)
 *
 */

#ifndef __HAA_HEADER_H__
#define __HAA_HEADER_H__

#include "../../common/common_headers.h"

// Version
#define HAA_FIRMWARE_VERSION                "12.14.2"
#define HAA_FIRMWARE_BETA_REVISION          ""          // Format: "b01"
#define HAA_FIRMWARE_CODENAME               "Merlin"

// Characteristic types (ch_type)
#define CH_TYPE_BOOL                        (0)
#define CH_TYPE_INT8                        (1)
#define CH_TYPE_INT                         (2)
#define CH_TYPE_FLOAT                       (3)
#define CH_TYPE_STRING                      (4)

#define FLOAT_FACTOR_SAVE_AS_INT            (100000.f)

// Task Stack Sizes
#define GLOBAL_TASK_SIZE                    (TASK_SIZE_FACTOR * (640))

#define INITIAL_SETUP_TASK_SIZE             (TASK_SIZE_FACTOR * (1600))
#define NTP_TASK_SIZE                       (TASK_SIZE_FACTOR * (512))
#define PING_TASK_SIZE                      (TASK_SIZE_FACTOR * (896))
#define AUTODIMMER_TASK_SIZE                GLOBAL_TASK_SIZE
#define IRRF_TX_TASK_SIZE                   (TASK_SIZE_FACTOR * (456))
#define UART_ACTION_TASK_SIZE               (TASK_SIZE_FACTOR * (384))
#define NETWORK_ACTION_TASK_SIZE            (TASK_SIZE_FACTOR * (544))
#define TEMPERATURE_TASK_SIZE               GLOBAL_TASK_SIZE
#define PROCESS_TH_TASK_SIZE                GLOBAL_TASK_SIZE
#define PROCESS_HUMIDIF_TASK_SIZE           GLOBAL_TASK_SIZE
#define PROCESS_FAN_TASK_SIZE               GLOBAL_TASK_SIZE
#define SET_ZONES_TASK_SIZE                 GLOBAL_TASK_SIZE
#define LIGHTBULB_TASK_SIZE                 GLOBAL_TASK_SIZE
#define POWER_MONITOR_TASK_SIZE             GLOBAL_TASK_SIZE
#define FREE_MONITOR_TASK_SIZE              GLOBAL_TASK_SIZE
#define LIGHT_SENSOR_TASK_SIZE              GLOBAL_TASK_SIZE
#define WIFI_PING_GW_TASK_SIZE              (TASK_SIZE_FACTOR * (384))
#define WIFI_RECONNECTION_TASK_SIZE         GLOBAL_TASK_SIZE
#define RECV_UART_TASK_SIZE                 (TASK_SIZE_FACTOR * (384))
#define REBOOT_TASK_SIZE                    (TASK_SIZE_FACTOR * (384))
#define IRRF_CAPTURE_TASK_SIZE              (TASK_SIZE_FACTOR * (512))

// Task Priorities
#define INITIAL_SETUP_TASK_PRIORITY         (tskIDLE_PRIORITY + 1)
#define NTP_TASK_PRIORITY                   (tskIDLE_PRIORITY + 1)
#define PING_TASK_PRIORITY                  (tskIDLE_PRIORITY + 2)
#define AUTODIMMER_TASK_PRIORITY            (tskIDLE_PRIORITY + 1)
#define IRRF_TX_TASK_PRIORITY               (tskIDLE_PRIORITY + 1)
#define UART_ACTION_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)
#define NETWORK_ACTION_TASK_PRIORITY        (tskIDLE_PRIORITY + 1)
#define TEMPERATURE_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)
#define PROCESS_TH_TASK_PRIORITY            (tskIDLE_PRIORITY + 1)
#define PROCESS_HUMIDIF_TASK_PRIORITY       (tskIDLE_PRIORITY + 1)
#define PROCESS_FAN_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)
#define SET_ZONES_TASK_PRIORITY             (tskIDLE_PRIORITY + 1)
#define LIGHTBULB_TASK_PRIORITY             (tskIDLE_PRIORITY + 1)
#define POWER_MONITOR_TASK_PRIORITY         (tskIDLE_PRIORITY + 1)
#define FREE_MONITOR_TASK_PRIORITY          (tskIDLE_PRIORITY + 1)
#define LIGHT_SENSOR_TASK_PRIORITY          (tskIDLE_PRIORITY + 1)
#define WIFI_PING_GW_TASK_PRIORITY          (tskIDLE_PRIORITY + 1)
#define WIFI_RECONNECTION_TASK_PRIORITY     (tskIDLE_PRIORITY + 1)
#define RECV_UART_TASK_PRIORITY             (tskIDLE_PRIORITY + 4)
#define REBOOT_TASK_PRIORITY                (tskIDLE_PRIORITY + 3)
#define IRRF_CAPTURE_TASK_PRIORITY          (configMAX_PRIORITIES - 2)

// Button Events
#define SINGLEPRESS_EVENT                   (0)
#define DOUBLEPRESS_EVENT                   (1)
#define LONGPRESS_EVENT                     (2)

// Initial states
#define INIT_STATE_FIXED_INPUT              (4)
#define INIT_STATE_LAST                     (5)
#define INIT_STATE_INV_LAST                 (6)
#define INIT_STATE_LAST_STR                 "{\"s\":5}"

// MEPLHAA Script
#define GENERAL_CONFIG                      "c"
#define MDNS_TTL                            "ttl"
#define MDNS_TTL_DEFAULT                    (4500)
#define MDNS_TTL_PERIOD_DEFAULT             (2250)
#define CUSTOM_HOSTNAME                     "n"
#define CUSTOM_NTP_HOST                     "ntp"
#define NTP_DISABLE_STRING                  "0"
#define TIMEZONE                            "tz"
#define LOG_OUTPUT                          "o"
#define LOG_OUTPUT_TARGET                   "ot"
#define ALLOWED_SETUP_MODE_TIME             "m"
#define STATUS_LED_GPIO                     "l"
#define INVERTED                            "i"
#define BUTTON_FILTER                       "f"
#define FLEX_FILTER_ARRAY                   "fl"
#define BUTTON_CONTINUOS_MODE               "c"
#define PWMS_FREQ                           "q"
#define PWMH_FREQ_ARRAY                     "y"
#define PWMH_FREQ_DEFAULT                   (5000)
#define PWMH_BITS_DIVISOR                   (6)
#define PWM_ZEROCROSSING_ARRAY_SET          "zc"
#define ENABLE_HOMEKIT                      "h"
#define HOMEKIT_SERVER_MAX_CLIENTS          "h"
#define HOMEKIT_SERVER_MAX_CLIENTS_MAX      (12)
#define HOMEKIT_SERVER_MAX_CLIENTS_DEFAULT  (HOMEKIT_SERVER_MAX_CLIENTS_MAX)
#define ALLOW_INSECURE_CONNECTIONS          "u"
#define UART_CONFIG_ARRAY                   "r"
#define UART_CONFIG_ENABLE                  "n"
#define UART_CONFIG_STOPBITS                "b"
#define UART_CONFIG_PARITY                  "p"
#define UART_CONFIG_SPEED                   "s"
#define UART_CONFIG_MODE                    "m"
#define UART_CONFIG_GPIO_ARRAY              "g"
#define RECV_UART_BUFFER_LEN_ARRAY_SET      "l"
#define RECV_UART_BUFFER_MIN_LEN_DEFAULT    (1)
#define RECV_UART_BUFFER_MAX_LEN_DEFAULT    (128)
#define ACCESSORIES_ARRAY                   "a"
#define EXTRA_SERVICES_ARRAY                "es"
#define BUTTONS_ARRAY                       "b"
#define FIXED_BUTTONS_ARRAY_0               "f0"
#define FIXED_BUTTONS_ARRAY_1               "f1"
#define FIXED_BUTTONS_ARRAY_2               "f2"
#define FIXED_BUTTONS_ARRAY_3               "f3"
#define FIXED_BUTTONS_ARRAY_4               "f4"
#define FIXED_BUTTONS_ARRAY_5               "f5"
#define FIXED_BUTTONS_ARRAY_6               "f6"
#define FIXED_BUTTONS_ARRAY_7               "f7"
#define FIXED_BUTTONS_ARRAY_8               "f8"
#define FIXED_BUTTONS_STATUS_ARRAY_0        "g0"
#define FIXED_BUTTONS_STATUS_ARRAY_1        "g1"
#define FIXED_BUTTONS_STATUS_ARRAY_2        "g2"
#define FIXED_BUTTONS_STATUS_ARRAY_3        "g3"
#define PINGS_ARRAY                         "l"
#define PINGS_ARRAY_1                       "l1"
#define FIXED_PINGS_ARRAY_0                 "p0"
#define FIXED_PINGS_ARRAY_1                 "p1"
#define FIXED_PINGS_ARRAY_2                 "p2"
#define FIXED_PINGS_ARRAY_3                 "p3"
#define FIXED_PINGS_ARRAY_4                 "p4"
#define FIXED_PINGS_ARRAY_5                 "p5"
#define FIXED_PINGS_ARRAY_6                 "p6"
#define FIXED_PINGS_ARRAY_7                 "p7"
#define FIXED_PINGS_ARRAY_8                 "p8"
#define FIXED_PINGS_STATUS_ARRAY_0          "q0"
#define FIXED_PINGS_STATUS_ARRAY_1          "q1"
#define FIXED_PINGS_STATUS_ARRAY_2          "q2"
#define FIXED_PINGS_STATUS_ARRAY_3          "q3"
#define PING_HOST                           "h"
#define PING_RESPONSE_TYPE                  "r"
#define PING_IGNORE_LAST_RESPONSE           "i"
#define PING_DISABLE_WITHOUT_WIFI           "d"
#define PING_RETRIES                        (4)
#define PING_POLL_PERIOD                    "pt"
#define PING_POLL_PERIOD_DEFAULT            (5.f)
#define BUTTON_PRESS_TYPE                   "t"
#define BUTTON_MODE                         "m"
#define PULLUP_RESISTOR                     "p"
#define VALUE                               "v"
#define AUTOSWITCH_TIME                     "i"
#define PIN_GPIO                            "g"
#define INITIAL_STATE                       "s"
#define EXEC_ACTIONS_ON_BOOT                "xa"
#define KILLSWITCH                          "ks"
#define IO_CONFIG_ARRAY                     "io"
#define IO_GPIO_MODE                        io_value[0]
#define IO_GPIO_PULL_UP_DOWN                io_value[1]
#define IO_GPIO_BUTTON_MODE                 io_value[2]
#define IO_GPIO_BUTTON_FILTER               io_value[3]
#define IO_GPIO_US_TIME_BETWEEN_PULSES      io_value[4]
#define IO_GPIO_OUTPUT_INIT_VALUE           io_value[2]
#define IO_GPIO_PWM_MODE                    io_value[3]
#define IO_GPIO_PWM_DITHERING               io_value[4]
#define IO_GPIO_PWMH_TIMER                  io_value[4]
#define IO_GPIO_ADC_ATTENUATION             io_value[2]

#define TIMETABLE_ACTION_ARRAY              "tt"
#define ALL_MONS                            13
#define ALL_MDAYS                           0
#define ALL_HOURS                           24
#define ALL_MINS                            60
#define ALL_WDAYS                           7

#define VALVE_SYSTEM_TYPE                   "w"
#define VALVE_SYSTEM_TYPE_DEFAULT           (0)
#define VALVE_MAX_DURATION                  "d"
#define VALVE_MAX_DURATION_DEFAULT          (3600)

#define BINARY_SENSOR_EXTRA_DATA_SET        "dt"

#define THERMOSTAT_TYPE                     "w"
#define TH_TYPE                             ch_group->num_i[5]
#define THERMOSTAT_TYPE_HEATER              (1)
#define THERMOSTAT_TYPE_COOLER              (2)
#define THERMOSTAT_TYPE_HEATERCOOLER        (3)
#define THERMOSTAT_TYPE_HEATERCOOLER_NOAUTO (4)
#define THERMOSTAT_INIT_TARGET_MODE         "e"
#define THERMOSTAT_INIT_TARGET_MODE_DEFAULT (3)
#define THERMOSTAT_MIN_TEMP                 "m"
#define TH_HEATER_MIN_TEMP                  *ch_group->ch[5]->min_value
#define TH_COOLER_MIN_TEMP                  *ch_group->ch[6]->min_value
#define THERMOSTAT_DEFAULT_MIN_TEMP         (10)
#define THERMOSTAT_MAX_TEMP                 "x"
#define TH_HEATER_MAX_TEMP                  *ch_group->ch[5]->max_value
#define TH_COOLER_MAX_TEMP                  *ch_group->ch[6]->max_value
#define THERMOSTAT_DEFAULT_MAX_TEMP         (38)
#define THERMOSTAT_DEADBAND                 "d"
#define TH_DEADBAND                         ch_group->num_f[2]
#define THERMOSTAT_DEADBAND_FORCE_IDLE      "df"
#define TH_DEADBAND_FORCE_IDLE              ch_group->num_f[3]
#define THERMOSTAT_DEADBAND_SOFT_ON         "ds"
#define TH_DEADBAND_SOFT_ON                 ch_group->num_f[4]
#define THERMOSTAT_DEADBAND_OFFSET          "o"
#define TH_DEADBAND_OFFSET                  ch_group->num_f[5]
#define THERMOSTAT_UPDATE_DELAY             "dl"
#define THERMOSTAT_UPDATE_DELAY_MIN         (0.15f)
#define THERMOSTAT_UPDATE_DELAY_DEFAULT     (3.0f)
#define THERMOSTAT_TARGET_TEMP_STEP         "st"
#define TH_IAIRZONING_CONTROLLER_SET        "ia"
#define TH_IAIRZONING_GATE_ALL_OFF_STATE_SET "it"
#define THERMOSTAT_CURRENT_ACTION           ch_group->num_i[6]
#define TH_IAIRZONING_CONTROLLER            ch_group->num_i[7]
#define TH_IAIRZONING_GATE_CURRENT_STATE    ch_group->num_i[8]
#define TH_IAIRZONING_GATE_ALL_OFF_STATE    ch_group->num_i[9]
#define TH_IAIRZONING_GATE_CLOSE            (0)
#define TH_IAIRZONING_GATE_OPEN             (1)
#define THERMOSTAT_MODE_OFF                 (0)
#define THERMOSTAT_MODE_IDLE                (1)
#define THERMOSTAT_MODE_HEATER              (2)
#define THERMOSTAT_MODE_COOLER              (3)
#define THERMOSTAT_TARGET_MODE_AUTO         (0)
#define THERMOSTAT_TARGET_MODE_HEATER       (1)
#define THERMOSTAT_TARGET_MODE_COOLER       (2)
#define THERMOSTAT_ACTION_TOTAL_OFF         (0)
#define THERMOSTAT_ACTION_HEATER_IDLE       (1)
#define THERMOSTAT_ACTION_COOLER_IDLE       (2)
#define THERMOSTAT_ACTION_HEATER_ON         (3)
#define THERMOSTAT_ACTION_COOLER_ON         (4)
#define THERMOSTAT_ACTION_SENSOR_ERROR      (5)
#define THERMOSTAT_ACTION_TEMP_UP           (6)
#define THERMOSTAT_ACTION_TEMP_DOWN         (7)
#define THERMOSTAT_ACTION_HEATER_FORCE_IDLE (8)
#define THERMOSTAT_ACTION_COOLER_FORCE_IDLE (9)
#define THERMOSTAT_ACTION_HEATER_SOFT_ON    (10)
#define THERMOSTAT_ACTION_COOLER_SOFT_ON    (11)
#define THERMOSTAT_ACTION_GATE_CLOSE        (12)
#define THERMOSTAT_ACTION_GATE_OPEN         (13)
#define THERMOSTAT_ACTION_ON                (14)
#define THERMOSTAT_TEMP_UP                  (0)
#define THERMOSTAT_TEMP_DOWN                (1)
#define TH_ACTIVE                           ch_group->ch[2]
#define TH_MODE                             ch_group->ch[3]
#define TH_TARGET_MODE                      ch_group->ch[4]
#define TH_HEATER_TARGET_TEMP               ch_group->ch[5]
#define TH_COOLER_TARGET_TEMP               ch_group->ch[6]
#define TH_ACTIVE_INT                       ch_group->ch[2]->value.int_value
#define TH_MODE_INT                         ch_group->ch[3]->value.int_value
#define TH_TARGET_MODE_INT                  ch_group->ch[4]->value.int_value
#define TH_HEATER_TARGET_TEMP_FLOAT         ch_group->ch[5]->value.float_value
#define TH_COOLER_TARGET_TEMP_FLOAT         ch_group->ch[6]->value.float_value

#define IAIRZONING_LAST_ACTION              iairzoning_group->num_i[0]
#define IAIRZONING_MAIN_MODE                iairzoning_group->num_i[1]
#define IAIRZONING_DELAY_ACTION_SET         "d"
#define IAIRZONING_DELAY_ACTION_DEFAULT     (0.1f)
#define IAIRZONING_DELAY_ACTION_NUM         num_f[0]
#define IAIRZONING_DELAY_ACTION             iairzoning_group->IAIRZONING_DELAY_ACTION_NUM
#define IAIRZONING_DELAY_ACTION_CH_GROUP    ch_group->IAIRZONING_DELAY_ACTION_NUM
#define IAIRZONING_DELAY_AFT_CLOSE_SET      "c"
#define IAIRZONING_DELAY_AFT_CLOSE_DEFAULT  (0.f)
#define IAIRZONING_DELAY_AFT_CLOSE_NUM      num_f[1]
#define IAIRZONING_DELAY_AFT_CLOSE          iairzoning_group->IAIRZONING_DELAY_AFT_CLOSE_NUM
#define IAIRZONING_DELAY_AFT_CLOSE_CH_GROUP ch_group->IAIRZONING_DELAY_AFT_CLOSE_NUM

#define TEMPERATURE_SENSOR_GPIO             "g"
#define TH_SENSOR_GPIO                      ch_group->num_i[0]
#define TH_SENSOR_GPIO_OUTPUT               ch_group->num_i[1]
#define TEMPERATURE_SENSOR_TYPE             "n"
#define TH_SENSOR_TYPE                      ch_group->num_i[2]
#define TEMPERATURE_SENSOR_INDEX            "u"
#define TH_SENSOR_INDEX                     ch_group->num_i[3]
#define TEMPERATURE_SENSOR_POLL_PERIOD      "j"
#define TH_SENSOR_POLL_PERIOD_DEFAULT       (30)
#define TH_SENSOR_POLL_PERIOD_MIN           (0.1f)
#define TEMPERATURE_OFFSET                  "z"
#define TH_SENSOR_TEMP_OFFSET               ch_group->num_f[0]
#define HUMIDITY_OFFSET                     "k"
#define TH_SENSOR_HUM_OFFSET                ch_group->num_f[1]
#define SENSOR_TEMPERATURE_FLOAT            ch_group->ch[0]->value.float_value
#define SENSOR_HUMIDITY_FLOAT               ch_group->ch[1]->value.float_value
#define TH_SENSOR_ERROR_COUNT               ch_group->num_i[4]
#define TH_SENSOR_MAX_ALLOWED_ERRORS        (3)
#define TH_SENSOR_TEMP_VALUE_WHEN_ERROR     (-99.f)

#define HUMIDIF_TYPE                        "w"
#define HM_TYPE                             ch_group->num_i[5]
#define HUMIDIF_TYPE_HUM                    (1)
#define HUMIDIF_TYPE_DEHUM                  (2)
#define HUMIDIF_TYPE_HUMDEHUM               (3)
#define HUMIDIF_TYPE_HUMDEHUM_NOAUTO        (4)
#define HUMIDIF_INIT_TARGET_MODE            "e"
#define HUMIDIF_INIT_TARGET_MODE_DEFAULT    (3)
#define HUMIDIF_DEADBAND                    "d"
#define HM_DEADBAND                         ch_group->num_f[2]
#define HUMIDIF_DEADBAND_FORCE_IDLE         "df"
#define HM_DEADBAND_FORCE_IDLE              ch_group->num_f[3]
#define HUMIDIF_DEADBAND_SOFT_ON            "ds"
#define HM_DEADBAND_SOFT_ON                 ch_group->num_f[4]
#define HUMIDIF_DEADBAND_OFFSET             "o"
#define HM_DEADBAND_OFFSET                  ch_group->num_f[5]
#define HUMIDIF_CURRENT_ACTION              ch_group->num_i[6]
#define HUMIDIF_MODE_OFF                    (0)
#define HUMIDIF_MODE_IDLE                   (1)
#define HUMIDIF_MODE_HUM                    (2)
#define HUMIDIF_MODE_DEHUM                  (3)
#define HUMIDIF_TARGET_MODE_AUTO            (0)
#define HUMIDIF_TARGET_MODE_HUM             (1)
#define HUMIDIF_TARGET_MODE_DEHUM           (2)
#define HUMIDIF_ACTION_TOTAL_OFF            (0)
#define HUMIDIF_ACTION_HUM_IDLE             (1)
#define HUMIDIF_ACTION_DEHUM_IDLE           (2)
#define HUMIDIF_ACTION_HUM_ON               (3)
#define HUMIDIF_ACTION_DEHUM_ON             (4)
#define HUMIDIF_ACTION_UP                   (6)
#define HUMIDIF_ACTION_DOWN                 (7)
#define HUMIDIF_ACTION_HUM_FORCE_IDLE       (8)
#define HUMIDIF_ACTION_DEHUM_FORCE_IDLE     (9)
#define HUMIDIF_ACTION_HUM_SOFT_ON          (10)
#define HUMIDIF_ACTION_DEHUM_SOFT_ON        (11)
#define HUMIDIF_ACTION_ON                   (14)
#define HUMIDIF_UP                          (0)
#define HUMIDIF_DOWN                        (1)
#define HM_ACTIVE                           ch_group->ch[2]
#define HM_MODE                             ch_group->ch[3]
#define HM_TARGET_MODE                      ch_group->ch[4]
#define HM_HUM_TARGET                       ch_group->ch[5]
#define HM_DEHUM_TARGET                     ch_group->ch[6]
#define HM_ACTIVE_INT                       ch_group->ch[2]->value.int_value
#define HM_MODE_INT                         ch_group->ch[3]->value.int_value
#define HM_TARGET_MODE_INT                  ch_group->ch[4]->value.int_value
#define HM_HUM_TARGET_FLOAT                 ch_group->ch[5]->value.float_value
#define HM_DEHUM_TARGET_FLOAT               ch_group->ch[6]->value.float_value

#define LIGHTBULB_TYPE_SET                  "ty"
#define LIGHTBULB_TYPE                      lightbulb_group->type
#define LIGHTBULB_TYPE_VIRTUAL              (0)
#define LIGHTBULB_TYPE_PWM                  (1)
#define LIGHTBULB_TYPE_PWM_CWWW             (2)
#define LIGHTBULB_TYPE_MY92X1               (3)
#define LIGHTBULB_TYPE_SM16716              (7)
#define LIGHTBULB_TYPE_NRZ                  (8)
#define LIGHTBULB_NRZ_TIMES_ARRAY_SET       "nrz"
#define LIGHTBULB_CHANNELS_SET              "n"
#define LIGHTBULB_CHANNELS                  lightbulb_group->channels
#define LIGHTBULB_INITITAL_STATE_ARRAY_SET  "it"
#define LIGHTBULB_GPIO_ARRAY_SET            "g"
#define LIGHTBULB_RGB_ARRAY_SET             "rgb"
#define LIGHTBULB_CMY_ARRAY_SET             "cmy"
#define LIGHTBULB_MAX_POWER_SET             "mp"
#define LIGHTBULB_MAX_POWER                 ch_group->num_f[0]
#define LIGHTBULB_CURVE_FACTOR_SET          "cf"
#define LIGHTBULB_CURVE_FACTOR              ch_group->num_f[1]
#define LIGHTBULB_COLOR_MAP_SET             "cm"
#define LIGHTBULB_RANGE_START               lightbulb_group->range_start
#define LIGHTBULB_RANGE_END                 lightbulb_group->range_end
#define LIGHTBULB_STEP_VALUE                lightbulb_group->step_value
#define LIGHTBULB_FLUX_ARRAY_SET            "fa"
#define LIGHTBULB_COORDINATE_ARRAY_SET      "ca"
#define LIGHTBULB_WHITE_POINT_SET           "wp"
#define RGBW_PERIOD                         (20)
#define RGBW_STEP_SET                       "st"
#define RGBW_STEP_DEFAULT                   (102.4 * RGBW_PERIOD)
#define PWM_SCALE                           (UINT16_MAX)
#define COLOR_TEMP_MIN                      (71)
#define COLOR_TEMP_MAX                      (400)
#define LIGHTBULB_BRIGHTNESS_UP             (0)
#define LIGHTBULB_BRIGHTNESS_DOWN           (1)
#define AUTODIMMER_DELAY                    (400)
#define AUTODIMMER_TASK_DELAY_SET           "d"
#define AUTODIMMER_TASK_DELAY_DEFAULT       (1000)
#define AUTODIMMER_TASK_STEP_SET            "e"
#define AUTODIMMER_TASK_STEP_DEFAULT        (20)
#define LIGHTBULB_SET_DELAY_MS              (100)
#define R                                   lightbulb_group->r
#define G                                   lightbulb_group->g
#define B                                   lightbulb_group->b
#define CW                                  lightbulb_group->cw
#define WW                                  lightbulb_group->ww
#define WP                                  lightbulb_group->wp
#define myCMY                               lightbulb_group->cmy
#define myRGB                               lightbulb_group->rgb
#define LIGHTBULB_AUTODIMMER_TIMER          ch_group->timer
#define LIGHTBULB_SET_DELAY_TIMER           lightbulb_group->timer

#define GD_CURRENT_DOOR_STATE               ch_group->ch[0]
#define GD_TARGET_DOOR_STATE                ch_group->ch[1]
#define GD_OBSTRUCTION_DETECTED             ch_group->ch[2]
#define GD_CURRENT_DOOR_STATE_INT           ch_group->ch[0]->value.int_value
#define GD_TARGET_DOOR_STATE_INT            ch_group->ch[1]->value.int_value
#define GD_OBSTRUCTION_DETECTED_BOOL        ch_group->ch[2]->value.bool_value
#define GARAGE_DOOR_OPENED                  (0)
#define GARAGE_DOOR_CLOSED                  (1)
#define GARAGE_DOOR_OPENING                 (2)
#define GARAGE_DOOR_CLOSING                 (3)
#define GARAGE_DOOR_STOPPED                 (4)
#define GD_FROM_STOPPED_ACTION_OFFSET       (10)
#define GARAGE_DOOR_TIME_OPEN_SET           "d"
#define GARAGE_DOOR_TIME_OPEN_DEFAULT       (30)
#define GARAGE_DOOR_TIME_CLOSE_SET          "c"
#define GARAGE_DOOR_TIME_MARGIN_SET         "e"
#define GARAGE_DOOR_TIME_MARGIN_DEFAULT     (0)
#define GARAGE_DOOR_CURRENT_TIME            ch_group->num_f[0]
#define GARAGE_DOOR_WORKING_TIME            ch_group->num_f[1]
#define GARAGE_DOOR_CLOSE_TIME_FACTOR       ch_group->num_f[2]
#define GARAGE_DOOR_TIME_MARGIN             ch_group->num_i[0]
#define GARAGE_DOOR_HAS_F2                  ch_group->num_i[1]
#define GARAGE_DOOR_HAS_F3                  ch_group->num_i[2]
#define GARAGE_DOOR_HAS_F4                  ch_group->num_i[3]
#define GARAGE_DOOR_HAS_F5                  ch_group->num_i[4]
#define GARAGE_DOOR_VIRTUAL_STOP            ch_group->num_i[5]
#define GARAGE_DOOR_WORKER_TIMER            ch_group->timer

#define VIRTUAL_STOP_SET                    "vs"
#define VIRTUAL_STOP_DEFAULT                (0)

#define WINDOW_COVER_CLOSING                (0)
#define WINDOW_COVER_OPENING                (1)
#define WINDOW_COVER_STOP                   (2)
#define WINDOW_COVER_CLOSING_FROM_MOVING    (3)
#define WINDOW_COVER_OPENING_FROM_MOVING    (4)
#define WINDOW_COVER_STOP_FROM_CLOSING      (5)
#define WINDOW_COVER_STOP_FROM_OPENING      (6)
#define WINDOW_COVER_OBSTRUCTION            (7)
#define WINDOW_COVER_SINGLE_INPUT           (100)
#define WINDOW_COVER_TYPE_SET               "w"
#define WINDOW_COVER_TYPE_DEFAULT           (0)
#define WINDOW_COVER_TIME_OPEN_SET          "o"
#define WINDOW_COVER_TIME_CLOSE_SET         "c"
#define WINDOW_COVER_TIME_DEFAULT           (15)
#define WINDOW_COVER_CORRECTION_SET         "f"
#define WINDOW_COVER_CORRECTION_DEFAULT     (0)
#define WINDOW_COVER_TIMER_WORKER_PERIOD_MS (200)
#define WINDOW_COVER_MARGIN_SYNC_SET        "m"
#define WINDOW_COVER_MARGIN_SYNC_DEFAULT    (5)
#define WINDOW_COVER_TIME_OPEN_STEP         ch_group->num_f[0]
#define WINDOW_COVER_TIME_CLOSE_STEP        ch_group->num_f[1]
#define WINDOW_COVER_MOTOR_POSITION         ch_group->num_f[2]
#define WINDOW_COVER_HOMEKIT_POSITION       ch_group->num_f[3]
#define WINDOW_COVER_MARGIN_SYNC            ch_group->num_i[0]
#define WINDOW_COVER_CORRECTION             ch_group->num_i[1]
#define WINDOW_COVER_STOP_ENABLE            ch_group->num_i[2]
#define WINDOW_COVER_STOP_ENABLE_DELAY_MS   (80)
#define WINDOW_COVER_VIRTUAL_STOP           ch_group->num_i[3]
#define WINDOW_COVER_SEND_CUR_POS_COUNTER   ch_group->num_i[4]
#define WINDOW_COVER_MUST_STOP              ch_group->num_i[5]
#define WINDOW_COVER_SEND_CUR_POS_MAX       (2200 / WINDOW_COVER_TIMER_WORKER_PERIOD_MS)    // Send every 2200ms
#define WINDOW_COVER_CH_CURRENT_POSITION    ch_group->ch[0]
#define WINDOW_COVER_CH_TARGET_POSITION     ch_group->ch[1]
#define WINDOW_COVER_CH_STATE               ch_group->ch[2]
#define WINDOW_COVER_CH_OBSTRUCTION         ch_group->ch[3]

#define TV_INPUTS_ARRAY                     "i"
#define TV_INPUT_NAME                       "n"

#define FAN_SPEED_STEPS                     "e"
#define FAN_SET_DELAY_MS                    (200)
#define FAN_SET_DELAY_TIMER                 ch_group->timer
#define FAN_CURRENT_ACTION                  ch_group->num_i[0]

#define BATTERY_LOW_THRESHOLD_SET           "l"
#define BATTERY_LOW_THRESHOLD_DEFAULT       (20)
#define BATTERY_LOW_THRESHOLD               ch_group->num_i[0]
#define BATTERY_LEVEL_CH                    ch_group->ch[0]
#define BATTERY_LEVEL_CH_INT                ch_group->ch[0]->value.int_value
#define BATTERY_STATUS_LOW_CH               ch_group->ch[1]
#define BATTERY_STATUS_LOW_CH_INT           ch_group->ch[1]->value.int_value
#define BATTERY_CHARGING_STATE              ch_group->ch[2]

#define PM_SENSOR_TYPE_SET                  "n"
#define PM_SENSOR_TYPE_DEFAULT              (0)
#define PM_SENSOR_TYPE                      ch_group->num_i[0]
#define PM_LAST_SAVED_CONSUPTION            ch_group->num_f[7]
#define PM_SENSOR_DATA_ARRAY_SET            "dt"
#define PM_SENSOR_DATA_DEFAULT              (-1)
#define PM_SENSOR_DATA_DEFAULT_INT_TYPE     (2) // GPIO_INTTYPE_EDGE_NEG
#define PM_SENSOR_HLW_GPIO                  ch_group->num_i[1]
#define PM_SENSOR_HLW_GPIO_CF               ch_group->num_i[2]
#define PM_SENSOR_ADE_BUS                   ch_group->num_i[1]
#define PM_SENSOR_ADE_ADDR                  ch_group->num_i[2]
#define PM_POLL_PERIOD_DEFAULT              (5.f)
#define PM_POLL_PERIOD                      ch_group->num_f[6]
#define PM_VOLTAGE_FACTOR_SET               "vf"
#define PM_VOLTAGE_FACTOR_DEFAULT           (1)
#define PM_VOLTAGE_FACTOR                   ch_group->num_f[0]
#define PM_VOLTAGE_OFFSET_SET               "vo"
#define PM_VOLTAGE_OFFSET_DEFAULT           (0)
#define PM_VOLTAGE_OFFSET                   ch_group->num_f[1]
#define PM_CURRENT_FACTOR_SET               "cf"
#define PM_CURRENT_FACTOR_DEFAULT           (1)
#define PM_CURRENT_FACTOR                   ch_group->num_f[2]
#define PM_CURRENT_OFFSET_SET               "co"
#define PM_CURRENT_OFFSET_DEFAULT           (0)
#define PM_CURRENT_OFFSET                   ch_group->num_f[3]
#define PM_POWER_FACTOR_SET                 "pf"
#define PM_POWER_FACTOR_DEFAULT             (1)
#define PM_POWER_FACTOR                     ch_group->num_f[4]
#define PM_POWER_OFFSET_SET                 "po"
#define PM_POWER_OFFSET_DEFAULT             (0)
#define PM_POWER_OFFSET                     ch_group->num_f[5]

#define FM_SENSOR_TYPE_SET                  "n"
#define FM_SENSOR_TYPE_FREE                 (1)
#define FM_SENSOR_TYPE_PULSE_FREQ           (2)
#define FM_SENSOR_TYPE_PULSE_US_TIME        (3)
#define FM_SENSOR_TYPE_PWM_DUTY             (4)
#define FM_SENSOR_TYPE_MATHS                (5)
#define FM_SENSOR_TYPE_ADC                  (10)
#define FM_SENSOR_TYPE_ADC_INV              (11)
#define FM_SENSOR_TYPE_NETWORK              (15)
#define FM_SENSOR_TYPE_NETWORK_PATTERN_TEXT (16)
#define FM_SENSOR_TYPE_NETWORK_PATTERN_HEX  (17)
#define FM_SENSOR_TYPE_I2C                  (20)
#define FM_SENSOR_TYPE_I2C_TRIGGER          (21)
#define FM_SENSOR_TYPE_UART                 (23)
#define FM_SENSOR_TYPE_UART_PATTERN_HEX     (24)
#define FM_SENSOR_TYPE_UART_PATTERN_TEXT    (25)
#define FM_SENSOR_TYPE_DEFAULT              FM_SENSOR_TYPE_FREE
#define FM_SENSOR_TYPE                      ch_group->num_i[0]
#define FM_SENSOR_TYPE_B                    ch_group_b->num_i[0]
#define FM_SENSOR_GPIO_ARRAY_SET            "g"
#define FM_SENSOR_GPIO                      ch_group->num_i[1]
#define FM_SENSOR_GPIO_INT_TYPE             ch_group->num_i[2]
#define FM_SENSOR_GPIO_TRIGGER              ch_group->num_i[3]
#define FM_NEW_VALUE                        ch_group->num_f[2]
#define FM_OVERRIDE_VALUE                   ch_group->num_f[3]
#define FM_SENSOR_PWM_DUTY_STATUS           ch_group->num_i[2]
#define FM_SENSOR_PWM_DUTY_STATUS_START     (0)
#define FM_SENSOR_PWM_DUTY_STATUS_HIGH      (1)
#define FM_SENSOR_PWM_DUTY_STATUS_LOW       (2)
#define FM_SENSOR_PWM_DUTY_STATUS_END       (3)
#define FM_SENSOR_PWM_DUTY_TIME_HIGH        ch_group->num_f[2]
#define FM_SENSOR_PWM_DUTY_TIME_LOW         ch_group->num_f[3]
#define FM_FACTOR_SET                       "ff"
#define FM_FACTOR_DEFAULT                   (1)
#define FM_FACTOR                           ch_group->num_f[0]
#define FM_OFFSET_SET                       "fo"
#define FM_OFFSET_DEFAULT                   (0)
#define FM_OFFSET                           ch_group->num_f[1]
#define FM_LIMIT_ARRAY_SET                  "l"
#define FM_MATHS_OPERATION_NONE             (0)
#define FM_MATHS_OPERATION_ADD              (1)
#define FM_MATHS_OPERATION_SUB              (2)
#define FM_MATHS_OPERATION_SUB_INV          (3)
#define FM_MATHS_OPERATION_MUL              (4)
#define FM_MATHS_OPERATION_DIV              (5)
#define FM_MATHS_OPERATION_DIV_INV          (6)
#define FM_MATHS_OPERATION_MOD              (7)
#define FM_MATHS_OPERATION_MOD_INV          (8)
#define FM_MATHS_OPERATION_POW              (9)
#define FM_MATHS_OPERATION_POW_INV          (10)
#define FM_MATHS_OPERATION_INV              (11)
#define FM_MATHS_OPERATION_ABS              (12)
#define FM_MATHS_GET_TIME_HOUR              (-1)
#define FM_MATHS_GET_TIME_MINUTE            (-2)
#define FM_MATHS_GET_TIME_SECOND            (-3)
#define FM_MATHS_GET_TIME_DAYWEEK           (-4)
#define FM_MATHS_GET_TIME_DAYMONTH          (-5)
#define FM_MATHS_GET_TIME_MONTH             (-6)
#define FM_MATHS_GET_TIME_DAYYEAR           (-7)
#define FM_MATHS_GET_TIME_YEAR              (-8)
#define FM_MATHS_GET_TIME_IS_SAVING         (-9)
#define FM_MATHS_GET_TIME_UNIX              (-10)
#define FM_MATHS_GEN_RANDOM_NUMBER          (-11)
#define FM_MATHS_GET_UPTIME                 (-12)
#define FM_MATHS_GET_WIFI_RSSI              (-13)
#define FM_MATHS_GET_HK_CLIENT_IPADDR       (-14)
#define FM_MATHS_GET_HK_CLIENT_COUNT        (-15)
#define FM_MATHS_OPERATIONS                 ch_group->num_i[1]
#define FM_MATHS_FIRST_OPERATION            (2)
#define FM_MATHS_INT                        ch_group->num_i
#define FM_MATHS_FLOAT_FIRST                (4)
#define FM_MATHS_FLOAT                      ch_group->num_f
#define FM_VAL_LEN                          ch_group->num_i[1]
#define FM_VAL_TYPE                         ch_group->num_i[2]
#define FM_BUFFER_LEN_ARRAY_SET             "bl"
#define FM_BUFFER_LEN_MIN                   ch_group->num_i[3]
#define FM_BUFFER_LEN_MAX                   ch_group->num_i[4]
#define FM_UART_PORT_SET                    "u"
#define FM_UART_PORT                        ch_group->num_i[5]
#define FM_TARGET_CH_ARRAY_SET              "tg"
#define FM_POLL_PERIOD_DEFAULT              (30.f)
#define FM_PATTERN_ARRAY_SET                "pt"
#define FM_PATTERN_CH_WRITE                 ch_group->ch[1]
#define FM_PATTERN_CH_READ                  (pattern_t*) FM_PATTERN_CH_WRITE
#define FM_I2C_DEVICE_DATA_ARRAY_SET        "ic"
#define FM_I2C_START_COMMANDS_ARRAY_SET     "in"
#define FM_I2C_TRIGGER_COMMAND_ARRAY_SET    "it"
#define FM_READ_COMMAND_DATA_ARRAY          "dt"
#define FM_I2C_VAL_OFFSET_SET               "io"
#define FM_I2C_VAL_OFFSET_DEFAULT           (0)
#define FM_I2C_BUS                          ch_group->num_i[3]
#define FM_I2C_BUS_B                        ch_group_b->num_i[3]
#define FM_I2C_ADDR                         ch_group->num_i[4]
#define FM_I2C_ADDR_B                       ch_group_b->num_i[4]
#define FM_I2C_REG_LEN                      ch_group->num_i[5]
#define FM_I2C_VAL_OFFSET                   ch_group->num_i[6]
#define FM_I2C_REG_FIRST                    (7)
#define FM_I2C_REG                          ch_group->num_i     // from num_i[FM_I2C_REG_FIRST] to num_i[FM_I2C_REG_FIRST + 3]
#define FM_I2C_TRIGGER_REG_LEN              ch_group->num_i[11]
#define FM_I2C_TRIGGER_VAL_LEN              ch_group->num_i[12]
#define FM_I2C_TRIGGER_REG_FIRST            (13)
#define FM_I2C_TRIGGER_REG                  ch_group->num_i     // from num_i[FM_I2C_TRIGGER_REG_FIRST] to num_i[FM_I2C_TRIGGER_REG_FIRST + 3]
#define FM_I2C_TRIGGER_VAL_FIRST            (17)
#define FM_I2C_TRIGGER_VAL                  ch_group->num_i     // from num_i[FM_I2C_TRIGGER_VAL_FIRST] to num_i[FM_I2C_TRIGGER_VAL_FIRST + 3]
#define FM_ACCUMULATVE_VALUE_RESET          (-2182017)

#define RECV_UART_POLL_PERIOD_MS            (50)

#define LIGHT_SENSOR_TYPE_SET               "n"
#define LIGHT_SENSOR_TYPE_DEFAULT           (0)
#define LIGHT_SENSOR_TYPE                   ch_group->num_i[0]
#define LIGHT_SENSOR_GPIO_SET               "g"
#define LIGHT_SENSOR_GPIO                   ch_group->num_i[1]
#define LIGHT_SENSOR_POLL_PERIOD_DEFAULT    (3.f)
#define LIGHT_SENSOR_FACTOR_SET             "f"
#define LIGHT_SENSOR_FACTOR_DEFAULT         (1)
#define LIGHT_SENSOR_FACTOR                 ch_group->num_f[0]
#define LIGHT_SENSOR_OFFSET_SET             "o"
#define LIGHT_SENSOR_OFFSET_DEFAULT         (0)
#define LIGHT_SENSOR_OFFSET                 ch_group->num_f[1]
#define LIGHT_SENSOR_RESISTOR_SET           "re"
#define LIGHT_SENSOR_RESISTOR_DEFAULT       (10000)
#define LIGHT_SENSOR_RESISTOR               ch_group->num_f[2]
#define LIGHT_SENSOR_POW_SET                "po"
#define LIGHT_SENSOR_POW_DEFAULT            (1)
#define LIGHT_SENSOR_POW                    ch_group->num_f[3]
#define LIGHT_SENSOR_I2C_DATA_ARRAY_SET     "dt"
#define LIGHT_SENSOR_I2C_BUS                ch_group->num_i[1]
#define LIGHT_SENSOR_I2C_ADDR               ch_group->num_i[2]

#define SEC_SYSTEM_MODES_ARRAY_SET          "n"
#define SEC_SYSTEM_CH_CURRENT_STATE         ch_group->ch[0]
#define SEC_SYSTEM_CH_TARGET_STATE          ch_group->ch[1]
#define SEC_SYSTEM_AT_HOME                  (0)
#define SEC_SYSTEM_OFF                      (3)
#define SEC_SYSTEM_ALARM                    (4)
#define SEC_SYSTEM_REC_ALARM_TIMER          ch_group->timer
#define SEC_SYSTEM_REC_ALARM_PERIOD_MS      (6000)

#define AQ_EXTRA_DATA_ARRAY_SET             "dt"
#define AQ_OZONE_DENSITY                    (1)
#define AQ_NITROGEN_DIOXIDE_DENSITY         (2)
#define AQ_SULPHUR_DIOXIDE_DENSITY          (3)
#define AQ_PM25_DENSITY                     (4)
#define AQ_PM10_DENSITY                     (5)
#define AQ_VOC_DENSITY                      (6)

#define HIST_DATA_ARRAY_SET                 "h"
#define HIST_READ_ON_CLOCK_READY_SET        "x"
#define HIST_LAST_REGISTER                  ch_group->num_f[0]
#define HIST_TIME_SIZE                      (4)     // (sizeof(uint32_t))
#define HIST_DATA_SIZE                      (4)     // (sizeof(int32_t))
#define HIST_REGISTER_SIZE                  (HIST_TIME_SIZE + HIST_DATA_SIZE)
#define HIST_REGISTERS_BY_BLOCK             (100)
#define HIST_BLOCK_SIZE                     (HIST_REGISTERS_BY_BLOCK * HIST_REGISTER_SIZE)

#define AUTOOFF_TIMER                       ch_group->timer2

#define MAX_ACTIONS                         (51)    // from 0 to (MAX_ACTIONS - 1)
#define MAX_WILDCARD_ACTIONS                (4)     // from 0 to (MAX_WILDCARD_ACTIONS - 1)
#define WILDCARD_ACTIONS_ARRAY_HEADER       "y"
#define NO_LAST_WILDCARD_ACTION             (-1000000.f)
#define WILDCARD_ACTIONS                    "0"
#define WILDCARD_ACTION_REPEAT              "r"
#define COPY_ACTIONS                        "a"
#define BINARY_OUTPUTS_ARRAY                "r"
#define INITIAL_VALUE                       "n"
#define SYSTEM_ACTIONS_ARRAY                "s"
#define NETWORK_ACTIONS_ARRAY               "h"
#define SERVICE_MANAGER_ACTIONS_ARRAY       "m"
#define SET_CH_ACTIONS_ARRAY                "c"
#define IRRF_ACTIONS_ARRAY                  "i"
#define IRRF_ACTION_PROTOCOL                "p"
#define IRRF_ACTION_PROTOCOL_LEN_2BITS      (14)
#define IRRF_ACTION_PROTOCOL_LEN_4BITS      (22)
#define IRRF_ACTION_PROTOCOL_LEN_6BITS      (30)
#define IRRF_CODE_HEADER_MARK_POS           (0)
#define IRRF_CODE_HEADER_SPACE_POS          (1)
#define IRRF_CODE_BIT0_MARK_POS             (2)
#define IRRF_CODE_BIT0_SPACE_POS            (3)
#define IRRF_CODE_BIT1_MARK_POS             (4)
#define IRRF_CODE_BIT1_SPACE_POS            (5)
#define IRRF_CODE_BIT2_MARK_POS             (6)
#define IRRF_CODE_BIT2_SPACE_POS            (7)
#define IRRF_CODE_BIT3_MARK_POS             (8)
#define IRRF_CODE_BIT3_SPACE_POS            (9)
#define IRRF_CODE_BIT4_MARK_POS             (10)
#define IRRF_CODE_BIT4_SPACE_POS            (11)
#define IRRF_CODE_BIT5_MARK_POS             (12)
#define IRRF_CODE_BIT5_SPACE_POS            (13)
#define IRRF_CODE_FOOTER_MARK_POS_2BITS     (6)
#define IRRF_CODE_FOOTER_MARK_POS_4BITS     (10)
#define IRRF_CODE_FOOTER_MARK_POS_6BITS     (14)
#define IRRF_ACTION_PROTOCOL_CODE           "c"
#define IRRF_ACTION_RAW_CODE                "w"
#define IR_ACTION_TX_GPIO                   "t"
#define IR_ACTION_TX_GPIO_INVERTED          "j"
#define RF_ACTION_TX_GPIO                   "g"
#define RF_ACTION_TX_GPIO_INVERTED          "k"
#define IRRF_ACTION_FREQ                    "x"
#define IRRF_ACTION_REPEATS                 "r"
#define IRRF_ACTION_REPEATS_PAUSE           "d"
#define SYSTEM_ACTION                       "a"
#define NETWORK_ACTION_HOST                 "h"
#define NETWORK_ACTION_PORT                 "p"
#define NETWORK_ACTION_URL                  "u"
#define NETWORK_ACTION_METHOD               "m"
#define NETWORK_ACTION_HEADER               "e"
#define NETWORK_ACTION_CONTENT              "c"
#define NETWORK_ACTION_WAIT_RESPONSE_SET    "w"
#define NETWORK_ACTION_WILDCARD_VALUE       "#HAA@"
#define SYSTEM_ACTION_REBOOT                (0)
#define SYSTEM_ACTION_SETUP_MODE            (1)
#define SYSTEM_ACTION_OTA_UPDATE            (2)
#define SYSTEM_ACTION_WIFI_RECONNECTION     (3)
#define SYSTEM_ACTION_WIFI_RECONNECTION_2   (4)
#define SYSTEM_ACTION_WIFI_DISCONNECTION    (5)
#define UART_ACTIONS_ARRAY                  "u"
#define UART_ACTION_UART                    "n"
#define UART_ACTION_PAUSE                   "d"
#define PWM_ACTIONS_ARRAY                   "q"

#define I2C_CONFIG_ARRAY                    "ic"

#define MCP23017_ARRAY                      "mc"

#define SERVICE_TYPE_SET                    "t"
#define SERV_TYPE_ROOT_DEVICE               (0)
#define SERV_TYPE_SWITCH                    (1)
#define SERV_TYPE_OUTLET                    (2)
#define SERV_TYPE_BUTTON                    (3)
#define SERV_TYPE_LOCK                      (4)
#define SERV_TYPE_CONTACT_SENSOR            (5)
#define SERV_TYPE_OCCUPANCY_SENSOR          (6)
#define SERV_TYPE_LEAK_SENSOR               (7)
#define SERV_TYPE_SMOKE_SENSOR              (8)
#define SERV_TYPE_CARBON_MONOXIDE_SENSOR    (9)
#define SERV_TYPE_CARBON_DIOXIDE_SENSOR     (10)
#define SERV_TYPE_FILTER_CHANGE_SENSOR      (11)
#define SERV_TYPE_MOTION_SENSOR             (12)
#define SERV_TYPE_DOORBELL                  (13)
#define SERV_TYPE_AIR_QUALITY               (15)
#define SERV_TYPE_WATER_VALVE               (20)
#define SERV_TYPE_THERMOSTAT                (21)
#define SERV_TYPE_TEMP_SENSOR               (22)
#define SERV_TYPE_HUM_SENSOR                (23)
#define SERV_TYPE_TH_SENSOR                 (24)
#define SERV_TYPE_THERMOSTAT_WITH_HUM       (25)
#define SERV_TYPE_HUMIDIFIER                (26)
#define SERV_TYPE_HUMIDIFIER_WITH_TEMP      (27)
#define SERV_TYPE_LIGHTBULB                 (30)
#define SERV_TYPE_GARAGE_DOOR               (40)
#define SERV_TYPE_WINDOW_COVER              (45)
#define SERV_TYPE_LIGHT_SENSOR              (50)
#define SERV_TYPE_SECURITY_SYSTEM           (55)
#define SERV_TYPE_TV                        (60)
#define SERV_TYPE_FAN                       (65)
#define SERV_TYPE_BATTERY                   (70)
#define SERV_TYPE_POWER_MONITOR             (75)
#define SERV_TYPE_FREE_MONITOR              (80)
#define SERV_TYPE_FREE_MONITOR_ACCUMULATVE  (81)
#define SERV_TYPE_DATA_HISTORY              (95)
#define SERV_TYPE_IAIRZONING                (99)

#define SERIAL_STRING                       "sn"
#define SERIAL_STRING_LEN                   (11)

#define HOMEKIT_DEVICE_CATEGORY_SET         "ct"
#define HOMEKIT_DEVICE_CATEGORY_DEFAULT     (1)

#define BOOT_EARLY_DELAY                    "v"
#define NEXT_SERV_CREATION_DELAY            "cd"
#define EXIT_EMERGENCY_SETUP_MODE_TIME      (3000)
#define SETUP_MODE_ACTIVATE_COUNT           "z"
#define SETUP_MODE_DEFAULT_ACTIVATE_COUNT   (8)
#define SETUP_MODE_TOGGLE_TIME_MS           (1050)
#define CUSTOM_HAA_COMMAND                  "pt"
#define CUSTOM_HAA_ADVANCED_COMMAND_LEN     (2)
#define HAA_SETUP_ACCESSORY_SET             "s"

#define IRRF_CAPTURE_BUFFER_SIZE            (2048)

#define ACTION_TASK_TYPE_UART               (0)
#define ACTION_TASK_TYPE_NETWORK            (1)
#define ACTION_TASK_TYPE_IRRF               (2)
#define ACTION_TASK_MAX_ERRORS              (10)

#define SAVE_STATES_TIMER                   ch_group_find_by_serv(SERV_TYPE_ROOT_DEVICE)->timer
#define WIFI_WATCHDOG_TIMER                 ch_group_find_by_serv(SERV_TYPE_ROOT_DEVICE)->timer2
#define WIFI_STATUS_LONG_DISCONNECTED       (0)
#define WIFI_STATUS_DISCONNECTED            (1)
#define WIFI_STATUS_CONNECTING              (2)
#define WIFI_STATUS_CONNECTED               (3)
#define WIFI_PING_ERRORS                    "w"
#define WIFI_SLEEP_MODE_SET                 "d"
#define WIFI_BANDWIDTH_40_SET               "dt"

#define WIFI_RECONNECTION_POLL_PERIOD_MS    (1500)
#define WIFI_DISCONNECTED_LONG_TIME         (170)    // * WIFI_RECONNECTION_POLL_PERIOD_MS

#define WIFI_WATCHDOG_POLL_PERIOD_MS        (1500)
#define WIFI_WATCHDOG_ARP_PERIOD_SET        "e"
#define WIFI_WATCHDOG_ARP_PERIOD_DEFAULT    (190)   // * WIFI_WATCHDOG_POLL_PERIOD_MS
#define WIFI_WATCHDOG_ROAMING_PERIOD        (1234)  // * WIFI_WATCHDOG_POLL_PERIOD_MS

#define STATUS_LED_DURATION_ON              (30)
#define STATUS_LED_DURATION_OFF             (120)

#define SAVE_STATES_DELAY_MS                (3000)
#define RANDOM_DELAY_MS                     (3500)

#define HOMEKIT_RE_PAIR_TIME_MS             (300000)

#define ACCESSORIES_WITHOUT_BRIDGE          (4)     // Max number of accessories without dedicated HomeKit bridge

#define NTP_SERVER_FALLBACK                 "pool.ntp.org"

#define WOL_PACKET_LEN                      (102)

#define SYSTEM_UPTIME_MS                    ((float) sdk_system_get_time_raw() * 1e-3)

#define HAA_MIN(x, y)                       (((x) < (y)) ? (x) : (y))
#define HAA_MAX(x, y)                       (((x) > (y)) ? (x) : (y))

#define HAA_ADC_RESOLUTION_ESP32            (4096)
#define HAA_ADC_RESOLUTION_ESP8266          (1024)

#define HAA_ADC_FACTOR                      (HAA_ADC_RESOLUTION_ESP32 / HAA_ADC_RESOLUTION_ESP8266)


#ifdef ESP_PLATFORM

#if defined(CONFIG_IDF_TARGET_ESP32) \
    || defined(CONFIG_IDF_TARGET_ESP32S2)
#define HAA_RMT_LED_STRIP_BLOCK_SYMBOLS     (64)
#else
#define HAA_RMT_LED_STRIP_BLOCK_SYMBOLS     (48)
#endif

#define HAA_RMT_LED_STRIP_RESOLUTION_HZ     (10000000)
#define HAA_RMT_LED_STRIP_QUEUE_DEPTH       (2)

#define HAA_ADC_MAX_VALUE                   ((HAA_ADC_RESOLUTION_ESP32 - 1) / HAA_ADC_FACTOR)

#else   // ESP_PLATFORM

#define HAA_ADC_MAX_VALUE                   (HAA_ADC_RESOLUTION_ESP8266 - 1)

#endif  // ESP_PLATFORM


#define KELVIN_TO_CELSIUS(x)                ((x) - 273.15)

#define NTP_POLL_PERIOD_MS                  (3600 * 1000)   // 1 hour

#ifdef HAA_DEBUG
#define LIGHT_DEBUG
#endif

#ifdef LIGHT_DEBUG
#define L_DEBUG(message, ...)               INFO(message, ##__VA_ARGS__)
#else
#define L_DEBUG(message, ...)
#endif

#endif // __HAA_HEADER_H__
