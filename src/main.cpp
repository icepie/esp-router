
// NAPT example released to public domain

#include <ESP8266WiFi.h>
#include <lwip/napt.h>
#include <lwip/dns.h>
#include <LwipDhcpServer.h>

// config
#include "config.h"

void setSTA(const char *ssid, const char *passphrase) {

  WiFi.begin(ssid, passphrase);

  if (WiFi.status() == WL_CONNECTED) {
    // give DNS servers to AP side
    dhcpSoftAP.dhcps_set_dns(0, WiFi.dnsIP(0));
    dhcpSoftAP.dhcps_set_dns(1, WiFi.dnsIP(1));
  }

}

void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nNAPT Range extender\n");
  //Serial.printf("Heap on start: %d\n", ESP.getFreeHeap());

  // first, connect to STA so we can get a proper local DNS server
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  // while (WiFi.status() != WL_CONNECTED) {
  //   Serial.print('.');
  //   delay(500);
  // }

  Serial.printf("\nSTA: %s (dns: %s / %s)\n",
                WiFi.localIP().toString().c_str(),
                WiFi.dnsIP(0).toString().c_str(),
                WiFi.dnsIP(1).toString().c_str());

  if (WiFi.status() == WL_CONNECTED) {
    // give DNS servers to AP side
    dhcpSoftAP.dhcps_set_dns(0, WiFi.dnsIP(0));
    dhcpSoftAP.dhcps_set_dns(1, WiFi.dnsIP(1));
  }

  WiFi.softAPConfig(  // enable AP, with android-compatible google domain
    IPAddress(192, 168, 9, 1),
    IPAddress(192, 168, 9, 1),
    IPAddress(255, 255, 255, 0));

  WiFi.softAP(APSSID "extender", APPSK);
  Serial.printf("AP: %s\n", WiFi.softAPIP().toString().c_str());

  //Serial.printf("Heap before: %d\n", ESP.getFreeHeap());
  err_t ret = ip_napt_init(NAPT, NAPT_PORT);
  //Serial.printf("ip_napt_init(%d,%d): ret=%d (OK=%d)\n", NAPT, NAPT_PORT, (int)ret, (int)ERR_OK);
  if (ret == ERR_OK) {
    ret = ip_napt_enable_no(SOFTAP_IF, 1);
    //Serial.printf("ip_napt_enable_no(SOFTAP_IF): ret=%d (OK=%d)\n", (int)ret, (int)ERR_OK);
    if (ret == ERR_OK) {
      Serial.printf("WiFi router is running!");
    }
  }
  //Serial.printf("Heap after napt init: %d\n", ESP.getFreeHeap());
  if (ret != ERR_OK) {
    Serial.printf("NAPT initialization failed\n");
  }
}

void loop() {
}