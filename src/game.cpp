#include <iostream>
#include<fstream>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>



#define F first
#define S second
#define HEIGHT 720
#define WIDTH 720
#define PANEL_WIDTH 120
#define N 30
#define Gray Color{128, 128, 128, 255}
#define White Color({255, 255, 255, 255})
#define Yellow Color({255, 255, 0, 255})

#ifdef main
#undef main
#endif

using namespace std;

struct Color{
    int r;
    int g;
    int b;
    int a;
};

class Button{
private:
    pair<int, int> pos;
    pair<int, int> size;
    Color color; 
    SDL_Rect rect; 
public:
    Button(){
        size = {0, 0};
    }
    Button(pair<int, int> _pos, pair<int, int> _size, Color _color){
        pos = _pos;
        size = _size;
        color = _color;
        rect = {pos.F, pos.S, size.F, size.S}; 
    }
    
    void draw(SDL_Renderer *renderer){
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    bool is_Clicked(pair<int, int> _pos){
        if (_pos.F > pos.F && _pos.F < (pos.F + size.F) && _pos.S > pos.S && _pos.S < pos.S + size.S)
            return true;
        else
            return false;
    }

    void set_color(Color c){
        color = c;
    }
};

class Display{
private:
    SDL_Window *window;
    SDL_Renderer* rend;
    SDL_Event event;
    int cnt;
    int speed;
    bool board[N][N], on_off;
    Button *button_board[N][N];
    TTF_Font * font;
    SDL_Color text_color;
    SDL_Surface *image_sur1, *image_sur2, *image_sur3, *image_sur4, *image_sur5, *image_sur6, *image_sur7, *image_sur8, *image_sur9;
    Button next, start_pause, clear, speed_1, speed_2, speed_3, save, load, input;

public:
    Display(){
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
            printf("Error initializing SDL: %s\n", SDL_GetError());
            return;
        }
        window = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH + PANEL_WIDTH, HEIGHT, 0);
        if (!window){
            printf("Error creating window: %s\n", SDL_GetError());
            SDL_Quit();
            return;
        }
        Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
        rend = SDL_CreateRenderer(window, -1, render_flags);
        if (!rend){
            printf("Error creating renderer: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return;
        }

        if (TTF_Init() < 0 ) {
            cout << "Error initializing SDL_ttf: " << TTF_GetError() << endl;
            SDL_Quit();
            return;
        }

        if (IMG_Init(IMG_INIT_PNG) == 0) {
            std::cout << "Error SDL2_image Initialization";
            return;
        }

        for (int i = 0; i < N; i++){
            for (int j = 0; j < N; j++){
                button_board[i][j] = new Button({i * (WIDTH / N), j * (HEIGHT / N)}, {WIDTH / N, HEIGHT / N}, Gray);
                board[i][j] = false;
            }
        }
        next = Button({WIDTH + 10, 10}, {100, 100}, Color{0, 0, 0, 0});
        start_pause = Button({WIDTH + 10, 130}, {100, 100}, Color{0, 0, 0, 0});
        clear = Button({WIDTH + 10, 250}, {100, 100}, Color{0, 0, 0, 0});
        speed_1 = Button({WIDTH + 12, 360}, {30, 30}, Color{0, 0, 0, 0});
        speed_2 = Button({WIDTH + 46, 360}, {30, 30}, Color{0, 0, 0, 0});
        speed_3 = Button({WIDTH + 80, 360}, {30, 30}, Color{0, 0, 0, 0});
        save = Button({WIDTH + 25, 405}, {75, 75}, Color{0, 0, 0, 0});
        load = Button({WIDTH + 25, 485}, {75, 75}, Color{0, 0, 0, 0});
        input = Button({WIDTH + 29, 570}, {75, 75}, Color{0, 0, 0, 0});

        on_off = false;
        speed = 1;

        
        //loading font
        font = TTF_OpenFont("Arial.ttf", 25);
        text_color = SDL_Color{0, 0, 0};
        cnt = 0;

        //loading image
        image_sur1 = IMG_Load("icons/next.png");
        image_sur2 = IMG_Load("icons/start.png");
        image_sur3 = IMG_Load("icons/clear.png");
        image_sur4 = IMG_Load("icons/speed_1.png");
        image_sur5 = IMG_Load("icons/speed_2.png");
        image_sur6 = IMG_Load("icons/speed_3.png");
        image_sur7 = IMG_Load("icons/save.png");
        image_sur8 = IMG_Load("icons/load.png");
        image_sur9 = IMG_Load("icons/input.png");


    }

    ~Display(){
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                delete button_board[i][j];
            }
        }
    }

    void draw(){

        //rendering buttons
        for (int i = 0; i < N; i++){
            for (int j = 0; j < N; j++){
                button_board[i][j]->draw(rend);
            }
        }
        
        //rendering lines
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        for (int i = 0; i <= N; i++){
            SDL_RenderDrawLine(rend, i * (WIDTH / N), 0, i * (WIDTH / N), HEIGHT);
            SDL_RenderDrawLine(rend, 0, i * (HEIGHT / N), WIDTH, i * (HEIGHT / N));
        }

        //rendering images
        SDL_Texture* image_tex = SDL_CreateTextureFromSurface(rend, image_sur1);
        int imgW = 0;
        int imgH = 0;
        SDL_QueryTexture(image_tex, NULL, NULL, &imgW, &imgH);
        SDL_Rect img_rect = {WIDTH + 10, 10, imgW, imgH};
        SDL_RenderCopy(rend, image_tex, NULL, &img_rect);

        image_tex = SDL_CreateTextureFromSurface(rend, image_sur2);
        imgW = 0;
        imgH = 0;
        SDL_QueryTexture(image_tex, NULL, NULL, &imgW, &imgH);
        img_rect = {WIDTH + 10, 130, imgW, imgH};
        SDL_RenderCopy(rend, image_tex, NULL, &img_rect);

        image_tex = SDL_CreateTextureFromSurface(rend, image_sur3);
        imgW = 0;
        imgH = 0;
        SDL_QueryTexture(image_tex, NULL, NULL, &imgW, &imgH);
        img_rect = {WIDTH + 10, 250, imgW, imgH};
        SDL_RenderCopy(rend, image_tex, NULL, &img_rect);

        image_tex = SDL_CreateTextureFromSurface(rend, image_sur4);
        imgW = 0;
        imgH = 0;
        SDL_QueryTexture(image_tex, NULL, NULL, &imgW, &imgH);
        img_rect = {WIDTH + 12, 360, imgW, imgH};
        SDL_RenderCopy(rend, image_tex, NULL, &img_rect);

        image_tex = SDL_CreateTextureFromSurface(rend, image_sur5);
        imgW = 0;
        imgH = 0;
        SDL_QueryTexture(image_tex, NULL, NULL, &imgW, &imgH);
        img_rect = {WIDTH + 46, 360, imgW, imgH};
        SDL_RenderCopy(rend, image_tex, NULL, &img_rect);

        image_tex = SDL_CreateTextureFromSurface(rend, image_sur6);
        imgW = 0;
        imgH = 0;
        SDL_QueryTexture(image_tex, NULL, NULL, &imgW, &imgH);
        img_rect = {WIDTH + 80, 360, imgW, imgH};
        SDL_RenderCopy(rend, image_tex, NULL, &img_rect);

        image_tex = SDL_CreateTextureFromSurface(rend, image_sur7);
        imgW = 0;
        imgH = 0;
        SDL_QueryTexture(image_tex, NULL, NULL, &imgW, &imgH);
        img_rect = {WIDTH + 25, 405, imgW, imgH};
        SDL_RenderCopy(rend, image_tex, NULL, &img_rect);

        image_tex = SDL_CreateTextureFromSurface(rend, image_sur8);
        imgW = 0;
        imgH = 0;
        SDL_QueryTexture(image_tex, NULL, NULL, &imgW, &imgH);
        img_rect = {WIDTH + 25, 485, imgW, imgH};
        SDL_RenderCopy(rend, image_tex, NULL, &img_rect);

        image_tex = SDL_CreateTextureFromSurface(rend, image_sur9);
        imgW = 0;
        imgH = 0;
        SDL_QueryTexture(image_tex, NULL, NULL, &imgW, &imgH);
        img_rect = {WIDTH + 29, 570, imgW, imgH};
        SDL_RenderCopy(rend, image_tex, NULL, &img_rect);




        //rendering text
        string s = "Step :";
        SDL_Surface * surface = TTF_RenderText_Solid(font, s.c_str(), text_color);
        SDL_Texture * texture = SDL_CreateTextureFromSurface(rend, surface);
        int texW = 0;
        int texH = 0;
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        SDL_Rect dstrect = {WIDTH + PANEL_WIDTH / 5, 640, texW, texH };
        SDL_RenderCopy(rend, texture, NULL, &dstrect);

        surface = TTF_RenderText_Solid(font, to_string(cnt).c_str(), text_color);
        texture = SDL_CreateTextureFromSurface(rend, surface);
        texW = 0;
        texH = 0;
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        dstrect = {WIDTH + PANEL_WIDTH / 3, 680, texW, texH };
        SDL_RenderCopy(rend, texture, NULL, &dstrect);
    }

    void run(){
        bool running = true;
        bool lock = false;
        int Counter = 0;
        while(running){
            if(on_off&& (Counter ++) % (20 / speed) == 0){Next();}
            while(SDL_PollEvent(&event)){
                if (event.type == SDL_QUIT){
                    running = false;
                    SDL_Quit();
                    break;
                }
                if (event.type == SDL_MOUSEBUTTONDOWN && !lock){
                    lock = true;
                    handle_click(event.motion.x, event.motion.y);
                }
                if (event.type == SDL_MOUSEBUTTONUP && lock)
                    lock = false;
            }
            SDL_SetRenderDrawColor(rend, 0, 0, 255, 255);
            SDL_RenderClear(rend);
            draw();
            SDL_RenderPresent(rend);
        }
    }

    void handle_click(int x, int y){
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (button_board[i][j]->is_Clicked({x, y}))
                    if(board[i][j]){
                        board[i][j] = false;
                        button_board[i][j]->set_color(Gray);
                        cnt = 0;
                    }
                    else{
                        board[i][j] = true;
                        button_board[i][j]->set_color(Yellow);
                        cnt = 0;
                    }
        if (next.is_Clicked({x, y})){
            Next();
        }
        if (start_pause.is_Clicked({x, y})){
            on_off = !on_off;
        }
        if (clear.is_Clicked({x, y})){
            for(int i = 0; i < N; i++){
                for(int j = 0; j < N; j++){
                    board[i][j] = false;
                    button_board[i][j]->set_color(Gray);
                }
            }
            cnt = 0;
            on_off = false;
        }
        if (speed_1.is_Clicked({x, y})){speed = 1;}
        if (speed_2.is_Clicked({x, y})){speed = 2;}
        if (speed_3.is_Clicked({x, y})){speed = 3;}
        if (save.is_Clicked({x, y})){Save();}
        if (load.is_Clicked({x, y})){Load();}
        if (input.is_Clicked({x, y})){Input();}

    }

    void Next(){
        cnt++;
        int adjacient[N][N];
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                adjacient[i][j] = -board[i][j];
                for(int i2 = max(0, i - 1); i2 <= min(N - 1, i + 1); i2++){
                    for(int j2 = max(0, j - 1); j2 <= min(N - 1, j + 1); j2++){
                        adjacient[i][j] += board[i2][j2];
                    }
                }
            }
        }

        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                if(board[i][j]){
                    if(adjacient[i][j] <= 1|| adjacient[i][j] >= 4)
                    {
                        board[i][j] = false;
                        button_board[i][j]->set_color(Gray);
                    }
                }
                else{
                    if(adjacient[i][j] == 3){
                        board[i][j] = true;
                        button_board[i][j]->set_color(Yellow);
                    }
                }
            }
        }
    }

    void Save(){
        cout << "Enter the file name : \n";
        string s;
        cin >> s;

        ofstream o_f("saved/" + s + ".txt");
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                o_f << board[j][i] << " ";
            }
            o_f << '\n';
        }

        cout << "Saved successfully!\n";
    }

    void Load(){
        cout << "Enter the file name : \n";
        string s;
        cin >> s;

        ifstream i_f("saved/" + s + ".txt");

        if(!i_f.is_open()){
            cout << "There is no file with this name !!!\n";
            return ;
        }

        on_off = false;
        cnt = 0;
        
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                i_f >> board[j][i];
                if(board[j][i]){button_board[j][i]->set_color(Yellow);}
                else{button_board[j][i]->set_color(Gray);}
            }
        }

        cout << "Loaded successfully!\n";
    }

    void Input(){
        int n;
        cout << "Enter N : ";
        cin >> n;
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                board[i][j] = 0;
                button_board[i][j]->set_color(Gray);
            }
        }
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                cin >> board[j][i];
                if(board[j][i]){button_board[j][i]->set_color(Yellow);}
            }
        }
        cout << "Loaded successfully!\n";
    }

};


int main(int argc, char* argv[]){
    Display screen;
    screen.run();
    return 0;
}