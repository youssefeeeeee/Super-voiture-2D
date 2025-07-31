#ifndef UI_HPP
#define UI_HPP

#include <SFML/Graphics.hpp>
#include <string>

#include "Game.hpp"
class UI {
public:
    UI();
    ~UI();

    // Initialisation
    bool loadFont(const std::string& fontPath);
    bool loadtxt(const std::string& path,sf::RenderTarget* t);
    void setupElements(float windowWidth, float windowHeight); // Positionne tous les textes
    void setupText(sf::Text& text, const std::string& str, int charSize, sf::Color color, float posX, float posY, bool centralizeOrigin = false);

    // Gestion des événements et des clics
    // Retourne vrai si une action a été traitée qui pourrait changer l'état (ex: clic sur bouton)
    bool processEvent(const sf::Event& event, sf::RenderWindow& window, GameState& currentState, Difficulty& difficulty, float& musicVolume, bool& requestWindowClose, int& currentScore, int& highScore);

    // Mise à jour (pour les effets de survol, etc.)
    void update(const sf::Vector2i& mousePosition, GameState currentState);

    // Rendu
    void render(sf::RenderTarget& target, GameState currentState, Difficulty currentDifficulty, float currentVolume, int currentHighScore, int gameTimeScore);
    void updateHighScoreDisplayTexts(int score);

private:
    sf::Font uiFont;
    sf::Texture txt;
    sf::Sprite sp;
    // Éléments du Menu Principal
    sf::Text titleText;
    sf::Text playButtonText;
    sf::Text settingsButtonText;
    sf::Text quitButtonText;

    // Éléments de l'Écran des Paramètres
    sf::Text settingsTitleText;
    sf::Text difficultyText;
    sf::Text difficultyValueText;
    sf::Text volumeText;
    sf::Text volumeValueText;
    sf::Text backButtonText;
    sf::Text increaseDifficultyButton;
    sf::Text decreaseDifficultyButton;
    sf::Text increaseVolumeButton;
    sf::Text decreaseVolumeButton;
    sf::Text highScoreLabelText;
    sf::Text highScoreTextDisplay; // Pour afficher le high score dans les paramètres

    // Éléments de l'Écran de Confirmation pour Quitter
    sf::Text quitConfirmText;
    sf::Text yesButtonText;
    sf::Text noButtonText;

    // Éléments de l'Écran Game Over
    sf::Text gameOverScreenText;
    sf::Text finalScoreText;
    sf::Text returnToMenuText;
    sf::Text highScoreGameOverDisplay; // Pour afficher le high score sur l'écran Game Over

    // Méthodes utilitaires privées
    bool isMouseOverText(const sf::Text& text, const sf::Vector2i& mousePosition) const;
    void updateButtonStates(const sf::Vector2i& mousePosition, GameState currentState); // Gère le survol

    // Fonctions pour mettre à jour l'affichage des valeurs dynamiques
    void updateDifficultyValueText(Difficulty difficulty);
    void updateVolumeValueText(float volume);

};

#endif // UI_HPP