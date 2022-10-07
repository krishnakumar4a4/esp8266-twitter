#include <Arduino.h>
#include "WiFiManager.h"
#include "updater.h"
#include "webServer.h"
#include "configManager.h"
#include <NTPClient.h>
#include <TwitterWebAPI.h>
#include <ctime>
#include "Config.h"

const char *ntp_server = "pool.ntp.org";
int timezone = 5; // IST to GMT difference

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_server, (timezone * 3600) + 1800, 60000); // NTP server pool, offset (in seconds), update interval (in milliseconds)
TwitterClient tcr(timeClient, CONSUMER_KEY, CONSUMER_SECRET, ACCESS_TOKEN, ACCESS_TOKEN_SECRET);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  GUI.begin();
  configManager.begin();
  WiFiManager.begin("esp8266-twitter-tester");
  
  tcr.startNTP();
  Serial.println("started");
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiManager.loop();
  updater.loop();
  configManager.loop();

  Serial.println("called searchUser V2");
  String userdetails = tcr.searchUser("krishnakumart36");
  Serial.println(userdetails);

  Serial.println("called searchTwitter V2");
  String tweetDetails = tcr.searchTwitter("power-on");
  Serial.println(tweetDetails);

  Serial.println("called tweet V2");
  bool tweetresponse = tcr.tweet("Test tweet");
  Serial.println(tweetresponse);

  Serial.println("called searchTwitter V2");
  TwitterClient::Response tweetSearch = tcr.getV2("/tweets/search/recent?query=power-on");
  Serial.println(tweetSearch.respBodyString.c_str());

  Serial.println("called tweet V2");
  TwitterClient::Response tweetResp = tcr.postV2("/tweets", "{\"text\": \"Hello There!!\"}");
  Serial.println(tweetResp.respBodyString.c_str());

  delay(5000);
}