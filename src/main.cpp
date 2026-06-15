#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <cstdlib>
#include <ctime>

#include "Motobici.hpp"
#include "Calle.hpp"
#include "Ciudad.hpp"
#include "Bache.hpp"
#include "Enemigos.hpp"
#include "PowerUps.hpp"
#include "Chancla.hpp"

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    sf::RenderWindow window(sf::VideoMode({800, 400}), "La Batalla del Ciclista");
    window.setFramerateLimit(60);

    Motobici jugador;
    Calle calle;
    Ciudad ciudad;
    Bache baches;
    Enemigos enemigos;
    PowerUps powerUps;
    Chancla chancla;

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

    sf::Text bossText(font);
    bossText.setCharacterSize(18);
    bossText.setFillColor(sf::Color(255, 80, 80));
    bossText.setPosition({30.f, 85.f});

    int metrosRecorridos = 0;
    int scoreTick = 0;
    bool gameStarted = false;
    bool gamePaused  = false;

    // Estado de pelea con jefe 1 (Abuelita a 370m)
    bool bossFight1Active  = false;
    bool bossFight1Done    = false;
    int  bossChanclasFired = 0;
    int  bossHitCount      = 0;
    int  bossShootTimer    = 0;

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
            powerUps.reset();
            chancla.esconder();
            metrosRecorridos  = 0;
            bossFight1Active  = false;
            bossFight1Done    = false;
            bossChanclasFired = 0;
            bossHitCount      = 0;
            bossShootTimer    = 0;
        }

        if (gameStarted && !gamePaused) {

            // Activar pelea con jefe 1 al llegar a 370m
            if (!bossFight1Done && !bossFight1Active && metrosRecorridos >= 370) {
                bossFight1Active  = true;
                bossChanclasFired = 0;
                bossHitCount      = 0;
                bossShootTimer    = 80;
                chancla.esconder();
                enemigos.activateBossFight();
            }

            if (bossFight1Active) {
                // ── PELEA CON JEFE 1: Abuelita ──
                // El scroll se congela; solo el jugador y la chancla se mueven
                jugador.update(true, false);
                chancla.update(true, false);

                // La abuelita dispara cada 90 frames (1.5 s)
                bossShootTimer--;
                if (bossShootTimer <= 0 && bossChanclasFired < 5 && !chancla.isActiva()) {
                    sf::Vector2f ap = enemigos.getAbuelitaPosition();
                    chancla.lanzar(ap.x, ap.y);
                    bossChanclasFired++;
                    bossShootTimer = 90;
                }

                // Detección de impacto: -5% por golpe; al 3er golpe -30% extra
                if (chancla.isActiva() && jugador.checkCollision(chancla.getSprite())) {
                    bossHitCount++;
                    jugador.applyBossDamage(5.0f);
                    if (bossHitCount == 3) jugador.applyBossDamage(30.0f);
                    chancla.esconder();
                }

                // Fin de la pelea: 5 chanclas lanzadas y la última ya salió
                if (bossChanclasFired >= 5 && !chancla.isActiva()) {
                    bossFight1Active = false;
                    bossFight1Done   = true;
                    enemigos.dismissAbuelita();
                }

                bossText.setString(
                    "JEFA: Abuelita  |  Golpes: " + std::to_string(bossHitCount) +
                    "/3  |  Salta para esquivar!"
                );
                bateriaText.setString("Bateria: " + std::to_string(static_cast<int>(jugador.getBateria())) + "%");
                bateriaText.setFillColor(jugador.getBateria() < 30.f ? sf::Color::Red : sf::Color::Green);
                if (jugador.getBateria() <= 0) gamePaused = true;

            } else {
                // ── JUEGO NORMAL ──
                jugador.update(gameStarted, gamePaused);
                float baseSpeed   = 1.0f + std::min(static_cast<float>(metrosRecorridos) / 900.f, 1.f) * 0.8f;
                float speedFactor = jugador.getSpeedFactor() * baseSpeed;
                calle.update(gameStarted, gamePaused, speedFactor);
                ciudad.update(gameStarted, gamePaused, speedFactor);
                baches.update(gameStarted, gamePaused, speedFactor);
                auto respawnedPositions = baches.consumeRespawnPositions();
                for (float obstacleX : respawnedPositions) {
                    powerUps.notifyObstacleAppeared(obstacleX);
                }
                enemigos.update(gameStarted, gamePaused, metrosRecorridos, speedFactor);
                sf::Vector2f shootPos;
                if (enemigos.pollShootEvent(shootPos))
                    chancla.lanzar(shootPos.x, shootPos.y);
                powerUps.update(gameStarted, gamePaused, speedFactor);
                chancla.update(gameStarted, gamePaused);

                distanciaText.setString("Distancia: " + std::to_string(metrosRecorridos) + "m");
                bateriaText.setString("Bateria: " + std::to_string(static_cast<int>(jugador.getBateria())) + "%");
                bateriaText.setFillColor(jugador.getBateria() < 30.f ? sf::Color::Red : sf::Color::Green);

                {
                    int kmh = 20 + static_cast<int>(metrosRecorridos * 0.12f);
                    if (kmh > 120) kmh = 120;
                    float ratio = std::min(static_cast<float>(metrosRecorridos) / 900.f, 1.f);
                    speedBarFill.setSize({ratio * 160.f, 18.f});
                    if (jugador.getSpeedFactor() > 1.0f) {
                        speedText.setString("TURBO! " + std::to_string(kmh) + " km/h");
                        speedText.setFillColor(sf::Color(255, 200, 0));
                        speedBarFill.setFillColor(sf::Color(255, 200, 0, 220));
                    } else {
                        speedText.setString(std::to_string(kmh) + " km/h");
                        speedText.setFillColor(sf::Color::White);
                        speedBarFill.setFillColor(sf::Color(0, 200, 255, 200));
                    }
                }

                static int collisionCooldown = 0;
                bool collided = false;
                for (const auto& bache : baches.getObstacles()) {
                    if (jugador.checkCollision(bache)) collided = true;
                }
                for (const auto& enemy : enemigos.getEnemies()) {
                    if (jugador.checkCollision(enemy)) collided = true;
                }
                if (chancla.isActiva() && jugador.checkCollision(chancla.getSprite())) {
                    collided = true;
                    chancla.esconder();
                }

                PowerUpType collectedType;
                if (powerUps.collect(jugador.getGlobalBounds(), collectedType)) {
                    if (collectedType == PowerUpType::Battery)
                        jugador.applyBatteryPickup(20.0f);
                    else
                        jugador.applySpeedPickup(1.5f, 360);
                }

                if (collided && collisionCooldown == 0) {
                    jugador.applyCollisionPenalty();
                    collisionCooldown = 30;
                }
                if (collisionCooldown > 0) collisionCooldown--;

                if (jugador.getBateria() <= 0) gamePaused = true;

                scoreTick++;
                if (scoreTick >= 6) {
                    metrosRecorridos++;
                    scoreTick = 0;
                }
            }
        }
    
        window.clear(sf::Color(135, 206, 235));
        
        ciudad.draw(window);       
        calle.draw(window);        
        baches.draw(window);      
        enemigos.draw(window);
        powerUps.draw(window);
        chancla.draw(window);
        
        // ¡AQUÍ ESTÁ LA MAGIA! Por fin le decimos que dibuje tu motobici
        jugador.draw(window); 

        window.draw(distanciaText);
        window.draw(bateriaText);
        if (bossFight1Active) window.draw(bossText);
        window.draw(speedText);
        window.draw(speedBarBg);
        window.draw(speedBarFill);

        window.display();
    }

    return 0;
}