#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <stdexcept>

class Enemigos {
private:
    sf::Texture texturaAbuelita;
    sf::Texture texturaGhost;

    sf::Sprite spriteAbuelita;
    sf::Sprite spriteGhost;

    bool abuelitaActiva   = false;
    bool ghostActivo      = false;
    bool abuelitaAparecio = false;
    bool ghostAparecio    = false;

    float velocidad = 4.0f;
    float enemyY    = 0.f;

    int shootTimer = 0;
    static constexpr int shootInterval = 150; // dispara cada ~2.5s a 60fps
    std::optional<sf::Vector2f> pendingShot;

public:
    Enemigos() : spriteAbuelita(texturaAbuelita), spriteGhost(texturaGhost) {
        if (!texturaAbuelita.loadFromFile("assets/image/abuelita.png"))
            throw std::runtime_error("Error al cargar assets de la abuelita");
        if (!texturaGhost.loadFromFile("assets/image/ghost.png"))
            throw std::runtime_error("Error al cargar assets del fantasma");

        sf::Texture groundTexture;
        if (!groundTexture.loadFromFile("assets/image/ground.png"))
            throw std::runtime_error("Error al cargar textura del piso para calcular posición");

        const int groundRectHeight = 100;
        const sf::Vector2u groundSize = groundTexture.getSize();
        float roadScale = 800.f / static_cast<float>(groundSize.x);
        float groundY   = 400.f - static_cast<float>(groundRectHeight) * roadScale;
        enemyY = groundY - 45.f;

        // Actualizar rects — los sprites se construyeron con texturas vacías
        spriteAbuelita.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(texturaAbuelita.getSize().x),
                                                            static_cast<int>(texturaAbuelita.getSize().y)}));
        spriteGhost.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(texturaGhost.getSize().x),
                                                         static_cast<int>(texturaGhost.getSize().y)}));
        spriteAbuelita.setScale({0.22f, 0.22f});
        spriteGhost.setScale({0.35f, 0.35f});

        reset();
    }

    void update(bool gameStarted, bool gamePaused, int distancia, float speedFactor) {
        if (!gameStarted || gamePaused) return;

        // Aparición por distancia
        if (!abuelitaAparecio && distancia >= 370) {
            spriteAbuelita.setPosition({900.f, enemyY});
            abuelitaActiva   = true;
            abuelitaAparecio = true;
        }
        if (!ghostAparecio && distancia >= 770) {
            spriteGhost.setPosition({900.f, enemyY});
            ghostActivo   = true;
            ghostAparecio = true;
        }

        // Abuelita: se mueve, hace respawn al salir, dispara chancla
        if (abuelitaActiva) {
            spriteAbuelita.move({-velocidad * speedFactor, 0.f});
            if (spriteAbuelita.getPosition().x < -100.f) {
                spriteAbuelita.setPosition({900.f, enemyY});
                shootTimer = 0;
            } else {
                shootTimer++;
                if (shootTimer >= shootInterval) {
                    shootTimer  = 0;
                    pendingShot = spriteAbuelita.getPosition();
                }
            }
        }

        // Ghost (ciclista): más rápido, también hace respawn
        if (ghostActivo) {
            spriteGhost.move({-velocidad * 1.5f * speedFactor, 0.f});
            if (spriteGhost.getPosition().x < -100.f)
                spriteGhost.setPosition({900.f, enemyY});
        }
    }

    // Devuelve true y rellena outPos cuando la abuelita dispara
    bool pollShootEvent(sf::Vector2f& outPos) {
        if (pendingShot.has_value()) {
            outPos = pendingShot.value();
            pendingShot.reset();
            return true;
        }
        return false;
    }

    void draw(sf::RenderWindow& window) {
        if (abuelitaActiva) window.draw(spriteAbuelita);
        if (ghostActivo)    window.draw(spriteGhost);
    }

    void reset() {
        abuelitaActiva   = false;
        ghostActivo      = false;
        abuelitaAparecio = false;
        ghostAparecio    = false;
        shootTimer       = 0;
        pendingShot.reset();
    }

    std::vector<sf::Sprite> getEnemies() const {
        std::vector<sf::Sprite> result;
        if (abuelitaActiva) result.push_back(spriteAbuelita);
        if (ghostActivo)    result.push_back(spriteGhost);
        return result;
    }

    // Posiciona a la abuelita en pantalla para la pelea de jefe
    void activateBossFight() {
        abuelitaActiva   = true;
        abuelitaAparecio = true;
        spriteAbuelita.setPosition({650.f, enemyY});
    }

    // Quita a la abuelita tras la pelea
    void dismissAbuelita() {
        abuelitaActiva = false;
    }

    sf::Vector2f getAbuelitaPosition() const {
        return spriteAbuelita.getPosition();
    }

    // ── Jefe 2: El Ciclista Kamikaze ──
    void activateCiclistaBoss() {
        ghostActivo   = true;
        ghostAparecio = true;
        spriteGhost.setPosition({900.f, enemyY});
    }

    void moveCiclista(float speed) {
        spriteGhost.move({-speed, 0.f});
    }

    bool isCiclistaOffScreen() const {
        return spriteGhost.getPosition().x < -100.f;
    }

    void hideCiclista() {
        spriteGhost.setPosition({-200.f, enemyY});
    }

    void resetCiclistaForPass() {
        spriteGhost.setPosition({900.f, enemyY});
    }

    void dismissCiclista() {
        ghostActivo = false;
    }

    const sf::Sprite& getCiclistaSprite() const { return spriteGhost; }
};
