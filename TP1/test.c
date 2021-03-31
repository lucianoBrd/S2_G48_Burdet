
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

/* Compile : gcc test.c -o test */

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;

} /* Catch SIGINT */

void closeSocket(int s) {
	if (close(s) < 0) {
		perror("Close");

	} /* if error */

} /* Close connexion */

int main(int argc, char **argv)
{
	int s; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;

	/* Catch SIGINT */
	signal(SIGINT, intHandler);

	printf("TP1\r\n");

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket");
		return 1;

	} /*  Open connexion */

	/* Use vcan0 */
	strcpy(ifr.ifr_name, "vcan0" );
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

	/* Set id of message */
	frame.can_id = 0x8123;
	/* Set size in bytes of data */
	frame.can_dlc = 8;
	/* Set data */
	sprintf(frame.data, "8 bytes");

	while (keepRunning) {
		if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
			perror("Write");

			/* Close connexion */
			closeSocket(s);
			
			return 1;

		} /* Send message */

	
	} /* Loop until SIGINT */

	if (close(s) < 0) {
		perror("Close");
		return 1;

	} /* Close connexion */

	return 0;
}
