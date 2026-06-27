// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <limits>
#include <sstream>

#include "../internal/int128.hpp"

namespace {
using NicheLibrary::Int128;
using NicheLibrary::UInt128;

struct UInt128DivModResult {
    UInt128 quotient;
    UInt128 remainder;
};

constexpr UInt128 to_unsigned(Int128 value) {
    return UInt128::from_words(value.high(), value.low());
}

constexpr UInt128 abs_unsigned(Int128 value) {
    if (!value.is_negative()) {
        return to_unsigned(value);
    }
    return -to_unsigned(value);
}

constexpr bool get_bit(UInt128 value, int index) {
    if (index < 64) {
        return ((value.low() >> index) & 1) != 0;
    }
    return ((value.high() >> (index - 64)) & 1) != 0;
}

constexpr UInt128 bit_value(int index) {
    if (index < 64) {
        return UInt128::from_words(0, std::uint64_t{1} << index);
    }
    return UInt128::from_words(std::uint64_t{1} << (index - 64), 0);
}

constexpr UInt128 shift_left_one(UInt128 value) {
    return UInt128::from_words((value.high() * 2) + (value.low() >> 63),
                               value.low() * 2);
}

constexpr UInt128DivModResult reference_div_mod(UInt128 lhs, UInt128 rhs) {
    assert(rhs != UInt128{});
    UInt128 quotient;
    UInt128 remainder;
    for (int bit = 127; bit >= 0; bit -= 1) {
        remainder = shift_left_one(remainder);
        if (get_bit(lhs, bit)) {
            remainder += UInt128(1);
        }
        if (remainder >= rhs) {
            remainder -= rhs;
            quotient += bit_value(bit);
        }
    }
    return {quotient, remainder};
}

void test_unsigned_division(UInt128 lhs, UInt128 rhs) {
    if (rhs == UInt128{}) {
        return;
    }
    const UInt128DivModResult expected = reference_div_mod(lhs, rhs);

    UInt128 quotient;
    UInt128 remainder;
    UInt128::div_mod(lhs, rhs, quotient, remainder);
    assert(quotient == expected.quotient);
    assert(remainder == expected.remainder);
    assert(lhs / rhs == expected.quotient);
    assert(lhs % rhs == expected.remainder);
    assert(quotient * rhs + remainder == lhs);
    assert(remainder < rhs);

    UInt128 quotient_alias = lhs;
    UInt128 remainder_alias;
    UInt128::div_mod(quotient_alias, rhs, quotient_alias, remainder_alias);
    assert(quotient_alias == expected.quotient);
    assert(remainder_alias == expected.remainder);

    UInt128 rhs_alias = rhs;
    UInt128::div_mod(lhs, rhs_alias, rhs_alias, remainder_alias);
    assert(rhs_alias == expected.quotient);
    assert(remainder_alias == expected.remainder);

    UInt128 lhs_alias = lhs;
    UInt128::div_mod(lhs_alias, rhs, quotient_alias, lhs_alias);
    assert(quotient_alias == expected.quotient);
    assert(lhs_alias == expected.remainder);
}

void test_signed_division(Int128 lhs, Int128 rhs) {
    if (rhs == Int128{}) {
        return;
    }
    if (lhs == std::numeric_limits<Int128>::min() && rhs == Int128(-1)) {
        return;
    }

    Int128 quotient;
    Int128 remainder;
    Int128::div_mod(lhs, rhs, quotient, remainder);
    assert(lhs / rhs == quotient);
    assert(lhs % rhs == remainder);
    assert(quotient * rhs + remainder == lhs);
    assert(abs_unsigned(remainder) < abs_unsigned(rhs));
    if (remainder != Int128{}) {
        assert(remainder.is_negative() == lhs.is_negative());
    }
}
} // namespace

int main() {
    using NicheLibrary::Int128;
    using NicheLibrary::UInt128;

    static_assert(std::numeric_limits<UInt128>::is_integer,
                  "UInt128 must be integer.");
    static_assert(!std::numeric_limits<UInt128>::is_signed,
                  "UInt128 must be unsigned.");
    static_assert(std::numeric_limits<UInt128>::digits == 128,
                  "UInt128 must have 128 value bits.");

    static_assert(std::numeric_limits<Int128>::is_integer,
                  "Int128 must be integer.");
    static_assert(std::numeric_limits<Int128>::is_signed,
                  "Int128 must be signed.");
    static_assert(std::numeric_limits<Int128>::digits == 127,
                  "Int128 must have 127 value bits.");

    const UInt128 unsigned_values[] = {
        UInt128(0),
        UInt128(1),
        UInt128(2),
        UInt128(3),
        UInt128::from_words(0, (std::uint64_t{1} << 32) - 1),
        UInt128::from_words(0, std::uint64_t{1} << 32),
        UInt128::from_words(0, (std::uint64_t{1} << 32) + 1),
        UInt128::from_words(0, (std::uint64_t{1} << 63) - 1),
        UInt128::from_words(0, std::uint64_t{1} << 63),
        UInt128::from_words(0, ~std::uint64_t{}),
        UInt128::from_words(1, 0),
        UInt128::from_words(1, 1),
        UInt128::from_words((std::uint64_t{1} << 32) - 1, ~std::uint64_t{}),
        UInt128::from_words(std::uint64_t{1} << 32, 0),
        UInt128::from_words(std::uint64_t{1} << 63, 0),
        UInt128::from_words((std::uint64_t{1} << 63) + 123, 456),
        UInt128::from_words(0x123456789abcdef0ULL, 0xfedcba9876543210ULL),
        UInt128::from_words(0xfedcba9876543210ULL, 0x123456789abcdef0ULL),
        UInt128::from_words(~std::uint64_t{}, ~std::uint64_t{}),
    };

    for (const UInt128 lhs : unsigned_values) {
        for (const UInt128 rhs : unsigned_values) {
            test_unsigned_division(lhs, rhs);
        }
    }

    const Int128 signed_values[] = {
        Int128(0),
        Int128(1),
        Int128(-1),
        Int128(2),
        Int128(-2),
        Int128(3),
        Int128(-3),
        Int128::from_words(0, std::uint64_t{1} << 63),
        Int128::from_words(~std::uint64_t{}, std::uint64_t{1} << 63),
        Int128::from_words(0x123456789abcdef0ULL, 0xfedcba9876543210ULL),
        Int128::from_words(0xfedcba9876543210ULL, 0x123456789abcdef0ULL),
        std::numeric_limits<Int128>::max(),
        std::numeric_limits<Int128>::min(),
    };

    for (const Int128 lhs : signed_values) {
        for (const Int128 rhs : signed_values) {
            test_signed_division(lhs, rhs);
        }
    }

    {
        std::stringstream stream;
        stream << UInt128::from_words(~std::uint64_t{}, ~std::uint64_t{});
        assert(stream.str() == "340282366920938463463374607431768211455");
    }

    {
        std::stringstream stream;
        stream << Int128::from_words(std::uint64_t{1} << 63, 0);
        assert(stream.str() == "-170141183460469231731687303715884105728");
    }

    {
        std::stringstream stream("170141183460469231731687303715884105727");
        Int128 value;
        stream >> value;
        assert(value == std::numeric_limits<Int128>::max());
    }

    {
        std::stringstream stream("340282366920938463463374607431768211455");
        UInt128 value;
        stream >> value;
        assert(value == std::numeric_limits<UInt128>::max());
    }

    return 0;
}
