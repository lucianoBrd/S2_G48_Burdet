#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#define RECEIVE_ID_SIZE 8
#define ID_SIZE 4

/**
 * 
 * @brief
 * Permet d'envoyer un message CAN
 * 
 * @params
 * s : Socket
 * frame : Can Frame
 * 
 * @return
 * Retourne 1 en cas d'erreur, sinon 0
 * 
 **/
int sendMessage(int s, struct can_frame frame);

/**
 * @brief
 * Permet d'ajouter l'ID recu a notre tableau des IDs
 * 
 * @params
 * table : Tableau des IDs contenant 0 ou 1 pour chaque ID (1 trouvé, 0 non)
 * ids : Tableau des IDs déjà initialisé => C01 par exemple
 * size : Taille du tableau => identique pour les deux
 * id_num : id recu
 * 
 **/
void addIdReceive(int *table, char (*ids)[ID_SIZE], int size, canid_t id_num);

/**
 * @brief
 * Permet de savoir si nous avons recu tous les IDs
 * 
 * @params
 * table : Tableau des IDs contenant 0 ou 1 pour chaque ID (1 trouvé, 0 non)
 * size : Taille du tableau
 * 
 * @return
 * 0 si nous n'avons pas recu tous les IDs sinon 1
 **/
int checkReceive(int *table, int size);

void closeSocket(int s);

#endif