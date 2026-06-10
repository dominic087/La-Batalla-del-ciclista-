#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <stdexcept>

class Enemigos {
private:
    sf::Texture texturaAbuelita;
    sf::Texture texturaGhost;
    std::vector<sf::Sprite> enemigos;
    float velocidad = 4.0f;
    float enemyY = 0.f;

public:
    Enemigos() {
        if (!texturaAbuelita.loadFromFile("assets/image/abuelita.png")) {
            throw std::runtime_error("Error al cargar assets de la abuelita");
        }
        if (!texturaGhost.loadFromFile("assets/image/ghost.png")) {
            throw std::runtime_error("Error al cargar assets del fantasma");
        }

        // Cargar textura del piso para calcular posición Y exacta
        sf::Texture groundTexture;
        if (!groundTexture.loadFromFile("assets/image/ground.png")) {
            throw std::runtime_error("Error al cargar textura del piso para calcular posición");
        }

        // Calcular posición Y usando la misma lógica que Calle.hpp
        const int groundRectHeight = 100;
        const sf::Vector2u groundSize = groundTexture.getSize();
        float roadScale = 800.f / static_cast<float>(groundSize.x);
        float groundY = 400.f - static_cast<float>(groundRectHeight) * roadScale;

        // Los enemigos deben estar justo en el TOP del piso
        enemyY = groundY;

        reset();
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;

        for (auto& enemy : enemigos) {
            enemy.move({-velocidad, 0.f});
            if (enemy.getPosition().x < -100) {
                // Alternar entre abuelita y fantasma
                bool useAbuelita = (rand() % 2 == 0);
                if (useAbuelita) {
                    enemy.setTexture(texturaAbuelita);
                } else {
                    enemy.setTexture(texturaGhost);
                }
                enemy.setPosition({900.f + (rand() % 300), enemyY});
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        for (auto& enemy : enemigos) {
            window.draw(enemy);
        }
    }

    void reset() {
        enemigos.clear();
        for (int i = 0; i < 2; ++i) {
            bool useAbuelita = (i % 2 == 0);
            sf::Sprite s(useAbuelita ? texturaAbuelita : texturaGhost);
            s.setScale({0.15f, 0.15f});
            s.setPosition({800.f + (i * 600.f), enemyY});
            enemigos.push_back(s);
        }
    }

    const std::vector<sf::Sprite>& getEnemies() const { return enemigos; }
};
