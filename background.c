#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "background.h"

int initBack(background *b) {
    b->img = IMG_Load("bg1.png");
    if (!b->img) {
        fprintf(stderr, "Erreur chargement background.png : %s\n", IMG_GetError());
        return 0;
    }
    b->img_pos.x = b->img_pos.y = 0;
    b->img_size.x = b->img_size.y = 0;
    b->img_size.w = SCREEN_W;
    b->img_size.h = SCREEN_H;
    b->start_time = 0;
    return 1;
}

void afficherBack(background b, SDL_Surface *screen) {
    SDL_BlitSurface(b.img, &b.img_size, screen, &b.img_pos);
}

int musicLoad1(Mix_Music **music) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
        fprintf(stderr, "Erreur audio : %s\n", Mix_GetError());
        return 0;
    }
    *music = Mix_LoadMUS("music.mp3");
    if (!*music) {
        fprintf(stderr, "Erreur musique : %s\n", Mix_GetError());
        return 0;
    }
    Mix_PlayMusic(*music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    return 1;
}

void scrolling(background *b, int direction, int dx, int dy) {
    SDL_Rect nvpos = b->img_size;

    if (direction == 0) nvpos.x += dx;
    else if (direction == 1) nvpos.x -= dx;
    else if (direction == 2) nvpos.y -= dy;
    else if (direction == 3) nvpos.y += dy;

    if (nvpos.x < 0) nvpos.x = 0;
    if (nvpos.x + SCREEN_W > b->img->w) nvpos.x = b->img->w - SCREEN_W;

    if (nvpos.y < 0) nvpos.y = 0;
    if (nvpos.y + SCREEN_H > b->img->h) nvpos.y = b->img->h - SCREEN_H;

    b->img_size = nvpos;
}

int backgroundLoad_lvl1(image *img) {
    img->filename = "bg1.png";
    img->img = IMG_Load(img->filename);
    if (!img->img) {
        fprintf(stderr, "Erreur chargement bg1.png : %s\n", IMG_GetError());
        return 0;
    }
    img->img_size.x = img->img_size.y = 0;
    img->img_size.w = img->img->w;
    img->img_size.h = img->img->h;
    img->img_pos.x = img->img_pos.y = 0;
    return 1;
}

void backgroundDraw_lvl1(SDL_Surface *screen, image *img) {
    SDL_BlitSurface(img->img, &img->img_size, screen, &img->img_pos);
}

void splitScreen(SDL_Surface *screen, image *img, SDL_Rect cam1, SDL_Rect cam2) {
    SDL_Rect left_screen = {0, 0, SCREEN_W / 2, SCREEN_H};
    SDL_Rect right_screen = {SCREEN_W / 2, 0, SCREEN_W / 2, SCREEN_H};

    SDL_BlitSurface(img->img, &cam1, screen, &left_screen);
    SDL_BlitSurface(img->img, &cam2, screen, &right_screen);

    SDL_Rect border = {SCREEN_W / 2 - 1, 0, 2, SCREEN_H};
    Uint32 line_color = SDL_MapRGB(screen->format, 255, 255, 255);
    SDL_FillRect(screen, &border, line_color);
}


void afficherTempsGlobal(Uint32 start_time, SDL_Surface *screen, TTF_Font *font) {
    Uint32 current = SDL_GetTicks() - start_time;
    char time_str[20];
    sprintf(time_str, "Time: %02d:%02d", current / 60000, (current / 1000) % 60);
    SDL_Surface* txt = TTF_RenderText_Solid(font, time_str, (SDL_Color){255,255,255});
    SDL_Rect txt_rect = {SCREEN_W - txt->w - 10, 10};
    SDL_BlitSurface(txt, NULL, screen, &txt_rect);
    SDL_FreeSurface(txt);
}

void update_game_time(background *back, SDL_Surface *screen, TTF_Font *font) {
    if (!back->start_time) back->start_time = SDL_GetTicks();
    afficherTempsGlobal(back->start_time, screen, font);
}
void update_camera(SDL_Rect *cam, int direction, SDL_Surface *img, int dx, int dy) {
    if (direction == 0) cam->x += dx;
    else if (direction == 1) cam->x -= dx;
    else if (direction == 2) cam->y -= dy;
    else if (direction == 3) cam->y += dy;

    // Limites
    if (cam->x < 0) cam->x = 0;
    if (cam->x + cam->w > img->w) cam->x = img->w - cam->w;
    if (cam->y < 0) cam->y = 0;
    if (cam->y + cam->h > img->h) cam->y = img->h - cam->h;
}

SDL_Surface* loadImage(char *path) {
    SDL_Surface *img = IMG_Load(path);
    if (!img) {
        printf("Erreur chargement image %s : %s\n", path, IMG_GetError());
    }
    return img;
}

void afficherGuideAvecIcone(SDL_Surface *screen, TTF_Font *font) {
    SDL_Color texte_color = {0, 0, 0};
    SDL_Rect box = {(screen->w - 500) / 2, (screen->h - 140) / 2, 500, 140};

    SDL_Surface *overlay = SDL_CreateRGBSurface(SDL_SRCALPHA, box.w, box.h, 32,
        screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
    SDL_FillRect(overlay, NULL, SDL_MapRGBA(screen->format, 255, 255, 255, 200));
    SDL_BlitSurface(overlay, NULL, screen, &box);
    SDL_FreeSurface(overlay);

    const char *lignes[] = {
        "            === GUIDE DE JEU ===",
        "Joueur 1 : Z Q S D",
        "Joueur 2 :(up,left,down,right)", 
        "ESC : Quitter",
        NULL
    };

    int y_offset = box.y + 15;
    for (int i = 0; lignes[i] != NULL; i++) {
        SDL_Surface *txt = TTF_RenderText_Blended(font, lignes[i], texte_color);
        if (txt) {
            SDL_Rect pos = {box.x + 20, y_offset};
            SDL_BlitSurface(txt, NULL, screen, &pos);
            SDL_FreeSurface(txt);
            y_offset += 28;
        }
    }
}

int is_end_of_background(background *b) {
    return (b->img_size.x + SCREEN_W >= b->img->w);
}



// Mise à jour des frames de l'animation
void update_animation(background *image, int frame_width, int nb_frames) {
    image->camera_pos.x += frame_width;
    if (image->camera_pos.x >= frame_width * nb_frames) {
        image->camera_pos.x = 0;
    }
}
