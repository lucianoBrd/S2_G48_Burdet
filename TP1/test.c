
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

/* Compile : gcc test.c -o test */

void closeSocket(int s) {
	if (close(s) < 0) {
		perror("Close");

	} /* if error */

} /* Close connexion */

int main(int argc, char **argv)
{
	int s, nbytes; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_filter rfilter[1];

	printf("TP1\r\n");

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

	if (fork() == 0) {
		/* Set id of message */
		frame.can_id = 0x8123;
		/* Set size in bytes of data */
		frame.can_dlc = 8;
		/* Set data */
		sprintf(frame.data, "8 bytes");

		while (1) {
			if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
				perror("Write");

				/* Close connexion */
				closeSocket(s);
				
				return 1;

			} /* Send message */

		
		} /* Loop until SIGINT */
		
	} else {
		/* Set filter */
		rfilter[0].can_id   = 0x100;
		rfilter[0].can_mask = 0xF00;
		setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

		while (1) {
			nbytes = read(s, &frame, sizeof(struct can_frame));

			if (nbytes < 0) {
				perror("Read");

				/* Close connexion */
				closeSocket(s);

				return 1;
			}

			printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);

			for (int i = 0; i < frame.can_dlc; i++)
				printf("%02X ",frame.data[i]);

			printf("\r\n");

		
		} /* Loop until SIGINT */

	}

	if (close(s) < 0) {
		perror("Close");
		return 1;

	} /* Close connexion */

	return 0;
}
