#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <stdexcept>

class Motobici {
private:
    sf::Texture textura;
    sf::Sprite sprite;
    
    // Físicas y Batería
    float nivelBateria = 100.0f;
    bool estaSaltando = false;
    float velocidadSalto = -10.0f;
    float gravedad = 0.3f;
    float originalY = 200.f;

public:
    Motobici() {
        if (!textura.loadFromFile("assets/image/dinosaur/dino 1.png")) {
            throw std::runtime_error("Error al cargar asset de la motobici");
        }
        sprite.setTexture(textura);
        sprite.scale({0.25f, 0.25f});
        sprite.setPosition({20.f, originalY});
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;

        // Consumo constante de batería por avance
        if (nivelBateria > 0) {
            nivelBateria -= 0.02f;
        }

        // Lógica de Salto
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !estaSaltando) {
            estaSaltando = true;
            nivelBateria -= 1.0f; // Salto gasta batería extra
        }
        
        if (estaSaltando) {
            sprite.move({0.f, velocidadSalto});
            velocidadSalto += gravedad;
            if (sprite.getPosition().y >= originalY) {
                sprite.setPosition({20.f, originalY});
                estaSaltando = false;
                velocidadSalto = -10.0f;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    bool checkCollision(const sf::Sprite& obstaculo) {
        return sprite.getGlobalBounds().findIntersection(obstaculo.getGlobalBounds());
    }

    float getBateria() const { return nivelBateria; }

    void reset() {
        nivelBateria = 100.0f;
        sprite.setPosition({20.f, originalY});
        estaSaltando = false;
    }
};