#pragma once

#include "types.hpp"

namespace turinged {
namespace core {

int64 modq(int64 x, int64 q);

int64 center_rep(int64 x, int64 q);

int64 dot_product_modq(const std::vector<int64>& a, const std::vector<int64>& b, int64 q);

}
}