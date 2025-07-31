#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>

#include "Voiture.hpp"
#include "Background.hpp"
#include "Obstacle.hpp"
// �tats possibles du jeu
enum class GameState {
    MainMenu,
    Settings,
    Playing,
    GameOver,
    QuitConfirmation
};

// Niveaux de difficult�
enum class Difficulty {
    Facile,
    Moyen,
    Difficile
};

// D�claration anticip�e de la classe UI
class UI;

class Game {
private:
    // Fen�tre
    sf::RenderWindow* window;
    sf::VideoMode videoMode;
    sf::Event event;

    // �tat actuel du jeu
    GameState currentState;
    Difficulty currentDifficulty;

    // Objets du jeu
    Voiture joueur;
    Background background;
    std::vector<Obstacle> obstacles;
    std::vector<sf::Texture> obstacleTextures; // Stocke toutes les textures d'obstacles possibles

    // Logique du jeu
    float obstacleSpawnTimer;
    float obstacleSpawnTimerMax_base;
    int maxObstacles_base;
    sf::Clock gameClock;
    float baseObstacleSpeed_base;
    float currentSpeedMultiplier; // Pour l'augmentation progressive en jeu
    float gameTime;             // Utilis� comme score pendant la partie
    float difficultyTimer_inGame; // Timer pour l'augmentation de difficult� en jeu
    float difficultyInterval_inGame;
    bool pixelPerfectCollision(const sf::Sprite& sprite1, const sf::Sprite& sprite2);
    // High Score
    int highScore;

    // Musique
    sf::Music backgroundMusic;
    float musicVolume;

    // Interface Utilisateur (objet UI)
    std::unique_ptr<UI> gameUI; // Utilisation de unique_ptr pour g�rer la ressource

    // Fonctions priv�es d'initialisation
    void initWindow();
    void initVariables();
    void loadAssets();
    void initGameObjects();

    // Fonctions de gestion des �tats et logique principale
    void pollEventsAndUpdate(); // Combine la gestion des �v�nements et les mises � jour bas�es sur l'�tat

    void updatePlayingLogic(float deltaTime); // Logique sp�cifique � l'�tat Playing
    // Les autres updates (MainMenu, Settings, etc.) sont principalement g�r�es par l'UI pour les effets de survol,
   

    void renderGameContent(); // Rend les �l�ments du jeu (fond, joueur, obstacles)
    void renderCurrentUI();   // Appelle la m�thode de rendu appropri�e de l'UI

    // Fonctions de logique du jeu (modifi�es ou nouvelles)
    void resetGameForPlay(); // R�initialise l'�tat pour une nouvelle partie
    void trySpawnObstacle();
    void updateObstacles(float deltaTime);
    void checkCollisions();
    void increaseInGameDifficulty(); // Augmentation progressive pendant le jeu
    void applyCoreDifficultySettings(); // Applique les param�tres de difficult� � la logique du jeu

    // High Score
    void loadHighScoreFromFile();
    void saveHighScoreToFile();
    void checkAndSetHighScore(); // V�rifie si gameTime est un nouveau high score

public:
    Game();
    ~Game(); // s'assurera que gameUI est d�truit correctement
    void update();
	void render();
    bool isRunning() const;
    void run();
};

#endif // GAME_HPP
