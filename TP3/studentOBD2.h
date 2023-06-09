#ifndef STUDENT_OBD2_H
#define STUDENT_OBD2_H

#include <signal.h>

/**
 * 
 * @brief
 * Permet de récupérer la valeur de la vitesse ou rpm en fonction du PID
 * 
 * @param
 * pid : PID de OBD2
 * a : Pointeur pour retourner le premier octet
 * b : Pointeur pour retourner le second octet
 * 
 * @return
 * Retourne -1 en cas d'erreur, sinon la valeur souhaitée
 * 
 **/
int getValueFromVCan0 (int pid, int *a, int *b);

#endif