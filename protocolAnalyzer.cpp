#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define PORTUDP "2323"


int main(int argc, char *argv[]){
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo, *temp;

	if(argc < 2){
		argv[1] = "localhost";
	}

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM; // UDP datagram socket
	hints.ai_flags = AI_PASSIVE;   

	if ((status = getaddrinfo(argv[1], PORTUDP, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		printf("Error on address\n");
        return 1;
    }

	// loop through all the results and make a socket
    for(temp = servinfo; temp != NULL; temp = temp->ai_next) {
        if ((sockfd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol)) == -1) {
            perror("protocolAnalyzer: socket");
            continue;
        }

        break;
    }

    if (temp == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }
    if ((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0, p->ai_addr, p->ai_addrlen)) == -1) {
        perror("protocolAnalyzer: sendto");
        exit(1);
    }

	return 0;
}
