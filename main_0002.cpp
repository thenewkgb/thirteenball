#include <sdl2/sdl.h>
#include <sdl2/sdl_image.h>
#include <vector>
#include <iostream>
#include <stdexcept>

#define ballsize 120
#define rowonesize 3
#define rowtwosize 4
#define rowthreesize 5
#define rowfoursize 6

struct ball : sdl_rect
{
    int x = 0;
    int y = 0;
    int w = ballsize;
    int h = ballsize;
    int value;
    bool show = true;
    bool highlighted = false;
    sdl_texture *texture;
};

struct emptyball : sdl_rect
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    int value = 0;
};

struct table
{
    std::vector<ball> balls;
};

class game
{
  public:
    game()
    {
    }

    sdl_renderer *renderer;
    sdl_window *window;
    sdl_event e{};
    sdl_touchfingerevent f;

    sdl_rect quitbutton{};
    sdl_rect rerackbutton{};
    sdl_rect touch{};

    // some miscellanious vars
    int sw, sh;
    bool isquit{};
    std::vector<int> totalsum{};
    table table{};
    std::vector<int> locations{};

    int init();
    void setuptable();
    void loadgfx();

    bool canclick(int i);
    void highlight(int i);
    void deselect(int i);
    void handlehighlight(int i);
    void createempty(int i);
    void touchingball(int i);

    void arrangetotal();
    void replaceempty();
    int findlocation(int value, std::vector<int> v);

    bool totalthirteen();

    void clearvalues();
    void beginboard();
    void updatescreen();
    void checkinput();

    void quit();
};

// function for loading an image to sdl_texture
static sdl_texture *loadimage(sdl_renderer *renderer, const char *path)
{
    sdl_surface *img = img_load(path);
    if (img == null)
    {
        fprintf(stderr, "img_load error: %s\n", img_geterror());
        return null;
    }
    sdl_texture *texture = sdl_createtexturefromsurface(renderer, img);
    sdl_freesurface(img);
    if (texture == null)
    {
        fprintf(stderr, "sdl_createtexturefromsurface error: %s\n", sdl_geterror());
        return null;
    }
    return texture;
}

int game::init()
{
    if (sdl_init(sdl_init_everything) != 0)
    {
        return -1;
    }

    window = sdl_createwindow("hello sdl2", sdl_windowpos_undefined, sdl_windowpos_undefined, 640, 480, 0);
    if (window == null)
    {
        return -1;
    }

    renderer = sdl_createrenderer(window, -1, 0);

    sdl_getrendereroutputsize(renderer, &sw, &sh);

    srand(time(0));

    return 0;
}

void game::quit()
{
    sdl_destroyrenderer(renderer);
    sdl_destroywindow(window);
    sdl_quit();
}

void game::beginboard()
{
    clearvalues();
    loadgfx();
    setuptable();
    updatescreen();
}

void game::clearvalues()
{
    for(int i = 0;i < table.balls.size();++i)
        sdl_destroytexture(table.balls[i].texture);
    
    table.balls.clear();
    totalsum.clear();
    locations.clear();
}

void game::createempty(int i)
{
    table.balls[i].w = 0;
    table.balls[i].h = 0;
    table.balls[i].show = false;
    sdl_destroytexture(table.balls[i].texture);
    table.balls[i].texture = nullptr;
    // highlighted flips at sum of 13
    //table.balls[i].highlighted = false;
}

void game::loadgfx()
{
    // quit stuff
    quitbutton.x = 400;
    quitbutton.y = 150;
    quitbutton.w = 200;
    quitbutton.h = 100;

    rerackbutton.x = 100;
    rerackbutton.y = 150;
    rerackbutton.w = 200;
    rerackbutton.h = 100;
}

void game::highlight(int i)
{
    sdl_settexturecolormod(table.balls[i].texture, 200, 200, 255);
    table.balls[i].highlighted = true;
}

void game::deselect(int i)
{
    sdl_settexturecolormod(table.balls[i].texture, 255, 255, 255);
    table.balls[i].highlighted = false;
}

void game::handlehighlight(int i)
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

void game::setuptable()
{
    // positions
    for (int i = 2; i > -1; --i)
    {
        int x = (sw / 2) + (120 * i) - 180;
        ball temp;
        temp.x = x;
        temp.y = (sh / 2) - 180;
        //table.rowofthree.push_back(temp);
        table.balls.push_back(temp);
    }

    for (int i = 2; i > -2; --i)
    {
        int x = (sw / 2) + (120 * i) - 120;
        ball temp;
        temp.x = x;
        temp.y = (sh / 2) - 120;
        table.balls.push_back(temp);
        //table.rowoffour.push_back(temp);
    }

    for (int i = 2; i > -3; --i)
    {
        int x = (sw / 2) + (120 * i) - 60;
        ball temp;
        temp.x = x;
        temp.y = (sh / 2) - 60;
        table.balls.push_back(temp);
        //table.rowoffive.push_back(temp);
    }

    for (int i = 2; i > -4; --i)
    {
        int x = (sw / 2) + (120 * i);
        ball temp;
        temp.x = x;
        temp.y = (sh / 2);
        table.balls.push_back(temp);
        //table.rowofsix.push_back(temp);
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
        sdl_texture *temptex{};
        std::string s{};
        s += std::to_string(randvalue) + ".png";
        temptex = loadimage(renderer, s.c_str());
        // give ball a value
        table.balls[i].value = randvalue;
        // and corresponding texture
        table.balls[i].texture = temptex;
    }
}

bool game::canclick(int i)
{
    // i >= 0 && i < 3
    if (i >= 0 && i < rowonesize)
    {
        if (table.balls[i + rowonesize].show == false &&
            table.balls[i + rowonesize + 1].show == false)
        {
            return true;
        }
    }

    // i >= 3 && i < 7
    else if (i >= rowonesize && i < rowtwosize + rowonesize)
    {
        if (table.balls[i + rowtwosize].show == false && table.balls[i + rowtwosize + 1].show == false)
        {
            return true;
        }
    }

    // i >= 7 && i < 12
    else if (i >= rowonesize + rowtwosize && i < rowthreesize + rowtwosize + rowonesize)
    {
        if (table.balls[i + rowthreesize].show == false && table.balls[i + rowthreesize + 1].show == false)
        {
            return true;
        }
    }
    else if (i >= rowthreesize + rowtwosize + rowonesize && i < rowfoursize + rowthreesize + rowtwosize + rowonesize)
    {
        return true;
    }

    else
    {
        return false;
    }
}

bool game::totalthirteen()
{
    int total = 0;

    for (int t : totalsum)
        total += t;

    if (total == 13)
    {
        return true;
    }

    return false;
}

int game::findlocation(int value, std::vector<int> v)
{
    // search vector for same number
    for (int i = 0; i < v.size(); ++i)
    {
        if (value == v[i])
            return i;
    }
    return -1;
}

void game::arrangetotal()
{
    for (int i = 0; i < table.balls.size(); ++i)
    {
        // do not push if ball location already present
        if (table.balls[i].highlighted)
        {
           // int foundvalueat = findlocation(table.balls[i].value, totalsum);
            int loc = findlocation(i, locations);

            // if there is no location
            if (loc == -1)
            {
                totalsum.push_back(table.balls[i].value);
                locations.push_back(i);
            }
        }

        if (!table.balls[i].highlighted)
        {
            // remove value from totalsum
            //int foundvalueat = findlocation(table.balls[i].value, totalsum);
            int loc = findlocation(i, locations);

            // did find something
            if (loc > -1)
            {
                //totalsum.erase(totalsum.begin() + foundvalueat);
                totalsum.erase(totalsum.begin() + loc);
                locations.erase(locations.begin() + loc);
            }
        }
    }
}

void game::touchingball(int i)
{
    if (canclick(i))
    {
        handlehighlight(i);
        arrangetotal();
    }

    if (totalthirteen())
    {
        for (int x : locations)
        {
            createempty(x);
            handlehighlight(x);
            //totalsum.clear();
            //locations.clear();
        }
        arrangetotal();
    }
}

void game::checkinput()
{
    while (sdl_pollevent(&e) != 0)
    {
        //user touches screen
        if (e.type == sdl_fingerup)
        {
            f = e.tfinger;
            touch.x = f.x * sw;
            touch.y = f.y * sh;
            touch.w = 1;
            touch.h = 1;

            for (int i = 0; i < table.balls.size(); ++i)
            {
                sdl_rect b{};
                b.x = table.balls[i].x;
                b.y = table.balls[i].y;
                b.w = table.balls[i].w;
                b.h = table.balls[i].h;

                if (sdl_hasintersection(&touch, &b))
                {
                    touchingball(i);
                }
            }

            if (sdl_hasintersection(&touch, &rerackbutton))
            {
                beginboard();
            }

            if (sdl_hasintersection(&touch, &quitbutton))
            {
                isquit = true;
            }
        }
    }
}

void game::updatescreen()
{
    sdl_setrenderdrawcolor(renderer, 0, 0, 0, 255);
    sdl_renderclear(renderer);

    // pop a load of balls on screen
    for (int i = 0; i < table.balls.size(); ++i)
    {
        // convert ball to rect
        // rendercopy needs rect
        sdl_rect dest;
        dest.x = table.balls[i].x;
        dest.y = table.balls[i].y;
        dest.w = table.balls[i].w;
        dest.h = table.balls[i].h;

        if (table.balls[i].show == true)
        {
            sdl_rendercopy(renderer, table.balls[i].texture, null, &dest);
        }
    }
    // quit button
    sdl_setrenderdrawcolor(renderer, 255, 0, 0, 255);
    sdl_renderfillrect(renderer, &quitbutton);
    // play again button
    sdl_setrenderdrawcolor(renderer, 255, 255, 0, 255);
    sdl_renderfillrect(renderer, &rerackbutton);
    // display
    sdl_renderpresent(renderer);
}

/*
each rerack loads textures but does not
free them
the program slows down consideribly
after about 10 plays
*/

int main(int argc, char *argv[])
{
    game game{};

    if (game.init() != 0)
    {
        return -1;
    }
    else
    {
        game.beginboard();
    }

    while (!game.isquit)
    {
        game.checkinput();
        game.updatescreen();
    }

    game.quit();
    return 0;
}
