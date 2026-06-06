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
        fondo1.setPosition({0.f, 0.f});
        fondo2.setPosition({800.f, 0.f});
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;
        
        fondo1.move({-velocidad, 0.f});
        fondo2.move({-velocidad, 0.f});

        if (fondo1.getPosition().x <= -800) fondo1.setPosition({800.f, 0.f});
        if (fondo2.getPosition().x <= -800) fondo2.setPosition({800.f, 0.f});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(fondo1);
        window.draw(fondo2);
    }
};
