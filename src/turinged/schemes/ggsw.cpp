#include "turinged/schemes/ggsw.hpp"
#include "turinged/polynomial/polynomial.hpp"

namespace turinged {
namespace schemes {

GGSWCiphertext encrypt_ggsw(
    const Polynomial& message,
    const keys::GLWEPublicKey& pk,
    const keys::GLWESecretKey& sk,
    const Parameters& params,
    int l,
    int64 beta
) {
    std::size_t k = sk.s.size();
    GGSWCiphertext ggsw_ct(k);

    // Encrypt the first k rows: GLev(-S_i * M)
    for (std::size_t i = 0; i < k; ++i) {
        Polynomial si_m = polynomial::negacyclic_multiply(sk.s[i], message, params.q);
        Polynomial neg_si_m = polynomial::negate(si_m, params.q);
        ggsw_ct.glev_rows[i] = encrypt_glev(neg_si_m, pk, params, l, beta);
    }

    // Encrypt the final row: GLev(M)
    ggsw_ct.glev_rows[k] = encrypt_glev(message, pk, params, l, beta);

    return ggsw_ct;
}

Polynomial decrypt_ggsw(
    const GGSWCiphertext& ct,
    const keys::GLWESecretKey& sk,
    const Parameters& params,
    int level_idx,
    int64 beta
) {
    // Decrypt using the last GLev row, which encrypts M
    const GLevCiphertext& final_glev_row = ct.glev_rows.back();
    return decrypt_glev_level(final_glev_row, sk, params, level_idx, beta);
}

}
}