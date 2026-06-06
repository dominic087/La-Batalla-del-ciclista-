#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>

class Calle {
private:
    sf::Texture textura;
    sf::Sprite calle1, calle2;
    float velocidad = 6.0f;

public:
    Calle() : calle1(textura), calle2(textura) {
        if (!textura.loadFromFile("assets/image/ground.png")) {
            throw std::runtime_error("Error al cargar assets de la calle");
        }
        textura.setRepeated(true);
        const float roadScale = 0.8f;
        int groundHeight = static_cast<int>(textura.getSize().y);
        calle1.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(800, groundHeight)));
        calle2.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(800, groundHeight)));
        calle1.setScale({roadScale, roadScale});
        calle2.setScale({roadScale, roadScale});
        float groundY = 400.f - static_cast<float>(groundHeight) * roadScale;
        calle1.setPosition({0.f, groundY});
        calle2.setPosition({800.f * roadScale, groundY});
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;
        
        calle1.move({-velocidad, 0.f});
        calle2.move({-velocidad, 0.f});
        
        float roadWidth = 800.f * calle1.getScale().x;
        if (calle1.getPosition().x <= -roadWidth) calle1.setPosition({roadWidth, calle1.getPosition().y});
        if (calle2.getPosition().x <= -roadWidth) calle2.setPosition({roadWidth, calle2.getPosition().y});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(calle1);
        window.draw(calle2);
    }
};