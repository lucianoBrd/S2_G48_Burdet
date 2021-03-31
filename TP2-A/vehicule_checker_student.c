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
 * @params
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
 * @params
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

int main(int argc, char **argv)
{
	int s, i, nbytes, receive_id[RECEIVE_ID_SIZE]; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_filter rfilter[1];
    char    ids[RECEIVE_ID_SIZE][ID_SIZE],
            id[ID_SIZE];
    
    printf("TP2-A\r\n");

    /* Initialyze IDs */
    strcpy(id, "C0"); /* Begin ID constant */
    for (i = 0; i < RECEIVE_ID_SIZE; i++) {
        char    id_tmp[ID_SIZE],
                num = i + '0';
        
        /* Copy begin const in char tmp */
        strcpy(id_tmp, id);
        /* Add number at the end of tmp */
        strncat(id_tmp, &num, 1);
        /* Copy tmp in table of IDs */
        strcpy(ids[i], id_tmp);

    } /* Loop on ID table */

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket send");
		return 1;

	} /* Open socket */

	/* Use vcan0 */
	strcpy(ifr.ifr_name, "vcan0");
	ioctl(s, SIOCGIFINDEX, &ifr);

	/* Initialize */
	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

    memset(&receive_id, 0, sizeof(receive_id));

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");

		/* Close connexion */
		closeSocket(s);

		return 1;

	} /* Bind */

    /* Set filter */
    rfilter[0].can_id   = 0xC00;
    rfilter[0].can_mask = 0xFF0;
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    int result = 0;
    while (result == 0) {
        nbytes = read(s, &frame, sizeof(struct can_frame));

        if (nbytes < 0) {
            perror("Read");

            /* Close connexion */
            closeSocket(s);

            return 1;
        }
        
        /* Add ID */
        addIdReceive(receive_id, ids, RECEIVE_ID_SIZE, frame.can_id);

        /* Check we receive all IDs */
        result = checkReceive(receive_id, RECEIVE_ID_SIZE);              

    } /* Loop until receive all IDs */

    printf("--- Receive All - Ready ---\r\n");

    /* Set id of message */
    frame.can_id = 0x123;
    /* Set size in bytes of data */
    frame.can_dlc = 2;
    /* Set data */
    frame.data[0] = 0;
    frame.data[1] = 1;

    if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Write");

        /* Close connexion */
        closeSocket(s);
        
        return 1;

    } /* Send message */

	if (close(s) < 0) {
		perror("Close");
		return 1;

	} /* Close connexion */

	return 0;
}
