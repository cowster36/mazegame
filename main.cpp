#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

int main() {
    sf::RenderWindow window(sf::VideoMode(600, 600), "game");
    std::srand(std::sin(std::time(nullptr)) * 1000);
    int frameCount = 0;
    int moveTimer = 1000;

    bool updatePath = true;
    int orderedSet [15 * 15];
    int path [100];
    int pathSize = 0;
    int pathPos = 0;

    sf::Vector2i player = sf::Vector2i(13, 13);
    sf::Texture playerTex;
    playerTex.loadFromFile("player.png");
    sf::Sprite playerSprite;
    playerSprite.setTexture(playerTex);
    playerSprite.setPosition(player.x * 40.f, player.y * 40.f);

    sf::Vector2i opponent = sf::Vector2i(1, 1);
    sf::Texture opponentTex;
    opponentTex.loadFromFile("enemy.png");
    sf::Sprite opponentSprite;
    opponentSprite.setTexture(opponentTex);
    opponentSprite.setPosition(opponent.x * 40.f, opponent.y * 40.f);

    sf::Texture coinTex;
    coinTex.loadFromFile("coin.png");
    sf::Sprite coins [5];
    int numCoins = 0;
    int score = 0;

    int gameMap [15 * 15];
    sf::RectangleShape displayRects [15 * 15];

    for(int i = 0; i < 15; i++) {
        for(int j = 0; j < 15; j++) {
            displayRects[i + j * 15].setPosition(i * 40.f, j * 40.f);
            displayRects[i + j * 15].setSize(sf::Vector2f(40.f, 40.f));
            displayRects[i + j * 15].setOutlineThickness(1.f);
            displayRects[i + j * 15].setOutlineColor(sf::Color(0, 0, 0));

            if(!(i == opponent.x && j == opponent.y) && !(i == player.x && j == player.y)){
                if(std::rand()/ (float) RAND_MAX < 0.22f || i == 0 || j == 0 || i == 14 || j == 14) {
                    gameMap[i + j * 15] = 1;
                    displayRects[i + j * 15].setFillColor(sf::Color(0, 0, 0));
                }
            }
        }
    }

    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                window.close();
            } else if(event.type == sf::Event::KeyPressed) {
                switch(event.key.code){
                    case sf::Keyboard::Up:
                        if(gameMap[player.x + (player.y - 1) * 15] != 1) player.y -= 1;
                        break;
                    case sf::Keyboard::Down:
                        if(gameMap[player.x + (player.y + 1) * 15] != 1) player.y += 1;
                        break;
                    case sf::Keyboard::Right:
                        if(gameMap[(player.x + 1) + player.y * 15] != 1) player.x += 1;
                        break;
                    case sf::Keyboard::Left:
                        if(gameMap[(player.x - 1) + player.y * 15] != 1) player.x -= 1;
                        break;
                }
            }

            updatePath = true;
            pathSize = 0;
            pathPos = 0;
            playerSprite.setPosition(player.x * 40.f, player.y * 40.f);
        }

        while(numCoins < 3) {
            int col = (std::rand() / (float) RAND_MAX) * 15.f;
            int row = (std::rand() / (float) RAND_MAX) * 15.f;

            if(gameMap[col + row * 15] == 0) {
                gameMap[col + row * 15] = 2;
                coins[numCoins].setTexture(coinTex);
                coins[numCoins].setPosition(col * 40.f, row * 40.f);
                numCoins += 1;
            }
        }

        window.clear(sf::Color(255, 255, 255));

        for(int i = 0; i < 15 * 15; i++) {
            window.draw(displayRects[i]);
        }

        for(int i = 0; i < numCoins; i++) {
            window.draw(coins[i]);
        }

        if(updatePath == true) {
            int counter = 0;

            int fullSet [2000];
            int fullSetSize = 0;

            int openSet [100];
            int openSetSize = 2;
            openSet[0] = player.x + player.y * 15;
            openSet[1] = counter;

            int currentIndex = player.x + player.y * 15;

            while(currentIndex != opponent.x + opponent.y * 15) {
                currentIndex = openSet[0];
                counter = openSet[1] + 1;
                int neighbors [4];

                neighbors[0] = currentIndex - 1;
                neighbors[1] = currentIndex + 1;
                neighbors[2] = currentIndex - 15;
                neighbors[3] = currentIndex + 15;

                for(int i = 0; i < 8; i += 2) {
                    bool alreadyExists = false;

                    for(int j = 0; j < fullSetSize; j += 2) {
                        if(neighbors[i/2] == fullSet[j]) {
                            alreadyExists = true;
                            break;
                        }
                    }

                    if(alreadyExists == false) {
                        if(gameMap[neighbors[i/2]] != 1) {
                            fullSet[fullSetSize] = neighbors[i/2];
                            fullSet[fullSetSize + 1] = counter;
                            fullSetSize += 2;

                            openSet[openSetSize] = neighbors[i/2];
                            openSet[openSetSize + 1] = counter;
                            openSetSize += 2;
                        } else {
                            fullSet[fullSetSize] = neighbors[i/2];
                            fullSet[fullSetSize + 1] = 100000;
                            fullSetSize += 2;
                        }
                    }
                }

                for(int i = 0; i < openSetSize; i++) {
                    openSet[i] = openSet[i + 2];
                }

                openSetSize -= 2;
            }

            for(int i = 0; i < 15 * 15; i++) {
                orderedSet[i] = 100000;
            }

            for(int i = 0; i < fullSetSize; i += 2) {
                orderedSet[fullSet[i]] = fullSet[i + 1];
            }

            orderedSet[player.x + player.y * 15] = 0;

            int pathIndex = opponent.x + opponent.y * 15;

            while(pathIndex != player.x + player.y * 15) {
                int neighbors [4];

                neighbors[0] = pathIndex - 1;
                neighbors[1] = pathIndex + 1;
                neighbors[2] = pathIndex - 15;
                neighbors[3] = pathIndex + 15;

                int lowest [2] {0, 100000};
                for(int i = 0; i < 4; i++) {
                    if(orderedSet[neighbors[i]] < lowest[1]) {
                        lowest[0] = i;
                        lowest[1] = orderedSet[neighbors[i]];
                    }
                }

                pathIndex = neighbors[lowest[0]];

                path[pathSize] = pathIndex;

                pathSize += 1;
            }

            updatePath = false;
        }

        if(frameCount % moveTimer == 0) {
            opponent.x = path[pathPos] % 15;
            opponent.y = std::floor(path[pathPos]/15);
            opponentSprite.setPosition(opponent.x * 40.f, opponent.y * 40.f);
            pathPos += 1;

            if(moveTimer >= 250) {
                moveTimer = 1000 - std::floor(frameCount/1000) * 4;
            }
        }

        window.draw(playerSprite);
        window.draw(opponentSprite);

        window.display();

        if(gameMap[player.x + player.y * 15] == 2) {
            gameMap[player.x + player.y * 15] = 0;

            int coinIndex = -1;
            for(int i = 0; i < numCoins ; i++) {
                if(coins[i].getPosition().x/40 == player.x && coins[i].getPosition().y/40 == player.y) {
                    coinIndex = i;
                }
            }

            if(coinIndex != numCoins - 1) {
                coins[coinIndex] = coins[numCoins - 1];
            }

            score += 1;
            numCoins -= 1;
        }

        frameCount += 1;

        if(player == opponent) {
            std::cout << "Final Score: " << score << std::endl;
            return 0;
        }
    }
    return 0;
}
