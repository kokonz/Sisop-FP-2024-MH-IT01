#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <bcrypt.h>

#define PORT 8080
#define BUF_SIZE 1024
#define USER_FILE "/home/kokon/FP/DiscorIT/users.csv"
#define CHANNEL_FILE "/home/kokon/FP/DiscorIT/channels.csv"
#define MAX_USERNAME_LENGTH 50
char logged_in_user[MAX_USERNAME_LENGTH];

void *handle_client(void *arg);
void register_user(int client_socket, const char *username, const char *password);
void login_user(int client_socket, const char *username, const char *password);
void create_channel(int client_socket, const char *username, const char *channel, const char *key);
void edit_channel(int client_socket, const char *username, const char *old_channel, const char *new_channel);
void delete_channel(int client_socket, const char *username, const char *channel);
void create_room(int client_socket, const char *username, const char *channel, const char *room);
void edit_room(int client_socket, const char *username, const char *channel, const char *old_room, const char *new_room);
void delete_room(int client_socket, const char *username, const char *channel, const char *room);
void delete_all_rooms(int client_socket, const char *username, const char *channel);
int edit_profile(int client_socket, const char *username, const char *field, const char *new_value);
int ban_user(int client_socket, const char *logged_in_user, const char *username, const char *channel);
void unban_user(int client_socket, const char *logged_in_user, const char *username, const char *channel);
void remove_user(int client_socket, const char *logged_in_user, const char *username, const char *channel);
void list_user(int client_socket);
void list_channels(int client_socket);
void list_rooms(int client_socket, const char *channel);
void list_user_room(int client_socket, const char *channel);
int edit_user(int client_socket, const char *old_username, const char *new_username, int is_self_edit);
void edit_user_password(int client_socket, const char *username, const char *new_password, int is_self_edit);
int remove_user_root(int client_socket, const char *username);
int join_channel(int client_socket, const char *username, const char *channel, const char *key);
int join_room(int client_socket, const char *username, const char *channel, const char *room);
void chat(int client_socket, const char *username, const char *channel, const char *room, const char *text);
void see_chat(int client_socket, const char *channel, const char *room);
void edit_chat(int client_socket, const char *username, const char *channel, const char *room, int chat_id, const char *text);
void del_chat(int client_socket, const char *username, const char *channel, const char *room, int chat_id);
void initialize_directories();
int user_role_auth(const char *username, const char *channel, const char *role);
void log_activity(const char *channel, const char *activity);
int remove_directory(const char *path);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t thread_id;

    initialize_directories();

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) >= 0) {
        if (pthread_create(&thread_id, NULL, handle_client, (void *)&new_socket) != 0) {
            perror("pthread_create");
        }
    }

    if (new_socket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUF_SIZE];
    int valread;

    char current_channel[BUF_SIZE] = "";
    char current_room[BUF_SIZE] = "";
    char username[BUF_SIZE] = "";
    char old_username[BUF_SIZE], new_username[BUF_SIZE];
    char channel[BUF_SIZE];
    char new_password[BUF_SIZE];

    while ((valread = read(client_socket, buffer, BUF_SIZE)) > 0) {
        buffer[valread] = '\0';
        char command[BUF_SIZE], password[BUF_SIZE], key[BUF_SIZE], new_channel[BUF_SIZE], room[BUF_SIZE], new_room[BUF_SIZE], user_to_ban[BUF_SIZE], user_to_unban[BUF_SIZE], text[BUF_SIZE];
        int chat_id, is_self_edit;

        if (strcmp(buffer, "EXIT") == 0) {
            if (strcmp(current_room, "") != 0) {
                strcpy(current_room, "");
                snprintf(buffer, BUF_SIZE, "[%s/%s] ", username, current_channel);
                write(client_socket, buffer, strlen(buffer));
            } else if (strcmp(current_channel, "") != 0) {
                strcpy(current_channel, "");
                snprintf(buffer, BUF_SIZE, "[%s] ", username);
                write(client_socket, buffer, strlen(buffer));
            } else {
                snprintf(buffer, BUF_SIZE, "[%s] ", username);
                write(client_socket, buffer, strlen(buffer));
                break;
            }
            continue;
        }

        if (sscanf(buffer, "REGISTER %s -p %s", username, password) == 2) {
            register_user(client_socket, username, password);
        } else if (sscanf(buffer, "LOGIN %s -p %s", username, password) == 2) {
            login_user(client_socket, username, password);
        } else if (strcmp(buffer, "LIST USER") == 0) {
            list_user(client_socket);
        } else if (sscanf(buffer, "EDIT WHERE %s -u %s", old_username, new_username) == 2) {
            is_self_edit = 0;
            edit_user(client_socket, old_username, new_username, is_self_edit);
        } else if (sscanf(buffer, "EDIT WHERE %s -p %s", username, new_password) == 2) {
            is_self_edit = 0;
            edit_user_password(client_socket, username, new_password, is_self_edit);
        } else if (sscanf(buffer, "EDIT PROFILE SELF -u %s", new_username) == 1) {
            int is_self_edit = 1;
            if (edit_user(client_socket, username, new_username, 1)) {
                snprintf(username, sizeof(username), "%s", new_username);
                snprintf(buffer, BUF_SIZE, "Profil diupdate\n[%s] ", username);
                write(client_socket, buffer, strlen(buffer));
            }
        } else if (sscanf(buffer, "EDIT PROFILE SELF -p %s", new_password) == 1) {
            int is_self_edit = 1;
            edit_user_password(client_socket, username, new_password, 1);
        } else if (sscanf(buffer, "REMOVE %s", username) == 1) {
            remove_user_root(client_socket, username);
        } else if (strcmp(current_channel, "") == 0) {
            if (strcmp(buffer, "LIST CHANNEL") == 0) {
                list_channels(client_socket);
            } else if (sscanf(buffer, "JOIN %s", channel) == 1) {
                if (join_channel(client_socket, username, channel, key)) {
                    snprintf(current_channel, sizeof(current_channel), "%s", channel);
                }
            } else if (sscanf(buffer, "CREATE CHANNEL %s -k %s", channel, key) == 2) {
                create_channel(client_socket, username, channel, key);
            } else if (sscanf(buffer, "EDIT CHANNEL %s TO %s", channel, new_channel) == 2) {
                edit_channel(client_socket, username, channel, new_channel);
            } else if (sscanf(buffer, "DEL CHANNEL %s", channel) == 1) {
                delete_channel(client_socket, username, channel);
            } else {
                write(client_socket, "Invalid command\n", strlen("Invalid command\n"));
            }
        } else if (strcmp(current_room, "") == 0) {
            if (sscanf(buffer, "JOIN %s", room) == 1) {
                if (join_room(client_socket, username, current_channel, room)) {
                    snprintf(current_room, sizeof(current_room), "%s", room);
                }
            } else if (strcmp(buffer, "LIST ROOM") == 0) {
                list_rooms(client_socket, current_channel);
            } else if (strcmp(buffer, "LIST USER") == 0) {
                list_user_room(client_socket, current_channel);
            } else if (sscanf(buffer, "CREATE ROOM %s", room) == 1) {
                create_room(client_socket, username, current_channel, room);
            } else if (sscanf(buffer, "EDIT ROOM %s TO %s", room, new_room) == 2) {
                edit_room(client_socket, username, current_channel, room, new_room);
            } else if (strcmp(buffer, "DEL ROOM ALL") == 0) {
                delete_all_rooms(client_socket, username, current_channel);
            } else if (sscanf(buffer, "DEL ROOM %s", room) == 1) {
                delete_room(client_socket, username, current_channel, room);
            } else if (sscanf(buffer, "BAN %s", user_to_ban) == 1) {
                ban_user(client_socket, logged_in_user, user_to_ban, current_channel);
            } else if (sscanf(buffer, "UNBAN %s", user_to_unban) == 1) {
                unban_user(client_socket, logged_in_user, user_to_unban, current_channel);
            } else if (sscanf(buffer, "REMOVE USER %s", username) == 1) {
                remove_user(client_socket, logged_in_user, username, current_channel);
            } else {
                write(client_socket, "Invalid command\n", strlen("Invalid command\n"));
            }
        } else {
            if (sscanf(buffer, "CHAT \"%[^\"]\"", text) == 1) {
                chat(client_socket, username, current_channel, current_room, text);
            } else if (strcmp(buffer, "SEE CHAT") == 0) {
                see_chat(client_socket, current_channel, current_room);
            } else if (sscanf(buffer, "EDIT CHAT %d \"%[^\"]\"", &chat_id, text) == 2) {
                edit_chat(client_socket, username, current_channel, current_room, chat_id, text);
            } else if (sscanf(buffer, "DEL CHAT %d", &chat_id) == 1) {
                del_chat(client_socket, username, current_channel, current_room, chat_id);
            } else {
                write(client_socket, "Invalid command\n", strlen("Invalid command\n"));
            }
        }
    }

    close(client_socket);
    pthread_exit(NULL);
}

void register_user(int client_socket, const char *username, const char *password) {
    FILE *file = fopen(USER_FILE, "r+");
    if (!file) {
        perror("fopen");
        return;
    }

    char line[BUF_SIZE];
    int user_exists = 0;
    int is_first_user = 1;
    int user_id = 0;

    while (fgets(line, sizeof(line), file)) {
        char stored_username[BUF_SIZE];
        sscanf(line, "%d,%[^,],%*[^,],%*s", &user_id, stored_username);

        if (strcmp(stored_username, username) == 0) {
            user_exists = 1;
            break;
        }

        is_first_user = 0;
    }

    if (user_exists) {
        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "%s sudah terdaftar\n", username);
        write(client_socket, msg, strlen(msg));
    } else {
        user_id++;
        const char *role = is_first_user ? "ROOT" : "USER";
        char salt[BCRYPT_HASHSIZE];
        char hashed_password[BCRYPT_HASHSIZE];
        bcrypt_gensalt(12, salt);
        bcrypt_hashpw(password, salt, hashed_password);
        fprintf(file, "%d,%s,%s,%s\n", user_id, username, hashed_password, role);
        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "%s berhasil register\n", username);
        write(client_socket, msg, strlen(msg));
    }

    fclose(file);
}

void login_user(int client_socket, const char *username, const char *password) {
    FILE *file = fopen(USER_FILE, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    char line[BUF_SIZE];
    int login_success = 0;

    while (fgets(line, sizeof(line), file)) {
        char stored_username[BUF_SIZE], stored_password[BUF_SIZE];
        sscanf(line, "%*d,%[^,],%[^,],%*s", stored_username, stored_password);

        if (strcmp(stored_username, username) == 0 && bcrypt_checkpw(password, stored_password) == 0) {
            login_success = 1;
            break;
        }
    }

    if (login_success) {
        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "%s berhasil login\n[%s] ", username, username);
        strncpy(logged_in_user, username, sizeof(logged_in_user) - 1);
        write(client_socket, msg, strlen(msg));
    } else {
        char msg[] = "Login gagal\n";
        write(client_socket, msg, strlen(msg));
    }
    fclose(file);
}

void create_channel(int client_socket, const char *username, const char *channel, const char *key) {
    FILE *file = fopen(CHANNEL_FILE, "a+");
    if (!file) {
        perror("fopen");
        return;
    }

    int channel_id = 1;
    char line[BUF_SIZE];

    while (fgets(line, sizeof(line), file)) {
        channel_id++;
    }

    char salt[BCRYPT_HASHSIZE];
    char hashed_key[BCRYPT_HASHSIZE];
    bcrypt_gensalt(12, salt);
    bcrypt_hashpw(key, salt, hashed_key);

    fprintf(file, "%d,%s,%s\n", channel_id, channel, hashed_key);
    fclose(file);

    char channel_path[BUF_SIZE];
    snprintf(channel_path, sizeof(channel_path), "/home/kokon/FP/DiscorIT/%s", channel);

    if (mkdir(channel_path, 0700) == -1) {
        perror("mkdir channel_path");
        return;
    }

    char admin_path[BUF_SIZE];
    if (snprintf(admin_path, sizeof(admin_path), "%s/admin", channel_path) >= sizeof(admin_path)) {
        perror("snprintf admin_path");
        return;
    }
    if (mkdir(admin_path, 0700) == -1) {
        perror("mkdir admin_path");
        return;
    }

    char auth_file_path[BUF_SIZE];
    if (snprintf(auth_file_path, sizeof(auth_file_path), "%s/auth.csv", admin_path) >= sizeof(auth_file_path)) {
        perror("snprintf auth_file_path");
        return;
    }
    FILE *auth_file = fopen(auth_file_path, "a+");
    if (!auth_file) {
        perror("fopen auth_file_path");
        return;
    }

    int user_id = 0;
    FILE *user_file = fopen(USER_FILE, "r");
    if (user_file) {
        char user_line[BUF_SIZE];
        while (fgets(user_line, sizeof(user_line), user_file)) {
            char stored_username[BUF_SIZE];
            sscanf(user_line, "%d,%[^,],%*[^,],%*s", &user_id, stored_username);
            if (strcmp(stored_username, username) == 0) {
                break;
            }
        }
        fclose(user_file);
    }

    fprintf(auth_file, "%d,%s,ADMIN\n", user_id, username);
    fclose(auth_file);

    char log_file_path[BUF_SIZE];
    if (snprintf(log_file_path, sizeof(log_file_path), "%s/user.log", admin_path) >= sizeof(log_file_path)) {
        perror("snprintf log_file_path");
        return;
    }
    FILE *log_file = fopen(log_file_path, "a+");
    if (!log_file) {
        perror("fopen log_file_path");
        return;
    }

    fclose(log_file);

    char msg[BUF_SIZE];
    snprintf(msg, sizeof(msg), "Channel %s dibuat\n", channel);
    write(client_socket, msg, strlen(msg));

    char activity[BUF_SIZE];
    snprintf(activity, sizeof(activity), "%s membuat channel %s", username, channel);
    log_activity(channel, activity);
}

void edit_channel(int client_socket, const char *username, const char *old_channel, const char *new_channel) {
    if (!user_role_auth(username, old_channel, "ROOT") && !user_role_auth(username, old_channel, "ADMIN")) {
        char msg[] = "Permission denied\n";
        write(client_socket, msg, strlen(msg));
        return;
    }

    FILE *file = fopen(CHANNEL_FILE, "r+");
    if (!file) {
        perror("fopen");
        char msg[] = "Gagal membuka file channel\n";
        write(client_socket, msg, strlen(msg));
        return;
    }

    char line[BUF_SIZE];
    long pos;
    int channel_found = 0;

    while (fgets(line, sizeof(line), file)) {
        char stored_channel[BUF_SIZE], stored_key[BUF_SIZE];
        int id;
        sscanf(line, "%d,%[^,],%s", &id, stored_channel, stored_key);

        if (strcmp(stored_channel, old_channel) == 0) {
            channel_found = 1;
            pos = ftell(file) - strlen(line);
            fseek(file, pos, SEEK_SET);
            fprintf(file, "%d,%s,%s\n", id, new_channel, stored_key);
            break;
        }
    }

    fclose(file);

    if (channel_found) {
        char old_path[BUF_SIZE];
        snprintf(old_path, sizeof(old_path), "/home/kokon/FP/DiscorIT/%s", old_channel);

        char new_path[BUF_SIZE];
        snprintf(new_path, sizeof(new_path), "/home/kokon/FP/DiscorIT/%s", new_channel);

        if (rename(old_path, new_path) != 0) {
            perror("rename");
            char msg[] = "Gagal mengganti nama direktori channel\n";
            write(client_socket, msg, strlen(msg));
            return;
        }

        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "Channel %s berhasil diubah menjadi %s\n", old_channel, new_channel);
        write(client_socket, msg, strlen(msg));

        char activity[BUF_SIZE];
        snprintf(activity, sizeof(activity), "%s mengubah channel %s menjadi %s", username, old_channel, new_channel);
        log_activity(old_channel, activity);
    } else {
        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "Channel %s tidak ditemukan\n", old_channel);
        write(client_socket, msg, strlen(msg));
    }
}

void delete_channel(int client_socket, const char *username, const char *channel) {
    if (!user_role_auth(username, channel, "ROOT") && !user_role_auth(username, channel, "ADMIN")) {
        char msg[] = "Permission denied\n";
        write(client_socket, msg, strlen(msg));
        return;
    }

    FILE *file = fopen(CHANNEL_FILE, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    char temp_file[] = "/home/kokon/FP/DiscorIT/channels_temp.csv";
    FILE *temp = fopen(temp_file, "w");
    if (!temp) {
        perror("fopen");
        fclose(file);
        return;
    }

    char line[BUF_SIZE];
    int channel_found = 0;

    while (fgets(line, sizeof(line), file)) {
        char stored_channel[BUF_SIZE];
        sscanf(line, "%*d,%[^,],%*s", stored_channel);

        if (strcmp(stored_channel, channel) == 0) {
            channel_found = 1;
        } else {
            fputs(line, temp);
        }
    }

    fclose(file);
    fclose(temp);

    if (channel_found) {
        rename(temp_file, CHANNEL_FILE);

        char channel_path[BUF_SIZE];
        snprintf(channel_path, sizeof(channel_path), "/home/kokon/FP/DiscorIT/%s", channel);

        if (remove_directory(channel_path) == 0) {
            char msg[BUF_SIZE];
            snprintf(msg, sizeof(msg), "Channel %s berhasil dihapus\n", channel);
            write(client_socket, msg, strlen(msg));

            char activity[BUF_SIZE];
            snprintf(activity, sizeof(activity), "%s menghapus channel %s", username, channel);
            log_activity(username, activity);
        } else {
            char msg[] = "Gagal menghapus channel\n";
            write(client_socket, msg, strlen(msg));
        }
    } else {
        remove(temp_file);
        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "Channel %s tidak ditemukan\n", channel);
        write(client_socket, msg, strlen(msg));
    }
}

void create_room(int client_socket, const char *username, const char *channel, const char *room) {
    if (!user_role_auth(username, channel, "ROOT") && !user_role_auth(username, channel, "ADMIN")) {
        char msg[] = "Permission denied\n";
        write(client_socket, msg, strlen(msg));
        return;
    }

    char room_path[BUF_SIZE];
    snprintf(room_path, sizeof(room_path), "/home/kokon/FP/DiscorIT/%s/%s", channel, room);

    if (mkdir(room_path, 0700) == -1) {
        perror("mkdir room_path");
        return;
    }

    char chat_file_path[BUF_SIZE];
    if (snprintf(chat_file_path, sizeof(chat_file_path), "%s/chat.csv", room_path) >= sizeof(chat_file_path)) {
        perror("snprintf chat_file_path");
        return;
    }
    FILE *chat_file = fopen(chat_file_path, "a+");
    if (!chat_file) {
        perror("fopen chat_file_path");
        return;
    }
    fclose(chat_file);

    char msg[BUF_SIZE];
    snprintf(msg, sizeof(msg), "Room %s dibuat\n", room);
    write(client_socket, msg, strlen(msg));

    char activity[BUF_SIZE];
    snprintf(activity, sizeof(activity), "%s membuat room %s di channel %s", username, room, channel);
    log_activity(channel, activity);
}

void edit_room(int client_socket, const char *username, const char *channel, const char *old_room, const char *new_room) {
    if (!user_role_auth(username, channel, "ROOT") && !user_role_auth(username, channel, "ADMIN")) {
        char msg[] = "Permission denied\n";
        write(client_socket, msg, strlen(msg));
        return;
    }

    char old_path[BUF_SIZE], new_path[BUF_SIZE];
    snprintf(old_path, sizeof(old_path), "/home/kokon/FP/DiscorIT/%s/%s", channel, old_room);
    snprintf(new_path, sizeof(new_path), "/home/kokon/FP/DiscorIT/%s/%s", channel, new_room);

    if (rename(old_path, new_path) == 0) {
        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "Room %s berhasil diubah menjadi %s\n", old_room, new_room);
        write(client_socket, msg, strlen(msg));

        char activity[BUF_SIZE];
        snprintf(activity, sizeof(activity), "%s mengubah room %s menjadi %s di channel %s", username, old_room, new_room, channel);
        log_activity(channel, activity);
    } else {
        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "Room %s gagal diubah\n", old_room);
        write(client_socket, msg, strlen(msg));
    }
}

void delete_room(int client_socket, const char *username, const char *channel, const char *room) {
    if (!user_role_auth(username, channel, "ROOT") && !user_role_auth(username, channel, "ADMIN")) {
        char msg[] = "Permission denied\n";
        write(client_socket, msg, strlen(msg));
        return;
    }

    char room_path[BUF_SIZE];
    snprintf(room_path, sizeof(room_path), "/home/kokon/FP/DiscorIT/%s/%s", channel, room);

    int room_found = 0;
    struct stat st;
    if (stat(room_path, &st) == 0) {
        room_found = 1;
    }

    if (room_found) {
        if (remove_directory(room_path) == 0) {
            char msg[BUF_SIZE];
            snprintf(msg, sizeof(msg), "Room %s berhasil dihapus\n", room);
            write(client_socket, msg, strlen(msg));

            char activity[BUF_SIZE];
            snprintf(activity, sizeof(activity), "%s menghapus room %s di channel %s", username, room, channel);
            log_activity(channel, activity);
        } else {
            char msg[BUF_SIZE];
            snprintf(msg, sizeof(msg), "Room %s gagal dihapus\n", room);
            write(client_socket, msg, strlen(msg));
        }
    } else {
        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "Room %s tidak ditemukan\n", room);
        write(client_socket, msg, strlen(msg));
    }
}

void delete_all_rooms(int client_socket, const char *username, const char *channel) {
    if (!user_role_auth(username, channel, "ROOT") && !user_role_auth(username, channel, "ADMIN")) {
        char msg[] = "Permission denied\n";
        write(client_socket, msg, strlen(msg));
        return;
    }

    char channel_path[BUF_SIZE];
    snprintf(channel_path, sizeof(channel_path), "/home/kokon/FP/DiscorIT/%s", channel);

    struct dirent *entry;
    DIR *dp = opendir(channel_path);

    if (dp == NULL) {
        char msg[] = "Gagal membuka channel\n";
        write(client_socket, msg, strlen(msg));
        return;
    }

    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, "admin") != 0) {
            char room_path[BUF_SIZE];
            if (snprintf(room_path, sizeof(room_path), "%s/%s", channel_path, entry->d_name) >= sizeof(room_path)) {
                perror("snprintf room_path");
                continue;
            }
            remove_directory(room_path);
        }
    }

    closedir(dp);

    char msg[] = "Semua room dihapus\n";
    write(client_socket, msg, strlen(msg));

    char activity[BUF_SIZE];
    snprintf(activity, sizeof(activity), "%s menghapus semua room di channel %s", username, channel);
    log_activity(channel, activity);
}

int ban_user(int client_socket, const char *logged_in_user, const char *username, const char *channel) {
    if (!user_role_auth(logged_in_user, channel, "ADMIN")) {
        char msg[] = "Permission denied\n";
        write(client_socket, msg, strlen(msg));
        return 0;
    }

    char auth_file_path[BUF_SIZE];
    snprintf(auth_file_path, sizeof(auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/auth.csv", channel);
    FILE *auth_file = fopen(auth_file_path, "r");
    if (!auth_file) {
        perror("fopen");
        return 0;
    }

    char temp_auth_file_path[BUF_SIZE];
    snprintf(temp_auth_file_path, sizeof(temp_auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/temp_auth.csv", channel);
    FILE *temp_auth_file = fopen(temp_auth_file_path, "w");
    if (!temp_auth_file) {
        perror("fopen");
        fclose(auth_file);
        return 0;
    }

    int user_id;
    char line[BUF_SIZE];
    char stored_username[BUF_SIZE], stored_role[BUF_SIZE];
    int user_found = 0;

    while (fgets(line, sizeof(line), auth_file)) {
        sscanf(line, "%d,%[^,],%s", &user_id, stored_username, stored_role);
        if (strcmp(stored_username, username) == 0) {
            user_found = 1;
            fprintf(temp_auth_file, "%d,%s,BANNED\n", user_id, stored_username);
        } else {
            fprintf(temp_auth_file, "%s", line);
        }
    }

    fclose(auth_file);
    fclose(temp_auth_file);

    if (user_found) {
        remove(auth_file_path);
        rename(temp_auth_file_path, auth_file_path);

        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "%s diban\n", username);
        write(client_socket, msg, strlen(msg));
        return 1;
    } else {
        remove(temp_auth_file_path);
        char msg[] = "User tidak ditemukan\n";
        write(client_socket, msg, strlen(msg));
        return 0;
    }
}

void unban_user(int client_socket, const char *logged_in_user, const char *username, const char *channel) {
    if (!user_role_auth(logged_in_user, channel, "ADMIN")) {
        char msg[] = "Permission denied\n";
        write(client_socket, msg, strlen(msg));
        return;
    }

    char auth_file_path[BUF_SIZE];
    snprintf(auth_file_path, sizeof(auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/auth.csv", channel);
    FILE *auth_file = fopen(auth_file_path, "r");
    if (!auth_file) {
        perror("fopen");
        return;
    }

    char temp_auth_file_path[BUF_SIZE];
    snprintf(temp_auth_file_path, sizeof(temp_auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/temp_auth.csv", channel);
    FILE *temp_auth_file = fopen(temp_auth_file_path, "w");
    if (!temp_auth_file) {
        perror("fopen");
        fclose(auth_file);
        return;
    }

    int user_id;
    char line[BUF_SIZE];
    char stored_username[BUF_SIZE], stored_role[BUF_SIZE];
    int user_found = 0;

    while (fgets(line, sizeof(line), auth_file)) {
        sscanf(line, "%d,%[^,],%s", &user_id, stored_username, stored_role);
        if (strcmp(stored_username, username) == 0) {
            user_found = 1;
            fprintf(temp_auth_file, "%d,%s,USER\n", user_id, stored_username);
        } else {
            fprintf(temp_auth_file, "%s", line);
        }
    }

    fclose(auth_file);
    fclose(temp_auth_file);

    if (user_found) {
        remove(auth_file_path);
        rename(temp_auth_file_path, auth_file_path);

        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "%s kembali\n", username);
        write(client_socket, msg, strlen(msg));
    } else {
        remove(temp_auth_file_path);
        char msg[] = "User tidak ditemukan atau tidak dibanned\n";
        write(client_socket, msg, strlen(msg));
    }
}

void remove_user(int client_socket, const char *logged_in_user, const char *username, const char *channel) {
    if (!user_role_auth(logged_in_user, channel, "ADMIN")) {
        char msg[] = "Permission denied\n";
        write(client_socket, msg, strlen(msg));
        return;
    }

    char auth_file_path[BUF_SIZE];
    snprintf(auth_file_path, sizeof(auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/auth.csv", channel);
    FILE *auth_file = fopen(auth_file_path, "r");
    if (!auth_file) {
        perror("fopen");
        return;
    }

    char temp_auth_file_path[BUF_SIZE];
    snprintf(temp_auth_file_path, sizeof(temp_auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/temp_auth.csv", channel);
    FILE *temp_auth_file = fopen(temp_auth_file_path, "w");
    if (!temp_auth_file) {
        perror("fopen");
        fclose(auth_file);
        return;
    }

    int user_id;
    char line[BUF_SIZE];
    char stored_username[BUF_SIZE], stored_role[BUF_SIZE];
    int user_found = 0;

    while (fgets(line, sizeof(line), auth_file)) {
        sscanf(line, "%d,%[^,],%s", &user_id, stored_username, stored_role);
        if (strcmp(stored_username, username) == 0) {
            user_found = 1;
        } else {
            fprintf(temp_auth_file, "%s", line);
        }
    }

    fclose(auth_file);
    fclose(temp_auth_file);

    if (user_found) {
        remove(auth_file_path);
        rename(temp_auth_file_path, auth_file_path);

        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "%s dikick\n", username);
        write(client_socket, msg, strlen(msg));
    } else {
        remove(temp_auth_file_path);
        char msg[] = "User tidak ditemukan\n";
        write(client_socket, msg, strlen(msg));
    }
}

void list_user(int client_socket) {
    FILE *file = fopen(USER_FILE, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    char line[BUF_SIZE];
    char response[BUF_SIZE] = "Daftar user:\n";

    while (fgets(line, sizeof(line), file)) {
        char username[BUF_SIZE];
        sscanf(line, "%*d,%[^,],%*s", username);
        strcat(response, username);
        strcat(response, "\n");
    }

    fclose(file);
    write(client_socket, response, strlen(response));
}

void list_channels(int client_socket) {
    FILE *file = fopen(CHANNEL_FILE, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    char line[BUF_SIZE];
    char response[BUF_SIZE] = "Daftar channel:\n";

    while (fgets(line, sizeof(line), file)) {
        char channel[BUF_SIZE];
        sscanf(line, "%*d,%[^,],%*s", channel);
        strcat(response, channel);
        strcat(response, "\n");
    }

    write(client_socket, response, strlen(response));
    fclose(file);
}

void list_rooms(int client_socket, const char *channel) {
    char channel_path[BUF_SIZE];
    snprintf(channel_path, sizeof(channel_path), "/home/kokon/FP/DiscorIT/%s", channel);

    struct dirent *entry;
    DIR *dp = opendir(channel_path);

    if (dp == NULL) {
        char msg[] = "Gagal membuka channel\n";
        write(client_socket, msg, strlen(msg));
        return;
    }

    char response[BUF_SIZE] = "Daftar room:\n";
    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, "admin") != 0) {
            strcat(response, entry->d_name);
            strcat(response, "\n");
        }
    }

    write(client_socket, response, strlen(response));
    closedir(dp);
}

void list_user_room(int client_socket, const char *channel) {
    char auth_file_path[BUF_SIZE];
    snprintf(auth_file_path, sizeof(auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/auth.csv", channel);

    FILE *file = fopen(auth_file_path, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    char line[BUF_SIZE];
    char response[BUF_SIZE] = "Daftar pengguna:\n";

    while (fgets(line, sizeof(line), file)) {
        char user[BUF_SIZE * 10];
        sscanf(line, "%*d,%[^,],%*s", user);
        strcat(response, user);
        strcat(response, "\n");
    }

    write(client_socket, response, strlen(response));
    fclose(file);
}

int edit_user(int client_socket, const char *old_username, const char *new_username, int is_self_edit) {
    char logged_in_user_role[BUF_SIZE];
    FILE *user_file = fopen(USER_FILE, "r");
    if (!user_file) {
        perror("fopen");
        return 0;
    }

    char user_line[BUF_SIZE];
    while (fgets(user_line, sizeof(user_line), user_file)) {
        char stored_username[BUF_SIZE];
        sscanf(user_line, "%*d,%[^,],%*[^,],%s", stored_username, logged_in_user_role);
        if (strcmp(stored_username, logged_in_user) == 0) {
            break;
        }
    }
    fclose(user_file);

    if (!is_self_edit && strcmp(logged_in_user_role, "ROOT") != 0) {
        char msg[] = "Permission denied\n";
        write(client_socket, msg, strlen(msg));
        return 0;
    }

    user_file = fopen(USER_FILE, "r");
    char temp_user_file_path[BUF_SIZE];
    snprintf(temp_user_file_path, sizeof(temp_user_file_path), "/home/kokon/FP/DiscorIT/temp_user.csv");
    FILE *temp_file = fopen(temp_user_file_path, "w");
    if (!user_file || !temp_file) {
        perror("fopen");
        return 0;
    }

    char line[BUF_SIZE];
    int user_found = 0;
    int user_id;
    char stored_password[BUF_SIZE];
    char stored_username[BUF_SIZE];
    char stored_role[BUF_SIZE];

    while (fgets(line, sizeof(line), user_file)) {
        sscanf(line, "%d,%[^,],%[^,],%s", &user_id, stored_username, stored_password, stored_role);

        if (strcmp(stored_username, old_username) == 0) {
            user_found = 1;
            fprintf(temp_file, "%d,%s,%s,%s\n", user_id, new_username, stored_password, stored_role);
        } else {
            fprintf(temp_file, "%s", line);
        }
    }

    fclose(user_file);
    fclose(temp_file);

    remove(USER_FILE);
    rename(temp_user_file_path, USER_FILE);

    if (user_found) {
        DIR *dir;
        struct dirent *entry;

        dir = opendir("/home/kokon/FP/DiscorIT");
        if (!dir) {
            perror("opendir");
            return 0;
        }

        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char auth_file_path[BUF_SIZE];
                snprintf(auth_file_path, sizeof(auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/auth.csv", entry->d_name);
                FILE *auth_file = fopen(auth_file_path, "r");
                if (auth_file) {
                    char temp_auth_file_path[BUF_SIZE];
                    snprintf(temp_auth_file_path, sizeof(temp_auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/temp_auth.csv", entry->d_name);
                    FILE *temp_auth_file = fopen(temp_auth_file_path, "w");

                    if (!temp_auth_file) {
                        perror("fopen");
                        fclose(auth_file);
                        closedir(dir);
                        return 0;
                    }

                    while (fgets(line, sizeof(line), auth_file)) {
                        sscanf(line, "%d,%[^,],%s", &user_id, stored_username, stored_role);

                        if (strcmp(stored_username, old_username) == 0) {
                            fprintf(temp_auth_file, "%d,%s,%s\n", user_id, new_username, stored_role);
                        } else {
                            fprintf(temp_auth_file, "%s", line);
                        }
                    }

                    fclose(auth_file);
                    fclose(temp_auth_file);

                    remove(auth_file_path);
                    rename(temp_auth_file_path, auth_file_path);
                }
            }
        }

        closedir(dir);

        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "%s berhasil diubah menjadi %s\n", old_username, new_username);
        write(client_socket, msg, strlen(msg));
        return 1;
    } else {
        write(client_socket, "User tidak ditemukan\n", strlen("User tidak ditemukan\n"));
        return 0;
    }
}

void edit_user_password(int client_socket, const char *username, const char *new_password, int is_self_edit) {
    char logged_in_user_role[BUF_SIZE];

    FILE *user_file = fopen(USER_FILE, "r");
    if (!user_file) {
        perror("fopen");
        return;
    }

    char user_line[BUF_SIZE];
    while (fgets(user_line, sizeof(user_line), user_file)) {
        char stored_username[BUF_SIZE];
        sscanf(user_line, "%*d,%[^,],%*[^,],%s", stored_username, logged_in_user_role);
        if (strcmp(stored_username, logged_in_user) == 0) {
            break;
        }
    }
    fclose(user_file);

    if (!is_self_edit && strcmp(logged_in_user_role, "ROOT") != 0) {
        char msg[] = "Permission denied\n";
        write(client_socket, msg, strlen(msg));
        return;
    }

    user_file = fopen(USER_FILE, "r+");
    if (!user_file) {
        perror("fopen");
        return;
    }

    char line[BUF_SIZE];
    char temp_file[] = "/home/kokon/FP/DiscorIT/temp_user.csv";
    FILE *temp = fopen(temp_file, "w");

    if (!temp) {
        perror("fopen");
        return;
    }

    int user_found = 0;
    char salt[BCRYPT_HASHSIZE];
    char hashed_password[BCRYPT_HASHSIZE];
    bcrypt_gensalt(12, salt);
    bcrypt_hashpw(new_password, salt, hashed_password);
    while (fgets(line, sizeof(line), user_file)) {
        char stored_username[BUF_SIZE];
        sscanf(line, "%*d,%[^,],%*s", stored_username);

        if (strcmp(stored_username, username) == 0) {
            user_found = 1;
            int id;
            char password[BUF_SIZE], role[BUF_SIZE];
            sscanf(line, "%d,%[^,],%[^,],%s", &id, stored_username, password, role);
            fprintf(temp, "%d,%s,%s,%s\n", id, stored_username, hashed_password, role);
        } else {
            fputs(line, temp);
        }
    }

    fclose(user_file);
    fclose(temp);

    remove(USER_FILE);
    rename(temp_file, USER_FILE);

    if (user_found) {
        char response[BUF_SIZE];
        snprintf(response, sizeof(response), "Password %s berhasil diubah\n", username);
        write(client_socket, response, strlen(response));
    } else {
        write(client_socket, "User tidak ditemukan\n", strlen("User tidak ditemukan\n"));
    }
}

int remove_user_root(int client_socket, const char *username) {
    char logged_in_user_role[BUF_SIZE];
    FILE *user_file = fopen(USER_FILE, "r");
    if (!user_file) {
        perror("fopen");
        return 0;
    }

    char user_line[BUF_SIZE];
    while (fgets(user_line, sizeof(user_line), user_file)) {
        char stored_username[BUF_SIZE];
        sscanf(user_line, "%*d,%[^,],%*[^,],%s", stored_username, logged_in_user_role);
        if (strcmp(stored_username, logged_in_user) == 0) {
            break;
        }
    }
    fclose(user_file);

    if (strcmp(logged_in_user_role, "ROOT") != 0) {
        char msg[] = "Permission denied\n";
        write(client_socket, msg, strlen(msg));
        return 0;
    }

    user_file = fopen(USER_FILE, "r");
    char temp_user_file_path[BUF_SIZE];
    snprintf(temp_user_file_path, sizeof(temp_user_file_path), "/home/kokon/FP/DiscorIT/temp_user.csv");
    FILE *temp_file = fopen(temp_user_file_path, "w");
    if (!user_file || !temp_file) {
        perror("fopen");
        return 0;
    }

    char line[BUF_SIZE];
    int user_found = 0;
    int user_id;
    char stored_password[BUF_SIZE];
    char stored_username[BUF_SIZE];
    char stored_role[BUF_SIZE];

    while (fgets(line, sizeof(line), user_file)) {
        sscanf(line, "%d,%[^,],%s", &user_id, stored_username, stored_password);

        if (strcmp(stored_username, username) == 0) {
            user_found = 1;
        } else {
            fprintf(temp_file, "%s", line);
        }
    }

    fclose(user_file);
    fclose(temp_file);

    if (user_found) {
        remove(USER_FILE);
        rename(temp_user_file_path, USER_FILE);

        DIR *dir;
        struct dirent *entry;

        dir = opendir("/home/kokon/FP/DiscorIT");
        if (!dir) {
            perror("opendir");
            return 0;
        }

        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char auth_file_path[BUF_SIZE];
                snprintf(auth_file_path, sizeof(auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/auth.csv", entry->d_name);
                FILE *auth_file = fopen(auth_file_path, "r");
                if (auth_file) {
                    char temp_auth_file_path[BUF_SIZE];
                    snprintf(temp_auth_file_path, sizeof(temp_auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/temp_auth.csv", entry->d_name);
                    FILE *temp_auth_file = fopen(temp_auth_file_path, "w");

                    if (!temp_auth_file) {
                        perror("fopen");
                        fclose(auth_file);
                        closedir(dir);
                        return 0;
                    }

                    while (fgets(line, sizeof(line), auth_file)) {
                        sscanf(line, "%d,%[^,],%s", &user_id, stored_username, stored_role);

                        if (strcmp(stored_username, username) == 0) {
                        } else {
                            fprintf(temp_auth_file, "%s", line);
                        }
                    }

                    fclose(auth_file);
                    fclose(temp_auth_file);

                    remove(auth_file_path);
                    rename(temp_auth_file_path, auth_file_path);
                }
            }
        }

        closedir(dir);

        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "%s berhasil dihapus\n", username);
        write(client_socket, msg, strlen(msg));
        return 1;
    } else {
        write(client_socket, "User tidak ditemukan\n", strlen("User tidak ditemukan\n"));
        return 0;
    }
}

int join_channel(int client_socket, const char *username, const char *channel, const char *key) {
    FILE *file = fopen(CHANNEL_FILE, "r");
    if (!file) {
        perror("fopen");
        return 0;
    }

    char line[BUF_SIZE];
    int channel_found = 0;
    int user_in_auth = 0;
    char user_role[BUF_SIZE] = "";

    FILE *user_file = fopen(USER_FILE, "r");
    if (!user_file) {
        perror("fopen");
        fclose(file);
        return 0;
    }

    char user_line[BUF_SIZE];
    while (fgets(user_line, sizeof(user_line), user_file)) {
        char stored_username[BUF_SIZE];
        sscanf(user_line, "%*d,%[^,],%*[^,],%s", stored_username, user_role);
        if (strcmp(stored_username, username) == 0) {
            break;
        }
    }
    fclose(user_file);

    if (strcmp(user_role, "ROOT") == 0 || user_role_auth(username, channel, "ADMIN")) {
        channel_found = 1;

        char auth_file_path[BUF_SIZE];
        snprintf(auth_file_path, sizeof(auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/auth.csv", channel);
        FILE *auth_file = fopen(auth_file_path, "r");
        if (!auth_file) {
            perror("fopen");
            fclose(file);
            return 0;
        }

        char auth_line[BUF_SIZE];
        while (fgets(auth_line, sizeof(auth_line), auth_file)) {
            char stored_user[BUF_SIZE];
            sscanf(auth_line, "%*d,%[^,],%*s", stored_user);
            if (strcmp(stored_user, username) == 0) {
                user_in_auth = 1;
                break;
            }
        }
        fclose(auth_file);

        if (!user_in_auth) {
            auth_file = fopen(auth_file_path, "a");
            if (!auth_file) {
                perror("fopen");
                fclose(file);
                return 0;
            }

            int user_id = 0;
            user_file = fopen(USER_FILE, "r");
            if (user_file) {
                while (fgets(user_line, sizeof(user_line), user_file)) {
                    char stored_username[BUF_SIZE];
                    sscanf(user_line, "%d,%[^,],%*[^,],%*s", &user_id, stored_username);
                    if (strcmp(stored_username, username) == 0) {
                        break;
                    }
                }
                fclose(user_file);
            }

            fprintf(auth_file, "%d,%s,%s\n", user_id, username, user_role);
            fclose(auth_file);
        }
    } else if (user_role_auth(username, channel, "BANNED")) {
        write(client_socket, "anda telah diban, silahkan menghubungi admin\n", strlen("anda telah diban, silahkan menghubungi admin\n"));
        return 0;
    } else {
        char auth_file_path[BUF_SIZE];
        snprintf(auth_file_path, sizeof(auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/auth.csv", channel);
        FILE *auth_file = fopen(auth_file_path, "r");
        if (!auth_file) {
            perror("fopen");
            fclose(file);
            return 0;
        }

        int user_found_in_auth = 0;
        char auth_line[BUF_SIZE];
        while (fgets(auth_line, sizeof(auth_line), auth_file)) {
            char stored_user[BUF_SIZE];
            sscanf(auth_line, "%*d,%[^,],%*s", stored_user);
            if (strcmp(stored_user, username) == 0) {
                user_found_in_auth = 1;
                break;
            }
        }
        fclose(auth_file);

        if (user_found_in_auth) {
            channel_found = 1;
        } else {
            while (fgets(line, sizeof(line), file)) {
                char stored_channel[BUF_SIZE], stored_key[BUF_SIZE];
                sscanf(line, "%*d,%[^,],%s", stored_channel, stored_key);

                if (strcmp(stored_channel, channel) == 0) {
                    channel_found = 1;
                    if (key == NULL || bcrypt_checkpw(key, stored_key) != 0) {
                        write(client_socket, "Key: ", strlen("Key: "));
                        char input_key[BUF_SIZE];
                        int valread = read(client_socket, input_key, BUF_SIZE);
                        input_key[valread] = '\0';
                        if (bcrypt_checkpw(input_key, stored_key) != 0) {
                            write(client_socket, "Key salah\n", strlen("Key salah\n"));
                            fclose(file);
                            return 0;
                        }
                    }

                    auth_file = fopen(auth_file_path, "a");
                    if (!auth_file) {
                        perror("fopen");
                        fclose(file);
                        return 0;
                    }

                    int user_id = 0;
                    user_file = fopen(USER_FILE, "r");
                    if (user_file) {
                        while (fgets(user_line, sizeof(user_line), user_file)) {
                            char stored_username[BUF_SIZE];
                            sscanf(user_line, "%d,%[^,],%*[^,],%*s", &user_id, stored_username);
                            if (strcmp(stored_username, username) == 0) {
                                break;
                            }
                        }
                        fclose(user_file);
                    }

                    fprintf(auth_file, "%d,%s,USER\n", user_id, username);
                    fclose(auth_file);
                    break;
                }
            }
        }
    }
    fclose(file);

    if (channel_found) {
        char msg[BUF_SIZE];
        snprintf(msg, sizeof(msg), "[%s/%s] ", username, channel);
        write(client_socket, msg, strlen(msg));

        snprintf(msg, sizeof(msg), "Berhasil masuk ke channel %s\n", channel);
        write(client_socket, msg, strlen(msg));

        char activity[BUF_SIZE];
        snprintf(activity, sizeof(activity), "%s bergabung dengan channel %s", username, channel);
        log_activity(channel, activity);
        return 1;
    } else {
        write(client_socket, "Channel tidak ditemukan\n", strlen("Channel tidak ditemukan\n"));
        return 0;
    }
}

int join_room(int client_socket, const char *username, const char *channel, const char *room) {
    char room_path[BUF_SIZE];
    snprintf(room_path, sizeof(room_path), "/home/kokon/FP/DiscorIT/%s/%s", channel, room);

    struct stat st;
    if (stat(room_path, &st) == -1) {
        write(client_socket, "Room tidak ditemukan\n", strlen("Room tidak ditemukan\n"));
        return 0;
    }

    char msg[BUF_SIZE];
    snprintf(msg, sizeof(msg), "[%s/%s/%s] ", username, channel, room);
    write(client_socket, msg, strlen(msg));

    snprintf(msg, sizeof(msg), "Berhasil masuk ke room %s\n", room);
    write(client_socket, msg, strlen(msg));

    char activity[BUF_SIZE];
    snprintf(activity, sizeof(activity), "%s bergabung dengan room %s di channel %s", username, room, channel);
    log_activity(channel, activity);
    return 1;
}

void chat(int client_socket, const char *username, const char *channel, const char *room, const char *text) {
    char chat_file_path[BUF_SIZE];
    snprintf(chat_file_path, sizeof(chat_file_path), "/home/kokon/FP/DiscorIT/%s/%s/chat.csv", channel, room);

    FILE *file = fopen(chat_file_path, "a+");
    if (!file) {
        perror("fopen");
        return;
    }

    int chat_id = 1;
    char line[BUF_SIZE];

    while (fgets(line, sizeof(line), file)) {
        chat_id++;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", t);

    fprintf(file, "[%s][%d][%s] \"%s\"\n", time_str, chat_id, username, text);
    fclose(file);

    char msg[BUF_SIZE];
    snprintf(msg, sizeof(msg), "Pesan dikirim\n");
    write(client_socket, msg, strlen(msg));

    char activity[BUF_SIZE];
    snprintf(activity, sizeof(activity), "%s mengirim pesan di room %s channel %s: %s", username, room, channel, text);
    log_activity(channel, activity);
}

void see_chat(int client_socket, const char *channel, const char *room) {
    char chat_file_path[BUF_SIZE];
    snprintf(chat_file_path, sizeof(chat_file_path), "/home/kokon/FP/DiscorIT/%s/%s/chat.csv", channel, room);

    FILE *file = fopen(chat_file_path, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    char line[BUF_SIZE];
    char response[BUF_SIZE * 10] = "";

    while (fgets(line, sizeof(line), file)) {
        strcat(response, line);
    }

    if (strlen(response) == 0) {
        snprintf(response, sizeof(response), "Tidak ada pesan di room %s channel %s\n", room, channel);
    }

    fclose(file);

    write(client_socket, response, strlen(response));
}

void edit_chat(int client_socket, const char *username, const char *channel, const char *room, int chat_id, const char *text) {
    char chat_file_path[BUF_SIZE];
    snprintf(chat_file_path, sizeof(chat_file_path), "/home/kokon/FP/DiscorIT/%s/%s/chat.csv", channel, room);

    FILE *file = fopen(chat_file_path, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    char temp_chat_file_path[BUF_SIZE];
    snprintf(temp_chat_file_path, sizeof(temp_chat_file_path), "/home/kokon/FP/DiscorIT/%s/%s/temp_chat.csv", channel, room);
    FILE *temp = fopen(temp_chat_file_path, "w");
    if (!temp) {
        perror("fopen");
        fclose(file);
        return;
    }

    char line[BUF_SIZE];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        int id;
        char stored_username[BUF_SIZE], stored_text[BUF_SIZE];
        sscanf(line, "[%*[^]]][%d][%[^]]]", &id, stored_username);

        if (id == chat_id && strcmp(stored_username, username) == 0) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char time_str[20];
            strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", t);

            fprintf(temp, "[%s][%d][%s] \"%s\"\n", time_str, chat_id, username, text);
            found = 1;
        } else {
            fputs(line, temp);
        }
    }

    fclose(file);
    fclose(temp);

    remove(chat_file_path);
    rename(temp_chat_file_path, chat_file_path);

    if (found) {
        write(client_socket, "Pesan berhasil diedit\n", strlen("Pesan berhasil diedit\n"));
    } else {
        write(client_socket, "Pesan tidak ditemukan\n", strlen("Pesan tidak ditemukan\n"));
    }
}

void del_chat(int client_socket, const char *username, const char *channel, const char *room, int chat_id) {
    char chat_file_path[BUF_SIZE];
    snprintf(chat_file_path, sizeof(chat_file_path), "/home/kokon/FP/DiscorIT/%s/%s/chat.csv", channel, room);

    FILE *file = fopen(chat_file_path, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    char temp_chat_file_path[BUF_SIZE];
    snprintf(temp_chat_file_path, sizeof(temp_chat_file_path), "/home/kokon/FP/DiscorIT/%s/%s/temp_chat.csv", channel, room);
    FILE *temp = fopen(temp_chat_file_path, "w");
    if (!temp) {
        perror("fopen");
        fclose(file);
        return;
    }

    char line[BUF_SIZE];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        int id;
        char stored_username[BUF_SIZE];
        sscanf(line, "[%*[^]]][%d][%[^]]]", &id, stored_username);

        if (id == chat_id && strcmp(stored_username, username) == 0) {
            found = 1;
        } else {
            fputs(line, temp);
        }
    }

    fclose(file);
    fclose(temp);

    remove(chat_file_path);
    rename(temp_chat_file_path, chat_file_path);

    if (found) {
        write(client_socket, "Pesan berhasil dihapus\n", strlen("Pesan berhasil dihapus\n"));
    } else {
        write(client_socket, "Pesan tidak ditemukan\n", strlen("Pesan tidak ditemukan\n"));
    }
}

void initialize_directories() {
    mkdir("/home/kokon/FP/DiscorIT", 0700);
    FILE *user_file = fopen(USER_FILE, "a+");
    if (!user_file) {
        perror("fopen user_file");
        exit(EXIT_FAILURE);
    }
    fclose(user_file);

    FILE *channel_file = fopen(CHANNEL_FILE, "a+");
    if (!channel_file) {
        perror("fopen channel_file");
        exit(EXIT_FAILURE);
    }
    fclose(channel_file);
}

int user_role_auth(const char *username, const char *channel, const char *role) {
    char auth_file_path[BUF_SIZE];
    snprintf(auth_file_path, sizeof(auth_file_path), "/home/kokon/FP/DiscorIT/%s/admin/auth.csv", channel);
    FILE *file = fopen(auth_file_path, "r");
    if (!file) {
        perror("fopen");
        return 0;
    }

    char line[BUF_SIZE];
    int role_match = 0;

    while (fgets(line, sizeof(line), file)) {
        char stored_username[BUF_SIZE], stored_role[BUF_SIZE];
        sscanf(line, "%*d,%[^,],%s", stored_username, stored_role);

        if (strcmp(stored_username, username) == 0 && strcmp(stored_role, role) == 0) {
            role_match = 1;
            break;
        }
    }

    fclose(file);
    return role_match;
}

void log_activity(const char *channel, const char *activity) {
    char log_file_path[BUF_SIZE];
    snprintf(log_file_path, sizeof(log_file_path), "/home/kokon/FP/DiscorIT/%s/admin/user.log", channel);

    FILE *file = fopen(log_file_path, "a");
    if (!file) {
        perror("fopen");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", t);

    fprintf(file, "[%s] %s\n", time_str, activity);
    fclose(file);
}

int remove_directory(const char *path) {
    struct dirent *entry;
    char full_path[BUF_SIZE];
    DIR *dir = opendir(path);

    if (!dir) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            if (entry->d_type == DT_DIR) {
                remove_directory(full_path);
            } else {
                unlink(full_path);
            }
        }
    }

    closedir(dir);
    rmdir(path);
    return 0;
}
