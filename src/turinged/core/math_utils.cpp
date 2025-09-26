#include "turinged/core/math_utils.hpp"
#include <stdexcept>

namespace turinged {
namespace core {

int64 modq(int64 x, int64 q) {
    int64 r = x % q;
    if (r < 0) r += q;
    return r;
}

int64 center_rep(int64 x, int64 q) {
    int64 v = modq(x, q);
    int64 half = q / 2;
    if (v > half) v -= q;
    return v;
}

int64 dot_product_modq(const std::vector<int64>& a, const std::vector<int64>& b, int64 q) {
    if (a.size() != b.size()) {
        throw std::runtime_error("Vector size mismatch in dot product");
    }

    int128 acc = 0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        acc += static_cast<int128>(a[i]) * static_cast<int128>(b[i]);
    }

    int64 acc64 = static_cast<int64>(acc % q);
    if (acc64 < 0) acc64 += q;
    return acc64;
}

}
}