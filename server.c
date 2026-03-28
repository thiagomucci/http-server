#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <malloc.h>
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
	memset(&servaddr, 0, sizeof(servaddr));

	int port = strtol(argv[1], NULL, 10);
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
		printf("waiting for client");
		int clifd = accept(sockfd, NULL, NULL);
		if(clifd < 0)
		{
			perror("accept");
			exit(1);
		}
		if( (n = read(clifd, buf, sizeof(buf) -1)) > 0)
		{
			buf[n] = '\0';
		}

		char method[16];
		char path[256];
		sscanf(buf, "\%s \%s", method, path);

		if(strcmp(path, "/") == 0)
		{
			strcpy(path, "/index.html");
		}
		int file_fd = open(path + 1, O_RDONLY);
		if(file_fd != -1 )
		{
			long fsize = lseek(file_fd, 0, SEEK_END);
    		lseek(file_fd, 0, SEEK_SET);

			char *fbuffer = malloc(fsize);

			read(file_fd, fbuffer, fsize);
			close(file_fd);

			char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
			write(clifd, header, strlen(header));

			write(clifd, fbuffer, fsize);

			free(fbuffer);
		}
		else
		{
			char *error_404 = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
    write(clifd, error_404, strlen(error_404));
		}
	close(clifd);
	}
return 0;
}
