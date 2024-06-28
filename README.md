# Sisop-FP-2024-MH-IT01

## Anggota
### Nayla Raissa Azzahra (5027231054)
### Ryan Adya Purwanto (5027231046)
### Rafael Gunawan (5027231019)

## Ketentuan
### Struktur repository seperti berikut : 
    -fp/
        ---discorit.c 
        ---monitor.c 
        ---server.c 

## discorit.c
### Penjelasan Kode
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    
    #define PORT 8080
    #define BUF_SIZE 1024
    #define PROMPT_SIZE 4096
#### Bagian ini merupakan pustaka yang diperlukan, seperti stdio.h untuk input dan output, stdlib.h untuk fungsi umum, string.h untuk manipulasi string, unistd.h untuk fungsionalitas POSIX, dan arpa/inet.h untuk fungsionalitas jaringan. Kemudian, mendefinisikan beberapa konstanta: PORT diatur ke 8080 untuk menentukan port server, BUF_SIZE diatur ke 1024 untuk ukuran buffer data, dan PROMPT_SIZE diatur ke 4096 untuk ukuran buffer prompt interaktif.

    void send_request(int sock, const char *request);
    void interactive_mode(int sock, const char *username);
#### Dalam bagian ini, dua fungsi di declare yaitu send_request untuk mengirim permintaan ke server dan interactive_mode untuk menangani interaksi setelah pengguna berhasil login. Deklarasi ini memungkinkan fungsi-fungsi ini digunakan di seluruh program.

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
#### Fungsi main dimulai dengan memeriksa apakah jumlah argumen yang diberikan sudah cukup. Jika tidak, program akan mencetak pesan penggunaan yang benar dan keluar. Selanjutnya, buffer request diinisialisasi dengan nol. Berdasarkan argumen yang diberikan, program membentuk permintaan yang sesuai menggunakan snprintf untuk berbagai perintah seperti "REGISTER", "LOGIN", dan lainnya. Jika format perintah tidak valid, program akan mencetak pesan kesalahan dan keluar. Kode kemudian membuat soket, mengatur alamat server, dan mencoba untuk terhubung ke server. Jika ada kesalahan pada salah satu langkah ini, pesan kesalahan akan dicetak dan program akan berhenti. Setelah terhubung, fungsi send_request digunakan untuk mengirim permintaan ke server. Jika permintaan adalah "LOGIN", program akan masuk ke mode interaktif menggunakan fungsi interactive_mode. Terakhir, soket akan ditutup.

    void send_request(int sock, const char *request) {
        char buffer[BUF_SIZE] = {0};
        send(sock, request, strlen(request), 0);
        read(sock, buffer, BUF_SIZE);
        printf("%s", buffer);
    }
#### Fungsi send_request mengirim permintaan ke server dan menunggu respons. Buffer buffer digunakan untuk menyimpan data yang diterima dari server, dan kemudian data ini dicetak ke layar. Fungsi ini memastikan bahwa permintaan dikirim dan respons dari server ditampilkan.

    void interactive_mode(int sock, const char *username) {
        char input[BUF_SIZE];
        char buffer[BUF_SIZE];
        char prompt[PROMPT_SIZE];  
        char current_channel[BUF_SIZE] = "";
        char current_room[BUF_SIZE] = "";
    
        while (1) {
            printf("%s", prompt);
            if (fgets(input, BUF_SIZE, stdin) == NULL) {
                break;
            }
            input[strcspn(input, "\n")] = 0;  
            send(sock, input, strlen(input), 0);
            int valread = read(sock, buffer, BUF_SIZE);
            buffer[valread] = '\0';
            printf("%s\n", buffer);
    
            if (strncmp(input, "JOIN ", 5) == 0) {
                char join_type[BUF_SIZE];
                sscanf(input, "JOIN %s", join_type);
    
                if (strcmp(current_channel, "") == 0 && strcmp(current_room, "") == 0) {
                    strcpy(current_channel, join_type);  
                } else if (strcmp(current_room, "") == 0) {
                    strcpy(current_room, join_type);  
                }
            }
    
            if (current_room[0] != '\0') {
                snprintf(prompt, sizeof(prompt), "[%s/%s/%s] ", username, current_channel, current_room);
            } else if (current_channel[0] != '\0') {
                snprintf(prompt, sizeof(prompt), "[%s/%s] ", username, current_channel);
            } else {
                snprintf(prompt, sizeof(prompt), "[%s] ", username);
            }
    
            if (strcmp(input, "EXIT") == 0) {
                if (strcmp(current_channel, "") == 0 && strcmp(current_room, "") == 0) {
                    break; 
                } else if (strcmp(current_room, "") == 0) {
                    strcpy(current_channel, ""); 
                } else {
                    strcpy(current_room, ""); 
                }
            }
        }
    }
#### Fungsi interactive_mode mengelola mode interaktif setelah pengguna login. Pengguna dapat memasukkan perintah yang dikirim ke server, dan respons dari server ditampilkan. Prompt diperbarui berdasarkan channel dan room yang sedang aktif. Jika pengguna memasukkan "EXIT", maka pengguna akan keluar dari channel/room atau keluar dari mode interaktif jika tidak ada channel/room yang aktif. Buffer input digunakan untuk menyimpan input dari pengguna, sementara buffer buffer digunakan untuk menyimpan respons dari server. 

## monitor.c
### Penjelasan Kode
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
#### Bagian ini merupakan pustaka yang diperlukan untuk berbagai fungsi seperti input/output (stdio.h), manajemen memori (stdlib.h), manipulasi string (string.h), operasi sistem (unistd.h), jaringan (arpa/inet.h), threading (pthread.h), dan fungsi waktu (time.h). Konstanta BUFFER_SIZE diatur ke 1024 untuk ukuran buffer data, dan PORT diatur ke 8080 untuk port server.

    int sockfd;
    char username[50];
    char channel[50] = "";
    char room[50] = "";
    bool in_room = false;
    bool running = true;
#### Ini adalah variabel global yang dideklarasikan untuk menyimpan deskriptor soket (sockfd), nama pengguna (username), nama channel (channel), nama room (room), status apakah berada di dalam room (in_room), dan status apakah program sedang berjalan (running).

void get_timestamp(char *buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, buffer_size, "[%d/%m/%Y %H:%M:%S]", t);
}
#### Fungsi get_timestamp digunakan untuk mendapatkan timestamp saat ini dalam format [dd/mm/yyyy hh:mm:ss]. Ini menggunakan fungsi time untuk mendapatkan waktu saat ini, localtime untuk mengonversinya ke waktu lokal, dan strftime untuk memformatnya.

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
    
            if (strncmp(buffer, "Invalid command", 15) != 0) {
                printf("%s\n", buffer);
            }
        }
        return NULL;
    }
#### Fungsi receive_messages dijalankan sebagai thread untuk menerima pesan dari server secara terus-menerus selama program berjalan (running). Jika koneksi terputus, program akan menutup soket dan keluar. Pesan yang diterima hanya dicetak jika bukan perintah "Invalid command".

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
#### Fungsi handle_account menangani proses login dengan mengirimkan informasi login ke server dan menunggu respons. Jika login berhasil, fungsi mengembalikan 1, sebaliknya 0.

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
#### Fungsi connect_server membuat koneksi ke server. Jika pembuatan soket atau koneksi ke server gagal, fungsi akan mencetak pesan kesalahan dan mengembalikan 0. Jika berhasil, fungsi mengembalikan 1.

    void clear_terminal() {
        printf("\033[H\033[J");
    }
#### Fungsi clear_terminal membersihkan terminal dengan mencetak karakter pelarian ANSI yang membersihkan layar.

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
#### Fungsi display_chat_history menampilkan riwayat obrolan dari file yang ditentukan oleh filepath. Jika file tidak dapat dibuka, fungsi mencetak pesan kesalahan. Setiap baris dari file dibaca dan dicetak ke layar.

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
#### Fungsi input_handler dijalankan sebagai thread untuk menangani input pengguna. Jika pengguna memasukkan "EXIT", fungsi akan mengatur ulang variabel terkait dan mengirimkan perintah "EXIT" ke server, kemudian menghentikan program. Jika pengguna memasukkan perintah untuk bergabung ke channel dan room, fungsi akan memperbarui variabel channel dan room, menampilkan riwayat obrolan, dan memperbarui prompt. Input lain akan dikirim langsung ke server.

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
#### Fungsi main memulai dengan memeriksa argumen yang diberikan. Jika argumen tidak valid, program mencetak pesan penggunaan dan keluar. Selanjutnya, nama pengguna dan channel disimpan dalam variabel global. Fungsi connect_server dipanggil untuk menghubungkan ke server, dan handle_account untuk menangani proses login. Jika koneksi atau login gagal, program mencetak pesan kesalahan dan keluar. Dua thread kemudian dibuat: satu untuk menerima pesan (recv_thread) dan satu untuk menangani input pengguna (input_thread). Fungsi pthread_join digunakan untuk menunggu input_thread selesai, dan pthread_cancel untuk menghentikan recv_thread setelahnya. Program kemudian berakhir.

## server.c
### Penjelasan kode
```bash
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
```
Bagian di atas berfungsi untuk mendefinisikan beberapa header yang diperlukan dalam program, menetapkan port yang digunakan server untuk menerima koneksi, ukuran buffer untuk pengoperasian file, mendefinisikan path ke file users.csv dan channels.csv, menetapkan panjang maksimum username user, dan mendeklarasikan variabel global logged_in_user yang akan digunakan dalam beberapa fungsi dalam program.
```bash
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
```
Deklarasi fungsi-fungsi apa saja yang akan digunakan dalam program.
```bash
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
```
Fungsi main dalam server.c berfungsi untuk menginisialisasi dan menjalankan server TCP yang mendengarkan koneksi masuk dari client. 
* Mendeklarasikan variabel yang diperlukan untuk socket server, socket client, alamat server, opsi socket, ukuran alamat, dan ID thread.
* Memanggil fungsi initialize_directories() untuk memastikan struktur direktori yang diperlukan ada.
* Membuat socket server menggunakan socket().
* Mengatur alamat dan port server menggunakan struktur sockaddr_in.
* Mengikat socket server ke alamat dan port yang telah ditentukan menggunakan bind().
* Memasukkan socket ke mode mendengarkan menggunakan listen().
* Dalam loop, menerima koneksi masuk dari client menggunakan accept(). Untuk setiap koneksi client yang diterima, membuat thread baru untuk menangani client tersebut menggunakan pthread_create(), dengan menjalankan fungsi handle_client.
* Jika ada kesalahan dalam menerima koneksi, mencetak pesan kesalahan dan menghentikan program.
```bash
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
```
Fungsi handle_client adalah fungsi yang digunakan untuk menangani setiap koneksi klien secara individual dalam aplikasi server DiscorIT.
* Menginisialisasi beberapa variabel yaitu client_socket, variabel untuk menyimpan buffer, nama channel, room, dan username.
* Dalam loop utama, akan dibaca perintah dari klien ke dalam buffer. Jika perintah adalah EXIT, maka pengguna keluar dari room atau channel saat ini, atau keluar dari sistem jika tidak berada di dalam room atau channel. Jika perintah bukan EXIT, maka akan diproses berbagai perintah seperti REGISTER, LOGIN, LIST USER, EDIT PROFILE, CREATE CHANNEL, JOIN, BAN, CHAT, dll. Program menggunakan sscanf untuk memparsing perintah dan argumen yang diterima.
* Menutup socket klien setelah keluar dari loop.
```bash
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
```
Fungsi register_user bertujuan untuk mendaftarkan pengguna baru ke sistem. 
* Membuka file yang menyimpan informasi pengguna (USER_FILE) dalam mode baca dan tulis (r+). Jika file tidak dapat dibuka, fungsi akan mengeluarkan pesan kesalahan dan keluar.
* Menginsialisasi variabel-variabel yang akan digunakan dalam fungsi ini, yaitu line untuk membaca baris dari file, user_exist, is_first_user untuk menandaia apakah user ROOT atau USER biasa, dan user_id.
* Membaca setiap baris di file dan mengambil user_id dan stored_username, lalu membandingkan stored_username dengan username yang diberikan. Jika cocok, berarti pengguna sudah ada dan flag user_exists diatur ke 1, serta is_first_user ke 0 setelah membaca baris pertama.
* Jika pengguna sudah terdaftar, maka akan dikirim pesan ke klien bahwa pengguna sudah terdaftar.
* Jika pengguna belum terdaftar, akan ditambahkan user_id untuk pengguna baru, menentukan peran (role) sebagai "ROOT" jika ini pengguna pertama, dan "USER" untuk pengguna berikutnya. Kemudian, menghasilkan salt dan hash password menggunakan bcrypt, menulis informasi pengguna baru ke file, dan mengirim pesan ke klien bahwa pengguna berhasil mendaftar.
* Menutup file setelah semuanya berhasil dijalankan.
```bash
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
```
Fungsi login_user bertujuan untuk memverifikasi kredensial login pengguna. 
* Membuka file yang menyimpan informasi pengguna (USER_FILE) dalam mode baca. Jika gagal membuka file, mencetak pesan error dan keluar dari fungsi.
* Menginisialisasi variabel line untuk menyimpan setiap baris yang dibaca dari file dan login_success untuk menandai apakah login berhasil (0 = gagal, 1 = berhasil).
* Menggunakan fgets untuk membaca setiap baris dari file, lalu memisahkan username dan password yang disimpan dalam file menggunakan sscanf.
* Memeriksa apakah username yang dimasukkan sama dengan username yang disimpan dan memverifikasi password menggunakan bcrypt_checkpw. Jika cocok, mengatur login_success ke 1 dan keluar dari loop.
* Jika login berhasil, membuat pesan keberhasilan login dan mengirimkannya ke client. Menyimpan username ke logged_in_user. Jika login gagal, mengirim pesan kegagalan login ke client.
* Menutup file setelah selesai digunakan.
```bash
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
```
Fungsi create_channel bertugas membuat channel baru di layanan DiscorIT, termasuk menambahkan channel ke dalam berkas, membuat direktori yang diperlukan, serta menambahkan pengguna sebagai admin.
* Berkas yang menyimpan daftar channel dibuka dengan mode "a+" (append dan read). Jika gagal, fungsi berakhir dengan pesan error.
* ID Channel baru akan dihitung. ID channel diinisialisasi dengan nilai 1. Setiap baris di berkas dibaca untuk menghitung ID berikutnya.
* Kunci channel yang dimasukkan oleh admin channel di-hash menggunakan bcrypt dan disimpan di berkas channel bersama dengan ID dan nama channel.
* Direktori baru untuk channel dibuat. Jika gagal, fungsi berakhir dengan pesan error.
* Direktori admin dan berkas auth.csv di dalamnya dibuat untuk menyimpan informasi pengguna yang berhak mengakses channel sebagai admin. File log dibuat di direktori admin.
* Pengguna ditambahkan ke berkas auth.csv dengan peran sebagai ADMIN. ID pengguna didapat dari file users.csv.
* Pesan bahwa channel sukses dibuat akan dikirimkan ke client.
* Aktivitas pembuatan channel akan dicatat ke dalam file log.
```bash
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
```
Fungsi edit_channel berfungsi untuk mengubah nama channel yang ada di sistem.
* Mengecek apakah pengguna memiliki peran "ROOT" atau "ADMIN" di channel lama. Jika tidak, mengirim pesan "Permission denied" ke klien dan mengembalikan kontrol.
* Membuka file channel (CHANNEL_FILE) dalam mode baca-tulis. Jika gagal membuka file, mengirim pesan error ke klien.
* Membaca setiap baris dalam file untuk mencari channel lama. Jika channel ditemukan, mengganti nama direktori channel lama menjadi nama direktori channel baru. Jika gagal, mengirim pesan error ke klien.
* Jika berhasil, mengirim pesan keberhasilan ke klien dan mencatat aktivitas pengubahan channel. Jika tidak ditemukan, mengirim pesan bahwa channel tidak ditemukan.
```bash
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
```
Fungsi delete_channel bertugas untuk menghapus sebuah channel di DiscorIT, jika user yang melakukan permintaan memiliki otorisasi yang tepat. 
* Memeriksa apakah pengguna memiliki peran "ROOT" atau "ADMIN" pada channel tersebut. Jika tidak, kirim pesan "Permission denied" dan keluar dari fungsi.
* Membuka file yang berisi daftar channel. Jika gagal membuka file, cetak pesan kesalahan dan keluar dari fungsi.
* Membuka file sementara untuk menulis data channel yang tidak dihapus. Jika gagal membuka file sementara, cetak pesan kesalahan, tutup file channel, dan keluar dari fungsi.
* Membaca setiap baris dari file channel dan mengecek apakah channel yang sedang dibaca adalah channel yang ingin dihapus. Jika channel tersebut ditemukan, tandai dengan channel_found = 1. Jika bukan, salin baris tersebut ke file sementara.
* Jika channel ditemukan, ganti file channel lama dengan file baru yang sudah disalin. Hapus direktori channel dan kirim pesan berhasil atau gagal ke client. Jika channel tidak ditemukan, hapus file sementara dan kirim pesan bahwa channel tidak ditemukan.
```bash
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
```
Fungsi create_room bertujuan untuk membuat room baru dalam sebuah channel, dengan memeriksa terlebih dahulu apakah pengguna memiliki izin yang cukup (ROOT atau ADMIN).
* Fungsi memeriksa apakah username memiliki peran ROOT atau ADMIN di channel menggunakan fungsi user_role_auth. Jika pengguna tidak memiliki izin yang cukup, kirim pesan "Permission denied" ke klien dan keluar dari fungsi.
* Buat path direktori untuk ruangan baru dengan format /home/kokon/FP/DiscorIT/<channel>/<room>.
* Buat path untuk file chat chat.csv dalam direktori ruangan. Buka atau buat file chat.csv dengan mode a+ (menambah atau membuat file baru). Jika gagal membuka file, tampilkan pesan error dan keluar dari fungsi. Tutup file setelah dibuka.
* Kirim pesan ke klien bahwa ruangan telah berhasil dibuat.
* Buat catatan aktivitas yang mencatat bahwa username telah membuat ruangan room di channel. Panggil fungsi log_activity untuk mencatat aktivitas ini.
```bash
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
```
Fungsi edit_room bertujuan untuk mengubah nama sebuah room dalam channel tertentu pada layanan DiscorIT. 
* Fungsi mengecek apakah pengguna (username) memiliki peran "ROOT" atau "ADMIN" di channel (channel) yang dimaksud. Jika pengguna tidak memiliki izin yang sesuai, kirim pesan "Permission denied" ke socket klien (client_socket) dan hentikan eksekusi fungsi.
* Menyusun path lengkap untuk room lama (old_room) dan room baru (new_room) dalam direktori DiscorIT menggunakan fungsi snprintf. Path ini berdasarkan struktur direktori yang telah ditentukan, yaitu /home/{user}/FP/DiscorIT/{channel}/{room}.
* Menggunakan fungsi rename untuk mengganti nama direktori dari old_path ke new_path. Jika operasi rename berhasil, kirim pesan ke socket klien yang menyatakan bahwa perubahan nama room berhasil. Jika operasi gagal, kirim pesan yang menyatakan kegagalan.
* Jika perubahan nama berhasil, log aktivitas perubahan room dengan mencatat siapa yang melakukan perubahan dan apa perubahan tersebut. Aktivitas ini dicatat dengan fungsi log_activity.
```bash
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
```
Fungsi delete_room berfungsi untuk menghapus sebuah room (ruang obrolan) dari sebuah channel (saluran) jika user (pengguna) memiliki peran yang sesuai (ROOT atau ADMIN). 
* Fungsi user_role_auth digunakan untuk memeriksa apakah username memiliki peran "ROOT" atau "ADMIN" pada channel. Jika tidak memiliki peran yang sesuai, kirim pesan "Permission denied" ke client_socket dan keluar dari fungsi.
* Membuat path lengkap dari room yang akan dihapus menggunakan snprintf dan menyimpannya dalam room_path. Gunakan stat untuk memeriksa apakah room_path ada. Jika ada, atur room_found menjadi 1.
* Jika room_found adalah 1, coba hapus direktori room_path menggunakan remove_directory. Jika berhasil, kirim pesan "Room berhasil dihapus" ke client_socket dan catat aktivitas ini menggunakan log_activity. Jika gagal, kirim pesan "Room gagal dihapus" ke client_socket. Jika room_path tidak ditemukan, kirim pesan "Room tidak ditemukan" ke client_socket.
```bash
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
```
Fungsi delete_all_rooms bertujuan untuk menghapus semua "room" di dalam suatu "channel" yang ditentukan, dengan beberapa langkah pengecekan dan otorisasi.
* Fungsi ini pertama-tama memeriksa apakah pengguna memiliki hak akses sebagai "ROOT" atau "ADMIN" dalam channel. Jika tidak, pesan "Permission denied" dikirim ke client dan fungsi berakhir.
* Menentukan path direktori channel dan mencoba membuka direktori tersebut. Jika gagal membuka direktori, pesan kesalahan dikirim ke client.
* Mengiterasi setiap entry di dalam direktori channel. Jika entry adalah direktori dan bukan direktori khusus seperti "." atau "..", serta bukan direktori "admin", maka direktori tersebut dihapus menggunakan remove_directory.
* Menutup direktori dan mengirim pesan konfirmasi ke client bahwa semua room telah dihapus.
* Mencatat aktivitas penghapusan room ke log dengan informasi pengguna dan channel.
```bash
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
```
Fungsi ban_user berfungsi untuk melakukan ban pada user agar user tersebut tidak bisa join atau bergabung ke dalam sebuah channel.
* Fungsi user_role_auth memeriksa apakah logged_in_user memiliki peran "ADMIN" di channel. Jika tidak, mengirim pesan "Permission denied" ke client_socket dan mengembalikan nilai 0.
* Menentukan path file auth.csv yang berisi informasi otorisasi user. Membuka file auth.csv untuk dibaca. Jika gagal, mencetak pesan kesalahan dan mengembalikan nilai 0.
* Menentukan path file sementara temp_auth.csv dan membuka file ini untuk menulis. Jika gagal, mencetak pesan kesalahan, menutup file otorisasi, dan mengembalikan nilai 0.
* Membaca setiap baris dari auth.csv untuk mencari username yang ingin diban. Jika ditemukan, menandai user tersebut sebagai "BANNED" dan menulis kembali ke file sementara. Jika tidak, menyalin baris tersebut langsung ke file sementara.
* Menutup kedua file. Jika user ditemukan, mengganti file otorisasi asli dengan file sementara dan mengirim pesan bahwa user telah diban. Jika tidak ditemukan, menghapus file sementara dan mengirim pesan "User tidak ditemukan".
* Mengembalikan nilai 1 jika user berhasil diban. Mengembalikan nilai 0 jika user tidak ditemukan atau terjadi kesalahan.
```bash
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
```
Fungsi unban_user digunakan untuk menghapus status ban seorang pengguna dari channel tertentu oleh seorang admin. 
* Fungsi user_role_auth memeriksa apakah pengguna yang sedang masuk (logged_in_user) memiliki peran sebagai "ADMIN" di channel tersebut. Jika tidak, kirim pesan "Permission denied" ke client dan keluar dari fungsi.
* Buat path ke file auth.csv yang berisi informasi otorisasi untuk channel tersebut. Buka file auth.csv untuk dibaca. Jika gagal membuka file, tampilkan pesan kesalahan dan keluar.
* Buat path ke file sementara temp_auth.csv. Buka file temp_auth.csv untuk ditulis. Jika gagal membuka file, tutup file auth.csv dan keluar.
* Baca setiap baris dari auth.csv. Jika nama pengguna (stored_username) sesuai dengan pengguna yang ingin di-unban (username), ubah peran pengguna tersebut menjadi "USER" dan tulis ke temp_auth.csv. Jika tidak, salin baris tersebut ke temp_auth.csv. Tandai apakah pengguna ditemukan atau tidak (user_found).
* Tutup file auth.csv dan temp_auth.csv.
* Jika pengguna ditemukan, hapus auth.csv lama dan ganti dengan temp_auth.csv. Kirim pesan ke client bahwa pengguna telah di-unban. Jika pengguna tidak ditemukan, hapus temp_auth.csv dan kirim pesan ke client bahwa pengguna tidak ditemukan atau tidak di-banned.
```bash
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
```
Fungsi remove_user digunakan untuk menghapus pengguna dari saluran (channel) tertentu jika pengguna yang meminta penghapusan memiliki peran "ADMIN".
* Mengecek apakah pengguna yang masuk (logged_in_user) memiliki peran "ADMIN" di saluran yang ditentukan (channel). Jika tidak, kirim pesan "Permission denied" ke klien dan keluar dari fungsi.
* Membuka file auth.csv yang berisi informasi pengguna dan peran mereka dalam saluran. Jika file tidak dapat dibuka, fungsi akan mengeluarkan pesan kesalahan dan keluar.
* Membuat file sementara temp_auth.csv untuk menulis data otorisasi baru setelah pengguna dihapus. Jika file sementara tidak dapat dibuka, fungsi akan mengeluarkan pesan kesalahan, menutup file otorisasi, dan keluar.
* Membaca setiap baris dari file otorisasi, mengecek apakah nama pengguna (username) sesuai dengan pengguna yang akan dihapus. Jika tidak sesuai, tulis baris tersebut ke file sementara. Tandai jika pengguna ditemukan.
* Menutup file otorisasi dan file sementara.
* Jika pengguna ditemukan, hapus file otorisasi asli dan ganti dengan file sementara. Kirim pesan konfirmasi ke klien. Jika tidak ditemukan, hapus file sementara dan kirim pesan "User tidak ditemukan" ke klien.
```bash
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
```
Fungsi list_user berfungsi untuk membaca daftar pengguna dari file dan mengirimkannya ke client melalui socket.
* File yang berisi daftar pengguna dibuka dalam mode baca ("r"). Jika gagal membuka file, fungsi akan mencetak pesan error dan keluar dari fungsi.
* Dua buffer diinisialisasi: line untuk membaca setiap baris dari file, dan response untuk menyimpan respons yang akan dikirim ke client.
* Setiap baris dalam file dibaca menggunakan fgets. sscanf digunakan untuk mem-parsing baris dan mengekstrak username dari format id_user,username,password,role. Username ditambahkan ke response dengan strcat.
* Setelah semua baris dibaca, file ditutup. Respons yang berisi daftar user dikirim ke client melalui socket.
```bash
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
```
Fungsi list_channels bertanggung jawab untuk membaca daftar channel dari file dan mengirimkannya ke client melalui socket. 
* File yang berisi daftar channel dibuka dalam mode read-only ("r"). Jika file gagal dibuka, fungsi perror mencetak pesan kesalahan, dan fungsi mengembalikan tanpa melanjutkan lebih lanjut.
* Menginisialisasi dua variabel buffer. Buffer line digunakan untuk membaca baris dari file, sedangkan response diinisialisasi dengan header "Daftar channel:\n".
* Membaca file yang sedang dibuka. fgets membaca baris dari file, sscanf digunakan untuk memparsing nama channel dari setiap baris. Baris diasumsikan dalam format id,channel_name,key, jadi sscanf mengekstrak channel_name dan mengabaikan id dan key. strcat menambahkan nama channel ke buffer response, diikuti dengan newline.
* Isi response dikirim ke client melalui socket, dan file ditutup setelah selesai membaca.
```bash
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
```
Fungsi list_rooms bertujuan untuk mengirim daftar nama ruangan (rooms) yang ada dalam sebuah channel kepada client yang terhubung.
* Menggabungkan nama channel dengan path dasar untuk mendapatkan path lengkap ke directory channel.
* Membuka directory yang ditentukan oleh channel_path. Jika gagal membuka directory, kirim pesan kesalahan ke client.
* Menggunakan readdir untuk membaca setiap entry (file atau directory) dalam directory yang dibuka.
* Memeriksa apakah entry adalah sebuah directory dan bukan directory khusus (seperti . dan .., serta admin). Jika memenuhi kriteria, tambahkan nama directory ke response.
* Mengirimkan daftar room yang telah dibuat dalam response kepada client melalui socket.
* Menutup directory yang telah dibuka setelah selesai digunakan.
```bash
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
```
Fungsi list_user_room berfungsi untuk mengirim daftar pengguna yang terdaftar dalam file auth.csv di sebuah channel ke client. 
* Membentuk path lengkap dari file auth.csv berdasarkan nama channel yang diberikan.
* Membuka file auth.csv dalam mode read-only. Jika file tidak dapat dibuka, fungsi perror akan mencetak pesan error dan fungsi return akan mengakhiri eksekusi.
* Menginisialisasi string response dengan header "Daftar pengguna:\n".
* Membaca file auth.csv baris per baris menggunakan fgets. Menggunakan sscanf untuk mem-parsing baris dan mengekstrak nama pengguna (user). Menambahkan nama pengguna ke string response diikuti dengan newline (\n).
* Mengirim string response ke client melalui socket, lalu menutup file auth.csv setelah selesai diproses.
```bash
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
```
Fungsi edit_user digunakan untuk mengubah nama pengguna dalam sistem.
* Membuka berkas USER_FILE dan membacanya untuk mendapatkan peran pengguna yang saat ini sedang login. Jika pengguna tidak mengedit nama mereka sendiri (is_self_edit = 0) dan peran mereka bukan ROOT, akses ditolak.
* Membuka kembali USER_FILE untuk membaca dan berkas sementara untuk menulis data yang telah diedit. Jika ada kesalahan saat membuka berkas, fungsi akan mengembalikan nilai 0.
* Membaca setiap baris dari USER_FILE dan memeriksa apakah baris tersebut berisi old_username. Jika ditemukan, menulis new_username ke berkas sementara; jika tidak, menulis baris asli. Menutup kedua berkas tersebut, menghapus USER_FILE lama, dan mengganti nama berkas sementara menjadi USER_FILE.
* Membuka direktori path ke folder DiscorIT dan mencari semua subdirektori. Untuk setiap subdirektori, membuka berkas auth.csv dalam subdirektori admin, dan membuat berkas sementara untuk menulis data yang telah diedit. Jika old_username ditemukan, menggantinya dengan new_username dan menulis ke berkas sementara.
* Menutup kedua berkas tersebut, menghapus berkas auth.csv lama, dan mengganti nama berkas sementara menjadi auth.csv.
* Jika pengguna ditemukan dan berhasil diubah, mengirimkan pesan sukses ke klien. Jika tidak ditemukan, mengirimkan pesan bahwa pengguna tidak ditemukan.
```bash
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
```
Fungsi edit_user_password digunakan untuk mengubah kata sandi pengguna di server. 
* Membuka file pengguna (USER_FILE) untuk membaca, mencari baris yang sesuai dengan pengguna yang sedang masuk (dari variabel logged_in_user), dan mendapatkan peran pengguna (logged_in_user_role). Jika pengguna tidak mengedit kata sandinya sendiri (!is_self_edit) dan bukan pengguna dengan peran ROOT, maka permintaan ditolak dengan mengirimkan pesan "Permission denied" ke klien.
* Membuka file pengguna (USER_FILE) untuk membaca dan menulis, lalu membuka file sementara (temp_user.csv) untuk menulis data pengguna yang telah diperbarui. Menggunakan bcrypt untuk menghasilkan salt dan meng-hash kata sandi baru (bcrypt_gensalt dan bcrypt_hashpw).
* Membaca setiap baris dalam file pengguna. Jika menemukan pengguna yang sesuai dengan username, mengubah kata sandinya dengan yang baru. Jika tidak, menulis baris asli ke file sementara.
* Menutup kedua file, lalu menghapus file pengguna asli dan menggantinya dengan file sementara yang berisi data yang telah diperbarui.
* Jika pengguna ditemukan dan kata sandi berhasil diubah, mengirimkan pesan konfirmasi ke klien. Jika pengguna tidak ditemukan, mengirimkan pesan "User tidak ditemukan" ke klien.
```bash
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
```
Fungsi remove_user_root bertujuan untuk menghapus pengguna dari file CSV yang menyimpan informasi pengguna, tetapi hanya jika pengguna yang meminta penghapusan adalah seorang ROOT. 
* Membaca file USER_FILE untuk mendapatkan peran (role) dari pengguna yang sedang login (logged_in_user). Membandingkan peran pengguna yang sedang login dengan "ROOT". Jika bukan ROOT, kirim pesan "Permission denied" dan keluar dari fungsi.
* Membuka file USER_FILE untuk membaca data pengguna, lalu membuka file sementara untuk menulis data pengguna yang tidak dihapus.
* Membaca setiap baris dari USER_FILE dan memeriksa apakah username cocok dengan yang ingin dihapus. Jika cocok, abaikan baris tersebut. Jika tidak cocok, tulis baris ke file sementara.
* Jika pengguna ditemukan dan dihapus, ganti file USER_FILE dengan file sementara. Jika tidak, kirim pesan "User tidak ditemukan" ke client.
* Membuka direktori utama ke folder DiscorIT. Iterasi melalui setiap subdirektori untuk mencari file auth.csv di dalam admin subdirektori.
* Membaca setiap baris dari file auth.csv dan menulis ke file sementara jika username tidak cocok dengan yang ingin dihapus.
* Mengganti file auth.csv dengan file sementara.
* Jika pengguna berhasil dihapus, kirim pesan keberhasilan ke client. Jika tidak, kirim pesan "User tidak ditemukan" ke client.
```bash
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
```
Fungsi join_channel berfungsi untuk memungkinkan pengguna bergabung ke channel tertentu dalam aplikasi DiscorIT. 
* Fungsi membuka file yang menyimpan informasi tentang channel (CHANNEL_FILE). Jika file tidak dapat dibuka, fungsi mengembalikan 0 dan mencetak pesan kesalahan.
* Membuka file pengguna (USER_FILE) untuk membaca informasi tentang pengguna yang sedang mencoba bergabung ke channel. Mengekstrak peran (role) pengguna dari file pengguna. Jika pengguna ditemukan, peran disimpan dalam variabel user_role.
* Jika pengguna memiliki peran "ROOT" atau adalah admin di channel tersebut, proses dilanjutkan untuk memeriksa apakah pengguna sudah ada di file otorisasi (auth.csv) channel tersebut. Jika pengguna belum terdaftar dalam file otorisasi, pengguna ditambahkan ke file tersebut.
* Jika pengguna memiliki status "BANNED", pengguna diberi tahu bahwa mereka telah diban dan tidak bisa bergabung ke channel.
* Jika pengguna tidak memiliki peran admin atau ROOT, fungsi memeriksa apakah pengguna sudah ada di file otorisasi. Jika tidak, fungsi membaca kunci channel dari CHANNEL_FILE.
* Jika kunci channel diperlukan, pengguna diminta untuk memasukkan kunci. Kunci ini kemudian divalidasi menggunakan bcrypt.
* Jika kunci benar, pengguna ditambahkan ke file otorisasi (auth.csv) sebagai pengguna dengan peran "USER".
* Jika channel ditemukan dan pengguna berhasil bergabung, fungsi mengirim pesan konfirmasi ke socket klien. Juga mencatat aktivitas bergabungnya pengguna ke log aktivitas channel. Jika channel tidak ditemukan, fungsi mengirim pesan bahwa channel tidak ditemukan.
```bash
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
```
Fungsi join_room bertujuan untuk menangani proses bergabungnya seorang pengguna ke dalam sebuah room.
* Membuat path lengkap menuju room berdasarkan nama channel dan room. room_path adalah string yang menyimpan path tersebut.
* Menggunakan fungsi stat untuk memeriksa apakah path room tersebut ada. Jika tidak ada (nilai kembalian -1), kirim pesan "Room tidak ditemukan" ke klien dan kembalikan 0.
* Mengirim pesan ke klien dengan format [username/channel/room] untuk menunjukkan konteks saat ini. Mengirim pesan tambahan yang menyatakan pengguna berhasil masuk ke room tersebut.
* Membuat string aktivitas yang mencatat bahwa pengguna bergabung ke room di channel tertentu.
* Memanggil fungsi log_activity untuk mencatat aktivitas ini ke log.
* Mengembalikan 1 untuk menunjukkan bahwa proses berhasil.
```bash
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
```
Fungsi chat bertujuan untuk menangani pengiriman pesan dalam sebuah channel dan room pada layanan DiscorIT. 
* Fungsi ini mengkonstruksi path file chat.csv untuk channel dan room yang ditentukan. File ini akan digunakan untuk menyimpan semua pesan yang dikirim.
* Menggunakan fopen() dengan mode "a+" untuk membuka file chat dalam mode append. Jika file tidak bisa dibuka, fungsi akan keluar dengan pesan error.
* Menghitung jumlah baris (pesan) yang sudah ada dalam file untuk menentukan ID unik untuk pesan yang baru. Ini dilakukan dengan membaca setiap baris dan menghitung jumlahnya.
* Mendapatkan waktu saat ini menggunakan time() dan localtime(), lalu memformatnya ke dalam string time_str sesuai dengan format yang diinginkan (DD/MM/YYYY HH:MM
* Menggunakan fprintf() untuk menulis pesan baru ke dalam file chat. Pesan termasuk informasi waktu, ID pesan, username pengirim, dan teks pesan itu sendiri.
* Mengirimkan pesan balasan ke client melalui write(), memberitahu bahwa pesan berhasil dikirim.
* Memanggil fungsi log_activity() untuk mencatat aktivitas pengiriman pesan ke dalam file log aktivitas channel. Ini membantu untuk melacak dan memantau aktivitas yang terjadi di DiscorIT.
```bash
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
```
Fungsi see_chat digunakan untuk membaca dan mengirimkan isi dari file chat (chat.csv) dari suatu room di dalam sebuah channel kepada client melalui socket. 
* chat_file_path digunakan untuk menyimpan path lengkap ke file chat.csv dari suatu room dalam suatu channel. Path ini dibuat dengan menggunakan snprintf untuk menggabungkan nama channel dan room ke dalam path yang sesuai dengan struktur direktori DiscorIT.
* File chat dibuka dalam mode read ("r") dengan fopen. Jika file tidak dapat dibuka, fungsi akan mengirimkan pesan error melalui perror dan kemudian keluar dari fungsi.
* Menggunakan fgets, fungsi membaca setiap baris dari file chat dan menggabungkannya ke dalam string response. Hal ini dilakukan dengan loop while hingga semua baris dibaca.
* Jika setelah membaca file response masih kosong (panjang string = 0), artinya tidak ada pesan chat dalam room tersebut. Dalam hal ini, fungsi akan mengisi response dengan pesan yang sesuai.
* Setelah selesai membaca file, file ditutup dengan fclose untuk membebaskan sumber daya yang digunakan.
* Menggunakan write, fungsi mengirimkan isi dari response ke client melalui socket yang telah disediakan (client_socket).
```bash
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
```
Fungsi edit_chat ini bertanggung jawab untuk mengedit pesan chat dalam file chat.csv yang terletak di direktori tertentu berdasarkan parameter channel dan room.
* Fungsi membuka file chat.csv yang sesuai dengan channel dan room yang diberikan oleh parameter. Jika file tidak dapat dibuka, fungsi akan mengembalikan pesan error.
* Fungsi membuat file sementara temp_chat.csv untuk menulis hasil edit pesan. Jika pembuatan file sementara gagal, fungsi akan menutup file utama dan mengembalikan pesan error.
* Fungsi akan membaca setiap baris dari file chat.csv. Jika ditemukan pesan dengan chat_id dan username yang sesuai dengan parameter, maka pesan akan diubah dengan teks baru yang diberikan. Format waktu juga diperbarui sesuai dengan waktu saat ini.
* Setiap baris yang tidak sesuai dengan pesan yang ingin diubah akan disalin ke file sementara. Baris yang sesuai akan diganti dengan pesan yang telah diedit.
* Setelah selesai mengedit, file chat.csv asli akan dihapus dan file sementara akan direname menjadi chat.csv agar perubahan dapat disimpan.
* Fungsi akan mengirimkan pesan balasan ke client berdasarkan apakah pesan berhasil diubah atau tidak ditemukan.
```bash
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
```
Fungsi del_chat ini digunakan untuk menghapus pesan chat dari file chat.csv di dalam sebuah room dalam channel pada DiscorIT.
* Fungsi snprintf digunakan untuk membuat path file chat dalam variabel chat_file_path. Path ini digunakan untuk membuka file chat.csv yang berada di dalam direktori tertentu berdasarkan channel dan room yang diberikan.
* fopen(chat_file_path, "r") membuka file chat.csv untuk dibaca. Jika file tidak dapat dibuka, fungsi akan mengembalikan pesan error.
* fopen(temp_chat_file_path, "w") membuka file temporary temp_chat.csv untuk ditulis. File ini akan digunakan untuk menyalin isi chat.csv kecuali pesan yang ingin dihapus.
* Melalui loop while (fgets(line, sizeof(line), file)), setiap baris dalam file chat.csv dibaca. Baris tersebut kemudian diproses untuk mengecek apakah merupakan pesan yang ingin dihapus.
* Setiap baris diparsing menggunakan sscanf untuk mengambil id dan stored_username dari pesan chat. Jika id chat dan username sesuai dengan yang diminta untuk dihapus, variabel found diset menjadi 1.
* Baris-baris yang tidak perlu dihapus (tidak memenuhi kondisi yang ingin dihapus) disalin kembali ke temp_chat.csv menggunakan fputs.
* Setelah selesai membaca dan menulis, kedua file (file dan temp) ditutup dengan fclose.
* File chat.csv yang asli dihapus menggunakan remove(chat_file_path) dan temp_chat.csv yang sudah terisi ulang (tanpa pesan yang dihapus) diganti namanya menjadi chat.csv menggunakan rename(temp_chat_file_path, chat_file_path).
* Jika pesan yang diminta untuk dihapus ditemukan (found == 1), maka pesan "Pesan berhasil dihapus\n" dikirimkan ke client menggunakan write(client_socket, ...). Jika tidak ditemukan, pesan "Pesan tidak ditemukan\n" dikirimkan.
```bash
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
```
Fungsi initialize_directories() ini bertujuan untuk menginisialisasi struktur direktori dan file yang diperlukan untuk aplikasi DiscorIT. 
* Fungsi mkdir() digunakan untuk membuat direktori baru dengan path /home/user/FP/DiscorIT.
* Fungsi fopen() digunakan untuk membuka file. a+ mengindikasikan membuka file untuk menulis (append) dan membaca. Jika file tidak ada, akan mencoba membuatnya.
* Jika fopen() gagal membuka file, perror() akan menampilkan pesan kesalahan terkait sistem berkas. exit(EXIT_FAILURE) digunakan untuk menghentikan program dan mengembalikan status keluar EXIT_FAILURE, menandakan bahwa inisialisasi tidak berhasil.
* Setelah membuka dan memeriksa file, kedua file ditutup dengan fclose() untuk membersihkan sumber daya setelah selesai digunakan.
```bash
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
```
Fungsi user_role_auth digunakan untuk melakukan otorisasi berdasarkan peran (role) pengguna dalam suatu channel pada sistem DiscorIT. 
* Fungsi pertama kali membangun path file auth.csv untuk channel tertentu. Path ini dibangun berdasarkan nama channel yang diterima sebagai argumen. auth.csv berisi informasi mengenai otorisasi pengguna (username dan role) dalam suatu channel.
* Fungsi kemudian membuka file auth.csv untuk channel yang sesuai. Jika file tidak dapat dibuka, fungsi akan mengembalikan 0 (false).
* Fungsi membaca setiap baris dalam file auth.csv dan membandingkan informasi username dan role yang tersimpan dengan yang diberikan sebagai argumen. fgets digunakan untuk membaca baris dari file, sscanf digunakan untuk mem-parsing baris yang dibaca, mengabaikan id_user, dan mengambil stored_username dan stored_role, dan strcmp digunakan untuk membandingkan stored_username dan username, serta stored_role dan role yang diberikan.
* Setelah selesai membaca file, fungsi menutup file dan mengembalikan nilai role_match yang menunjukkan apakah pengguna dengan username dan role tertentu memiliki akses yang sesuai dalam channel tersebut.
```bash
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
```
Fungsi log_activity mencatat aktivitas tertentu ke dalam file log pada path yang telah ditentukan.
* Menggunakan snprintf untuk membuat path file log dengan memasukkan nama channel yang diberikan ke dalam format string path.
* Membuka file log dalam mode append (menambahkan di akhir file). Jika file tidak bisa dibuka, perror akan mencetak pesan kesalahan dan fungsi akan keluar.
* Mendapatkan waktu saat ini dan memformatnya menjadi string waktu dengan format dd/mm/yyyy HH:MM:SS.
* Menulis waktu dan aktivitas ke dalam file log dalam format [waktu] aktivitas.
* Menutup file log setelah penulisan selesai.
```bash
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
```
Fungsi remove_directory bertugas untuk menghapus direktori dan semua isinya secara rekursif.
* Membuka direktori yang diberikan oleh parameter path. Jika pembukaan direktori gagal, fungsi mencetak pesan kesalahan dan mengembalikan -1.
* Membaca setiap entri dalam direktori menggunakan readdir.
* Mengecek apakah entri saat ini bukan direktori khusus "." atau "..".
* Membentuk path lengkap dari entri saat ini.
* Jika entri adalah direktori, fungsi dipanggil kembali secara rekursif untuk menghapus subdirektori tersebut. Jika entri adalah file, file dihapus menggunakan unlink.
* Menutup direktori yang dibuka sebelumnya, lalu menghapus direktori yang telah kosong.
* Mengembalikan 0 untuk menandakan bahwa direktori dan semua isinya telah dihapus.

## Dokumentasi
![image](https://github.com/kokonz/Sisop-FP-2024-MH-IT01/assets/151918510/59c56260-ae35-4680-9fb8-90ef020cc63d)
![image](https://github.com/kokonz/Sisop-FP-2024-MH-IT01/assets/151918510/a4ceaf28-12dc-44f8-ae49-59d81841ed6b)
![image](https://github.com/kokonz/Sisop-FP-2024-MH-IT01/assets/151918510/af3e5a57-c7c8-4fb5-9292-b23faec994cb)
![image](https://github.com/kokonz/Sisop-FP-2024-MH-IT01/assets/151918510/1238fcde-b601-4391-88b0-bd8c570dbceb)
![image](https://github.com/kokonz/Sisop-FP-2024-MH-IT01/assets/151918510/384c36b7-11be-49fc-b603-50b162a110cb)
