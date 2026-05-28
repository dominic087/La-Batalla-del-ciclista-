#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

#include "Motobici.hpp"
#include "Calle.hpp"
#include "Ciudad.hpp"
#include "Bache.hpp"
#include "Ciclista.hpp"

class Juego {
private:
    sf::RenderWindow window;
    Motobici jugador;
    Calle calle;
    Ciudad ciudad;
    Bache baches;
    Ciclista jefeCiclista;

    sf::Texture titleTexture;
    sf::Sprite titleSprite;
    sf::Music backgroundMusic;
    sf::Font font;
    sf::Text distanciaText;
    sf::Text bateriaText;

    int metrosRecorridos = 0;
    int scoreTick = 0;
    bool gameStarted = false;
    bool gamePaused = false;

public:
    Juego() : 
        window(sf::VideoMode({800, 400}), "La Batalla del Ciclista"),
        titleSprite(titleTexture),
        distanciaText(font),
        bateriaText(font) 
    {
        window.setFramerateLimit(60);
        
        if (titleTexture.loadFromFile("assets/image/title.jpg")) {
            titleSprite.setPosition({-20.f, 0.f});
            titleSprite.setScale({0.65f, 0.65f});
        }
        
        if (backgroundMusic.openFromFile("assets/audio/melody.mp3")) {
            backgroundMusic.setLoopPoints({sf::milliseconds(500), sf::seconds(2000)});
            backgroundMusic.play();
        }
        
        if (font.openFromFile("assets/fonts/font.ttf")) {
            distanciaText.setCharacterSize(22);
            distanciaText.setFillColor(sf::Color::White);
            distanciaText.setPosition({30.f, 10.f});

            bateriaText.setCharacterSize(22);
            bateriaText.setFillColor(sf::Color::Green);
            bateriaText.setPosition({550.f, 10.f});
        }
    }

    void ejecutar() {
        while (window.isOpen()) {
            procesarEventos();
            actualizar();
            renderizar();
        }
    }

private:
    void procesarEventos() {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        if (!gameStarted) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                gameStarted = true;
            }
        } else if (gamePaused) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                gamePaused = false;
                jugador.reset();
                baches.reset();
                jefeCiclista.reset();
                metrosRecorridos = 0;
            }
        }
    }

    void actualizar() {
        if (gameStarted && !gamePaused) {
            jugador.update(gameStarted, gamePaused);
            calle.update(gameStarted, gamePaused);
            ciudad.update(gameStarted, gamePaused);
            baches.update(gameStarted, gamePaused);
            jefeCiclista.update(gameStarted, gamePaused);

            distanciaText.setString("Distancia: " + std::to_string(metrosRecorridos) + "m");
            bateriaText.setString("Bateria: " + std::to_string(static_cast<int>(jugador.getBateria())) + "%");

            if (jugador.getBateria() < 30.0f) {
                bateriaText.setFillColor(sf::Color::Red);
            } else {
                bateriaText.setFillColor(sf::Color::Green);
            }

            for (const auto& bache : baches.getObstacles()) {
                if (jugador.checkCollision(bache)) {
                    gamePaused = true;
                }
            }

            if (jugador.checkCollision(jefeCiclista.getSprite())) {
                gamePaused = true;
            }

            if (jugador.getBateria() <= 0) {
                gamePaused = true;
            }

            scoreTick++;
            if (scoreTick >= 6) {
                metrosRecorridos++;
                scoreTick = 0;
            }
        }
    }

    void renderizar() {
        window.clear(sf::Color(135, 206, 235));
        
        if (!gameStarted) {
            window.draw(titleSprite);
        } else {
            ciudad.draw(window);
            jefeCiclista.draw(window);
            calle.draw(window);
            baches.draw(window);
            jugador.draw(window);
            window.draw(distanciaText);
            window.draw(bateriaText);
        }
        
        window.display();
    }
};