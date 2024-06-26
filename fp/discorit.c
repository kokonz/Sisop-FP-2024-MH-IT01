#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024
#define PROMPT_SIZE 4096  // Increase the size of the prompt buffer

void send_request(int sock, const char *request);
void interactive_mode(int sock, const char *username);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <REGISTER|LOGIN|CREATE CHANNEL|EDIT CHANNEL|DEL CHANNEL|CREATE ROOM|EDIT ROOM|DEL ROOM|BAN|UNBAN|LIST CHANNEL|LIST ROOM|LIST USER|JOIN> <args>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char request[BUF_SIZE] = {0};

    if (strcmp(argv[1], "REGISTER") == 0 && argc == 5 && strcmp(argv[3], "-p") == 0) {
        snprintf(request, sizeof(request), "REGISTER %s -p %s", argv[2], argv[4]);
    } else if (strcmp(argv[1], "LOGIN") == 0 && argc == 5 && strcmp(argv[3], "-p") == 0) {
        snprintf(request, sizeof(request), "LOGIN %s -p %s", argv[2], argv[4]);
    } else if (strcmp(argv[1], "CREATE CHANNEL") == 0 && argc == 5 && strcmp(argv[3], "-k") == 0) {
        snprintf(request, sizeof(request), "CREATE CHANNEL %s -k %s", argv[2], argv[4]);
    } else if (strcmp(argv[1], "EDIT CHANNEL") == 0 && argc == 5) {
        snprintf(request, sizeof(request), "EDIT CHANNEL %s TO %s", argv[2], argv[4]);
    } else if (strcmp(argv[1], "DEL CHANNEL") == 0 && argc == 3) {
        snprintf(request, sizeof(request), "DEL CHANNEL %s", argv[2]);
    } else if (strcmp(argv[1], "CREATE ROOM") == 0 && argc == 4) {
        snprintf(request, sizeof(request), "CREATE ROOM %s/%s", argv[2], argv[3]);
    } else if (strcmp(argv[1], "EDIT ROOM") == 0 && argc == 5) {
        snprintf(request, sizeof(request), "EDIT ROOM %s/%s TO %s", argv[2], argv[3], argv[4]);
    } else if (strcmp(argv[1], "DEL ROOM") == 0 && argc == 4) {
        snprintf(request, sizeof(request), "DEL ROOM %s/%s", argv[2], argv[3]);
    } else if (strcmp(argv[1], "DEL ROOM ALL") == 0 && argc == 3) {
        snprintf(request, sizeof(request), "DEL ROOM ALL %s", argv[2]);
    } else if (strcmp(argv[1], "BAN") == 0 && argc == 4) {
        snprintf(request, sizeof(request), "BAN %s/%s", argv[2], argv[3]);
    } else if (strcmp(argv[1], "UNBAN") == 0 && argc == 4) {
        snprintf(request, sizeof(request), "UNBAN %s/%s", argv[2], argv[3]);
    } else if (strcmp(argv[1], "LIST CHANNEL") == 0) {
        snprintf(request, sizeof(request), "LIST CHANNEL");
    } else if (strcmp(argv[1], "LIST ROOM") == 0 && argc == 3) {
        snprintf(request, sizeof(request), "LIST ROOM %s", argv[2]);
    } else if (strcmp(argv[1], "LIST USER") == 0 && argc == 2) {
    snprintf(request, sizeof(request), "LIST USER");
    } else if (strcmp(argv[1], "JOIN") == 0 && argc == 5 && strcmp(argv[3], "-k") == 0) {
        snprintf(request, sizeof(request), "JOIN %s -k %s", argv[2], argv[4]);
    } else if (strcmp(argv[1], "JOIN ROOM") == 0 && argc == 4) {
        snprintf(request, sizeof(request), "JOIN %s/%s", argv[2], argv[3]);
    } else if (strcmp(argv[1], "EDIT WHERE") == 0 && argc == 5 && strcmp(argv[3], "-u") == 0) {
        snprintf(request, sizeof(request), "EDIT WHERE %s -u %s", argv[2], argv[4]);
    } else if (strcmp(argv[1], "EDIT WHERE") == 0 && argc == 5 && strcmp(argv[3], "-p") == 0) {
        snprintf(request, sizeof(request), "EDIT WHERE %s -p %s", argv[2], argv[4]);
    } else if (strcmp(argv[1], "REMOVE") == 0 && argc == 3) {
        snprintf(request, sizeof(request), "REMOVE %s", argv[2]);
    } else if (strcmp(argv[1], "REMOVE") == 0 && strcmp(argv[2], "USER") == 0 && argc == 4) {
    snprintf(request, sizeof(request), "REMOVE USER %s", argv[3]);
    } else {
        fprintf(stderr, "Invalid command format\n");
        exit(EXIT_FAILURE);
    }

    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    send_request(sock, request);

    if (strstr(request, "LOGIN") != NULL) {
        interactive_mode(sock, argv[2]);
    }

    close(sock);

    return 0;
}

void send_request(int sock, const char *request) {
    char buffer[BUF_SIZE] = {0};
    send(sock, request, strlen(request), 0);
    read(sock, buffer, BUF_SIZE);
    printf("%s", buffer);
}

void interactive_mode(int sock, const char *username) {
    char input[BUF_SIZE];
    char buffer[BUF_SIZE];
    char prompt[PROMPT_SIZE];  // Ukuran buffer untuk prompt
    char current_channel[BUF_SIZE] = "";
    char current_room[BUF_SIZE] = "";

    while (1) {
        printf("%s", prompt);
        if (fgets(input, BUF_SIZE, stdin) == NULL) {
            break;
        }
        input[strcspn(input, "\n")] = 0;  // Menghapus karakter newline
        send(sock, input, strlen(input), 0);
        int valread = read(sock, buffer, BUF_SIZE);
        buffer[valread] = '\0';
        printf("%s\n", buffer);

        // Memperbarui prompt berdasarkan respons
        if (strncmp(input, "JOIN ", 5) == 0) {
            char join_type[BUF_SIZE];
            sscanf(input, "JOIN %s", join_type);

            if (strcmp(current_channel, "") == 0 && strcmp(current_room, "") == 0) {
                // User wants to join a channel
                strcpy(current_channel, join_type);  // Set channel name from user input
            } else if (strcmp(current_room, "") == 0) {
                // User wants to join a room within the current channel
                strcpy(current_room, join_type);  // Set room name from user input
            }
        }

        // Pembaruan prompt
        if (current_room[0] != '\0') {
            snprintf(prompt, sizeof(prompt), "[%s/%s/%s] ", username, current_channel, current_room);
        } else if (current_channel[0] != '\0') {
            snprintf(prompt, sizeof(prompt), "[%s/%s] ", username, current_channel);
        } else {
            snprintf(prompt, sizeof(prompt), "[%s] ", username);
        }

        if (strcmp(input, "EXIT") == 0) {
            if (strcmp(current_channel, "") == 0 && strcmp(current_room, "") == 0) {
                break; // Keluar dari loop dan tutup koneksi jika tidak dalam channel atau room
            } else if (strcmp(current_room, "") == 0) {
                strcpy(current_channel, ""); // Keluar dari channel
            } else {
                strcpy(current_room, ""); // Keluar dari room
            }
        }
    }
}
