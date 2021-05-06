#include "sensorsCAN.h"
#include "utils.h"

/* Compile : make */

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

int main(int argc, char **argv)
{
	int s, i, nbytes; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_filter rfilter[1];
    
    printf("Sensors CAN\r\n");

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

    while (keepRunning) {
        nbytes = read(s, &frame, sizeof(struct can_frame));

        if (nbytes < 0) {
            perror("Read");

            /* Close connexion */
            closeSocket(s);

            return 1;
        }
        

            
    } /* Loop until stop */

	/* Close connexion */
    closeSocket(s);

	return 0;
}