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

    // Gestion des �v�nements et des clics
    // Retourne vrai si une action a �t� trait�e qui pourrait changer l'�tat (ex: clic sur bouton)
    bool processEvent(const sf::Event& event, sf::RenderWindow& window, GameState& currentState, Difficulty& difficulty, float& musicVolume, bool& requestWindowClose, int& currentScore, int& highScore);

    // Mise � jour (pour les effets de survol, etc.)
    void update(const sf::Vector2i& mousePosition, GameState currentState);

    // Rendu
    void render(sf::RenderTarget& target, GameState currentState, Difficulty currentDifficulty, float currentVolume, int currentHighScore, int gameTimeScore);
    void updateHighScoreDisplayTexts(int score);

private:
    sf::Font uiFont;
    sf::Texture txt;
    sf::Sprite sp;
    // �l�ments du Menu Principal
    sf::Text titleText;
    sf::Text playButtonText;
    sf::Text settingsButtonText;
    sf::Text quitButtonText;

    // �l�ments de l'�cran des Param�tres
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
    sf::Text highScoreTextDisplay; // Pour afficher le high score dans les param�tres

    // �l�ments de l'�cran de Confirmation pour Quitter
    sf::Text quitConfirmText;
    sf::Text yesButtonText;
    sf::Text noButtonText;

    // �l�ments de l'�cran Game Over
    sf::Text gameOverScreenText;
    sf::Text finalScoreText;
    sf::Text returnToMenuText;
    sf::Text highScoreGameOverDisplay; // Pour afficher le high score sur l'�cran Game Over

    // M�thodes utilitaires priv�es
    bool isMouseOverText(const sf::Text& text, const sf::Vector2i& mousePosition) const;
    void updateButtonStates(const sf::Vector2i& mousePosition, GameState currentState); // G�re le survol

    // Fonctions pour mettre � jour l'affichage des valeurs dynamiques
    void updateDifficultyValueText(Difficulty difficulty);
    void updateVolumeValueText(float volume);

};

#endif // UI_HPP