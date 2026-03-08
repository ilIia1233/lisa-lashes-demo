# ── Stage 1: Build ─────────────────────────────────────────────────────────────
FROM alpine:3.20 AS builder

RUN apk update && apk add --no-cache \
    g++ make cmake \
    postgresql-dev \
    argon2-dev

WORKDIR /app

COPY include      /app/include
COPY lib          /app/lib
COPY src          /app/src
COPY back         /app/back
COPY CMakeLists.txt /app/CMakeLists.txt

RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

# ── Stage 2: Runtime ───────────────────────────────────────────────────────────
FROM alpine:3.20

RUN apk update && apk add --no-cache \
    libpq \
    argon2

WORKDIR /app

COPY --from=builder /app/build/server /app/build/server
COPY front /app/front

EXPOSE 8000

WORKDIR /app/build

ENTRYPOINT ["/app/build/server"]
