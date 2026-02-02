#pragma once

#include <csignal>
#include <sys/socket.h>

#include <logger/log.h>

namespace brewtils {

namespace sys {

size_t send(int sockfd, const void *buf, size_t len, int flags);

size_t recv(int sockfd, void *buf, size_t len, int flags);

int socket(int domain, int type, int protocol) noexcept(true);

int listen(int sockfd, int backlog) noexcept(true);

int bind(int sockfd, const struct sockaddr *addr,
         socklen_t addrlen) noexcept(true);

void exitIf(int signal) noexcept(true);

} // namespace sys

} // namespace brewtils