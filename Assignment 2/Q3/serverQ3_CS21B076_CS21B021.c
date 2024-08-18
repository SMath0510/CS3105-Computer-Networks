#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>

#define MAX_BUFFER_SIZE 1024
#define USERNAME_SIZE 100

int PORT = 8000;
int MAX_CLIENTS = 100;
int DISCONNECTION_TIME_THRESHOLD = 10;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for thread safety

struct ClientData{
    char username[USERNAME_SIZE];
    int client_socket_interface;
    time_t last_connected_time;
    double idle_time;
};

struct ClientData * active_client_socket_list;

void send_all(const char *, const char *);
int check_existing_username(const char *);
void get_username(struct ClientData *);
void add_client(struct ClientData *);
void remove_client(struct ClientData *);
void listener(int *);
void timeout_monitor(struct ClientData *);

// Generate the current connected user list
void generate_user_list(char * user_list){
    bzero(user_list, sizeof(user_list));
    strcpy(user_list, "Connected Users:\n");
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(active_client_socket_list[i].client_socket_interface != -1){
            strcat(user_list, active_client_socket_list[i].username);
            strcat(user_list, "\n");
        }
    }
    printf("UserList: %s\n", user_list);
}

// Disconnect a client and close up the socket
void disconnect_client(int client_socket) {
    // Step 1: Shutdown the connection
    if (shutdown(client_socket, SHUT_RDWR) == -1) {
        perror("Error shutting down socket");
    }
    
    // Step 2: Close the socket
    if (close(client_socket) == -1) {
        perror("Error closing socket");
    }
}

// Broadcast a message and send it to all
void send_all(const char * message, const char * username){
    printf("%s is sending %s to all\n", username, message);
    char formatted_message[MAX_BUFFER_SIZE];
    strcpy(formatted_message, username);
    strcat(formatted_message, ": ");
    strcat(formatted_message, message);
    // lock
    for(int i = 0; i < MAX_CLIENTS; i++){
        // struct ClientData active_client_socket_list[i] = active_client_socket_list[i];
        if(active_client_socket_list[i].client_socket_interface != -1){
            if(strcmp(active_client_socket_list[i].username, username) != 0) {
                printf("Sending from %s to %s, the string %s\n", username,  active_client_socket_list[i].username, message);
                send(active_client_socket_list[i].client_socket_interface, formatted_message, MAX_BUFFER_SIZE, 0);
                printf("Sent from %s to %s, the string %s\n", username,  active_client_socket_list[i].username, message);
            }
        }
    }
    printf("%s sent the message %s to all the users\n", username, message);
    // unlock
}

int check_existing_username(const char * username){
    printf("Finding duplicate entry for %s\n", username);
    int username_exists = 0;
    for(int i = 0; i < MAX_CLIENTS; i++){
        // struct ClientData active_client_socket_list[i] = active_client_socket_list[i];
        if(active_client_socket_list[i].client_socket_interface != -1){
            if(strcmp(active_client_socket_list[i].username, username) == 0){
                printf("Found duplicate entry for %s\n", username);
                username_exists = 1;
            }
        }
    }
    
    printf("%s Returning from duplicate entry function\n", username);
    return username_exists;
}

void get_username(struct ClientData *client_data){
    int received_username = 0;
    printf("Collecting username at socketfd %d\n", client_data->client_socket_interface);
    char user_list[MAX_BUFFER_SIZE];
    char init_message[MAX_BUFFER_SIZE];
    generate_user_list(user_list);
    strcpy(init_message, "Welcome to the chat bot!!\n");
    strcat(init_message, user_list);
    strcat(init_message, "Enter your username: ");
    send(client_data->client_socket_interface, init_message, MAX_BUFFER_SIZE, 0);
    while(1){
        ssize_t size_read = recv(client_data->client_socket_interface, client_data->username, sizeof(client_data->username), 0);
        if (size_read == -1) {
            // Error reading from socket
            perror("Error reading from socket");
            pthread_mutex_lock(&mutex);
            remove_client(client_data);
            pthread_mutex_unlock(&mutex);
            printf("%s Exited the Chat\n", client_data->username);
            pthread_exit(NULL); // Exit the thread
        } else if (size_read == 0) {
            // Client disconnected gracefully
            printf("%s disconnected gracefully\n", client_data->username);
            pthread_mutex_lock(&mutex);
            remove_client(client_data);
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL); // Exit the thread
        }
        time(&(client_data->last_connected_time)); // Reconnected
        client_data->username[size_read] = '\0';
        printf("Collected %s in socket %d\n", client_data->username, client_data->client_socket_interface);
        if(check_existing_username(client_data->username) == 0) break;
        send(client_data->client_socket_interface, "Enter your username: ", MAX_BUFFER_SIZE, 0);
    }
    // time(last_connected_time);
    printf("Exiting the function after storing the username %s and the socket id is %d\n", client_data->username, client_data->client_socket_interface);
}

// Add a connected client
void add_client(struct ClientData *client_data){
    get_username(client_data);
    printf("Received the username %s", client_data->username);
    printf("Adding the client %s\n", client_data->username);
    int i;
    for(i = 0; i < MAX_CLIENTS; i++){
        struct ClientData current_client_data = active_client_socket_list[i];
        if((current_client_data).client_socket_interface == -1){
            active_client_socket_list[i] = *(client_data);
            char temp[USERNAME_SIZE + 20];
            strcpy(temp, client_data->username);
            strcat(temp, " joined the chat");
            send_all(temp, "Server");
            break;
        }
    }
    printf("Added %s to the client list and socket is %d\n", client_data->username, client_data->client_socket_interface);
    // if(i >= 1) printf("SanityCheck:%s %d at 0\n%s %d at 1\n", active_client_socket_list[0].username, active_client_socket_list[0].client_socket_interface, active_client_socket_list[1].username, active_client_socket_list[1].client_socket_interface);
}

// Remove a disconnected client
void remove_client(struct ClientData * client_data){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(active_client_socket_list[i].client_socket_interface != -1){
            if(strcmp(active_client_socket_list[i].username, client_data->username) == 0){
                char temp[USERNAME_SIZE + 20];
                strcpy(temp, active_client_socket_list[i].username);
                strcat(temp, " left the chat");
                send_all(temp, "Server");
                active_client_socket_list[i].client_socket_interface = -1;
                break;
            }
        }
    }
    disconnect_client(client_data->client_socket_interface);
}

// Initialize the client list
void initialize_client_list(){
    active_client_socket_list = (struct ClientData *) malloc(MAX_CLIENTS * sizeof(struct ClientData));
    for(int i = 0; i < MAX_CLIENTS; i++){
        active_client_socket_list[i].client_socket_interface = -1;
    }
}

// Check for timeout
void timeout_monitor(struct ClientData *client_data) {
    client_data->idle_time = 0;
    time(&(client_data->last_connected_time));
    while (1) {
        sleep(1); // Adjust the sleep interval as needed

        time_t current_time;
        time(&current_time);
        
        // Calculate idle time
        client_data->idle_time = difftime(current_time, client_data->last_connected_time);

        // Check if idle time exceeds threshold
        if (client_data->idle_time > DISCONNECTION_TIME_THRESHOLD) {
            printf("%s has been idle for too long. Disconnecting...\n", client_data->username);
            char * exit_message = "You have been disconnected. Reconnect to join back the chatbox\n";
            send(client_data->client_socket_interface, exit_message, MAX_BUFFER_SIZE, 0);
            pthread_mutex_lock(&mutex);
            remove_client(client_data);
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL); // Exit the thread
        }
    }
}

// Listen to the incoming request
void listener(int *socket_id_ref) {
    int socket_id = *socket_id_ref;
    struct ClientData client_data;
    client_data.client_socket_interface = socket_id;
    pthread_t timeout_manager;
    pthread_create(&timeout_manager, NULL, (void *)timeout_monitor, (void *) &client_data);
    printf("Got the client on the socket %d\n", client_data.client_socket_interface);
    char buffer[MAX_BUFFER_SIZE];
    int first = 1;
    pthread_mutex_lock(&mutex);
    add_client(&client_data);
    pthread_mutex_unlock(&mutex);
    printf("Thread is working on %s at %d\n", client_data.username, socket_id);
    
    while (1) {        // Check for incoming data
        if(client_data.idle_time > DISCONNECTION_TIME_THRESHOLD){
            printf("%s Disconnected due to timeout\n", client_data.username);
            pthread_exit(NULL);
        }
        bzero(buffer, sizeof(buffer));
        ssize_t size_read = recv(socket_id, buffer, sizeof(buffer), 0);
        
        if (size_read == -1) {
            // Error reading from socket
            perror("Error reading from socket");
            pthread_mutex_lock(&mutex);
            remove_client(&client_data);
            pthread_mutex_unlock(&mutex);
            printf("%s Exited the Chat\n", client_data.username);
            pthread_exit(NULL); // Exit the thread
        } else if (size_read == 0) {
            // Client disconnected gracefully
            printf("%s disconnected gracefully\n", client_data.username);
            pthread_mutex_lock(&mutex);
            remove_client(&client_data);
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL); // Exit the thread
        }

        // Update last connected time
        time(&(client_data.last_connected_time));
        if(strcmp(buffer, "\\list") == 0){
            char user_list[MAX_BUFFER_SIZE];
            generate_user_list(user_list);
            pthread_mutex_lock(&mutex);
            send(client_data.client_socket_interface, user_list, MAX_BUFFER_SIZE, 0);
            pthread_mutex_unlock(&mutex);
        }
        else if(strcmp(buffer, "\\bye") == 0){
            pthread_mutex_lock(&mutex);
            remove_client(&client_data);
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL); // Exit the thread
        }
        else{
            printf("%s wrote %s into socket %d\n", client_data.username, buffer, socket_id);
            pthread_mutex_lock(&mutex);
            send_all(buffer, client_data.username);
            pthread_mutex_unlock(&mutex);
        }
    }
}

int main(int argc, char ** argv) {

    if(argc != 4){
        printf("Incomplete set of arguments passed\n");
        exit(EXIT_FAILURE);
    }

    PORT = atoi(argv[1]);
    MAX_CLIENTS = atoi(argv[2]);
    DISCONNECTION_TIME_THRESHOLD = atoi(argv[3]);

    int server_socket;
    struct sockaddr_in server_addr;

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
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    initialize_client_list();
    printf("Client list initialized\n");
    while(1){
        socklen_t client_addr_len;
        struct sockaddr_in client_addr;
        client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        pthread_t listener_id;
        pthread_create(&listener_id, NULL, (void *)listener, (void *) &client_socket);
    }

    close(server_socket);

    return 0;
}
