#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> 

#define MAX_MSG_LEN 2048
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 4007


int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT)
    };
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    int max_fd = sock_fd + 1;
    fd_set read_fds;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(sock_fd, &read_fds);

        if (select(max_fd, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select failed");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            char input[MAX_MSG_LEN];
            fgets(input, MAX_MSG_LEN, stdin);

            if (strncmp(input, "/active", 7) == 0) {
                write(sock_fd, input, strlen(input));

                char buffer[MAX_MSG_LEN];
                int n = read(sock_fd, buffer, MAX_MSG_LEN);
                if (n < 0) {
                    perror("Failed to read from server");
                    exit(EXIT_FAILURE);
                } else if (n == 0) {
                    printf("Disconnected from server\n");
                    exit(EXIT_SUCCESS);
                }

                buffer[n] = '\0';
                printf("%s", buffer);
            } 
            
            else if (strncmp(input, "/sendgroup", 10) == 0) {
                char* id_str = strtok(input + 11, " \n");
                char* message = strtok(NULL, "");
                message[strcspn(message,"\n")]='\0';
                if (id_str == NULL || message == NULL) {
                    printf("Usage: /sendgroup <id> <message>\n");
                } else {
                    int id = atoi(id_str);
                    if (id == 0) {
                        printf("Invalid id\n");
                    } else {
                        char buffer[MAX_MSG_LEN];
                        sprintf(buffer, "/sendgroup %d %s", id, message);
                        write(sock_fd, buffer, strlen(buffer));
                    }
                }
            } 
            
            else if (strncmp(input, "/send", 5) == 0) {
            
                char* id_str = strtok(input + 6, " \n");
                char* message = strtok(NULL, "");

                if (id_str == NULL || message == NULL) {
                    printf("Usage: /send <id> <message>\n");
                } else {
                    int id = atoi(id_str);
                    if (id == 0) {
                        printf("Invalid id\n");
                    } else {
                        char buffer[MAX_MSG_LEN];
                        sprintf(buffer, "/send %d %s", id, message);
                        write(sock_fd, buffer, strlen(buffer));
                    }
                }
            } 


            
            else if (strncmp(input, "/addtogroup", 11) == 0) {
            
                char* id_str = strtok(input + 12, " \n");
                char* message = strtok(NULL, "");

                message[strcspn(message,"\n")]='\0';

                if (id_str == NULL || message == NULL) {
                    printf("Usage: /addtogroup <group_id> <message>\n");
                } else {
                    int id = atoi(id_str);
                    if (id == 0) {
                        printf("Invalid id\n");
                    } else {
                        char buffer[MAX_MSG_LEN];
                        sprintf(buffer, "/addtogroup %d %s", id, message);
                        write(sock_fd, buffer, strlen(buffer));
                    }
                }
            } 

            else if (strncmp(input, "/broadcast", 10) == 0) {
        
                char buffer[MAX_MSG_LEN];
                sprintf(buffer, "%s", input);
                write(sock_fd, buffer, strlen(buffer));

            }
            else if (strncmp(input, "/makegroupbroadcast", 19) == 0) {
                char buffer[MAX_MSG_LEN];
                
                input[strcspn(input,"\n")]='\0';
                write(sock_fd, input, strlen(input));

            }

            else if (strncmp(input, "/makegroupreq", 13)== 0) {
                char* id_str = strtok(input + 14, " \n");
                char* message = strtok(NULL, "");
                // message[strcspn(message,"\n")]='\0';

                if (id_str == NULL || message == NULL) 
                    printf("Usage: /makegroupreq <id> <id> ....<id><space>\n");
                
               
                else {
                    char buffer[MAX_MSG_LEN];
                    sprintf(buffer, "/makegroupreq %s %s", id_str, message);
                    write(sock_fd, buffer, strlen(buffer));
                }
                
            }



            else if (strncmp(input, "/makegroup", 10)== 0) {
                char* id_str = strtok(input + 11, " \n");
                char* message = strtok(NULL, "");
                // message[strcspn(message,"\n")]='\0';
                if (id_str == NULL || message == NULL) 
                    printf("Usage: /makegroup <id> <id> ....<id><space>\n");
                
                else {

                        char buffer[MAX_MSG_LEN];
                        sprintf(buffer, "/makegroup %s %s", id_str, message);
                        write(sock_fd, buffer, strlen(buffer));
                    }
                
            } 

            else if (strncmp(input, "/joingroup", 10)== 0) {
                char* id_str = strtok(input + 11, " \n");
                char* message = strtok(NULL, "");
                // message[strcspn(message,"\n")]='\0';
                if (id_str == NULL || message == NULL) 
                    printf("Usage: /joingroup <id> <id> ....<id><space>\n");
                
                else {

                        char buffer[MAX_MSG_LEN];
                        sprintf(buffer, "/joingroup %s %s", id_str, message);
                        write(sock_fd, buffer, strlen(buffer));
                    }
                
            } 
            else if (strncmp(input, "/declinegroup", 10)== 0) {

                char* id_str = strtok(input + 11, " \n");
                char* message = strtok(NULL, "");

                if (id_str == NULL || message == NULL) 
                    printf("Usage: /declinegroup <id> <id> ....<id><space>\n");
                
                else {

                        char buffer[MAX_MSG_LEN];
                        sprintf(buffer, "/declinegroup %s %s", id_str, message);
                        write(sock_fd, buffer, strlen(buffer));
                    }
                
            } 

            else if (strncmp(input, "/activegroups", 12) == 0) {
                
                char buffer[MAX_MSG_LEN];
                sprintf(buffer, "%s", input);
                write(sock_fd, buffer, strlen(buffer));

            }

            else if (strncmp(input, "/makeadmin", 10)== 0) {
                char* id_str = strtok(input + 11, " \n");
                char* message = strtok(NULL, "\n");

                message[strcspn(message,"\n")]='\0';

                if (id_str == NULL || message == NULL) 
                    printf("Usage: /makeadmin <group_id> <client_id>\n");
                
                else {

                        char buffer[MAX_MSG_LEN];
                        sprintf(buffer, "/makeadmin %s %s", id_str, message);
                        write(sock_fd, buffer, strlen(buffer));
                    }
                
            } 


            else if (strncmp(input, "/removefromgroup", 16)== 0) {
                char* id_str = strtok(input + 17, " \n");
                char* message = strtok(NULL, "\n");

                // message[strcspn(message,"\n")]='\0';

                if (id_str == NULL || message == NULL) 
                    printf("Usage: /removefromgroup <group_id> <client_id>\n");
                
                else {

                        char buffer[MAX_MSG_LEN];
                        sprintf(buffer, "/removefromgroup %s %s", id_str, message);
                        write(sock_fd, buffer, strlen(buffer));
                    }
                
            } 

            else if (strncmp(input, "/quit", 5) == 0) {
                printf("quitting \n");
                exit(0);
            }

            
            else {
                printf("Unknown command: %s\n", input);
            }
        }

        if (FD_ISSET(sock_fd, &read_fds)) {
            char buffer[MAX_MSG_LEN];
            int n = read(sock_fd, buffer, MAX_MSG_LEN);
            if (n < 0) {
                perror("Failed to read from server");
                exit(EXIT_FAILURE);
            } else if (n == 0) {
                printf("Disconnected from server\n");
                exit(EXIT_SUCCESS);
            }

            buffer[n] = '\0';
            printf("%s", buffer);
        }
    }

    return 0;
}
