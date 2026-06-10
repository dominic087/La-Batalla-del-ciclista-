#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>

class Motobici {
private:
    sf::Texture textura;
    sf::Sprite sprite;
    float nivelBateria = 100.0f;
    bool estaSaltando = false;
    float velocidadSalto = -10.0f;
    float gravedad = 0.3f;
    float originalY = 200.f;

public:
    Motobici() : sprite(textura) {
        if (!textura.loadFromFile("assets/image/dinosaur/dino 1.png")) {
            throw std::runtime_error("Error al cargar asset de la motobici");
        }

        sprite = sf::Sprite(textura);
        sprite.setScale({0.8f, 0.8f});

        sf::Texture groundTexture;
        if (!groundTexture.loadFromFile("assets/image/ground.png")) {
            throw std::runtime_error("Error al cargar textura del piso para calcular posición del personaje");
        }
        const int groundRectHeight = 100;
        float roadScale = 800.f / static_cast<float>(groundTexture.getSize().x);
        float groundY = 400.f - static_cast<float>(groundRectHeight) * roadScale;

        originalY = groundY - sprite.getGlobalBounds().size.y;
        sprite.setPosition({20.f, originalY});
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;

        if (nivelBateria > 0) {
            nivelBateria -= 0.02f;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !estaSaltando) {
            estaSaltando = true;
            nivelBateria -= 1.0f; 
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

    // Sintaxis corregida para std::optional de SFML 3
    bool checkCollision(const sf::Sprite& obstaculo) {
        return sprite.getGlobalBounds().findIntersection(obstaculo.getGlobalBounds()).has_value();
    }

    float getBateria() const { return nivelBateria; }

    void reset() {
        nivelBateria = 100.0f;
        sprite.setPosition({20.f, originalY});
        estaSaltando = false;
        velocidadSalto = -10.0f;
    }
};