#include <mochios/client/client.h>

int main(int argc, char **argv) {
  mochios::client::Connection connection;
  connection.host = "expresso.aditjain.me";
  connection.port = 80;
  connection.timeout = 2; // this is in seconds

  mochios::Client client(connection);
  client.interceptors.request.use([](mochios::messages::Request &request) {
    logger::info("Intercepting request!");
    request.print();
  });

  mochios::messages::Request request("/about");
  try {
    mochios::messages::Response response = client.get(request);
    logger::success("Request to \"" + request.path +
                    "\" succeeded with status code " +
                    std::to_string(response.statusCode));
    response.print();
  } catch (const mochios::messages::Response &e) {
    logger::error("Request to \"" + request.path +
                  "\" failed with status code " + std::to_string(e.statusCode));
    e.print();
  }

  return EXIT_SUCCESS;
}