#include <mochios/helpers/cookie.h>

std::string
mochios::helpers::cookie::serialize(const mochios::messages::Cookie &cookie) {
  std::string cookieString = brewtils::url::encode(cookie.name) + "=" +
                             brewtils::url::encode(cookie.value);
  if (!cookie.domain.empty()) {
    cookieString += "; Domain=" + cookie.domain;
  }
  if (!cookie.path.empty()) {
    cookieString += "; Path=" + cookie.path;
  }
  if (!cookie.expires.empty()) {
    cookieString += "; Expires=" + cookie.expires;
  }
  if (!cookie.maxAge.empty()) {
    cookieString += "; Max-Age=" + cookie.maxAge;
  }
  if (!cookie.sameSite.empty()) {
    cookieString += "; SameSite=" + cookie.sameSite;
  }
  if (cookie.secure) {
    cookieString += "; Secure";
  }
  if (cookie.httpOnly) {
    cookieString += "; HttpOnly";
  }
  if (cookie.partitioned) {
    cookieString += "; Partitioned";
  }
  return cookieString;
}