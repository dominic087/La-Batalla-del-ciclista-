#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>

class Ciclista {
private:
    sf::Texture textura;
    sf::Sprite sprite;
    float velocidad = 8.5f;

public:
    Ciclista() : sprite(textura) {
        if (!textura.loadFromFile("assets/image/ghost.png")) {
            throw std::runtime_error("Error al cargar textura del ciclista");
        }
        sprite.setScale({0.3f, 0.3f});
        reset();
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;
        
        sprite.move({-velocidad, 0.f});
        
        if (sprite.getPosition().x < -100) {
            reset();
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    void reset() {
        sprite.setPosition({1200.f, 210.f});
    }

    const sf::Sprite& getSprite() const { return sprite; }
};