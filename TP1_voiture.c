
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>


int transmitDataSim(struct can_frame frame)
{
    	int s; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	

	printf("CAN Sockets Demo\r\n");

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

	if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("Write");
		return 1;
	}

	if (close(s) < 0) {
		perror("Close");
		return 1;
	}
}

void allumerPhare()
{
    struct can_frame frame;
    frame.can_id = 0x123;
	frame.can_dlc = 2;
    frame.data[0] = 0x02;
    frame.data[1] = 0x00;
	//sprintf(frame.data, "0");

    transmitDataSim(frame);
}

void clignotantDroit()
{
    struct can_frame frame;
    frame.can_id = 0x123;
	frame.can_dlc = 2;
    frame.data[0] = 0x00;
    frame.data[1] = 0x01;
	//sprintf(frame.data, "0");

    transmitDataSim(frame);
}

void clignotantGacuhe()
{
    struct can_frame frame;
    frame.can_id = 0x123;
	frame.can_dlc = 2;
    frame.data[0] = 0x00;
    frame.data[1] = 0x02;
	//sprintf(frame.data, "0");

    transmitDataSim(frame);
}

void ralentir()
{
    struct can_frame frame;
    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0] = 0x00;
    frame.data[1] = 0x20;
	frame.data[2] = 0x00;
    //sprintf(frame.data, "0");

    transmitDataSim(frame);
}


void allumerFeu()
{
    struct can_frame frame;
    frame.can_id = 0x123;
	frame.can_dlc = 2;
    frame.data[0] = 0x01;
    frame.data[1] = 0x00;
    transmitDataSim(frame);
}

void pleinPhare()
{
    struct can_frame frame;
    frame.can_id = 0x123;
	frame.can_dlc = 2;
    frame.data[0] = 0x02;
    frame.data[1] = 0x00;
    transmitDataSim(frame);
}

void eteintPhare()
{
    struct can_frame frame;
    frame.can_id = 0x123;
	frame.can_dlc = 2;
    frame.data[0] = 0x00;
    frame.data[1] = 0x00;
    transmitDataSim(frame);
}

void tournerAGauche()
{
    struct can_frame frame;
    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0] = 0x64;
    frame.data[1] = 0x00;
    frame.data[2] = 0x64;
    transmitDataSim(frame);
}


void tournerADroite()
{
    struct can_frame frame;
    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0] = 0x64;
    frame.data[1] = 0x00;
    frame.data[2] = 0x7F;
    transmitDataSim(frame);
}


void accelerer()
{
    struct can_frame frame;
    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0] = 0x64;
    frame.data[1] = 0x00;
    frame.data[2] = 0x00;
    transmitDataSim(frame);
}

void freiner()
{
    struct can_frame frame;
    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0] = 0x00;
    frame.data[1] = 0x20;
    frame.data[2] = 0x00;
    transmitDataSim(frame);
}


int main(int argc, char **argv)
{
    /*allumerPhare();
    sleep(3);
    eteindrePhare();
    sleep(3);
    accelerer();
    sleep(3);
    ralentir();*/

/**PPPPP*/
    struct can_frame frame;
    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0] = 0x00;
    frame.data[1] = 0x20;
    frame.data[2] = 0x00;
    transmitDataSim(frame);

    tournerADroite();


    //sprintf(frame.data, "0");

    //transmitDataSim(frame);

	return 0;
}


