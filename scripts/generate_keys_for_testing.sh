
#!/bin/bash
set -euo pipefail

# generate keys for testing
mkdir -p build/openssl_keys
cd openssl_keys
openssl genrsa -out "private_key_tool.pem" 4096
openssl genrsa -out "private_key_lve.pem" 4096
openssl rsa -pubout -in "private_key_tool.pem" -out "public_key_tool.pem"
echo public_key_tool.pem > public_key_tools.txt 
