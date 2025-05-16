	#include <stdlib.h>
	#include <SDL/SDL.h>
	#include <SDL/SDL_image.h>
	#include <SDL/SDL_mixer.h>
	#include <SDL/SDL_ttf.h>
	#include "background.h"
	background robot;
	    
	SDL_Surface *iconeGuide = NULL;
	Uint32 start_time; // déclaration globale
	int score = 0;


	int main(int argc, char *argv[]) {
	    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	    IMG_Init(IMG_INIT_PNG);
	    TTF_Init();

	    start_time = SDL_GetTicks(); 

	    SDL_Surface *screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	    SDL_WM_SetCaption("Jeu ", NULL);

	    TTF_Font *font = TTF_OpenFont("October-Wish.ttf", 24);
	    if (!font) {
		fprintf(stderr, "Erreur chargement October-Wish.ttf : %s\n", TTF_GetError());
		return 1;
	    }
	// Charger l'image nom.png
	SDL_Surface *nom_bg = IMG_Load("nom.png");
	if (!nom_bg) {
	    fprintf(stderr, "Erreur chargement nom.png : %s\n", IMG_GetError());
	    return 1;
	}

	// Centrer dans l'écran fullscreen
	SDL_Rect centered;
	centered.x = (SCREEN_W - 800) / 2;
	centered.y = (SCREEN_H - 600) / 2;

	SDL_BlitSurface(nom_bg, NULL, screen, &centered);
	SDL_Flip(screen);



	char player_name[20] = "";
	int input_done = 0;
	int char_index = 0;
	SDL_Event event;

	SDL_EnableUNICODE(1); // Activer l'entrée Unicode pour récupérer les caractères
	while (!input_done) {
	    while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
		    SDL_FreeSurface(nom_bg);
		    TTF_CloseFont(font);
		    SDL_Quit();
		    return 0;
		}

		if (event.type == SDL_KEYDOWN) {
		    if (event.key.keysym.sym == SDLK_RETURN && char_index > 0) {
		        input_done = 1;
		    } else if (event.key.keysym.sym == SDLK_BACKSPACE && char_index > 0) {
		        player_name[--char_index] = '\0';
		    } else if ((event.key.keysym.unicode & 0xFF80) == 0 && char_index < 19) {
		        player_name[char_index++] = event.key.keysym.unicode & 0x7F;
		        player_name[char_index] = '\0';
		    }
		}
	    }

	// Réafficher l'image nom.png centrée

	SDL_BlitSurface(nom_bg, NULL, screen, &centered);

	SDL_Color textColor = {255, 255, 255};
	SDL_Surface *textSurface = NULL;
	if (strlen(player_name) > 0)
	    textSurface = TTF_RenderText_Solid(font, player_name, textColor);

	if (textSurface) {
	    SDL_Rect textPos;
	    textPos.x = centered.x + (800 - textSurface->w) / 2;
	    textPos.y = centered.y + (600 - textSurface->h) / 2 + 20; // Décalé de 10px vers le bas
	    SDL_BlitSurface(textSurface, NULL, screen, &textPos);
	    SDL_FreeSurface(textSurface);
	}

	SDL_Flip(screen);

	}


	    iconeGuide = loadImage("guide.png");
	    if (!iconeGuide) {
		printf("Erreur chargement guide.png\n");
		return 1;
	    }

	    SDL_Rect posIcone = {10, 10};
	    int afficher_guide = 0;

	    image lvl1_img, lvl2_img;
	    if (!backgroundLoad_lvl1(&lvl1_img)) return 1;
	    lvl2_img.filename = "bg2.png";
	    lvl2_img.img = IMG_Load(lvl2_img.filename);
	    if (!lvl2_img.img) {
		fprintf(stderr, "Erreur chargement bg2.png : %s\n", IMG_GetError());
		return 1;
	    }
	    lvl2_img.img_size.x = lvl2_img.img_size.y = 0;
	    lvl2_img.img_size.w = lvl2_img.img->w;
	    lvl2_img.img_size.h = lvl2_img.img->h;
	    lvl2_img.img_pos.x = lvl2_img.img_pos.y = 0;

	    int niveau_p1 = 1, niveau_p2 = 1;

	    Mix_Music *music;
	    if (!musicLoad1(&music)) fprintf(stderr, "Continuer sans musique.\n");

	    
	    int running = 1, direction1 = -1, direction2 = -1, split_screen = 0;
	    SDL_Rect cam1 = {0, 0, SCREEN_W / 2, SCREEN_H};
	    SDL_Rect cam2 = {SCREEN_W / 2, 0, SCREEN_W / 2, SCREEN_H};
	    
	    while (running) {

		while (SDL_PollEvent(&event)) {
		    if (event.type == SDL_QUIT) running = 0;

		    if (event.type == SDL_KEYDOWN) {
		        switch (event.key.keysym.sym) {
		            case SDLK_d: direction1 = 0; break;
		            case SDLK_q: direction1 = 1; break;
		            case SDLK_z: direction1 = 2; break;
		            case SDLK_s: direction1 = 3; break;

		            case SDLK_RIGHT: direction2 = 0; break;
		            case SDLK_LEFT:  direction2 = 1; break;
		            case SDLK_UP:    direction2 = 2; break;
		            case SDLK_DOWN:  direction2 = 3; break;

		            case SDLK_p: split_screen = !split_screen; break;
		            case SDLK_ESCAPE: running = 0; break;
		            default: break; 
		        }
		    }

		    if (event.type == SDL_KEYUP) {
		        switch (event.key.keysym.sym) {
		            case SDLK_d: case SDLK_q: case SDLK_z: case SDLK_s:
		                direction1 = -1; break;
		            case SDLK_RIGHT: case SDLK_LEFT: case SDLK_UP: case SDLK_DOWN:
		                direction2 = -1; break;
	 default: break;

		        }
		    }

		    if (event.type == SDL_MOUSEBUTTONDOWN) {
		        int mx = event.button.x;
		        int my = event.button.y;

		        if (mx >= posIcone.x && mx <= posIcone.x + iconeGuide->w &&
		            my >= posIcone.y && my <= posIcone.y + iconeGuide->h) {
		            afficher_guide = !afficher_guide;
		        }
		    }
		}
		
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

		if (split_screen) {
		    if (niveau_p1 == 1) update_camera(&cam1, direction1, lvl1_img.img, 5, 5);
		    else update_camera(&cam1, direction1, lvl2_img.img, 5, 5);

		    if (niveau_p2 == 1) update_camera(&cam2, direction2, lvl1_img.img, 5, 5);
		    else update_camera(&cam2, direction2, lvl2_img.img, 5, 5);

		    SDL_Rect left_screen = {0, 0, SCREEN_W / 2, SCREEN_H};
		    SDL_Rect right_screen = {SCREEN_W / 2, 0, SCREEN_W / 2, SCREEN_H};

		    if (niveau_p1 == 1)
		        SDL_BlitSurface(lvl1_img.img, &cam1, screen, &left_screen);
		    else
		        SDL_BlitSurface(lvl2_img.img, &cam1, screen, &left_screen);

		    if (niveau_p2 == 1)
		        SDL_BlitSurface(lvl1_img.img, &cam2, screen, &right_screen);
		    else
		        SDL_BlitSurface(lvl2_img.img, &cam2, screen, &right_screen);

		    // Mise à jour de chaque niveau + reset camera si passage au niveau 2
		    if (niveau_p1 == 1 && cam1.x + cam1.w >= lvl1_img.img->w) {
		        niveau_p1 = 2;
		        cam1.x = 0; cam1.y = 0; // retour au début du niveau 2
		    }
		    if (niveau_p2 == 1 && cam2.x + cam2.w >= lvl1_img.img->w) {
		        niveau_p2 = 2;
		        cam2.x = 0; cam2.y = 0;
		    }
		} else {
		    static background bg, bg2;
		    static int bg2_charge = 0;
		    if (!bg.img) initBack(&bg);

	if (niveau_p1 == 1) {
	    int old_x = bg.img_size.x;

	    scrolling(&bg, direction1, 5, 5);

	    if (bg.img_size.x > old_x) {
		score += 1; // Incrémente le score quand on scrolle à droite
	    }

	    afficherBack(bg, screen);
		        
		        if (is_end_of_background(&bg)) {
		            niveau_p1 = 2;
		            if (!bg2_charge) {
		                bg2.img = IMG_Load("bg2.png");
		                if (!bg2.img) {
		                    fprintf(stderr, "Erreur chargement bg2.png : %s\n", IMG_GetError());
		                    return 1;
		                }
		                bg2.img_pos.x = bg2.img_pos.y = 0;
		                bg2.img_size.x = bg2.img_size.y = 0;
		                bg2.img_size.w = SCREEN_W;
		                bg2.img_size.h = SCREEN_H;
		                bg2.start_time = SDL_GetTicks();
		                bg2_charge = 1;
		            }
		        }
		    } else {
    int old_x = bg2.img_size.x;
    scrolling(&bg2, direction1, 5, 5);
    if (bg2.img_size.x > old_x) score += 1;
    afficherBack(bg2, screen);
    
  
    if (is_end_of_background(&bg2)) {
        running = 0; // Quitte la boucle principale pour passer à l’affichage des scores
    }
}

		    /**update_game_time((niveau_p1 == 1) ? &bg : &bg2, screen, font);*/
		}

		SDL_BlitSurface(iconeGuide, NULL, screen, &posIcone);
		if (afficher_guide) afficherGuideAvecIcone(screen, font);
		afficherTempsGlobal(start_time, screen, font);
	char score_str[30];
	sprintf(score_str, "Score: %d", score);
	SDL_Surface *score_txt = TTF_RenderText_Solid(font, score_str, (SDL_Color){255, 255, 255});
	SDL_Rect score_pos = {10, 80}; // En haut à gauche sous l’horloge
	SDL_BlitSurface(score_txt, NULL, screen, &score_pos);
	SDL_FreeSurface(score_txt);

		SDL_Flip(screen);
		SDL_Delay(30);
	    }

	    if (iconeGuide) SDL_FreeSurface(iconeGuide);
	    SDL_FreeSurface(lvl1_img.img);
	    SDL_FreeSurface(lvl2_img.img);
	    if (music) Mix_FreeMusic(music);
	    Mix_CloseAudio();
// Enregistrer le score du joueur
FILE *f = fopen("scores.txt", "a");
if (f) {
    fprintf(f, "%s : %d\n", player_name, score);
    fclose(f);
}

// Afficher les 3 meilleurs scores dans best.jpg
SDL_Surface *best_img = IMG_Load("best.png");
if (!best_img) {
    fprintf(stderr, "Erreur chargement best.png : %s\n", IMG_GetError());
} else {
    SDL_BlitSurface(best_img, NULL, screen, NULL);

    // Lire tous les scores
    char noms[100][50];
    int scores[100];
    int count = 0;

    f = fopen("scores.txt", "r");
    if (f) {
        while (fscanf(f, "%s : %d", noms[count], &scores[count]) == 2) {
            count++;
            if (count >= 100) break;
        }
        fclose(f);
    }

    // Trier décroissant
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[j] > scores[i]) {
                int tmp = scores[i]; scores[i] = scores[j]; scores[j] = tmp;
                char tmpn[50]; strcpy(tmpn, noms[i]); strcpy(noms[i], noms[j]); strcpy(noms[j], tmpn);
            }
        }
    }

    // Afficher top 3
// Afficher top 3
SDL_Color color = {255, 255, 255};
for (int i = 0; i < count && i < 3; i++) {
    char line[100];
    sprintf(line, " %s%15s%d", noms[i], "", scores[i]);
    SDL_Surface *txt = TTF_RenderText_Solid(font, line, color);
    if (txt) {
        SDL_Rect pos = {(SCREEN_W - txt->w+20) / 2 - 110, 220 + i * 115};

        SDL_BlitSurface(txt, NULL, screen, &pos);
        SDL_FreeSurface(txt);
    } else {
        fprintf(stderr, "Erreur rendu texte score %d : %s\n", i + 1, TTF_GetError());
    }
}

    SDL_Flip(screen);
    SDL_Delay(5000);
    SDL_FreeSurface(best_img);
} 

// Nettoyage final
if (iconeGuide) SDL_FreeSurface(iconeGuide);
TTF_CloseFont(font);
if (music) Mix_FreeMusic(music);
Mix_CloseAudio();
TTF_Quit();
IMG_Quit();
SDL_Quit();
return 0;
}

