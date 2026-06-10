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
    int slowTimer = 0;
    int speedBoostTimer = 0;
    float speedBoostAmount = 0.f;
    const float baseSpeedKmh = 20.f;

public:
    Motobici() : sprite(textura) {
        if (!textura.loadFromFile("assets/image/dinosaur/dino 1.png")) {
            throw std::runtime_error("Error al cargar asset de la motobici");
        }

        sprite = sf::Sprite(textura);
        sprite.setScale({0.35f, 0.35f});

        sf::Texture groundTexture;
        if (!groundTexture.loadFromFile("assets/image/ground.png")) {
            throw std::runtime_error("Error al cargar textura del piso para calcular posición del personaje");
        }
        const int groundRectHeight = 100;
        float roadScale = 800.f / static_cast<float>(groundTexture.getSize().x);
        float groundY = 400.f - static_cast<float>(groundRectHeight) * roadScale;

        // Poner la base del sprite sobre la calle
        float spriteHeight = sprite.getGlobalBounds().size.y;
        sprite.setOrigin({0.f, spriteHeight});
        originalY = groundY;
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

        if (slowTimer > 0) {
            slowTimer -= 1;
        }

        if (speedBoostTimer > 0) {
            speedBoostTimer -= 1;
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    // Sintaxis corregida para std::optional de SFML 3
    bool checkCollision(const sf::Sprite& obstaculo) {
        return sprite.getGlobalBounds().findIntersection(obstaculo.getGlobalBounds()).has_value();
    }

    bool checkCollision(const sf::FloatRect& bounds) {
        return sprite.getGlobalBounds().findIntersection(bounds).has_value();
    }

    sf::FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

    float getBateria() const { return nivelBateria; }

    float getSpeedFactor() const {
        if (slowTimer > 0) return 0.5f;
        if (speedBoostTimer > 0) return 1.3f;
        return 1.0f;
    }

    float getSpeedKmh() const {
        float speed = baseSpeedKmh;
        if (speedBoostTimer > 0) speed += speedBoostAmount;
        if (slowTimer > 0) speed -= 8.f;
        if (speed < 0.f) speed = 0.f;
        if (speed > 35.f) speed = 35.f;
        return speed;
    }

    void applyCollisionPenalty() {
        nivelBateria -= 15.0f;
        if (nivelBateria < 0.0f) {
            nivelBateria = 0.0f;
        }
        slowTimer = 180; // 3 segundos a 60fps
    }

    void applyBatteryPickup(float amount) {
        nivelBateria += amount;
        if (nivelBateria > 100.f) nivelBateria = 100.f;
    }

    void applySpeedPickup(float amount, int durationFrames) {
        speedBoostAmount = amount;
        speedBoostTimer = durationFrames;
    }

    void reset() {
        nivelBateria = 100.0f;
        sprite.setPosition({20.f, originalY});
        estaSaltando = false;
        velocidadSalto = -10.0f;
        slowTimer = 0;
        speedBoostTimer = 0;
        speedBoostAmount = 0.f;
    }
};