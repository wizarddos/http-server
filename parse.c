#include <stdio.h>
#include <string.h>

#include "includes/parse_http.h"

struct http_headers parse_headers(char *unparsed){
    struct http_headers headers;
    int parse_err;

    char *cl = strstr(unparsed, "Content-Length:");
    if(cl){
        parse_err = sscanf(cl, "Content-Length: %d", &headers.content_length);
    }else{
        headers.content_length = -1;
    }

    char *ua = strstr(unparsed, "User-Agent:");
    if(ua){
        ua = strtok(ua, "\r\n");
        ua+=12; // Skip over label
        headers.user_agent = ua;     
    }else{
        headers.user_agent = "No-UA-Specified";
    }

    headers.method = strtok(unparsed, " ");
    headers.path = strtok(NULL, " ");

    headers.error_code = 0;
    if(strcmp(headers.method, "GET") != 0 && 
       strcmp(headers.method, "POST") != 0 &&
       strcmp(headers.method, "HEAD") != 0 ){
        headers.error_code = 405;
    }

    if(strstr(headers.path, "../")){
        headers.error_code = 400; // Path-traverse detected
    }

    if(parse_err == -1){
        headers.error_code= 400;
    }

    if(headers.path[0] != '/'){
        headers.error_code = 400;
    }

    /* printf("Cl: %d\n Method: %s\nPath: %s\nUser-Agent: %s\n", 
        headers.content_length, 
        headers.method,
        headers.path,
        headers.user_agent
    ); */
    
    return headers;
}

