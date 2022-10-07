#include "TwitterWebAPI.h"
#include "url.h"

  bool TwitterClient::requestV2(String message, const std::string &url, TwitterClient::RequestOption const &opt, String *reply)
  {
    Serial.print("Tweet to be published: ");
    Serial.println(message);
    if (reply)
      *reply = "";
    if (opt.method == RequestOption::GET)
    {
      if (opt.upload_path.empty())
      {
        String host, path;
        int port = 0;
        {
          URL l(url);
          host = l.host().c_str();
          path = l.path().c_str();
          port = l.port();
        }
        WiFiClientSecure client;
        client.setInsecure();
        if (client.connected())
        {
          client.flush();
          client.stop();
        }
        client.setTimeout(TWI_TIMEOUT * 10);
        if (client.connect(host.c_str(), port))
        {
          client.println("GET " + path + opt.right_str.c_str() + " HTTP/1.1");
          client.println("Host: " + host);
          client.print("Authorization: OAuth ");
          client.write((uint8_t const *)opt.get_begin, opt.len);
          client.println();
          client.println("User-Agent: ESP8266");
          client.println("Accept: */*");
          client.println("Connection: keep-alive");
          client.println("Content-Type: application/json");
          // client.println("Accept-Encoding: gzip, deflate, br");
          client.println("Content-Length: 0");
          client.println();
          while (client.connected())
          {
            Serial.println("inside client.connected()");
            String header = client.readStringUntil('\n');
            Serial.print("header: ");
            Serial.println(header);
            if (header == "\r") {
              Serial.print("received: \r breaking");
              break; // headers received
            }
          }
          *reply = client.readString();
          if (client.available())
          {
            client.flush();
            client.stop();
          }
          return true;
        }
      }
    }
    else if (opt.method == RequestOption::POST)
    {
      if (reply)
        *reply = "";
      ;
      if (opt.upload_path.empty())
      {
        String host, path;
        int port = 0;
        {
          URL l(url);
          host = l.host().c_str();
          path = l.path().c_str();
          port = l.port();
        }
        WiFiClientSecure client;
        client.setInsecure();
        if (client.connected())
        {
          client.flush();
          client.stop();
        }
        client.setTimeout(TWI_TIMEOUT);
        if (client.connect(host.c_str(), port))
        {
          int len = opt.post_end - opt.post_begin;
          client.println("POST " + path + " HTTP/1.1");
          client.println("Host: " + host);
          client.print("Authorization: OAuth ");
          client.write((uint8_t const *)opt.post_begin, len);
          client.println();
          client.println("User-Agent: ESP8266");
          client.println("Connection: keep-alive");
          client.println("Content-Type: application/json");
          client.println("Accept: */*");
          // client.println("Accept-Encoding: gzip, deflate, br");
          client.print("Content-Length: ");
          String body = "{\"text\": \"";
          body.concat(message);
          body.concat("\"}");
          client.println(body.length());
          client.println();
          client.println(body);
          String s = client.readString();
          *reply = s;
          if (client.available())
          {
            client.flush();
            client.stop();
          }
          return true;
        }
      }
      else
      {
        // not implemented
      }
    }
    return false;
  }

  TwitterClient::TwitterClient(NTPClient &timeClient, std::string const &consumer_key, std::string const &consumer_sec, std::string const &accesstoken, std::string const &accesstoken_sec)
  {
    // this->client = &client;
    this->timeClient = &timeClient;
    data.keys.consumer_key = consumer_key;
    data.keys.consumer_sec = consumer_sec;
    data.keys.accesstoken = accesstoken;
    data.keys.accesstoken_sec = accesstoken_sec;
  }

  void TwitterClient::startNTP()
  {
    // client->setTimeout(TWI_TIMEOUT);
    timeClient->begin();
    timeClient->forceUpdate();
  }

  void TwitterClient::startNTP(int timeOffset)
  {
    // client->setTimeout(TWI_TIMEOUT);
    timeClient->setTimeOffset(timeOffset);
    timeClient->begin();
    timeClient->forceUpdate();
  }

  void TwitterClient::startNTP(int timeOffset, int updateInterval)
  {
    // client->setTimeout(TWI_TIMEOUT);
    timeClient->setTimeOffset(timeOffset);
    timeClient->setUpdateInterval(updateInterval);
    timeClient->begin();
    timeClient->forceUpdate();
  }

  String TwitterClient::getTimeStamp()
  {
    timeClient->update();
    return timeClient->getFormattedTime();
  }

  unsigned long TwitterClient::getEpoch()
  {
    timeClient->update();
    return timeClient->getEpochTime();
  }

  bool TwitterClient::tweet(std::string message, const std::vector<std::string> *media_ids /*= nullptr*/)
  {
    timeClient->update();
    time_t currentTime = (time_t)timeClient->getEpochTime();

    if (message.empty())
    {
      return false;
    }

    std::string url = "https://api.twitter.com/2/tweets";
    oauth::Request oauth_req = oauth::sign(url.c_str(), oauth::POST, keys(), currentTime);
    String res;
    RequestOption opt;
    char const *p = oauth_req.post.c_str();
    opt.set_post_data(p, p + oauth_req.post.size());
    boolean status = requestV2(message.c_str(), oauth_req.url, opt, &res);
    Serial.println(res);
    return status;
  }

  String TwitterClient::searchTwitter(std::string message)
  {
    timeClient->update();
    time_t currentTime = (time_t)timeClient->getEpochTime();
    // Serial.print("Epoch: "); Serial.println(currentTime);

    if (message.empty())
    {
      return "Error with user search term!";
    }

    std::string url = "https://api.twitter.com/2/tweets/search/recent";

    url += "?query=";
    url += misc::url_encode(message);

    oauth::Request oauth_req = oauth::sign(url.c_str(), oauth::GET, keys(), currentTime);
    String res;
    RequestOption opt;
    char const *p = oauth_req.getdata.c_str();
    char const *r = oauth_req.right.c_str();
    opt.set_get_data(p, p + oauth_req.getdata.size(), r);
    Serial.println("before request V2 call");
    bool status = requestV2("",oauth_req.url, opt, &res);
    if (status)
    {
      return res;
    }
    else
    {
      return "Error";
    }
  }

  String TwitterClient::searchUser(std::string message)
  {
    timeClient->update();
    time_t currentTime = (time_t)timeClient->getEpochTime();
    //    Serial.print("Epoch: "); Serial.println(currentTime);

    if (message.empty())
    {
      return "Error with search term!";
    }

    std::string url = "https://api.twitter.com/2/users/by/username/";
    url += misc::url_encode(message);

    Serial.println("url formed");
    Serial.println(url.c_str());
    oauth::Request oauth_req = oauth::sign(url.c_str(), oauth::GET, keys(), currentTime);
    Serial.println("sign");
    String res;
    RequestOption opt;
    char const *p = oauth_req.getdata.c_str();
    char const *r = oauth_req.right.c_str();
    opt.set_get_data(p, p + oauth_req.getdata.size(), r);
    Serial.println("before request V2 call");
    bool status = requestV2("",oauth_req.url, opt, &res);
    if (status)
    {
      return res;
    }
    else
    {
      return "Error";
    }
  }