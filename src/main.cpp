#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

#include "Motobici.hpp"
#include "Calle.hpp"
#include "Ciudad.hpp"
#include "Bache.hpp"


int main() {
    
    sf::RenderWindow window(sf::VideoMode({800, 400}), "La Batalla del Ciclista");
    window.setFramerateLimit(60);

    
    Motobici jugador;
    Calle calle;
    Ciudad ciudad;
    Bache baches;

    
    sf::Texture titleTexture;
    if (!titleTexture.loadFromFile("assets/image/title.jpg")) {
        return -1; 
    }
    sf::Sprite titleSprite(titleTexture);
    titleSprite.setPosition({-20.f, 0.f});
    titleSprite.setScale({0.65f, 0.65f});

    
    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("assets/audio/melody.mp3")) {
        return -1;
    }
    backgroundMusic.setLoopPoints({sf::milliseconds(500), sf::seconds(2000)});
    backgroundMusic.play();

    
    sf::Font font;
    if (!font.openFromFile("assets/fonts/font.ttf")) {
        return -1;
    }

    sf::Text distanciaText(font);
    distanciaText.setCharacterSize(22);
    distanciaText.setFillColor(sf::Color::White);
    distanciaText.setPosition({30.f, 10.f});

    sf::Text bateriaText(font);
    bateriaText.setCharacterSize(22);
    bateriaText.setFillColor(sf::Color::Green);
    bateriaText.setPosition({550.f, 10.f});

    
    int metrosRecorridos = 0;
    int scoreTick = 0;
    bool gameStarted = false;
    bool gamePaused = false;

    
    while (window.isOpen()) {
        
        
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        
        if (!gameStarted) {
            window.clear(sf::Color(135, 206, 235));
            window.draw(titleSprite);
            window.display();
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                gameStarted = true;
            }
            continue;
        }

        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && gamePaused) {
            gamePaused = false;
            jugador.reset();
            baches.reset();
            metrosRecorridos = 0;
        }

        
        if (gameStarted && !gamePaused) {
            
            // Actualizar posiciones y físicas
            jugador.update(gameStarted, gamePaused);
            calle.update(gameStarted, gamePaused);
            ciudad.update(gameStarted, gamePaused);
            baches.update(gameStarted, gamePaused);

            
            distanciaText.setString("Distancia: " + std::to_string(metrosRecorridos) + "m");
            bateriaText.setString("Bateria: " + std::to_string(static_cast<int>(jugador.getBateria())) + "%");

            if (jugador.getBateria() < 30.0f) {
                bateriaText.setFillColor(sf::Color::Red);
            } else {
                bateriaText.setFillColor(sf::Color::Green);
            }

            // Detección de colisiones contra los baches
            for (const auto& bache : baches.getObstacles()) {
                if (jugador.checkCollision(bache)) {
                    gamePaused = true; 
                }
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
    
        window.clear(sf::Color(135, 206, 235));
        
        ciudad.draw(window);       
        calle.draw(window);        
        baches.draw(window);      
        jugador.draw(window);      
        
        window.draw(distanciaText);
        window.draw(bateriaText);

        window.display();
    }

    return 0;
}