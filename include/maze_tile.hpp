/* 
 * maze_tile.hpp
 * Created on 2023.06.20 by RZIN
 * 
 * 一个计算迷宫墙不同情况下的贴图索引的函数
 * 
 * Edited on 2025.04.15 by RZIN
 */
#pragma once
#include <concepts>
namespace cha
{



template <typename T>
concept PosToBool = requires(T func)
{
    { func(0, 0) } -> std::convertible_to<bool>;
};



/// @brief 计算迷宫墙不同情况下的贴图索引
/// @param y 当前坐标 y
/// @param x 当前坐标 x
/// @param isw 墙的判断函数
/// @return 贴图索引
template <PosToBool Func>
[[nodiscard]] inline int maze_tile(int y, int x, Func isw)
{
    constexpr int id[]{
        36, 36,  0,  0, 36, 36,  0,  0, 37, 37,  1,  8, 37, 37,  1,  8,
        39, 39,  3,  3, 39, 39, 11, 11, 38, 38,  2,  5, 38, 38,  6, 10,
        36, 36,  0,  0, 36, 36,  0,  0, 37, 37,  1,  8, 37, 37,  1,  8,
        39, 39,  3,  3, 39, 39, 11, 11, 38, 38,  2,  5, 38, 38,  6, 10,
        24, 24, 12, 12, 24, 24, 12, 12, 25, 25, 13, 16, 25, 25, 13, 16,
        27, 27, 15, 15, 27, 27, 19, 19, 26, 26, 14, 43, 26, 26, 40,  9,
        24, 24, 12, 12, 24, 24, 12, 12, 44, 44, 28, 20, 44, 44, 28, 20,
        27, 27, 15, 15, 27, 27, 19, 19, 41, 41,  7, 32, 41, 41, 21, 17,
        36, 36,  0,  0, 36, 36,  0,  0, 37, 37,  1,  8, 37, 37,  1,  8,
        39, 39,  3,  3, 39, 39, 11, 11, 38, 38,  2,  5, 38, 38,  6, 10,
        36, 36,  0,  0, 36, 36,  0,  0, 37, 37,  1,  8, 37, 37,  1,  8,
        39, 39,  3,  3, 39, 39, 11, 11, 38, 38,  2,  5, 38, 38,  6, 10,
        24, 24, 12, 12, 24, 24, 12, 12, 25, 25, 13, 16, 25, 25, 13, 16,
        47, 47, 31, 31, 47, 47, 35, 35, 42, 42,  4, 34, 42, 42, 23, 18,
        24, 24, 12, 12, 24, 24, 12, 12, 44, 44, 28, 20, 44, 44, 28, 20,
        47, 47, 31, 31, 47, 47, 35, 35, 45, 45, 46, 29, 45, 45, 30, 33
    };
    return id[
        isw(y - 1, x - 1) << 7 |
        isw(y - 1, x    ) << 6 |
        isw(y - 1, x + 1) << 5 |
        isw(y    , x - 1) << 4 |
        isw(y    , x + 1) << 3 |
        isw(y + 1, x - 1) << 2 |
        isw(y + 1, x    ) << 1 |
        isw(y + 1, x + 1)
    ];
}



/*
 * example
 */
[[maybe_unused]] inline void output(char* out, bool* mat, int h, int w)
{
    extern int sprintf(char* out, const char* fmt, ...);

    constexpr const char* wall[48] {
        "┃ ", "┏━", "┳━", "┓ ", "╋━", "┳━", "┳━", "╋━", "┏━", "┻━", "━━", "┓ ",
        "┃ ", "┣━", "╋━", "┫ ", "┣━", "┛ ", "┗━", "┫ ", "┃ ", "╋━", "  ", "┣━",
        "┃ ", "┗━", "┻━", "┛ ", "┣━", "┓ ", "┏━", "┫ ", "┫ ", "█▋", "╋━", "┃ ",
        "█▋", "━━", "━━", "━ ", "╋━", "┻━", "┻━", "╋━", "┗━", "━━", "┳━", "┛ "
    };
    auto is_wall = [mat, h, w](int y, int x) {
        return (y | x | h - y - 1 | w - x - 1) >= 0 && mat[y * w + x];
    };

    int pos = 0;
    for (int i{}; i < h; ++i) {
        for (int j{}; j < w; ++j) {
            if (is_wall(i, j)) {
                pos += sprintf(out + pos, "%s", wall[maze_tile(i, j, is_wall)]);
            } else {
                pos += sprintf(out + pos, "%s", wall[22]);
            }
        }
    }
}



} // namespace cha