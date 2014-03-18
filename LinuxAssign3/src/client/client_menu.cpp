#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 768;
const int SCREEN_BPP = 32;

SDL_Surface *background = NULL;
SDL_Surface *textbox = NULL;
SDL_Surface *message = NULL;
SDL_Surface *name_label = NULL;
SDL_Surface *ip_label = NULL;
SDL_Surface *port_label = NULL;
SDL_Surface *button;
SDL_Surface *screen = NULL;
SDL_Event event;
TTF_Font *font = NULL;
SDL_Color textColor = { 0xFF, 0xFF, 0xFF };
int x;  // mouse position x
int y;  // mouse position y
bool namebox_clicked = false;
bool ipbox_clicked = false;
bool portnumber_clicked = false;
bool namebox_filled = false;
bool ipbox_filled = false;
bool portnumber_filled = false;

//The key press interpreter
class string_input{

    private:
    //The storage string
    std::string str1;
    std::string str2;
    std::string str3;

    //The text surface
    SDL_Surface *text1;
    SDL_Surface *text2;
    SDL_Surface *text3;

    public:
    // constructor
    string_input();

    // deconstructor
    ~string_input();

    // handles input
    void handle_input();

    // displays the message on screen
    void display(SDL_Surface*);
};

SDL_Surface *load_image(std::string filename){
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;

    // load the image
    loadedImage = IMG_Load(filename.c_str());

    // if image load is successful
    if(loadedImage != NULL){
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat(loadedImage);

        // free the old surface
        SDL_FreeSurface(loadedImage);

        // if the surface was optimized
        if(optimizedImage != NULL){
            //Color key surface
            SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB(optimizedImage->format, 0, 0xFF, 0xFF));
        }
    }

    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    // initialize all SDL subsystems
    if(SDL_Init( SDL_INIT_EVERYTHING ) == -1){
        return false;
    }

    // set up the screen
    if((screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE)) == NULL){
    	return false;
    }

    //Initialize SDL_ttf
    if(TTF_Init() == -1){
        return false;
    }

    // set caption
    SDL_WM_SetCaption( "High Score", NULL );

    return true;
}

bool load_background()
{
    // load background image
    if((background = load_image("../assets/background.png")) == NULL){
    	return false;
    }

    // open font
    if((font = TTF_OpenFont("../assets/sixty.ttf", 42 )) == NULL){
        return false;
    }

    return true;
}

bool load_textbox()
{
    // load a textbox
    if((textbox = load_image("../assets/textfield.png")) == NULL){
        return false;
    }

    return true;
}

void clean_up(){

    SDL_FreeSurface(background);
    SDL_FreeSurface(message);

    TTF_CloseFont(font);

    // close ttf
    TTF_Quit();

    // close SDL
    SDL_Quit();
}

string_input::string_input(){
    str1 = "";
    str2 = "";
    str3 = "";

    text1 = NULL;
    text2 = NULL;
    text3 = NULL;

    SDL_EnableUNICODE(SDL_ENABLE);
}

string_input::~string_input(){
    SDL_FreeSurface(text1);
    SDL_FreeSurface(text2);
    SDL_FreeSurface(text3);

    SDL_EnableUNICODE( SDL_DISABLE );
}

void string_input::handle_input(){

    // namebox is clicked
    if(event.type == SDL_MOUSEBUTTONDOWN){
        if(event.button.button == SDL_BUTTON_LEFT){
            x = event.button.x;
            y = event.button.y;

            // if namebox is clicked
            if((x > (SCREEN_WIDTH - textbox->w) / 2 + 200)
                && (x < (SCREEN_WIDTH - textbox->w) / 2 + 200 + textbox->w)
                && (y > ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 100)
                && (y < ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 100 + textbox->h)){
                
                namebox_clicked = true;
                ipbox_clicked = false;
                portnumber_clicked = false;
                //show_menu = true;
            }

            // if ipbox is clicked
            if((x > (SCREEN_WIDTH - textbox->w) / 2 + 200)
                && (x < (SCREEN_WIDTH - textbox->w) / 2 + 200 + textbox->w)
                && (y > ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 180)
                && (y < ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 180 + textbox->h)){
                
                namebox_clicked = false;
                ipbox_clicked = true;
                portnumber_clicked = false;
                //show_menu = true;
            }

            // if portnumber is clicked
            if((x > (SCREEN_WIDTH - textbox->w) / 2 + 200)
                && (x < (SCREEN_WIDTH - textbox->w) / 2 + 200 + textbox->w)
                && (y > ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 260)
                && (y < ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 260 + textbox->h)){
                
                namebox_clicked = false;
                ipbox_clicked = false;
                portnumber_clicked = true;
                //show_menu = true;
            }
        }
    }

    // if a key was pressed
    if(event.type == SDL_KEYDOWN){
        std::string temp1 = str1;
        std::string temp2 = str2;
        std::string temp3 = str3;


        if(namebox_clicked){
            // if the string less than maximum size
            if(str1.length() <= 16 ){
                
                //If the key is a number
                if((event.key.keysym.unicode >= (Uint16)'0' ) && ( event.key.keysym.unicode <= (Uint16)'9')){
                    str1 += (char)event.key.keysym.unicode;
                    namebox_filled = true;
                }

                //If the key is a uppercase letter
                else if((event.key.keysym.unicode >= (Uint16)'A' ) && ( event.key.keysym.unicode <= (Uint16)'Z')){
                    str1 += (char)event.key.keysym.unicode;
                    namebox_filled = true;
                }

                //If the key is a lowercase letter
                else if((event.key.keysym.unicode >= (Uint16)'a' ) && ( event.key.keysym.unicode <= (Uint16)'z')){
                    str1 += (char)event.key.keysym.unicode;
                    namebox_filled = true;
                }
            }

            if(str1.length() == 0){
                namebox_filled = false;
            }
        }

        if(ipbox_clicked){
            // if the string less than maximum size
            if(str2.length() <= 14 ){

                //If the key is a space
                if(event.key.keysym.unicode == (Uint16)'.'){
                    str2 += (char)event.key.keysym.unicode;
                    ipbox_filled = true;
                }
                //If the key is a number
                else if((event.key.keysym.unicode >= (Uint16)'0' ) && ( event.key.keysym.unicode <= (Uint16)'9')){
                    str2 += (char)event.key.keysym.unicode;
                    ipbox_filled = true;
                }
            }

            if(str2.length() == 0){
                ipbox_filled = false;
            }
        }

        if(portnumber_clicked){
            // if the string less than maximum size
            if(str3.length() <= 4){

                //If the key is a number
                if((event.key.keysym.unicode >= (Uint16)'0' ) && ( event.key.keysym.unicode <= (Uint16)'9')){
                    str3 += (char)event.key.keysym.unicode;
                    portnumber_filled = true;
                }
            }

            if(str3.length() == 0){
                portnumber_filled = false;
            }
        }
        

        //If backspace was pressed and the string isn't blank
        if((event.key.keysym.sym == SDLK_BACKSPACE) && namebox_clicked && (str1.length() != 0)){
            // erase the last character
            str1.erase(str1.length() - 1);
        }

        //If backspace was pressed and the string isn't blank
        if((event.key.keysym.sym == SDLK_BACKSPACE) && ipbox_clicked && (str2.length() != 0)){
            // erase the last character
            str2.erase(str2.length() - 1);
        }

        //If backspace was pressed and the string isn't blank
        if((event.key.keysym.sym == SDLK_BACKSPACE) && portnumber_clicked && (str3.length() != 0)){
            // erase the last character
            str3.erase(str3.length() - 1);
        }

        // update old text surface
        if(str1 != temp1){
            // free the old surface
            SDL_FreeSurface(text1);

            // render new text surface
            textColor = { 0x00, 0x00, 0x00 };
            text1 = TTF_RenderText_Solid(font, str1.c_str(), textColor);
            textColor = { 0xFF, 0xFF, 0xFF };
        }

        // update old text surface
        if(str2 != temp2){
            // free the old surface
            SDL_FreeSurface(text2);

            // render new text surface
            textColor = { 0x00, 0x00, 0x00 };
            text2 = TTF_RenderText_Solid(font, str2.c_str(), textColor);
            textColor = { 0xFF, 0xFF, 0xFF };
        }

        // update old text surface
        if(str3 != temp3){
            // free the old surface
            SDL_FreeSurface(text3);

            // render new text surface
            textColor = { 0x00, 0x00, 0x00 };
            text3 = TTF_RenderText_Solid(font, str3.c_str(), textColor);
            textColor = { 0xFF, 0xFF, 0xFF };
        }
    }
}

void string_input::display(SDL_Surface *surf){
	// if text is not null
    if(text1 != NULL || text2 != NULL || text3 != NULL){

        //Show the name
        if(namebox_filled){
            apply_surface((SCREEN_WIDTH - text1->w) / 2 + 200, ((SCREEN_HEIGHT / 2) - text1->h) / 2 + 102, text1, screen);
        }

        //Show the ip
        if(ipbox_filled){
            apply_surface((SCREEN_WIDTH - text2->w) / 2 + 200, ((SCREEN_HEIGHT / 2) - text2->h) / 2 + 182, text2, screen);
        }

        //Show the port number
        if(portnumber_filled){
            apply_surface((SCREEN_WIDTH - text3->w) / 2 + 200, ((SCREEN_HEIGHT / 2) - text3->h) / 2 + 262, text3, screen);
        }
    }
}

int main(int argc, char* args[]){
    bool done = false;

    //Keep track if whether or not the user has entered their name
    bool ip_entered = false;

    // initialize SDL and ttl
    if(init() == false){
        return 1;
    }

    // IP address
    string_input input;

    // load background
    if(load_background() == false){
        return 1;
    }

    // load textbox
    if(load_textbox() == false){
        return 1;
    }

    // set the message
    message = TTF_RenderText_Solid(font, "Client", textColor);
    name_label = TTF_RenderText_Solid(font, "Your Name", textColor);
    ip_label = TTF_RenderText_Solid(font, "IP Address", textColor);
    port_label = TTF_RenderText_Solid(font, "Port Number", textColor);
    button = TTF_RenderText_Solid(font, "Connect", textColor);


    while(done == false){
        while(SDL_PollEvent(&event)){
            if(ip_entered == false){
                
                // get user input
                input.handle_input();

                // if enter key is pressed
                if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_RETURN)){
                    ip_entered = true;

                    SDL_FreeSurface(message);

                    textColor = { 0xFF, 0x00, 0x00 };
                    message = TTF_RenderText_Solid(font, "You entered an ip address", textColor);
                    textColor = { 0xFF, 0xFF, 0xFF };

                }
            }

            // if user closes the window
            if( event.type == SDL_QUIT ){
                done = true;
            }
        }

        // display the background
        apply_surface(0, 0, background, screen);
        
        // display the caption
        apply_surface((SCREEN_WIDTH - message->w) / 2, ((SCREEN_HEIGHT / 2) - message->h) / 2, message, screen);

        // display the label
        apply_surface((SCREEN_WIDTH - name_label->w) / 2 - 120, ((SCREEN_HEIGHT / 2) - name_label->h) / 2 + 100, name_label, screen);  
        apply_surface((SCREEN_WIDTH - ip_label->w) / 2 - 120, ((SCREEN_HEIGHT / 2) - ip_label->h) / 2 + 180, ip_label, screen);  
        apply_surface((SCREEN_WIDTH - port_label->w) / 2 - 120, ((SCREEN_HEIGHT / 2) - port_label->h) / 2 + 260, port_label, screen);  

        // display the textbox
        apply_surface((SCREEN_WIDTH - textbox->w) / 2 + 200, ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 100, textbox, screen);  
        apply_surface((SCREEN_WIDTH - textbox->w) / 2 + 200, ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 180, textbox, screen);  
        apply_surface((SCREEN_WIDTH - textbox->w) / 2 + 200, ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 260, textbox, screen);  

        // display the button
        apply_surface((SCREEN_WIDTH - button->w) / 2 + 100, ((SCREEN_HEIGHT / 2) - button->h) / 2 + 400, button, screen);  

        // display the ip address on the screen
        input.display(textbox);

        // update the screen
        if(SDL_Flip( screen ) == -1){
            return 1;
        }
    }

    clean_up();

    return 0;
}
