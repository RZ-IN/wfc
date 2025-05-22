#pragma once
#include <filesystem>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>



class Renderer
{
public:
    Renderer() = default;

    [[nodiscard]] bool init(const std::filesystem::path& tilepath, const sf::Vector2u tilesize);
    [[nodiscard]] bool load(const int map[], const sf::Vector2u mapsize);

    void setTile(sf::Vector2u pos, int id);
    void draw(sf::RenderTarget& target) const;

private:
    sf::Vector2u     m_tilesize;
    sf::Texture      m_tileset;
    sf::VertexArray  m_vertices;
    sf::RenderStates m_states;

    sf::Vector2u     m_mapsize;
};