/*

███████ ██ ███    ███ ██    ██ ██       █████  ████████ ███████ ██    ██ ██████          ████████ ██ ███████ ███████ ██    ██     ██████
██      ██ ████  ████ ██    ██ ██      ██   ██    ██    ██      ██    ██ ██   ██            ██    ██ ██      ██      ██    ██    ██
███████ ██ ██ ████ ██ ██    ██ ██      ███████    ██    █████   ██    ██ ██████             ██    ██ ███████ ███████ ██    ██    ██
     ██ ██ ██  ██  ██ ██    ██ ██      ██   ██    ██    ██      ██    ██ ██   ██            ██    ██      ██      ██ ██    ██    ██
███████ ██ ██      ██  ██████  ███████ ██   ██    ██    ███████  ██████  ██   ██ ███████    ██    ██ ███████ ███████  ██████  ██  ██████


*/
#include "simulateur_tissu.h"

Tissu_t* initialiser_tissu(int nb_rangees, int nb_colonnes, float_t epaisseur, float_t espacement_particule) {
    Tissu_t* tissu = (Tissu_t*)malloc(sizeof(Tissu_t));
    assert(tissu != NULL && "Échec de l'allocation pour la structure Tissu_t.");


    tissu->nb_rangees = nb_rangees;
    tissu->nb_colonnes = nb_colonnes;
    tissu->espacement_particule = espacement_particule;
    tissu->nb_triangles = 0;
    tissu->nb_trigs = 0;
    tissu->nb_contraintes_distance = 0;
    tissu->nb_contraintes_flexion = 0;
    tissu->nb_collisions = 0;
    tissu->epaisseur = epaisseur;
    tissu->nb_particules = nb_rangees * nb_colonnes;
    tissu->collisions = (collision_t*)malloc(sizeof(collision_t) * tissu->nb_particules * 100);
    
    

    // Initialisation des particules
    tissu->particules = (Particule_t*)malloc(nb_rangees * nb_colonnes * sizeof(Particule_t));
    assert(tissu->particules != NULL && "Échec de l'allocation des particules.");
    
    for (int i = 0; i < nb_rangees; i++) {
        for (int j = 0; j < nb_colonnes; j++) {
            int index = i * nb_colonnes + j;
            tissu->particules[index].position.x = i * espacement_particule - (nb_rangees / 2.0) * espacement_particule;
            tissu->particules[index].position.y = 8;
            tissu->particules[index].position.z = j * espacement_particule - (nb_colonnes / 2.0) * espacement_particule;


            tissu->particules[index].position_prec.x = i * espacement_particule - (nb_rangees / 2.0) * espacement_particule;
            tissu->particules[index].position_prec.y = 8;
            tissu->particules[index].position_prec.z = j * espacement_particule - (nb_colonnes / 2.0) * espacement_particule;


            // Initialisation des autres propriétés des particules
            tissu->particules[index].vitesse.x = 0;
            tissu->particules[index].vitesse.y = 0;
            tissu->particules[index].est_fixe = false;
            tissu->particules[index].vitesse.z = 0;
            tissu->particules[index].masse = 1;

            
            if ((j == nb_colonnes - 1) && (i == 0 || i == nb_rangees - 1))
            {
                tissu->particules[index].est_fixe = true;
            }
            
            
            
            
            

           
        }
        
    }
    

    // Génération des triangles de la surface
    generer_triangles_surface(tissu);

    return tissu;
}


void reset_tissu(Tissu_t* tissu, float espacement_particule) {
    for (int i = 0; i < tissu->nb_rangees; i++) {
        for (int j = 0; j < tissu->nb_colonnes; j++) {
            int index = i * tissu->nb_colonnes + j;
            tissu->particules[index].position.x = i * espacement_particule - (tissu->nb_rangees / 2.0) * espacement_particule;
            tissu->particules[index].position.y = 8; // Réinitialisez la position y si nécessaire
            tissu->particules[index].position.z = j * espacement_particule - (tissu->nb_colonnes / 2.0) * espacement_particule;

            tissu->particules[index].position_prec.x = tissu->particules[index].position.x;
            tissu->particules[index].position_prec.y = tissu->particules[index].position.y;
            tissu->particules[index].position_prec.z = tissu->particules[index].position.z;

            // Réinitialisez d'autres propriétés des particules si nécessaire
        }
    }

    // Réinitialisez d'autres paramètres du tissu si nécessaire
}


void generer_triangles_surface(Tissu_t* tissu) {
    tissu->triangles = (unsigned int *)malloc(6 * (tissu->nb_rangees - 1) * (tissu->nb_colonnes - 1) * sizeof(unsigned int ));
    assert(tissu->triangles != NULL && "Échec de l'allocation pour les triangles.");
    tissu->nb_triangles = 0;
    tissu->trigs = (triangle_t*)malloc(2 * (tissu->nb_rangees - 1) * (tissu->nb_colonnes - 1) * sizeof(triangle_t));
    assert(tissu->trigs != NULL && "Échec de l'allocation pour les triangles.");

    tissu->contraintes_distance = (Contrainte_t*)malloc(6 * (tissu->nb_rangees - 1) * (tissu->nb_colonnes - 1) * sizeof(Contrainte_t));
    assert(tissu->contraintes_distance != NULL && "Échec de l'allocation pour les contraintes de distance.");
    tissu->nb_contraintes_distance = 0;

    tissu->contraintes_flexion = (Contrainte_t*)malloc(12 * (tissu->nb_rangees - 1) * (tissu->nb_colonnes - 1) * sizeof(Contrainte_t));
    assert(tissu->contraintes_flexion != NULL && "Échec de l'allocation pour les contraintes de flexion.");
    tissu->nb_contraintes_flexion = 0;


    for (int i = 0; i < tissu->nb_rangees - 1; i++) {
        for (int j = 0; j < tissu->nb_colonnes - 1; j++) {
            int coin_sup_gauche = i * tissu->nb_colonnes + j;
            int coin_sup_droit = i * tissu->nb_colonnes + (j + 1);
            int coin_inf_gauche = (i + 1) * tissu->nb_colonnes + j;
            int coin_inf_droit = (i + 1) * tissu->nb_colonnes + (j + 1);


            tissu->triangles[tissu->nb_triangles++] = coin_sup_droit;
            tissu->triangles[tissu->nb_triangles++] = coin_inf_droit;
            tissu->triangles[tissu->nb_triangles++] = coin_inf_gauche;
            

            tissu->triangles[tissu->nb_triangles++] = coin_sup_gauche;
            tissu->triangles[tissu->nb_triangles++] = coin_sup_droit;
            tissu->triangles[tissu->nb_triangles++] = coin_inf_gauche;

    
            // Ajout des contraintes de distance sur les bords des triangles

            // Triangle 1
            tissu->contraintes_distance[tissu->nb_contraintes_distance++] = init_contrainte(coin_sup_gauche, coin_inf_gauche, Norme_euclidienne(sub(tissu->particules[coin_sup_gauche].position, tissu->particules[coin_inf_gauche].position)));
            tissu->contraintes_distance[tissu->nb_contraintes_distance++] = init_contrainte(coin_sup_gauche, coin_sup_droit, Norme_euclidienne(sub(tissu->particules[coin_sup_gauche].position, tissu->particules[coin_sup_droit].position)));

            // Diagonale
            //tissu->contraintes_flexion[tissu->nb_contraintes_flexion++] = init_contrainte(coin_inf_gauche, coin_sup_droit, Norme_euclidienne(sub(tissu->particules[coin_inf_gauche].position, tissu->particules[coin_sup_droit].position)));

            // Triangle 2
            tissu->contraintes_distance[tissu->nb_contraintes_distance++] = init_contrainte(coin_inf_gauche, coin_inf_droit, Norme_euclidienne(sub(tissu->particules[coin_inf_gauche].position, tissu->particules[coin_inf_droit].position)));
            tissu->contraintes_distance[tissu->nb_contraintes_distance++] = init_contrainte(coin_inf_droit, coin_sup_droit, Norme_euclidienne(sub(tissu->particules[coin_inf_droit].position, tissu->particules[coin_sup_droit].position)));

            // Contrainte de flexion
            tissu->contraintes_flexion[tissu->nb_contraintes_flexion++] = init_contrainte(coin_sup_gauche, coin_inf_droit, Norme_euclidienne(sub(tissu->particules[coin_sup_gauche].position, tissu->particules[coin_inf_droit].position)));
        }
    }
}


void nettoyer_tissu(Tissu_t* tissu) {
    free(tissu->particules);
    free(tissu->triangles);
    free(tissu->contraintes_distance);
    free(tissu->contraintes_flexion);
    free(tissu);
}

Contrainte_t init_contrainte(unsigned int  p1, unsigned int  p2, float_t d) {
    Contrainte_t contrainte;
    contrainte.p1 = p1;
    contrainte.p2 = p2;
    contrainte.dist = d;
    return contrainte;
}

