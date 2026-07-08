# http server

A lightweight, minimal HTTP/1.1 web server written in C using raw POSIX sockets. This project focuses on understanding network architecture, system calls, memory layout, and web security basics.

Like my other systems projects, this was developed as part of a deep dive into **Advanced Programming in the UNIX Environment (APUE)** and **Unix Network Programming (UNP)**.


## Features

* **Modular Architecture:** Fully refactored into clean, isolated layers (`get_content_type`, `setup_server_socket`, and `handle_client`).
* **Static File Serving:** Supports serving `.html`, `.css`, and `.jpg`/`.jpeg` files dynamically with accurate MIME type headers.
* **Security Middleware:** Built-in mitigation against **Path Traversal Attacks** (`..`) to prevent unauthorized access to the host file system.
* **Robust Socket Configuration:** Uses `SO_REUSEADDR` to bypass the dreaded `Address already in use` kernel cooldown state during server restarts.
* **Safe String Manipulation:** Built using bounded I/O functions (`snprintf`, `%15s` in `sscanf`) to enforce memory boundaries and prevent Buffer Overflows.


## Architectural Layout

```text
  [Client Browser] 
         │  (HTTP Request)
         ▼
   [setup_server_socket] ──► Creates & binds TCP Socket (SO_REUSEADDR)
         │
         ▼
   [accept() Loop] ──────► Spawns active client file descriptor (clifd)
         │
         ▼
   [handle_client] ──────► 1. Bounded read() of HTTP header
         │                 2. Path Traversal security validation (strstr)
         │                 3. Route resolution & MIME matching
         ▼
    [File System] ───────► Opens local static asset (O_RDONLY) ──► Streams to clifd
````
## Build

Compile using any standard C compiler with strict diagnostic flags:

````bash
cc -Wall -Wextra -pedantic server.c -o mini-httpd
````
## Usage

Start the server by passing a custom port as an argument:
````bash
./mini-httpd <port>
````
## Example
````bash
$ ./mini-httpd 8080
Servidor rodando na porta 8080... Aguardando conexões.

Now, place an index.html file in the same directory and visit http://localhost:8080 in your browser or test it via curl:
Bash

curl -i http://localhost:8080/index.html
````
## Edge Case Handling & Security

    404 Not Found: If the requested path does not exist on the disk, the server gracefully handles the failed open() call and streams a raw fallback HTTP 404 block.

    400 Bad Request: If a malicious user inputs paths containing relative steps like ../../etc/passwd, the internal parsing layer traps the signature and closes the connection instantly.

    Default Routing: Accessing the root path / automatically maps and expands to /index.html.

## References

    W. Richard Stevens — Advanced Programming in the UNIX Environment (APUE)

    W. Richard Stevens — Unix Network Programming, Volume 1: The Sockets Networking API (UNP)

    RFC 2616 — Hypertext Transfer Protocol -- HTTP/1.1

````text
 _     _   _                                        
| |   | | | |                                       
| |__ | |_| |_ _ __    ___  ___ _ ____   _____ _ __ 
| '_ \| __| __| '_ \  / __|/ _ \ '__\ \ / / _ \ '__|
| | | | |_| |_| |_) | \__ \  __/ |   \ V /  __/ |   
|_| |_|\__|\__| .__/  |___/\___|_|    \_/ \___|_|   
              | |                                   
              |_|                                   
````



    
