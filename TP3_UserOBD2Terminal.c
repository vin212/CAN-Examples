#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

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


	if (strcpy(ifr.ifr_name, "vcan1" ) == NULL)
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
    uint16_t tourMoteur = 0;


    if (strcpy(ifr.ifr_name, "vcan1" ) == NULL)
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

    if ((nbytes > 0) && (frame->data[2] == 0x0D))
    {
        printf("la vitesse est de : %d \n", frame->data[3]);
    }
    else if ((nbytes > 0) && (frame->data[2] == 0x0C))
    {

        tourMoteur = (frame->data[4] << 8) | frame->data[3];
        printf("tour moteur : %d \n", tourMoteur);
    }
    else if ((nbytes > 0) && (frame->data[2] == 0x11))
    {
        printf("le throttle est de : %0.2f /100 \n", 1/2.55 * frame->data[3]);
    }

    return retour;
}

int askSpeed(int s)
{
    int retour = 0;
    struct can_frame frame;
    struct can_filter rfilter[1];

    frame.can_id = 0x7DF;
	frame.can_dlc = 3;
    frame.data[0] = 0x02;
    frame.data[1] = 0x01;
    frame.data[2] = 0x0D;
    retour = transmitDataSim(frame,s);

    rfilter[0].can_id   = 0x7E8;
    rfilter[0].can_mask = 0xFFF;

    getValeur(&frame,rfilter,s);
    return retour;

}


int askRPM(int s)
{
    int retour = 0;
    struct can_frame frame;
    struct can_filter rfilter[1];

    frame.can_id = 0x7DF;
	frame.can_dlc = 3;
    frame.data[0] = 0x02;
    frame.data[1] = 0x01;
    frame.data[2] = 0x0C;
    retour = transmitDataSim(frame,s);

    rfilter[0].can_id   = 0x7E8;
    rfilter[0].can_mask = 0xFFF;

    getValeur(&frame,rfilter,s);
    return retour;

}

int askTrhottle(int s)
{
    int retour = 0;
    struct can_frame frame;
    struct can_filter rfilter[1];

    frame.can_id = 0x7DF;
	frame.can_dlc = 3;
    frame.data[0] = 0x02;
    frame.data[1] = 0x01;
    frame.data[2] = 0x11;
    retour = transmitDataSim(frame,s);

    rfilter[0].can_id   = 0x7E8;
    rfilter[0].can_mask = 0xFFF;

    getValeur(&frame,rfilter,s);
    return retour;
}



int main(int argc, char **argv)   
{

    int retour = 0;
    int s;

    if (argc != 2)
    {
        perror("Nombre argument incorrect");
        retour = 1;
    }
    else
    {
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

        if (argv[1][0] == 's')
        {
            askSpeed(s);
        }
        else if (argv[1][0] == 'r')
        {
            askRPM(s);
        }
        else if (argv[1][0] == 't')
        {
            askTrhottle(s);
        }
        else
        {
            printf("Les arguments entré ne sont pas valider \n\
            s => Pour demander la vitesse 's'peed \n\
            r => pour demander les tours par minutes 'R'PM\n\
            t => pour demander le 'T'hrottle \n ");
        }
    }

    return retour;
}



