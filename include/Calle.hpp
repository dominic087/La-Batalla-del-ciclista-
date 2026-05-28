#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>

class Calle {
private:
    sf::Texture textura;
    sf::Sprite calle1, calle2;
    float velocidad = 6.0f;

public:
    Calle() {
        if (!textura.loadFromFile("assets/image/ground.png")) {
            throw std::runtime_error("Error al cargar assets de la calle");
        }
        textura.setRepeated(true);
        calle1.setTexture(textura);
        calle2.setTexture(textura);
        calle1.setPosition({0.f, 320.f});
        calle2.setPosition({800.f, 320.f});
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;
        
        calle1.move({-velocidad, 0.f});
        calle2.move({-velocidad, 0.f});
        
        if (calle1.getPosition().x <= -800.f) calle1.setPosition({800.f, 320.f});
        if (calle2.getPosition().x <= -800.f) calle2.setPosition({800.f, 320.f});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(calle1);
        window.draw(calle2);
    }
};