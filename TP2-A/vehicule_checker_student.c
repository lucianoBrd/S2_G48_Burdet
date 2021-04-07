
#include "vehicule_checker_student.h"
#include "utils.h"

/* Compile : gcc vehicule_checker_student.c -o vehicule_checker_student */


int main(int argc, char **argv)
{
	int s, i, nbytes, receive_id[RECEIVE_ID_SIZE], j; 
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

    printf("--- Basics check ---\r\n");
    /* Set id of message */
    frame.can_id = 0x123;
    /* Set size in bytes of data */
    frame.can_dlc = 2;

    for (i = 1; i >= 0; i--) {
        for (j = 1; j < 3; j++) {
            /* Set data */
            frame.data[i] = j;

            /* Send can message */
            sendMessage(s, frame);

            /* Wait 3 secondes */
            sleep(3);

        } /* Value 1 and 2 */

        /* Reset */
        frame.data[i] = 0;
        sendMessage(s, frame);

    } /* For each part of the data */

    printf("--- Basics check - Ok ---\r\n");

    printf("--- Drive check ---\r\n");
    /* Set id of message */
    frame.can_id = 0x321;
    /* Set size in bytes of data */
    frame.can_dlc = 3;
    /* Set data */
    frame.data[0] = 70;
    frame.data[1] = 0;
    frame.data[2] = 0;
    /* Send can message */
    sendMessage(s, frame);

    /* Wait 7 secondes */
    sleep(7);

    /* Set data */
    frame.data[0] = 40;
    frame.data[1] = 0;
    frame.data[2] = 21;
    /* Send can message */
    sendMessage(s, frame);

    /* Wait 3 secondes */
    sleep(3);

    /* Set data */
    frame.data[0] = 80;
    frame.data[1] = 0;
    frame.data[2] = 0;
    /* Send can message */
    sendMessage(s, frame);

    /* Wait 5 secondes */
    sleep(5);

    /* Set data */
    frame.data[0] = 0;
    frame.data[1] = 100;
    frame.data[2] = 0;
    /* Send can message */
    sendMessage(s, frame);

    printf("--- Drive check - Ok ---\r\n");

	/* Close connexion */
    closeSocket(s);

	return 0;
}
