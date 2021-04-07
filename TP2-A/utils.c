#include "utils.h"

/* Compile : gcc vehicule_checker_student.c -o vehicule_checker_student */

void closeSocket(int s) {
	if (close(s) < 0) {
		perror("Close");

	} /* if error */

} /* Close connexion */

/**
 * @brief
 * Permet de savoir si nous avons recu tous les IDs
 * 
 * @param
 * table : Tableau des IDs contenant 0 ou 1 pour chaque ID (1 trouvé, 0 non)
 * size : Taille du tableau
 * 
 * @return
 * 0 si nous n'avons pas recu tous les IDs sinon 1
 **/
int checkReceive(int *table, int size) {
    for (int i = 0; i < size; i++) {
        if (table[i] == 0) {
            return 0;
        }
    } /* Loop on table */

    return 1;

} /* Check we receive all IDs */

/**
 * @brief
 * Permet d'ajouter l'ID recu a notre tableau des IDs
 * 
 * @param
 * table : Tableau des IDs contenant 0 ou 1 pour chaque ID (1 trouvé, 0 non)
 * ids : Tableau des IDs déjà initialisé => C01 par exemple
 * size : Taille du tableau => identique pour les deux
 * id_num : id recu
 * 
 **/
void addIdReceive(int *table, char (*ids)[ID_SIZE], int size, canid_t id_num) {
    char id[ID_SIZE];

    /* Cast id num to string */
    sprintf(id, "%X", id_num & 0xfff);

    for (int i = 0; i < size; i++) {
        if (strcmp(id, ids[i]) == 0) {
            table[i] = 1;

            printf("Receive : %s\n\r", id);

            return;

        } /* ID found */

    } /* Loop on ID table */

} /* Add ID to the table receive */


/**
 * 
 * @brief
 * Permet d'envoyer un message CAN
 * 
 * @param
 * s : Socket
 * frame : Can Frame
 * 
 * @return
 * Retourne 1 en cas d'erreur, sinon 0
 * 
 **/
int sendMessage(int s, struct can_frame frame) {

    if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Write");

        /* Close connexion */
        closeSocket(s);
        
        return 1;

    } /* Send message */

    return 0;

} /* sendMessage */
