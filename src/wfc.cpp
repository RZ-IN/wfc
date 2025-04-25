#include <bitset>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <random>
#include <algorithm>
#include <thread>
#include <fmt/core.h>
#include "tools/Matrix.hpp"
#include "tools/binary_indexed_tree.hpp"
#include "tools/console_controller.hpp"
#include "tools/block_print.hpp"
#include "wfc.h"



template <typename T>
inline T mrand(const T a, const T b) {
    static std::minstd_rand gen(std::random_device{}());
    return std::uniform_int_distribution<T>(a, b)(gen);
}



template <typename Function>
    requires std::invocable<Function, int>
std::vector<int> shuffle(const std::size_t n, Function&& f)
{
    std::vector<int> idx(n);
    std::iota(idx.begin(), idx.end(), 0);
    cha::BinaryIndexedTree<decltype(f(0))> bit(n, f);

    for (int i = n; ~--i;) {
        const int j = bit.lower(mrand(1, bit.get(i + 1)), i + 1);
        bit.add(j + 1, f(idx[i]) - f(idx[j]));
        std::swap(idx[i], idx[j]);
    }
    std::reverse(idx.begin(), idx.end());

    return idx;
}



struct WaveFunctionCollapse::Impl
{
    using Int2 = cha::Int2;
    using FactorID = int;
    static constexpr FactorID FACTOR_COUNT = 5;
    static constexpr std::array<int, FACTOR_COUNT> WEIGHTS = {
        1, 2, 3, 4, 5
    };

    struct Node
    {
        std::bitset<FACTOR_COUNT> factors;

        Node() = default;
        Node(std::initializer_list<FactorID> init) {
            for (const auto id : init) {
                factors.set(id);
            }
        }

        bool isEmpty() const {
            return factors.none();
        }

        FactorID getFactor() const {
            FactorID id = -1;
            for (int i{}; i < factors.size(); ++i) {
                if (factors[i]) {
                    if (~id) return -1; // 多个因子
                    id = i;
                }
            }
            return id;
        }

        double getEntropy() const {
            double ans = 0.0, down = 0.0;
            for (int i{}; i < factors.size(); ++i) {
                if (factors[i]) {
                    down += WEIGHTS[i];
                }
            }
            for (int i{}; i < factors.size(); ++i) {
                if (factors[i]) {
                    ans += WEIGHTS[i] / down * std::log2(WEIGHTS[i] / down);
                }
            }
            return -ans;
        }

        std::vector<FactorID> collapse() const {
            static std::vector<FactorID> tmp(FACTOR_COUNT);
            int num = 0;
            for (int i{}; i < factors.size(); ++i) {
                if (factors[i]) {
                    tmp[num++] = i;
                }
            }
            const auto idx = shuffle(num, [](int i) {
                return WEIGHTS[tmp[i]];
            });
            std::vector<FactorID> res(num);
            for (int i{}; i < num; ++i) {
                res[i] = tmp[idx[i]];
            }
            return res;
        }
    };

    Impl(const int h, const int w)
        : H(h), W(w), mat_(h, w), vis_(h, w, false) {}

    void init();
    
    [[nodiscard]] bool set(const cha::Int2 pos, const FactorID id);

    [[nodiscard]] bool generate();

    void print() const;

    [[nodiscard]] int getHeight() const { return H; }
    [[nodiscard]] int getWidth() const { return W; }
    
    int H, W;
    cha::Matrix<Node> mat_;
    std::unordered_set<Int2> can_selected_;

    [[nodiscard]] Int2 find_() const;
    
    cha::Matrix<bool> vis_;
    std::unordered_set<Int2> in_qu_;
    std::unordered_map<Int2, decltype(Node::factors)> backup_;
    std::queue<Int2> qu_;

    void clear_(const Int2 pos);
    [[nodiscard]] bool diffuse_(const Int2 pos);
};



WaveFunctionCollapse::WaveFunctionCollapse(const int h, const int w)
    : p_impl_(new Impl(h, w)) {}

WaveFunctionCollapse::~WaveFunctionCollapse() {
    delete p_impl_;
}

void WaveFunctionCollapse::init() {
    p_impl_->init();
}

bool WaveFunctionCollapse::set(const cha::Int2 pos, const FactorID id) {
    return p_impl_->set(pos, id);
}

bool WaveFunctionCollapse::generate() {
    return p_impl_->generate();
}

void WaveFunctionCollapse::print() const {
    p_impl_->print();
}



void WaveFunctionCollapse::Impl::init()
{
    mat_.fill(Node{0, 1, 2, 3, 4});
    for (const Int2 pos : Int2::Range({H, W})) {
        can_selected_.insert(pos);
    }
}



bool WaveFunctionCollapse::Impl::set(const Int2 pos, const FactorID id)
{
    mat_[pos] = Node{id};
    can_selected_.erase(pos);
    return diffuse_(pos);
}



bool WaveFunctionCollapse::Impl::generate()
{
    cha::ConsoleController cc;
    cc.ShowCursor(false);
    // DFS
    // std::function<bool(int)> next = [&](
    //     int depth
    // ) {
    //     if (depth == H * W) {
    //         return true;
    //     }
    //     const Int2 pos = find_();
    //     can_selected_.erase(pos);
    //     const auto backup = mat_[pos];
    //     for (const FactorID id : mat_[pos].collapse()) {
    //         mat_[pos] = Node{id};
    //         if (diffuse_(pos) && next(depth + 1)) {
    //             return true;
    //         }
    //     }
    //     // 复位
    //     mat_[pos] = backup;
    //     can_selected_.insert(pos);
    //     return false;
    // };
    // return next(0);

    // BFS
    struct State {
        Int2 pos;
        Node backup;
        std::vector<FactorID> factors;
        int idx;
    };
    std::stack<State> st;
    auto create = [&] {
        const Int2 pos = find_();
        can_selected_.erase(pos);
        st.emplace(pos, mat_[pos], mat_[pos].collapse(), 0);
    };
    bool ret = false;
    create();
    while (!st.empty()) {
        if (ret) st.pop();
        else {
            auto& s = st.top();
            if (s.idx == s.factors.size()) {
                // 复位
                mat_[s.pos] = s.backup;

                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                cc.SetCursor(s.pos.x + 1, s.pos.y + 1);
                fmt::print("{}", "? .+*#"[mat_[s.pos].getFactor() + 1]);

                can_selected_.insert(s.pos);
                st.pop();
            } else {
                mat_[s.pos] = Node{s.factors[s.idx++]};

                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                cc.SetCursor(s.pos.x + 1, s.pos.y + 1);
                fmt::print("{}", "? .+*#"[mat_[s.pos].getFactor() + 1]);

                if (!diffuse_(s.pos)) continue;
                if (st.size() == H * W) {
                    st.pop();
                    ret = true;
                    continue;
                }
                create();
            }
        }
    }
    cc.SetCursor(0, H + 2);
    cc.ShowCursor(true);
    return ret;
}



void WaveFunctionCollapse::Impl::print() const
{
    cha::BlockPrint block;
    for (int i{}; i < H; ++i) {
        for (int j{}; j < W; ++j) {
            block.push("{}", "? .+*#"[mat_(i, j).getFactor() + 1]);
        }
        block.newline();
    }
    block.removeExtraLines();
    block.print();
}



cha::Int2 WaveFunctionCollapse::Impl::find_() const
{
    Int2 res;
    double entropy = std::numeric_limits<double>::max();
    int cnt = 1;
    for (const Int2 pos : can_selected_) {
        double e = mat_[pos].getEntropy();
        if (e < entropy) {
            res = pos;
            entropy = e;
            cnt = 1;
        }
        else if (e == entropy) {
            // 水池抽样
            if (mrand(1, ++cnt) == 1) {
                res = pos;
            }
        }
    }
    return res;
}



void WaveFunctionCollapse::Impl::clear_(const cha::Int2 pos)
{
    while (!qu_.empty()) qu_.pop();
    for (const auto [p, f] : backup_) {
        vis_[p] = false;
    }
    backup_.clear();
    in_qu_.clear();
    vis_[pos] = false;
}



bool WaveFunctionCollapse::Impl::diffuse_(const cha::Int2 pos)
{
    vis_[pos] = true;
    qu_.push(pos);
    while (!qu_.empty()) {
        for (int t = qu_.size(); t--;) {
            const Int2 pp = qu_.front();
            qu_.pop();
            const auto factors = mat_[pp].factors;
            // const bool n = pp.y <= (H - 1) / 2,
            //            s = pp.y >= H / 2,
            //            w = pp.x <= (W - 1) / 2,
            //            e = pp.x >= W / 2;
            for (const Int2 d : cha::DIR4) {
                if (const Int2 p = pp + d; Int2::Range({H, W}).contains(p) && !vis_[p]) {
                    Node& node = mat_[p];
                    decltype(node.factors) valid;
                    {
                        valid = factors;
                        // if (d.x < 0) valid |= w ? factors >> 1 : factors << 1;
                        // else if (d.x > 0) valid |= e ? factors >> 1 : factors << 1;
                        // else if (d.y < 0) valid |= n ? factors >> 1 : factors << 1;
                        // else if (d.y > 0) valid |= s ? factors >> 1 : factors << 1;
                        valid |= factors >> 1 | factors << 1;
                    }
                    const auto tmp = node.factors;
                    node.factors &= valid;
                    if (tmp != node.factors) {
                        backup_.insert({p, tmp});
                        if (mat_[p].isEmpty()) {
                            // 复位
                            for (const auto [p, f] : backup_) {
                                mat_[p].factors = f;
                            }
                            clear_(pos);
                            return false;
                        }
                        if (!in_qu_.contains(p)) {
                            in_qu_.insert(p);
                            qu_.push(p);
                        }
                    }
                }
                for (const Int2 p : in_qu_) {
                    vis_[p] = true;
                }
                in_qu_.clear();
            }
        }
    }
    clear_(pos);
    return true;
}