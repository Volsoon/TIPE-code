/*

██████  ███████ ███    ██ ██████  ██    ██    ██   ██
██   ██ ██      ████   ██ ██   ██ ██    ██    ██   ██
██████  █████   ██ ██  ██ ██   ██ ██    ██    ███████
██   ██ ██      ██  ██ ██ ██   ██ ██    ██    ██   ██
██   ██ ███████ ██   ████ ██████   ██████  ██ ██   ██


*/
#ifndef RENDU_TISSU_H
#define RENDU_TISSU_H

#include "simulateur_tissu.h"


typedef vect3D_t Vertex;

// Prototypes de fonctions pour le rendu
void initialiser_fenetre(int largeur, int hauteur);
void dessiner_tissu(Tissu_t* tissu);
void mettre_a_jour_position_camera(int largeur_fenetre, int hauteur_fenetre, int translation_x, int translation_y, int distance_camera, int angle_pitch_camera, int angle_yaw_camera);
void nettoyer(SDL_Window* window);
void loadObj(const char* filename, Model* model);
void drawObj(Model* model);
void dessiner_sol_infini();

#endif // RENDU_TISSU_H
