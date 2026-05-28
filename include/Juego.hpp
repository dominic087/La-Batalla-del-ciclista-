#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>
#include <cstdlib>

class Ciclista {
private:
    sf::Texture texturaCiclista;
    sf::Sprite sprite;
    float velocidadCiclista = 7.5f; // Va más rápido que los baches para alcanzarte

public:
    Ciclista() {
        // Temporalmente usamos la imagen del fantasma hasta que agregues el arte del ciclista
        if (!texturaCiclista.loadFromFile("assets/image/ghost.png")) {
            throw std::runtime_error("Error al cargar textura del ciclista");
        }
        sprite.setTexture(texturaCiclista);
        
        // Ajustamos el parámetro de escala exactamente a 0.35 para que el tamaño
        // del ciclista tenga la proporción correcta frente a la motobici.
        sprite.scale({0.35f, 0.35f});
        
        // Lo posicionamos fuera de la pantalla, en el carril del suelo
        sprite.setPosition({1500.f, 210.f}); 
    }

    void update(bool gameStarted, bool gamePaused) {
        if (!gameStarted || gamePaused) return;
        
        // El ciclista avanza hacia ti rápidamente ("frenando de golpe" en tu cara)
        sprite.move({-velocidadCiclista, 0.f});
        
        // Si el ciclista sale de la pantalla, vuelve a aparecer después de un tiempo aleatorio
        if (sprite.getPosition().x < -500) {
            float distanciaAleatoria = static_cast<float>(rand() % 1000);
            sprite.setPosition({1500.f + distanciaAleatoria, 210.f});
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    void reset() {
        sprite.setPosition({1500.f, 210.f});
    }

    sf::Sprite& getSprite() { return sprite; }
};