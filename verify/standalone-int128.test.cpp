// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <limits>
#include <sstream>

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

    {
        const UInt128 x = UInt128::from_words(1, 0);
        UInt128 q;
        UInt128 r;
        UInt128::div_mod(x, UInt128(3), q, r);

        assert(q == UInt128(6148914691236517205ULL));
        assert(r == UInt128(1));
    }

    {
        const UInt128 x = UInt128::from_words(0, ~std::uint64_t{});
        UInt128 q;
        UInt128 r;
        UInt128::div_mod(x, UInt128(1000000000), q, r);

        assert(q == UInt128(18446744073ULL));
        assert(r == UInt128(709551615U));
    }

    {
        std::stringstream stream;
        stream << UInt128::from_words(0, ~std::uint64_t{});
        assert(stream.str() == "18446744073709551615");
    }

    {
        std::stringstream stream;
        stream << Int128::from_words(~std::uint64_t{}, 0);
        assert(stream.str() == "-18446744073709551616");
    }

    {
        std::stringstream stream("170141183460469231731687303715884105727");
        Int128 value;
        stream >> value;
        assert(value == std::numeric_limits<Int128>::max());
    }

    return 0;
}
