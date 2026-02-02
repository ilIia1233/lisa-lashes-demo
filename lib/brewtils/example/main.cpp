#include <brewtils/base64.h>
#include <brewtils/date.h>
#include <brewtils/env.h>
#include <brewtils/os.h>
#include <brewtils/string.h>
#include <brewtils/url.h>
#include <brewtils/uuid.h>

int main(int argc, char **argv) {
  brewtils::env::init("../.env");
  logger::info("USER: " + brewtils::env::get("USER", "adit"));

  logger::info(brewtils::uuid::v4());

  logger::info(brewtils::uuid::v5("6ba7b810-9dad-11d1-80b4-00c04fd430c8",
                                  "hello world"));

  std::string data =
      "   {\"widget\" : {\"debug\" : \"on\",      \"window\" : {        "
      "\"title\" "
      ": \"Sample Konfabulator Widget\",        \"name\" : \"main_window\",    "
      "    \"width\" : 500,        \"height\" : 500      },      \"image\" : { "
      "       \"src\" : \"Images/Sun.png\",        \"name\" : \"sun1\",        "
      "\"hOffset\" : 250,        \"vOffset\" : 250,        \"alignment\" : "
      "\"center\"      },      \"text\" : {        \"data\" : \"Click Here\",  "
      "      \"size\" : 36,        \"style\" : \"bold\",        \"name\" : "
      "\"text1\",        \"hOffset\" : 250,        \"vOffset\" : 100,        "
      "\"alignment\" : \"center\",        \"onMouseUp\" : \"sun1.opacity = "
      "(sun1.opacity / 100) * 90;\"      }    }  }   ";
  std::string encoded = brewtils::base64::encode(data);
  logger::info(encoded);

  std::string decoded = brewtils::base64::decode(encoded);
  logger::info(decoded);

  std::string trimmedData = brewtils::string::trim(data);
  logger::info(trimmedData);

  std::vector<std::string> strings = brewtils::string::split(data, " ");
  std::string joined = brewtils::string::join(strings, "");
  logger::info(joined);

  data = "https://www.google.com/search?q=hello+world";
  encoded = brewtils::url::encode(data);
  logger::info(encoded);

  decoded = brewtils::url::decode(encoded);
  logger::info(decoded);

  for (std::string file : brewtils::os::dir::list("/")) {
    logger::info(file);
  }

  logger::info("---------------------------");
  for (std::string file : brewtils::os::dir::tree("../")) {
    logger::info(file);
  }

  logger::info("---------------------------");
  logger::info(brewtils::os::isDocker() ? "Running in Docker"
                                        : "Not in Docker");

  logger::info("---------------------------");
  logger::info(brewtils::os::file::getMimeType("test.txt"));

  logger::info("---------------------------");
  logger::info(brewtils::date::getCurrentUTC());

  return EXIT_SUCCESS;
}
