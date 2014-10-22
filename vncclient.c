#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <netdb.h>

#define BUFLEN 256

int main (int argc, char* argv[])
{
	int serv_sock, port_no;
	struct sockaddr_in serv_addr;
	struct hostent* server;
	
	char buffer[BUFLEN];
	
	if (argc < 3)
	{
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0);
	}
	
	port_no = atoi(argv[2]);
	
	/* Create socket */
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock < 0)
	{
		perror("ERROR creating socket.");
		exit(1);
	}
	
	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}

	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
	
	/* Connect to server */
	if (connect(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("ERROR, could not connect.");
		exit(2);
	}

	printf("Client: ");
	bzero(buffer, BUFLEN);
	fgets(buffer, BUFLEN - 1, stdin);
	
	/* Send message to server */
	int status = write(serv_sock, buffer, strlen(buffer));
	if (status < 0)
	{
		perror("ERROR writing to socket.");
		exit(3);
	}
	
	/* Read server response */
	bzero(buffer, BUFLEN);
	status = read(serv_sock, buffer, BUFLEN - 1);
	if (status < 0)
	{
		perror("ERROR reading from socket.");
		exit(4);
	}
	printf("Server: %s", buffer);
	return 0;
}
