#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

#include "Motobici.hpp"
#include "Calle.hpp"
#include "Ciudad.hpp"
#include "Bache.hpp"
#include "Enemigos.hpp"
#include "PowerUps.hpp"


int main() {
    
    sf::RenderWindow window(sf::VideoMode({800, 400}), "La Batalla del Ciclista");
    window.setFramerateLimit(60);

    
    Motobici jugador;
    Calle calle;
    Ciudad ciudad;
    Bache baches;
    Enemigos enemigos;
    PowerUps powerUps;

    
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

    sf::Text speedText(font);
    speedText.setCharacterSize(20);
    speedText.setFillColor(sf::Color::White);
    speedText.setPosition({550.f, 38.f});

    sf::RectangleShape speedBarBg({160.f, 18.f});
    speedBarBg.setFillColor(sf::Color(50, 50, 50, 200));
    speedBarBg.setPosition({550.f, 65.f});

    sf::RectangleShape speedBarFill({0.f, 18.f});
    speedBarFill.setFillColor(sf::Color(0, 200, 255, 200));
    speedBarFill.setPosition(speedBarBg.getPosition());

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
            float speedFactor = jugador.getSpeedFactor();
            calle.update(gameStarted, gamePaused, speedFactor);
            ciudad.update(gameStarted, gamePaused, speedFactor);
            baches.update(gameStarted, gamePaused, speedFactor);
            auto respawnedPositions = baches.consumeRespawnPositions();
            for (float obstacleX : respawnedPositions) {
                powerUps.notifyObstacleAppeared(obstacleX);
            }
            enemigos.update(gameStarted, gamePaused, metrosRecorridos, speedFactor);
            powerUps.update(gameStarted, gamePaused, metrosRecorridos, speedFactor);

            
            distanciaText.setString("Distancia: " + std::to_string(metrosRecorridos) + "m");
            bateriaText.setString("Bateria: " + std::to_string(static_cast<int>(jugador.getBateria())) + "%");

            if (jugador.getBateria() < 30.0f) {
                bateriaText.setFillColor(sf::Color::Red);
            } else {
                bateriaText.setFillColor(sf::Color::Green);
            }

            // Detección de colisiones contra los baches
            static int collisionCooldown = 0;
            bool collided = false;
            for (const auto& bache : baches.getObstacles()) {
                if (jugador.checkCollision(bache)) {
                    collided = true;
                }
            }

            // Detección de colisiones contra los enemigos
            for (const auto& enemy : enemigos.getEnemies()) {
                if (jugador.checkCollision(enemy)) {
                    collided = true;
                }
            }

            PowerUpType collectedType;
            if (powerUps.collect(jugador.getGlobalBounds(), collectedType)) {
                if (collectedType == PowerUpType::Battery) {
                    jugador.applyBatteryPickup(15.0f);
                } else {
                    jugador.applySpeedPickup(8.0f, 360);
                }
            }

            if (collided && collisionCooldown == 0) {
                jugador.applyCollisionPenalty();
                collisionCooldown = 30; // impedir múltiples penalizaciones instantáneas
            }
            if (collisionCooldown > 0) {
                collisionCooldown--;
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
        powerUps.draw(window);
        jugador.draw(window);      
        
        speedText.setString("Speed: " + std::to_string(static_cast<int>(jugador.getSpeedKmh())) + " km/h");
        float speedRatio = jugador.getSpeedKmh() / 35.f;
        speedBarFill.setSize({std::max(0.f, std::min(160.f, 160.f * speedRatio)), 18.f});

        window.draw(distanciaText);
        window.draw(bateriaText);
        window.draw(speedText);
        window.draw(speedBarBg);
        window.draw(speedBarFill);

        window.display();
    }

    return 0;
}