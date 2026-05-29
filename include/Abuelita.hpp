#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>

class Abuelita {
private:
    sf::Texture textura;
    sf::Sprite sprite;
    float velocidad = 4.0f; // Más lenta que el ciclista, pero peligrosa

public:
    Abuelita() : sprite(textura) {
        if (!textura.loadFromFile("assets/image/abuelita.png")) {
            throw std::runtime_error("Error al cargar textura de la abuelita");
        }
        // Ajusta este 0.25f si la imagen sale muy grande o muy chica
        sprite.setScale({0.25f, 0.25f}); 
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
        // Aparece a la derecha, un poco separada de los baches
        sprite.setPosition({1100.f, 210.f}); 
    }

    void esconder() {
        sprite.setPosition({-1000.f, 210.f}); // La sacamos de la pantalla
    }

    const sf::Sprite& getSprite() const { return sprite; }
};