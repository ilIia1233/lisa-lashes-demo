// auth_routes.cpp
#include "auth_routes.h"
#include "expresso/messages/cookie.h"
#include <string>

static expresso::messages::Cookie *
createSessionCookie(const std::string &token) {
  std::string cookie = "session=" + token +
                       "; Path=/"
                       "; HttpOnly"
                       "; SameSite=Strict"
                       "; Secure";

  return new expresso::messages::Cookie(cookie);
}
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
    std::string email = (body.find("email") != body.end())
                              ? std::string(body["email"])
                              : "";
    user.email = email;

    auto userIdOpt = UserContext::UserService->registerUser(user);

    if (!userIdOpt) {
      json::object err;
      err["message"] =
          "Registration failed (phone or email may already be in use)";
      return res.status(expresso::enums::STATUS_CODE::CONFLICT).json(err).end();
    }
    user.user_id = *userIdOpt;
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

    if (body.find("identifier") == body.end() ||
        body.find("password") == body.end()) {
      json::object err;
      err["message"] = "Missing identifier or password";

      return res.status(expresso::enums::STATUS_CODE::BAD_REQUEST)
          .json(err)
          .end();
    }

    std::string identifier = std::string(body["identifier"]);
    std::string password = std::string(body["password"]);

    auto userIdOpt = UserContext::UserService->loginUser(identifier, password);

    if (!userIdOpt.has_value()) {
      json::object err;
      err["message"] = "Invalid credentials";

      return res.status(expresso::enums::STATUS_CODE::UNAUTHORIZED)
          .json(err)
          .end();
    }

    int userId = userIdOpt.value();

    // create session
    std::string token = UserContext::SessionService->createSession(userId);

    res.setCookie(createSessionCookie(token));
    json::object data;

    data["message"] = "Login successful";

    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    logger::error(e.what());

    json::object err;
    err["message"] = "Internal server error";

    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(err)
        .end();
  }
}

// GET /api/users — return all users (admin)
void GetUsersRoute(expresso::messages::Request &req,
                   expresso::messages::Response &res) {
  try {
    auto users = UserContext::UserService->getAllUsers();

    json::object response;
    response["users"].resize(0);

    for (const auto &u : users) {
      json::object item;
      item["id"] = u.id;
      item["first_name"] = u.first_name;
      item["last_name"] = u.last_name;
      item["phone"] = u.phone;
      item["email"] = u.email;

      response["users"].push_back(item);
    }

    return res.status(expresso::enums::STATUS_CODE::OK).json(response).end();

  } catch (const std::exception &e) {
    json::object err;
    err["message"] = "Internal server error";
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(err)
        .end();
  }
}

// PUT /api/users?id=<id> — update user (admin)
void PutUserRoute(expresso::messages::Request &req,
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
    json::object body = req.body;

    UserContext::UserService->updateUser(id, body);

    json::object data;
    data["message"] = "User updated successfully";

    return res.status(expresso::enums::STATUS_CODE::OK).json(data).end();

  } catch (const std::exception &e) {
    json::object err;
    err["message"] = "Internal server error";
    return res.status(expresso::enums::STATUS_CODE::INTERNAL_SERVER_ERROR)
        .json(err)
        .end();
  }
}
