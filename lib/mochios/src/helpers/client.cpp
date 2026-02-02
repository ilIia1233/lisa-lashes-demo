#include <mochios/helpers/client.h>

std::pair<std::string, std::string>
mochios::helpers::client::buildRequest(mochios::messages::Request &request) {
  std::ostringstream oss;
  oss << request.method << " " << request.path;
  if (request.queries.size() > 0) {
    oss << "?";
    bool isFirst = true;
    for (const std::pair<std::string, std::string> &query : request.queries) {
      if (isFirst) {
        isFirst = false;
      } else {
        oss << "&";
      }
      oss << brewtils::url::encode(query.first) << "="
          << brewtils::url::encode(query.second);
    }
  }
  oss << " HTTP/1.1\r\n";

  std::string jsonifiedBody = "";
  if (request.method == mochios::enums::method::POST ||
      request.method == mochios::enums::method::PUT ||
      request.method == mochios::enums::method::PATCH) {
    jsonifiedBody = request.body.dumps(0);
    request.set("Content-Type", "application/json");
    request.set("Content-Length", std::to_string(jsonifiedBody.size()));
  }
  for (const std::pair<std::string, std::string> &header : request.headers) {
    oss << header.first << ": " << header.second << "\r\n";
  }
  oss << "\r\n";

  return std::make_pair(oss.str(), jsonifiedBody);
}

void mochios::helpers::client::buildResponse(mochios::messages::Response &res,
                                             std::stringstream &response) {
  std::string line;

  // Headers
  while (std::getline(response, line) && line != "\r") {
    size_t separator = line.find(":");
    if (separator == std::string::npos) {
      continue;
    }

    std::string key = brewtils::string::lower(line.substr(0, separator));
    std::string value = line.substr(separator + 2, line.size() - separator - 3);
    if (key == "set-cookie") {
      mochios::messages::Cookie *newCookie =
          new mochios::messages::Cookie(value);
      res.cookies.push_back(newCookie);
      continue;
    }

    res.set(key, value);
  }

  std::stringstream body;
  while (std::getline(response, line) && line != "\r") {
    body << line;
  }

  res.body = body.str();
  return;
}

void mochios::helpers::client::buildDefaultResponse(
    mochios::messages::Response &res, int statusCode) {
  res.statusCode = statusCode;
  json::object responseBody;

  switch (statusCode) {
  case 400:
    responseBody["error"] = "Bad Request";
    responseBody["message"] =
        "The request could not be understood by the server.";
    break;
  case 404:
    responseBody["error"] = "Not Found";
    responseBody["message"] = "The requested resource could not be found.";
    break;
  case 408:
    responseBody["error"] = "Request Timeout";
    responseBody["message"] = "The server timed out waiting for the request.";
    break;
  case 500:
    responseBody["error"] = "Internal Server Error";
    responseBody["message"] = "An unexpected error occurred on the server.";
    break;
  default:
    responseBody["error"] = "Unknown Error";
    responseBody["message"] = "An unknown error occurred.";
    break;
  }

  res.body = responseBody.dumps(0);
  res.set("Content-Type", "application/json");
  res.set("Connection", "close");
  res.set("Content-Length", std::to_string(res.body.size()));
  return;
}

mochios::messages::Response
mochios::helpers::client::send(mochios::messages::Request &request,
                               const int &socket) {
  mochios::messages::Response res;
  bool socketClosed = false;
  try {
    std::pair<std::string, std::string> requestString =
        mochios::helpers::client::buildRequest(request);
    if (brewtils::sys::send(socket, requestString.first.c_str(),
                            requestString.first.size(), 0) < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        mochios::helpers::client::buildDefaultResponse(res, 408);
        throw res;
      } else {
        logger::error("Error sending request headers",
                      "mochios::messages::Response "
                      "mochios::helpers::client::send(mochios::messages::"
                      "Request &request, const mochios::enums::method &method, "
                      "const int &socket)");
      }
    }
    if (requestString.second.size() > 0) {
      if (brewtils::sys::send(socket, requestString.second.c_str(),
                              requestString.second.size(), 0) < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          mochios::helpers::client::buildDefaultResponse(res, 408);
          throw res;
        } else {
          if (brewtils::sys::send(socket, requestString.second.c_str(),
                                  requestString.second.size(), 0) < 0) {
            logger::error("Error sending request body",
                          "mochios::messages::Response "
                          "mochios::helpers::client::send(mochios::messages::"
                          "Request &request, const mochios::enums::method "
                          "&method, const int &socket)");
          }
        }
      }
    }

    std::stringstream oss;
    char buffer[4096];
    int bytesRead;
    while ((bytesRead = brewtils::sys::recv(socket, buffer, 4096, 0)) > 0) {
      oss.write(buffer, bytesRead);
      memset(buffer, 0, 4096);
    }
    close(socket);
    socketClosed = true;

    if (bytesRead < 0) {
      if (oss.str().empty() || errno == EAGAIN || errno == EWOULDBLOCK) {
        mochios::helpers::client::buildDefaultResponse(res, 408);
        throw res;
      } else {
        logger::error("Receive failed: " + std::string(strerror(errno)),
                      "mochios::messages::Response "
                      "mochios::helpers::client::send(mochios::messages::"
                      "Request &request, const mochios::enums::method &method, "
                      "const int &socket)");
      }
    }

    std::string line;
    std::getline(oss, line);
    std::vector<std::string> parts = brewtils::string::split(line, " ");
    res.statusCode = std::stoi(brewtils::string::trim(parts[1]));
    if (parts.size() > 2) {
      res.statusText = brewtils::string::trim(parts[2]);
    }
    mochios::helpers::client::buildResponse(res, oss);
  } catch (const mochios::messages::Response &e) {
  } catch (const std::exception &e) {
    logger::error("Exception occurred: " + std::string(e.what()));
    if (!socketClosed) {
      close(socket);
      socketClosed = true;
    }
    mochios::helpers::client::buildDefaultResponse(res, 500);
  }

  if (res.statusCode >= 200 && res.statusCode < 300) {
    return res;
  } else {
    throw res;
  }
}