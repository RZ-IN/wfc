/*
 * block_print.hpp
 * Created on 2025.04.25 by RZIN
 * Edited on 2025.04.25 by RZIN
 */
#pragma once
#include <iterator>
#include <vector>
#include <string>
#include <fmt/core.h>
namespace cha
{



class BlockPrint
{
public:
    BlockPrint() = default;
    
    template <typename... Args>
    void push(fmt::format_string<Args...> fmt, Args&&... args) {
        if (lines_.empty()) {
            newline();
        }
        fmt::format_to(std::back_inserter(lines_.back()), fmt, std::forward<Args>(args)...);
        while (width_ < lines_.back().size()) {
            border_.append("━");
            ++width_;
        }
    }

    void newline() {
        lines_.emplace_back();
    }

    void removeExtraLines() {
        while (!lines_.empty() && lines_.back().empty()) {
            lines_.pop_back();
        }
    }

    void print() const {
        fmt::print("┏{}┓\n", border_);
        for (const auto& line : lines_) {
            fmt::print("┃{:<{}}┃\n", line, width_);
        }
        fmt::print("┗{}┛\n", border_);
    }

    void clear() {
        lines_.clear();
        border_.clear();
        width_ = 0;
    }

private:
    std::vector<std::string> lines_;
    std::string border_;
    int width_ = 0;
};



} // namespace cha