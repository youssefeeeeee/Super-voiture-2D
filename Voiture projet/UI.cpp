#include "UI.hpp"
#include "Game.hpp" // Pour GameState, Difficulty (si pas déjà dans UI.hpp via une autre inclusion)
#include <iostream>  // Pour std::cerr
#include <string>    // Pour std::to_string
#include <algorithm> // Pour std::min/max

UI::UI() {
    // Le constructeur peut rester vide si loadFont est appelé explicitement.
}

UI::~UI() {
    // Pas de ressources dynamiques à libérer ici si la police est gérée correctement.
}

bool UI::loadFont(const std::string& fontPath) {
    if (!uiFont.loadFromFile(fontPath)) {
        std::cerr << "ERREUR UI: Impossible de charger la police depuis " << fontPath << std::endl;
        return false;
    }
    return true;
}
bool UI::loadtxt(const std::string& path,sf::RenderTarget* t) {
	if (!txt.loadFromFile(path)) {
		std::cerr << "ERREUR UI: Impossible de charger la police depuis " << path << std::endl;
		return false;
	}
	sp.setTexture(txt);
    sp.setScale(
        static_cast<float>(t->getSize().x) / txt.getSize().x,
        static_cast<float>(t->getSize().y) / txt.getSize().y
    );


	return true;
}

void UI::setupText(sf::Text& text, const std::string& str, int charSize, sf::Color color, float posX, float posY, bool centralizeOrigin) {
    text.setFont(uiFont);
    text.setString(str);
    text.setCharacterSize(charSize);
    text.setFillColor(color);
    if (centralizeOrigin) {
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    }
    text.setPosition(posX, posY);
}

void UI::setupElements(float windowWidth, float windowHeight) {
    // Menu Principal
    setupText(titleText, "Super Voiture 2D", 70, sf::Color::Yellow, windowWidth / 2.f, windowHeight / 4.f, true);
    setupText(playButtonText, "Jouer", 50, sf::Color::White, windowWidth / 2.f, windowHeight / 2.f, true);
    setupText(settingsButtonText, "Parametres", 50, sf::Color::White, windowWidth / 2.f, windowHeight / 2.f + 80.f, true);
    setupText(quitButtonText, "Quitter", 50, sf::Color::White, windowWidth / 2.f, windowHeight / 2.f + 160.f, true);

    // Paramètres
   // setupText(settingsTitleText, "Parametres", 60, sf::Color::Cyan, windowWidth / 2.f, windowHeight / 5.f, true);

    float settingsYStart = windowHeight / 3.f + 50.f;
    float settingsXLabel = windowWidth / 2.f - 250.f;
    float settingsXValue = windowWidth / 2.f + 100.f;
    float settingsXIncDec = windowWidth / 2.f - 50.f;


    setupText(difficultyText, "Difficulte:", 40, sf::Color::White, settingsXLabel, settingsYStart);
    setupText(decreaseDifficultyButton, "<", 40, sf::Color::Green, settingsXValue - 60.f, settingsYStart);
    setupText(difficultyValueText, "Moyen", 40, sf::Color::Yellow, settingsXValue + 50.f, settingsYStart, true); // Centrer pour taille variable
    setupText(increaseDifficultyButton, ">", 40, sf::Color::Green, settingsXValue + 160.f, settingsYStart);

    settingsYStart += 80.f;
    setupText(volumeText, "Volume Musique:", 40, sf::Color::White, settingsXLabel, settingsYStart);
    setupText(decreaseVolumeButton, "-", 50, sf::Color::Green, settingsXValue - 60.f, settingsYStart - 5.f); // Ajustement pour taille de police
    setupText(volumeValueText, "50%", 40, sf::Color::Yellow, settingsXValue + 50.f, settingsYStart, true);
    setupText(increaseVolumeButton, "+", 40, sf::Color::Green, settingsXValue + 130.f, settingsYStart); // Ajustement pour taille de police

    settingsYStart += 80.f;
    setupText(highScoreLabelText, "High Score:", 40, sf::Color::White, settingsXLabel, settingsYStart);
    setupText(highScoreTextDisplay, "0", 40, sf::Color::Yellow, settingsXValue + 50.f, settingsYStart, true);


    setupText(backButtonText, "Retour", 50, sf::Color::Red, windowWidth / 2.f, windowHeight * 0.85f, true);

    // Confirmation Quitter
    setupText(quitConfirmText, "Voulez-vous vraiment quitter ?", 40, sf::Color::White, windowWidth / 2.f, windowHeight / 3.f, true);
    setupText(yesButtonText, "Oui", 50, sf::Color::Green, windowWidth / 2.f - 100.f, windowHeight / 2.f, true);
    setupText(noButtonText, "Non", 50, sf::Color::Red, windowWidth / 2.f + 100.f, windowHeight / 2.f, true);

    // Game Over
    setupText(gameOverScreenText, "GAME OVER", 80, sf::Color::Red, windowWidth / 2.f, windowHeight / 3.f, true);
    setupText(finalScoreText, "Votre Score: 0", 40, sf::Color::Yellow, windowWidth / 2.f, windowHeight / 2.f, true);
    setupText(highScoreGameOverDisplay, "High Score: 0", 30, sf::Color::Cyan, windowWidth / 2.f, windowHeight / 2.f + 60.f, true);
    setupText(returnToMenuText, "Appuyez sur Entree pour retourner au menu", 30, sf::Color::White, windowWidth / 2.f, windowHeight * 0.7f, true);
}


bool UI::isMouseOverText(const sf::Text& text, const sf::Vector2i& mousePosition) const {
    return text.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition));
}

void UI::updateButtonStates(const sf::Vector2i& mousePosition, GameState currentState) {
    // Réinitialiser les couleurs par défaut (ou styles)
    playButtonText.setFillColor(sf::Color::White);
    settingsButtonText.setFillColor(sf::Color::White);
    quitButtonText.setFillColor(sf::Color::White);
    decreaseDifficultyButton.setFillColor(sf::Color::Green);
    increaseDifficultyButton.setFillColor(sf::Color::Green);
    decreaseVolumeButton.setFillColor(sf::Color::Green);
    increaseVolumeButton.setFillColor(sf::Color::Green);
    backButtonText.setFillColor(sf::Color::Red);
    yesButtonText.setFillColor(sf::Color::Green);
    noButtonText.setFillColor(sf::Color::Red);

    // Appliquer les effets de survol
    switch (currentState) {
    case GameState::MainMenu:
        if (isMouseOverText(playButtonText, mousePosition)) playButtonText.setFillColor(sf::Color::Yellow);
        if (isMouseOverText(settingsButtonText, mousePosition)) settingsButtonText.setFillColor(sf::Color::Yellow);
        if (isMouseOverText(quitButtonText, mousePosition)) quitButtonText.setFillColor(sf::Color::Yellow);
        break;
    case GameState::Settings:
        if (isMouseOverText(decreaseDifficultyButton, mousePosition)) decreaseDifficultyButton.setFillColor(sf::Color::Cyan);
        if (isMouseOverText(increaseDifficultyButton, mousePosition)) increaseDifficultyButton.setFillColor(sf::Color::Cyan);
        if (isMouseOverText(decreaseVolumeButton, mousePosition)) decreaseVolumeButton.setFillColor(sf::Color::Cyan);
        if (isMouseOverText(increaseVolumeButton, mousePosition)) increaseVolumeButton.setFillColor(sf::Color::Cyan);
        if (isMouseOverText(backButtonText, mousePosition)) backButtonText.setFillColor(sf::Color::Yellow);
        break;
    case GameState::QuitConfirmation:
        if (isMouseOverText(yesButtonText, mousePosition)) yesButtonText.setFillColor(sf::Color::Yellow);
        if (isMouseOverText(noButtonText, mousePosition)) noButtonText.setFillColor(sf::Color::Yellow);
        break;
    default:
        break;
    }
}

void UI::update(const sf::Vector2i& mousePosition, GameState currentState) {
    updateButtonStates(mousePosition, currentState);
}

bool UI::processEvent(const sf::Event& event, sf::RenderWindow& window, GameState& currentState, Difficulty& difficulty, float& musicVolume, bool& requestWindowClose, int& currentScore, int& highScore) {
    bool actionTaken = false;

    if (event.type == sf::Event::Closed) {
        currentState = GameState::QuitConfirmation; // Aller à la confirmation
        actionTaken = true;
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            actionTaken = true; // Un clic est une action potentielle

            switch (currentState) {
            case GameState::MainMenu:
                if (isMouseOverText(playButtonText, mousePos)) currentState = GameState::Playing; // Game::resetGame sera appelé par Game
                else if (isMouseOverText(settingsButtonText, mousePos)) currentState = GameState::Settings;
                else if (isMouseOverText(quitButtonText, mousePos)) currentState = GameState::QuitConfirmation;
                else actionTaken = false;
                break;

            case GameState::Settings:
                if (isMouseOverText(backButtonText, mousePos)) currentState = GameState::MainMenu;
                else if (isMouseOverText(increaseDifficultyButton, mousePos)) {
                    if (difficulty == Difficulty::Facile) difficulty = Difficulty::Moyen;
                    else if (difficulty == Difficulty::Moyen) difficulty = Difficulty::Difficile;
                    updateDifficultyValueText(difficulty);
                }
                else if (isMouseOverText(decreaseDifficultyButton, mousePos)) {
                    if (difficulty == Difficulty::Difficile) difficulty = Difficulty::Moyen;
                    else if (difficulty == Difficulty::Moyen) difficulty = Difficulty::Facile;
                    updateDifficultyValueText(difficulty);
                }
                else if (isMouseOverText(increaseVolumeButton, mousePos)) {
                    musicVolume = std::min(100.f, musicVolume + 10.f);
                    updateVolumeValueText(musicVolume);
                }
                else if (isMouseOverText(decreaseVolumeButton, mousePos)) {
                    musicVolume = std::max(0.f, musicVolume - 10.f);
                    updateVolumeValueText(musicVolume);
                }
                else actionTaken = false;
                break;

            case GameState::QuitConfirmation:
                if (isMouseOverText(yesButtonText, mousePos)) requestWindowClose = true;
                else if (isMouseOverText(noButtonText, mousePos)) currentState = GameState::MainMenu; // Ou état précédent
                else actionTaken = false;
                break;

            case GameState::Playing: // Pas de clics UI pendant le jeu pour l'instant
            case GameState::GameOver:
                actionTaken = false;
                break;
            default:
                actionTaken = false;
                break;
            }
        }
    }

    if (currentState == GameState::GameOver && event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter) {
            currentState = GameState::MainMenu;
            actionTaken = true;
        }
    }
    return actionTaken;
}


void UI::updateDifficultyValueText(Difficulty difficulty) {
    switch (difficulty) {
    case Difficulty::Facile: difficultyValueText.setString("Facile"); break;
    case Difficulty::Moyen: difficultyValueText.setString("Moyen"); break;
    case Difficulty::Difficile: difficultyValueText.setString("Difficile"); break;
    }
    // Recentrer si la largeur change significativement
    sf::FloatRect bounds = difficultyValueText.getLocalBounds();
    difficultyValueText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
}

void UI::updateVolumeValueText(float volume) {
    volumeValueText.setString(std::to_string(static_cast<int>(volume)) + "%");
    sf::FloatRect bounds = volumeValueText.getLocalBounds();
    volumeValueText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
}

void UI::updateHighScoreDisplayTexts(int score) {
    std::string hsString = std::to_string(score);
    highScoreTextDisplay.setString(hsString);
    highScoreGameOverDisplay.setString("High Score: " + hsString);

    sf::FloatRect bounds1 = highScoreTextDisplay.getLocalBounds();
    highScoreTextDisplay.setOrigin(bounds1.left + bounds1.width / 2.0f, bounds1.top + bounds1.height / 2.0f);
    sf::FloatRect bounds2 = highScoreGameOverDisplay.getLocalBounds();
    highScoreGameOverDisplay.setOrigin(bounds2.left + bounds2.width / 2.0f, bounds2.top + bounds2.height / 2.0f);
}


void UI::render(sf::RenderTarget& target, GameState currentState, Difficulty currentDifficulty, float currentVolume, int currentHighScore, int gameTimeScore) {
    if (currentState != GameState::Playing) {
        target.draw(sp); // Dessine le fond uniquement si on n'est pas en train de jouer
    }
    // S'assurer que les textes dynamiques sont à jour avant le rendu
    updateDifficultyValueText(currentDifficulty); // Au cas où changé par autre chose que UI
    updateVolumeValueText(currentVolume);
    updateHighScoreDisplayTexts(currentHighScore);


    switch (currentState) {
    case GameState::MainMenu:
        target.draw(titleText);
        target.draw(playButtonText);
        target.draw(settingsButtonText);
        target.draw(quitButtonText);
        break;
    case GameState::Settings:
        target.draw(settingsTitleText);
        target.draw(difficultyText);
        target.draw(decreaseDifficultyButton);
        target.draw(difficultyValueText);
        target.draw(increaseDifficultyButton);
        target.draw(volumeText);
        target.draw(decreaseVolumeButton);
        target.draw(volumeValueText);
        target.draw(increaseVolumeButton);
        target.draw(highScoreLabelText);
        target.draw(highScoreTextDisplay);
        target.draw(backButtonText);
        break;
    case GameState::QuitConfirmation:
        target.draw(quitConfirmText);
        target.draw(yesButtonText);
        target.draw(noButtonText);
        break;
    case GameState::GameOver:
        // Le rendu du jeu (voiture, obstacles, fond) est fait par la classe Game avant d'appeler UI::render
        finalScoreText.setString("Votre Score: " + std::to_string(gameTimeScore));
        // Recentrer le texte du score final car sa longueur change
        setupText(finalScoreText, "Votre Score: " + std::to_string(gameTimeScore), 40, sf::Color::Yellow, target.getSize().x / 2.f, target.getSize().y / 2.f, true);

        target.draw(gameOverScreenText);
        target.draw(finalScoreText);
        target.draw(highScoreGameOverDisplay);
        target.draw(returnToMenuText);
        break;
    case GameState::Playing:
        // Pendant le jeu, on pourrait afficher le score actuel via l'UI
        sf::Text currentScoreDisplay;
        setupText(currentScoreDisplay, "Score: " + std::to_string(gameTimeScore), 24, sf::Color::White, 10, 10);
        target.draw(currentScoreDisplay);
        break;
    }
}
