#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>
#include <stdexcept>

enum class PowerUpType { Battery, Speed };

struct PowerUp {
    PowerUpType type;
    sf::CircleShape shape;
    std::vector<sf::RectangleShape> icons;
};

class PowerUps {
private:
    std::vector<PowerUp> items;
    float spawnY = 0.f;
    int nextBatteryDistance = 500;
    int nextSpeedDistance = 900;
    bool batterySpawned = false;
    bool speedSpawned = false;

public:
    PowerUps() {

        sf::Texture groundTexture;
        if (!groundTexture.loadFromFile("assets/image/ground.png")) {
            throw std::runtime_error("Error al cargar textura del piso para calcular posición de powerups");
        }

        const int groundRectHeight = 100;
        const sf::Vector2u groundSize = groundTexture.getSize();
        float roadScale = 800.f / static_cast<float>(groundSize.x);
        float groundY = 400.f - static_cast<float>(groundRectHeight) * roadScale;

        // Colocar los powerups justo arriba de la calle
        spawnY = groundY - 70.f;
    }

    void update(bool gameStarted, bool gamePaused, int distancia, float speedFactor) {
        if (!gameStarted || gamePaused) return;

        if (!batterySpawned && distancia >= nextBatteryDistance) {
            spawnPowerUp(PowerUpType::Battery);
            batterySpawned = true;
        }

        if (!speedSpawned && distancia >= nextSpeedDistance) {
            spawnPowerUp(PowerUpType::Speed);
            speedSpawned = true;
        }

        for (auto& item : items) {
            const sf::Vector2f movement = {-6.0f * speedFactor, 0.f};
            item.shape.move(movement);
            for (auto& icon : item.icons) {
                icon.move(movement);
            }
        }

        items.erase(std::remove_if(items.begin(), items.end(), [](const PowerUp& item) {
            return item.shape.getPosition().x < -50.f;
        }), items.end());
    }

    void draw(sf::RenderWindow& window) {
        for (auto& item : items) {
            window.draw(item.shape);
            for (auto& icon : item.icons) {
                window.draw(icon);
            }
        }
    }

    bool collect(const sf::FloatRect& bounds, PowerUpType& outType) {
        for (std::size_t i = 0; i < items.size(); ++i) {
            if (items[i].shape.getGlobalBounds().findIntersection(bounds).has_value()) {
                outType = items[i].type;
                items.erase(items.begin() + i);
                return true;
            }
        }
        return false;
    }

    void reset() {
        items.clear();
        batterySpawned = false;
        speedSpawned = false;
        nextBatteryDistance = 500;
        nextSpeedDistance = 900;
    }

private:
    void spawnPowerUp(PowerUpType type) {
        PowerUp powerUp;
        powerUp.type = type;
        powerUp.shape = sf::CircleShape(18.f);
        powerUp.shape.setOrigin({18.f, 18.f});
        powerUp.shape.setPosition({800.f, spawnY});
        if (type == PowerUpType::Battery) {
            powerUp.shape.setFillColor(sf::Color(0, 200, 0, 200));
        } else {
            powerUp.shape.setFillColor(sf::Color(0, 150, 255, 200));
        }
        powerUp.shape.setOutlineThickness(2.f);
        powerUp.shape.setOutlineColor(sf::Color::White);

        const sf::Vector2f center = powerUp.shape.getPosition();
        if (type == PowerUpType::Battery) {
            sf::RectangleShape vertical({4.f, 14.f});
            vertical.setFillColor(sf::Color::White);
            vertical.setOrigin({2.f, 7.f});
            vertical.setPosition(center);
            sf::RectangleShape horizontal({14.f, 4.f});
            horizontal.setFillColor(sf::Color::White);
            horizontal.setOrigin({7.f, 2.f});
            horizontal.setPosition(center);
            powerUp.icons.push_back(vertical);
            powerUp.icons.push_back(horizontal);
        } else {
            sf::RectangleShape bolt1({16.f, 4.f});
            bolt1.setFillColor(sf::Color::White);
            bolt1.setOrigin({0.f, 2.f});
            bolt1.setPosition(sf::Vector2f{center.x - 6.f, center.y});
            bolt1.setRotation(sf::degrees(-25.f));
            sf::RectangleShape bolt2({16.f, 4.f});
            bolt2.setFillColor(sf::Color::White);
            bolt2.setOrigin({0.f, 2.f});
            bolt2.setPosition(sf::Vector2f{center.x + 2.f, center.y + 8.f});
            bolt2.setRotation(sf::degrees(-25.f));
            powerUp.icons.push_back(bolt1);
            powerUp.icons.push_back(bolt2);
        }

        items.push_back(powerUp);
    }
};
