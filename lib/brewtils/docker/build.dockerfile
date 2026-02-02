FROM alpine:3.20.2 AS builder

RUN apk update && apk add --no-cache g++ make cmake

WORKDIR /app

COPY example /app/example
COPY include /app/include
COPY lib /app/lib
COPY src /app/src
COPY CMakeLists.txt /app/CMakeLists.txt

RUN mkdir build && cd build && cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_EXE_LINKER_FLAGS="-static" .. && make
RUN chmod +x /app/build/brewtils

FROM scratch

WORKDIR /app

COPY --from=builder /app/build/brewtils /app/brewtils

ENTRYPOINT ["/app/brewtils"]
