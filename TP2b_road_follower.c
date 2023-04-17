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
    int retour = 0;
    int i; 
	struct sockaddr_can addr;
	struct ifreq ifr;


	if (strcpy(ifr.ifr_name, "vcan0" ) == NULL)
    {
        retour = 1;
    };

	ioctl(s, SIOCGIFINDEX, &ifr);

	if (memset(&addr, 0, sizeof(addr)) == NULL)
    {
        retour = 1;
    };
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");
		retour =  1;
	}

	if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("Write");
		retour = 1;
	}

	/*if (close(s) < 0) {
		perror("Close");
		return 1;
	}*/

    return retour;
}


int getValeur(struct can_frame *frame, struct can_filter *rfilter, int s)
{
    int retour = 0;
    int i;
    int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;


    if (strcpy(ifr.ifr_name, "vcan0" ) == NULL)
    {
        retour = 1;
    };

    ioctl(s, SIOCGIFINDEX, &ifr);

    if (memset(&addr, 0, sizeof(addr)) == NULL)
    {
        retour = 1;
    };
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        retour = 1;
    }


    /*rfilter[1].can_id   = 0xC07;
    rfilter[1].can_mask = 0xFFF;*/

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, rfilter, sizeof(rfilter));

    //printf("CAN Sockets Receive Filter Demo\r\n");
    nbytes = read(s, frame, sizeof(struct can_frame));

    return retour;
}

int getValeurGauche(struct can_frame *frame, int s)
{
    int retour = 0;
    struct can_filter rfilter[1];

    rfilter[0].can_id   = 0xC01;
    rfilter[0].can_mask = 0xFFF;

    retour = getValeur(frame,rfilter,s);

    return retour;

}

int getValeurDroit(struct can_frame *frame, int s)
{
    int retour = 0;
    struct can_filter rfilter[1];

    rfilter[0].can_id   = 0xC04;
    rfilter[0].can_mask = 0xFFF;

    retour = getValeur(frame,rfilter,s);

    return retour;
}

int getVitesse(struct can_frame *frame, int s)
{
    int retour = 0;
    struct can_filter rfilter[1];

    rfilter[0].can_id   = 0xC07;
    rfilter[0].can_mask = 0xFFF;

    retour = getValeur(frame,rfilter,s);

    return retour;
}

int getFullLeft(struct can_frame *frame, int s)
{
    int retour = 0;
    struct can_filter rfilter[1];

    rfilter[0].can_id   = 0xC00;
    rfilter[0].can_mask = 0xFFF;

    retour = getValeur(frame,rfilter,s);

    return retour;
}

int getFullRight(struct can_frame *frame, int s)
{
    int retour = 0;
    struct can_filter rfilter[1];

    rfilter[0].can_id   = 0xC05;
    rfilter[0].can_mask = 0xFFF;

    retour = getValeur(frame,rfilter,s);

    return retour;
}



int main(int argc, char **argv)   
{

int retour = 0;
int s;

    if (argc != 1)
    {
        perror("Nombre argument incorrect");
        retour = 1;
    }

    if (printf("Début : %s\n",argv[0]) < 0)
    {
        retour = 1;
    }

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s  < 0) 
    {
        perror("Socket");
        retour =  1;
    }
    

    struct can_frame frame;
    struct can_frame frameRec;

    /*Accelerer*/
    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0] = 0x64;
    frame.data[1] = 0x00;
    frame.data[2] = 0x00;
    retour = transmitDataSim(frame,s) | retour;

    sleep(2);

    /*Rouler*/
    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0] = 0x00;
    frame.data[1] = 0x00;
    frame.data[2] = 0x00;
    retour = transmitDataSim(frame,s)| retour;


    int  i = 0;
    while ((i < 2048) && (retour == 0))
    {
        retour =  getVitesse(&frameRec,s)| retour;
        int vitesse = frameRec.data[0];
        retour =  getValeurGauche(&frameRec,s)| retour;
        int valGauche = frameRec.data[0];
        retour =  getValeurDroit(&frameRec,s)| retour;
        int valDroit = frameRec.data[0];
        //printf("%d %d\n",valGauche, valDroit);

        retour =  getFullLeft(&frameRec,s)| retour;
        int fullLeft = frameRec.data[0];
        retour =  getFullRight(&frameRec,s)| retour;
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


        if (((valGauche < 75) || (fullLeft < 50)) && (valDroit > valGauche) && (fullRight > fullLeft) )
        {
            //printf("Droit \n");
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[2] = 0xED;
        }
        else if (((valDroit < 80) || (fullRight < 50)) && (valDroit < valGauche) &&  (fullRight < fullLeft))
        {
            //printf("Gauche \n");
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[2] = 0x12;
        }
        else if ((valGauche < 80) || (fullLeft < 50))
        {
            //printf("Droit \n");
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[2] = 0xED;
        }
        else if ((valDroit < 80) || (fullRight < 40))
        {
            //printf("Gauche \n");
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[2] = 0x12;
        }
        else
        {
            //printf("la \n");
            frame.can_id = 0x321;
            frame.can_dlc = 3;
            frame.data[2] = 0x00; 
        }
        retour = transmitDataSim(frame,s)| retour;

        i++;

    }
    return 0; 
}