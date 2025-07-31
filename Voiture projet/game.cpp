#include "Game.hpp"
#include "UI.hpp" 
#include <cstdlib>
#include <algorithm>
#include <string>
#include <vector> 


// Constructeur
Game::Game() {
    initVariables(); // Initialise les variables AVANT la fenêtre et l'UI
    initWindow();

    gameUI = std::make_unique<UI>(); // Créer l'objet UI

    loadAssets();    // Charger polices pour UI, textures, musique

    if (gameUI) { // Vérifier si gameUI est bien initialisé
        gameUI->setupElements(static_cast<float>(window->getSize().x), static_cast<float>(window->getSize().y));
    }

    initGameObjects(); // Initialise la voiture, le fond (après textures)

    applyCoreDifficultySettings(); // Appliquer la difficulté par défaut à la logique du jeu

    if (backgroundMusic.openFromFile("assets/audios/pixel-song-27-72476.ogg")) {
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(musicVolume);
        backgroundMusic.play(); // Décommentez pour jouer au démarrage
    }
    else {
        std::cerr << "ERREUR: Impossible de charger music.ogg" << std::endl;
    }
}

Game::~Game() {
    saveHighScoreToFile();
    delete this->window; // gameUI (unique_ptr) sera détruit automatiquement
}
void Game::update() {
    // Poll events and update game state
    pollEventsAndUpdate();

    // Update logic based on the current game state
    if (currentState == GameState::Playing) {
        float deltaTime = gameClock.restart().asSeconds();
        updatePlayingLogic(deltaTime);
    }
    else if (currentState == GameState::GameOver) {
        // Handle any specific logic for the GameOver state if needed
    }
    else if (currentState == GameState::MainMenu || currentState == GameState::Settings) {
        // UI handles updates for these states
        if (gameUI) {
            gameUI->update(sf::Mouse::getPosition(*window), currentState);
        }
    }
}
void Game::render() {
    // Clear the window with a default background color
    window->clear(sf::Color(20, 20, 50));

    // Render game content (background, player, obstacles) if applicable
    renderGameContent();

    // Render the UI on top of the game content
    renderCurrentUI();

    // Display the rendered frame
    window->display();
}

void Game::initVariables() {
    this->window = nullptr; // Sera initialisé dans initWindow()
    videoMode = sf::VideoMode(1200, 800);
    currentState = GameState::MainMenu;
    currentDifficulty = Difficulty::Moyen;
    musicVolume = 50.f;
    highScore = 0;

    obstacleSpawnTimer = 0.f;
    obstacleSpawnTimerMax_base = 2.0f; // Sera ajusté par applyCoreDifficultySettings
    maxObstacles_base = 4;          // Sera ajusté
    baseObstacleSpeed_base = 280.f; // Sera ajusté
    // minSpawnDistance_base est maintenant géré dans applyCoreDifficultySettings

    currentSpeedMultiplier = 1.0f; // Pour l'augmentation en jeu
    gameTime = 0.f;
    difficultyTimer_inGame = 0.f;
    difficultyInterval_inGame = 12.f; // Augmenter la difficulté en jeu toutes les 12 secondes
}

void Game::initWindow() {
    window = new sf::RenderWindow(videoMode, "Voiture 2D - Evite les Obstacles", sf::Style::Titlebar | sf::Style::Close);
    window->setFramerateLimit(60);
    window->setVerticalSyncEnabled(true);
}

void Game::loadAssets() {
    // Charger la police via l'UI
    if (gameUI && !gameUI->loadFont("./assets/fonts/ARIAL.TTF")) { // REMPLACEZ par votre chemin de police
        std::cerr << "ERREUR GAME: La police n'a pas pu etre chargee par l'UI." << std::endl;
        // Gérer l'erreur, peut-être quitter si la police est essentielle
        exit(EXIT_FAILURE);
    }
	if (gameUI && !gameUI->loadtxt("assets/image/UIbgr.png",window)) { // REMPLACEZ par votre chemin de police
		std::cerr << "ERREUR GAME: La police n'a pas pu etre chargee par l'UI." << std::endl;
		// Gérer l'erreur, peut-être quitter si la police est essentielle
		exit(EXIT_FAILURE);
	}

    joueur.initTexture("assets/image/voiture.png");

    std::vector<std::string> obstacleTextureFiles = {
        "assets/image/enemycar1.png", "assets/image/enemycar2.png",
        "assets/image/enemycar3.png", "assets/image/enemycar4.png"
    };
    obstacleTextures.clear(); // S'assurer qu'il est vide avant de charger
    for (const auto& file : obstacleTextureFiles) {
        sf::Texture texture;
        if (texture.loadFromFile(file)) {
            obstacleTextures.push_back(texture);
        }
        else {
            std::cerr << "ERREUR: Impossible de charger la texture d'obstacle: " << file << std::endl;
        }
    }
    if (obstacleTextures.empty()) {
        std::cerr << "ERREUR CRITIQUE: Aucune texture d'obstacle chargee." << std::endl;
        exit(EXIT_FAILURE);
    }

    background.initTextures("assets/image/road.png", "assets/image/farcity.png");
    loadHighScoreFromFile();
}

void Game::initGameObjects() {
    background.initSprites(*window);
    // La position du joueur est réinitialisée dans resetGameForPlay()
}

void Game::resetGameForPlay() {
    gameTime = 0.f;
    difficultyTimer_inGame = 0.f;
    currentSpeedMultiplier = 1.0f;
    obstacles.clear();

    float roadTopY = background.getRoadTopY(*window);
    float roadVisualH = background.getRoadVisualHeight();
    joueur.initSprite(*window, roadTopY, roadVisualH);

    applyCoreDifficultySettings(); // Appliquer les paramètres de difficulté actuels
    gameClock.restart();
    currentState = GameState::Playing; // Assurer que l'état est bien Playing
}


bool Game::isRunning() const {
    return window->isOpen();
}

void Game::run() {
    while (isRunning()) {
        float deltaTime = gameClock.restart().asSeconds(); // deltaTime est toujours utile

        pollEventsAndUpdate(); // Gère événements et logique de mise à jour principale

        // Rendu
        window->clear(sf::Color(20, 20, 50)); // Fond par défaut

        renderGameContent(); // Rend les éléments du jeu si nécessaire (ex: fond pour GameOver)
        renderCurrentUI();   // L'UI dessine par-dessus ou gère son propre fond

        window->display();
    }
}

void Game::pollEventsAndUpdate() {
    bool closeRequestedByUI = false;
    GameState previousState = currentState;


    while (window->pollEvent(event)) {
        if (gameUI) {
            // Passer currentScore (gameTime) et highScore à processEvent
            gameUI->processEvent(event, *window, currentState, currentDifficulty, musicVolume, closeRequestedByUI, reinterpret_cast<int&>(gameTime), highScore);
        }
        if (event.type == sf::Event::Closed && currentState != GameState::QuitConfirmation) { // Si la croix est cliquée et qu'on n'est pas déjà en confirmation
            currentState = GameState::QuitConfirmation;
        }
    }

    if (closeRequestedByUI) {
        window->close();
        return; // Sortir tôt si la fermeture est demandée
    }

    // Si l'état a changé à cause de l'UI (ex: clic sur "Jouer")
    if (previousState != currentState) {
        if (currentState == GameState::Playing && previousState != GameState::Playing) { // On entre en jeu
            resetGameForPlay();
        }
        applyCoreDifficultySettings(); // Réappliquer si la difficulté a changé dans les settings
        if (backgroundMusic.getStatus() == sf::Music::Playing) { // Mettre à jour le volume si la musique joue
            backgroundMusic.setVolume(musicVolume);
        }
    }


    // Mises à jour spécifiques à l'état
    if (gameUI) {
        gameUI->update(sf::Mouse::getPosition(*window), currentState);
    }

    if (currentState == GameState::Playing) {
        float dt = gameClock.getElapsedTime().asSeconds(); // Utiliser le temps écoulé depuis le dernier redémarrage pour Playing
        // Ou passer le deltaTime calculé au début de run()
        if (dt == 0.f && previousState != GameState::Playing) { // Si on vient d'entrer en jeu, dt peut être 0 la première frame
            // Ne rien faire ou utiliser un petit dt fixe pour la première frame de logique de jeu
        }
      
    }
    if (currentState == GameState::GameOver && previousState == GameState::Playing) { // Transition de Playing à GameOver
        checkAndSetHighScore();
    }

}

void Game::updatePlayingLogic(float deltaTime) { // deltaTime est maintenant le temps écoulé pour la frame de jeu
    if (deltaTime <= 0) deltaTime = 1.f / 60.f; // Protection contre deltaTime nul ou négatif

    gameTime += deltaTime;
    difficultyTimer_inGame += deltaTime;

    if (difficultyTimer_inGame >= difficultyInterval_inGame) {
        difficultyTimer_inGame = 0.f;
        increaseInGameDifficulty();
    }

    background.update(deltaTime, *window);
    joueur.update(*window, background.getRoadTopY(*window), background.getRoadVisualHeight());
    updateObstacles(deltaTime);

    obstacleSpawnTimer += deltaTime;
    float actualSpawnTimerMax = obstacleSpawnTimerMax_base; // Modulé par applyCoreDifficultySettings
    int actualMaxObstacles = maxObstacles_base;          // Modulé par applyCoreDifficultySettings

    if (obstacleSpawnTimer >= actualSpawnTimerMax && obstacles.size() < static_cast<size_t>(actualMaxObstacles)) {
        trySpawnObstacle();
    }
    checkCollisions(); // Si collision -> currentState = GameState::GameOver
}


void Game::renderGameContent() {
    // Rendre le fond et les éléments du jeu si l'état est Playing ou GameOver (pour voir le jeu figé)
    if (currentState == GameState::Playing || currentState == GameState::GameOver) {
        background.render(*window);
        joueur.render(*window);
        for (const auto& obstacle : obstacles) {
            obstacle.render(*window);
        }
    }
}

void Game::renderCurrentUI() {
    if (gameUI) {
        gameUI->render(*window, currentState, currentDifficulty, musicVolume, highScore, static_cast<int>(gameTime));
    }
}


void Game::trySpawnObstacle() {
    int actualMaxObstacles = maxObstacles_base; // Déjà ajusté par applyCoreDifficultySettings
    float actualMinSpawnDistance; // Sera défini par applyCoreDifficultySettings

    switch (currentDifficulty) {
    case Difficulty::Facile: actualMinSpawnDistance = 1.25f * 350.f; break;
    case Difficulty::Moyen:  actualMinSpawnDistance = 1.0f * 350.f; break;
    case Difficulty::Difficile: actualMinSpawnDistance = 0.75f * 350.f; break;
    default: actualMinSpawnDistance = 350.f;
    }
    // Utiliser les valeurs directement mises à jour par applyCoreDifficultySettings au lieu de recalculer ici.


    if (obstacles.size() >= static_cast<size_t>(actualMaxObstacles)) return;
    if (obstacleTextures.empty()) return;

    bool canSpawn = true;
    if (!obstacles.empty()) {
        const Obstacle& lastObstacle = obstacles.back();
        if (static_cast<float>(window->getSize().x) - lastObstacle.getSprite().getPosition().x < actualMinSpawnDistance && lastObstacle.getSprite().getPosition().x > 0) {
            canSpawn = false;
        }
    }

    if (canSpawn) {
        float roadTopY = background.getRoadTopY(*window);
        float roadVisualH = background.getRoadVisualHeight();
        obstacles.emplace_back(obstacleTextures, *window, roadTopY, roadVisualH);
        obstacleSpawnTimer = 0.f;
    }
}

void Game::updateObstacles(float deltaTime) {
    float actualSpeed = baseObstacleSpeed_base; // Déjà ajusté par applyCoreDifficultySettings

    for (size_t i = 0; i < obstacles.size(); ++i) {
        obstacles[i].update(deltaTime, actualSpeed, currentSpeedMultiplier);
        if (obstacles[i].isOffScreenLeft()) {
            obstacles.erase(obstacles.begin() + i);
            --i;
        }
    }
}

void Game::checkCollisions() {
    for (auto& obstacle : obstacles) {
        if (pixelPerfectCollision(joueur.getSprite(), obstacle.getSprite())) {
            currentState = GameState::GameOver;
            // checkAndSetHighScore(); // Déplacé à la transition d'état
            return;
        }
    }
}

void Game::increaseInGameDifficulty() {
    currentSpeedMultiplier = std::min(currentSpeedMultiplier + 0.05f, 2.5f);
    
}

void Game::applyCoreDifficultySettings() {
    // Cette fonction ajuste les variables de base du jeu en fonction de currentDifficulty
    // Elle est appelée quand la difficulté change dans les settings ou au début d'une partie.
    switch (currentDifficulty) {
    case Difficulty::Facile:
        baseObstacleSpeed_base = 220.f;
        obstacleSpawnTimerMax_base = 2.5f;
        maxObstacles_base = 3;
        
        break;
    case Difficulty::Moyen:
        baseObstacleSpeed_base = 280.f;
        obstacleSpawnTimerMax_base = 2.0f;
        maxObstacles_base = 4;
        
        break;
    case Difficulty::Difficile:
        baseObstacleSpeed_base = 350.f;
        obstacleSpawnTimerMax_base = 1.5f;
        maxObstacles_base = 5;
        
        break;
    }
}


void Game::loadHighScoreFromFile() {
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
    else {
        highScore = 0;
    }
    if (gameUI) gameUI->updateHighScoreDisplayTexts(highScore); // Mettre à jour l'UI
}

void Game::saveHighScoreToFile() {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
    else {
        std::cerr << "ERREUR: Impossible d'ouvrir highscore.txt pour sauvegarder." << std::endl;
    }
}

void Game::checkAndSetHighScore() {
    if (static_cast<int>(gameTime) > highScore) {
        highScore = static_cast<int>(gameTime);
        saveHighScoreToFile(); // Sauvegarder immédiatement
        if (gameUI) gameUI->updateHighScoreDisplayTexts(highScore); // Mettre à jour l'UI
    }
}

// La fonction pixelPerfectCollision reste la même
bool Game::pixelPerfectCollision(const sf::Sprite& sprite1, const sf::Sprite& sprite2) {
    sf::FloatRect bounds1 = sprite1.getGlobalBounds();
    sf::FloatRect bounds2 = sprite2.getGlobalBounds();
    sf::FloatRect intersection;
    if (!bounds1.intersects(bounds2, intersection)) return false;
    if (!sprite1.getTexture() || !sprite2.getTexture()) return true;
    const sf::Image& img1 = sprite1.getTexture()->copyToImage();
    const sf::Image& img2 = sprite2.getTexture()->copyToImage();
    sf::IntRect texRect1 = sprite1.getTextureRect();
    sf::IntRect texRect2 = sprite2.getTextureRect();
    for (float i = intersection.left; i < intersection.left + intersection.width; ++i) {
        for (float j = intersection.top; j < intersection.top + intersection.height; ++j) {
            sf::Vector2f localPos1 = sprite1.getInverseTransform().transformPoint(i, j);
            sf::Vector2f localPos2 = sprite2.getInverseTransform().transformPoint(i, j);
            if (localPos1.x >= 0 && localPos1.x < texRect1.width &&
                localPos1.y >= 0 && localPos1.y < texRect1.height &&
                localPos2.x >= 0 && localPos2.x < texRect2.width &&
                localPos2.y >= 0 && localPos2.y < texRect2.height) {
                sf::Color pixel1 = img1.getPixel(static_cast<unsigned int>(texRect1.left + localPos1.x), static_cast<unsigned int>(texRect1.top + localPos1.y));
                sf::Color pixel2 = img2.getPixel(static_cast<unsigned int>(texRect2.left + localPos2.x), static_cast<unsigned int>(texRect2.top + localPos2.y));
                if (pixel1.a > 128 && pixel2.a > 128) return true;
            }
        }
    }
    return false;
}
