# Super-voiture-2D
Développement d’un mini-jeu de course en C++ avec 
gestion d’obstacles et d’un trajet 
1. Introduction :  
Ce projet s’inscrit dans le cadre du module de programmation orientée objet en C++. Il a pour 
objectif la conception et la réalisation d’un mini-jeu de course, dans lequel le joueur contrôle une 
voiture devant éviter les obstacles sur la route tout en progressant sur un trajet. 
Ce projet nous permet de mettre en pratique les notions de programmation orientée objet, la 
gestion d’événements en temps réel, l’utilisation de bibliothèques graphiques (comme SFML), ainsi 
que la structuration d’un projet logiciel en C++. 
À travers ce rapport, nous allons détailler les étapes de conception, le choix des outils, 
l’implémentation du jeu ainsi que les difficultés rencontrées durant le développement. 
2. Conception et modélisation : 
La conception du jeu a été structurée de manière modulaire afin de favoriser la clarté, la 
réutilisabilité et la maintenabilité du code. Cinq composants principaux ont été identifiés et 
modélisés sous forme de classes distinctes : Voiture, Obstacle, Game, Background et UI (Interface 
Utilisateur). Cette architecture orientée objet permet une séparation claire des responsabilités et 
facilite l'évolution du projet. 
a) Composants du jeu : 
• Voiture : Représente le véhicule contrôlé par le joueur. Elle peut se déplacer 
latéralement (haut/bas) en réponse aux entrées clavier. Elle est également responsable 
de la détection de collisions avec les obstacles. 
• Obstacle : Objets dynamiques générés périodiquement sur la route. et descendent 
verticalement pour simuler le mouvement. Le joueur doit les éviter. 
• Game : Constitue le cœur de l’application. Cette classe centralise la logique du jeu, 
notamment la boucle principale, la gestion des événements (entrées clavier, fermetures 
de fenêtre), le calcul des collisions, le score et les conditions de fin de partie. 
• Background : Gère le fond visuel du jeu, notamment l’effet de défilement vertical de la 
route et du décor afin de donner l’illusion de vitesse et de déplacement continu. 
• UI (Interface Utilisateur) : S’occupe de l’affichage des informations essentielles à l’écran 
: score du joueur, messages contextuels (ex. : "Game Over", "Appuyez sur jouer pour 
commencer", etc.). 
