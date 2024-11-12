#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFERLENGTH 256

/* Function to display error messages and exit */
void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

/* Function to write a message to the socket */
int writeResult(int sockfd, const char *buffer, size_t bufsize) {
    ssize_t n;

    // Send the size of the message
    n = write(sockfd, &bufsize, sizeof(size_t));
    if (n < 0) {
        fprintf(stderr, "ERROR: Writing message size to socket failed\n");
        return -1;
    }

    // Send the actual message
    n = write(sockfd, buffer, bufsize);
    if ((size_t)n != bufsize) {
        fprintf(stderr, "ERROR: Couldn't write %ld bytes, wrote %ld bytes\n", bufsize, n);
        return -1;
    }

    return 0; // Success
}

char *response(int sockfd){
    char buffer[10];
    ssize_t bytes_recieved;
    size_t total = 0;
    char *response = malloc(1);
    response[0] = '\0';

    while ((bytes_recieved = recv(sockfd, buffer, 9, 0)) > 0){
        buffer[bytes_recieved] = '\0';
        total += bytes_recieved;

        response = realloc(response, total + 1);
        if(!response){
            perror("Failed to reallcate memory for response");
            close(sockfd);
            return NULL;
        }
        strcat(response, buffer);
    }
    if (bytes_recieved < 0){
        return NULL;
    }
    return response;

}

/* Function to read a message from the socket */
char *readRes(int sockfd) {
    size_t bufsize;
    ssize_t res;

    // Read the size of the incoming message
    res = read(sockfd, &bufsize, sizeof(size_t));
    if (res != sizeof(size_t)) {
        error("ERROR: Reading message size from socket");
    }

    // Allocate memory for the message
    char *buffer = malloc(bufsize + 1);
    if (!buffer) {
        error("ERROR: Memory allocation for buffer failed");
    }

    // Read the actual message
    res = read(sockfd, buffer, bufsize);
    if ((size_t)res != bufsize) {
        free(buffer);
        error("ERROR: Reading message content from socket");
    }

    buffer[bufsize] = '\0'; // Null-terminate the message
    return buffer;
}

int main(int argc, char *argv[]) {
    int sockfd, res;
    struct addrinfo hints, *result, *rp;
    char buffer[BUFFERLENGTH];

    // Validate port number
    int port = atoi(argv[2]);
    if (port <= 0 || port > 65535) {
        fprintf(stderr, "ERROR: Invalid port number %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    // Set up the hints structure for getaddrinfo
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;          // IPv4
    hints.ai_socktype = SOCK_STREAM;    // TCP socket

    // Resolve the server address and port
    res = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (res != 0) {
        fprintf(stderr, "ERROR: getaddrinfo: %s\n", gai_strerror(res));
        exit(EXIT_FAILURE);
    }

    // Try each address until we successfully connect
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) {
            continue; // Try the next address
        }

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            break; // Successfully connected
        }

        close(sockfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "ERROR: Could not connect to the server\n");
        freeaddrinfo(result);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result); // No longer needed

    // Main loop to interact with the server
    // if (!fgets(buffer, BUFFERLENGTH, stdin)) {
    //     fprintf(stderr, "ERROR: Failed to read input\n");
    // }

    // Remove newline character from input
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }

    // Exit condition
    if (strcmp(buffer, "exit") == 0) {
        printf("Exiting...\n");
    }

    memset(buffer, 0, BUFFERLENGTH);
    for (int i = 3; i < argc; i++) {
        strncat(buffer, argv[i], BUFFERLENGTH - strlen(buffer) - 1);
        if (i < argc - 1) {
            strncat(buffer, " ", BUFFERLENGTH - strlen(buffer) - 1);
        }
    }


    // Send the message to the server
    if (writeResult(sockfd, buffer, strlen(buffer)) < 0) {
        fprintf(stderr, "ERROR: Failed to send message to server\n");
    }

    // Wait for a reply from the server
    char *response_msg = response(sockfd);
    if (!response_msg) {
        fprintf(stderr, "ERROR: Failed to read response from server\n");
    }
    if (strchr(response_msg, '\n') == 0){
        strcat(response_msg, "\n");
    }
    printf("%s", response_msg);
    free(response_msg);

    close(sockfd); // Clean up the socket
    return 0;
}
