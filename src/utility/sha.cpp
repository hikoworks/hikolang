
#include "sha.hpp"
#include <openssl/sha.h>

namespace hk {

[[nodiscard]] std::array<char, 32> sha256(std::string_view str)
{
    auto r = std::array<char, SHA256_DIGEST_LENGTH>{};
    auto r_ptr = reinterpret_cast<unsigned char *>(r.data());
    auto str_ptr = reinterpret_cast<unsigned char const *>(str.data());

    ::SHA256(str_ptr, str.size(), r_ptr);
    return r;
}


}