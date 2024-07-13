/*

██████  ██   ██ ██    ██ ███████ ██  ██████  ██    ██ ███████     ██████
██   ██ ██   ██  ██  ██  ██      ██ ██    ██ ██    ██ ██         ██
██████  ███████   ████   ███████ ██ ██    ██ ██    ██ █████      ██
██      ██   ██    ██         ██ ██ ██ ▄▄ ██ ██    ██ ██         ██
██      ██   ██    ██    ███████ ██  ██████   ██████  ███████ ██  ██████
                                        ▀▀

*/


#include "physique.h"
#include "rendu.h"


void appliquer_forces(Particule_t* p, vect3D_t gravite, float_t dt) {
    if (!p->est_fixe) {
        p->position = add(p->position, add(produit_par_scalaire(p->vitesse, dt), produit_par_scalaire(add(gravite, produit_par_scalaire(p->vitesse, -0.4)), dt*dt)));

    }
}

void resoudre_contraintes_distance(Tissu_t* tissu, float_t compliance, float_t dt) {

    float_t alpha = compliance/ dt / dt;

    for (int i = 0; i < tissu->nb_contraintes_distance; i++) {
        Contrainte_t contrainte = tissu->contraintes_distance[i];
        Particule_t p1 = tissu->particules[contrainte.p1];
        Particule_t p2 = tissu->particules[contrainte.p2];

        // on calcule le gradient
        vect3D_t grad = sub(tissu->particules[contrainte.p2].position, tissu->particules[contrainte.p1].position);
        float_t longueur_actuelle = Norme_euclidienne(grad);

        float_t w0 = 1 / p2.masse;
        float_t w1 = 1 / p1.masse;

        // On calcule le terme correctif
        float_t correction = ((longueur_actuelle - contrainte.dist) / (longueur_actuelle + 1e-5)) / (w0 + w1 + alpha);


        if (!p1.est_fixe) {
            tissu->particules[contrainte.p1].position = add(tissu->particules[contrainte.p1].position, produit_par_scalaire(grad, w0 * correction));
        }
        if (!p2.est_fixe) {
            tissu->particules[contrainte.p2].position = sub(tissu->particules[contrainte.p2].position, produit_par_scalaire(grad, w1 * correction));
        }

    }
}

void resoudre_contraintes_flexion(Tissu_t* tissu, float_t compliance, float_t dt) {

    float_t alpha = compliance / dt / dt;

    for (int i = 0; i < tissu->nb_contraintes_flexion; i++) {
        Contrainte_t contrainte = tissu->contraintes_flexion[i];
        Particule_t p1 = tissu->particules[contrainte.p1];
        Particule_t p2 = tissu->particules[contrainte.p2];

        vect3D_t delta = sub(tissu->particules[contrainte.p2].position, tissu->particules[contrainte.p1].position);
        float_t longueur_actuelle = Norme_euclidienne(delta);

        float_t w0 = 1 / p2.masse;
        float_t w1 = 1 / p1.masse;
        float_t correction = ((longueur_actuelle - (contrainte.dist*contrainte.dist)) / (longueur_actuelle + 1e-5)) / (w0 + w1 + alpha);

        if (!p1.est_fixe) {
            tissu->particules[contrainte.p1].position = add(tissu->particules[contrainte.p1].position, produit_par_scalaire(delta, w0 * correction));
        }
        if (!p2.est_fixe) {
            tissu->particules[contrainte.p2].position = sub(tissu->particules[contrainte.p2].position, produit_par_scalaire(delta, w1 * correction));
        }

    }
}

plan_t ComputePlane(point_t a, point_t b, point_t c)
{
    plan_t p;
    p.n = normalisation(produit_vectoriel(sub(b, a), sub(c, a)));
    p.d = produit_scalaire(p.n, a);
    return p;
}


float DistPointPlane(point_t q, plan_t p)
{
    return (produit_scalaire(p.n, q) - p.d) / produit_scalaire(p.n, p.n);
}



point_t IntersectSegmentPlane(point_t a, point_t b, plan_t p)
{
    // Calculer le point t où le segement AB coupe le plan
    vect3D_t ab = sub(b, a);
    float_t t = (p.d - produit_scalaire(p.n, a)) / produit_scalaire(p.n, ab);
    // Si t est dans [0..1] on retourne le point d'intersection
    if (t >= 0.0f && t <= 1.0f) {
    return add(a, produit_par_scalaire(ab, t));
    }
    // sinon on renvoie le point de depart
    return a;
}


void resoudre_collisions(Tissu_t* tissu, Model E, int i)
{

    float_t smallestDist = FLT_MAX;
    plan_t h;
    for (int k = 0; k < E.faceCount; k++)
    {

        vect3D_t P[3];
        P[0] = E.vertices[E.faces[k].v1-1];
        P[1] = E.vertices[E.faces[k].v2-1];
        P[2] = E.vertices[E.faces[k].v3-1];
        plan_t p = ComputePlane(P[0], P[1], P[2]);

        float_t dist = DistPointPlane(tissu->particules[i].position, p);

        if(dist > 0.0f) return;
        if(smallestDist > dist){
            smallestDist = dist;
            h = p;
        }

    }

    tissu->particules[i].position = IntersectSegmentPlane(tissu->particules[i].position_prec, tissu->particules[i].position, h);
}

