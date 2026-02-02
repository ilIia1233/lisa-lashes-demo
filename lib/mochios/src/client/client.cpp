#include <mochios/client/client.h>
#include <mochios/helpers/client.h>

mochios::Client::Client(const mochios::client::Connection &connection)
    : connection(connection) {
  struct addrinfo hints {};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(connection.host.c_str(),
                  std::to_string(connection.port).c_str(), &hints,
                  &this->server) != 0) {
    logger::error("Failed to get address info for " + connection.host,
                  "mochios::Client::Client(const mochios::client::Connection "
                  "&connection)");
  }

  this->connect();
  close(this->socket);

  this->internalInterceptors.request.use(
      mochios::interceptor::request::userAgent);
  this->internalInterceptors.request.use(mochios::interceptor::request::cookie);
  this->internalInterceptors.response.use(mochios::interceptor::response::json);
  return;
}

mochios::Client::~Client() {
  if (this->socket > 0) {
    close(this->socket);
  }

  return;
}

mochios::messages::Response
mochios::Client::get(mochios::messages::Request &request) {
  request.method = mochios::enums::method::GET;
  return this->sendHelper(request);
}

mochios::messages::Response
mochios::Client::post(mochios::messages::Request &request) {
  request.method = mochios::enums::method::POST;
  return this->sendHelper(request);
}

mochios::messages::Response
mochios::Client::put(mochios::messages::Request &request) {
  request.method = mochios::enums::method::PUT;
  return this->sendHelper(request);
}

mochios::messages::Response
mochios::Client::patch(mochios::messages::Request &request) {
  request.method = mochios::enums::method::PATCH;
  return this->sendHelper(request);
}

mochios::messages::Response
mochios::Client::del(mochios::messages::Request &request) {
  request.method = mochios::enums::method::DELETE;
  return this->sendHelper(request);
}

mochios::messages::Response
mochios::Client::options(mochios::messages::Request &request) {
  request.method = mochios::enums::method::OPTIONS;
  return this->sendHelper(request);
}

mochios::messages::Response
mochios::Client::head(mochios::messages::Request &request) {
  request.method = mochios::enums::method::HEAD;
  return this->sendHelper(request);
}

void mochios::Client::connect() {
  this->socket =
      brewtils::sys::socket(this->server->ai_family, this->server->ai_socktype,
                            this->server->ai_protocol);
  if (this->socket < 0) {
    freeaddrinfo(this->server);
    logger::error("Failed to create socket", "void mochios::Client::connect()");
  }

  int flags = fcntl(this->socket, F_GETFL, 0);
  fcntl(this->socket, F_SETFL, flags | O_NONBLOCK);

  int result =
      ::connect(this->socket, this->server->ai_addr, this->server->ai_addrlen);
  if (result < 0 && errno != EINPROGRESS) {
    freeaddrinfo(this->server);
    close(this->socket);
    this->socket = -1;
    logger::error("Failed to connect to server at " + connection.host,
                  "void mochios::Client::connect()");
    return;
  }

  if (result != 0) {
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(this->socket, &writefds);

    struct timeval timeout {};
    timeout.tv_sec = this->connection.timeout;
    timeout.tv_usec = 0;

    int sel = select(this->socket + 1, nullptr, &writefds, nullptr, &timeout);
    if (sel <= 0) {
      close(this->socket);
      this->socket = -1;
      freeaddrinfo(this->server);
      logger::error("Connection timed out", "mochios::Client::connect()");
      return;
    }

    int so_error;
    socklen_t len = sizeof(so_error);
    getsockopt(this->socket, SOL_SOCKET, SO_ERROR, &so_error, &len);
    if (so_error != 0) {
      close(this->socket);
      this->socket = -1;
      freeaddrinfo(this->server);
      logger::error("Connection failed: " + std::to_string(so_error),
                    "mochios::Client::connect()");
      return;
    }
  }

  fcntl(this->socket, F_SETFL, flags);

  struct timeval timeout;
  timeout.tv_sec = this->connection.timeout;
  timeout.tv_usec = 0;
  setsockopt(this->socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  timeout.tv_sec = this->connection.timeout;
  timeout.tv_usec = 0;
  setsockopt(this->socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

mochios::messages::Response
mochios::Client::sendHelper(mochios::messages::Request &request) {
  request.set("Host", this->connection.host);
  request.set("Accept", "*/*");
  request.set("Connection", "close");
  for (const std::function<void(mochios::messages::Request &)>
           &requestInterceptor : this->interceptors.request.interceptors) {
    requestInterceptor(request);
  }
  for (const std::function<void(mochios::messages::Request &)>
           &requestInterceptor :
       this->internalInterceptors.request.interceptors) {
    requestInterceptor(request);
  }
  this->connect();
  mochios::messages::Response res =
      mochios::helpers::client::send(request, this->socket);
  for (const std::function<void(mochios::messages::Response &)>
           &responseInterceptor :
       this->internalInterceptors.response.interceptors) {
    responseInterceptor(res);
  }
  for (const std::function<void(mochios::messages::Response &)>
           &responseInterceptor : this->interceptors.response.interceptors) {
    responseInterceptor(res);
  }
  return res;
}