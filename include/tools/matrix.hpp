/*
 * matrix.hpp
 * Created on 2025.02.20 by RZIN
 * Edited on 2025.04.022 by RZIN
 */
#pragma once
#include <cstddef>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "index2.hpp"
namespace cha
{



/// @brief 扁平化 vector 实现的二维动态数组类模板
/// @tparam T 
/// @note 该类计划在不久的将来进行重构
template <typename T>
class Matrix
{
public:
    /// @brief 迭代器类型
    using iterator = typename std::vector<T>::iterator;

    /// @brief 常量迭代器类型
    using const_iterator = typename std::vector<T>::const_iterator;

    using reference = typename std::vector<T>::reference;
    using const_reference = typename std::vector<T>::const_reference;
    using size_type = typename std::vector<T>::size_type;

private:
    std::size_t rows_ = 0;
    std::size_t cols_ = 0;
    std::vector<T> data_;

public:
    /// @brief 默认构造函数
    Matrix() = default;

    /// @brief 构造函数
    /// @param rows 数组行数
    /// @param cols 数组列数
    /// @param init_val 元素初始值
    Matrix(std::size_t rows, std::size_t cols, const T& init_val = T{})
        : rows_(rows), cols_(cols), data_(rows * cols, init_val) {}

    Matrix(const Matrix&) = default;
    Matrix& operator=(const Matrix&) = default;

    Matrix(Matrix&& other) noexcept
        : rows_(other.rows_), cols_(other.cols_), data_(std::move(other.data_)) {
        other.rows_ = 0;
        other.cols_ = 0;
    }

    Matrix& operator=(Matrix&& other) noexcept {
        if (this != &other) {
            rows_ = other.rows_;
            cols_ = other.cols_;
            data_ = std::move(other.data_);
            other.rows_ = 0;
            other.cols_ = 0;
        }
        return *this;
    }

    /// @brief 通过行列索引访问元素
    /// @param row 行索引
    /// @param col 列索引
    /// @return 元素引用
    reference operator()(std::size_t row, std::size_t col) noexcept {
        return data_[row * cols_ + col];
    }

    /// @brief 通过行列索引访问元素
    /// @param row 行索引
    /// @param col 列索引
    /// @return 元素引用
    const_reference operator()(std::size_t row, std::size_t col) const noexcept {
        return data_[row * cols_ + col];
    }

    /// @brief 带有边界检查的行列索引元素访问
    /// @param row 行索引
    /// @param col 列索引
    /// @return 元素引用
    reference at(std::size_t row, std::size_t col) {
        check_bounds(row, col);
        return data_[row * cols_ + col];
    }

    /// @brief 带有边界检查的行列索引元素访问
    /// @param row 行索引
    /// @param col 列索引
    /// @return 元素引用
    const_reference at(std::size_t row, std::size_t col) const {
        check_bounds(row, col);
        return data_[row * cols_ + col];
    }

    /// @brief 通过一维索引访问元素
    /// @param idx 一维索引
    /// @return 元素引用
    reference operator[](std::size_t idx) noexcept {
        return data_[idx];
    }
    
    /// @brief 通过一维索引访问元素
    /// @param idx 一维索引
    /// @return 元素引用
    const_reference operator[](std::size_t idx) const noexcept {
        return data_[idx];
    }

    /// @brief 带有边界检查的一维索引元素访问
    /// @param idx 一维索引
    /// @return 元素引用
    reference at(std::size_t idx) {
        check_bounds(idx); 
        return data_.at(idx);
    }

    /// @brief 带有边界检查的一维索引元素访问
    /// @param idx 一维索引
    /// @return 元素引用
    const_reference at(std::size_t idx) const {
        check_bounds(idx);
        return data_.at(idx);
    }

    reference operator[](Int2 idx) noexcept {
        return data_[idx.y * cols_ + idx.x];
    }

    const_reference operator[](Int2 idx) const noexcept {
        return data_[idx.y * cols_ + idx.x];
    }

    reference at(Int2 idx) {
        check_bounds(idx.y, idx.x);
        return data_[idx.y * cols_ + idx.x];
    }

    const_reference at(Int2 idx) const {
        check_bounds(idx.y, idx.x);
        return data_[idx.y * cols_ + idx.x];
    }

    /// @brief 调整数组大小
    /// @param new_rows 新的行数
    /// @param new_cols 新的列数
    /// @param fill_value 填充值
    void resize(std::size_t new_rows, std::size_t new_cols, const T& fill_value = T{}) {
        std::vector<T> new_data(new_rows * new_cols, fill_value);
        const std::size_t min_rows = std::min(rows_, new_rows);
        const std::size_t min_cols = std::min(cols_, new_cols);
        for (std::size_t r = 0; r < min_rows; ++r) {
            for (std::size_t c = 0; c < min_cols; ++c) {
                new_data[r * new_cols + c] = (*this)(r, c);
            }
        }
        data_ = std::move(new_data);
        rows_ = new_rows;
        cols_ = new_cols;
    }

    /// @brief 交换两个数组
    /// @param other 
    void swap(Matrix& other) noexcept {
        using std::swap;
        swap(rows_, other.rows_);
        swap(cols_, other.cols_);
        data_.swap(other.data_);
    }

    /// @brief 交换两个数组
    /// @param a 
    /// @param b 
    friend void swap(Matrix& a, Matrix& b) noexcept {
        a.swap(b);
    }

    /// @brief 清空数组
    void clear() noexcept {
        rows_ = 0;
        cols_ = 0;
        data_.clear();
    }

    /// @brief 填充数组
    /// @param value 填充值
    void fill(const T& value) {
        std::fill(data_.begin(), data_.end(), value);
    }

    /// @brief 获取数组行数
    /// @return 数组行数
    std::size_t rows() const noexcept { return rows_; }

    /// @brief 获取数组列数
    /// @return 数组列数
    std::size_t cols() const noexcept { return cols_; }

    /// @brief 获取数组大小
    /// @return {数组行数, 数组列数}
    std::pair<std::size_t, std::size_t> size() const noexcept { return std::make_pair(rows_, cols_); }

    /// @brief 获取数组长度
    /// @return 数组长度
    std::size_t length() const noexcept { return data_.size(); }

    /// @brief 判断数组是否为空
    /// @return 是否为空
    bool empty() const noexcept { return rows_ == 0 || cols_ == 0; }

    /// @brief 获取数组数据指针
    /// @return 数据指针
    T* data() noexcept { return data_.data(); }

    /// @brief 获取数组数据指针
    /// @return 数据指针
    const T* data() const noexcept { return data_.data(); }



    iterator begin() noexcept { return data_.begin(); }
    iterator end() noexcept { return data_.end(); }
    const_iterator begin() const noexcept { return data_.begin(); }
    const_iterator end() const noexcept { return data_.end(); }
    const_iterator cbegin() const noexcept { return data_.cbegin(); }
    const_iterator cend() const noexcept { return data_.cend(); }

private:
    void check_bounds(std::size_t row, std::size_t col) const {
        if (row >= rows_ || col >= cols_)
            throw std::out_of_range("Dynamic2DArray index out of range");
    }

    void check_bounds(std::size_t idx) const {
        if (idx >= data_.size())
            throw std::out_of_range("Dynamic2DArray index out of range");
    }
};



/// @brief 交换两个数组
/// @param a 
/// @param b 
template <typename T>
void swap(Matrix<T>& a, Matrix<T>& b) noexcept {
    a.swap(b);
}



} // namespace cha