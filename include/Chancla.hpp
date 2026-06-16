#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>

class Chancla {
private:
    sf::Texture textura;
    sf::Sprite sprite;
    float velocidad = 8.0f; // La chancla viaja muy rápido
    bool activa = false;

public:
    Chancla() : sprite(textura) {
        if (!textura.loadFromFile("assets/image/chancla.png")) {
            throw std::runtime_error("Error al cargar textura de la chancla");
        }
        textura.setSmooth(false);
        sprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(textura.getSize().x),
                                                    static_cast<int>(textura.getSize().y)}));
        sprite.setScale({0.15f, 0.15f});
        esconder();
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused || !activa) return;
        
        // Mueve la chancla hacia la izquierda
        sprite.move({-velocidad, 0.f});
        
        // Si la chancla sale de la pantalla por la izquierda, se esconde
        if (sprite.getPosition().x < -100) {
            esconder();
        }
    }

    void draw(sf::RenderWindow& window) {
        if (activa) {
            window.draw(sprite);
        }
    }

    // Esta función la usará la abuelita para disparar
    void lanzar(float startX, float startY) {
        sprite.setPosition({startX, startY});
        activa = true;
    }

    void esconder() {
        sprite.setPosition({-1000.f, -1000.f}); // La mandamos fuera de la pantalla
        activa = false;
    }

    bool isActiva() const { return activa; }
    const sf::Sprite& getSprite() const { return sprite; }

    // Hitbox reducido al 50% central para colisiones más justas
    sf::FloatRect getHitbox() const {
        sf::FloatRect b = sprite.getGlobalBounds();
        float mx = b.size.x * 0.25f;
        float my = b.size.y * 0.25f;
        return {{b.position.x + mx, b.position.y + my}, {b.size.x * 0.5f, b.size.y * 0.5f}};
    }
};