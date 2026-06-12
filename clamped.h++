/* Arithmetic template who's value is
 *  always clamped into range ON STORE
 *  and converts implicitly.
 */
#ifndef CLAMPED_HPP
#define CLAMPED_HPP

template<
    typename T,
    T min_value,
    T max_value
>
class clamped_t {
    static constexpr
    T clamp(T value) {
        if (value < min_value) {
            return min_value;
        }
        if (value > max_value) {
            return max_value;
        }
        return value;
    }

    T value_;

  public:
    clamped_t(T value = min_value)
        : value_(clamp(value)) {}

    operator T() const {
        return value_;
    }

    clamped_t& operator=(T value) {
        value_ = clamp(value);
        return *this;
    }

    clamped_t& operator+=(T rhs) {
        value_ = clamp(value_ + rhs);
        return *this;
    }

    clamped_t& operator-=(T rhs) {
        value_ = clamp(value_ - rhs);
        return *this;
    }
};

#endif
