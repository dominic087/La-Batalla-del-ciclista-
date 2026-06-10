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

    void update(bool gameStarted, bool gamePaused, float speedFactor) {
        if (!gameStarted || gamePaused) return;
        
        for (auto& bache : baches) {
            bache.move({-velocidad * speedFactor, 0.f});
            if (bache.getPosition().x < -50) {
                float maxX = 0.f;
                for (const auto& other : baches) {
                    if (&other != &bache) {
                        maxX = std::max(maxX, other.getPosition().x);
                    }
                }
                float nextX = std::max(900.f + static_cast<float>(rand() % 400), maxX + 350.f);
                bache.setPosition({nextX, obstacleY});
                respawnCount += 1;
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
        for (int i = 0; i < 2; ++i) {
            sf::Sprite s(textura);
            s.setScale({0.18f, 0.18f});
            s.setPosition({800.f + (i * 450.f), obstacleY});
            baches.push_back(s);
        }
    }

    int consumeRespawnCount() {
        int count = respawnCount;
        respawnCount = 0;
        return count;
    }

    const std::vector<sf::Sprite>& getObstacles() const { return baches; }
};