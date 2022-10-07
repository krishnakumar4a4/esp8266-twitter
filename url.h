
#include <NTPClient.h>
#include <string>

class URL
{
private:
  std::string scheme_;
  std::string host_;
  int port_ = 0;
  std::string path_;

public:
  URL(const std::string &url)
  {
    char const *str = url.c_str();
    char const *left;
    char const *right;
    left = str;
    right = strstr(left, "://");
    if (right)
    {
      scheme_.assign(str, right - str);
      left = right + 3;
    }
    right = strchr(left, '/');
    if (right)
    {
      char const *p = strchr(left, ':');
      if (p && left < p && p < right)
      {
        int n = 0;
        char const *q = p + 1;
        while (q < right)
        {
          if (isdigit(*q & 0xff))
          {
            n = n * 10 + (*q - '0');
          }
          else
          {
            n = -1;
            break;
          }
          q++;
        }
        host_.assign(left, p - left);
        if (n > 0 && n < 65536)
        {
          port_ = n;
        }
      }
      else
      {
        host_.assign(left, right - left);
      }
      path_ = right;
    }
    if (port_ == 0)
    {
      if (scheme_ == "http")
      {
        port_ = 80;
      }
      else if (scheme_ == "https")
      {
        port_ = 443;
      }
    }
  }
  std::string const &scheme() const { return scheme_; }
  std::string const &host() const { return host_; }
  int port() const { return port_; }
  std::string const &path() const { return path_; }
  bool isssl() const
  {
    if (scheme() == "https")
      return true;
    if (scheme() == "http")
      return false;
    if (port() == 443)
      return true;
    return false;
  }
};

