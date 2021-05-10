#include "sensorsCAN.h"
#include "utils.h"

/* Compile : make */

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

int main(int argc, char **argv)
{
	int s_v0, s_v2, i, nbytes, receive_id[RECEIVE_ID_SIZE], weight_id[RECEIVE_ID_SIZE], weight; 
	struct sockaddr_can addr_v0, addr_v2;
	struct ifreq ifr_v0, ifr_v2;
	struct can_frame frame_v0;
	struct canfd_frame frame_v2;
	struct can_filter rfilter_v0[1];
	char    ids[RECEIVE_ID_SIZE][ID_SIZE],
            id[ID_SIZE];
    
    printf("Sensors CAN\r\n");

	/* Initialyze IDs */
    strcpy(id, "C0"); /* Begin ID constant */
	weight = 0;
    for (i = 0; i < RECEIVE_ID_SIZE; i++) {
        char    id_tmp[ID_SIZE],
                num = i + '0';
        
        /* Copy begin const in char tmp */
        strcpy(id_tmp, id);
        /* Add number at the end of tmp */
        strncat(id_tmp, &num, 1);
        /* Copy tmp in table of IDs */
        strcpy(ids[i], id_tmp);

		if (i < RECEIVE_ID_SIZE - 2) {
			/* C00 to C05 weight 8 */
			weight_id[i] = weight;
			weight += 8;
		} else {
			/* C06 to C07 weight 2 */
			weight_id[i] = weight;
			weight += 2;
		}

    } /* Loop on ID table */

    /* Catch ctr+c */
    signal(SIGINT, intHandler);

	if ((s_v0 = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket send");
		return 1;

	} /* Open socket */

	if ((s_v2 = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket send");

		/* Close connexion */
		closeSocket(s_v0);

		return 1;

	} /* Open socket */

	/* Use vcan0 */
	strcpy(ifr_v0.ifr_name, "vcan0");
	ioctl(s_v0, SIOCGIFINDEX, &ifr_v0);
	/* Use vcan2 */
	strcpy(ifr_v2.ifr_name, "vcan2");
	ioctl(s_v2, SIOCGIFINDEX, &ifr_v2);

	/* Initialize vcan0 */
	memset(&addr_v0, 0, sizeof(addr_v0));
	addr_v0.can_family = AF_CAN;
	addr_v0.can_ifindex = ifr_v0.ifr_ifindex;
	/* Initialize vcan2 */
	memset(&addr_v2, 0, sizeof(addr_v2));
	addr_v2.can_family = AF_CAN;
	addr_v2.can_ifindex = ifr_v2.ifr_ifindex;

	if (bind(s_v0, (struct sockaddr *)&addr_v0, sizeof(addr_v0)) < 0) {
		perror("Bind");

		/* Close connexion */
		closeSocket(s_v0);
		closeSocket(s_v2);

		return 1;

	} /* Bind */

	if (bind(s_v2, (struct sockaddr *)&addr_v2, sizeof(addr_v2)) < 0) {
		perror("Bind");

		/* Close connexion */
		closeSocket(s_v0);
		closeSocket(s_v2);

		return 1;

	} /* Bind */

    /* Set filter vcan0 */
    rfilter_v0[0].can_id   = 0xC00;
    rfilter_v0[0].can_mask = 0xFF0;
    setsockopt(s_v0, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter_v0, sizeof(rfilter_v0));
	/* Set can fd vcan2 */
	int enable_canfd = 1;
	setsockopt(s_v2, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd, sizeof(enable_canfd));

	memset(&receive_id, 0, sizeof(receive_id));

	/* Set id of message */
    frame_v2.can_id = 0x100;
    /* Set size in bytes of data */
    frame_v2.len = CAN_FD_SIZE;
    
    while (keepRunning) {
        nbytes = read(s_v0, &frame_v0, sizeof(struct can_frame));

        if (nbytes < 0) {
            perror("Read");

            /* Close connexion */
            closeSocket(s_v0);
			closeSocket(s_v2);

            return 1;
        }
        
		/* Add ID */
        addIdReceive(receive_id, ids, RECEIVE_ID_SIZE, frame_v0.can_id);

		char id[ID_SIZE];

        /* Cast id num to string */
        sprintf(id, "%X", frame_v0.can_id & 0xf);
		int num_id = id[0] - 48; /* Convert ASCII to int */

		/* Add data to canfd */
		for (i = 0; i < frame_v0.can_dlc; i++) {
			if (num_id >= 0 && num_id < RECEIVE_ID_SIZE) {
				int index = weight_id[num_id] + i;

				if (index >= 0 && index < CAN_FD_SIZE) {
					frame_v2.data[index] = frame_v0.data[i];

				} /* Check index exist */

			} /* Check id is correct */

		} /* For each bytes receive */

        /* Check we receive all IDs */
        int result = checkReceive(receive_id, RECEIVE_ID_SIZE); 

		if (result == 1) {
			/* Send canfd message */
			if (write(s_v2, &frame_v2, sizeof(struct canfd_frame)) != sizeof(struct canfd_frame)) {
				perror("Write");

				/* Close connexion */
				closeSocket(s_v0);
				closeSocket(s_v2);
				
				return 1;

			} /* Send message */

			printf("Receive all sensors from vcan0 - send CAN-FD on vcan2\n\r");

			memset(&receive_id, 0, sizeof(receive_id));

		} /* End case */
            
    } /* Loop until stop */

	/* Close connexion */
    closeSocket(s_v0);
	closeSocket(s_v2);

	return 0;
}