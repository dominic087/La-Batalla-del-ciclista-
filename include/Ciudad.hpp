#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>
#include <algorithm>
#include <cmath>

class Ciudad {
private:
    sf::Texture textura;
    sf::Sprite fondo1, fondo2;
    float velocidad = 1.0f; 
    float spriteWidth = 0.f;
    float offsetX = 0.f;

public:
    Ciudad() : fondo1(textura), fondo2(textura) {
        if (!textura.loadFromFile("assets/image/cloud.jpeg")) {
            throw std::runtime_error("Error al cargar textura de la ciudad");
        }
        const sf::Vector2u textureSize = textura.getSize();
        float cloudScale = std::max(800.f / static_cast<float>(textureSize.x), 400.f / static_cast<float>(textureSize.y));
        // Activar tiling y suavizado para evitar artefactos en los bordes
        textura.setRepeated(true);
        textura.setSmooth(true);
        fondo1.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(textureSize.x, textureSize.y)));
        fondo2.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(textureSize.x, textureSize.y)));
        fondo1.setScale({cloudScale, cloudScale});
        fondo2.setScale({cloudScale, cloudScale});
        fondo1.setPosition({0.f, 0.f});
        // Calcular y almacenar el ancho del sprite escalado y posicionar la segunda copia
        spriteWidth = fondo1.getGlobalBounds().size.x;
        offsetX = 0.f;
        fondo2.setPosition({spriteWidth, 0.f});
    }

    void update(bool gameStarted, bool gamePaused, float speedFactor) {
        if (!gameStarted || gamePaused) return;
        
        // Actualizar offset y normalizar para evitar acumulación de errores de punto flotante
        offsetX -= velocidad * speedFactor;
        if (offsetX <= -spriteWidth) offsetX += spriteWidth;

        // Alineamos a píxeles enteros para evitar seams por muestreo subpíxel
        float drawX = std::floor(offsetX);
        fondo1.setPosition({drawX, 0.f});
        fondo2.setPosition({drawX + spriteWidth, 0.f});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(fondo1);
        window.draw(fondo2);
    }
};
