# Zippuccino

A simple, fast and lightweight C++ zipping library, all without any external dependencies, which allows you to understand how zipping actually works.

## Installation

0. Make sure you have the gnu g++ compiler installed on your system. You can install it by running the following command:

   ```bash
   sudo apt install g++ build-essential
   ```

1. Clone the repository:

   ```bash
   git clone git@github.com:coding-cpp/zippuccino
   cd zippuccino
   ```

2. Clone the submodules:

   ```bash
   git submodule init
   git submodule update
   ```

3. Build the project:

   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

4. Run the project to zip the project itself :)

   ```bash
   ./zippuccino
   ```

## Example

You can find an example of how to use zippuccino in the [example](./example) directory.
