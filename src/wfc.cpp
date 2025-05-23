#include "wfc.h"
#include <queue>
#include <algorithm>
#include <fmt/core.h>
#include "tools/binary_indexed_tree.hpp"
#include "tools/generator.hpp"
#include "tools/index2.hpp"
namespace cha
{



double WaveFunctionCollapse::Node::getEntropy(const WaveFunctionCollapse& wfc) const
{
    double res = 0.0;
    WeightType down{};
    for (FactorType i = 0; i < wfc.getFactorCount(); ++i) {
        if (bitset >> i & 1u) {
            down += wfc.weights_[i];
        }
    }
    for (FactorType i = 0; i < wfc.getFactorCount(); ++i) {
        if (bitset >> i & 1u) {
            const double x = wfc.weights_[i] / double(down);
            res += x * std::log2(x);
        }
    }
    return -res;
}



std::vector<WaveFunctionCollapse::FactorType> WaveFunctionCollapse::Node::collapse(const WaveFunctionCollapse& wfc) const
{
    static std::vector<FactorType> tmp(wfc.getFactorCount());
    int num = 0;
    for (FactorType i = 0; i < wfc.getFactorCount(); ++i) {
        if (bitset >> i & 1u) {
            tmp[num++] = i;
        }
    }
    auto func = [&wfc](int i) {
        return wfc.weights_[tmp[i]];
    };
    std::vector<int> idx(num);
    std::iota(idx.begin(), idx.end(), 0);
    cha::BinaryIndexedTree<WeightType> bit(num, func);
    for (int i = num; ~--i;) {
        std::uniform_int_distribution<int> dist(1, bit.get(i + 1));
        const int j = bit.lower(dist(*wfc.gen_ptr_), i + 1);
        bit.add(j + 1, func(idx[i]) - func(idx[j]));
        std::swap(idx[i], idx[j]);
    }
    std::reverse(idx.begin(), idx.end());

    std::vector<FactorType> res(num);
    for (int i = 0; i < num; ++i) {
        res[i] = tmp[idx[i]];
    }
    return res;
}



    
static std::minstd_rand& get_gen_instance() {
    static std::minstd_rand gen(std::random_device{}());
    return gen;
}

WaveFunctionCollapse::WaveFunctionCollapse(int height, int width, std::minstd_rand* gen_ptr)
    : gen_ptr_(gen_ptr), size_(height, width), mat_(height, width), vis_(height, width, false)
{
    if (gen_ptr_ == nullptr) {
        gen_ptr_ = &get_gen_instance();
    }
}



bool WaveFunctionCollapse::init()
{
    if (getFactorCount() == 0) {
        return false;
    }
    mat_.fill(Node(getFactorMask()));
    for (const Int2 pos : Int2::Range(size_)) {
        todo_set_.insert(pos);
    }
    return true;
}



WaveFunctionCollapse::BitsetType WaveFunctionCollapse::get(Int2 pos) const
{
    return mat_[pos].bitset;
}



bool WaveFunctionCollapse::set(Int2 pos, BitsetType bitset)
{
    return diffuse_(pos, Node(bitset));
}



void WaveFunctionCollapse::backtrack()
{
    for (const auto [pos, node] : backup_) {
        mat_[pos] = node;
    }
    backup_.clear();
}



bool WaveFunctionCollapse::generate()
{
    // BFS
    struct State {
        Int2 pos;
        std::vector<FactorType> factors;
        int idx;
        decltype(backup_) backup;
    };
    std::vector<State> states(size_.y * size_.x);
    int top = -1;

    auto create = [this, &states, &top] {
        const Int2 pos = find_();
        todo_set_.erase(pos);
        ++top;
        states[top].pos = pos;
        states[top].factors = mat_[pos].collapse(*this);
        states[top].idx = 0;
        states[top].backup.clear();
    };

    bool ret = false;
    create();
    while (top >= 0) {
        // fmt::print("stack size: {}\n", top + 1);
        if (ret) --top;
        else [[likely]] {
            auto& [pos, factors, idx, backup] = states[top];
            
            // 复位
            backup_.swap(backup);
            backtrack();

            if (idx == factors.size()) {
                todo_set_.insert(pos);
                --top;
            } else [[likely]] {
                const Node node(toBitset({factors[idx++]}));
                if (!diffuse_(pos, node)) continue;
                if (top == states.size() - 1) [[unlikely]] {
                    --top;
                    ret = true;
                    continue;
                }
                backup.swap(backup_);
                backup_.clear();
                create();
            }
        }
    }
    return ret;
}



Generator<std::pair<Int2, WaveFunctionCollapse::FactorType>> WaveFunctionCollapse::generate_async()
{
    // BFS
    struct State {
        Int2 pos;
        std::vector<FactorType> factors;
        int idx;
        decltype(backup_) backup;
    };
    std::vector<State> states(size_.y * size_.x);
    int top = -1;

    auto create = [this, &states, &top] {
        const Int2 pos = find_();
        todo_set_.erase(pos);
        ++top;
        states[top].pos = pos;
        states[top].factors = mat_[pos].collapse(*this);
        states[top].idx = 0;
        states[top].backup.clear();
    };

    bool ret = false;
    create();
    while (top >= 0) {
        // fmt::print("stack size: {}\n", top + 1);
        if (ret) --top;
        else [[likely]] {
            auto& [pos, factors, idx, backup] = states[top];
            
            // 复位
            backup_.swap(backup);
            backtrack();

            if (idx == factors.size()) {
                co_yield std::make_pair(pos, -1);
                todo_set_.insert(pos);
                --top;
            } else [[likely]] {
                const Node node(toBitset({factors[idx++]}));
                if (!diffuse_(pos, node)) continue;
                co_yield std::make_pair(pos, factors[idx - 1]);
                if (top == states.size() - 1) [[unlikely]] {
                    --top;
                    ret = true;
                    continue;
                }
                backup.swap(backup_);
                backup_.clear();
                create();
            }
        }
    }
    co_return;
}



void WaveFunctionCollapse::print() const
{
    static const char* symbols = &"? .:+*%#@/"[1];

    for (int i = 0; i < size_.y; ++i) {
        for (int j = 0; j < size_.x; ++j) {
            fmt::print("{}", symbols[std::min(toFactor(get({i, j})), 8)]);
        }
        fmt::print("\n");
    }
}



Int2 WaveFunctionCollapse::find_() const
{
    Int2 res;
    int cnt = 0;
    double entropy = std::numeric_limits<double>::max();
    for (Int2 pos : todo_set_) {
        const double ep = mat_[pos].getEntropy(*this);
        if (ep < entropy) {
            res = pos;
            entropy = ep;
            cnt = 1;
        }
        else if (ep == entropy) {
            // 水池抽样
            std::uniform_int_distribution<int> dist(1, ++cnt);
            if (dist(*gen_ptr_) == 1) {
                res = pos;
            }
        }
    }
    return res;
}



bool WaveFunctionCollapse::diffuse_(Int2 ppos, Node node)
{
    static std::queue<Int2> queue;
    static std::unordered_set<Int2> in_queue;
    while (!queue.empty()) {
        queue.pop();
    }
    in_queue.clear();
    
    /*
     * 对 mat_[pos] 施加约束
     * 取 mar_[pos].factors 与 valid 的交集
     * 如果 mat_[pos].factors 变空，返回 false
     */
    auto update_node = [this](const Int2 pos, const BitsetType valid) {
        Node& node = mat_[pos];
        const Node tmp = node;
        node.bitset &= valid & getFactorMask();
        if (tmp != node) {
            backup_.emplace(pos, tmp);
            if (mat_[pos].isEmpty()) [[unlikely]] {
                return false;
            }
            if (!in_queue.contains(pos)) [[likely]] {
                in_queue.insert(pos);
            }
        }
        return true;
    };

    backup_.emplace(ppos, mat_[ppos]);
    mat_[ppos] = node;
    vis_[ppos] = true;
    queue.push(ppos);
    while (!queue.empty()) {
        for (int t = queue.size(); t--;) {
            const Int2 pp = queue.front();
            queue.pop();
            for (const auto& [dirs, func] : diffuse_funcs_) {
                for (const Int2 dp : dirs) {
                    if (const Int2 pos = pp + dp; Int2::Range(size_).contains(pos) && !vis_[pos]) [[likely]] {
                        const BitsetType valid = func(mat_[pp].bitset, dp);
                        if (!update_node(pos, valid)) [[unlikely]] {
                            for (const auto [pos, node] : backup_) {
                                mat_[pos] = node;
                                vis_[pos] = false;
                            }
                            backup_.clear();
                            return false;
                        }
                    }
                }
            }
        }
        for (const Int2 pos : in_queue) {
            vis_[pos] = true;
            queue.push(pos);
        }
        in_queue.clear();
    }

    for (const auto [pos, _] : backup_) {
        vis_[pos] = false;
    }
    return true;
}



} // namespace cha