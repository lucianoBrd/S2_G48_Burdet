#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <signal.h>

/**
 * 
 * @brief Retourn la position de la valeur la plus grande du tableau
 * 
 * @param
 * camera: Tableau des différentes valeur de caméra (taille : CAMERA_SIZE)
 * 
 * @return 
 * La position de la valeur la plus grande
 * 
 **/
int findDirection(int *camera);

#endif