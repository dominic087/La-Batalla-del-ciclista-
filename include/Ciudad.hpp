#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>

class Ciudad {
private:
    sf::Texture textura;
    sf::Sprite fondo1, fondo2;
    float velocidad = 1.0f; 

public:
    Ciudad() : fondo1(textura), fondo2(textura) {
        if (!textura.loadFromFile("assets/image/cloud.jpeg")) {
            throw std::runtime_error("Error al cargar textura de la ciudad");
        }
        textura.setRepeated(true);
        const float cityScale = 0.75f;
        fondo1.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(800, 400)));
        fondo2.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(800, 400)));
        fondo1.setScale({cityScale, cityScale});
        fondo2.setScale({cityScale, cityScale});
        fondo1.setPosition({0.f, 0.f});
        fondo2.setPosition({800.f * cityScale, 0.f});
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;
        
        fondo1.move({-velocidad, 0.f});
        fondo2.move({-velocidad, 0.f});

        const float cityWidth = 800.f * fondo1.getScale().x;
        if (fondo1.getPosition().x <= -cityWidth) fondo1.setPosition({cityWidth, 0.f});
        if (fondo2.getPosition().x <= -cityWidth) fondo2.setPosition({cityWidth, 0.f});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(fondo1);
        window.draw(fondo2);
    }
};
