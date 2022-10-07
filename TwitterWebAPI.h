#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <stdint.h>
#include <NTPClient.h>
#include <time.h>
#include <vector>
#include <string>
#include <algorithm>
#include "oauth.h"

#ifndef TWI_TIMEOUT
#define TWI_TIMEOUT 1500 // in msec
#endif

class TwitterClient {
private:
  NTPClient *timeClient;
  struct Data
  {
    oauth::Keys keys;
  } data;
  oauth::Keys const &keys() const
  {
    return data.keys;
  }
  struct RequestOption
  {
    enum Method
    {
      GET,
      POST,
    };
    Method method = GET;
    char const *post_begin = nullptr;
    char const *post_end = nullptr;
    char const *get_begin = nullptr;
    char const *get_end = nullptr;
    std::string upload_path;
    std::string right_str;
    unsigned int len = 0;
    void set_post_data(char const *begin, char const *end)
    {
      post_begin = begin;
      post_end = end;
      method = POST;
    }
    void set_get_data(char const *begin, char const *end, char const *right)
    {
      get_begin = begin;
      get_end = end;
      len = get_end - get_begin;
      // Serial.print("LenFn: ");
      // Serial.println(len);
      method = GET;
      right_str = std::string(right);
    }
    void set_upload_path(std::string const &path)
    {
      upload_path = path;
    }
  };

  bool requestV2(String message, const std::string &url, RequestOption const &opt, String *reply);

public:
  struct Response {
    bool isSuccessful;
    std::string respBodyString;
  };

  TwitterClient(){};
  TwitterClient(NTPClient &timeClient, std::string const &consumer_key, std::string const &consumer_sec, std::string const &accesstoken, std::string const &accesstoken_sec);
  void startNTP();
  void startNTP(int timeOffset);
  void startNTP(int timeOffset, int updateInterval);
  String getTimeStamp();
  unsigned long getEpoch();
  bool tweet(std::string message, const std::vector<std::string> *media_ids = nullptr);
  String searchTwitter(std::string message);
  String searchUser(std::string message);
  Response getV2(std::string pathWithParams);
  Response postV2(std::string pathWithParams, std::string bodyString);
};
