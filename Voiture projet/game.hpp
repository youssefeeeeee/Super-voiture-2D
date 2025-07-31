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
// États possibles du jeu
enum class GameState {
    MainMenu,
    Settings,
    Playing,
    GameOver,
    QuitConfirmation
};

// Niveaux de difficulté
enum class Difficulty {
    Facile,
    Moyen,
    Difficile
};

// Déclaration anticipée de la classe UI
class UI;

class Game {
private:
    // Fenêtre
    sf::RenderWindow* window;
    sf::VideoMode videoMode;
    sf::Event event;

    // État actuel du jeu
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
    float gameTime;             // Utilisé comme score pendant la partie
    float difficultyTimer_inGame; // Timer pour l'augmentation de difficulté en jeu
    float difficultyInterval_inGame;
    bool pixelPerfectCollision(const sf::Sprite& sprite1, const sf::Sprite& sprite2);
    // High Score
    int highScore;

    // Musique
    sf::Music backgroundMusic;
    float musicVolume;

    // Interface Utilisateur (objet UI)
    std::unique_ptr<UI> gameUI; // Utilisation de unique_ptr pour gérer la ressource

    // Fonctions privées d'initialisation
    void initWindow();
    void initVariables();
    void loadAssets();
    void initGameObjects();

    // Fonctions de gestion des états et logique principale
    void pollEventsAndUpdate(); // Combine la gestion des événements et les mises à jour basées sur l'état

    void updatePlayingLogic(float deltaTime); // Logique spécifique à l'état Playing
    // Les autres updates (MainMenu, Settings, etc.) sont principalement gérées par l'UI pour les effets de survol,
   

    void renderGameContent(); // Rend les éléments du jeu (fond, joueur, obstacles)
    void renderCurrentUI();   // Appelle la méthode de rendu appropriée de l'UI

    // Fonctions de logique du jeu (modifiées ou nouvelles)
    void resetGameForPlay(); // Réinitialise l'état pour une nouvelle partie
    void trySpawnObstacle();
    void updateObstacles(float deltaTime);
    void checkCollisions();
    void increaseInGameDifficulty(); // Augmentation progressive pendant le jeu
    void applyCoreDifficultySettings(); // Applique les paramètres de difficulté à la logique du jeu

    // High Score
    void loadHighScoreFromFile();
    void saveHighScoreToFile();
    void checkAndSetHighScore(); // Vérifie si gameTime est un nouveau high score

public:
    Game();
    ~Game(); // s'assurera que gameUI est détruit correctement
    void update();
	void render();
    bool isRunning() const;
    void run();
};

#endif // GAME_HPP
