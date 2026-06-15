#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>
#include <stdexcept>

enum class PowerUpType { Battery, Speed };

struct PowerUp {
    PowerUpType type;
    sf::Sprite sprite;

    PowerUp(PowerUpType t, const sf::Texture& texture)
        : type(t), sprite(texture) {}
};

class PowerUps {
private:
    std::vector<PowerUp> items;
    sf::Texture batteryTexture;
    sf::Texture speedTexture;
    float groundY = 0.f;
    int obstacleCounter = 0;
    bool nextIsBattery = true;
    const int batteryThreshold = 2;
    const int speedThreshold = 2;
    static constexpr float pixelsPerMeter = 36.f;
    static constexpr float powerUpOffsetMeters = 10.f;

public:
    PowerUps() {
        if (!batteryTexture.loadFromFile("assets/image/bateria_up.png")) {
            throw std::runtime_error("Error al cargar textura de power-up de bateria");
        }
        if (!speedTexture.loadFromFile("assets/image/velocidad_up.png")) {
            throw std::runtime_error("Error al cargar textura de power-up de velocidad");
        }

        sf::Texture groundTexture;
        if (!groundTexture.loadFromFile("assets/image/ground.png")) {
            throw std::runtime_error("Error al cargar textura del piso para calcular posición de powerups");
        }

        const int groundRectHeight = 100;
        const sf::Vector2u groundSize = groundTexture.getSize();
        float roadScale = 800.f / static_cast<float>(groundSize.x);
        groundY = 400.f - static_cast<float>(groundRectHeight) * roadScale;
    }

    void update(bool gameStarted, bool gamePaused, float speedFactor) {
        if (!gameStarted || gamePaused) return;

        for (auto& item : items) {
            item.sprite.move({-6.0f * speedFactor, 0.f});
        }

        items.erase(std::remove_if(items.begin(), items.end(), [](const PowerUp& item) {
            return item.sprite.getPosition().x < -50.f;
        }), items.end());
    }

    void notifyObstacleAppeared(float obstacleX) {
        obstacleCounter += 1;
        if (nextIsBattery && obstacleCounter >= batteryThreshold) {
            spawnPowerUp(PowerUpType::Battery, obstacleX + powerUpOffsetMeters * pixelsPerMeter);
            obstacleCounter = 0;
            nextIsBattery = false;
        } else if (!nextIsBattery && obstacleCounter >= speedThreshold) {
            spawnPowerUp(PowerUpType::Speed, obstacleX + powerUpOffsetMeters * pixelsPerMeter);
            obstacleCounter = 0;
            nextIsBattery = true;
        }
    }

    void draw(sf::RenderWindow& window) {
        for (auto& item : items) {
            window.draw(item.sprite);
        }
    }

    bool collect(const sf::FloatRect& bounds, PowerUpType& outType) {
        for (std::size_t i = 0; i < items.size(); ++i) {
            if (items[i].sprite.getGlobalBounds().findIntersection(bounds).has_value()) {
                outType = items[i].type;
                items.erase(items.begin() + i);
                return true;
            }
        }
        return false;
    }

    void reset() {
        items.clear();
        obstacleCounter = 0;
        nextIsBattery = true;
    }

private:
    void spawnPowerUp(PowerUpType type, float spawnX) {
        const sf::Texture& texture = (type == PowerUpType::Battery ? batteryTexture : speedTexture);
        PowerUp powerUp(type, texture);

        const sf::FloatRect textureBounds = powerUp.sprite.getGlobalBounds();
        float desiredSize = 60.f; // power-ups más visibles
        float scale = desiredSize / std::max(textureBounds.size.x, textureBounds.size.y);
        powerUp.sprite.setScale({scale, scale});
        powerUp.sprite.setOrigin({powerUp.sprite.getGlobalBounds().size.x / 2.f, powerUp.sprite.getGlobalBounds().size.y / 2.f});
        float spawnY = groundY - powerUp.sprite.getGlobalBounds().size.y / 2.f - 8.f;
        powerUp.sprite.setPosition({spawnX, spawnY});

        items.push_back(std::move(powerUp));
    }
};
