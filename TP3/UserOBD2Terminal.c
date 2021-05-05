#include "UserOBD2Terminal.h"
#include "utils.h"

/* Compile : make */

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

int main(int argc, char **argv)
{
	int s, i, nbytes,
        rpm = 0,
        speed = 0,
        gear = 0,
        camera[CAMERA_SIZE]; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_filter rfilter[1];
    char    id[ID_SIZE],
            direction[3];
    
    printf("Dashboard\r\n");

    /* Catch ctr+c */
    signal(SIGINT, intHandler);

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

    while (keepRunning) {
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

        if (id[0] == '6') {
            /* C06 case */
            if (frame.can_dlc == 2) {
                rpm = ((frame.data[1] << 8) & 0xff00) | (frame.data[0] & 0x00ff);

            } /* Check we receive 2 bytes */
            

        } else if (id[0] == '7') {
            /* C07 case */
            if (frame.can_dlc == 2) {
                speed = frame.data[0];
                gear = frame.data[1];

            } /* Check we receive 2 bytes */

        } else {
            /* C00 to C05 */
            if (frame.can_dlc > 1) {
                camera[(int)id[0] - 48] = frame.data[0]; /* Convert ASCII to int */
                
            } /* Check we receive min 1 bytes */
        }

        int pos = findDirection(camera);
        if (pos < 2) {
            /* Left */
            strcpy(direction, "<-");

        } else if (pos > 3) {
            /* Right */
            strcpy(direction, "->");

        } else {
            /* Straigth */
            strcpy(direction, "^");

        }

        printf("Speed: %d km/h\nGear: %d\nMotor speed: %d rpm\nAction to follow the road: %s\n\n\r", speed, gear, rpm, direction); 

    } /* Loop until stop */

	/* Close connexion */
    closeSocket(s);

	return 0;
}