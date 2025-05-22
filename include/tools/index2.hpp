/*
 * index2.hpp
 * Created on 2025.03.01 by RZIN
 * Edited on 2025.04.15 by RZIN
 */
#pragma once
#include <cstddef>
#include <functional>
namespace cha
{



template <typename T>
struct Template_Index2
{
    T y{};
    T x{};
    
    constexpr Template_Index2() noexcept = default;
    constexpr Template_Index2(const Template_Index2& other) noexcept = default;
    constexpr Template_Index2& operator=(const Template_Index2& other) noexcept = default;

    constexpr Template_Index2(T y, T x) noexcept : y(y), x(x) {}
    constexpr explicit Template_Index2(T idx) noexcept : y(idx), x(idx) {}

    [[nodiscard]] static constexpr Template_Index2 fromIndex(T idx, T width) {
        return {idx / width, idx % width};
    }

    [[nodiscard]] constexpr T toIndex(T width) const noexcept {
        return y * width + x;
    }

    [[nodiscard]] constexpr T operator*() const noexcept {
        return x * y;
    }

    [[nodiscard]] constexpr Template_Index2 operator-() const noexcept {
        return {-y, -x};
    }

    [[nodiscard]] constexpr Template_Index2 operator+(const Template_Index2& other) const noexcept {
        return {y + other.y, x + other.x};
    }

    [[nodiscard]] constexpr Template_Index2 operator-(const Template_Index2& other) const noexcept {
        return {y - other.y, x - other.x};
    }
    
    [[nodiscard]] constexpr Template_Index2 operator*(const Template_Index2& other) const noexcept {
        return {y * other.y, x * other.x};
    }

    [[nodiscard]] constexpr Template_Index2 operator/(const Template_Index2& other) const {
        return {y / other.y, x / other.x};
    }

    [[nodiscard]] constexpr Template_Index2 operator+(T scalar) const noexcept {
        return {y + scalar, x + scalar};
    }

    [[nodiscard]] constexpr Template_Index2 operator-(T scalar) const noexcept {
        return {y - scalar, x - scalar};
    }

    [[nodiscard]] constexpr Template_Index2 operator*(T scalar) const noexcept {
        return {y * scalar, x * scalar};
    }

    [[nodiscard]] constexpr Template_Index2 operator/(T scalar) const {
        return {y / scalar, x / scalar};
    }

    constexpr Template_Index2& operator+=(const Template_Index2& other) noexcept {
        y += other.y;
        x += other.x;
        return *this;
    }

    constexpr Template_Index2& operator-=(const Template_Index2& other) noexcept {
        y -= other.y;
        x -= other.x;
        return *this;
    }

    constexpr Template_Index2& operator*=(const Template_Index2& other) noexcept {
        y *= other.y;
        x *= other.x;
        return *this;
    }

    constexpr Template_Index2& operator/=(const Template_Index2& other) {
        y /= other.y;
        x /= other.x;
        return *this;
    }

    constexpr Template_Index2& operator+=(T scalar) noexcept {
        y += scalar;
        x += scalar;
        return *this;
    }

    constexpr Template_Index2& operator-=(T scalar) noexcept {
        y -= scalar;
        x -= scalar;
        return *this;
    }

    constexpr Template_Index2& operator*=(T scalar) noexcept {
        y *= scalar;
        x *= scalar;
        return *this;
    }

    constexpr Template_Index2& operator/=(T scalar) {
        y /= scalar;
        x /= scalar;
        return *this;
    }

    [[nodiscard]] constexpr bool operator==(const Template_Index2& other) const noexcept {
        return y == other.y && x == other.x;
    }

    [[nodiscard]] constexpr bool operator!=(const Template_Index2& other) const noexcept {
        return !(*this == other);
    }

    [[nodiscard]] constexpr bool operator<(const Template_Index2& other) const noexcept {
        return (y == other.y) & (x < other.x) | (y < other.y);
    }

    [[nodiscard]] constexpr bool strictLess(const Template_Index2& other) const noexcept {
        return (y < other.y) & (x < other.x);
    }

    [[nodiscard]] constexpr bool strictLessEqual(const Template_Index2& other) const noexcept {
        return (y <= other.y) & (x <= other.x);
    }

    struct Range
    {
        Template_Index2 tl;
        Template_Index2 br;

        constexpr Range(const Range& other) noexcept = default;
        constexpr Range& operator=(const Range& other) noexcept = default;
        constexpr Range(const Template_Index2& begin, const Template_Index2& end) noexcept : tl(begin), br(end) {}
        constexpr explicit Range(const Template_Index2& end) noexcept : tl(0), br(end) {}

        [[nodiscard]] constexpr bool contains(const Template_Index2& index) const noexcept {
            return tl.strictLessEqual(index) & index.strictLess(br);
        }

        [[nodiscard]] constexpr bool operator==(const Range& other) const noexcept {
            return tl == other.tl && br == other.br;
        }

        [[nodiscard]] constexpr bool operator!=(const Range& other) const noexcept {
            return !(*this == other);
        }

        struct Iterator
        {
            using value_type = Template_Index2;

            Range range;
            Template_Index2 index;

            constexpr Iterator() noexcept : range({}) {}
            constexpr explicit Iterator(Range range) noexcept : range(range), index(range.tl) {}

            constexpr Iterator& operator++() noexcept {
                ++index.x;
                if (index.x == range.br.x) {
                    index.x = range.tl.x;
                    ++index.y;
                }
                return *this;
            }

            constexpr Iterator operator++(int) noexcept {
                Iterator temp = *this;
                ++*this;
                return temp;
            }

            [[nodiscard]] constexpr value_type operator*() const noexcept {
                return index;
            }

            [[nodiscard]] constexpr bool operator==([[maybe_unused]] const Iterator& other) const noexcept {
                return index.y >= range.br.y;
            }

            [[nodiscard]] constexpr bool operator!=(const Iterator& other) const noexcept {
                return !(*this == other);
            }
        };

        [[nodiscard]] constexpr Iterator begin() const noexcept {
            return Iterator(*this);
        }

        [[nodiscard]] constexpr Iterator end() const noexcept {
            return Iterator();
        }
    };
};



template <typename T>
[[nodiscard]] inline constexpr Template_Index2<T> operator*(T scalar, const Template_Index2<T> index) noexcept {
    return index * scalar;
}



using Int2 = Template_Index2<int>;



inline constexpr Int2 DIR9[9] = {
    {-1,-1}, {-1, 0}, {-1, 1},
    { 0,-1}, { 0, 0}, { 0, 1},
    { 1,-1}, { 1, 0}, { 1, 1}
};



inline constexpr Int2 DIR8[8] = {
    {-1,-1}, {-1, 0}, {-1, 1},
    { 0,-1},          { 0, 1},
    { 1,-1}, { 1, 0}, { 1, 1}
};



inline constexpr Int2 DIR5[5] = {
             {-1, 0},
    { 0,-1}, { 0, 0},  { 0, 1},
             { 1, 0}
};



inline constexpr Int2 DIR4[4] = {
             {-1, 0},
    { 0,-1},          { 0, 1},
             { 1, 0}
};



inline constexpr Int2 DIRX[5] = {
    {-1,-1},          {-1, 1},
             { 0, 0},
    { 1,-1},          { 1, 1}
};



inline constexpr Int2 DIRx[4] = {
    {-1, -1},         {-1, 1},
    
    { 1, -1},         { 1, 1}
};



} // namespace cha



namespace std
{
    template <typename T>
    struct hash<cha::Template_Index2<T>>
    {
        [[nodiscard]] size_t operator()(const cha::Template_Index2<T>& index) const noexcept {
            return hash<T>()(index.y) ^ hash<T>()(index.x) << 1;
        }
    };
}