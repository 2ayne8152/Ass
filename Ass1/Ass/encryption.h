#pragma once
#include <string>
#define OPENSSL_API_COMPAT 0x10100000L
#include <openssl/sha.h>  // classic SHA API
using namespace std;

class SHA256 {
public:
    static string hash(const string& input);
};