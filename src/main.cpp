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

    sf::Texture metaTexture1, metaTexture2;
    if (!metaTexture1.loadFromFile("assets/image/meta_casa.png"))   return -1;
    if (!metaTexture2.loadFromFile("assets/image/meta_parque.png")) return -1;
    sf::Sprite metaSprite1(metaTexture1), metaSprite2(metaTexture2);
    {
        const sf::Vector2u ms1 = metaTexture1.getSize();
        metaSprite1.setScale({800.f / static_cast<float>(ms1.x), 400.f / static_cast<float>(ms1.y)});
        const sf::Vector2u ms2 = metaTexture2.getSize();
        metaSprite2.setScale({800.f / static_cast<float>(ms2.x), 400.f / static_cast<float>(ms2.y)});
    }
    int   metaIndex   = 0;     // 0 = casa, 1 = parque, alterna en cada reinicio
    float metaScrollX = 800.f; // posición X de la imagen de meta (empieza fuera a la derecha)

    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("assets/audio/melody.ogg")) {
        return -1;
    }
    backgroundMusic.setLooping(true);
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

    sf::RectangleShape congratsBg({700.f, 90.f});
    congratsBg.setFillColor(sf::Color(0, 0, 0, 190));
    congratsBg.setPosition({50.f, 145.f});

    sf::Text winTitleText(font);
    winTitleText.setString("En hora buena, has llegado a tu destino!");
    winTitleText.setCharacterSize(24);
    winTitleText.setFillColor(sf::Color::Yellow);
    winTitleText.setPosition({(800.f - winTitleText.getLocalBounds().size.x) / 2.f, 158.f});

    sf::Text winSubText(font);
    winSubText.setString("Presiona SPACE para jugar de nuevo");
    winSubText.setCharacterSize(18);
    winSubText.setFillColor(sf::Color::White);
    winSubText.setPosition({(800.f - winSubText.getLocalBounds().size.x) / 2.f, 198.f});

    sf::Text bossText(font);
    bossText.setCharacterSize(18);
    bossText.setFillColor(sf::Color(255, 80, 80));
    bossText.setPosition({30.f, 85.f});

    int metrosRecorridos = 0;
    int scoreTick = 0;
    bool gameStarted = false;
    bool gamePaused  = false;
    bool gameWon     = false;
    float pedalFactor = 0.f; // 0 = detenido, 1 = velocidad completa

    // Estado de pelea con jefe 1 (Abuelita a 370m)
    bool bossFight1Active  = false;
    bool bossFight1Done    = false;
    int  bossChanclasFired  = 0;
    int  bossChanclasDodged = 0;
    int  bossHitCount       = 0;
    int  bossShootTimer     = 0;

    // Estado de pelea con jefe 2 (Ciclista a 710m)
    bool bossFight2Active  = false;
    bool bossFight2Done    = false;
    int  boss2PassCount    = 0;
    int  boss2DodgedCount  = 0;
    int  boss2HitCount     = 0;
    int  boss2WaitTimer    = 0;
    bool boss2PassActive   = false;
    bool boss2HitThisPass  = false;

    // Estado de pelea con jefe 3 (Taxista a 1100m)
    bool bossFight3Active  = false;
    bool bossFight3Done    = false;
    int  boss3PassCount    = 0;
    int  boss3DodgedCount  = 0;
    int  boss3HitCount     = 0;
    int  boss3WaitTimer    = 0;
    bool boss3PassActive   = false;
    bool boss3HitThisPass  = false;
    int  boss3SubPhase     = 0;
    int  boss3SubTimer     = 0;
    bool boss3ChanclaFired = false;

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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && gameWon) {
            metaIndex   = 1 - metaIndex;
            metaScrollX = 800.f;
            gameWon = false; gameStarted = false; gamePaused = false;
            metrosRecorridos = 0; scoreTick = 0;
            jugador.reset(); jugador.setXPosition(20.f);
            baches.reset(); enemigos.reset(); powerUps.reset(); chancla.esconder();
            bossFight1Active = false; bossFight1Done = false;
            bossChanclasFired = 0; bossChanclasDodged = 0; bossHitCount = 0; bossShootTimer = 0;
            bossFight2Active = false; bossFight2Done = false;
            boss2PassCount = 0; boss2DodgedCount = 0; boss2HitCount = 0; boss2WaitTimer = 0;
            boss2PassActive = false; boss2HitThisPass = false;
            bossFight3Active = false; bossFight3Done = false;
            boss3PassCount = 0; boss3DodgedCount = 0; boss3HitCount = 0; boss3WaitTimer = 0;
            boss3PassActive = false; boss3HitThisPass = false;
            boss3SubPhase = 0; boss3SubTimer = 0; boss3ChanclaFired = false;
            pedalFactor = 0.f;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && gamePaused) {
            gamePaused = false;
            jugador.reset();
            jugador.setXPosition(20.f);
            baches.reset();
            enemigos.reset();
            powerUps.reset();
            chancla.esconder();
            metrosRecorridos  = 0;
            metaScrollX       = 800.f;
            bossFight1Active  = false;
            bossFight1Done    = false;
            bossChanclasFired  = 0;
            bossChanclasDodged = 0;
            bossHitCount       = 0;
            bossShootTimer     = 0;
            bossFight2Active  = false;
            bossFight2Done    = false;
            boss2PassCount    = 0;
            boss2DodgedCount  = 0;
            boss2HitCount     = 0;
            boss2WaitTimer    = 0;
            boss2PassActive   = false;
            boss2HitThisPass  = false;
            bossFight3Active  = false;
            bossFight3Done    = false;
            boss3PassCount    = 0;
            boss3DodgedCount  = 0;
            boss3HitCount     = 0;
            boss3WaitTimer    = 0;
            boss3PassActive   = false;
            boss3HitThisPass  = false;
            boss3SubPhase     = 0;
            boss3SubTimer     = 0;
            boss3ChanclaFired = false;
            pedalFactor       = 0.f;
        }

        if (gameStarted && !gamePaused && !gameWon) {

            // Activar pelea con jefe 1 al llegar a 370m
            if (!bossFight1Done && !bossFight1Active && metrosRecorridos >= 370) {
                bossFight1Active  = true;
                bossChanclasFired = 0;
                bossHitCount      = 0;
                bossShootTimer    = 80;
                chancla.esconder();
                enemigos.activateBossFight();
                jugador.setBossFightMode(true);
                jugador.setJumpCost(1.0f);
            }

            // Activar pelea con jefe 2 al llegar a 770m
            // Activar pelea con jefe 3 al llegar a 1100m
            if (bossFight2Done && !bossFight3Done && !bossFight3Active && metrosRecorridos >= 1100) {
                bossFight3Active = true;
                boss3PassCount   = 0;
                boss3DodgedCount = 0;
                boss3HitCount    = 0;
                boss3WaitTimer   = 90;
                boss3PassActive  = false;
                boss3HitThisPass = false;
                enemigos.activateTaxistaBoss();
                jugador.setBossFightMode(true);
                jugador.setBossSlowDrain(true);
                jugador.setJumpCost(1.0f);
                jugador.setXPosition(250.f);
            }

            if (bossFight1Done && !bossFight2Done && !bossFight2Active && metrosRecorridos >= 710) {
                bossFight2Active = true;
                boss2PassCount   = 0;
                boss2DodgedCount = 0;
                boss2HitCount    = 0;
                boss2WaitTimer   = 90;
                boss2PassActive  = false;
                enemigos.activateCiclistaBoss();
                jugador.setBossFightMode(true);
                jugador.setJumpCost(1.0f);
                jugador.setXPosition(250.f);
            }

            if (bossFight1Active) {
                // ── PELEA CON JEFE 1: Abuelita ──
                bool chanclaWasActiva = chancla.isActiva();

                jugador.update(true, false);
                chancla.update(true, false);

                // Capturar si la chancla salió de pantalla ANTES de que el timer lance una nueva
                bool chanclaDesaparecioEstFrame = chanclaWasActiva && !chancla.isActiva();

                // La abuelita dispara cada 90 frames (1.5 s)
                bossShootTimer--;
                if (bossShootTimer <= 0 && bossChanclasFired < 4 && !chancla.isActiva()) {
                    sf::Vector2f ap = enemigos.getAbuelitaPosition();
                    chancla.lanzar(ap.x, ap.y);
                    bossChanclasFired++;
                    bossShootTimer = 90;
                }

                // Detección de impacto: -5% por golpe; al 3er golpe -15% extra
                bool hitThisFrame = false;
                if (chancla.isActiva() && jugador.checkCollisionRect(chancla.getHitbox())) {
                    bossHitCount++;
                    hitThisFrame = true;
                    jugador.applyBossDamage(5.0f);
                    if (bossHitCount == 3) jugador.applyBossDamage(15.0f);
                    chancla.esconder();
                    chanclaDesaparecioEstFrame = false; // fue un golpe, no una esquiva
                }

                // Esquiva: la chancla salió de pantalla sin haberle pegado al jugador
                if (chanclaDesaparecioEstFrame) {
                    bossChanclasDodged++;
                }

                // Fin de la pelea: 4 chanclas lanzadas y la última ya pasó o pegó
                if (bossChanclasFired >= 4 && !chancla.isActiva()) {
                    bossFight1Active = false;
                    bossFight1Done   = true;
                    enemigos.dismissAbuelita();
                    jugador.setBossFightMode(false);
                    jugador.setJumpCost(1.0f);
                    powerUps.spawnVictoryBattery();
                    distanciaText.setString("Distancia: " + std::to_string(metrosRecorridos) + "m");
                }

                bossText.setString(
                    "JEFA: Abuelita  |  Chanclas: " + std::to_string(bossChanclasFired) +
                    "/4  |  Salta para esquivar!"
                );
                bateriaText.setString("Bateria: " + std::to_string(static_cast<int>(jugador.getBateria())) + "%");
                bateriaText.setFillColor(jugador.getBateria() < 30.f ? sf::Color::Red : sf::Color::Green);
                if (jugador.getBateria() <= 0) gamePaused = true;

            } else if (bossFight2Active) {
                // ── PELEA CON JEFE 2: El Ciclista Kamikaze ──
                jugador.update(true, false);

                if (boss2PassActive) {
                    // 3 pasadas: 12.0, 12.8, 13.8 px/frame
                    const float chargeSpeeds[3] = {12.0f, 12.8f, 13.8f};
                    float chargeSpeed = chargeSpeeds[boss2PassCount < 3 ? boss2PassCount : 2];
                    enemigos.moveCiclista(chargeSpeed);

                    // Golpe: solo se aplica una vez por pasada; el ciclista sigue de largo
                    if (!boss2HitThisPass && jugador.checkCollision(enemigos.getCiclistaSprite())) {
                        boss2HitCount++;
                        boss2HitThisPass = true;
                        jugador.applyBossDamage(10.0f);
                    }

                    // Fin de pasada: ciclista salió completamente por la izquierda
                    if (enemigos.isCiclistaOffScreen()) {
                        if (!boss2HitThisPass) boss2DodgedCount++;
                        boss2PassActive  = false;
                        boss2HitThisPass = false;
                        boss2PassCount++;
                        boss2WaitTimer = 60;
                    }
                } else {
                    if (boss2WaitTimer > 0) {
                        boss2WaitTimer--;
                    } else if (boss2PassCount < 3) {
                        enemigos.resetCiclistaForPass();
                        boss2PassActive  = true;
                        boss2HitThisPass = false;
                    } else {
                        // Las 5 pasadas completadas — fin de la pelea
                        bossFight2Active = false;
                        bossFight2Done   = true;
                        enemigos.dismissCiclista();
                        jugador.setBossFightMode(false);
                        jugador.setJumpCost(1.0f);
                        jugador.setXPosition(20.f);
                        powerUps.spawnVictoryBattery();
                    }
                }

                bossText.setString(
                    "JEFE: Ciclista  |  Ataques: " + std::to_string(boss2PassCount) +
                    "/3  |  Salta cuando se lance!"
                );
                bateriaText.setString("Bateria: " + std::to_string(static_cast<int>(jugador.getBateria())) + "%");
                bateriaText.setFillColor(jugador.getBateria() < 30.f ? sf::Color::Red : sf::Color::Green);
                if (jugador.getBateria() <= 0) gamePaused = true;

            } else if (bossFight3Active) {
                // ── PELEA CON JEFE FINAL: El Taxista ──
                jugador.update(true, false);

                const char* boss3Msg = "Salta cuando se lance!";

                if (boss3PassActive) {
                    switch (boss3PassCount) {

                    case 0: { // Embestida simple
                        enemigos.moveTaxista(14.0f);
                        if (!boss3HitThisPass && jugador.checkCollision(enemigos.getTaxistaSprite())) {
                            boss3HitCount++; boss3HitThisPass = true;
                            jugador.applyBossDamage(12.0f);
                        }
                        if (enemigos.isTaxistaOffScreen()) {
                            boss3PassActive = false; boss3HitThisPass = false;
                            boss3PassCount++; boss3WaitTimer = 90;
                        }
                        break;
                    }

                    case 1: { // Entra lento → para → arranca rápido
                        boss3Msg = "Cuidado! Va a acelerar!";
                        if (boss3SubPhase == 0) {
                            enemigos.moveTaxista(2.5f);
                            if (enemigos.getTaxistaPos().x <= 580.f) {
                                boss3SubPhase = 1;
                                boss3SubTimer = 180; // 3 segundos parado
                            }
                        } else if (boss3SubPhase == 1) {
                            boss3Msg = "Acelerando... SALTA!";
                            if (--boss3SubTimer <= 0) boss3SubPhase = 2;
                        } else {
                            enemigos.moveTaxista(19.0f);
                            if (!boss3HitThisPass && jugador.checkCollision(enemigos.getTaxistaSprite())) {
                                boss3HitCount++; boss3HitThisPass = true;
                                jugador.applyBossDamage(12.0f);
                            }
                            if (enemigos.isTaxistaOffScreen()) {
                                boss3PassActive = false; boss3HitThisPass = false;
                                boss3SubPhase = 0; boss3PassCount++; boss3WaitTimer = 90;
                            }
                        }
                        break;
                    }

                    case 2: { // Lanza 3 chanclas seguidas
                        boss3Msg = "Lanzando objetos! Salta!";
                        const int TOTAL_CHANCLAS_TAXI = 3;
                        if (boss3SubTimer > 0) {
                            // Espera entre chanclas
                            boss3SubTimer--;
                            if (boss3SubTimer == 0) boss3ChanclaFired = false;
                        } else if (!boss3ChanclaFired) {
                            chancla.lanzar(enemigos.getTaxistaPos().x, enemigos.getTaxistaPos().y);
                            boss3ChanclaFired = true;
                        } else {
                            // Chancla en vuelo
                            bool cw3 = chancla.isActiva();
                            chancla.update(true, false);
                            if (chancla.isActiva() && jugador.checkCollisionRect(chancla.getHitbox())) {
                                jugador.applyBossDamage(12.0f);
                                chancla.esconder();
                            }
                            if (cw3 && !chancla.isActiva()) {
                                boss3SubPhase++;
                                if (boss3SubPhase < TOTAL_CHANCLAS_TAXI) {
                                    boss3SubTimer = 55; // ~1s entre chanclas
                                } else {
                                    // Terminaron las 3 chanclas
                                    boss3PassActive   = false;
                                    boss3ChanclaFired = false;
                                    boss3SubPhase     = 0;
                                    boss3PassCount++;
                                    boss3WaitTimer    = 90;
                                }
                            }
                        }
                        break;
                    }

                    case 3: { // Embestida ultra-rápida
                        boss3Msg = "VELOCIDAD MAXIMA! SALTA!";
                        enemigos.moveTaxista(23.0f);
                        if (!boss3HitThisPass && jugador.checkCollision(enemigos.getTaxistaSprite())) {
                            boss3HitCount++; boss3HitThisPass = true;
                            jugador.applyBossDamage(12.0f);
                        }
                        if (enemigos.isTaxistaOffScreen()) {
                            boss3PassActive = false; boss3HitThisPass = false;
                            boss3PassCount++; boss3WaitTimer = 0;
                        }
                        break;
                    }

                    } // end switch
                } else {
                    if (boss3WaitTimer > 0) {
                        boss3WaitTimer--;
                    } else if (boss3PassCount < 4) {
                        if (boss3PassCount == 2)
                            enemigos.positionTaxistaForChancla();
                        else
                            enemigos.resetTaxistaForPass();
                        boss3PassActive  = true;
                        boss3HitThisPass = false;
                        boss3SubPhase    = 0;
                        boss3SubTimer    = 0;
                    } else {
                        // Fin de la pelea
                        bossFight3Active = false;
                        bossFight3Done   = true;
                        enemigos.dismissTaxista();
                        jugador.setBossFightMode(false);
                        jugador.setBossSlowDrain(false);
                        jugador.setJumpCost(1.0f);
                        jugador.setXPosition(20.f);
                        powerUps.spawnVictoryBattery();
                    }
                }

                bossText.setString(
                    "JEFE FINAL: Taxista [" + std::to_string(boss3PassCount + 1) + "/4]  |  " + boss3Msg
                );
                bateriaText.setString("Bateria: " + std::to_string(static_cast<int>(jugador.getBateria())) + "%");
                bateriaText.setFillColor(jugador.getBateria() < 30.f ? sf::Color::Red : sf::Color::Green);
                if (jugador.getBateria() <= 0) gamePaused = true;

            } else {
                // ── JUEGO NORMAL ──
                jugador.update(gameStarted, gamePaused);

                // Pedaleo: D o → mantiene el avance, soltar frena gradualmente
                bool pedaleando = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
                               || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
                if (pedaleando)
                    pedalFactor = std::min(pedalFactor + 0.04f, 1.0f);
                else
                    pedalFactor = std::max(pedalFactor - 0.025f, 0.0f);

                float baseSpeed   = 1.0f + std::min(static_cast<float>(metrosRecorridos) / 900.f, 1.f) * 0.8f;
                float speedFactor = jugador.getSpeedFactor() * baseSpeed * pedalFactor;

                // Deslizar imagen de meta desde la derecha a partir de 1200m
                if (bossFight3Done && metrosRecorridos >= 1200 && metaScrollX > 0.f) {
                    metaScrollX -= 3.0f;
                    if (metaScrollX < 0.f) metaScrollX = 0.f;
                }

                calle.update(gameStarted, gamePaused, speedFactor);
                ciudad.update(gameStarted, gamePaused, speedFactor);
                bool spawnBaches = !(metrosRecorridos >= 330 && !bossFight1Done)
                                && !(metrosRecorridos >= 670 && !bossFight2Done)
                                && !(metrosRecorridos >= 1060 && !bossFight3Done)
                                && !bossFight3Done;
                baches.update(gameStarted, gamePaused, speedFactor, spawnBaches);
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
                if (chancla.isActiva() && jugador.checkCollisionRect(chancla.getHitbox())) {
                    collided = true;
                    chancla.esconder();
                }

                PowerUpType collectedType;
                if (powerUps.collect(jugador.getGlobalBounds(), collectedType)) {
                    if (collectedType == PowerUpType::Battery)
                        jugador.applyBatteryPickup(23.0f);
                    else
                        jugador.applySpeedPickup(1.35f, 280);
                }

                if (collided && collisionCooldown == 0) {
                    jugador.applyCollisionPenalty();
                    collisionCooldown = 30;
                }
                if (collisionCooldown > 0) collisionCooldown--;

                if (jugador.getBateria() <= 0) gamePaused = true;

                // Llegar a 1360m con el jefe final vencido = victoria
                if (bossFight3Done && metrosRecorridos >= 1250 && !gameWon)
                    gameWon = true;

                if (pedalFactor > 0.f) {
                    scoreTick++;
                    if (scoreTick >= 6) {
                        metrosRecorridos++;
                        scoreTick = 0;
                    }
                }
            }
        }
    
        window.clear(sf::Color(135, 206, 235));

        // Ciudad siempre visible; desde 1200m la imagen de meta entra deslizándose desde la derecha
        ciudad.draw(window);
        if (bossFight3Done && metrosRecorridos >= 1200) {
            sf::Sprite& currentMeta = (metaIndex == 0 ? metaSprite1 : metaSprite2);
            currentMeta.setPosition({metaScrollX, 0.f});
            window.draw(currentMeta);
        }

        calle.draw(window);
        baches.draw(window);
        enemigos.draw(window);
        powerUps.draw(window);
        chancla.draw(window);
        jugador.draw(window);

        if (!gameWon) {
            window.draw(distanciaText);
            window.draw(bateriaText);
            if (bossFight1Active || bossFight2Active || bossFight3Active) window.draw(bossText);
            window.draw(speedText);
            window.draw(speedBarBg);
            window.draw(speedBarFill);
        } else {
            window.draw(congratsBg);
            window.draw(winTitleText);
            window.draw(winSubText);
        }

        window.display();
    }

    return 0;
}