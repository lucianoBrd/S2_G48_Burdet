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
        send = 0; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_filter rfilter[1];
    char    id[ID_SIZE];
    
    printf("User OBD2 Terminal\r\n");

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
    rfilter[0].can_id   = 0x7E8;
    rfilter[0].can_mask = 0xFFF;
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    while (keepRunning) {

        if (send == 0) {
            /* Send case */
            printf("Enter 1, 2 or 3 in order to know :\n1. - Vehicle Speed -\n2. - Engine RPM -\n3. - Throttle position -\n");
            int input = -1;
            /* Get user input */
            scanf("%d", &input);
            printf("\r\n");

            if (input >= 1 && input <= 3) {

                int pid; 

                if (input == 1) {
                    pid = PID_SPEED;
                } else if (input == 2) {
                    pid = PID_RPM;
                } else {
                    pid = PID_SPEED;
                }

                /* Set id of message */
                frame.can_id = 0x7DF;
                /* Set size in bytes of data */
                frame.can_dlc = 8;
                /* Set data */
                frame.data[0] = 2;
                frame.data[1] = 1;
                frame.data[2] = pid;
                frame.data[3] = 55;
                frame.data[4] = 55;
                frame.data[5] = 55;
                frame.data[6] = 55;
                frame.data[7] = 55;

                /* Send can message */
                sendMessage(s, frame);
                send = 1;

            } else {
                printf("Wrong input! try Again.\n");

            }

        } else {
            /* Receive case */
            nbytes = read(s, &frame, sizeof(struct can_frame));

            if (nbytes < 0) {
                perror("Read");

                /* Close connexion */
                closeSocket(s);

                return 1;
            }
            
            if (frame.can_dlc == 8) {
                printf("0x%03X [%d] ", frame.can_id, frame.can_dlc);

                /* Display what we receive */
                for (i = 0; i < frame.can_dlc; i++)
                    printf("%02X ",frame.data[i]);

                printf("\nFor OBD2 converter : ");

                for (i = 1; i < frame.can_dlc; i++)
                    printf("%02X",frame.data[i]);

                /* Get PID */
                int pid = frame.data[2];
                if (PID_SPEED == pid) {
                    printf("\n%d km/h", frame.data[3]);

                } else if (PID_RPM == pid) {
                    int rpm = (frame.data[3] * 256 + frame.data[4]) / 4;
                    printf("\n%d rpm", rpm);
                    
                }

                printf("\r\n\n");

                send = 0;
            }

        }

    } /* Loop until stop */

	/* Close connexion */
    closeSocket(s);

	return 0;
}