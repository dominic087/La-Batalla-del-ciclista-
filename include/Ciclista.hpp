#pragma once
#include <SFML/Graphics.hpp>
#include <cstdlib>

class Ciclista {
private:
    sf::Texture textura;
    sf::Sprite sprite;
    float velocidad = 8.5f; // Es más rápido que los baches normales

public:
    Ciclista() {
        if (textura.loadFromFile("assets/image/ghost.png")) { // Usamos el asset existente temporalmente
            sprite.setTexture(textura);
        }
        sprite.setScale({0.3f, 0.3f});
        reset();
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;
        
        sprite.move({-velocidad, 0.f});
        
        // Si sale de pantalla, reaparece a la derecha
        if (sprite.getPosition().x < -100) {
            reset();
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    void reset() {
        sprite.setPosition({1200.f, 210.f}); // Aparece lejos a la derecha
    }

    sf::Sprite& getSprite() { return sprite; }
};