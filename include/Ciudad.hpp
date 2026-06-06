#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>
#include <algorithm>

class Ciudad {
private:
    sf::Texture textura;
    sf::Sprite fondo1, fondo2;
    float velocidad = 1.0f; 

public:
    Ciudad() : fondo1(textura), fondo2(textura) {
        if (!textura.loadFromFile("assets/image/cloud.jpeg")) {
            throw std::runtime_error("Error al cargar textura de la ciudad");
        }
        const sf::Vector2u textureSize = textura.getSize();
        float cloudScale = std::max(800.f / static_cast<float>(textureSize.x), 400.f / static_cast<float>(textureSize.y));
        textura.setRepeated(false);
        fondo1.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(textureSize.x, textureSize.y)));
        fondo2.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(textureSize.x, textureSize.y)));
        fondo1.setScale({cloudScale, cloudScale});
        fondo2.setScale({cloudScale, cloudScale});
        fondo1.setPosition({0.f, 0.f});
        fondo2.setPosition({textureSize.x * cloudScale, 0.f});
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;
        
        fondo1.move({-velocidad, 0.f});
        fondo2.move({-velocidad, 0.f});

        const float cityWidth = 800.f;
        if (fondo1.getPosition().x <= -cityWidth) fondo1.setPosition({cityWidth, 0.f});
        if (fondo2.getPosition().x <= -cityWidth) fondo2.setPosition({cityWidth, 0.f});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(fondo1);
        window.draw(fondo2);
    }
};
