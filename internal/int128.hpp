#ifndef INTERNAL_INT128_HPP
#define INTERNAL_INT128_HPP

// 128 bit 符号なし整数型 UInt128 と 128 bit 符号付き整数型 Int128 を提供する。
// GCC 拡張には依存しない。

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>

namespace NicheLibrary {
class UInt128 {
  public:
    constexpr UInt128() = default;

    template <
        class T,
        std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, int> = 0>
    constexpr UInt128(T value)
        : high_(value < 0 ? ~std::uint64_t{} : std::uint64_t{}),
          low_(static_cast<std::uint64_t>(value)) {}

    template <class T,
              std::enable_if_t<std::is_integral_v<T> && !std::is_signed_v<T>,
                               int> = 0>
    constexpr UInt128(T value)
        : high_(0), low_(static_cast<std::uint64_t>(value)) {}

    static constexpr UInt128 from_words(std::uint64_t high, std::uint64_t low) {
        UInt128 value;
        value.high_ = high;
        value.low_ = low;
        return value;
    }

    constexpr std::uint64_t high() const { return high_; }
    constexpr std::uint64_t low() const { return low_; }

    template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    explicit constexpr operator T() const {
        return static_cast<T>(low_);
    }

    explicit constexpr operator bool() const { return high_ != 0 || low_ != 0; }

    explicit constexpr operator long double() const {
        return static_cast<long double>(high_) * 0x1p64L +
               static_cast<long double>(low_);
    }

    friend constexpr bool operator==(UInt128 lhs, UInt128 rhs) {
        return lhs.high_ == rhs.high_ && lhs.low_ == rhs.low_;
    }

    friend constexpr bool operator!=(UInt128 lhs, UInt128 rhs) {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(UInt128 lhs, UInt128 rhs) {
        if (lhs.high_ != rhs.high_) {
            return lhs.high_ < rhs.high_;
        }
        return lhs.low_ < rhs.low_;
    }

    friend constexpr bool operator>(UInt128 lhs, UInt128 rhs) {
        return rhs < lhs;
    }

    friend constexpr bool operator<=(UInt128 lhs, UInt128 rhs) {
        return !(rhs < lhs);
    }

    friend constexpr bool operator>=(UInt128 lhs, UInt128 rhs) {
        return !(lhs < rhs);
    }

    constexpr UInt128 operator+() const { return *this; }

    constexpr UInt128 operator-() const { return UInt128{} - *this; }

    constexpr UInt128 &operator+=(UInt128 rhs) {
        const std::uint64_t old_low = low_;
        low_ += rhs.low_;
        high_ += rhs.high_ + (low_ < old_low ? 1 : 0);
        return *this;
    }

    constexpr UInt128 &operator-=(UInt128 rhs) {
        const std::uint64_t old_low = low_;
        low_ -= rhs.low_;
        high_ -= rhs.high_ + (old_low < rhs.low_ ? 1 : 0);
        return *this;
    }

    constexpr UInt128 &operator*=(UInt128 rhs) {
        *this = *this * rhs;
        return *this;
    }

    constexpr UInt128 &operator/=(UInt128 rhs) {
        *this = *this / rhs;
        return *this;
    }

    constexpr UInt128 &operator%=(UInt128 rhs) {
        *this = *this % rhs;
        return *this;
    }

    friend constexpr UInt128 operator+(UInt128 lhs, UInt128 rhs) {
        lhs += rhs;
        return lhs;
    }

    friend constexpr UInt128 operator-(UInt128 lhs, UInt128 rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend constexpr UInt128 operator*(UInt128 lhs, UInt128 rhs) {
        const UInt128 p00 = multiply_u64(lhs.low_, rhs.low_);
        return from_words(
            p00.high_ + lhs.low_ * rhs.high_ + lhs.high_ * rhs.low_, p00.low_);
    }

    static constexpr void div_mod(UInt128 lhs, UInt128 rhs, UInt128 &quotient,
                                  UInt128 &remainder) {
        assert(rhs != UInt128{});

        if (rhs.high_ == 0 &&
            rhs.low_ <= std::numeric_limits<std::uint32_t>::max()) {
            std::uint32_t rem = 0;
            quotient =
                div_mod_uint32(lhs, static_cast<std::uint32_t>(rhs.low_), rem);
            remainder = UInt128(rem);
            return;
        }

        div_mod_binary(lhs, rhs, quotient, remainder);
    }

    friend constexpr UInt128 operator/(UInt128 lhs, UInt128 rhs) {
        UInt128 quotient;
        UInt128 remainder;
        div_mod(lhs, rhs, quotient, remainder);
        return quotient;
    }

    friend constexpr UInt128 operator%(UInt128 lhs, UInt128 rhs) {
        UInt128 quotient;
        UInt128 remainder;
        div_mod(lhs, rhs, quotient, remainder);
        return remainder;
    }

  private:
    static constexpr UInt128 div_mod_uint32(UInt128 value,
                                            std::uint32_t divisor,
                                            std::uint32_t &remainder) {
        assert(divisor != 0);

        constexpr std::uint64_t mask = (std::uint64_t{1} << 32) - 1;
        const std::uint32_t words[4] = {
            static_cast<std::uint32_t>(value.high() >> 32),
            static_cast<std::uint32_t>(value.high() & mask),
            static_cast<std::uint32_t>(value.low() >> 32),
            static_cast<std::uint32_t>(value.low() & mask),
        };

        std::uint32_t quotient_words[4] = {};
        std::uint64_t rem = 0;
        for (int i = 0; i < 4; ++i) {
            const std::uint64_t current = (rem << 32) | words[i];
            quotient_words[i] = static_cast<std::uint32_t>(current / divisor);
            rem = current % divisor;
        }

        remainder = static_cast<std::uint32_t>(rem);
        return UInt128::from_words(
            (static_cast<std::uint64_t>(quotient_words[0]) << 32) |
                quotient_words[1],
            (static_cast<std::uint64_t>(quotient_words[2]) << 32) |
                quotient_words[3]);
    }

    static constexpr void div_mod_binary(UInt128 lhs, UInt128 rhs,
                                         UInt128 &quotient,
                                         UInt128 &remainder) {
        assert(rhs != UInt128{});

        quotient = UInt128{};
        remainder = UInt128{};
        for (int bit = 127; bit >= 0; --bit) {
            shift_left_one(remainder);
            if (get_bit(lhs, bit)) {
                remainder.low_ |= 1;
            }
            if (remainder >= rhs) {
                remainder -= rhs;
                set_bit(quotient, bit);
            }
        }
    }

    static constexpr bool get_bit(UInt128 value, int index) {
        if (index < 64) {
            return ((value.low_ >> index) & 1) != 0;
        }
        return ((value.high_ >> (index - 64)) & 1) != 0;
    }

    static constexpr void set_bit(UInt128 &value, int index) {
        if (index < 64) {
            value.low_ |= std::uint64_t{1} << index;
        } else {
            value.high_ |= std::uint64_t{1} << (index - 64);
        }
    }

    static constexpr void shift_left_one(UInt128 &value) {
        value.high_ = (value.high_ << 1) | (value.low_ >> 63);
        value.low_ <<= 1;
    }

    static constexpr UInt128 multiply_u64(std::uint64_t lhs,
                                          std::uint64_t rhs) {
        constexpr std::uint64_t mask = (std::uint64_t{1} << 32) - 1;
        const std::uint64_t lhs_low = lhs & mask;
        const std::uint64_t lhs_high = lhs >> 32;
        const std::uint64_t rhs_low = rhs & mask;
        const std::uint64_t rhs_high = rhs >> 32;

        const std::uint64_t p00 = lhs_low * rhs_low;
        const std::uint64_t p01 = lhs_low * rhs_high;
        const std::uint64_t p10 = lhs_high * rhs_low;
        const std::uint64_t p11 = lhs_high * rhs_high;

        const std::uint64_t middle = (p00 >> 32) + (p01 & mask) + (p10 & mask);
        return from_words(p11 + (p01 >> 32) + (p10 >> 32) + (middle >> 32),
                          (middle << 32) | (p00 & mask));
    }

    std::uint64_t high_ = 0;
    std::uint64_t low_ = 0;
};

class Int128 {
  public:
    constexpr Int128() = default;

    template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    constexpr Int128(T value) : value_(value) {}

    static constexpr Int128 from_words(std::uint64_t high, std::uint64_t low) {
        Int128 value;
        value.value_ = UInt128::from_words(high, low);
        return value;
    }

    constexpr std::uint64_t high() const { return value_.high(); }
    constexpr std::uint64_t low() const { return value_.low(); }

    template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    explicit constexpr operator T() const {
        return static_cast<T>(value_.low());
    }

    explicit constexpr operator bool() const { return value_ != UInt128{}; }

    constexpr bool is_negative() const { return (value_.high() >> 63) != 0; }

    explicit constexpr operator long double() const {
        const long double magnitude =
            static_cast<long double>(abs_unsigned(*this));
        return is_negative() ? -magnitude : magnitude;
    }

    friend constexpr bool operator==(Int128 lhs, Int128 rhs) {
        return lhs.value_ == rhs.value_;
    }

    friend constexpr bool operator!=(Int128 lhs, Int128 rhs) {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(Int128 lhs, Int128 rhs) {
        const bool lhs_negative = lhs.is_negative();
        const bool rhs_negative = rhs.is_negative();
        if (lhs_negative != rhs_negative) {
            return lhs_negative;
        }
        return lhs.value_ < rhs.value_;
    }

    friend constexpr bool operator>(Int128 lhs, Int128 rhs) {
        return rhs < lhs;
    }

    friend constexpr bool operator<=(Int128 lhs, Int128 rhs) {
        return !(rhs < lhs);
    }

    friend constexpr bool operator>=(Int128 lhs, Int128 rhs) {
        return !(lhs < rhs);
    }

    constexpr Int128 operator+() const { return *this; }

    constexpr Int128 operator-() const {
        Int128 value;
        value.value_ = -value_;
        return value;
    }

    constexpr Int128 &operator+=(Int128 rhs) {
        value_ += rhs.value_;
        return *this;
    }

    constexpr Int128 &operator-=(Int128 rhs) {
        value_ -= rhs.value_;
        return *this;
    }

    constexpr Int128 &operator*=(Int128 rhs) {
        value_ *= rhs.value_;
        return *this;
    }

    constexpr Int128 &operator/=(Int128 rhs) {
        *this = *this / rhs;
        return *this;
    }

    constexpr Int128 &operator%=(Int128 rhs) {
        *this = *this % rhs;
        return *this;
    }

    friend constexpr Int128 operator+(Int128 lhs, Int128 rhs) {
        lhs += rhs;
        return lhs;
    }

    friend constexpr Int128 operator-(Int128 lhs, Int128 rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend constexpr Int128 operator*(Int128 lhs, Int128 rhs) {
        lhs *= rhs;
        return lhs;
    }

    static constexpr void div_mod(Int128 lhs, Int128 rhs, Int128 &quotient,
                                  Int128 &remainder) {
        assert(rhs != Int128{});
        assert(!(lhs == min_value() && rhs == Int128(-1)));

        const bool quotient_negative = lhs.is_negative() != rhs.is_negative();
        const bool remainder_negative = lhs.is_negative();

        UInt128 quotient_abs;
        UInt128 remainder_abs;
        UInt128::div_mod(abs_unsigned(lhs), abs_unsigned(rhs), quotient_abs,
                         remainder_abs);

        quotient = from_unsigned(quotient_abs, quotient_negative);
        remainder = from_unsigned(remainder_abs, remainder_negative);
    }

    friend constexpr Int128 operator/(Int128 lhs, Int128 rhs) {
        Int128 quotient;
        Int128 remainder;
        div_mod(lhs, rhs, quotient, remainder);
        return quotient;
    }

    friend constexpr Int128 operator%(Int128 lhs, Int128 rhs) {
        Int128 quotient;
        Int128 remainder;
        div_mod(lhs, rhs, quotient, remainder);
        return remainder;
    }

  private:
    static constexpr Int128 from_twos_complement(UInt128 value) {
        Int128 result;
        result.value_ = value;
        return result;
    }

    static constexpr Int128 min_value() {
        return from_words(std::uint64_t{1} << 63, 0);
    }

    static constexpr UInt128 abs_unsigned(Int128 value) {
        if (!value.is_negative()) {
            return value.value_;
        }
        return -value.value_;
    }

    static constexpr Int128 from_unsigned(UInt128 value, bool negative) {
        if (!negative) {
            assert((value.high() >> 63) == 0);
            return from_twos_complement(value);
        }
        assert(value <= UInt128::from_words(std::uint64_t{1} << 63, 0));
        return from_twos_complement(-value);
    }

    friend std::istream &operator>>(std::istream &input, Int128 &value);
    friend std::ostream &operator<<(std::ostream &output, Int128 value);

    UInt128 value_;
};

namespace int128_internal {
inline UInt128 read_uint128_decimal(const std::string &text,
                                    std::size_t first) {
    assert(first < text.size());

    UInt128 value = 0;

    std::size_t i = first;
    const std::size_t first_len = (text.size() - first) % 9;
    if (first_len != 0) {
        std::uint32_t chunk = 0;
        for (std::size_t j = 0; j < first_len; ++j) {
            assert('0' <= text[i] && text[i] <= '9');
            chunk = chunk * 10 + static_cast<std::uint32_t>(text[i] - '0');
            ++i;
        }
        value = UInt128(chunk);
    }

    while (i < text.size()) {
        std::uint32_t chunk = 0;
        for (int j = 0; j < 9; ++j) {
            assert('0' <= text[i] && text[i] <= '9');
            chunk = chunk * 10 + static_cast<std::uint32_t>(text[i] - '0');
            ++i;
        }
        value *= UInt128(1000000000);
        value += UInt128(chunk);
    }

    return value;
}

inline void write_padded_9(std::ostream &output, std::uint32_t value) {
    char buffer[9];
    for (int i = 8; i >= 0; --i) {
        buffer[i] = static_cast<char>('0' + value % 10);
        value /= 10;
    }
    output.write(buffer, 9);
}

inline void write_uint128_decimal(std::ostream &output, UInt128 value) {
    if (value == UInt128(0)) {
        output << '0';
        return;
    }

    constexpr std::uint32_t base = 1000000000;
    std::uint32_t parts[5] = {};
    int size = 0;
    while (value != UInt128(0)) {
        UInt128 quotient;
        UInt128 remainder;
        UInt128::div_mod(value, UInt128(base), quotient, remainder);
        parts[size] = static_cast<std::uint32_t>(remainder);
        value = quotient;
        ++size;
    }

    output << parts[size - 1];
    for (int i = size - 2; i >= 0; --i) {
        write_padded_9(output, parts[i]);
    }
}
} // namespace int128_internal

inline std::istream &operator>>(std::istream &input, UInt128 &value) {
    std::string text;
    input >> text;
    if (!input) {
        return input;
    }

    std::size_t first = 0;
    if (text[first] == '+') {
        ++first;
    }
    assert(first < text.size());
    assert(text[first] != '-');

    value = int128_internal::read_uint128_decimal(text, first);
    return input;
}

inline std::ostream &operator<<(std::ostream &output, UInt128 value) {
    int128_internal::write_uint128_decimal(output, value);
    return output;
}

inline std::istream &operator>>(std::istream &input, Int128 &value) {
    std::string text;
    input >> text;
    if (!input) {
        return input;
    }

    std::size_t first = 0;
    bool negative = false;
    if (text[first] == '+' || text[first] == '-') {
        negative = text[first] == '-';
        ++first;
    }
    assert(first < text.size());

    const UInt128 magnitude =
        int128_internal::read_uint128_decimal(text, first);
    value = Int128::from_unsigned(magnitude, negative);
    return input;
}

inline std::ostream &operator<<(std::ostream &output, Int128 value) {
    if (value.is_negative()) {
        output << '-';
        int128_internal::write_uint128_decimal(output,
                                               Int128::abs_unsigned(value));
    } else {
        int128_internal::write_uint128_decimal(
            output, UInt128::from_words(value.high(), value.low()));
    }
    return output;
}
} // namespace NicheLibrary

namespace std {
template <> class numeric_limits<NicheLibrary::UInt128> {
  public:
    static constexpr bool is_specialized = true;

    static constexpr NicheLibrary::UInt128 min() noexcept { return 0; }

    static constexpr NicheLibrary::UInt128 max() noexcept {
        return NicheLibrary::UInt128::from_words(~std::uint64_t{},
                                                 ~std::uint64_t{});
    }

    static constexpr NicheLibrary::UInt128 lowest() noexcept { return 0; }

    static constexpr int digits = 128;
    static constexpr int digits10 = 38;
    static constexpr int max_digits10 = 0;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr int radix = 2;

    static constexpr NicheLibrary::UInt128 epsilon() noexcept { return 0; }

    static constexpr NicheLibrary::UInt128 round_error() noexcept { return 0; }

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;

    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;

    static constexpr NicheLibrary::UInt128 infinity() noexcept { return 0; }

    static constexpr NicheLibrary::UInt128 quiet_NaN() noexcept { return 0; }

    static constexpr NicheLibrary::UInt128 signaling_NaN() noexcept {
        return 0;
    }

    static constexpr NicheLibrary::UInt128 denorm_min() noexcept { return 0; }

    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_zero;
};

template <> class numeric_limits<NicheLibrary::Int128> {
  public:
    static constexpr bool is_specialized = true;

    static constexpr NicheLibrary::Int128 min() noexcept {
        return NicheLibrary::Int128::from_words(std::uint64_t{1} << 63, 0);
    }

    static constexpr NicheLibrary::Int128 max() noexcept {
        return NicheLibrary::Int128::from_words((std::uint64_t{1} << 63) - 1,
                                                ~std::uint64_t{});
    }

    static constexpr NicheLibrary::Int128 lowest() noexcept { return min(); }

    static constexpr int digits = 127;
    static constexpr int digits10 = 38;
    static constexpr int max_digits10 = 0;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr int radix = 2;

    static constexpr NicheLibrary::Int128 epsilon() noexcept { return 0; }

    static constexpr NicheLibrary::Int128 round_error() noexcept { return 0; }

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;

    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;

    static constexpr NicheLibrary::Int128 infinity() noexcept { return 0; }

    static constexpr NicheLibrary::Int128 quiet_NaN() noexcept { return 0; }

    static constexpr NicheLibrary::Int128 signaling_NaN() noexcept { return 0; }

    static constexpr NicheLibrary::Int128 denorm_min() noexcept { return 0; }

    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_zero;
};
} // namespace std

#endif
