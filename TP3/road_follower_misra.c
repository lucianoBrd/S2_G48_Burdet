#include "road_follower.h"
#include "utils.h"

/* Compile : make */

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

int main(int argc, char **argv)
{
	int s; 
    int i; 
    int nbytes;
    int speed = 0; 
    int camera[CAMERA_SIZE]; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_filter rfilter[1];
    char    id[ID_SIZE];
    
    printf("Road Follower\r\n");

    /* Catch ctr+c */
    signal(SIGINT, intHandler);

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (s < 0) {
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

    memset(&camera, 0, sizeof(camera));

    while (keepRunning == 1) {
        nbytes = read(s, &frame, sizeof(struct can_frame));

        if (nbytes < 0) {
            perror("Read");

            /* Close connexion */
            closeSocket(s);

            return 1;
        }
        
        char id[ID_SIZE];

        /* Cast id num to string */
        sprintf(id, "%X", frame.can_id & 0xf);

        if (id[0] == '7') {
            /* C07 case */
            if (frame.can_dlc == 2) {
                speed = frame.data[0];

            } /* Check we receive 2 bytes */

        } else if (id[0] != '6') {
            /* C00 to C05 */
            int num_id = id[0] - 48;
            if (frame.can_dlc > 1 && num_id >= 0 && num_id < CAMERA_SIZE) {
                camera[num_id] = frame.data[0]; /* Convert ASCII to int */
                
            } /* Check we receive min 1 bytes */
        }

        /* Set id of message */
        frame.can_id = 0x321;
        /* Set size in bytes of data */
        frame.can_dlc = 3;
        /* Set data */
        frame.data[0] = 40;
        frame.data[1] = 0;
        if (speed > 50) {
            /* Must decrease */
            frame.data[0] = frame.data[0] - 20;
            frame.data[1] = frame.data[1] + 5;

        }

        int pos = findDirection(camera);
        if (pos < 2) {
            /* Left */
            /* Set data */
            frame.data[2] = 10;

        } else if (pos > 3) {
            /* Right */
            /* Set data */
            frame.data[2] = -10;

        } else {
            /* Straigth */
            /* Set data */
            frame.data[2] = 0;

        }

        /* Send can message */
        sendMessage(s, frame);

    } /* Loop until stop */

	/* Close connexion */
    closeSocket(s);

	return 0;
}