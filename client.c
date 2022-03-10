#include <stdio.h>      /* printf, sprintf */
#include <string.h>     /* strcmp */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[])
{
    char cmd[50];
    char host[50] = "34.118.48.238";
    char content[50] = "application/json";
    char *login_cookie = NULL;
    char jwt[LINELEN];
    char data[BUFLEN], line[LINELEN];
    int ok = 0;

    while (1)
    {   
        fgets(cmd, 50, stdin);
        ok = 0;

        if(!strcmp(cmd, "register\n")){
            ok = 1;
            int sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);
            char user[50], pwd[50];

            printf("username=");
            fgets(user, 50, stdin);
            user[strlen(user)-1] = '\0'; //fara newline

            printf("password=");
            fgets(pwd, 50, stdin);
            pwd[strlen(pwd)-1] = '\0'; //fara newline

            //creez datele de trimis
            memset(data, 0, BUFLEN);
            sprintf(line, "{");
            compute_message(data, line);

            sprintf(line, "\t\"username\": \"%s\",", user);
            compute_message(data, line);

            sprintf(line, "\t\"password\": \"%s\"", pwd);
            compute_message(data, line);

            sprintf(line, "}");
            compute_message(data, line);

            //creez payload
            char *payload, *response;
            payload = compute_post_request(host, "/api/v1/tema/auth/register", content, data, NULL, NULL);

            //trimit la server
            send_to_server(sockfd, payload);

            //primesc de la server
            response = receive_from_server(sockfd);
            
            printf("%s\n", response);
            close_connection(sockfd);
        }

        if(!strcmp(cmd, "login\n")){
            ok = 1;
            //daca da cineva login fara logout sa se stearga jwt de dinainte
            if(login_cookie){
                memset(login_cookie, 0, strlen(login_cookie));
            }
            memset(jwt, 0, LINELEN);
            int sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);
            char user[50], pwd[50];

            printf("username=");
            fgets(user, 50, stdin);
            user[strlen(user)-1] = '\0'; //fara newline

            printf("password=");
            fgets(pwd, 50, stdin);
            pwd[strlen(pwd)-1] = '\0'; //fara newline

            //creez datele de trimis
            memset(data, 0, BUFLEN);
            sprintf(line, "{");
            compute_message(data, line);

            sprintf(line, "\t\"username\": \"%s\",", user);
            compute_message(data, line);

            sprintf(line, "\t\"password\": \"%s\"", pwd);
            compute_message(data, line);

            sprintf(line, "}");
            compute_message(data, line);

            //creez payload
            char *payload, *response;
            payload = compute_post_request(host, "/api/v1/tema/auth/login", content, data, NULL, NULL);

            //trimit la server
            send_to_server(sockfd, payload);

            //primesc de la server
            response = receive_from_server(sockfd);
            
            printf("%s\n", response);

            //sectiune cookie
            response = strstr(response, "Set-Cookie:");

            //iau primul cookie, am presupus ca celelalte nu mi trebuie:))
            if(response){
                strtok(response, " ");
                login_cookie = strtok(NULL, " ");
                login_cookie[strlen(login_cookie) - 1] = '\0';
            }
            close_connection(sockfd);
        }

        if(!strcmp(cmd, "enter_library\n")){
            ok = 1;
            int sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);
            char *payload, *response;
            //fac si trimit cererea
            payload = compute_get_request(host, "/api/v1/tema/library/access", NULL, login_cookie, NULL);
            send_to_server(sockfd, payload);
            response = receive_from_server(sockfd);
            
            printf("%s\n", response);

            //extrag jwt token
            response = strstr(response, "token");

            if(response){
                strcpy(jwt, response+8);
                jwt[strlen(jwt) - 2] = '\0';
            }
            close_connection(sockfd);
        }

        if(!strcmp(cmd, "get_books\n")){
            ok = 1;
            int sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

            char *payload, *response;
            //fac si trimit cererea
            payload = compute_get_request(host, "/api/v1/tema/library/books", NULL, login_cookie, jwt);
            send_to_server(sockfd, payload);
            response = receive_from_server(sockfd);
            
            printf("%s\n", response);

            close_connection(sockfd);
        }

        if(!strcmp(cmd, "get_book\n")){
            ok = 1;
            printf("id=");
            int id;

            scanf("%d", &id);

            int sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

            char *payload, *response;
            //fac si trimit cererea
            char url[LINELEN];
            sprintf(url, "/api/v1/tema/library/books/%d", id);
            payload = compute_get_request(host, url, NULL, login_cookie, jwt);
            send_to_server(sockfd, payload);
            response = receive_from_server(sockfd);
            
            printf("%s\n", response);

            close_connection(sockfd);
            fgets(cmd, 50, stdin);
        }

        if(!strcmp(cmd, "add_book\n")){
            ok = 1;
            char title[50], author[50], genre[50], publisher[50];
            int n_pages;
            memset(data, 0, BUFLEN);
            sprintf(line, "{");
            compute_message(data, line);

            printf("tile=");
            fgets(title, 50, stdin);
            title[strlen(title)-1] = '\0'; //fara newline

            sprintf(line, "\t\"title\": \"%s\",", title);
            compute_message(data, line);
            
            printf("author=");
            fgets(author, 50, stdin);
            author[strlen(author)-1] = '\0'; //fara newline

            sprintf(line, "\t\"author\": \"%s\",", author);
            compute_message(data, line);

            printf("genre=");
            fgets(genre, 50, stdin);
            genre[strlen(genre)-1] = '\0'; //fara newline

            sprintf(line, "\t\"genre\": \"%s\",", genre);
            compute_message(data, line);

            printf("publisher=");
            fgets(publisher, 50, stdin);
            publisher[strlen(publisher)-1] = '\0'; //fara newline

            printf("page_count=");
            scanf("%d", &n_pages);
            
            sprintf(line, "\t\"page_count\": %d,", n_pages);
            compute_message(data, line);

            sprintf(line, "\t\"publisher\": \"%s\"", publisher);
            compute_message(data, line);

            sprintf(line, "}");
            compute_message(data, line);

            int sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

            //creez payload
            char *payload, *response;
            payload = compute_post_request(host, "/api/v1/tema/library/books", content, data, login_cookie, jwt);

            //trimit la server
            send_to_server(sockfd, payload);

            //primesc de la server
            response = receive_from_server(sockfd);
            
            printf("%s\n", response);
            close_connection(sockfd);
            fgets(cmd, 50, stdin);
        }

        if(!strcmp(cmd, "delete_book\n")){
            ok = 1;
            int id;
            printf("id=");
            scanf("%d", &id);

            int sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

            char *payload, *response;
            //fac si trimit cererea
            char url[LINELEN];
            sprintf(url, "/api/v1/tema/library/books/%d", id);
            payload = compute_delete_request(host, url, NULL, login_cookie, jwt);
            send_to_server(sockfd, payload);
            response = receive_from_server(sockfd);
            
            printf("%s\n", response);

            close_connection(sockfd);
            fgets(cmd, 50, stdin);
        }

        if(!strcmp(cmd, "logout\n")){
            ok = 1;
            if(login_cookie){
                memset(login_cookie, 0, strlen(login_cookie));
            }
            memset(jwt, 0, LINELEN);
        }

        if(!strcmp(cmd, "exit\n")){
            ok = 1;
            if(login_cookie){
                memset(login_cookie, 0, strlen(login_cookie));
            }
            memset(jwt, 0, LINELEN);
            break;
        }

        if(ok == 0){
            printf("Bad command!\n");
        }
    }
    
    return 0;
}