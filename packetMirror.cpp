/*  Author: Avner Shanan
	Description: 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "60607"
#define UDPPORT "60608"

#include <iostream>
using namespace std;

int max(int a, int b){
	if(a > b)
		return a;
	else return b;
}

int main(void){
    struct sockaddr_storage client_addr, their_addr;
    socklen_t addr_size;
    struct addrinfo hints, hintsUDP, *res, *resUDP;
    int socketFileDescriptor, connectionFileDescriptor, sockUDP, status, n, maxfdp1;
    char buffer[256];
	unsigned long int serial = 529, recvPacket, retPacket;
	fd_set rset;
	struct timeval timeout;

	printf("Author: Avner Shanan\n");
	printf("CS ID: ashanan@cs.uic.edu\n\n");
	printf("Starting server...\nListening on port %s....\n",PORT);
	printf("Serial # %ld...\n", serial);

	// Set up UDP socket
	memset(&hintsUDP, 0, sizeof(hintsUDP));
    hintsUDP.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hintsUDP.ai_socktype = SOCK_DGRAM;
	
	printf("set up hints\n");	
	if ((status = getaddrinfo(NULL, UDPPORT, &hintsUDP, &resUDP)) != 0) {
	    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
	    return 1;
	}
	
	printf("getaddrinfo ok\n");	
	if((sockUDP = socket(resUDP->ai_family, resUDP->ai_socktype, resUDP->ai_protocol)) == -1){
		//socket error 
		printf("socket error");
	}	
	printf("socket ok\n");	
	 if (bind(sockUDP, resUDP->ai_addr, resUDP->ai_addrlen) == -1) {
        close(sockUDP);
        perror("listener: bind");
		printf("error on bind\n");
    }
	printf("bind ok\n");	

	addr_size = sizeof client_addr;
	socklen_t udpClilen = sizeof(their_addr);
	printf("socklen fine\n");

	while(1){
		printf("receiving UDP\n");
	
		if((n = recvfrom(sockUDP, (char *)&recvPacket, sizeof(long), 0, resUDP->ai_addr, &udpClilen)) == -1){
					printf("error on recvFrom");
		}
		printf("rcvd: %ld\n",recvPacket);
		retPacket = recvPacket + serial + 1;
		printf("Here is the return message: %ld\n",retPacket);
		n = sendto(sockUDP, (char *)&retPacket, sizeof(long), 0, (struct sockaddr *)&their_addr, udpClilen);
		printf("sent: %ld\n",retPacket);
	}//end while loop
	//close(connectionFileDescriptor);
	//close(socketFileDescriptor);
}
