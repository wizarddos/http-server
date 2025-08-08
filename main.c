#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "includes/logs.h"
#include "includes/parse_http.h"

#define PORT 3490


int main(){
    int sockfd;
    struct sockaddr_in my_addr, client_addr;
    char request[256];
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0){
        printf("Failed to find a socket\n");
    }
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (const struct sockaddr *)&my_addr, sizeof(struct sockaddr)) < 0){
        printf("Failed to bind a socket\n");
        return -1;
    }

    listen(sockfd, 15);

    
    int clientfd;
    int req_bytes = -1;
    printf("Server running on 127.0.0.1:%d\n", PORT);
    while(req_bytes != 0){
        char request[1024], org_req[1024];
        char *content;
        int total_read = 0, bytes = 0;

        socklen_t client_len = sizeof(client_addr);
        clientfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_len);
        if(clientfd < 0){
            printf("Failed to connect client");
            continue;
        }
        
        while((bytes = recv(clientfd, request+total_read, sizeof(request)-total_read, 0)) > 0){
            total_read += bytes;
            if(DEBUG){
                printf("Reading bytes\n");
            }

            if(strstr(request, "\r\n\r\n")){
                break;
            }
        }
        // Parse HTTP
        strncpy(org_req, request, 1024);
        char *headers_end = strstr(request, "\r\n\r\n");
        if(!headers_end){
            printf("Malformed request\n");
            send_response(400, NULL, clientfd);
            close(clientfd);
            continue;
        }
        
        struct http_headers headers;
        
        headers = parse_headers(request);
        if(headers.error_code != 0){
            send_response(headers.error_code, NULL, clientfd);
            close(clientfd);
        }

        if(DEBUG){
            printf("headers parsed\n");
        }

        char extracted_params[1024];
        if(!strcmp(headers.method, "POST") && headers.content_length > 0){
            bytes = recv(clientfd, org_req+total_read, sizeof(org_req)-total_read, 0);
            total_read+= bytes;
            
            char *params = strstr(org_req, "\r\n\r\n");
            params+=4; // skip over CRLF

            if(*params == '\0'){
                *params = '0';
            }else{
                strncpy(extracted_params, params, strlen(params));
            }
        }


        if(!strcmp(headers.method, "GET")){
            char *params = strstr(headers.path, "?");
            if(params){
                params++;
                strncpy(extracted_params, params, strlen(params));
                params--;
                *params = '\0';
            }
        }

        char *body = resolve_request(headers);
        if(body == NULL){
            printf("GO FUCK YOURSELF!\n");
            send_response(404, NULL, clientfd);
            close(clientfd);
            continue;
        }

        if(!strcmp(headers.method, "HEAD")){
            body = NULL;
        }

        if(send_response(200, body, clientfd) != -1){
            if(DEBUG){
                printf("resp sent\n");
            }
            free(body);
            close(clientfd);
            continue;
        }else{
            printf("Error sending response");
        }
    }
    
    close(sockfd);
    bzero(request, 256);
}
