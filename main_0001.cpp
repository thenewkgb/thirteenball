#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <iostream>
#include <stdexcept>

#define BALLSIZE 120
#define ROWONESIZE 3
#define ROWTWOSIZE 4
#define ROWTHREESIZE 5
#define ROWFOURSIZE 6

struct Ball : SDL_Rect
{
    int x = 0;
    int y = 0;
    int w = BALLSIZE;
    int h = BALLSIZE;
    int value;
    bool show = true;
    bool highlighted = false;
    SDL_Texture *texture;
};

struct EmptyBall : SDL_Rect
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    int value = 0;
};

struct Table
{
    std::vector<Ball> balls;
};

class Game
{
  public:
    Game()
    {
    }

    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Event e{};
    SDL_TouchFingerEvent f;

    SDL_Rect quitButton{};
    SDL_Rect reRackButton{};
    SDL_Rect touch{};

    // some miscellanious vars
    int sw, sh;
    bool isQuit{};
    std::vector<int> totalSum{};
    Table table{};
    std::vector<int> locations{};

    int init();
    void setupTable();
    void loadGFX();

    bool canClick(int i);
    void highlight(int i);
    void deselect(int i);
    void handleHighlight(int i);
    void createEmpty(int i);
    void touchingBall(int i);

    void arrangeTotal();
    void replaceEmpty();
    int findLocation(int value, std::vector<int> v);

    bool totalThirteen();

    void clearValues();
    void beginBoard();
    void updateScreen();
    void checkInput();

    void quit();
};

// Taken from SDL2 help files
static SDL_Texture *loadImage(SDL_Renderer *renderer, const char *path)
{
    SDL_Surface *img = IMG_Load(path);
    if (img == NULL)
    {
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, img);
    SDL_FreeSurface(img);

    if (texture == NULL)
    {
        return NULL;
    }
    return texture;
}

int Game::init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        return -1;
    }

    window = SDL_CreateWindow("Hello SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    if (window == NULL)
    {
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_GetRendererOutputSize(renderer, &sw, &sh);

    srand(time(0));

    return 0;
}

void Game::quit()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::beginBoard()
{
    clearValues();
    loadGFX();
    setupTable();
    updateScreen();
}

void Game::clearValues()
{
    table.balls.clear();
    totalSum.clear();
    locations.clear();
}

void Game::createEmpty(int i)
{
    table.balls[i].w = 0;
    table.balls[i].h = 0;
    table.balls[i].show = false;
    // highlighted flips at sum of 13
    //table.balls[i].highlighted = false;
}

void Game::loadGFX()
{
    // quit stuff
    quitButton.x = 400;
    quitButton.y = 150;
    quitButton.w = 200;
    quitButton.h = 100;

    reRackButton.x = 100;
    reRackButton.y = 150;
    reRackButton.w = 200;
    reRackButton.h = 100;
}

void Game::highlight(int i)
{
    SDL_SetTextureColorMod(table.balls[i].texture, 200, 200, 255);
    table.balls[i].highlighted = true;
}

void Game::deselect(int i)
{
    SDL_SetTextureColorMod(table.balls[i].texture, 255, 255, 255);
    table.balls[i].highlighted = false;
}

void Game::handleHighlight(int i)
{
    if (!table.balls[i].highlighted)
    {
        highlight(i);
    }
    else
    {
        deselect(i);
    }
}

void Game::setupTable()
{
    // positions
    for (int i = 2; i > -1; --i)
    {
        int x = (sw / 2) + (120 * i) - 180;
        Ball temp;
        temp.x = x;
        temp.y = (sh / 2) - 180;
        //table.rowOfThree.push_back(temp);
        table.balls.push_back(temp);
    }

    for (int i = 2; i > -2; --i)
    {
        int x = (sw / 2) + (120 * i) - 120;
        Ball temp;
        temp.x = x;
        temp.y = (sh / 2) - 120;
        table.balls.push_back(temp);
        //table.rowOfFour.push_back(temp);
    }

    for (int i = 2; i > -3; --i)
    {
        int x = (sw / 2) + (120 * i) - 60;
        Ball temp;
        temp.x = x;
        temp.y = (sh / 2) - 60;
        table.balls.push_back(temp);
        //table.rowOfFive.push_back(temp);
    }

    for (int i = 2; i > -4; --i)
    {
        int x = (sw / 2) + (120 * i);
        Ball temp;
        temp.x = x;
        temp.y = (sh / 2);
        table.balls.push_back(temp);
        //table.rowOfSix.push_back(temp);
    }

    // randomly place numbers 1 to 13
    // into table
    // as we have their positions sorted
    //
    // we need separate textures
    // for each ball
    // as we could have duplicates

    for (int i = 0; i < table.balls.size(); ++i)
    {
        int randvalue = 1 + (rand() % 12);
        // create a number separately
        SDL_Texture *tempTex{};
        std::string s{};
        s += std::to_string(randvalue) + ".png";
        tempTex = loadImage(renderer, s.c_str());
        // give ball a value
        table.balls[i].value = randvalue;
        // and corresponding texture
        table.balls[i].texture = tempTex;
    }
}

bool Game::canClick(int i)
{
    // i >= 0 && i < 3
    if (i >= 0 && i < ROWONESIZE)
    {
        if (table.balls[i + ROWONESIZE].show == false &&
            table.balls[i + ROWONESIZE + 1].show == false)
        {
            return true;
        }
    }

    // i >= 3 && i < 7
    else if (i >= ROWONESIZE && i < ROWTWOSIZE + ROWONESIZE)
    {
        if (table.balls[i + ROWTWOSIZE].show == false && table.balls[i + ROWTWOSIZE + 1].show == false)
        {
            return true;
        }
    }

    // i >= 7 && i < 12
    else if (i >= ROWONESIZE + ROWTWOSIZE && i < ROWTHREESIZE + ROWTWOSIZE + ROWONESIZE)
    {
        if (table.balls[i + ROWTHREESIZE].show == false && table.balls[i + ROWTHREESIZE + 1].show == false)
        {
            return true;
        }
    }
    else if (i >= ROWTHREESIZE + ROWTWOSIZE + ROWONESIZE && i < ROWFOURSIZE + ROWTHREESIZE + ROWTWOSIZE + ROWONESIZE)
    {
        return true;
    }

    else
    {
        return false;
    }
}

bool Game::totalThirteen()
{
    int total = 0;

    for (int t : totalSum)
        total += t;

    if (total == 13)
    {
        return true;
    }

    return false;
}

int Game::findLocation(int value, std::vector<int> v)
{
    // search vector for same number
    for (int i = 0; i < v.size(); ++i)
    {
        if (value == v[i])
            return i;
    }
    return -1;
}

void Game::arrangeTotal()
{
    for (int i = 0; i < table.balls.size(); ++i)
    {
        // do not push if ball location already present
        if (table.balls[i].highlighted)
        {
           // int foundValueAt = findLocation(table.balls[i].value, totalSum);
            int loc = findLocation(i, locations);

            // if there is no location
            if (loc == -1)
            {
                totalSum.push_back(table.balls[i].value);
                locations.push_back(i);
            }
        }

        if (!table.balls[i].highlighted)
        {
            // remove value from totalsum
            //int foundValueAt = findLocation(table.balls[i].value, totalSum);
            int loc = findLocation(i, locations);

            // did find something
            if (loc > -1)
            {
                //totalSum.erase(totalSum.begin() + foundValueAt);
                totalSum.erase(totalSum.begin() + loc);
                locations.erase(locations.begin() + loc);
            }
        }
    }
}

void Game::touchingBall(int i)
{
    if (canClick(i))
    {
        handleHighlight(i);
        arrangeTotal();
    }

    if (totalThirteen())
    {
        for (int x : locations)
        {
            createEmpty(x);
            handleHighlight(x);
            //totalSum.clear();
            //locations.clear();
            //arrangeTotal();
        }
    }
}

void Game::checkInput()
{
    while (SDL_PollEvent(&e) != 0)
    {
        //User touches screen
        if (e.type == SDL_FINGERUP)
        {
            f = e.tfinger;
            touch.x = f.x * sw;
            touch.y = f.y * sh;
            touch.w = 1;
            touch.h = 1;

            for (int i = 0; i < table.balls.size(); ++i)
            {
                SDL_Rect B{};
                B.x = table.balls[i].x;
                B.y = table.balls[i].y;
                B.w = table.balls[i].w;
                B.h = table.balls[i].h;

                if (SDL_HasIntersection(&touch, &B))
                {
                    touchingBall(i);
                }
            }

            if (SDL_HasIntersection(&touch, &reRackButton))
            {
                beginBoard();
            }

            if (SDL_HasIntersection(&touch, &quitButton))
            {
                isQuit = true;
            }
        }
    }
}

void Game::updateScreen()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // pop a load of balls on screen
    for (int i = 0; i < table.balls.size(); ++i)
    {
        // convert Ball to Rect
        // RenderCopy needs Rect
        SDL_Rect dest;
        dest.x = table.balls[i].x;
        dest.y = table.balls[i].y;
        dest.w = table.balls[i].w;
        dest.h = table.balls[i].h;

        if (table.balls[i].show == true)
        {
            SDL_RenderCopy(renderer, table.balls[i].texture, NULL, &dest);
        }
    }
    // quit button
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &quitButton);
    // play again button
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(renderer, &reRackButton);
    // display
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    Game game{};

    if (game.init() != 0)
    {
        return -1;
    }
    else
    {
        game.beginBoard();
    }

    while (!game.isQuit)
    {
        game.checkInput();
        game.updateScreen();
    }

    game.quit();
    return 0;
}
