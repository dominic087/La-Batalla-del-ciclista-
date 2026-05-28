#pragma once
#include <SFML/Graphics.hpp>

class Ciudad {
private:
    sf::Texture textura;
    sf::Sprite fondo1, fondo2;
    float velocidad = 1.0f; 

public:
    Ciudad() {
        if (textura.loadFromFile("assets/image/cloud.jpg")) {
            textura.setRepeated(true);
            fondo1.setTexture(textura);
            fondo2.setTexture(textura);
        }
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