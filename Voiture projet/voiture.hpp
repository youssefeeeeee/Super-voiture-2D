#ifndef VOITURE_HPP
#define VOITURE_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Voiture {
public:
    Voiture();

    void initTexture(const std::string& texturePath);
    void initSprite(const sf::RenderWindow& window, float roadTop, float roadHeight);
    void update(const sf::RenderWindow& window, float roadTop, float roadHeight);
    void render(sf::RenderTarget& target) const;
    sf::Sprite& getSprite(); // Nécessaire pour la détection de collision

private:
    sf::Texture texture;
    sf::Sprite sprite;
};

#endif // VOITURE_HPP