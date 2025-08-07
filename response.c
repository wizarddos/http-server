#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "includes/parse_http.h"


char *resolve_request(struct http_headers headers){
    char path[256];
    FILE *file;
    char *body = malloc(5120+1);

    strcat(path, "./pages/");
    strcat(path, strlen(headers.path) == 1 ? "index" : headers.path++);

    file = fopen(path , "r");
    if(file == NULL){
        return NULL;
    }

    while(fgets(body, 5120, file)!= NULL){
        //printf("%s\n", body);
    }

    return body;
}

int send_response(int status_code, char *body, int clientfd){
    // * Resp codes implemented: 200, 400 404 405 500
    char response[5120] = "HTTP/1.1 ";

    switch (status_code){
        case 200:
            strcat(response, "200 OK"); break;
        case 400:
            strcat(response, "400 Bad Request"); break;
        case 404:
            strcat(response, "404 Not Found"); break;
        case 405:
            strcat(response, "405 Method Not Allowed"); break;
        case 500:
            strcat(response, "500 Internal Server Error"); break;
        default:
            break;
    }

    if(DEBUG){
        printf("first line assembled\n");
    }

    strcat(response, "\r\n");
    strcat(response, "Server: wizarddos-serv\r\n");
    strcat(response, "Content-Type: text/plain\r\n");
    strcat(response, "Content-Length: ");
    if(body == NULL){
        body = "";
    }
        char *cl;
        sprintf(cl, "%d", strlen(body));
        strcat(response, cl);
    
    strcat(response, "\r\n\r\n");
    strcat(response, body);


    printf("%s\n", response);
    size_t err = send(clientfd, response, sizeof(response), 0);

    return err;
}