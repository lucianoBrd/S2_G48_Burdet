#include "dashboard.h"
#include "utils.h"

/* Compile : gcc dashboard.c -o dashboard */

int main(int argc, char **argv)
{
	int s, i, nbytes, receive_id[RECEIVE_ID_SIZE], j; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_filter rfilter[1];
    char    ids[RECEIVE_ID_SIZE][ID_SIZE],
            id[ID_SIZE];
    
    printf("Dashboard\r\n");

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
    rfilter[0].can_id   = 0xC06;
    rfilter[0].can_mask = 0xFFE;
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
        
        printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);             

    } /* Loop until receive all IDs */

	/* Close connexion */
    closeSocket(s);

	return 0;
}
