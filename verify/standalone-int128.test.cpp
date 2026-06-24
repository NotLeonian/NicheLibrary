// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <limits>

#include "../internal/int128.hpp"

int main() {
    using NicheLibrary::Int128;
    using NicheLibrary::UInt128;

    static_assert(std::numeric_limits<UInt128>::is_integer);
    static_assert(!std::numeric_limits<UInt128>::is_signed);
    static_assert(std::numeric_limits<UInt128>::digits == 128);

    static_assert(std::numeric_limits<Int128>::is_integer);
    static_assert(std::numeric_limits<Int128>::is_signed);
    static_assert(std::numeric_limits<Int128>::digits == 127);

    const UInt128 two_64 = UInt128::from_words(1, 0);
    assert(two_64 / UInt128(3) == UInt128(6148914691236517205ULL));
    assert(two_64 % UInt128(3) == UInt128(1));

    const UInt128 max_u64 = UInt128::from_words(0, ~std::uint64_t{});
    assert(max_u64 / UInt128(3) == UInt128(6148914691236517205ULL));
    assert(max_u64 % UInt128(3) == UInt128(0));

    assert(Int128(-7) / Int128(3) == Int128(-2));
    assert(Int128(-7) % Int128(3) == Int128(-1));
    assert(Int128(7) / Int128(-3) == Int128(-2));
    assert(Int128(7) % Int128(-3) == Int128(1));

    const Int128 big = Int128::from_words(0, ~std::uint64_t{});
    assert(big > Int128(0));
    assert(big / Int128(3) == Int128(6148914691236517205ULL));
    assert(big % Int128(3) == Int128(0));

    return 0;
}
