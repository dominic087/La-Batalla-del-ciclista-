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

public:
    Bache() {
        if (!textura.loadFromFile("assets/image/obstacle.png")) {
            throw std::runtime_error("Error al cargar assets del bache");
        }
        reset();
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;
        
        for (auto& bache : baches) {
            bache.move({-velocidad, 0.f});
            if (bache.getPosition().x < -50) {
                bache.setPosition({900.f + (rand() % 400), 280.f});
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
            s.setScale({0.1f, 0.1f});
            s.setPosition({800.f + (i * 400.f), 280.f});
            baches.push_back(s);
        }
    }

    const std::vector<sf::Sprite>& getObstacles() const { return baches; }
};