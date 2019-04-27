#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"


#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define SPRITE_WIDTH 18
#define SPRITE_HEIGHT 28

#define TRUE 1
#define FALSE 0    

int main(int argc, char** argv) {
    printf("Sprite Blitting Example\n");

    // Initialize the random number generator
    srand(time(NULL));
    
    // Initialize SDL and SDL_Image
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        SDL_Log("Unable to initialize SDL_Image: %s", SDL_GetError());
        return 1;
    }

    // Create the window and the renderer
    SDL_Window* window;
    SDL_Renderer* renderer;
    if (SDL_CreateWindowAndRenderer(
                WINDOW_WIDTH,
                WINDOW_HEIGHT,
                SDL_WINDOW_OPENGL,
                &window,
                &renderer) == -1) {
        SDL_Log("Unable to initialize the window: %s", SDL_GetError());
        return 1;
    }
    SDL_SetWindowTitle(window, "Sprite Blitting Example");

    // Load the spritesheet
    SDL_Surface* spritesheet = IMG_Load("BrogueFont5.png");
    if (spritesheet == NULL) {
        SDL_Log("Unable to load BrogueFont5.png! %s", SDL_GetError());
        return 1;
    }

    // Set the background color (black) to transparent
    if (SDL_SetColorKey(spritesheet, 1, 0x000000FF) != 0) {
        SDL_Log("Unable to set color key on the spritesheet: %s",
                SDL_GetError());
        return 1;
    }
    SDL_PixelFormat* fmt = spritesheet->format;

    // Convert the spritesheet to a texture
    SDL_Texture* tex_sheet = SDL_CreateTextureFromSurface(renderer, spritesheet);
    if (tex_sheet == NULL) {
        SDL_Log("Unable to convert spritesheet to a texture: %s", SDL_GetError());
        return 1;
    }

    // Set the blend mode so we don't get jagged edges
    SDL_SetTextureBlendMode(tex_sheet, SDL_BLENDMODE_ADD);

    // Set up a double buffer to blit to, so we don't have to worry about
    // tearing
    SDL_Texture *buffer = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            WINDOW_WIDTH,
            WINDOW_HEIGHT);
    if (buffer == NULL) {
        SDL_Log("Unable to create double buffer: %s", SDL_GetError());
        return 1;
    }

    SDL_Event event;
    Uint32 ticks_elapsed = 1001; // We want to blit immediately at start
    Uint32 old_ticks;
    int quit = FALSE;
    while (!quit) {
        // We want to render once a second, but we don't want to hang up our event
        // pump, so SDL_Delay is a bad choice.  Instead figure out how long the main
        // loop takes, and increment a counter until a second has passed, then render
        old_ticks = SDL_GetTicks();
        
        while(SDL_PollEvent(&event)) {
            switch (event.type) { 
                case SDL_QUIT: 
                    quit = TRUE;
                    break;
                case SDL_KEYDOWN:
                    printf("Key pressed: %i\n", event.key.keysym.sym);
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        quit = TRUE;
                    }
                    break;
                default:
                    ;
            }
        }

        // Set the renderer to render to the double buffer
        SDL_SetRenderTarget(renderer, buffer);

        // Now clear the buffer and blit a random assortment of sprites,
        // with a random assortment of colors, to the double buffer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        if (ticks_elapsed > 1000) {
            ticks_elapsed = 0;
            for (int i = 0; i < WINDOW_WIDTH / SPRITE_WIDTH + 1; i++) {
                for (int j = 0; j < WINDOW_HEIGHT / SPRITE_HEIGHT + 1; j++) {
                    int glyph_x = rand() % 16;
                    int glyph_y = 3 + rand() % 13;
                    SDL_Color fg = {
                        rand() % 256,
                        rand() % 256,
                        rand() % 256,
                        255};
                    SDL_Color bg = {
                        rand() % 256,
                        rand() % 256,
                        rand() % 256,
                        255};

                    // Clipping rects for source (cut from spritesheet)
                    // and dest (buffer location)
                    SDL_Rect dest_rect = { i * 18, j * 28, 18, 28 };
                    SDL_Rect src_rect = { glyph_x * 18, glyph_y * 28, 18, 28 };
                    
                    // Random color for background
                    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, 255);
                    SDL_RenderFillRect(renderer, &dest_rect);
                    
                    // Tint the sprite to be blitted
                    SDL_SetTextureColorMod(tex_sheet, fg.r, fg.g, fg.b);
                    SDL_RenderCopy(renderer, tex_sheet, &src_rect, &dest_rect);
                }
            }
            // Set the renderer back to the screen, clear it, and flip the buffer
            SDL_SetRenderTarget(renderer, NULL);
            SDL_RenderCopy(
                           renderer,
                           buffer,
                           NULL,
                           NULL);
            SDL_RenderPresent(renderer);
        }

        ticks_elapsed += SDL_GetTicks() - old_ticks;
    }

    // Teardown
    IMG_Quit();
    SDL_Quit();
}
