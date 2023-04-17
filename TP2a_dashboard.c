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

int main(int argc, char **argv)   
{ 
    uint8_t vitesse = 0; 
    uint8_t rapport = 0;
    uint16_t tourMoteur = 0;


    int s, i; 
    int nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    
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

        struct can_filter rfilter[2];

        rfilter[0].can_id   = 0xC06;
        rfilter[0].can_mask = 0xFFF;
        rfilter[1].can_id   = 0xC07;
        rfilter[1].can_mask = 0xFFF;

        setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    while(1)
    {
        //printf("CAN Sockets Receive Filter Demo\r\n");
        nbytes = read(s, &frame, sizeof(struct can_frame));

        if (nbytes < 0) {
            perror("Read");
            return 1;
        }

        if ( (0xC07 | CAN_EFF_FLAG) == frame.can_id )
        {
            //printf("Je suis ici %x \n",frame.can_id);
            vitesse = frame.data[0];
            rapport = frame.data[1];
        }
        else
        {
            tourMoteur = (frame.data[1] << 8) | frame.data[0];
        }

        printf("Vitesse : %3d km -- Rapport : %3d -- tourMoteur : %5d\r",vitesse, rapport,tourMoteur);


    }

    if (close(s) < 0) {
        perror("Close");
        return 1;
    }
}