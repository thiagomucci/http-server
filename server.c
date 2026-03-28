#include <stdin.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAXBUF 4096

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		write(STDERR_FILENO, "try: ./prog <port>", sizeof("try: ./prog <port>") -1);
	}
	char buf[MAXBUF];
	int n, sockfd;
	struct sockaddr_in servaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));

	int port = strtol(argv[1]);
	servaddr.sin_port = htons(port);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("socket");
		exit(1);
	}

	if( bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("bind");
		exit(1);
	}

	if( listen(sockfd, 1) < 0)
	{
		perror("listen");
		exit(1);
	}

	while(1)
	{
		int clifd = accept(sockfd, NULL, NULL);
		if(clifd < 0)
		{
			perror("accept");
			exit(1);
		}

	}
}
