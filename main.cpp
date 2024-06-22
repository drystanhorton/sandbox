#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int CELL_SIZE = 8;

const int GRID_WIDTH = WINDOW_WIDTH / CELL_SIZE;
const int GRID_HEIGHT = WINDOW_HEIGHT / CELL_SIZE;

enum CellType {
    EMPTY,
    SAND,
    WATER,
    STONE,
    WOOD,
    FIRE // Adding fire
};

struct Cell {
    CellType type;

    Cell() : type(EMPTY) {}
};

std::vector<std::vector<Cell>> grid(GRID_WIDTH, std::vector<Cell>(GRID_HEIGHT));

void handleFire(int x, int y) {
    // extinguish fire if it comes in contact with water
    if (y + 1 < GRID_HEIGHT && grid[x][y + 1].type == WATER) {
        grid[x][y].type = EMPTY;
        return;
    }
    if (y - 1 >= 0 && grid[x][y - 1].type == WATER) {
        grid[x][y].type = EMPTY;
        return;
    }
    if (x + 1 < GRID_WIDTH && grid[x + 1][y].type == WATER) {
        grid[x][y].type = EMPTY;
        return;
    }
    if (x - 1 >= 0 && grid[x - 1][y].type == WATER) {
        grid[x][y].type = EMPTY;
        return;
    }

    // spread fire to adjacent wood cells
    if (y + 1 < GRID_HEIGHT && grid[x][y + 1].type == WOOD) {
        grid[x][y + 1].type = FIRE;
    }
    if (y - 1 >= 0 && grid[x][y - 1].type == WOOD) {
        grid[x][y - 1].type = FIRE;
    }
    if (x + 1 < GRID_WIDTH && grid[x + 1][y].type == WOOD) {
        grid[x + 1][y].type = FIRE;
    }
    if (x - 1 >= 0 && grid[x - 1][y].type == WOOD) {
        grid[x - 1][y].type = FIRE;
    }

    if (rand() % 100 < 5) { // 5% chance to extinguish
        grid[x][y].type = EMPTY;
    }
}

void updateGrid() {
    for (int y = GRID_HEIGHT - 2; y >= 0; --y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (grid[x][y].type == SAND) {
                if (y + 1 < GRID_HEIGHT && grid[x][y + 1].type == EMPTY) {
                    std::swap(grid[x][y], grid[x][y + 1]);
                }
                else if (x - 1 >= 0 && x + 1 < GRID_WIDTH && y + 1 < GRID_HEIGHT && grid[x - 1][y + 1].type == EMPTY && grid[x + 1][y + 1].type == EMPTY) {
                    if (rand() % 2) {
                        std::swap(grid[x][y], grid[x - 1][y + 1]);
                    }
                    else {
                        std::swap(grid[x][y], grid[x + 1][y + 1]);
                    }
                }
                else if (x - 1 >= 0 && y + 1 < GRID_HEIGHT && grid[x - 1][y + 1].type == EMPTY) {
                    std::swap(grid[x][y], grid[x - 1][y + 1]);
                }
                else if (x + 1 < GRID_WIDTH && y + 1 < GRID_HEIGHT && grid[x + 1][y + 1].type == EMPTY) {
                    std::swap(grid[x][y], grid[x + 1][y + 1]);
                }
                else if (y + 1 < GRID_HEIGHT && grid[x][y + 1].type == WATER) {
                    std::swap(grid[x][y], grid[x][y + 1]);
                }
            }
            else if (grid[x][y].type == WATER) {
                if (y + 1 < GRID_HEIGHT && grid[x][y + 1].type == EMPTY) {
                    std::swap(grid[x][y], grid[x][y + 1]);
                }
                else {
                    int direction = rand() % 3;
                    if (direction == 0 && x - 1 >= 0 && y + 1 < GRID_HEIGHT && grid[x - 1][y + 1].type == EMPTY) {
                        std::swap(grid[x][y], grid[x - 1][y + 1]);
                    }
                    else if (direction == 1 && x + 1 < GRID_WIDTH && y + 1 < GRID_HEIGHT && grid[x + 1][y + 1].type == EMPTY) {
                        std::swap(grid[x][y], grid[x + 1][y + 1]);
                    }
                    else {
                        direction = rand() % 2;
                        if (direction == 0 && x - 1 >= 0 && grid[x - 1][y].type == EMPTY) {
                            std::swap(grid[x][y], grid[x - 1][y]);
                        }
                        else if (direction == 1 && x + 1 < GRID_WIDTH && grid[x + 1][y].type == EMPTY) {
                            std::swap(grid[x][y], grid[x + 1][y]);
                        }
                    }
                }
            }
            else if (grid[x][y].type == FIRE) {
                handleFire(x, y);
            }
            // add wood behavior (does not move)
            else if (grid[x][y].type == WOOD) {
                // do this later
            }
        }
    }
}

void handleMouseClick(sf::RenderWindow& window, CellType selectedType) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        // get the current mouse position in the window
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // convert it to world coordinates
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
        int gridX = worldPos.x / CELL_SIZE;
        int gridY = worldPos.y / CELL_SIZE;

        // make sure none of the particles exit the boundaries
        if (gridX >= 3 && gridX < GRID_WIDTH - 3 && gridY >= 3 && gridY < GRID_HEIGHT - 3) {
            grid[gridX][gridY].type = selectedType;
            grid[gridX + 1][gridY].type = selectedType;
            grid[gridX][gridY - 1].type = selectedType;
            grid[gridX][gridY + 1].type = selectedType;
            grid[gridX - 1][gridY].type = selectedType;
        }
    }
}

void drawGrid(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);

    sf::RectangleShape cellShape(sf::Vector2f(CELL_SIZE, CELL_SIZE));

    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            if (grid[x][y].type == SAND) {
                cellShape.setFillColor(sf::Color::Yellow);
                cellShape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                window.draw(cellShape);
            }
            else if (grid[x][y].type == WATER) {
                cellShape.setFillColor(sf::Color::Blue);
                cellShape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                window.draw(cellShape);
            }
            else if (grid[x][y].type == STONE) {
                cellShape.setFillColor(sf::Color(211, 211, 211)); // grey color for stone
                cellShape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                window.draw(cellShape);
            }
            else if (grid[x][y].type == WOOD) {
                cellShape.setFillColor(sf::Color(139, 69, 19)); // brown color for wood
                cellShape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                window.draw(cellShape);
            }
            else if (grid[x][y].type == FIRE) {
                cellShape.setFillColor(sf::Color::Red); // red color for fire
                cellShape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                window.draw(cellShape);
            }
        }
    }

    window.display();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "sand");
    window.setVerticalSyncEnabled(true);

    CellType selectedType = SAND;  // start with sand by default

    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) {
                    selectedType = SAND;
                }
                else if (event.key.code == sf::Keyboard::Num2) {
                    selectedType = WATER;
                }
                else if (event.key.code == sf::Keyboard::Num3) {
                    selectedType = STONE;
                }
                else if (event.key.code == sf::Keyboard::Num4) {
                    selectedType = WOOD;
                }
                else if (event.key.code == sf::Keyboard::Num5) {
                    selectedType = FIRE; // Add fire selection
                }
            }
        }

        handleMouseClick(window, selectedType);
        updateGrid();
        drawGrid(window);
    }

    return 0;
}
