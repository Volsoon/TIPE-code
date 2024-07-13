/*
███    ███  █████  ██ ███    ██     ██████
████  ████ ██   ██ ██ ████   ██    ██
██ ████ ██ ███████ ██ ██ ██  ██    ██
██  ██  ██ ██   ██ ██ ██  ██ ██    ██
██      ██ ██   ██ ██ ██   ████ ██  ██████


*/
#include "physique.h"
#include "rendu.h"
#include <time.h>


vect3D_t gravite = {0,-9.8, 0};
int taux_image = 60;
float_t dt = 1.0 / 60;

int largeur_fenetre = 800;
int hauteur_fenetre = 600;

// Paramètres de rotation
float vitesse_rotation = 0.5;
float rotation_x = 0;
float rotation_y = 0;

// Paramètres de translation
float vitesse_translation = 0.05;
float translation_x = 0;
float translation_y = 0;

// État de la souris
int glisser_souris = 0;          // Utiliser 0 pour False, 1 pour True
int pos_souris_prec[2] = {0, 0}; // Utiliser NULL pour None

// Paramètres de déplacement de la caméra
float distance_camera = 25;
float angle_yaw_camera = 0;
float angle_pitch_camera = 0;

int main() {
  // Initialisation de SDL2
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Erreur d'initialisation de SDL: %s\n", SDL_GetError());
    return -1;
  }

  // Création de la fenêtre
  SDL_Window *window = SDL_CreateWindow(
      "Simulation de Tissu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (!window) {
    fprintf(stderr, "Erreur lors de la création de la fenêtre: %s\n",
            SDL_GetError());
    SDL_Quit();
    return -1;
  }

  // Initialisation du contexte OpenGL
  SDL_GLContext contexte = SDL_GL_CreateContext(window);
  if (!contexte) {
    fprintf(stderr, "Erreur lors de la création du contexte OpenGL: %s\n",
            SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }
  initialiser_fenetre(800, 600);

  // Initialiser votre tissu ici
  Tissu_t *tissu = initialiser_tissu(50, 100, 1, 0.1);
  Model model;
  Model model2;
  memset(&model, 0, sizeof(Model));
  memset(&model2, 0, sizeof(Model));
  loadObj("your_model.obj", &model);
  loadObj("your_model_2.obj", &model2);

  int debut_frame;
  int fin_frame;
  int delai_frame;

  int fps_cible = 60;               // Changer selon vos besoins
  int delai_max = 1000 / fps_cible; // Calcul du délai maximal en millisecondes

  // Boucle principale
  int en_cours = 1;
  int frame = 0;
  while (en_cours) {
    SDL_Event event;
    debut_frame = SDL_GetTicks(); // Enregistrez le temps au début de la trame

    // Définissez max_vertical_rotation comme π/2
    const float max_vertical_rotation = 90;

    // appuit sur la touche S pour passer à la frame suivante sinon attendre
    while (frame > 10) {
      SDL_PollEvent(&event);
      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_s) {
          frame = 0;
          break;
        }
      }
    }

    frame++;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mettre_a_jour_position_camera(largeur_fenetre, hauteur_fenetre,
                                  translation_x, translation_y, distance_camera,
                                  angle_pitch_camera, angle_yaw_camera);

    // Boucle de simulation de la PBD

    int nb_substep = 15;
    double sdt = dt / nb_substep;

    // int labmda = 0;

    for (int i = 0; i < nb_substep; i++) {
      tissu->nb_collisions = 0;


      // On met à jour les positions
      for (int i = 0; i < (tissu->nb_rangees * tissu->nb_colonnes); i++) {
        tissu->particules[i].corr = (vect3D_t){0, 0, 0};
        appliquer_forces(&tissu->particules[i], gravite, sdt);
      }

      // Solveur de Gauss-Seidel
      for (size_t j = 0; j < 1; j++) {
        resoudre_contraintes_distance(tissu, 0.0, sdt);
        // resoudre_contraintes_flexion(tissu, 0.5, sdt);
      }

      for (int j = 0; j < tissu->nb_particules; j++) {
        // resoudre_collisions(tissu, model, j);
      }

      tissu->nb_collisions = 0;

      for (int j = 0; j < (tissu->nb_rangees * tissu->nb_colonnes); j++) {
        if (tissu->particules[j].position.y <= 0) {
          tissu->particules[j].position.y = 0.001;
        }
      }

      // On met à jour les vitesses avec la méthode de Verlet

      for (int i = 0; i < tissu->nb_rangees * tissu->nb_colonnes; i++) {
        if (!(tissu->particules[i].est_fixe)) {
          tissu->particules[i].vitesse =
              produit_par_scalaire(sub(tissu->particules[i].position,
                                       tissu->particules[i].position_prec),
                                   (1 / sdt));
          tissu->particules[i].position_prec = tissu->particules[i].position;
        }
      }
    }

    dessiner_sol_infini();
    dessiner_tissu(tissu);
    // drawObj(&model2);
    // dessiner_contraintes(tissu);

    fin_frame = SDL_GetTicks(); // Enregistrez le temps à la fin de la trame
    delai_frame = fin_frame - debut_frame;

    // Vérifiez si le délai est inférieur au délai maximal pour atteindre le FPS
    // cible
    if (delai_frame < delai_max) {
      SDL_Delay(delai_max - delai_frame); // Pause pour atteindre le FPS cible
    }

    // Afficher le FPS
    //printf("FPS: %f\n", 1000.0 / (SDL_GetTicks() - debut_frame));

    SDL_GL_SwapWindow(window);
  }

  // Libération des ressources à la fin du programme
  nettoyer_tissu(tissu);
  free(model.vertices);
  free(model.faces);
  free(model2.vertices);
  free(model2.faces);

  // Fermeture de la fenêtre et nettoyage de SDL
  SDL_GL_DeleteContext(contexte);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
