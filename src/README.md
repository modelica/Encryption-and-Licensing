# Building instructions

## Prererequisites

### Windows
- CMake 3.20 or better
- Perl, e.g. from http://strawberryperl.com/
- Nasm, from http://nasm.sourceforge.net/
- Visual Studio (or MinGW)
- Miniz ziplibrary (https://code.google.com/p/miniz/)
  
### Linux
- CMake 3.20 or better
- Perl
- Normal compilation tools, e.g. GCC
- Miniz ziplibrary (https://code.google.com/p/miniz/)


## Building on Windows with Visual Studio

- Start a MSVC compilation shell for the target platform (32 or 64-bit).

- Add CMake, Perl and Nasm binary directories to PATH.

- Create a build directory and cd to it.

- Execute the commands:
  ``` 
  cmake <source directory> ^
      -DCMAKE_INSTALL_PREFIX=<target directory> ^
      -DDECRYPTOR=<choice of decryptor> ^
      -DLICENSE_MANAGER=<choice of license manager> ^
      -DOBFUSCATOR=<choice of obfuscator module> ^
      -DTOOL_PRIVATE_KEY_DIRECTORY=<openssl_keys directory> ^
      -DLVE_KEYS_DIRECTORY=<openssl_keys directory> ^
      -DTOOLS_PUBLIC_KEYS_DIRECTORY=<openssl_keys directory> ^
      -G"NMake Makefiles"

  cmake --build . --config Release --target install
  ``` 
  Where:
  - `LICENSE_MANAGER`, `DECRYPTOR`, and `OBFUSCATOR` must be set, default implementations are:
    - `OBFUSCATOR`: `src/obfuscators/dummy`
    - `DECRYPTOR`: `src/decryptors/default`
    - `LICENSE_MANAGER`: `src/license_managers/testingdummy`
  - Create a new `openssl_keys` directory by opening "Git Bash" and executing the commands:
    ```
    mkdir openssl_keys
    cd openssl_keys
    openssl genrsa -out "private_key_tool.pem" 4096
    dos2unix "private_key_tool.pem"
    openssl genrsa -out "private_key_lve.pem" 4096
    dos2unix "private_key_lve.pem"
    openssl rsa -pubout -in "private_key_tool.pem" -out "public_key_tool.pem"
    dos2unix "public_key_tool.pem"
    echo public_key_tool.pem > public_key_tools.txt 
    ```
  - The `openssl_keys` directory then contains:
    ```
    private_key_lve.pem
    private_key_tool.pem
    public_key_tool.pem
    public_key_tools.txt
    ```

- To run the unit tests, execute the command:
  ```
  ctest -C Release
  ```

# Building on Linux

## Building on Linux with Docker
- Execute the command:
  ```
  docker run --rm -u $(id -u):$(id -g) -v $(pwd):$(pwd) -w $(pwd) -it python /bin/bash -c '
      set -euo pipefail

      pip install --target cmake cmake
      export PYTHONPATH=$(pwd)/cmake${PYTHONPATH:+:${PYTHONPATH:-}}
      export PATH=$(pwd)/cmake/bin:${PATH}
      git clone https://github.com/modelica/Encryption-and-Licensing.git SEMLA
      cd SEMLA
      mkdir build
      cd build

      # generate keys for testing
      mkdir openssl_keys
      cd openssl_keys
      openssl genrsa -out "private_key_tool.pem" 4096
      openssl genrsa -out "private_key_lve.pem" 4096
      openssl rsa -pubout -in "private_key_tool.pem" -out "public_key_tool.pem"
      echo public_key_tool.pem > public_key_tools.txt 
      cd ..

      # build
      SEMLA_DIR=$(realpath ..)
      cmake "${SEMLA_DIR}/src" \
          -DCMAKE_INSTALL_PREFIX="${SEMLA_DIR}/build" \
          -DDECRYPTOR="${SEMLA_DIR}/src/decryptors/default" \
          -DLICENSE_MANAGER="${SEMLA_DIR}/src/license_managers/testingdummy" \
          -DOBFUSCATOR="${SEMLA_DIR}/src/obfuscators/dummy" \
          -DTOOL_PRIVATE_KEY_DIRECTORY="${SEMLA_DIR}/build/openssl_keys" \
          -DLVE_KEYS_DIRECTORY="${SEMLA_DIR}/build/openssl_keys" \
          -DTOOLS_PUBLIC_KEYS_DIRECTORY="${SEMLA_DIR}/build/openssl_keys"

      cmake --build .
      cmake --build . --target install

      # test
      ctest -C Release
  '
  ```

## Building on Linux without Docker
- Create a build directory and cd to it.
  
- Execute the commands:
  ``` 
  cmake <source directory> \
      -DCMAKE_INSTALL_PREFIX=<target directory> \
      -DDECRYPTOR=<choice of decryptor> \
      -DLICENSE_MANAGER=<choice of license manager> \
      -DOBFUSCATOR=<choice of obfuscator module> \
      -DTOOL_PRIVATE_KEY_DIRECTORY=<openssl_keys directory> \
      -DLVE_KEYS_DIRECTORY=<openssl_keys directory> \
      -DTOOLS_PUBLIC_KEYS_DIRECTORY=<openssl_keys directory>

  cmake --build .
  cmake --build . --target install
  ```
  Where:
  - `LICENSE_MANAGER`, `DECRYPTOR`, and `OBFUSCATOR` must be set, default implementations are:
    - `OBFUSCATOR`: `src/obfuscators/dummy`
    - `DECRYPTOR`: `src/decryptors/default`
    - `LICENSE_MANAGER`: `src/license_managers/testingdummy`
  - Create a new `openssl_keys` directory by executing the commands:
    ```
    mkdir openssl_keys
    cd openssl_keys
    openssl genrsa -out "private_key_tool.pem" 4096
    dos2unix "private_key_tool.pem"
    openssl genrsa -out "private_key_lve.pem" 4096
    dos2unix "private_key_lve.pem"
    openssl rsa -pubout -in "private_key_tool.pem" -out "public_key_tool.pem"
    dos2unix "public_key_tool.pem"
    echo public_key_tool.pem > public_key_tools.txt 
    ```
  - The `openssl_keys` directory then contains:
    ```
    private_key_lve.pem
    private_key_tool.pem
    public_key_tool.pem
    public_key_tools.txt
    ```

- To run the unit tests, execute the command:
  ```
  ctest -C Release
  ```

# Notes 
When running the tool library test executable `test_tool` (test_tool.c):
- If you want to fetch an encrypted file, that file must be encrypted with the latest `encrypt_file` executable that uses the same random key as LVE to encrypt/decrypt files.
- If you get an error saying "Error: SSL: Failed to create client CTX structure." when you run `test_tool` this means
   that the size of the tool's private key (in its c-file) is different from the extern statement in source file (mlle_licensing.c).
