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
        const int groundRectHeight = 100;
        const sf::Vector2u textureSize = textura.getSize();
        float roadScale = 800.f / static_cast<float>(textureSize.x);
        int textureHeight = static_cast<int>(textureSize.y);
        calle1.setTextureRect(sf::IntRect(sf::Vector2i(0, textureHeight - groundRectHeight), sf::Vector2i(textureSize.x, groundRectHeight)));
        calle2.setTextureRect(sf::IntRect(sf::Vector2i(0, textureHeight - groundRectHeight), sf::Vector2i(textureSize.x, groundRectHeight)));
        calle1.setScale({roadScale, roadScale});
        calle2.setScale({roadScale, roadScale});
        float groundY = 400.f - static_cast<float>(groundRectHeight) * roadScale;
        calle1.setPosition({0.f, groundY});
        calle2.setPosition({textureSize.x * roadScale, groundY});
    }

    void update(bool gameStarted, bool gamePaused, float speedFactor) {
        if (!gameStarted || gamePaused) return;
        
        calle1.move({-velocidad * speedFactor, 0.f});
        calle2.move({-velocidad * speedFactor, 0.f});
        
        float spriteWidth = calle1.getGlobalBounds().size.x;
        if (calle1.getPosition().x <= -spriteWidth) calle1.setPosition({calle2.getPosition().x + spriteWidth, calle1.getPosition().y});
        if (calle2.getPosition().x <= -spriteWidth) calle2.setPosition({calle1.getPosition().x + spriteWidth, calle2.getPosition().y});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(calle1);
        window.draw(calle2);
    }
};