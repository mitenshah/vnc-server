#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFLEN 256
#define MAX_CLIENTS 5

void process (int sock);

int main (int argc, char* argv[])
{
	int serv_sock, port_no, cli_sock;
	struct sockaddr_in serv_addr, cli_addr;
	
	/* Create socket */
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock < 0)
	{
		perror("ERROR creating socket.");
		exit(1);
	}
	
	/* Initialize socket structure */
	bzero((char*) &serv_addr, sizeof(serv_addr));
	port_no = 5900;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port_no);
	
	/* Bind the address to the socket */
	if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("ERROR binding socket.");
		exit(2);
	}

	/* Wait for incoming connection */
	listen(serv_sock, MAX_CLIENTS);  
	int cli_addr_len = sizeof(cli_addr);

	while (1)
	{
		/* Accept incoming connection */
		cli_sock = accept(serv_sock, (struct sockaddr*) &cli_addr, &cli_addr_len);
		if (cli_sock < 0)
		{
			perror("ERROR accepting connection.");
			exit(3);
		}
	
		/* Create child process */
		int pid = fork();
		if (pid < 0)
		{
			perror("ERROR on fork.");
			exit(4);
		}

		if (pid == 0)
		{
			/* Client process */
			close(serv_sock);
			process(cli_sock);
			exit(0);
		}
		
		else
		{
			/* Server process */
			close(cli_sock);
		}

	}
	return 0;
}

void process(int sock)
{
	int status;
	char buffer[BUFLEN];
	bzero(buffer, BUFLEN);
	
	status = read(sock, buffer, BUFLEN - 1);
	if (status < 0)
	{
		perror("ERROR reading from socket.");
		exit(4);
	}
	
	printf("Client: %s", buffer);

	bzero(buffer, BUFLEN);

	printf("Server: ");
	fgets(buffer, BUFLEN - 1, stdin);
	status = write(sock, buffer, strlen(buffer));
	if (status < 0)
	{
		perror("ERROR writing into socket.");
		exit(5);
	}
}
