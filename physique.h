/*

██████  ██   ██ ██    ██ ███████ ██  ██████  ██    ██ ███████    ██   ██
██   ██ ██   ██  ██  ██  ██      ██ ██    ██ ██    ██ ██         ██   ██
██████  ███████   ████   ███████ ██ ██    ██ ██    ██ █████      ███████
██      ██   ██    ██         ██ ██ ██ ▄▄ ██ ██    ██ ██         ██   ██
██      ██   ██    ██    ███████ ██  ██████   ██████  ███████ ██ ██   ██
                                        ▀▀

*/
#ifndef PHYSIQUE_TISSU_H
#define PHYSIQUE_TISSU_H

#include "simulateur_tissu.h"

// Prototypes de fonctions pour la simulation physique
void appliquer_forces(Particule_t* tissu, vect3D_t gravite, float_t dt);
void resoudre_contraintes_distance(Tissu_t* tissu, float_t compliance, float_t dt);
void resoudre_contraintes_flexion(Tissu_t* tissu, float_t compliance, float_t dt);
void resoudre_collisions(Tissu_t* tissu, Model E, int i);


#endif // PHYSIQUE_TISSU_H
