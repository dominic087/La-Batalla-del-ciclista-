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
        // Usar el ancho real del sprite escalado para posicionar y hacer el wrap sin rayas
        float spriteWidth = fondo1.getGlobalBounds().size.x;
        fondo2.setPosition({spriteWidth, 0.f});
    }

    void update(bool gameStarted, bool gamePaused, float speedFactor) {
        if (!gameStarted || gamePaused) return;
        
        fondo1.move({-velocidad * speedFactor, 0.f});
        fondo2.move({-velocidad * speedFactor, 0.f});

        // Usar el ancho real del sprite escalado para el wrap; evita discontinuidades visuales
        float spriteWidth = fondo1.getGlobalBounds().size.x;
        if (fondo1.getPosition().x <= -spriteWidth) fondo1.setPosition({fondo2.getPosition().x + spriteWidth, 0.f});
        if (fondo2.getPosition().x <= -spriteWidth) fondo2.setPosition({fondo1.getPosition().x + spriteWidth, 0.f});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(fondo1);
        window.draw(fondo2);
    }
};
