
#define DOMAIN_NAME "u.io"
#define DNS_PORT 53
#define MONITOR_SPEED 115200

// sta conf
#ifndef STASSID
#define STASSID "singzer"
#define STAPSK  "1008610086"
#endif

// ap conf
#define AP_IP IPAddress(192, 168, 9, 1)
#define GATEWAY_IP IPAddress(192, 168, 9, 1)
#define AP_SUBNET IPAddress(255, 255, 255, 0)

#ifndef APSSID
#define APSSID "icepie-iot-"
#define APPSK  ""
#endif

// nat conf
#define NAPT 1000
#define NAPT_PORT 10
