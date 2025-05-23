#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <fmt/core.h>
#include "wfc.h"
#include "renderer.h"
#include "tools/index2.hpp"
constexpr bool ASYNC_ON = true;
constexpr sf::Vector2u TILE_SIZE{16u, 16u};
constexpr sf::Vector2u MAP_SIZE{64u, 64u};
constexpr sf::Vector2u SCREEN_SIZE{TILE_SIZE.x * MAP_SIZE.x, TILE_SIZE.y * MAP_SIZE.y};
constexpr sf::Color BACKGROUND_COLOR(0, 0, 0);

int map[MAP_SIZE.x * MAP_SIZE.y];
cha::WaveFunctionCollapse wfc(MAP_SIZE.y, MAP_SIZE.x);
Renderer render;
bool stop;



bool init();
void handle_event(std::optional<sf::Event> event, sf::RenderWindow& window);
cha::Generator<std::pair<cha::Int2, int>>& get_gen() {
    static auto gen = wfc.generate_async();
    return gen;
}



int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_SIZE), "WFC");
    window.setVerticalSyncEnabled(true);
    window.setView(sf::View(
        sf::Vector2f(SCREEN_SIZE / 2u),
        sf::Vector2f(SCREEN_SIZE)
    ));

    if (!init()) {
        fmt::print("Failed to initialize\n");
        return -1;
    }

    sf::Clock clock;
    sf::Time timer;
    constexpr sf::Time FRAME_TIME = sf::milliseconds(3);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            handle_event(event, window);
        }
        if (!window.hasFocus()) continue;
        if constexpr (ASYNC_ON) {
            timer += clock.restart();
            while (timer > FRAME_TIME) {
                timer -= FRAME_TIME;
                if (stop) continue;
                if (const auto result = get_gen().next()) [[likely]] {
                    const auto [pos, id] = *result;
                    render.setTile({unsigned(pos.x), unsigned(pos.y)}, id);
                }
            }
        }
        window.clear(BACKGROUND_COLOR);
        render.draw(window);
        window.display();
    }
}



bool init()
{
    wfc.getWeights() = {
        1, 1, 1, 1, 1
    };
    wfc.getDiffuseFuncs().emplace_back(
        std::vector(cha::DIR4, cha::DIR4 + 4),
        [](auto bitset, auto dir) {
            cha::WaveFunctionCollapse::BitsetType res;
            if (dir.y) {
                res = (bitset & 1u) << 2
                    | (bitset & 2u) << 1
                    | (bitset & 4u) >> 2
                    | (bitset & 8u) >> 3;
                res *= 3;
            } else {
                res = (bitset & 1u) << 1
                    | (bitset & 2u) >> 1
                    | (bitset & 4u) >> 1
                    | (bitset & 8u) >> 3;
                res *= 5;
            }
            if (dir.y < 0) res |= !!(bitset & 0b0011u) << 4; else {
            if (dir.y > 0) res |= !!(bitset & 0b1100u) << 4; else {
            if (dir.x < 0) res |= !!(bitset & 0b0101u) << 4; else {
            if (dir.x > 0) res |= !!(bitset & 0b1010u) << 4; }}}
            if (bitset & 16u) {
                if (dir.y < 0) res |= 0b1100u; else {
                if (dir.y > 0) res |= 0b0011u; else {
                if (dir.x < 0) res |= 0b1010u; else {
                if (dir.x > 0) res |= 0b0101u; }}}
                res |= 16u;
            }
            return res;
        }
    );

    wfc.init();
    if constexpr (ASYNC_ON) {
        for (int i = sizeof (map) / sizeof(map[0]); i--;) {
            map[i] = (-1 + 6) % 6;
        }
    } else {
        sf::Clock clock;
        fmt::print("Generating map...\n");
        if (!wfc.generate()) {
            fmt::print("Failed to generate map\n");
            return -1;
        }
        for (int y = 0; y < MAP_SIZE.y; ++y) {
            for (int x = 0; x < MAP_SIZE.x; ++x) {
                const int v = cha::WaveFunctionCollapse::toFactor(wfc.get({y, x}));
                map[x + y * MAP_SIZE.x] = (v + 6) % 6;
            }
        }
        fmt::print("Generation took {}ms\n", clock.getElapsedTime().asMilliseconds());
        // wfc.print();
    }

    if (!render.init("assets/pipe.png", TILE_SIZE)) {
        fmt::print("Failed to load tileset\n");
        return -1;
    }

    if (!render.load(map, MAP_SIZE)) {
        fmt::print("Failed to load map\n");
        return -1;
    }

    stop = false;

    return true;
}



void handle_event(std::optional<sf::Event> event, sf::RenderWindow& window)
{
    static float factor = 1.f;
    static sf::Vector2f lastpos;
    static bool pressed = false;

    if (event->is<sf::Event::Closed>()) {
        window.close();
    }
    else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
        auto view = window.getView();
        view.setSize(sf::Vector2f(resized->size));
        view.zoom(1.f / factor);
        window.setView(view);
    }
    else if (const auto* mouse = event->getIf<sf::Event::MouseWheelScrolled>()) {
        const float delta = mouse->delta;
        constexpr float min_factor = []{
            float f = 1.f;
            while (f / 1.1f >= 0.1f)
               f /= 1.1f;
            return f;
        }();
        constexpr float max_factor = []{
            float f = 1.f;
            while (f * 1.1f <= 10.f)
                f *= 1.1f;
            return f;
        }();
        factor = delta > 0 ? factor * 1.1f : factor / 1.1f;
        factor = std::clamp(factor, min_factor, max_factor);
        auto view = window.getView();
        view.setSize(sf::Vector2f(window.getSize()));
        view.zoom(1.f / factor);
        const sf::Vector2f oldpos = window.mapPixelToCoords(mouse->position);
        const sf::Vector2f newpos = window.mapPixelToCoords(mouse->position, view);
        view.move(oldpos - newpos);
        window.setView(view);
    }
    else if (const auto *moved = event->getIf<sf::Event::MouseMoved>()) {
        if (pressed && sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle)) {
            auto tmp = sf::Vector2f(moved->position);
            auto view = window.getView();
            view.move((lastpos - tmp) / factor);
            window.setView(view);
            lastpos = tmp;
        }
    }
    else if (const auto *moved = event->getIf<sf::Event::MouseButtonPressed>()) {
        auto get_xy = [&window](sf::Vector2i pos) {
            const sf::Vector2f croods = window.mapPixelToCoords(pos);
            const sf::Vector2i xy = {
                int(std::floor(croods.x / TILE_SIZE.x)),
                int(std::floor(croods.y / TILE_SIZE.y))
            };
            return xy;
        };
        switch (moved->button) {
        case sf::Mouse::Button::Left: {
            const sf::Vector2i xy = get_xy(moved->position);
            fmt::print("left click: ({}, {})\n", xy.y, xy.x);
            break;
        }
        case sf::Mouse::Button::Right: {
            const sf::Vector2i xy = get_xy(moved->position);
            fmt::print("right click: ({}, {})\n", xy.y, xy.x);
            break;
        }
        case sf::Mouse::Button::Middle: {
            lastpos = sf::Vector2f(moved->position);
            pressed = true;
            break;
        }
        default:
            break;
        }
    }
    else if (const auto *moved = event->getIf<sf::Event::MouseButtonReleased>()) {
        switch (moved->button) {
        case sf::Mouse::Button::Left:
            break;
        case sf::Mouse::Button::Right:
            break;
        case sf::Mouse::Button::Middle: {
            pressed = false;
            break;
        }
        default:
            break;
        }
    }
    else if (const auto *key = event->getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Space) {
            if (stop) {
                stop = false;
            } else {
                stop = true;
            }
        }
    }
}
