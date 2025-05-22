/*
 * generator.hpp
 * Created on 2025.05.15 by RZIN
 * Edited on 2025.05.15 by RZIN
 */
#pragma once
#include <coroutine>
#include <optional>
namespace cha
{



template <typename T>
struct Generator
{
    struct promise_type
    {
        std::optional<T> current_value;
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        Generator get_return_object() { return Generator{std::coroutine_handle<promise_type>::from_promise(*this)}; }
        void unhandled_exception() {}
        void return_void() {}
        std::suspend_always yield_value(T value) {
            current_value = value;
            return {};
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    Generator(handle_type h) : coro(h) {}
    Generator(const Generator&) = delete;
    Generator(Generator&& other) noexcept : coro(other.coro) {
        other.coro = nullptr;
    }
    ~Generator() { if (coro) coro.destroy(); }

    std::optional<T> next() {
        if (!coro || coro.done()) {
            return std::nullopt;
        }
        coro.resume();
        return coro.promise().current_value;
    }

private:
    handle_type coro;
};



} // namespace cha