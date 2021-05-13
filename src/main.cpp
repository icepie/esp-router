
// NAPT example released to public domain
#include <ESP8266WiFi.h>
#include <lwip/napt.h>
#include <lwip/dns.h>
#include <LwipDhcpServer.h>
#include <TaskScheduler.h>
#include <ESPAsyncDNSServer.h>
#include "ESPAsyncWebServer.h"

// config
#include "config.h"

AsyncDNSServer dnsServer;
AsyncWebServer server(80);

Scheduler runner;

void dnsProcess(){
  //dnsServer.processNextRequest();
}

Task dnsTask(0, TASK_FOREVER, &dnsProcess, &runner,true);

void dhcpReset()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    // give DNS servers to AP side
    dhcpSoftAP.dhcps_set_dns(0, WiFi.dnsIP(0));
    dhcpSoftAP.dhcps_set_dns(1, WiFi.dnsIP(1));
  }
}

void setSTA(const char *ssid, const char *passphrase)
{

  WiFi.begin(ssid, passphrase);
  dhcpReset();
}

void ipNaptEnable()
{
  err_t ret = ip_napt_init(NAPT, NAPT_PORT);
  //Serial.printf("ip_napt_init(%d,%d): ret=%d (OK=%d)\n", NAPT, NAPT_PORT, (int)ret, (int)ERR_OK);
  if (ret == ERR_OK)
  {
    ret = ip_napt_enable_no(SOFTAP_IF, 1);
    //Serial.printf("ip_napt_enable_no(SOFTAP_IF): ret=%d (OK=%d)\n", (int)ret, (int)ERR_OK);
    if (ret == ERR_OK)
    {
      Serial.printf("WiFi router is running!");
    }
  }
  //Serial.printf("Heap after napt init: %d\n", ESP.getFreeHeap());
  if (ret != ERR_OK)
  {
    Serial.printf("NAPT initialization failed\n");
  }
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void setup()
{
  Serial.begin(115200);

  // first, connect to STA so we can get a proper local DNS server
  WiFi.mode(WIFI_STA);

  setSTA(STASSID, STAPSK);
  // Serial.printf("\nSTA: %s (dns: %s / %s)\n",
  //               WiFi.localIP().toString().c_str(),
  //               WiFi.dnsIP(0).toString().c_str(),
  //               WiFi.dnsIP(1).toString().c_str());

  WiFi.softAPConfig( // enable AP, with android-compatible google domain
      AP_IP,
      GATEWAY_IP,
      AP_SUBNET);

  WiFi.softAP(APSSID "extender", APPSK);

  ipNaptEnable();

  // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
  dnsServer.setTTL(300);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is AsyncDNSReplyCode::NonExistentDomain
  dnsServer.setErrorReplyCode(AsyncDNSReplyCode::ServerFailure);

  // start DNS server for a specific domain name
  dnsServer.start(DNS_PORT, DOMAIN_NAME, AP_IP);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "Hello, world"); });

  server.onNotFound(notFound);

  server.begin();

  // runner setting
  //runner.init();

  // Serial.printf("AP: %s\n", WiFi.softAPIP().toString().c_str());
}

void loop()
{
  runner.execute();
}