#include "renderer.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>



bool Renderer::init(const std::filesystem::path& tilepath, const sf::Vector2u tilesize)
{
    if (!m_tileset.loadFromFile(tilepath)) {
        return false;
    }
    m_tilesize = tilesize;
    m_states.texture = &m_tileset;

    return true;
}



bool Renderer::load(const int map[], const sf::Vector2u mapsize)
{
    m_mapsize = mapsize;
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_vertices.resize(mapsize.x * mapsize.y * 6);

    const unsigned int cols = m_tileset.getSize().x / m_tilesize.x;
    for (unsigned int y = 0; y < mapsize.y; ++y) {
        for (unsigned int x = 0; x < mapsize.x; ++x) {
            const int tileid = map[x + y * mapsize.x];
            const int tu = tileid % cols;
            const int tv = tileid / cols;
            sf::Vertex* triangles = &m_vertices[(x + y * mapsize.x) * 6];
            // define the 6 corners of the two triangles
            triangles[0].position = sf::Vector2f(x * m_tilesize.x, y * m_tilesize.y);
            triangles[1].position = sf::Vector2f((x + 1) * m_tilesize.x, y * m_tilesize.y);
            triangles[2].position = sf::Vector2f(x * m_tilesize.x, (y + 1) * m_tilesize.y);
            triangles[3].position = sf::Vector2f(x * m_tilesize.x, (y + 1) * m_tilesize.y);
            triangles[4].position = sf::Vector2f((x + 1) * m_tilesize.x, y * m_tilesize.y);
            triangles[5].position = sf::Vector2f((x + 1) * m_tilesize.x, (y + 1) * m_tilesize.y);
            // define the 6 matching texture coordinates
            triangles[0].texCoords = sf::Vector2f(tu * m_tilesize.x, tv * m_tilesize.y);
            triangles[1].texCoords = sf::Vector2f((tu + 1) * m_tilesize.x, tv * m_tilesize.y);
            triangles[2].texCoords = sf::Vector2f(tu * m_tilesize.x, (tv + 1) * m_tilesize.y);
            triangles[3].texCoords = sf::Vector2f(tu * m_tilesize.x, (tv + 1) * m_tilesize.y);
            triangles[4].texCoords = sf::Vector2f((tu + 1) * m_tilesize.x, tv * m_tilesize.y);
            triangles[5].texCoords = sf::Vector2f((tu + 1) * m_tilesize.x, (tv + 1) * m_tilesize.y);
        }
    }

    return true;
}



void Renderer::setTile(sf::Vector2u pos, int id)
{
    const int tu = id % (m_tileset.getSize().x / m_tilesize.x);
    const int tv = id / (m_tileset.getSize().x / m_tilesize.x);
    sf::Vertex* triangles = &m_vertices[(pos.x + pos.y * m_mapsize.x) * 6];
    triangles[0].texCoords = sf::Vector2f(tu * m_tilesize.x, tv * m_tilesize.y);
    triangles[1].texCoords = sf::Vector2f((tu + 1) * m_tilesize.x, tv * m_tilesize.y);
    triangles[2].texCoords = sf::Vector2f(tu * m_tilesize.x, (tv + 1) * m_tilesize.y);
    triangles[3].texCoords = sf::Vector2f(tu * m_tilesize.x, (tv + 1) * m_tilesize.y);
    triangles[4].texCoords = sf::Vector2f((tu + 1) * m_tilesize.x, tv * m_tilesize.y);
    triangles[5].texCoords = sf::Vector2f((tu + 1) * m_tilesize.x, (tv + 1) * m_tilesize.y);
}



void Renderer::draw(sf::RenderTarget& target) const
{
    target.draw(m_vertices, m_states);
}