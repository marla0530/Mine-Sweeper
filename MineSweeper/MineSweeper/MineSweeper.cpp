#include <SFML/Graphics.hpp>
#include <iostream>
#include "Random.h"
#include "TextureManager.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <string>
#include <sstream>
using namespace std;
struct Space {
    Space();
    sf::Sprite flag;
    sf::Sprite tile;
    sf::Sprite clicked;
    sf::Sprite bomb;
    sf::Sprite number;
    bool bombHere;
    bool flagHere;
    bool tileHere;
    bool clickedHere;
    bool numberHere;
    bool recursive;
    void Draw(sf::RenderWindow& window);
    void DrawBomb(sf::RenderWindow& window);
    vector<Space*> adjacentTiles;
    int nearBomb;
    void setNumber();
};
Space::Space() {
    flag.setTexture(TextureManager::GetTexture("flag"));
    tile.setTexture(TextureManager::GetTexture("tile_hidden"));
    clicked.setTexture(TextureManager::GetTexture("tile_revealed"));
    bomb.setTexture(TextureManager::GetTexture("mine"));
    nearBomb = 0;
    recursive = false;
    bombHere = false;
    flagHere = false;
    tileHere = true;
    clickedHere = false;
    
}
void Space::setNumber() {
    if (nearBomb == 1) { number.setTexture(TextureManager::GetTexture("number_1")); }
    else if (nearBomb == 2) { number.setTexture(TextureManager::GetTexture("number_2")); }
    else if (nearBomb == 3) { number.setTexture(TextureManager::GetTexture("number_3")); }
    else if (nearBomb == 4) { number.setTexture(TextureManager::GetTexture("number_4")); }
    else if (nearBomb == 5) { number.setTexture(TextureManager::GetTexture("number_5")); }
    else if (nearBomb == 6) { number.setTexture(TextureManager::GetTexture("number_6")); }
    else if (nearBomb == 7) { number.setTexture(TextureManager::GetTexture("number_7")); }
    else if (nearBomb == 8) { number.setTexture(TextureManager::GetTexture("number_8")); }
}
void Space::Draw(sf::RenderWindow& window) {
    if (tileHere) {
        window.draw(tile);
    }
    if (clickedHere) {
        window.draw(clicked);
    }
    if (numberHere) {
        window.draw(number);
    }

    if (flagHere) {
        window.draw(flag);
    }

}

void Space::DrawBomb(sf::RenderWindow& window) {
    window.draw(bomb);
}


void Refresh(sf::RenderWindow& window, int& mines,  int& rows,  int& columns, vector<vector<Space>>& board,int& gameState, int& trueMines) {
    set<int> random;
    mines = trueMines;
    while (random.size() < mines) {
        random.insert(Random::Int(0, rows * columns));
    }
    for (int i = 0; i < rows; i++) {

        vector<Space> temp;
        for (int j = 0; j < columns; j++) {
            Space space;
            space.tile.setPosition(j * 32, i * 32);
            temp.push_back(space);

        }
        board.push_back(temp);

    }
    auto iter = random.begin();
    for (; iter != random.end(); iter++) {

        int i;
        int j = *iter % columns;
        if (j != 0|| *iter<columns) {
                i = (*iter / columns);
            }
            else {
                i = (*iter / columns)-1;
            }
            board[i][j].bombHere = true;
            board[i][j].bomb.setPosition(board[i][j].tile.getPosition().x, board[i][j].tile.getPosition().y);
        }
    

    
    gameState = 0;
    }   
void revealNear(Space& space) {
    space.recursive = true;
    for (int i = 0; i < space.adjacentTiles.size(); i++) {
        if (space.adjacentTiles[i]->nearBomb == 0 && !space.adjacentTiles[i]->flagHere && !space.adjacentTiles[i]->bombHere&& !space.adjacentTiles[i]->recursive) {
            space.adjacentTiles[i]->clickedHere = true;
            space.adjacentTiles[i]->recursive = true;
            space.adjacentTiles[i]->clicked.setPosition(space.adjacentTiles[i]->tile.getPosition().x, space.adjacentTiles[i]->tile.getPosition().y);
            revealNear(*space.adjacentTiles[i]);
        }
        else {
            if (!space.adjacentTiles[i]->flagHere) {
                space.adjacentTiles[i]->numberHere = true;
                space.adjacentTiles[i]->clickedHere = true;
                space.adjacentTiles[i]->setNumber();
                space.adjacentTiles[i]->clicked.setPosition(space.adjacentTiles[i]->tile.getPosition().x, space.adjacentTiles[i]->tile.getPosition().y);
                space.adjacentTiles[i]->number.setPosition(space.adjacentTiles[i]->tile.getPosition().x, space.adjacentTiles[i]->tile.getPosition().y);
            }
        }
    }
}
void LeftMouse(sf::Vector2i mouse, vector<vector<Space>>& board, int& gameState, int rows, int columns, int mines) {

    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board[i].size(); j++)
        {
            if (board[i][j].bombHere) {
                auto bLocation = board[i][j].bomb.getGlobalBounds();
                if (bLocation.contains(mouse.x, mouse.y) && !board[i][j].flagHere) {
                    gameState = 2;

                }
            }
        }
    }




    if (gameState == 0) {
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                auto cLocation = board[i][j].tile.getGlobalBounds();
                if (cLocation.contains(mouse.x, mouse.y)&&!board[i][j].flagHere&&!board[i][j].clickedHere) {
                    board[i][j].clickedHere = true;
                    board[i][j].clicked.setPosition(board[i][j].tile.getPosition().x, board[i][j].tile.getPosition().y);
                    if (board[i][j].nearBomb != 0) {
                        board[i][j].numberHere = true;
                        board[i][j].setNumber();
                        board[i][j].number.setPosition(board[i][j].tile.getPosition().x, board[i][j].tile.getPosition().y);
                    }
                    else  {
                        revealNear(board[i][j]);
                    }
                }
            }
        }
    }

}
void RightMouse(sf::Vector2i mouse, vector<vector<Space>>& board,int gameState) {
    if (gameState == 0) {
        
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                auto fLocation = board[i][j].tile.getGlobalBounds();
                if (fLocation.contains(mouse.x, mouse.y)&&!board[i][j].clickedHere) {
                    board[i][j].flagHere =!board[i][j].flagHere;
                    board[i][j].flag.setPosition(board[i][j].tile.getPosition().x, board[i][j].tile.getPosition().y);
                }
            }
        }
    }

}
void condition(int& gameState, vector<vector<Space>>& board, sf::RenderWindow& window) {
    if (gameState == 1) {
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                if (board[i][j].bombHere) {
                    board[i][j].flagHere = true;
             }

            }
        }
    }
    if (gameState == 0||gameState==1) {
        
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                board[i][j].Draw(window);
                
            }
        }
    }
    else if (gameState == 2) { 
        
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                board[i][j].Draw(window);
                if (board[i][j].bombHere) {
                    board[i][j].clicked.setPosition(board[i][j].tile.getPosition().x, board[i][j].tile.getPosition().y);
                    board[i][j].clickedHere = true;
                    board[i][j].tileHere = false;
                    board[i][j].DrawBomb(window);
                }

                
            }
        }
    }
}

void FlagCount(int mines, vector<vector<Space>>& board,int rows, sf::RenderWindow& window) {
    int count = 0;
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board[i].size(); j++)
        {
            if (board[i][j].flagHere) {
                count++;
            }
        }
    }
    int temp= mines - count;
    vector<sf::Sprite> rects;
    for (int i = 0; i < 4; i++) {
        sf::Sprite rect;
        rect.setTexture(TextureManager::GetTexture("digits"));
        rect.setPosition(21 * i, rows * 32);
        rects.push_back(rect);
    }

    string display = std::to_string(temp);
    if(display[0]=='-'){
        rects[0].setTextureRect(sf::IntRect(21 * 10, 0, 21, 32));
        
        if (display.size() == 4) {
            for (int i = 1; i < 4; i++) {
                int num = display[i] - 48;
                rects[i].setTextureRect(sf::IntRect(num * 21, 0, 21, 32));
            }
        }
        else if(display.size() == 3){
            rects[1].setTextureRect(sf::IntRect(0, 0, 21, 32));
            for (int i = 2; i < 4; i++) {
                int num = display[i-1] - 48;
                rects[i].setTextureRect(sf::IntRect(num * 21, 0, 21, 32));
            }
        }
        else if (display.size() == 2) {
            rects[1].setTextureRect(sf::IntRect(0, 0, 21, 32));
            rects[2].setTextureRect(sf::IntRect(0, 0, 21, 32));
            int num = display[1] - 48;
            rects[3].setTextureRect(sf::IntRect(num * 21, 0, 21, 32));
            }
        window.draw(rects[0]);
        window.draw(rects[1]);
        window.draw(rects[2]);
        window.draw(rects[3]);
        }

    else {
        if (display.size() == 3) {
            for (int i = 1; i < 4; i++) {
                int num = display[i - 1] - 48;
                rects[i].setTextureRect(sf::IntRect(num * 21, 0, 21, 32));
            }
        }
        else if (display.size() == 2) {
            rects[1].setTextureRect(sf::IntRect(0, 0, 21, 32));
            for (int i = 2; i < 4; i++) {
                int num = display[i - 2] - 48;
                rects[i].setTextureRect(sf::IntRect(num * 21, 0, 21, 32));
            }
        }
        else if (display.size() == 1) {
            rects[1].setTextureRect(sf::IntRect(0, 0, 21, 32));
            rects[2].setTextureRect(sf::IntRect(0, 0, 21, 32));
            int num = display[0] - 48;
            rects[3].setTextureRect(sf::IntRect(num*21, 0, 21, 32));
        }
        window.draw(rects[1]);
        window.draw(rects[2]);
        window.draw(rects[3]);
    }
    


    




}

void Load(string file,int& rows,int& columns, vector<vector<Space>>& board, sf::RenderWindow& window,int& mines) {
    ifstream inFile(file);
    string line;
    mines = 0;
    vector<vector<string>> input;
    for (int i = 0; i < rows; i++) {

        vector<Space> temp;
        for (int j = 0; j < columns; j++) {
            Space space;
            space.tile.setPosition(j * 32, i * 32);
            temp.push_back(space);
        }
        board.push_back(temp);

    }
    for (int i = 0; i < rows; i++) {
        getline(inFile, line);
        vector<string> temp;
        for (int j = 0; j < columns; j++) {
            string num;
            num = line[j];
            temp.push_back(num);
        }
        input.push_back(temp);
    }

    for (int i = 0; i < rows; i++) {
        
        for (int j = 0; j < columns; j++) {
            board[i][j].clickedHere = false;
            if (input[i][j] == "1")
            {
                mines++;
                board[i][j].bombHere = true;
                board[i][j].bomb.setPosition(board[i][j].tile.getPosition().x, board[i][j].tile.getPosition().y);
            }
            else {
                board[i][j].bombHere = false;
            }
        }
    }
    inFile.close();


}

void nearUpdate(int& rows, int& columns, vector<vector<Space>>& board) {
    board[0][0].adjacentTiles.push_back(&board[0][1]);
    board[0][0].adjacentTiles.push_back(&board[1][0]);
    board[0][0].adjacentTiles.push_back(&board[1][1]);
    board[rows - 1][columns - 1].adjacentTiles.push_back(&board[rows - 2][columns - 2]);
    board[rows - 1][columns - 1].adjacentTiles.push_back(&board[rows - 1][columns - 2]);
    board[rows - 1][columns - 1].adjacentTiles.push_back(&board[rows - 2][columns - 1]);
    board[0][columns - 1].adjacentTiles.push_back(&board[1][columns - 1]);
    board[0][columns - 1].adjacentTiles.push_back(&board[1][columns - 2]);
    board[0][columns - 1].adjacentTiles.push_back(&board[0][columns - 2]);
    board[rows - 1][0].adjacentTiles.push_back(&board[rows - 2][0]);
    board[rows - 1][0].adjacentTiles.push_back(&board[rows - 1][1]);
    board[rows - 1][0].adjacentTiles.push_back(&board[rows - 2][1]);

    for (int i = 1; i < rows - 1; i++) {
        board[i][0].adjacentTiles.push_back(&board[i - 1][0]);
        board[i][0].adjacentTiles.push_back(&board[i + 1][0]);
        board[i][0].adjacentTiles.push_back(&board[i][1]);
        board[i][0].adjacentTiles.push_back(&board[i-1][1]);
        board[i][0].adjacentTiles.push_back(&board[i+1][1]);

        board[i][columns - 1].adjacentTiles.push_back(&board[i - 1][columns - 1]);
        board[i][columns - 1].adjacentTiles.push_back(&board[i + 1][columns - 1]);
        board[i][columns - 1].adjacentTiles.push_back(&board[i][columns - 2]);
        board[i][columns - 1].adjacentTiles.push_back(&board[i - 1][columns - 2]);
        board[i][columns - 1].adjacentTiles.push_back(&board[i + 1][columns - 2]);

    }
    for (int i = 1; i < columns - 1; i++) {
        board[0][i].adjacentTiles.push_back(&board[0][i-1]);
        board[0][i].adjacentTiles.push_back(&board[0][i + 1]);
        board[0][i].adjacentTiles.push_back(&board[1][i]);
        board[0][i].adjacentTiles.push_back(&board[1][i-1]);
        board[0][i].adjacentTiles.push_back(&board[1][i+1]);

        board[rows - 1][i].adjacentTiles.push_back(&board[rows-1][i - 1]);
        board[rows - 1][i].adjacentTiles.push_back(&board[rows-1][i + 1]);
        board[rows - 1][i].adjacentTiles.push_back(&board[rows-2][i]);
        board[rows - 1][i].adjacentTiles.push_back(&board[rows - 2][i - 1]);
        board[rows - 1][i].adjacentTiles.push_back(&board[rows - 2][i + 1]);

    }
    for (int i = 1; i < rows-1; i++) {

        for (int j = 1; j < columns-1; j++) {
            board[i][j].adjacentTiles.push_back(&board[i - 1][j]);
            board[i][j].adjacentTiles.push_back(&board[i + 1][j]);
            board[i][j].adjacentTiles.push_back(&board[i][j+1]);
            board[i][j].adjacentTiles.push_back(&board[i][j - 1]);
            board[i][j].adjacentTiles.push_back(&board[i+1][j + 1]);
            board[i][j].adjacentTiles.push_back(&board[i-1][j + 1]);
            board[i][j].adjacentTiles.push_back(&board[i-1][j - 1]);
            board[i][j].adjacentTiles.push_back(&board[i + 1][j - 1]);
        }
    }

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < columns; j++) {
            if (!board[i][j].bombHere) {
                for (int k = 0; k < board[i][j].adjacentTiles.size(); k++) {
                    if (board[i][j].adjacentTiles[k]->bombHere) {
                        board[i][j].nearBomb++;
                    }
                }
           }
        }
    }
    

}


int main()
{   
    ifstream inFile("boards/config.cfg");
    string line;
    string temp;
    int columns;
    int rows;
    int mines;
    getline(inFile,temp);
    columns = stoi(temp);
    getline(inFile, temp);
    rows = stoi(temp);
    getline(inFile, temp);
    mines = stoi(temp);
    int gameState = 0;
    int trueMines = mines;
    sf::RenderWindow window(sf::VideoMode(columns*32, (rows*32)+88), "ANYTHING I WANT");
    sf::Sprite debug(TextureManager::GetTexture("debug"));
    sf::Sprite face_happy(TextureManager::GetTexture("face_happy"));
    sf::Sprite face_lose(TextureManager::GetTexture("face_lose"));
    sf::Sprite face_win(TextureManager::GetTexture("face_win"));  
    sf::Sprite test1(TextureManager::GetTexture("test_1"));
    sf::Sprite test2(TextureManager::GetTexture("test_2"));
    sf::Sprite test3(TextureManager::GetTexture("test_3"));
    vector<vector<Space>> board;
    Refresh(window, mines, rows, columns, board,gameState,trueMines);
    nearUpdate(rows, columns, board);
    face_happy.setPosition(columns * 32 / 2 - 32, rows * 32);
    face_lose.setPosition(columns * 32 / 2 - 32, rows * 32);
    face_win.setPosition(columns * 32 / 2 - 32, rows * 32);
    debug.setPosition(columns * 32 / 2 +96, rows * 32);
    test1.setPosition(columns * 32 / 2 + 160, rows * 32);
    test2.setPosition(columns * 32 / 2 + 224, rows * 32);
    test3.setPosition(columns * 32 / 2 + 288, rows * 32);
    
    bool debugPressed=false;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            sf::Vector2i mouse = sf::Mouse::getPosition(window);

            if (event.type == sf::Event::MouseButtonPressed)
            {
         
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    auto reset = face_happy.getGlobalBounds();
                    auto hacks = debug.getGlobalBounds();
                    auto t1 = test1.getGlobalBounds();
                    auto t2 = test2.getGlobalBounds();
                    auto t3 = test3.getGlobalBounds();

                    //checking for mines
                    if (reset.contains(mouse.x, mouse.y)) {
                        board.clear();
                        Refresh(window, mines, rows, columns, board,gameState,trueMines);
                        nearUpdate(rows, columns, board);
                    }
                    if (t1.contains(mouse.x, mouse.y)) {
                        board.clear();
                        Load("boards/testboard1.brd",rows,columns,board,window,mines);
                        nearUpdate(rows, columns, board);
                        gameState = 0;
                    }
                    if (t2.contains(mouse.x, mouse.y)) {
                        board.clear();
                        Load("boards/testboard2.brd", rows, columns, board, window, mines);
                        nearUpdate(rows, columns, board);
                        gameState = 0;
                    }
                    if(t3.contains(mouse.x, mouse.y)) {
                        board.clear();
                        Load("boards/testboard3.brd", rows, columns, board, window, mines);
                        nearUpdate(rows, columns, board);
                        gameState = 0;
                    }
                    if (hacks.contains(mouse.x, mouse.y)) {
                        debugPressed = !debugPressed;
                    }
                    LeftMouse(mouse, board, gameState, rows, columns, mines);

                }
                else if (event.mouseButton.button == sf::Mouse::Right) {
                    
                    RightMouse(mouse, board, gameState);
                    
                   
                }
            }
        }

        window.clear();
        int clickedCount = 0;
        for (int i = 0; i < board.size(); i++)
        {
            for (int j = 0; j < board[i].size(); j++)
            {
                if (board[i][j].clickedHere) {
                    clickedCount++;

                }
            }
        }

        if (rows * columns - clickedCount == mines) {
            gameState = 1;
        }
        condition(gameState, board, window);
        if (gameState == 0) {
            window.draw(face_happy);
        }
        else if (gameState == 1) {
            window.draw(face_win);
        }
        else if (gameState == 2) {
            window.draw(face_lose);
        }
        window.draw(debug);
        window.draw(test1);
        window.draw(test2);
        window.draw(test3);

        FlagCount(mines, board, rows, window);

        

        if(debugPressed&& gameState!=1){
            for (int i = 0; i < board.size(); i++)
            {
                for (int j = 0; j < board[i].size(); j++)
                {
                    if (board[i][j].bombHere) {
                        board[i][j].DrawBomb(window);
                    }
                }
            }
        }
      
        window.display();
    }
    TextureManager::Clear();

    return 0;
}