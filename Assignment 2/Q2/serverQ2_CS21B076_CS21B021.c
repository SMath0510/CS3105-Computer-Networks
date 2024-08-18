#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_BUFFER_SIZE 1024
#define FILENAME_SIZE 100
#define HEADER_SIZE 1024
#define FILE_EXTENSION_SIZE 10
#define LOCATION_404 "files/"
#define DEBUG 0

int PORT = 8080;
char FILE_DIRECTORY[FILENAME_SIZE] = "files";

struct request_stats{
    char type[20]; // text/html, text/css, image/jpg, image/png, etc..
    char file_location[FILENAME_SIZE]; // entire file location
    char infile_location[FILENAME_SIZE];
    char request_type[10]; // GET, POST
    char file_extension[FILE_EXTENSION_SIZE];
    char mime_type[100];
    char message[MAX_BUFFER_SIZE];
    long int file_size;
};

void handle_get_request(int , struct request_stats *);
void handle_post_request(int , struct request_stats *);

// Function to convert integer to string
char* int_to_string(int value) {
    char* str = (char*)malloc(20 * sizeof(char)); // Allocate memory for the string
    if (str == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    sprintf(str, "%d", value); // Convert integer to string
    return str;
}

int string_to_int(char * str_value){
    return atoi(str_value);
}

long int countWords(const char *str_ref) {
    long int count = 0;
    char str[MAX_BUFFER_SIZE];
    strcpy(str, str_ref);
    
    // Replace periods with spaces
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.' || str[i] == '?' || str[i] == '!' || str[i] =='\n' || str[i] == ',') {
            str[i] = ' ';
        }
    }
    // Remove initial and trailing spaces
    int start = 0;
    int end = strlen(str) - 1;
    while (str[start] == ' ' && start <= end) {
        start++;
    }
    while (str[end] == ' ' && end >= start) {
        end--;
    }
    str[end + 1] = '\0'; // Terminate the string after the last non-space character


    // Loop through each character in the string
    for (int i = 1; str[i] != '\0'; i++) {
        // If current character is not a space or a newline, and we are not already inside a word, increment the word count
        if ((str[i] == ' ') && (str[i-1] != ' ')) {
            count++;
        }
    }

    return count+1;
}

// Function to count the number of sentences in a string
long int countSentences(const char *str) {
    long int sentenceCount = 0;
    long int str_len = strlen(str);
    int add_one = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        // If current character is not a space or a newline, and we are not already inside a word, increment the word count
        if (str[i] == '.' || str[i] == '?' || str[i] == '!') {
            add_one = 0;
            sentenceCount++;
        }
        else if(str[i] == '\n'){
            sentenceCount++;
        }
        else if(str[i] == ' '){}
        else add_one = 1;
    }
    return sentenceCount + add_one;
}

// Function to get file size
long int get_file_size(char * file_location){ 
    FILE* fp = fopen(file_location, "r"); 
    if (fp == NULL) { 
        perror("File Not Found!\n"); 
        return -1; 
    } 
    fseek(fp, 0L, SEEK_END); 
    long int res = ftell(fp); 
    fclose(fp); 
    
    if(DEBUG) printf("File Size: %ld\n", res);
    return res; 
} 

// Function to get file extension
void get_file_extension(struct request_stats * request_stat){
    int i = strlen(request_stat->file_location)-1;
    for(; i >= 0; i --){
        if(request_stat->file_location[i] == '.') break;
    }
    if(i < 0){
        if(DEBUG) printf("No file extension\n");
        return;
    }
    int j = 0;
    char buff[100];
    for(; i < strlen(request_stat->file_location); i++){
        buff[j++] = request_stat->file_location[i];
    }
    buff[j] = '\0';
    bzero(request_stat->file_extension, sizeof(request_stat->file_extension));
    strcpy(request_stat->file_extension, buff);
    if(DEBUG) printf("File Extension Stored: %s\n", request_stat->file_extension);
}

// Function to set mime type
void set_mime_type(struct request_stats * request_stat){
    if(strcmp(request_stat->file_extension, ".html") == 0){
        strcpy(request_stat->mime_type, "text/html");
    }
    else if(strcmp(request_stat->file_extension, ".css") == 0){
        strcpy(request_stat->mime_type, "text/css");
    }
    else if(strcmp(request_stat->file_extension, ".js") == 0){
        strcpy(request_stat->mime_type, "text/javascript");
    }
    else if(strcmp(request_stat->file_extension, ".jpg") == 0){
        strcpy(request_stat->mime_type, "image/jpg");
    }
    else if(strcmp(request_stat->file_extension, ".gif") == 0){
        strcpy(request_stat->mime_type, "image/gif");
    }
    else if(strcmp(request_stat->file_extension, ".png") == 0){
        strcpy(request_stat->mime_type, "image/png");
    }
    else if(strcmp(request_stat->file_extension, ".jpeg") == 0){
        strcpy(request_stat->mime_type, "image/jpeg");
    }
    else if(strcmp(request_stat->file_extension, ".ico") == 0){
        strcpy(request_stat->mime_type, "image/x-icon");
    }
    else{
    }
    if(DEBUG) printf("Mime Type Stored: %s\n", request_stat->mime_type);
}

// Decodes the entire get request
void decode_get_request(struct request_stats * request_stat, char * request, int request_size){
    int count = 0;
    char buffer[MAX_BUFFER_SIZE];
    bzero(buffer, sizeof(buffer));
    int buffer_index = 0;
    if(DEBUG) printf("Decoding the request\n");
    for(int i = 0; i < request_size; i++){
        if(request[i] == ' ' && count == 0){
            strcpy(request_stat->request_type, buffer);
            if(DEBUG) printf("Request Type: %s %s\n", request_stat->request_type, buffer);
            count ++;
            buffer_index = 0;
            bzero(buffer, sizeof(buffer));
        }
        else if(request[i] == ' ' && count == 1){
            if(strcmp(buffer, "/") == 0){
                strcpy(buffer, "/index.html");
            }
            strcpy(request_stat->infile_location, buffer);
            strcat(request_stat->file_location, buffer);
            if(DEBUG) printf("File Location: %s %s\n", request_stat->file_location, buffer);
            count ++;
            buffer_index = 0;
            bzero(buffer, sizeof(buffer));
            break;
        } 
        else{
            buffer[buffer_index ++] = request[i];
        }

        if(count == 2) break;
    }

    request_stat->file_size = get_file_size(request_stat->file_location);
    get_file_extension(request_stat);
    set_mime_type(request_stat);

    if(DEBUG) printf("Decoded the request\n");
}

// Decodes the entire post request
void decode_post_request(struct request_stats * request_stat, char * request, int request_size){
    if(strlen(request) == 0) return;
    int i = 0;
    int start = -1, end = -1;
    for(int i = 0; i + 3 < request_size; i++){
        if(request[i] == '%' && request[i+1] == '*' && request[i+2] == '*' && request[i+3] == '%'){
            if(start == -1){
                start = i+4;
            }
            end = i;
            continue;
        }
    }
    char buffer[MAX_BUFFER_SIZE];
    for(int i = start; i < end; i++){
        buffer[i-start] = request[i];
    }
    buffer[end-start] = '\0';
    if(DEBUG) printf("Buffer is: %s\n", buffer);
    bzero(request_stat->message, sizeof(request_stat->message));
    strcpy(request_stat->message, buffer);
    if(DEBUG) printf("Message is: %s\n", request_stat->message);
    if(DEBUG) printf("Request is: %s\n", request);
}

// Handles request of the client
void request_handler(int * client_socket){
    // Receive request from client
    while(1){
        char request[MAX_BUFFER_SIZE];
        ssize_t bytes_received = recv(*client_socket, request, sizeof(request), 0);
        if (bytes_received == -1) {
            perror("Error receiving request");
            exit(EXIT_FAILURE);
        }
        // Null-terminate received data to use it as a string
        request[bytes_received] = '\0';
        if(strlen(request) < 4) return;
        int get_request = 1, post_request = 1;
        char get[3] = "GET";
        char post[4] = "POST";
        for(int i = 0; i < 3; i++){
            if(get[i] != request[i]) get_request = 0;
        }
        for(int i = 0; i < 4; i++){
            if(post[i] != request[i]) post_request = 0;
        }

        if(get_request == 1){
            struct request_stats request_stat;
            strcpy(request_stat.file_location, FILE_DIRECTORY);
            decode_get_request(&request_stat, request, bytes_received);
            if(DEBUG) printf("Request Stat:\n%s %s %s %s %ld\n", request_stat.file_location, request_stat.file_extension, request_stat.request_type, request_stat.mime_type, request_stat.file_size);
            if(DEBUG) printf("GET request handling ..\n");
            handle_get_request(*client_socket, &request_stat);
        }
        else if(post_request == 1){
            struct request_stats request_stat;
            strcpy(request_stat.file_location, FILE_DIRECTORY);
            decode_post_request(&request_stat, request, bytes_received);
            if(DEBUG) printf("Request Stat:\n%s %s\n", request_stat.file_location, request_stat.message);
            if(DEBUG) printf("POST request handling ..\n");
            handle_post_request(*client_socket, &request_stat);
        }
        else{
            if(DEBUG) printf("Invalid request type\n");
        }
        if(DEBUG) printf("Request Handled .. \n");
    }
    pthread_exit(NULL);
    close(*client_socket);
}

// ONLY FOR GET REQUEST
void get_header(char * http_header, struct request_stats * request_stat){
    bzero(http_header, sizeof(http_header));
    strcpy(http_header, "HTTP/1.1 200 OK\r\n");
    strcat(http_header, "Content-Type: ");
    strcat(http_header, request_stat->mime_type);
    strcat(http_header, "\r\n");
    strcat(http_header, "Content-Length: ");
    strcat(http_header, int_to_string(request_stat->file_size));
    strcat(http_header, "\r\n");
    strcat(http_header, "Connection: close\r\n\r\n");
    if(DEBUG) printf("The http header\n%s\n", http_header);
}

// Handles the get request sending info to the client part
void handle_get_request(int client_socket, struct request_stats * request_stat) {
    FILE *file_check = fopen(request_stat->file_location, "rb");
    if (!file_check) {
        if(DEBUG) printf("Orignal Request %s\n", request_stat->file_location);
        bzero(request_stat->file_location, sizeof(request_stat->file_location));
        char base_location[FILENAME_MAX];
        strcpy(base_location, LOCATION_404);
        strcat(base_location, request_stat->infile_location);
        strcpy(request_stat->file_location, base_location);
        request_stat->file_size = get_file_size(request_stat->file_location);
        if(DEBUG) printf("File Size: %ld\n", request_stat->file_size);
        get_file_extension(request_stat);
        set_mime_type(request_stat);
    }
    file_check = fopen(request_stat->file_location, "rb");
    if (!file_check) {
        if(DEBUG) printf("Orignal Request %s\n", request_stat->file_location);
        bzero(request_stat->file_location, sizeof(request_stat->file_location));
        char base_location[FILENAME_MAX];
        strcpy(base_location, LOCATION_404);
        strcat(base_location, "/404.html");
        strcpy(request_stat->file_location, base_location);
        request_stat->file_size = get_file_size(request_stat->file_location);
        if(DEBUG) printf("File Size: %ld\n", request_stat->file_size);
        get_file_extension(request_stat);
        set_mime_type(request_stat);
    }

    FILE *file = fopen(request_stat->file_location, "rb");
    if (!file) {
        perror("Error opening file");
        pthread_exit(NULL);
    }

    char buffer[MAX_BUFFER_SIZE];
    char http_header[HEADER_SIZE];
    get_header(http_header, request_stat);
    if(DEBUG) printf("Header: %s\n", http_header);

    if (send(client_socket, http_header, strlen(http_header), 0) == -1) {
        perror("Error sending HTTP headers");
        pthread_exit(NULL);
    }

    size_t bytes_read;
    if(DEBUG) printf("Going to send file %s in %d...\n", request_stat->file_location, client_socket);

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        ssize_t transferred_bytes = send(client_socket, buffer, bytes_read, 0);
        if (transferred_bytes != bytes_read) {
            perror("Error sending file");
            pthread_exit(NULL);
        }
        else if(transferred_bytes <= 0){
            if(DEBUG) printf("Disconnecting client\n");
            pthread_exit(NULL);
        }
    }
    if(DEBUG) printf("Sent the file %s through %d.. \n", request_stat->file_location, client_socket);
    fclose(file);
}

// Handles the post request sending info to the client part
void handle_post_request(int client_socket, struct request_stats *request_stat) {

    char response[MAX_BUFFER_SIZE];
    sprintf(response, "Received the message successfully!!\nChar Count: %ld\nWord Count: %ld\n Sentence Count: %ld\n", strlen(request_stat->message), countWords(request_stat->message), countSentences(request_stat->message));
    char http_header[HEADER_SIZE];
    sprintf(http_header, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n", strlen(response));

    // Send HTTP header
    if (send(client_socket, http_header, strlen(http_header), 0) == -1) {
        perror("Error sending HTTP headers");
        close(client_socket);
        return;
    }

    // Send response data
    if (send(client_socket, response, strlen(response), 0) == -1) {
        perror("Error sending response data");
        close(client_socket);
        return;
    }
}

int main(int argc, char ** argv) {

    if(argc != 3){
        printf("Usage ./server <PORT> <FILE_DIRECTORY>\n");
        exit(EXIT_FAILURE);
    }

    PORT = atoi(argv[1]);
    strcpy(FILE_DIRECTORY,argv[2]);

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
    if (listen(server_socket, 5) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connections and handle requests in separate threads
    while (1) {
        client_addr_len = sizeof(client_addr);
        int *client_socket = (int *)malloc(sizeof(int));
        if (!client_socket) {
            perror("Error allocating memory for client socket");
            exit(EXIT_FAILURE);
        }
        *client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (*client_socket == -1) {
            perror("Error accepting connection");
            free(client_socket);
            continue;
        }

        printf("Client connected\n");

        // Create a new thread to handle the client request
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, (void *)request_handler, (void *)client_socket) != 0) {
            perror("Error creating thread");
            close(*client_socket);
            free(client_socket);
            continue;
        }
        
        // Detach the thread to avoid memory leaks
        if (pthread_detach(thread_id) != 0) {
            perror("Error detaching thread");
            close(*client_socket);
            free(client_socket);
            continue;
        }
    }

    // Close server socket
    close(server_socket);

    return 0;
}
