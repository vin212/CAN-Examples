#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int transmitDataSim(struct can_frame frame, int s)
{
    int i; 
	struct sockaddr_can addr;
	struct ifreq ifr;


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

	/*if (close(s) < 0) {
		perror("Close");
		return 1;
	}*/
}


int getValeur(struct can_frame *frame, struct can_filter *rfilter, int s)
{
    int i;
    int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;


    strcpy(ifr.ifr_name, "vcan0" );
    ioctl(s, SIOCGIFINDEX, &ifr);

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        return 1;
    }


    /*rfilter[1].can_id   = 0xC07;
    rfilter[1].can_mask = 0xFFF;*/

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, rfilter, sizeof(rfilter));

    //printf("CAN Sockets Receive Filter Demo\r\n");
    nbytes = read(s, frame, sizeof(struct can_frame));

    return 0;
}

int getValeurGauche(struct can_frame *frame, int s)
{
    struct can_filter rfilter[1];

    rfilter[0].can_id   = 0xC01;
    rfilter[0].can_mask = 0xFFF;

    getValeur(frame,rfilter,s);

    return 0;

}

int getValeurDroit(struct can_frame *frame, int s)
{
    struct can_filter rfilter[1];

    rfilter[0].can_id   = 0xC04;
    rfilter[0].can_mask = 0xFFF;

    getValeur(frame,rfilter,s);

    return 0;
}

int getVitesse(struct can_frame *frame, int s)
{
    struct can_filter rfilter[1];

    rfilter[0].can_id   = 0xC07;
    rfilter[0].can_mask = 0xFFF;

    getValeur(frame,rfilter,s);

    return 0;
}

int getFullLeft(struct can_frame *frame, int s)
{
    struct can_filter rfilter[1];

    rfilter[0].can_id   = 0xC00;
    rfilter[0].can_mask = 0xFFF;

    getValeur(frame,rfilter,s);

    return 0;
}

int getFullRight(struct can_frame *frame, int s)
{
    struct can_filter rfilter[1];

    rfilter[0].can_id   = 0xC05;
    rfilter[0].can_mask = 0xFFF;

    getValeur(frame,rfilter,s);

    return 0;
}



int main(int argc, char **argv)   
{

int s;
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) 
    {
        perror("Socket");
        return 1;
    }

    struct can_frame frame;
    struct can_frame frameRec;

    /*Accelerer*/
    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0] = 0x64;
    frame.data[1] = 0x00;
    frame.data[2] = 0x00;
    transmitDataSim(frame,s);

    sleep(2);

    /*Rouler*/
    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0] = 0x00;
    frame.data[1] = 0x00;
    frame.data[2] = 0x00;
    transmitDataSim(frame,s);


    for (int i = 0 ; i < 2048; i++)
    {
        getVitesse(&frameRec,s);
        int vitesse = frameRec.data[0];
        getValeurGauche(&frameRec,s);
        int valGauche = frameRec.data[0];
        getValeurDroit(&frameRec,s);
        int valDroit = frameRec.data[0];
        printf("%d %d\n",valGauche, valDroit);

        getFullLeft(&frameRec,s);
        int fullLeft = frameRec.data[0];
        getFullRight(&frameRec,s);
        int fullRight = frameRec.data[0];


                
        if (vitesse < 45) 
        {
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[0] = 0x64;
            frame.data[1] = 0x00;
        }
        else if (vitesse > 51)
        {
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[0] = 0x00;
            frame.data[1] = 0x05;
        }
        else
        {
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[0] = 0x00;
            frame.data[1] = 0x00;
        }


        if ((valGauche < 75 || fullLeft < 50) && valDroit > valGauche && fullRight > fullLeft )
        {
            printf("Droit \n");
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[2] = 0xED;
        }
        else if ((valDroit < 80 || fullRight < 50) && valDroit < valGauche &&  fullRight < fullLeft)
        {
            printf("Gauche \n");
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[2] = 0x12;
        }
        else if (valGauche < 80 || fullLeft < 50)
        {
            printf("Droit \n");
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[2] = 0xED;
        }
        else if (valDroit < 80 || fullRight < 40)
        {
            printf("Gauche \n");
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[2] = 0x12;
        }
        else
        {
            printf("la \n");
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[2] = 0x00; 
        }
        transmitDataSim(frame,s);

    }
    return 0; 
}