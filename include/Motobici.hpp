#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>
#include <cmath>

class Motobici {
private:
    sf::Texture textura;
    sf::Sprite sprite;
    float originalY = 200.f; 
    float velocidadY = 0.f;
    float gravedad = 0.4f;
    float fuerzaSalto      = -12.2f;
    float fuerzaSaltoDoble = -15.5f;
    bool isJumping     = false;
    bool canDoubleJump = false;
    bool spaceReleased = true;
    float bateria = 100.0f; 

    // Variables para los Power-Ups
    float speedFactor = 1.0f;
    int speedTimer = 0;

    // Animación de moto
    float animTimer = 0.f;

    bool bossFightMode = false;

public:
    Motobici() : sprite(textura) {
        if (!textura.loadFromFile("assets/image/dinosaur/dino 1.png")) {
            throw std::runtime_error("Error al cargar asset de la motobici");
        }
        textura.setSmooth(false);
        // El sprite se construyó con la textura vacía; hay que actualizar su rect
        sprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(textura.getSize().x),
                                                    static_cast<int>(textura.getSize().y)}));
        sprite.setScale({0.35f, 0.35f});

        // Calcular el nivel del suelo para que el jugador quede parado encima
        sf::Texture groundTexture;
        if (!groundTexture.loadFromFile("assets/image/ground.png"))
            throw std::runtime_error("Error al cargar textura del piso en Motobici");
        const sf::Vector2u groundSize = groundTexture.getSize();
        float roadScale = 800.f / static_cast<float>(groundSize.x);
        float groundY   = 400.f - 100.f * roadScale;
        // Las ruedas están al ~88% de la altura del sprite (hay ~12% de espacio vacío abajo)
        // Posicionamos para que las ruedas queden exactamente en el nivel del asfalto
        originalY = groundY - sprite.getGlobalBounds().size.y * 0.68f;

        sprite.setPosition({20.f, originalY});
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;

        // Durante pelea de jefe el drenaje pasivo se pausa
        // Durante turbo activo el drenaje baja a la mitad
        if (!bossFightMode) {
            bateria -= (speedTimer > 0) ? 0.012f : 0.028f;
            if (bateria < 0) bateria = 0;
        }

        if (speedTimer > 0) {
            speedTimer--;
            if (speedTimer <= 0) speedFactor = 1.0f;
        }

        // Detectar flanco de subida de Space (requiere soltar y volver a pulsar)
        bool spaceNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
        if (!spaceNow) spaceReleased = true;

        if (spaceNow && spaceReleased) {
            spaceReleased = false;
            float costo = bossFightMode ? 8.0f : 1.0f;
            if (!isJumping) {
                // Primer salto
                velocidadY = fuerzaSalto;
                isJumping = true;
                canDoubleJump = true;
                bateria -= costo;
                if (bateria < 0.f) bateria = 0.f;
            } else if (canDoubleJump) {
                // Doble salto — más alto
                velocidadY = fuerzaSaltoDoble;
                canDoubleJump = false;
                bateria -= costo;
                if (bateria < 0.f) bateria = 0.f;
            }
        }

        velocidadY += gravedad;
        sprite.move({0.f, velocidadY});

        if (sprite.getPosition().y >= originalY) {
            sprite.setPosition({sprite.getPosition().x, originalY});
            isJumping     = false;
            velocidadY    = 0.f;
            canDoubleJump = false;
        }

        // Vibración de moto: el sprite se mantiene pegado al suelo con una leve vibración
        if (!isJumping) {
            animTimer += 0.4f * speedFactor;
            // (1 - cos) oscila entre 0 y +2: el sprite solo se hunde ligeramente en el asfalto
            // nunca sube por encima de originalY, así las ruedas siempre tocan el suelo
            float bounce = (1.f - std::cos(animTimer * 2.f)) * 0.7f;
            float tilt   = std::sin(animTimer * 0.5f) * 0.8f;
            sprite.setPosition({sprite.getPosition().x, originalY + bounce});
            sprite.setRotation(sf::degrees(tilt));
        } else {
            sprite.setRotation(sf::degrees(0.f));
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    // ==========================================
    // AQUÍ ESTÁN LAS FUNCIONES QUE PIDE TU MAIN
    // ==========================================

    sf::FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

    float getSpeedFactor() const {
        return speedFactor;
    }

    void applyBatteryPickup(float amount) {
        bateria += amount;
        if (bateria > 100.0f) bateria = 100.0f; 
    }

    void applySpeedPickup(float boostAmount, int duration) {
        speedFactor = boostAmount;
        speedTimer = duration; 
    }

    void applyCollisionPenalty() {
        bateria -= 15.0f;
        if (bateria < 0) bateria = 0;
    }

    void applyBossDamage(float percent) {
        bateria -= percent;
        if (bateria < 0.f) bateria = 0.f;
    }

    void setBossFightMode(bool active) { bossFightMode = active; }

    // ==========================================

    bool checkCollision(const sf::Sprite& obstacle) {
        return sprite.getGlobalBounds().findIntersection(obstacle.getGlobalBounds()).has_value();
    }

    void reset() {
        sprite.setPosition({20.f, originalY});
        sprite.setRotation(sf::degrees(0.f));
        velocidadY    = 0.f;
        isJumping     = false;
        canDoubleJump = false;
        spaceReleased = true;
        bateria       = 100.0f;
        speedFactor   = 1.0f;
        speedTimer    = 0;
        animTimer     = 0.f;
        bossFightMode = false;
    }

    float getBateria() const { return bateria; }
    const sf::Sprite& getSprite() const { return sprite; }
};