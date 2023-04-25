/*
Version 1.1
Soon - score and hints v1.2
Author thenewkgb
Title 13 Ball

=== todo ===

Win screen - particle FX

Score - you only plus a point if you
             manually clear the table
             no points for hitting rerack
             
SPARES - more feedback for spare

HARD - if i keep colour tints remember last colour
except i haven't studied pointers

=== bugs ===

Bug #1 - CurrentSpare - eventually we may use
                all the spares and the vector
                is still alive
                
Bug #2 - Delay still allows input
                so perhaps PollEvent || totalThirteen?
                
=====
                
Fixed - last ball touches does not highlight

Fixed - i cleared the board but the score
                didnt increase/beginBoard

Fixed - if currentSpare is max 
           then the ball fails to be erased
           but if you scroll it has gone
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <iostream>

constexpr int BALLSIZE = 120;
constexpr int FONTSIZE = 128;
constexpr int ROWONESIZE = 3;
constexpr int ROWTWOSIZE = 4;
constexpr int ROWTHREESIZE = 5;
constexpr int ROWFOURSIZE = 6;
constexpr int SPARESMAXNUM = 10;
constexpr int SONG18LENGTH = 50;

struct Font : SDL_Rect
{
    int x = 0;
    int y = 0;
    int w = FONTSIZE;
    int h = FONTSIZE;
    int value = 0;
    SDL_Texture *fontTexture = NULL;
};

struct Ball : SDL_Rect
{
    int x = 0;
    int y = 0;
    int w = BALLSIZE;
    int h = BALLSIZE;
    int value;
    bool show = true;
    bool highlighted = false;
    SDL_Texture *texture = NULL;

    // spares
    // if i'm still using colour tints
    // then remember colour
    int r = 0;
    int g = 0;
    int b = 0;
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
    std::vector<Ball> balls{};
    std::vector<Ball> spares{};
    std::vector<Font> fonts{};
    SDL_Texture *backgroundimg = nullptr;
    SDL_Texture *reRackimg = nullptr;
    SDL_Texture *quitimg = nullptr;
    SDL_Texture *turnballimg = nullptr;
    SDL_Texture *scoreDigitOne = nullptr;
};

class Game
{
    bool debug;

  public:
    Game()
    {
    }

    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Event e{};
    SDL_TouchFingerEvent f;

    // mixer
    Mix_Music *music1 = NULL;
    Mix_Music *music2 = NULL;
    Mix_Music *music3 = NULL;
    Mix_Music *music4 = NULL;

    Mix_Chunk *click = NULL;
    Mix_Chunk *win = NULL;

    SDL_Rect quitButton{};
    SDL_Rect reRackButton{};
    SDL_Rect spareButton{};
    SDL_Rect sparesPos{};
    SDL_Rect touch{};
    SDL_Rect background{};
    SDL_Rect scoreOnePos{};

    SDL_Rect timer{};
    SDL_Rect timeRemaining{};

    Table table{};

    // some miscellanious vars
    int sw, sh;
    unsigned int currentSpare = 0;
    unsigned int sparesLeft = SPARESMAXNUM;
    bool isQuit{};
    unsigned int songNo = 0;
    unsigned int timeLeft{SONG18LENGTH};
    unsigned int startTime = 0;
    unsigned int score = 0;

    std::vector<int> totalSum{};
    std::vector<int> locations{};

    int init();
    void quit();
    void setupTable();
    void loadGFX();
    int loadGameSFX();
    int loadGameMusic();

    void highlight(int i);
    void deselect(int i);
    void handleHighlight(int i);
    void hightlightSpare(int spareNum);
    void deselectSpare(int spareNum);
    void handleSpareHighlight(int spareNum);

    void createEmpty(int i);
    void touchingBall(int i);
    bool canClick(int i);
    void arrangeTotal();
    void replaceEmpty();
    int findLocation(int value, std::vector<int> v);

    bool totalThirteen();
    void handleThirteen();
    bool checkTableEmpty();

    void loopSpares();
    void colourSpares();
    void includeSpare();
    void colourBalls();

    void chooseSong();
    void checkTime();
    void updateUI();

    void clearValues();
    void beginBoard();
    void updateScreen();
    void checkInput();
};

// Function for loading an image to SDL_Texture
// by included example
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

// Function for loading music to Mix_Music
static Mix_Music *loadMusic(const char *path)
{
    Mix_Music *music = Mix_LoadMUS(path);
    if (music == NULL)
    {
        return NULL;
    }
    return music;
}

int Game::loadGameMusic()
{
    std::string m{};
    m = "sfx/song18.mp3";

    music1 = loadMusic(m.c_str());
    if (music1 == NULL)
        return -1;
    /*
    more music here
    */

    return 0;
}

static Mix_Chunk *loadSound(const char *path)
{
    Mix_Chunk *sound = Mix_LoadWAV(path);
    if (sound == NULL)
        return NULL;

    return sound;
}

int Game::loadGameSFX()
{
    std::string s{};
    //s = "sfx/click3.wav";
    s = "sfx/bleep24Stereo.wav";
    click = loadSound(s.c_str());

    std::string s2{};
    s2 = "sfx/exitLow.wav";
    win = loadSound(s2.c_str());

    if (click == NULL || win == NULL)
        return -1;
}

void Game::loadGFX()
{
    quitButton.x = 400;
    quitButton.y = 150;
    quitButton.w = 200;
    quitButton.h = 100;

    reRackButton.x = 100;
    reRackButton.y = 150;
    reRackButton.w = 200;
    reRackButton.h = 100;

    timer.x = 100;
    timer.y = 350;
    timer.w = 500;
    timer.h = 30;

    timeRemaining.x = 100;
    timeRemaining.y = 350;
    timeRemaining.w = 1;
    timeRemaining.h = 30;

    scoreOnePos.x = (sw / 2) - 64;
    scoreOnePos.y = 0;
    scoreOnePos.w = 128;
    scoreOnePos.h = 128;

    spareButton.x = 350;
    spareButton.y = (sh / 2) + 300;
    spareButton.w = 150;
    spareButton.h = 150;

    sparesPos.x = 200;
    sparesPos.y = (sh / 2) + 300;
    sparesPos.w = BALLSIZE;
    sparesPos.h = BALLSIZE;

    background.x = 0;
    background.y = 0;
    background.w = sw;
    background.h = sh;
}

int Game::init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 4096) == -1)
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
    //startTime = time(0);

    return 0;
}

void Game::quit()
{
    clearValues();

    Mix_FreeChunk(click);
    Mix_FreeMusic(music1);
    Mix_FreeMusic(music2);
    Mix_FreeMusic(music3);
    click = NULL;
    music1 = NULL;
    music2 = NULL;
    music3 = NULL;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = NULL;
    window = NULL;

    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Game::chooseSong()
{
    if (Mix_PlayingMusic() == 0)
    {
        switch (songNo)
        {
        case 0:
        {
            Mix_PlayMusic(music1, 0);
            ++songNo;
            break;
        }
        case 1:
        {
            Mix_PlayMusic(music1, 0);
            songNo = 0;
            break;
        }
        }
    }
}

void Game::beginBoard()
{
    Mix_PlayChannel(-1, win, 0);

    clearValues();
    loadGFX();
    setupTable();
    updateScreen();

    startTime = time(0);

    Mix_PlayMusic(music1, 0);
}

void Game::clearValues()
{
    if (table.backgroundimg != NULL)
    {
        SDL_DestroyTexture(table.backgroundimg);
        table.backgroundimg = NULL;
    }

    if (table.reRackimg != NULL)
    {
        SDL_DestroyTexture(table.reRackimg);
        table.reRackimg = NULL;
    }

    if (table.quitimg != NULL)
    {
        SDL_DestroyTexture(table.quitimg);
        table.quitimg = NULL;
    }

    if (table.turnballimg != NULL)
    {
        SDL_DestroyTexture(table.turnballimg);
        table.turnballimg = NULL;
    }

    for (int i = 0; i < table.balls.size(); ++i)
    {
        if (table.balls[i].texture != NULL)
        {
            SDL_DestroyTexture(table.balls[i].texture);
            table.balls[i].texture = NULL;
        }
    }

    for (int i = 0; i < table.spares.size(); ++i)
    {
        if (table.spares[i].texture != NULL)
        {
            SDL_DestroyTexture(table.spares[i].texture);
            table.spares[i].texture = NULL;
        }
    }

    for (int i = 0; i < table.fonts.size(); ++i)
    {
        if (table.fonts[i].fontTexture != NULL)
        {
            SDL_DestroyTexture(table.fonts[i].fontTexture);
            table.fonts[i].fontTexture = NULL;
        }
    }

    table.balls.clear();
    table.spares.clear();
    table.fonts.clear();
    totalSum.clear();
    locations.clear();

    // reset figures
    currentSpare = 0;
    sparesLeft = SPARESMAXNUM;
}

void Game::createEmpty(int i)
{
    table.balls[i].w = 0;
    table.balls[i].h = 0;
    table.balls[i].show = false;
    SDL_DestroyTexture(table.balls[i].texture);
    table.balls[i].texture = nullptr;
}

void Game::colourBalls()
{
    for (int i = 0; i < table.balls.size(); ++i)
    {
        // set
        int r = 100 + (rand() % 150);
        int g = 100 + (rand() % 150);
        int b = 100 + (rand() % 150);

        SDL_SetTextureColorMod(table.balls[i].texture, r, g, b);

        table.balls[i].r = r;
        table.balls[i].g = g;
        table.balls[i].b = b;
    }
}

void Game::colourSpares()
{
    for (int i = 0; i < SPARESMAXNUM; ++i)
    {
        int r = 100 + (rand() % 150);
        int g = 100 + (rand() % 150);
        int b = 100 + (rand() % 150);

        SDL_SetTextureColorMod(table.spares[i].texture, r, g, b);

        table.spares[i].r = r;
        table.spares[i].g = g;
        table.spares[i].b = b;
    }
}

void Game::updateUI()
{
    checkTime();
    // its a bit overkill to refresh the screen
    // every frame whereas before each
    // touch
    updateScreen();
}

bool Game::checkTableEmpty()
{
    for (int i = 0; i < table.balls.size(); ++i)
    {
        if (table.balls[i].show == true)
        {
            // ++score;
            //  return true;
            return false;
        }
    }
    ++score;
    return true;
}

void Game::highlight(int i)
{
    SDL_SetTextureColorMod(table.balls[i].texture, 200, 200, 255);
    table.balls[i].highlighted = true;

    Mix_PlayChannel(-1, click, 0);
}

void Game::deselect(int i)
{
    SDL_SetTextureColorMod(table.balls[i].texture, 255, 255, 255);
    table.balls[i].highlighted = false;
}

void Game::handleHighlight(int i)
{
    // not separate ifs
    if (!table.balls[i].highlighted)
    {
        highlight(i);
    }
    else
    {
        deselect(i);
    }
}

void Game::hightlightSpare(int spareNum)
{
    SDL_SetTextureColorMod(table.spares[spareNum].texture, 200, 255, 200);
    table.spares[spareNum].highlighted = true;
}

void Game::deselectSpare(int spareNum)
{
    SDL_SetTextureColorMod(table.spares[spareNum].texture, 255, 255, 255);
    table.spares[spareNum].highlighted = false;
}

void Game::handleSpareHighlight(int spareNum)
{
    if (!table.spares[spareNum].highlighted)
    {
        hightlightSpare(spareNum);
    }
    else
    {
        deselectSpare(spareNum);
    }
}

void Game::handleThirteen()
{
    // either OR spare/table touched last
    // and sum is 13
    if (totalThirteen())
    {
        // make sure we see the last highlight
        updateScreen();
        SDL_Delay(1000);

        if (table.spares[currentSpare].highlighted)
        {
            //delay
            table.spares.erase(table.spares.begin() + currentSpare);
            --sparesLeft;
        }
        /*
            if vector index is 0, erase 0
            and index 1 becomes 0
            thats why we see the next ball
            without ++currentSpare

            if vector size is 9 we erase 9
            vector.erase() weirdly kept the end spare
            
            give the program a nudge
        */
        if (currentSpare == sparesLeft)
            currentSpare = 0;

        // and table
        for (int x : locations)
        {
            createEmpty(x);
            // deselect
            handleHighlight(x);
        }
        arrangeTotal();
    }
    // check here?
    if (checkTableEmpty())
        beginBoard();
}

void Game::includeSpare()
{
    handleSpareHighlight(currentSpare);
    handleThirteen();
}

void Game::loopSpares()
{
    ++currentSpare;
    // lucky "no index error
    // will bump to 1

    // if prev was highlighted, not now
    if (table.spares[currentSpare - 1].highlighted)
    {
        // after incr deselect prev spare
        handleSpareHighlight(currentSpare - 1);
    }

    if (currentSpare >= sparesLeft)
    {
        currentSpare = 0;
    }

    /*
    a way to remove spares vector without
    out of range
    
    if(sparesLeft == 0)
    {
        currentSpare = -1;
    }
    */
}

void Game::setupTable()
{
    // bkgrd
    std::string s = "gfx/thirteen_backimg3.png";
    table.backgroundimg = loadImage(renderer, s.c_str());
    // reRack
    std::string r = "gfx/thirteenRack.png";
    table.reRackimg = loadImage(renderer, r.c_str());
    // quit
    std::string q = "gfx/thirteenQuit.png";
    table.quitimg = loadImage(renderer, q.c_str());
    // loop spares
    std::string t = "gfx/turnball2.png";
    table.turnballimg = loadImage(renderer, t.c_str());

    //
    // positions
    //
    for (int i = 2; i > -1; --i)
    {
        int x = (sw / 2) + (120 * i) - 180;
        Ball temp;
        temp.x = x;
        temp.y = (sh / 2) - 180;
        table.balls.push_back(temp);
    }

    for (int i = 2; i > -2; --i)
    {
        int x = (sw / 2) + (120 * i) - 120;
        Ball temp;
        temp.x = x;
        temp.y = (sh / 2) - 120;
        table.balls.push_back(temp);
    }

    for (int i = 2; i > -3; --i)
    {
        int x = (sw / 2) + (120 * i) - 60;
        Ball temp;
        temp.x = x;
        temp.y = (sh / 2) - 60;
        table.balls.push_back(temp);
    }

    for (int i = 2; i > -4; --i)
    {
        int x = (sw / 2) + (120 * i);
        Ball temp;
        temp.x = x;
        temp.y = (sh / 2);
        table.balls.push_back(temp);
    }

    // randomly place numbers 1 to 13
    // into table
    // as we have their positions sorted above
    //
    // we need separate textures
    // for each ball

    for (int i = 0; i < table.balls.size(); ++i)
    {
        int randvalue = 1 + (rand() % 12);

        SDL_Texture *tempTex{};
        std::string s{};
        s += "gfx/ballB"; // prefix
        s += std::to_string(randvalue) + ".png";
        tempTex = loadImage(renderer, s.c_str());
        table.balls[i].value = randvalue;
        table.balls[i].texture = tempTex;
    }

    // create spare balls
    for (int i = 0; i < SPARESMAXNUM; ++i)
    {
        int randvalue = 1 + (rand() % 12);
        SDL_Texture *tempTex{};
        std::string s{};
        s += std::to_string(randvalue) + ".png";
        tempTex = loadImage(renderer, s.c_str());

        Ball tempBall{};
        tempBall.texture = tempTex;
        tempBall.value = randvalue;
        table.spares.push_back(tempBall);
    }

    //
    // load score digits 1...10
    //
    for (int i = 0; i < 4; ++i)
    {
        Font tempT{};
        std::string s{};
        s += "gfx/no";
        s += std::to_string(i);
        s += "font.png";
        tempT.fontTexture = loadImage(renderer, s.c_str());
        table.fonts.push_back(tempT);
    }

    // add some more variation
    /*
        colourSpares();
        colourBalls();
    */
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

    // total is refreshed every time
    if (table.spares[currentSpare].highlighted)
        total += table.spares[currentSpare].value;

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
            int loc = findLocation(i, locations);
            // did find something
            if (loc > -1)
            {
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
    handleThirteen();
}

void Game::checkTime()
{
    // a bit like Lerp
    double elapsed = (time(0) - startTime);
    double length = SONG18LENGTH;
    double left = length - elapsed;
    double bar = 500;

    bar *= (left / length);

    if (bar < 0)
        bar = 0;

    timeRemaining.w = bar;
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
                // create Rect from Ball
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

            if (SDL_HasIntersection(&touch, &spareButton))
            {
                loopSpares();
            }

            if (SDL_HasIntersection(&touch, &sparesPos))
            {
                includeSpare();
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

    // first background
    SDL_RenderCopy(renderer, table.backgroundimg, NULL, &background);

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

    // displayscore
    SDL_RenderCopy(renderer, table.fonts[score].fontTexture, NULL, &scoreOnePos);

    // display current spare
    SDL_RenderCopy(renderer, table.spares[currentSpare].texture, NULL, &sparesPos);
    //
    // quit button
    //
    SDL_RenderCopy(renderer, table.quitimg, NULL, &quitButton);
    //
    // play again button
    //
    SDL_RenderCopy(renderer, table.reRackimg, NULL, &reRackButton);
    //
    // timer max/min
    //
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderFillRect(renderer, &timer);
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
    SDL_RenderFillRect(renderer, &timeRemaining);
    //
    // next spare
    //
    SDL_RenderCopy(renderer, table.turnballimg, NULL, &spareButton);

    //
    // debug
    // conditions
    //
    if (debug)
        SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    Game game{};

    // prepare for game or fail
    if (game.init() != 0)
    {
        return -1;
    }
    /*
    no music at the moment
    
    else if (game.loadGameMusic() != 0)
    {
        return -1;
    }
    else if (game.loadGameSFX() != 0)
    {
        return -1;
    }
    */
    
    // it worked
    else
    {
        game.beginBoard();
    }

    // pre update
    // if no score
    // game.updateScreen();

    while (!game.isQuit)
    {
        game.checkInput();
        game.updateUI();
    }

    game.quit();
    return 0;
}
