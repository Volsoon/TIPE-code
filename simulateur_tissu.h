/*


███████ ██ ███    ███ ██    ██ ██       █████  ████████ ███████ ██    ██ ██████          ████████ ██ ███████ ███████ ██    ██    ██   ██
██      ██ ████  ████ ██    ██ ██      ██   ██    ██    ██      ██    ██ ██   ██            ██    ██ ██      ██      ██    ██    ██   ██
███████ ██ ██ ████ ██ ██    ██ ██      ███████    ██    █████   ██    ██ ██████             ██    ██ ███████ ███████ ██    ██    ███████
     ██ ██ ██  ██  ██ ██    ██ ██      ██   ██    ██    ██      ██    ██ ██   ██            ██    ██      ██      ██ ██    ██    ██   ██
███████ ██ ██      ██  ██████  ███████ ██   ██    ██    ███████  ██████  ██   ██ ███████    ██    ██ ███████ ███████  ██████  ██ ██   ██


*/
#ifndef SIMULATEUR_TISSU_H
#define SIMULATEUR_TISSU_H

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>


// Structures utilitaires

typedef float float_t;


typedef struct
{
   float_t x;
   float_t y;
   float_t z;
} vect3D_t;

typedef vect3D_t point_t;

typedef struct {
    int v1, v2, v3;
} Face;

typedef struct {
    vect3D_t* vertices;
    Face* faces;
    int vertexCount, faceCount;
} Model;

static inline void printVect3D(vect3D_t v)
{
    printf("(%f, %f, %f)\n", v.x, v.y, v.z);
}

// Addition de deux vecteurs
static inline vect3D_t add(vect3D_t  v1, vect3D_t  v2) {
    vect3D_t result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    result.z = v1.z + v2.z;
    return result;
}

// Soustraction de deux vecteurs
static inline vect3D_t sub(vect3D_t  v1, vect3D_t  v2) {
    vect3D_t result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

// Produit scalaire de deux vecteurs
static inline float_t produit_scalaire(vect3D_t  v1, vect3D_t  v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Produit vectoriel de deux vecteurs
static inline vect3D_t produit_vectoriel(vect3D_t  v1, vect3D_t  v2) {
    vect3D_t result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}

// Normalisation d'un vecteur
static inline vect3D_t normalisation(vect3D_t  v) {
    float_t length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    vect3D_t result;
    if(length > 1e-5) {
        result.x = v.x / length;
        result.y = v.y / length;
        result.z = v.z / length;
        return result;
    }
    return v;
}

// Norme euclidienne d'un vecteur
static inline float_t Norme_euclidienne(vect3D_t v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline vect3D_t produit_par_scalaire(vect3D_t v, float_t scalaire) {
    vect3D_t resultat;
    resultat.x = v.x * scalaire;
    resultat.y = v.y * scalaire;
    resultat.z = v.z * scalaire;
    return resultat;
}


// Structure de particule
typedef struct {
    vect3D_t vitesse;
    vect3D_t position;
    vect3D_t position_prec;
    vect3D_t corr; //Pour la méthode de Jacobi
    float_t masse;
    bool est_fixe;
} Particule_t;


typedef struct
{
    vect3D_t v1;
    vect3D_t v2;
    vect3D_t v3;
}triangle_t;



typedef struct
{
    int I;
    triangle_t triangle;
    vect3D_t trig[3];
    Particule_t p;
    vect3D_t point;
    vect3D_t Normal;
}collision_t;

typedef struct
{
   unsigned int  p1;
   unsigned int  p2;
   unsigned int  p3; // utilisé pour la contrainte de collision
   unsigned int  q; // utilisé pour la contrainte de collision
   float_t dist;
} Contrainte_t;

typedef struct {
    Particule_t *p1;
    Particule_t *p2;
    Particule_t *p3;
    float_t longueur_repos;
    float_t poids;
    float_t kBend;
    float_t k_prime;
} ContrainteFlexion_t;

// Structure de tissu
typedef struct {
    Particule_t* particules;
    int nb_rangees;
    int nb_colonnes;
    int nb_particules;
    float_t epaisseur;
    float_t espacement_particule;
    unsigned int* triangles;
    int nb_triangles;
    triangle_t* trigs;
    int nb_trigs;
    Contrainte_t* contraintes_distance;
    int nb_contraintes_distance;
    Contrainte_t* contraintes_flexion;
    int nb_contraintes_flexion;
    collision_t* collisions;
    int nb_collisions;
} Tissu_t;



typedef struct
{
    float d;
    vect3D_t n;
}plan_t;


Tissu_t* initialiser_tissu(int nb_rangees, int nb_colonnes, float_t epaisseur, float_t espacement_particule);
void generer_triangles_surface(Tissu_t* tissu);
void nettoyer_tissu(Tissu_t* tissu);
Contrainte_t init_contrainte(unsigned int  p1, unsigned int  p2, float_t d);
void reset_tissu(Tissu_t* tissu, float espacement_particule);


#endif // SIMULATEUR_TISSU_H
