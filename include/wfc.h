#pragma once
#include <bit>
#include <concepts>
#include <cstdint>
#include <initializer_list>
#include <vector>
#include <map>
#include <unordered_set>
#include <random>
#include <functional>
#include <iterator>
#include "tools/index2.hpp"
#include "tools/matrix.hpp"
#include "tools/generator.hpp"
namespace cha
{



class WaveFunctionCollapse
{
private:
    class Node;

public:
    using FactorType = int;
    using BitsetType = uint32_t;
    using WeightType = int;
    using DiffuseFuncType = std::function<BitsetType(BitsetType bitset, Int2 displacement)>;

    WaveFunctionCollapse(int height, int width, std::minstd_rand* gen_ptr = nullptr);
    WaveFunctionCollapse(const WaveFunctionCollapse&) = delete;

    bool init();
    BitsetType get(Int2 pos) const;
    bool set(Int2 pos, BitsetType bitset);
    void backtrack();
    bool generate();
    Generator<std::pair<Int2, FactorType>> generate_async();
    void print() const;

    Int2 getSize() const noexcept {
        return size_;
    }

    FactorType getFactorCount() const noexcept {
        return static_cast<FactorType>(weights_.size());
    }

    BitsetType getFactorMask() const noexcept {
        return (1u << getFactorCount()) - 1u;
    }

    std::vector<WeightType>& getWeights() noexcept {
        return weights_;
    }

    std::vector<std::pair<std::vector<Int2>, DiffuseFuncType>>& getDiffuseFuncs() noexcept {
        return diffuse_funcs_;
    }

    constexpr static BitsetType toBitset(std::initializer_list<FactorType> factors) noexcept {
        BitsetType res = 0u;
        for (auto id : factors) res |= 1u << id;
        return res;
    }

    template <typename IT>
        requires std::input_iterator<IT> && std::same_as<std::iter_value_t<IT>, FactorType>
    constexpr static BitsetType toBitset(IT begin, IT end) noexcept {
        BitsetType res = 0u;
        for (auto it = begin; it != end; ++it) res |= 1u << *it;
        return res;
    }

    constexpr static FactorType toFactor(BitsetType bitset) noexcept {
        return std::popcount(bitset) == 1 ? std::countr_zero(bitset) : -1;
    }

private:
    // 随机数生成器
    std::minstd_rand* gen_ptr_;

    // 矩阵尺寸和数据
    Int2 size_;
    Matrix<Node> mat_;
    std::unordered_set<Int2> todo_set_;

    std::vector<WeightType> weights_;
    std::vector<std::pair<std::vector<Int2>, DiffuseFuncType>> diffuse_funcs_;

    // diffuse 的辅助变量
    Matrix<bool> vis_;
    std::map<Int2, Node> backup_;

    Int2 find_() const;
    bool diffuse_(Int2 pos, Node node);
};



class WaveFunctionCollapse::Node
{
public:
    BitsetType bitset;
    
    constexpr Node() noexcept
        : bitset(0u) {}

    constexpr Node(const Node&) noexcept = default;

    constexpr explicit Node(BitsetType bitset) noexcept
        : bitset(bitset) {}

    constexpr bool operator==(const Node& rhs) const noexcept {
        return bitset == rhs.bitset;
    }

    constexpr bool isEmpty() const noexcept {
        return !bitset;
    }

    double getEntropy(const WaveFunctionCollapse& wfc) const;
    std::vector<FactorType> collapse(const WaveFunctionCollapse& wfc) const;
};



} // namespace cha