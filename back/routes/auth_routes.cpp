// auth_routes.cpp
#include "auth_routes.h"
#include <string>

// POST /api/auth/register
// Body: { "first_name", "last_name", "phone", "password" }
void PostRegisterRoute(expresso::messages::Request &req,
                       expresso::messages::Response &res) {
  try {
    json::object body = req.body;

    if (body.find("first_name") == body.end() ||
        body.find("last_name") == body.end() ||
        body.find("phone") == body.end() ||
        body.find("password") == body.end()) {
      json::object err;
      err["message"] = "Missing required fields";
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
          .json(err)
          .end();
    }
    User user;

    user.first = std::string(body["first_name"]);
    user.last = std::string(body["last_name"]);
    user.phone = std::string(body["phone"]);
    user.password = std::string(body["password"]);
    std::string address = (body.find("address") != body.end())
                              ? std::string(body["address"])
                              : "";
    user.address = address;

    bool ok = UserContext::UserService->registerUser(user);

    if (!ok) {
      json::object err;
      err["message"] = "Registration failed (phone may already be in use)";
      return res.status(expresso::enums::STATUS_CODE::CONFLICT).json(err).end();
    }

    json::object data;
    data["message"] = "User registered successfully";
    return res.status(expresso::enums::STATUS_CODE::CREATED).json(data).end();
  } catch (const std::exception &e) {
    json::object err;
    err["message"] = "Internal server error";
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(err)
        .end();
  }
}

// DELETE /api/auth/user?id=<id>
void DeleteUserRoute(expresso::messages::Request &req,
                     expresso::messages::Response &res) {
  try {
    std::string id_str = req.queries["id"];

    if (id_str.empty()) {
      json::object err;
      err["message"] = "Missing user id";
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
          .json(err)
          .end();
    }

    int id = std::stoi(id_str);

    bool ok = UserContext::UserService->removeUser(id);

    if (!ok) {
      json::object err;
      err["message"] = "User not found or could not be deleted";
      return res.status(expresso::enums::STATUS_CODE::NOT_FOUND)
          .json(err)
          .end();
    }

    json::object data;
    data["message"] = "User deleted successfully";
    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();
  } catch (const std::exception &e) {
    json::object err;
    err["message"] = "Internal server error";
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(err)
        .end();
  }
}
void PostLoginRoute(expresso::messages::Request &req,
                    expresso::messages::Response &res) {
  try {
    json::object body = req.body;

    bool hasPhone = body.find("phone") != body.end();
    bool hasAddress = body.find("address") != body.end();
    bool hasPassword = body.find("password") != body.end();

    if ((!hasPhone && !hasAddress) || !hasPassword) {
      json::object err;
      err["message"] =
          "Missing credentials: provide phone or address (email), and password";
      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
          .json(err)
          .end();
    }

    std::string identifier =
        hasPhone ? std::string(body["phone"]) : std::string(body["address"]);

    auto userId =
        UserContext::UserService->loginUser(identifier, body["password"]);

    if (!userId.has_value()) {
      json::object err;
      err["message"] = "Invalid credentials";
      return res.status(expresso::enums::STATUS_CODE::UNAUTHORIZED)
          .json(err)
          .end();
    }

    json::object data;
    data["message"] = "Login successful";
    data["user_id"] = userId.value();
    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();
  } catch (const std::exception &e) {
    json::object err;
    err["message"] = std::string("Internal server error: ") + e.what();
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(err)
        .end();
  }
}
