#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_BUFFER_SIZE 1024  // Maximum buffer size for messages
unsigned int PORT;  // Port number
char root_directory[MAX_BUFFER_SIZE];  // Root directory for MP3 files
unsigned int max_clients = 5;  // Maximum number of clients allowed to connect

// Function to convert string to integer
int string_to_int(const char *str) {
    return atoi(str);
}

// Function to trim newline characters from a string
char * trim_request(char * request){
    char * trimmed_request = (char *) malloc(sizeof(char) * strlen(request));
    for(int i = 0; i < strlen(request); i++){
        if(request[i] != '\n') trimmed_request[i] = request[i];
        else trimmed_request[i] = '\0';
    }
    return trimmed_request;
}

// Function to send MP3 file to client
int send_mp3(int client_socket, const char *filename) {
    FILE *mp3_file = fopen(filename, "rb");
    if (!mp3_file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[4096];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), mp3_file)) > 0) {
        int status = send(client_socket, buffer, bytes_read, 0);
        if(status == -1){
            printf("Connection terminated midway\n");
            return 0;
        }
        if (status != (size_t)bytes_read) {
            perror("Error sending file");
            exit(EXIT_FAILURE);
        }
        
    }

    fclose(mp3_file);
    return 1;
}

// Function to manage client requests
void manage_client(int * client_socket){
    
    char request[MAX_BUFFER_SIZE];
    
    // Determine the filename based on the request
    char filename[MAX_BUFFER_SIZE];
    strcpy(filename, root_directory);

    while(1){
        ssize_t bytes_received = recv(*client_socket, request, sizeof(request), 0);
        if (bytes_received == 0) {
            close(*client_socket);
            pthread_exit(NULL);
        }
        if (bytes_received == -1) {
            perror("Error receiving request");
            exit(EXIT_FAILURE);
        }

        // Null-terminate received data to use it as a string
        request[bytes_received] = '\0';

        if(strlen(request) > 0) {

            // Print the requested song number
            printf("Requested song number: %s\n", request);
            
            strcat(filename, trim_request(request));
            strcat(filename, ".mp3");
            printf("File to transfer: %s\n", filename);
            // Send the requested mp3 file to the client
            int sending_status = send_mp3(*client_socket, filename);
            pthread_exit(NULL);
        }
    }
}

// Main function
int main(int argc, char ** argv) {
    if(argc < 4){
        printf("Insufficient number of arguments\n");
        exit(EXIT_FAILURE);
    }
    if (argc > 4){
        printf("Too many arguments\n");
        exit(EXIT_FAILURE);
    }

    // Setting the arguments
    PORT = string_to_int(argv[1]);
    strcpy(root_directory, argv[2]);
    max_clients = string_to_int(argv[3]);


    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, max_clients) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connection
    while(1){
        int client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }

        printf("Client IP: %s connected\n", inet_ntoa(client_addr.sin_addr));
        pthread_t thread_idx;
        pthread_create(&thread_idx, NULL, (void *)manage_client, (void *) &client_socket);
        // Receive request from client
    }
    close(server_socket);

    return 0;
}
