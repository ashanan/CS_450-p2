/*  Author: Avner Shanan
**  Description:
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>

#define PORT "60607"
#define UDPPORT "60608"

#include <iostream>
using namespace std;

int readable_timeout(int fd, int sec){
	fd_set rset;
	struct timeval timeout;

	FD_ZERO(&rset);
	FD_SET(fd, &rset);

	timeout.tv_sec = sec;
	timeout.tv_usec = 0;

	return(select(fd + 1, &rset, NULL, NULL, &timeout));
}

timespec diff(timespec start, timespec end){  //calculate the difference between two timespecs
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

void checkTransmissionErrors(int msgSent, int msgReceived, int serial, int *errors){
	if(msgReceived != msgSent + serial + 1){
		errors++;
		printf("packets lost or dropped: %d", *errors);
	}
}

int main(int argc, char *argv[])
{
	int sockfd, sockUDP, n, rv, numBytes, numPackets, errors = 0;  
	struct addrinfo hints, *serverInfo, *serverInfo2, *clientInfo;
	unsigned long sequenceNumber = 1, retPacket, packet, serial = 529;
	struct timespec transmissionStartTime, transmissionEndTime, sendTime, returnTime, totalTime;
	unsigned long timeDiff;
	bool csv = false;
	//fd_set rset, wset;

	if (argc < 2) {
	    //fprintf(stderr,"usage: client hostname\n");
	    //exit(1);
		argv[1] = "localhost";
	}
	if(argc < 3){
		numPackets = 5;
	}
	else{
		sscanf(argv[2], "%d", &numPackets);
	}
	if(argc == 4){
		csv = true;
	}

	if(!csv){
		printf("Author: Avner Shanan\n");
		printf("CS ID: ashanan@cs.uic.edu\n\n");
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(argv[1], UDPPORT, &hints, &serverInfo2)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		printf("getaddrinfo error\n");
		return 1;
	}
	
	//printf("got address\n");
	
	for(clientInfo = serverInfo2; clientInfo != NULL; clientInfo = clientInfo->ai_next) {
		if ((sockUDP = socket(clientInfo->ai_family, clientInfo->ai_socktype, clientInfo->ai_protocol)) == -1) {
			perror("client: socket");
			printf("socket error\n");
			continue;
		}
		break;
	}

	//printf("got socket\n");

	freeaddrinfo(serverInfo2); // all done with this structure
	if(clientInfo == NULL){
		printf("UDP error: socket\n");
	}	
	
	// Set up select
	//FD_ZERO(&rset);	
	//FD_ZERO(&wset);
	//FD_SET(sockfd, &rset);
	//FD_SET(sockfd, &wset);		
	//maxfdp1 = sockfd + 1;	

	printf("-------Sending UDP-------\n");

	for(sequenceNumber = 1;sequenceNumber < numPackets;sequenceNumber++){
		if(sequenceNumber == 1){ //first transmission, record time
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &transmissionStartTime);
			sendTime.tv_sec = transmissionStartTime.tv_sec;
			sendTime.tv_nsec = transmissionStartTime.tv_nsec;
		}
		else{
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &sendTime);
		}
		
		printf("sending:%ld\n", sequenceNumber);
		numBytes = sendto(sockUDP, (char *)&sequenceNumber, sizeof(long), 0, clientInfo->ai_addr, clientInfo->ai_addrlen);
		printf("sent %d bytes\n", numBytes);
		//numBytes = recvfrom(sockUDP, (char *)&retPacket, sizeof(long), 0, clientInfo->ai_addr, &clientInfo->ai_addrlen);
		//printf("rcvd %d bytes, msg: %ld\n", numBytes,retPacket);
		if(readable_timeout(sockUDP,5) == 0){
			printf("UDP socket timeout\n");
		}
		else{
			numBytes = recvfrom(sockUDP, (char *)&retPacket, sizeof(long), 0, clientInfo->ai_addr, &clientInfo->ai_addrlen);
			printf("rcvd %d bytes, msg: %ld\n", numBytes,retPacket);
			if(sequenceNumber == (numPackets - 1)){ //last transmission, record time
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &transmissionEndTime);
				totalTime = diff(sendTime, transmissionEndTime);
				if(csv){
					printf("%ld, %ld\n", retPacket, totalTime.tv_sec*1000000000 + totalTime.tv_nsec);
				}
				else{
					printf("Transmission time: %ld\n", totalTime.tv_sec*1000000000 + totalTime.tv_nsec);
				}
				totalTime = diff(transmissionStartTime, transmissionEndTime);
				if(csv){
					printf("UDP total, %ld\n", totalTime.tv_sec*1000000000 + totalTime.tv_nsec);
				}
				else{
					printf("Total UDP transmission time: %ld\n", totalTime.tv_sec*1000000000 + totalTime.tv_nsec);
				}
			}
			else{
				clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &returnTime);
				totalTime = diff(sendTime, returnTime);
				if(csv){
					printf("%ld, %ld\n", retPacket, totalTime.tv_sec*1000000000 + totalTime.tv_nsec);
				}
				else{
					printf("Transmission time: %ld\n", totalTime.tv_sec*1000000000 + totalTime.tv_nsec);
				}
			}
		
			//checkTransmissionErrors(sequenceNumber, retPacket, serial, &errors);
		}
	}
	return 0;
	

}
