#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024
#define PORT 8080

int sockfd;
char username[50];
char channel[50] = "";
char room[50] = "";
bool in_room = false;
bool running = true;

void get_timestamp(char *buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, buffer_size, "[%d/%m/%Y %H:%M:%S]", t);
}

void* receive_messages(void* arg) {
    char buffer[BUFFER_SIZE];

    while (running) {
        int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Koneksi terputus.\n");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        buffer[bytes_received] = '\0';

        // Hanya mencetak pesan jika bukan perintah
        if (strncmp(buffer, "Invalid command", 15) != 0) {
            printf("%s\n", buffer);
        }
    }
    return NULL;
}

int handle_account() {
    char login_info[BUFFER_SIZE];
    snprintf(login_info, sizeof(login_info), "LOGIN %s -p %s\n", username, channel);
    send(sockfd, login_info, strlen(login_info), 0);

    char response[BUFFER_SIZE];
    int bytes_received = recv(sockfd, response, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
        printf("Gagal menerima respon dari server.\n");
        return 0;
    }
    response[bytes_received] = '\0';
    printf("%s\n", response);

    if (strstr(response, "berhasil login")) {
        return 1;
    } else {
        return 0;
    }
}

int connect_server() {
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return 0;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection to server failed");
        close(sockfd);
        return 0;
    }

    return 1;
}

void clear_terminal() {
    printf("\033[H\033[J");
}

void display_chat_history(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        perror("Gagal membuka file chat");
        return;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}

void* input_handler(void* arg) {
    char input[BUFFER_SIZE];

    while (running) {
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = 0;

        if (strncmp(input, "EXIT", 4) == 0) {
            in_room = false;
            room[0] = '\0';
            channel[0] = '\0';
            printf("[%s] EXIT\n", username);
            send(sockfd, "EXIT\n", 5, 0);
            running = false;
        } else if (strncmp(input, "-channel ", 9) == 0) {
            sscanf(input, "-channel %s -room %s", channel, room);
            in_room = true;

            char filepath[BUFFER_SIZE];
            snprintf(filepath, sizeof(filepath), "/home/kokon/FP/DiscorIT/%s/%s/chat.csv", channel, room);
            display_chat_history(filepath);
            printf("[%s/%s/%s] ", username, channel, room);
            fflush(stdout);
        } else {
            send(sockfd, input, strlen(input), 0);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 5 || strcmp(argv[1], "LOGIN") != 0 || strcmp(argv[3], "-p") != 0) {
        fprintf(stderr, "Usage: %s LOGIN <username> -p <password>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    strcpy(username, argv[2]);
    strcpy(channel, argv[4]);

    if (!connect_server()) {
        fprintf(stderr, "Gagal terhubung ke server.\n");
        exit(EXIT_FAILURE);
    }

    if (!handle_account()) {
        fprintf(stderr, "Login gagal.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t recv_thread, input_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);
    pthread_create(&input_thread, NULL, input_handler, NULL);

    pthread_join(input_thread, NULL);
    pthread_cancel(recv_thread);

    return 0;
}
