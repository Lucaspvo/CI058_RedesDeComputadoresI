# include "main.h"




int create_sock (const char *device) {

	int rawsock = 0;
	int device_id = 0;
	struct ifreq ifr;
    	struct sockaddr_ll sock_ll;

	if((rawsock = socket(PF_PACKET, SOCK_RAW, 0)) == -1) {
	    perror("socket");
	    exit(1);
	}

	memset(&ifr, 0, sizeof(struct ifreq));
	memcpy(ifr.ifr_name, device, sizeof(device));
	if(ioctl(rawsock, SIOCGIFINDEX, &ifr)==-1) {
	    perror("ioctl");
	    exit(1);
	}
	device_id = ifr.ifr_ifindex;

	memset(&sock_ll, 0, sizeof(sock_ll));
	sock_ll.sll_family = AF_PACKET;
	sock_ll.sll_ifindex = device_id;
	sock_ll.sll_protocol = htons(ETH_P_ALL);
	if(bind(rawsock, (struct sockaddr*)&sock_ll, sizeof(sock_ll)) == -1) {
	    perror("bind");
	    exit(1);
	}

	struct packet_mreq mr;
	memset(&mr, 0, sizeof(mr));
	mr.mr_ifindex = device_id;
	mr.mr_type = PACKET_MR_PROMISC;
	if (setsockopt(rawsock, SOL_PACKET, PACKET_ADD_MEMBERSHIP,
		&mr, sizeof(mr)) == -1) {
			perror("setsockopt");
			exit(1);
	}

	return rawsock;
}
