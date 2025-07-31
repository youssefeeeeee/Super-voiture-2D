#include "Game.hpp"
#include "UI.hpp" 
#include <cstdlib>
#include <algorithm>
#include <string>
#include <vector> 


// Constructeur
Game::Game() {
    initVariables(); // Initialise les variables AVANT la fen�tre et l'UI
    initWindow();

    gameUI = std::make_unique<UI>(); // Cr�er l'objet UI

    loadAssets();    // Charger polices pour UI, textures, musique

    if (gameUI) { // V�rifier si gameUI est bien initialis�
        gameUI->setupElements(static_cast<float>(window->getSize().x), static_cast<float>(window->getSize().y));
    }

    initGameObjects(); // Initialise la voiture, le fond (apr�s textures)

    applyCoreDifficultySettings(); // Appliquer la difficult� par d�faut � la logique du jeu

    if (backgroundMusic.openFromFile("assets/audios/pixel-song-27-72476.ogg")) {
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(musicVolume);
        backgroundMusic.play(); // D�commentez pour jouer au d�marrage
    }
    else {
        std::cerr << "ERREUR: Impossible de charger music.ogg" << std::endl;
    }
}

Game::~Game() {
    saveHighScoreToFile();
    delete this->window; // gameUI (unique_ptr) sera d�truit automatiquement
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
    this->window = nullptr; // Sera initialis� dans initWindow()
    videoMode = sf::VideoMode(1200, 800);
    currentState = GameState::MainMenu;
    currentDifficulty = Difficulty::Moyen;
    musicVolume = 50.f;
    highScore = 0;

    obstacleSpawnTimer = 0.f;
    obstacleSpawnTimerMax_base = 2.0f; // Sera ajust� par applyCoreDifficultySettings
    maxObstacles_base = 4;          // Sera ajust�
    baseObstacleSpeed_base = 280.f; // Sera ajust�
    // minSpawnDistance_base est maintenant g�r� dans applyCoreDifficultySettings

    currentSpeedMultiplier = 1.0f; // Pour l'augmentation en jeu
    gameTime = 0.f;
    difficultyTimer_inGame = 0.f;
    difficultyInterval_inGame = 12.f; // Augmenter la difficult� en jeu toutes les 12 secondes
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
        // G�rer l'erreur, peut-�tre quitter si la police est essentielle
        exit(EXIT_FAILURE);
    }
	if (gameUI && !gameUI->loadtxt("assets/image/UIbgr.png",window)) { // REMPLACEZ par votre chemin de police
		std::cerr << "ERREUR GAME: La police n'a pas pu etre chargee par l'UI." << std::endl;
		// G�rer l'erreur, peut-�tre quitter si la police est essentielle
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
    // La position du joueur est r�initialis�e dans resetGameForPlay()
}

void Game::resetGameForPlay() {
    gameTime = 0.f;
    difficultyTimer_inGame = 0.f;
    currentSpeedMultiplier = 1.0f;
    obstacles.clear();

    float roadTopY = background.getRoadTopY(*window);
    float roadVisualH = background.getRoadVisualHeight();
    joueur.initSprite(*window, roadTopY, roadVisualH);

    applyCoreDifficultySettings(); // Appliquer les param�tres de difficult� actuels
    gameClock.restart();
    currentState = GameState::Playing; // Assurer que l'�tat est bien Playing
}


bool Game::isRunning() const {
    return window->isOpen();
}

void Game::run() {
    while (isRunning()) {
        float deltaTime = gameClock.restart().asSeconds(); // deltaTime est toujours utile

        pollEventsAndUpdate(); // G�re �v�nements et logique de mise � jour principale

        // Rendu
        window->clear(sf::Color(20, 20, 50)); // Fond par d�faut

        renderGameContent(); // Rend les �l�ments du jeu si n�cessaire (ex: fond pour GameOver)
        renderCurrentUI();   // L'UI dessine par-dessus ou g�re son propre fond

        window->display();
    }
}

void Game::pollEventsAndUpdate() {
    bool closeRequestedByUI = false;
    GameState previousState = currentState;


    while (window->pollEvent(event)) {
        if (gameUI) {
            // Passer currentScore (gameTime) et highScore � processEvent
            gameUI->processEvent(event, *window, currentState, currentDifficulty, musicVolume, closeRequestedByUI, reinterpret_cast<int&>(gameTime), highScore);
        }
        if (event.type == sf::Event::Closed && currentState != GameState::QuitConfirmation) { // Si la croix est cliqu�e et qu'on n'est pas d�j� en confirmation
            currentState = GameState::QuitConfirmation;
        }
    }

    if (closeRequestedByUI) {
        window->close();
        return; // Sortir t�t si la fermeture est demand�e
    }

    // Si l'�tat a chang� � cause de l'UI (ex: clic sur "Jouer")
    if (previousState != currentState) {
        if (currentState == GameState::Playing && previousState != GameState::Playing) { // On entre en jeu
            resetGameForPlay();
        }
        applyCoreDifficultySettings(); // R�appliquer si la difficult� a chang� dans les settings
        if (backgroundMusic.getStatus() == sf::Music::Playing) { // Mettre � jour le volume si la musique joue
            backgroundMusic.setVolume(musicVolume);
        }
    }


    // Mises � jour sp�cifiques � l'�tat
    if (gameUI) {
        gameUI->update(sf::Mouse::getPosition(*window), currentState);
    }

    if (currentState == GameState::Playing) {
        float dt = gameClock.getElapsedTime().asSeconds(); // Utiliser le temps �coul� depuis le dernier red�marrage pour Playing
        // Ou passer le deltaTime calcul� au d�but de run()
        if (dt == 0.f && previousState != GameState::Playing) { // Si on vient d'entrer en jeu, dt peut �tre 0 la premi�re frame
            // Ne rien faire ou utiliser un petit dt fixe pour la premi�re frame de logique de jeu
        }
      
    }
    if (currentState == GameState::GameOver && previousState == GameState::Playing) { // Transition de Playing � GameOver
        checkAndSetHighScore();
    }

}

void Game::updatePlayingLogic(float deltaTime) { // deltaTime est maintenant le temps �coul� pour la frame de jeu
    if (deltaTime <= 0) deltaTime = 1.f / 60.f; // Protection contre deltaTime nul ou n�gatif

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
    float actualSpawnTimerMax = obstacleSpawnTimerMax_base; // Modul� par applyCoreDifficultySettings
    int actualMaxObstacles = maxObstacles_base;          // Modul� par applyCoreDifficultySettings

    if (obstacleSpawnTimer >= actualSpawnTimerMax && obstacles.size() < static_cast<size_t>(actualMaxObstacles)) {
        trySpawnObstacle();
    }
    checkCollisions(); // Si collision -> currentState = GameState::GameOver
}


void Game::renderGameContent() {
    // Rendre le fond et les �l�ments du jeu si l'�tat est Playing ou GameOver (pour voir le jeu fig�)
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
    int actualMaxObstacles = maxObstacles_base; // D�j� ajust� par applyCoreDifficultySettings
    float actualMinSpawnDistance; // Sera d�fini par applyCoreDifficultySettings

    switch (currentDifficulty) {
    case Difficulty::Facile: actualMinSpawnDistance = 1.25f * 350.f; break;
    case Difficulty::Moyen:  actualMinSpawnDistance = 1.0f * 350.f; break;
    case Difficulty::Difficile: actualMinSpawnDistance = 0.75f * 350.f; break;
    default: actualMinSpawnDistance = 350.f;
    }
    // Utiliser les valeurs directement mises � jour par applyCoreDifficultySettings au lieu de recalculer ici.


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
    float actualSpeed = baseObstacleSpeed_base; // D�j� ajust� par applyCoreDifficultySettings

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
            // checkAndSetHighScore(); // D�plac� � la transition d'�tat
            return;
        }
    }
}

void Game::increaseInGameDifficulty() {
    currentSpeedMultiplier = std::min(currentSpeedMultiplier + 0.05f, 2.5f);
    
}

void Game::applyCoreDifficultySettings() {
    // Cette fonction ajuste les variables de base du jeu en fonction de currentDifficulty
    // Elle est appel�e quand la difficult� change dans les settings ou au d�but d'une partie.
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
    if (gameUI) gameUI->updateHighScoreDisplayTexts(highScore); // Mettre � jour l'UI
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
        saveHighScoreToFile(); // Sauvegarder imm�diatement
        if (gameUI) gameUI->updateHighScoreDisplayTexts(highScore); // Mettre � jour l'UI
    }
}

// La fonction pixelPerfectCollision reste la m�me
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
