/*
 * binary_indexed_tree.hpp
 * Created on 2025.04.20 by RZIN
 * Edited on 2025.04.20 by RZIN
 */
#pragma once
#include <bit>
#include <vector>
#include <iterator>
#include <concepts>
namespace cha
{



/// @brief 树状数组（Binary Indexed Tree）
/// @tparam `T` 计算的数据类型
template <typename T>
    requires std::signed_integral<T>
class BinaryIndexedTree
{
public:
    /// @brief Constructor
    /// @param `n` 原数组长度
    BinaryIndexedTree(const std::size_t n)
        : va_(n + 1, 0) {}
    
    /// @brief Constructor
    /// @param `first` 原数组的起始迭代器
    /// @param `last` 原数组的结束迭代器
    /// @details 对原数组进行预处理，时间复杂度为 O(n)
    template <typename ForwardIt>
        requires std::forward_iterator<ForwardIt>
    BinaryIndexedTree(ForwardIt first, ForwardIt last)
        : BinaryIndexedTree(std::distance(first, last), [&](const int _) { return *first++; }) {}
    
    /// @brief Constructor
    /// @param `n` 原数组长度
    /// @param `f` 原数组的初始化函数，参数为下标，返回值为原数组元素
    /// @details 对原数组进行预处理，时间复杂度为 O(n)
    template <typename Function>
        requires std::invocable<Function, int>
              && std::same_as<std::invoke_result_t<Function, int>, T>
    BinaryIndexedTree(const std::size_t n, Function&& f) {
        va_.resize(n + 1, 0);
        for (int i = 1; i < va_.size(); ++i) {
            va_[i] += f(i - 1);
            if (int j = i + i & -i; j < va_.size()) {
                va_[j] += va_[i];
            }
        }
    }
    
    /// @brief 直接访问树状数组元素
    /// @param `i` 从 1 开始的下标
    /// @return 树状数组元素，等价于原数组 `[i-(i&-i)...i-1]` 的区间和
    inline T operator[](const int i) const noexcept {
        return va_[i];
    }

    /// @brief 单点加
    /// @param `i` 从 1 开始的下标
    /// @param `x` 增加的值
    /// @details 该操作的时间复杂度为 O(log n)
    inline void add(int i, const T x) noexcept {
        while (i < va_.size()) va_[i] += x, i += i & -i;
    }

    /// @brief 区间和
    /// @param `i` 从 1 开始的下标
    /// @return 原数组 `[0...i-1]` 的区间和
    /// @details 该操作的时间复杂度为 O(log n)
    inline T get(const int i) const noexcept {
        return i == 0 ? 0 : va_[i] + get(i - (i & -i));
    }

    /// @brief 从指定长度前缀数组中查找前缀和小于给定值的最大下标
    /// @param `x` 给定值
    /// @param `n` 前缀数组长度
    /// @return 前缀和小于 `x` 的最大下标，从 1 开始
    /// @details 该操作的时间复杂度为 O(log n)
    inline int lower(const T x, const std::size_t n) const noexcept {
        T sum = 0;
        int k = std::bit_floor(n), i = 0;
        for (; k; k >>= 1)
            if (i + k <= n && sum + va_[i + k] < x)
                sum += va_[i += k];
        return i;
    }

private:
    std::vector<T> va_;
};



} // namespace cha