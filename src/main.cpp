#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

#include "Motobici.hpp"
#include "Calle.hpp"
#include "Ciudad.hpp"
#include "Bache.hpp"
#include "Enemigos.hpp"


int main() {
    
    sf::RenderWindow window(sf::VideoMode({800, 400}), "La Batalla del Ciclista");
    window.setFramerateLimit(60);

    
    Motobici jugador;
    Calle calle;
    Ciudad ciudad;
    Bache baches;
    Enemigos enemigos;

    
    sf::Texture titleTexture;
    if (!titleTexture.loadFromFile("assets/image/title.png")) {
        return -1; 
    }
    sf::Sprite titleSprite(titleTexture);
    const sf::Vector2u titleSize = titleTexture.getSize();
    float scaleX = 800.f / static_cast<float>(titleSize.x);
    float scaleY = 400.f / static_cast<float>(titleSize.y);
    titleSprite.setScale({scaleX, scaleY});
    titleSprite.setPosition(sf::Vector2f((800.f - titleSprite.getGlobalBounds().size.x) / 2.f, (400.f - titleSprite.getGlobalBounds().size.y) / 2.f));

    
    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("assets/audio/melody.ogg")) {
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

    sf::Text startText(font);
    startText.setString("Press SPACE to start");
    startText.setCharacterSize(20);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(sf::Vector2f(
        (800.f - startText.getLocalBounds().size.x) / 2.f,
        titleSprite.getPosition().y + titleSprite.getGlobalBounds().size.y + 25.f
    ));

    
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
            window.draw(startText);
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
            enemigos.reset();
            metrosRecorridos = 0;
        }

        
        if (gameStarted && !gamePaused) {
            
            // Actualizar posiciones y físicas
            jugador.update(gameStarted, gamePaused);
            calle.update(gameStarted, gamePaused);
            ciudad.update(gameStarted, gamePaused);
            baches.update(gameStarted, gamePaused);
            enemigos.update(gameStarted, gamePaused, metrosRecorridos);

            
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

            // Detección de colisiones contra los enemigos
            for (const auto& enemy : enemigos.getEnemies()) {
                if (jugador.checkCollision(enemy)) {
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
        enemigos.draw(window);
        jugador.draw(window);      
        
        window.draw(distanciaText);
        window.draw(bateriaText);

        window.display();
    }

    return 0;
}