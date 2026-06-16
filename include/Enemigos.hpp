#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <stdexcept>

class Enemigos {
private:
    sf::Texture texturaAbuelita;
    sf::Texture texturaGhost;
    sf::Texture texturaTaxista;

    sf::Sprite spriteAbuelita;
    sf::Sprite spriteGhost;
    sf::Sprite spriteTaxista;

    bool abuelitaActiva   = false;
    bool ghostActivo      = false;
    bool taxistaActivo    = false;
    bool abuelitaAparecio = false;
    bool ghostAparecio    = false;
    bool taxistaAparecio  = false;

    float velocidad = 4.0f;
    float enemyY    = 0.f;

    int shootTimer = 0;
    static constexpr int shootInterval = 150;
    std::optional<sf::Vector2f> pendingShot;

public:
    Enemigos() : spriteAbuelita(texturaAbuelita), spriteGhost(texturaGhost), spriteTaxista(texturaTaxista) {
        if (!texturaAbuelita.loadFromFile("assets/image/abuelita.png"))
            throw std::runtime_error("Error al cargar assets de la abuelita");
        if (!texturaGhost.loadFromFile("assets/image/ghost.png"))
            throw std::runtime_error("Error al cargar assets del fantasma");
        if (!texturaTaxista.loadFromFile("assets/image/taxista.png"))
            throw std::runtime_error("Error al cargar assets del taxista");

        sf::Texture groundTexture;
        if (!groundTexture.loadFromFile("assets/image/ground.png"))
            throw std::runtime_error("Error al cargar textura del piso para calcular posición");

        const int groundRectHeight = 100;
        const sf::Vector2u groundSize = groundTexture.getSize();
        float roadScale = 800.f / static_cast<float>(groundSize.x);
        float groundY   = 400.f - static_cast<float>(groundRectHeight) * roadScale;
        enemyY = groundY - 45.f;

        spriteAbuelita.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(texturaAbuelita.getSize().x),
                                                            static_cast<int>(texturaAbuelita.getSize().y)}));
        spriteGhost.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(texturaGhost.getSize().x),
                                                         static_cast<int>(texturaGhost.getSize().y)}));
        spriteTaxista.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(texturaTaxista.getSize().x),
                                                           static_cast<int>(texturaTaxista.getSize().y)}));
        spriteAbuelita.setScale({0.22f, 0.22f});
        spriteGhost.setScale({0.35f, 0.35f});
        spriteTaxista.setScale({0.35f, 0.35f});

        reset();
    }

    void update(bool gameStarted, bool gamePaused, int distancia, float speedFactor) {
        if (!gameStarted || gamePaused) return;

        if (!abuelitaAparecio && distancia >= 370) {
            spriteAbuelita.setPosition({900.f, enemyY});
            abuelitaActiva   = true;
            abuelitaAparecio = true;
        }
        if (!ghostAparecio && distancia >= 710) {
            spriteGhost.setPosition({900.f, enemyY});
            ghostActivo   = true;
            ghostAparecio = true;
        }

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

        if (ghostActivo) {
            spriteGhost.move({-velocidad * 1.5f * speedFactor, 0.f});
            if (spriteGhost.getPosition().x < -100.f)
                spriteGhost.setPosition({900.f, enemyY});
        }
    }

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
        if (taxistaActivo)  window.draw(spriteTaxista);
    }

    void reset() {
        abuelitaActiva   = false;
        ghostActivo      = false;
        taxistaActivo    = false;
        abuelitaAparecio = false;
        ghostAparecio    = false;
        taxistaAparecio  = false;
        shootTimer       = 0;
        pendingShot.reset();
    }

    std::vector<sf::Sprite> getEnemies() const {
        std::vector<sf::Sprite> result;
        if (abuelitaActiva) result.push_back(spriteAbuelita);
        if (ghostActivo)    result.push_back(spriteGhost);
        if (taxistaActivo)  result.push_back(spriteTaxista);
        return result;
    }

    // ── Jefe 1: Abuelita ──
    void activateBossFight() {
        abuelitaActiva   = true;
        abuelitaAparecio = true;
        spriteAbuelita.setPosition({650.f, enemyY});
    }

    void dismissAbuelita() { abuelitaActiva = false; }

    sf::Vector2f getAbuelitaPosition() const { return spriteAbuelita.getPosition(); }

    // ── Jefe 2: El Ciclista Kamikaze ──
    void activateCiclistaBoss() {
        ghostActivo   = true;
        ghostAparecio = true;
        spriteGhost.setPosition({900.f, enemyY});
    }

    void moveCiclista(float speed) { spriteGhost.move({-speed, 0.f}); }

    bool isCiclistaOffScreen() const {
        float rightEdge = spriteGhost.getPosition().x + spriteGhost.getGlobalBounds().size.x;
        return rightEdge < 0.f;
    }

    void hideCiclista()        { spriteGhost.setPosition({-200.f, enemyY}); }
    void resetCiclistaForPass(){ spriteGhost.setPosition({900.f, enemyY}); }
    void dismissCiclista()     { ghostActivo = false; }

    const sf::Sprite& getCiclistaSprite() const { return spriteGhost; }

    // ── Jefe 3: El Taxista (jefe final) ──
    void activateTaxistaBoss() {
        taxistaActivo   = true;
        taxistaAparecio = true;
        spriteTaxista.setPosition({900.f, enemyY});
    }

    void moveTaxista(float speed) { spriteTaxista.move({-speed, 0.f}); }

    bool isTaxistaOffScreen() const {
        float rightEdge = spriteTaxista.getPosition().x + spriteTaxista.getGlobalBounds().size.x;
        return rightEdge < 0.f;
    }

    void hideTaxista()        { spriteTaxista.setPosition({-200.f, enemyY}); }
    void resetTaxistaForPass(){ spriteTaxista.setPosition({900.f, enemyY}); }
    void dismissTaxista()     { taxistaActivo = false; }

    const sf::Sprite& getTaxistaSprite() const { return spriteTaxista; }
};
