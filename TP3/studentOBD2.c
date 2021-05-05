#include "studentOBD2.h"
#include "utils.h"

/* Compile : make */

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

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
int getValueFromVCan0 (int pid, int *a, int *b) {
    int s, i, nbytes,
        rpm = -1,
        speed = -1; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_filter rfilter[1];
    char id[ID_SIZE];

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket send");
		return -1;

	} /* Open socket */

	/* Use vcan0 */
	strcpy(ifr.ifr_name, "vcan0");
	ioctl(s, SIOCGIFINDEX, &ifr);

	/* Initialize */
	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");

		/* Close connexion */
		closeSocket(s);

		return -1;

	} /* Bind */

    /* Set filter */
    rfilter[0].can_id   = 0x321;
    rfilter[0].can_mask = 0x000;
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    while (keepRunning) {
        nbytes = read(s, &frame, sizeof(struct can_frame));

        if (nbytes < 0) {
            perror("Read");

            /* Close connexion */
            closeSocket(s);

            return -1;
        }
        
        char id[ID_SIZE];

        /* Cast id num to string */
        sprintf(id, "%X", frame.can_id & 0xf);
//TODO throttle
printf("%X\n", frame.can_id);
        if (id[0] == '6') {
            /* C06 case */
            if (frame.can_dlc == 2) {
                rpm = (((frame.data[1] << 8) & 0xff00) | (frame.data[0] & 0x00ff)) / 4;
                *a = frame.data[1];
                *b = frame.data[0];

            } /* Check we receive 2 bytes */
            

        } else if (id[0] == '7') {
            /* C07 case */
            if (frame.can_dlc == 2) {
                speed = frame.data[0];
                *a = frame.data[0];
                *b = -1;

            } /* Check we receive 2 bytes */

        }

        if (PID_SPEED == pid) {
            /* Seed case */
            if (speed >= 0) {
                return speed;
            }

        } else if (PID_RPM == pid) {
            /* RPM case */
            if (rpm >= 0) {
                return rpm;
            }
        }

    } /* Loop until stop */

	/* Close connexion */
    closeSocket(s);

    return -1;

} /* getValueFromVCan0 */

int main(int argc, char **argv)
{
	int s, i, nbytes,
        rpm = 0,
        speed = 0,
        gear = 0; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_filter rfilter[1];
    
    printf("Student OBD2\r\n");

    /* Catch ctr+c */
    signal(SIGINT, intHandler);

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket send");
		return 1;

	} /* Open socket */

	/* Use vcan0 */
	strcpy(ifr.ifr_name, "vcan1");
	ioctl(s, SIOCGIFINDEX, &ifr);

	/* Initialize */
	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");

		/* Close connexion */
		closeSocket(s);

		return 1;

	} /* Bind */

    /* Set filter */
    rfilter[0].can_id   = 0x7DF;
    rfilter[0].can_mask = 0xFFF;
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    while (keepRunning) {
        nbytes = read(s, &frame, sizeof(struct can_frame));

        if (nbytes < 0) {
            perror("Read");

            /* Close connexion */
            closeSocket(s);

            return 1;
        }
        
        if (frame.can_dlc == 8) {
            /* Get PID */
            int pid = frame.data[2],
                a   = -1,
                b   = -1;

            int value = getValueFromVCan0(pid, &a, &b);

            if (value == -1) {
                printf("Une erreur est survenue.\r\n");
                return 1;
            }

            /* Set id of message */
            frame.can_id = 0x7E8;
            /* Set size in bytes of data */
            frame.can_dlc = 8;
            /* Set data */
            frame.data[1] = 41;
            frame.data[2] = pid;

            if (PID_SPEED == pid) {
                /* Seed case */
                /* Set data */
                frame.data[0] = 3;
                frame.data[3] = a;
                frame.data[4] = 0xAA;
                frame.data[5] = 0xAA;
                frame.data[6] = 0xAA;
                frame.data[7] = 0xAA;

            } else if (PID_RPM == pid) {
                /* RPM case */
                /* Set data */
                frame.data[0] = 4;
                frame.data[3] = a;
                frame.data[4] = b;
                frame.data[5] = 0xAA;
                frame.data[6] = 0xAA;
                frame.data[7] = 0xAA;

            }

            /* Send can message */
            sendMessage(s, frame);

        } /* Check we receive 7 bytes */
            
    } /* Loop until stop */

	/* Close connexion */
    closeSocket(s);

	return 0;
}