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
    bool abuelitaAparecio = false;
    bool ciclstaAparecio = false;

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

        // Los enemigos deben estar centrados sobre la calle
        // Calcular una posición que los centre entre el espacio disponible
        enemyY = groundY - 80.f;  // Posición centrada sobre la calle pero visible

        reset();
    }

    void update(bool gameStarted, bool gamePaused, int distancia, float speedFactor) {
        if (!gameStarted || gamePaused) return;

        // Abuelita aparece a los 370 metros
        if (!abuelitaAparecio && distancia >= 370) {
            sf::Sprite s(texturaAbuelita);
            s.setScale({0.35f, 0.35f});
            s.setPosition({800.f, enemyY});
            enemigos.push_back(s);
            abuelitaAparecio = true;
        }

        // Ciclista (ghost) aparece a los 770 metros
        if (!ciclstaAparecio && distancia >= 770) {
            sf::Sprite s(texturaGhost);
            s.setScale({0.35f, 0.35f});
            s.setPosition({800.f, enemyY});
            enemigos.push_back(s);
            ciclstaAparecio = true;
        }

        for (auto& enemy : enemigos) {
            enemy.move({-velocidad * speedFactor, 0.f});
        }
    }

    void draw(sf::RenderWindow& window) {
        for (auto& enemy : enemigos) {
            window.draw(enemy);
        }
    }

    void reset() {
        enemigos.clear();
        abuelitaAparecio = false;
        ciclstaAparecio = false;
    }

    const std::vector<sf::Sprite>& getEnemies() const { return enemigos; }
};
