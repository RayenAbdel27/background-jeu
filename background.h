
#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#define SCREEN_W 800
#define SCREEN_H 600

typedef struct {
    SDL_Surface *img;
    SDL_Rect img_pos;
    SDL_Rect img_size;
    Uint32 start_time;

    // Champs pour l'animation robot
    SDL_Rect camera_pos;
    SDL_Rect pos_ecran;
    int direction;
} background;


typedef struct {
    char *filename;
    SDL_Surface *img;
    SDL_Rect img_size;
    SDL_Rect img_pos;
} image;
SDL_Surface* loadImage(char *path);
void afficherGuideAvecIcone(SDL_Surface *screen, TTF_Font *font);


int initBack(background *b);
void afficherBack(background b, SDL_Surface *screen);
int musicLoad1(Mix_Music **music);
void scrolling(background *b, int direction, int dx, int dy);
int backgroundLoad_lvl1(image *img);
void backgroundDraw_lvl1(SDL_Surface *screen, image *img);
void splitScreen(SDL_Surface *screen, image *img, SDL_Rect cam1, SDL_Rect cam2);
void update_camera(SDL_Rect *cam, int direction, SDL_Surface *img, int dx, int dy);
int is_end_of_background(background *b);
void update_game_time(background *back, SDL_Surface *screen, TTF_Font *font);
void afficherTempsGlobal(Uint32 start_time, SDL_Surface *screen, TTF_Font *font);
void init_animerBack(background* image);
void afficher_animerBack(background image, SDL_Surface *screen);
void update_animation(background *image, int frame_width, int nb_frames);


#endif
