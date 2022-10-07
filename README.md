# ESP8266 Twitter client 

This library forked from [TwitterWebAPI](https://github.com/debsahu/TwitterWebAPI). Existing TwitterWebAPI only works with Twitter API v1 which is not a default offering from twitter as of April 2022. So started enhancing this codebase to work with [Twitter API V2](https://developer.twitter.com/en/docs/twitter-api/tweets/lookup/introduction) endpoints.  
TODO:
- [x] Create Tweet
- [x] Search User
- [x] Search Twitter
- [x] Generic api for making V2 GET requests
- [x] Generic api for making V2 POST requests
- [ ] Modify Examples for V2

# About

An Arduino library to talk to Twitter using [Twitter Web API](https://dev.twitter.com/overview/api) made for ESP8266. This is based on a sketch posted [here](https://github.com/soramimi/ESP8266Tweet). There are other approaches like using a bearer token [arduino-twitter-api](https://github.com/witnessmenow/arduino-twitter-api), but there are limitations in terms of not being able to send tweet. This can both search/read and post tweets.



## Consumer Key, Consumer Secret, Access Token & Access Token Secret
In order to talk to Twitter,

You need to create a Project, click on “New Project” in your [dashboard](https://developer.twitter.com/en/portal/dashboard) or the [Projects & Apps](https://developer.twitter.com/en/portal/projects-and-apps) page within the developer portal. You’ll only be able to see this option if you haven’t already created a Project. You will be prompted to create a Project name, description, and use case. You will also be asked to create a new App or connect an existing standalone App.

* Goto https://developer.twitter.com/en/portal/dashboard and sign in (if you havent already).
* Fill in the fields, For website you can enter any webpage (e.g. http://google.com), and create your app
* Then click on the Keys & Tokens tab. Your Consumer Key (API Key) and Consumer Secret (Secret) will be under Consumer Keys section, if not click on Generate.
* Similarly, get access token and secret under Authentication tokens section.

Fill the obtained Consumer Key, Consumer Secret, Access Token and Access Token Secret inside the sketch.

## Using the Library
* Download this GitHub [library](https://github.com/krishnakumar4a4/esp8266-twitter/archive/refs/heads/master.zip).
* In Arduino, Goto Sketch -> Include Library -> Add .ZIP Library... and point to the zip file downloaded.
* Install [TimeLib library](https://github.com/PaulStoffregen/Time) and [NTPClient library](https://github.com/arduino-libraries/NTPClient) using the same procedure.

To use in your sketch include these lines.
```c++
#include <TwitterWebAPI.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
```
Define Twitter GET request timeout (optional, default is 1500ms)
```c++
#define TWI_TIMEOUT 2000  // in msec
```
Setup correct timezone to correct the time obtained from NTP server.
```c++
const char *ntp_server = "pool.ntp.org";  // time1.google.com, time.nist.gov, pool.ntp.org
int timezone = -5;                        // US Eastern timezone -05:00 HRS
```
You **WILL** need **Consumer Key, Consumer Secret, Access Token and Access Token Secret** that can be obtained from the above steps. 
```c++
// Values below are just a placeholder
// Obtain these by creating an app @ https://apps.twitter.com/
  static char const consumer_key[]    = "gkyjeH3EF32NJfiuheuyf8623";
  static char const consumer_sec[]    = "HbY5h$N86hg5jjd987HGFsRjJcMkjLaJw44628sOh353gI3H23";
  static char const accesstoken[]     = "041657084136508135-F3BE63U4Y6b346kj6bnkdlvnjbGsd3V";
  static char const accesstoken_sec[] = "bsekjH8YT3dCWDdsgsdHUgdBiosesDgv43rknU4YY56Tj";
```
Declare clients before setup().
```c++
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_server, timezone*3600, 60000);  // NTP server pool, offset (in seconds), update interval (in milliseconds)
TwitterClient tcr(timeClient, consumer_key, consumer_sec, accesstoken, accesstoken_sec);
```
In setup make sure to start NTP connection. A correct time is required to be able to post/search on Twitter.
```c++
tcr.startNTP();
```
**Search for a key word**
```c++
std::string search_str;
String resultJsonArray = tcr.searchTwitter(search_str);
```
**Search Twitter User by handle name**
```c++
std::string search_str;
String userJson = tcr.searchUser(search_str);
```
**Post to Twitter**
```c++
std::string twitter_post_msg;
bool status = tcr.tweet(twitter_post_msg);
```

## Using Generic methods
Since it will be hard to implement methods with all possible combination of api specfications like different path & query params. Generic methods are introduced where you can define the path string with any required query params (just like the way you write if you were to `curl`).

Wheere do I find API specification?
1. [Postman Collection On Cloud](https://t.co/twitter-api-postman)
2. [OpenAPI specification](https://api.twitter.com/2/openapi.json)
3. [Postman Collection Download for local](https://github.com/twitterdev/postman-twitter-api)
4. [Twitter API V2 documentation](https://developer.twitter.com/en/docs/twitter-api/tweets/lookup/introduction)


```c++
// for making GET requests for V2 API, pass any path with path and query params excluding the V2 base url

// E.g: if your full url is https://api.twitter.com/2/tweets/search/recent?query=some-string-to-search, you would pass only `/tweets/search/recent?query=some-string-to-search` as parameter for the function
tcr.getV2(pathWithQueryParams std::string)  
```

```c++
// for making POST requests for V2 API, pass string with path and query params excluding the V2 base url and the post body

// E.g: if your full url is https://api.twitter.com/2/tweets, you would pass only `/tweets` as first parameter for the function and the body as second parameter
tcr.postV2(pathWithQueryParams std::string, bodyString std::string) 
```

**Search recent tweets using search string**
```c++
  // some-to-string-to-search is a placeholder for search string you can replace with
  TwitterClient::Response searchResponse = tcr.getV2("/tweets/search/recent?query=some-string-to-search");
  Serial.println(tweetSsearchResponseearch.respBodyString.c_str());
```

**Post tweet**
```c++
  // require the body should be escaped json as per api specification
  TwitterClient::Response searchResponse = tcr.postV2("/tweets", "{\"text\": \"Hello There!!\"}");
  Serial.println(tweetSsearchResponseearch.respBodyString.c_str());
```

## Example: TwitterTweetSearchFSWiFiMgr
In addition to [TwitterWebAPI](https://github.com/debsahu/TwitterWebAPI), make sure that [TimeLib library](https://github.com/PaulStoffregen/Time) and [NTPClient library](https://github.com/arduino-libraries/NTPClient) is installed, and install the following libraries 
* [Arduino JSON](https://github.com/bblanchon/ArduinoJson)
* [WiFiManager](https://github.com/tzapu/WiFiManager)

For display you can either install
* [MAX7219 library](https://github.com/SensorsIot/MAX7219-4-digit-display-for-ESP8266) 
or 
* [MD Parola](https://github.com/MajicDesigns/MD_Parola) & [MD_Max72xx](https://github.com/MajicDesigns/MD_MAX72xx)
