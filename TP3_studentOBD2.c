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

int sendSpeed(uint8_t valeur, int s)
{
    struct can_frame frame;
    int retour = 0;
    frame.can_id = 0x7E8;
	frame.can_dlc = 4;
    frame.data[0] = 0x03;
    frame.data[1] = 0x41;
    frame.data[2] = 0x0D;
    frame.data[3] = valeur;
    retour = transmitDataSim(frame,s);
    return retour;
}

int sendRPM(uint8_t valeur1, uint8_t valeur2, int s)
{
    struct can_frame frame;
    int retour = 0;
    frame.can_id = 0x7E8;
	frame.can_dlc = 5;
    frame.data[0] = 0x04;
    frame.data[1] = 0x41;
    frame.data[2] = 0x0C;
    frame.data[3] = valeur1;
    frame.data[4] = valeur2;
    retour = transmitDataSim(frame,s);
    return retour;
}

int sendThrottle(uint8_t valeur, int s)
{
    struct can_frame frame;
    int retour = 0;
    frame.can_id = 0x7E8;
	frame.can_dlc = 4;
    frame.data[0] = 0x03;
    frame.data[1] = 0x41;
    frame.data[2] = 0x11;
    frame.data[3] = valeur;
    retour = transmitDataSim(frame,s);
    return retour;
}

int getRPM(uint8_t *valeur1, uint8_t *valeur2,int s)
{
    
    struct can_frame frame;
    struct can_filter rfilter[1];
    int retour = 0;
    int i;
    int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    uint16_t tourMoteur = 0;

    rfilter[0].can_id   = 0xC06;
    rfilter[0].can_mask = 0xFFF;


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
    nbytes = read(s, &frame, sizeof(struct can_frame));

    if (nbytes > 0)
    {
        *valeur1 = frame.data[0];
        *valeur2 = frame.data[1];

        tourMoteur = (frame.data[1] << 8) | frame.data[0];
        printf("tour moteur : %d \n", tourMoteur);
    }
    else
    {
        *valeur1 = 0;
        *valeur2 = 0;
    }

    return retour;
}

int getSpeed(uint8_t *valeur,int s)
{
    
    struct can_frame frame;
    struct can_filter rfilter[1];
    int retour = 0;
    int i;
    int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;

    rfilter[0].can_id   = 0xC07;
    rfilter[0].can_mask = 0xFFF;


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
    nbytes = read(s, &frame, sizeof(struct can_frame));

    if (nbytes > 0)
    {
        *valeur = frame.data[0];
        printf("vitesse : %d \n", frame.data[0]);
    }
    else
    {
        *valeur = -1;
    }

    return retour;
}


int getThrottle(uint8_t *valeur,int s)
{
    
    struct can_frame frame;
    struct can_filter rfilter[1];
    int retour = 0;
    int i;
    int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;

    rfilter[0].can_id   = 0x321;
    rfilter[0].can_mask = 0xFFF;


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

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, rfilter, sizeof(rfilter));

    //printf("CAN Sockets Receive Filter Demo\r\n");
    nbytes = read(s, &frame, sizeof(struct can_frame));

    if (nbytes > 0)
    {
        *valeur = frame.data[0];
        printf("throttle : %0.2f \n", 1/2.55 * frame.data[0]);
    }
    else
    {
        *valeur = -1;
    }

    return retour;
}


int getValeur(struct can_frame *frame, struct can_filter *rfilter, int s)
{
    int retour = 0;
    int i;
    int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    uint8_t valeur;

    uint8_t valeur1;
    uint8_t valeur2;


    


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
        getSpeed(&valeur,s);
        sendSpeed(valeur,s);
    }
    else if ((nbytes > 0) && (frame->data[2] == 0x0C))
    {
        getRPM(&valeur1,&valeur2,s);
        sendRPM(valeur1,valeur2,s);
    }
    else if ((nbytes > 0) && (frame->data[2] == 0x11))
    {
        getThrottle(&valeur,s);
        sendThrottle(valeur,s);
    }

    return retour;
}

int main(int argc, char **argv)   
{

    int retour = 0;
    int s;
    struct can_frame frame;

    struct can_filter rfilter[1];


    rfilter[0].can_id   = 0x7DF;
    rfilter[0].can_mask = 0xFFF;

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

    while(1)
    {
        getValeur(&frame,rfilter,s);
        
    }
}