#include "encryption.h"
#include <sstream>
#include <iomanip>
#define OPENSSL_API_COMPAT 0x10100000L
#include <openssl/sha.h>  // classic SHA API
using namespace std;

string SHA256::hash(const string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;

    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.size());
    SHA256_Final(hash, &sha256);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}
