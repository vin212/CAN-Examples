
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>


int transmit()
{
    	int s; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;

	printf("CAN Sockets Demo\r\n");

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket");
		return 1;
	}

	strcpy(ifr.ifr_name, "vcan0" );
	ioctl(s, SIOCGIFINDEX, &ifr);

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN ;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");
		return 1;
	}

	frame.can_id = 0x8123 | CAN_EFF_FLAG;
	frame.can_dlc = 8;
	sprintf(frame.data,"HelloM0");

	if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("Write");
		return 1;
	}

	if (close(s) < 0) {
		perror("Close");
		return 1;
	}

    return 0;
}

int receive()
{
    int s, i; 
    int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    int recu = 0;

            printf("CAN Sockets Receive Filter Demo\r\n");

            if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
                perror("Socket");
                return 1;
            }

            strcpy(ifr.ifr_name, "vcan0" );
            ioctl(s, SIOCGIFINDEX, &ifr);

            memset(&addr, 0, sizeof(addr));
            addr.can_family = AF_CAN;
            addr.can_ifindex = ifr.ifr_ifindex;

            if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("Bind");
                return 1;
            }

            struct can_filter rfilter[1];

            rfilter[0].can_id   = 0x000;
            rfilter[0].can_mask = 0xC00;
            //rfilter[1].can_id   = 0x200;
            //rfilter[1].can_mask = 0x700;

            setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

            nbytes = read(s, &frame, sizeof(struct can_frame));

            if (nbytes < 0) {
                perror("Read");
                return 1;
            }

            printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);

            for (i = 0; i < frame.can_dlc; i++)
                printf("%02X ",frame.data[i]);

            printf("\r\n");

            if (close(s) < 0) {
                perror("Close");
                return 1;
            }
    

    return 0;
}

int main(int argc, char **argv)
{
    transmit();

    while (1)
    {
        receive();
    }
    

	return 0;
}
