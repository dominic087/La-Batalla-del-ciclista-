#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <stdexcept>

class Bache {
private:
    sf::Texture textura;
    std::vector<sf::Sprite> baches;
    float velocidad = 6.0f;
    float obstacleY = 0.f;

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

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;
        
        for (auto& bache : baches) {
            bache.move({-velocidad, 0.f});
            if (bache.getPosition().x < -50) {
                bache.setPosition({900.f + (rand() % 400), obstacleY});
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
        for (int i = 0; i < 3; ++i) {
            sf::Sprite s(textura);
            s.setScale({0.25f, 0.25f});
            s.setPosition({800.f + (i * 400.f), obstacleY});
            baches.push_back(s);
        }
    }

    const std::vector<sf::Sprite>& getObstacles() const { return baches; }
};