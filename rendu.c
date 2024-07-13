/*


██████  ███████ ███    ██ ██████  ██    ██     ██████
██   ██ ██      ████   ██ ██   ██ ██    ██    ██
██████  █████   ██ ██  ██ ██   ██ ██    ██    ██
██   ██ ██      ██  ██ ██ ██   ██ ██    ██    ██
██   ██ ███████ ██   ████ ██████   ██████  ██  ██████


*/

#include "rendu.h"

void initialiser_fenetre(int largeur, int hauteur) {
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    gluPerspective(45, (double)largeur / (double)hauteur, 0.1, 50.0);
    glTranslatef(0.0, 0.0, -10);

    
}


void dessiner_sol_infini() {
    // Couleur du sol (par exemple, gris foncé)
    glColor3f(0.69, 0.82, 0.83);

    // Dessiner le plan du sol
    glBegin(GL_QUADS);
    glVertex3f(-100.0, 0.0, -100.0); // Coin inférieur gauche
    glVertex3f(-100.0, 0.0, 100.0);  // Coin supérieur gauche
    glVertex3f(100.0, 0.0, 100.0);   // Coin supérieur droit
    glVertex3f(100.0, 0.0, -100.0);  // Coin inférieur droit
    glEnd();
}

// ajouter un vecteur soleil pour la lumière
vect3D_t soleil = { 0.0, 1, 1 };


void dessiner_tissu(Tissu_t* tissu) {
    glColor3f(1, 0.79, 0.83);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < tissu->nb_triangles/3; i++) {
        
        
        int v1 = tissu->triangles[3*i];
        int v2 = tissu->triangles[3*i+1];
        int v3 = tissu->triangles[3*i+2];
        // calculant la normale du triangle
        vect3D_t n = normalisation(produit_vectoriel(sub(tissu->particules[v2].position, tissu->particules[v1].position), sub(tissu->particules[v3].position, tissu->particules[v1].position)));
        // calculant l'intensité de la lumière
        float_t intensite = produit_scalaire(n, normalisation(soleil));
        // si l'intensité est négative, on multiplie par -1
        if (intensite < 0) {
            intensite *= -1;
        }
        if (intensite == 0) {
            intensite = 0.1;
        }
        // on modifie la couleur du triangle en fonction de l'intensité en faisant un produit 
        // entre la couleur du tissu et l'intensité
        glColor3f(1 * intensite, 0.79 * intensite, 0.83 * intensite);

        glVertex3f(tissu->particules[v1].position.x, tissu->particules[v1].position.y, tissu->particules[v1].position.z);
        glVertex3f(tissu->particules[v2].position.x, tissu->particules[v2].position.y, tissu->particules[v2].position.z);
        glVertex3f(tissu->particules[v3].position.x, tissu->particules[v3].position.y, tissu->particules[v3].position.z);
    }
    glEnd();
}



void mettre_a_jour_position_camera(int largeur_fenetre, int hauteur_fenetre, int translation_x, int translation_y, int distance_camera, int angle_pitch_camera, int angle_yaw_camera) {
    glLoadIdentity();
    gluPerspective(45, (GLfloat)largeur_fenetre / (GLfloat)hauteur_fenetre, 0.1, 50.0);
    glTranslatef(translation_x, translation_y, -distance_camera);
    glRotatef(angle_pitch_camera, 1, 0, 0);
    glRotatef(angle_yaw_camera, 0, 1, 0);
}

void nettoyer(SDL_Window* window) {

    // Fermeture de la fenêtre et nettoyage de SDL
    SDL_GL_DeleteContext(SDL_GL_GetCurrentContext());
    SDL_DestroyWindow(window);
    SDL_Quit();
}





void loadObj(const char* filename, Model* model) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v') {
            model->vertexCount++;
        } else if (line[0] == 'f') {
            model->faceCount++;
        }
    }

    model->vertices = (Vertex*)malloc(model->vertexCount * sizeof(Vertex));
    model->faces = (Face*)malloc(model->faceCount * sizeof(Face));

    fseek(file, 0, SEEK_SET);

    int vertexIndex = 0, faceIndex = 0;
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v') {
            sscanf(line, "v %f %f %f", &model->vertices[vertexIndex].x, &model->vertices[vertexIndex].y, &model->vertices[vertexIndex].z);
            vertexIndex++;
        } else if (line[0] == 'f') {
            sscanf(line, "f %d %d %d", &model->faces[faceIndex].v1, &model->faces[faceIndex].v2, &model->faces[faceIndex].v3);
            faceIndex++;
        }
    }

    fclose(file);
}


void drawObj(Model* model) {
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < model->faceCount; ++i) {
        glColor3f(156, 67, 18);
        int v1 = model->faces[i].v1 - 1;
        int v2 = model->faces[i].v2 - 1;
        int v3 = model->faces[i].v3 - 1;

        

        glVertex3f(model->vertices[v1].x, model->vertices[v1].y, model->vertices[v1].z);
        glVertex3f(model->vertices[v2].x, model->vertices[v2].y, model->vertices[v2].z);
        glVertex3f(model->vertices[v3].x, model->vertices[v3].y, model->vertices[v3].z);
    }
    glEnd();
}


