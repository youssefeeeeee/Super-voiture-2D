#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include <SFML/Graphics.hpp>
#include <vector> // Pour passer les textures

class Obstacle {
public:
    // Le constructeur prend une r�f�rence au vecteur de textures pour �viter de les recharger
    Obstacle(const std::vector<sf::Texture>& availableTextures,
        const sf::RenderWindow& window,
        float roadTopY,
        float roadVisualHeight);
    ~Obstacle();

    void update(float deltaTime, float currentBaseSpeed, float speedMultiplier);
    void render(sf::RenderTarget& target) const;
    sf::Sprite& getSprite(); // Pour la d�tection de collision
    const sf::Sprite& getSprite() const;
    bool isOffScreenLeft() const;

private:
    sf::Sprite sprite;
    // La texture elle-m�me est poss�d�e par la classe Game et r�f�renc�e ici
};

#endif // OBSTACLE_HPP