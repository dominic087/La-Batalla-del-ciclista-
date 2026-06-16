#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <stdexcept>

class Bache {
private:
    sf::Texture textura;
    std::vector<sf::Sprite> baches;
    float velocidad = 6.0f;
    float obstacleY = 0.f;
    int respawnCount = 0;
    std::vector<float> respawnPositions;
    static constexpr float pixelsPerMeter = 4.f;
    static constexpr float minObstacleDistance = 1.f * pixelsPerMeter;

public:
    Bache() {
        if (!textura.loadFromFile("assets/image/obstacle.png")) {
            throw std::runtime_error("Error al cargar assets del bache");
        }
        
        // Cargar textura del piso para calcular su posición Y exacta
        sf::Texture groundTexture;
        if (!groundTexture.loadFromFile("assets/image/ground.png")) {
            throw std::runtime_error("Error al cargar textura del piso para calcular posición");
        }
        
        // Calcular posición Y usando la misma lógica que Calle.hpp
        const int groundRectHeight = 100;
        const sf::Vector2u groundSize = groundTexture.getSize();
        float roadScale = 800.f / static_cast<float>(groundSize.x);
        float groundY = 400.f - static_cast<float>(groundRectHeight) * roadScale;
        
        // Los obstáculos deben estar justo en el TOP del piso
        obstacleY = groundY;
        
        reset();
    }

    void update(bool gameStarted, bool gamePaused, float speedFactor, bool spawnEnabled = true) {
        if (!gameStarted || gamePaused) return;

        for (auto& bache : baches) {
            bache.move({-velocidad * speedFactor, 0.f});
            if (bache.getPosition().x < -50) {
                if (spawnEnabled) {
                    float nextX = 900.f + minObstacleDistance + static_cast<float>(rand() % 100);
                    bache.setPosition({nextX, obstacleY});
                    respawnPositions.push_back(nextX);
                    respawnCount += 1;
                } else {
                    // Aparcar fuera de pantalla sin respawnear
                    bache.setPosition({2000.f, obstacleY});
                }
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        for (auto& bache : baches) {
            window.draw(bache);
        }
    }

    void reset() {
        baches.clear();
        respawnCount = 0;
        respawnPositions.clear();
        sf::Sprite s(textura);
        s.setScale({0.18f, 0.18f});
        // Ajustar el origen al fondo del sprite para que `obstacleY` represente la base sobre la calle
        s.setOrigin({0.f, s.getGlobalBounds().size.y});
        s.setPosition({800.f, obstacleY});
        baches.push_back(s);
    }

    int consumeRespawnCount() {
        int count = respawnCount;
        respawnCount = 0;
        return count;
    }

    std::vector<float> consumeRespawnPositions() {
        std::vector<float> positions = std::move(respawnPositions);
        respawnPositions.clear();
        return positions;
    }

    const std::vector<sf::Sprite>& getObstacles() const { return baches; }
};