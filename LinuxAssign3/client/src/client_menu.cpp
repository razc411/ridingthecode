#include "client_defs.h"

SDL_Surface *button = NULL;
SDL_Surface *screen = NULL;
SDL_Event event;


SDL_Surface *background = NULL;
SDL_Surface *textbox = NULL;
SDL_Surface *chatbox = NULL;
SDL_Surface *inputbox = NULL;
SDL_Surface *clientbox = NULL;


TTF_Font *font = NULL;
SDL_Color textColor = { 0xFF, 0xFF, 0xFF };
int x = 0;  // mouse position x
int y = 0;  // mouse position y
bool namebox_clicked = false;
bool ipbox_clicked = false;
bool portbox_clicked = false;
bool button_clicked = false;

bool namebox_filled = false;
bool ipbox_filled = false;
bool portbox_filled = false;

//The key press interpreter
class client_utils{

    private:
    //The storage string
    std::string str;

    //The text surface
    SDL_Surface *text;

    public:
    // constructor
    client_utils();

    // deconstructor
    ~client_utils();

    // handles input
    void create_textbox();
    void create_ipbox();
    void create_portbox();
    void create_chatbox();

    // displays the message on screen
    void display(int, int);
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

    //BlitSurface
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
    SDL_WM_SetCaption("Chat Program", NULL);

    return true;
}

bool load_background()
{
    // load background image
    if((background = load_image("/home/timkim/Documents/ridingthecode/LinuxAssign3/assets/background.png")) == NULL){
    	return false;
    }

    // open font
    if((font = TTF_OpenFont("/home/timkim/Documents/ridingthecode/LinuxAssign3/assets/marlbo.ttf", 42 )) == NULL){
        return false;
    }

    return true;
}

bool load_textbox()
{
    // load a textbox
    if((textbox = load_image("/home/timkim/Documents/ridingthecode/LinuxAssign3/assets/textfield.png")) == NULL){
        return false;
    }

    return true;
}

bool load_chatbox()
{
    // load a chatbox
    if((chatbox = load_image("/home/timkim/Documents/ridingthecode/LinuxAssign3/assets/chatbox.png")) == NULL){
        return false;
    }

    return true;
}

bool load_inputbox()
{
    // load a inputbox
    if((inputbox = load_image("/home/timkim/Documents/ridingthecode/LinuxAssign3/assets/inputbox.png")) == NULL){
        return false;
    }

    return true;
}

bool load_clientbox()
{
    // load a inputbox
    if((clientbox = load_image("/home/timkim/Documents/ridingthecode/LinuxAssign3/assets/clientbox.png")) == NULL){
        return false;
    }

    return true;
}

client_utils::client_utils(){
    str = "";

    text = NULL;

    SDL_EnableUNICODE(SDL_ENABLE);
}

client_utils::~client_utils(){
    SDL_FreeSurface(text);

    SDL_EnableUNICODE( SDL_DISABLE );
}

void client_utils::create_textbox(){
    // if a key was pressed
    if(event.type == SDL_KEYDOWN){
        std::string temp = str;

        // if the string less than maximum size
        if(str.length() <= 16 ){
            
            //If the key is a number
            if((event.key.keysym.unicode >= (Uint16)'0') && (event.key.keysym.unicode <= (Uint16)'9')){
                str += (char)event.key.keysym.unicode;
                namebox_filled = true;
            }

            //If the key is a uppercase letter
            else if((event.key.keysym.unicode >= (Uint16)'A') && (event.key.keysym.unicode <= (Uint16)'Z')){
                str += (char)event.key.keysym.unicode;
                namebox_filled = true;
            }

            //If the key is a lowercase letter
            else if((event.key.keysym.unicode >= (Uint16)'a') && (event.key.keysym.unicode <= (Uint16)'z')){
                str += (char)event.key.keysym.unicode;
                namebox_filled = true;
            }
        }

        //If backspace was pressed and the string isn't blank
        if((event.key.keysym.sym == SDLK_BACKSPACE) && namebox_clicked && (str.length() != 0)){
            // erase the last character
            str.erase(str.length() - 1);

            if(str.length() == 0){
                namebox_filled = false;
            }
        }

        // update old text surface
        if(str != temp){
            // free the old surface
            SDL_FreeSurface(text);

            // render new text surface
            textColor = { 0x00, 0x00, 0x00 };
            text = TTF_RenderText_Solid(font, str.c_str(), textColor);
            textColor = { 0xFF, 0xFF, 0xFF };
        }
    }
}

void client_utils::create_ipbox(){
    // if a key was pressed
    if(event.type == SDL_KEYDOWN){
        std::string temp = str;

        // if the string less than maximum size
        if(str.length() <= 14 ){

            //If the key is a space
            if(event.key.keysym.unicode == (Uint16)'.'){
                str += (char)event.key.keysym.unicode;
                ipbox_filled = true;
            }
            //If the key is a number
            else if((event.key.keysym.unicode >= (Uint16)'0') && (event.key.keysym.unicode <= (Uint16)'9')){
                str += (char)event.key.keysym.unicode;
                ipbox_filled = true;
            }
        }

        //If backspace was pressed and the string isn't blank
        if((event.key.keysym.sym == SDLK_BACKSPACE) && ipbox_clicked && (str.length() != 0)){
            // erase the last character
            str.erase(str.length() - 1);

            if(str.length() == 0){
                ipbox_filled = false;
            }
        }

        // update old text surface
        if(str != temp){
            // free the old surface
            SDL_FreeSurface(text);

            // render new text surface
            textColor = { 0x00, 0x00, 0x00 };
            text = TTF_RenderText_Solid(font, str.c_str(), textColor);
            textColor = { 0xFF, 0xFF, 0xFF };
        }
    }
}

void client_utils::create_portbox(){
    // if a key was pressed
    if(event.type == SDL_KEYDOWN){
        std::string temp = str;

        // if the string less than maximum size
        if(str.length() <= 4){

            //If the key is a number
            if((event.key.keysym.unicode >= (Uint16)'0') && (event.key.keysym.unicode <= (Uint16)'9')){
                str += (char)event.key.keysym.unicode;
                portbox_filled = true;
            }
        }

        //If backspace was pressed and the string isn't blank
        if((event.key.keysym.sym == SDLK_BACKSPACE) && portbox_clicked && (str.length() != 0)){
            // erase the last character
            str.erase(str.length() - 1);

            if(str.length() == 0){
                portbox_filled = false;
            }
        }

        // update old text surface
        if(str != temp){
            // free the old surface
            SDL_FreeSurface(text);

            // render new text surface
            textColor = { 0x00, 0x00, 0x00 };
            text = TTF_RenderText_Solid(font, str.c_str(), textColor);
            textColor = { 0xFF, 0xFF, 0xFF };
        }
    }
}

void client_utils::create_chatbox(){
    // if a key was pressed
    if(event.type == SDL_KEYDOWN){
        std::string temp = str;

        // if the string less than maximum size
        if(str.length() <= 55 ){
            
            //If the key is a number
            if((event.key.keysym.unicode >= (Uint16)'0') && (event.key.keysym.unicode <= (Uint16)'9')){
                str += (char)event.key.keysym.unicode;
            }

            //If the key is a uppercase letter
            else if((event.key.keysym.unicode >= (Uint16)'A') && (event.key.keysym.unicode <= (Uint16)'Z')){
                str += (char)event.key.keysym.unicode;
            }

            //If the key is a lowercase letter
            else if((event.key.keysym.unicode >= (Uint16)'a') && (event.key.keysym.unicode <= (Uint16)'z')){
                str += (char)event.key.keysym.unicode;
            }
        }

        //If backspace was pressed and the string isn't blank
        if((event.key.keysym.sym == SDLK_BACKSPACE) && (str.length() != 0)){
            // erase the last character
            str.erase(str.length() - 1);
        }

        // update old text surface
        if(str != temp){
            // free the old surface
            SDL_FreeSurface(text);

            // render new text surface
            textColor = { 0x00, 0x00, 0x00 };
            text = TTF_RenderText_Solid(font, str.c_str(), textColor);
            textColor = { 0xFF, 0xFF, 0xFF };
        }
    }
}

void client_utils::display(int x, int y){
    // if surface is not null
    if(text != NULL){
        apply_surface((SCREEN_WIDTH - text->w) / 2 + x,
            ((SCREEN_HEIGHT / 2) - text->h) / 2 + y, text, screen);
    }
}

int main(int argc, char* args[]){
    SDL_Surface *name_label = NULL;
    SDL_Surface *ip_label = NULL;
    SDL_Surface *port_label = NULL;

    SDL_Surface *message = NULL;


    bool done = false;

    bool ip_entered = false;

    // initialize SDL and ttf
    if(init() == false){
        return 1;
    }

    // IP address
    client_utils input1;
    client_utils input2;
    client_utils input3;
    client_utils input4;

    // load background
    if(load_background() == false){
        return 1;
    }

    // load textbox
    if(load_textbox() == false){
        return 1;
    }

    // load chatbox
    if(load_chatbox() == false){
        return 1;
    }
    
    // load inputbox
    if(load_inputbox() == false){
        return 1;
    }

    // load clientbox
    if(load_clientbox() == false){
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
                            portbox_clicked = false;
                            button_clicked = false;
                        }

                        // if ipbox is clicked
                        if((x > (SCREEN_WIDTH - textbox->w) / 2 + 200)
                            && (x < (SCREEN_WIDTH - textbox->w) / 2 + 200 + textbox->w)
                            && (y > ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 180)
                            && (y < ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 180 + textbox->h)){
                            
                            namebox_clicked = false;
                            ipbox_clicked = true;
                            portbox_clicked = false;
                            button_clicked = false;
                        }

                        // if portbox is clicked
                        if((x > (SCREEN_WIDTH - textbox->w) / 2 + 200)
                            && (x < (SCREEN_WIDTH - textbox->w) / 2 + 200 + textbox->w)
                            && (y > ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 260)
                            && (y < ((SCREEN_HEIGHT / 2) - textbox->h) / 2 + 260 + textbox->h)){
                            
                            namebox_clicked = false;
                            ipbox_clicked = false;
                            portbox_clicked = true;
                            button_clicked = false;
                        }

                        // if connect button is clicked
                        if((x > (SCREEN_WIDTH - button->w) / 2 + 100)
                            && (x < (SCREEN_WIDTH - button->w) / 2 + 100 + button->w)
                            && (y > ((SCREEN_HEIGHT / 2) - button->h) / 2 + 400)
                            && (y < ((SCREEN_HEIGHT / 2) - button->h) / 2 + 400 + button->h)){
                            
                            namebox_clicked = false;
                            ipbox_clicked = false;
                            portbox_clicked = false;
                            button_clicked = true;
                        }
                    }
                }
                // get user input
                if(namebox_clicked){
                    input1.create_textbox();
                }

                else if(ipbox_clicked){
                    input2.create_ipbox();
                }

                else if(portbox_clicked){
                    input3.create_portbox();
                }
                else if(button_clicked){
                    input4.create_chatbox();
                    //go to chat view

                    textColor = { 0xFF, 0x00, 0x00 };
                    message = TTF_RenderText_Solid(font, "You are connected", textColor);
                    textColor = { 0xFF, 0xFF, 0xFF };
                    

                }

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

        if(button_clicked){
            // display the background
            apply_surface(0, 0, background, screen);

            // display the chatbox
            apply_surface((SCREEN_WIDTH - chatbox->w) / 2 + 160, ((SCREEN_HEIGHT / 2) - chatbox->h) / 2 + 150, chatbox, screen);
            
            // display the input box
            apply_surface((SCREEN_WIDTH - inputbox->w) / 2 + 160, ((SCREEN_HEIGHT / 2) - inputbox->h) / 2 + 500, inputbox, screen);
            input4.display(160, 500);

            // display the client box
            apply_surface((SCREEN_WIDTH - clientbox->w) / 2 - 410, ((SCREEN_HEIGHT / 2) - clientbox->h) / 2 + 186, clientbox, screen);
            
            // update the screen
            if(SDL_Flip(screen) == -1){
                return 1;
            }
        }

        else{
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

            //Show the name
            if(namebox_filled){
                input1.display(200, 101);
            }

            //Show the ip
            if(ipbox_filled){
                input2.display(200, 181);
            }

            //Show the port number
            if(portbox_filled){
                input3.display(200, 261);
            }

            // update the screen
            if(SDL_Flip(screen) == -1){
                return 1;
            }
        }
    }

    // free background surface
    SDL_FreeSurface(background);

    // free message surface
    SDL_FreeSurface(message);

    // close the font
    TTF_CloseFont(font);

    // close ttf
    TTF_Quit();

    // close SDL
    SDL_Quit();

    return 0;
}
