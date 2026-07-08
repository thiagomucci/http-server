#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXBUF 4096
#define SOCKMAXCONN 10

const char* get_content_type(const char *path)
{
  char *extension = strrchr(path, '.');
  if (extension == NULL) return "application/octet-stream";

  if (strcmp(extension, ".html") == 0) return "text/html";
  if (strcmp(extension, ".css") == 0) return "text/css";
  if (strcmp(extension, ".jpg") || strcmp(extension, ".jpeg") == 0) return "text/jpeg";

  return "application/octet-stream";
}

int setup_server_socket(int port)
{
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("socket");
    exit(1);
  }

  int opt = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_port = htons(port);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("bind");
    exit(1);
  }

  if (listen(sockfd, SOCKMAXCONN) < 0)
  {
    perror("listen");
    exit(1);
  }

  return sockfd;
}

void handle_client(int clifd)
{
  char buf[MAXBUF];
  int n;

  if ( (n = read(clifd, buf, sizeof(buf) -1)) <= 0)
  {
    close(clifd);
    return;
  }
  buf[n] = '\0';

  char method[16];
  char path[256];
  sscanf(buf, "%15s, %255s", method, path);

  if (strstr(path, "..") != NULL)
  {
    char *error_400 = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<html><body><h1>400 Bad Request</h1></body></html>";
    write(clifd, error_400, strlen(error_400));
    close(clifd);
    return;
  }

  if (strcmp(path, "/") == 0)
  {
    strcpy(path, "/index.html");
  }

  const char *content_type = get_content_type(path);
  const header[128];
  snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);

  int file_fd = open(path + 1, O_RDONLY);
  if (file_fd != -1)
  {
    write(clifd, header, strlen(header));
    while ((n = read(file_fd, buf, sizeof(bif))) > 0)
    {
      write(clifd, buf, n);
    }
    close(file_fd);
  } else {
    char *error_404 = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
    write(clifd, error_404, strlen(error_404));
  }
  close(clifd);
}

int main(int argc, char **argv)
{
  if(argc != 2)
  {
    write(STDERR_FILENO, "try: ./prog <port>\n", sizeof("try: ./prog <port>\n") - 1);
    exit(1);
  }

  int port = strtol(argv[1], NULL, 10);
  int sockfd = setup_server_socket(port);

  printf("Servidor rodando na porta %d... aguardando por conexões \n", port);

  while(1)
  {
    int clifd = accept(sockfd, NULL, NULL);
    if (clifd < 0)
    {
      perror("accept");
      continue;
    }

    handle_client(clifd);
  }
  close(sockfd);
  return 0;
}


