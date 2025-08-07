#ifndef PARSE_HTTP_H
#define PARSE_HTTP_H

#define DEBUG 1
struct http_headers{
    int error_code;

    int content_length;
    char *method;
    char *path;
    char *user_agent;
};

// Request functions
struct http_headers parse_headers(char *unparsed);
int parse_content(char **content);

char *resolve_request(struct http_headers headers);

// Response functions
int send_response(int status_code, char *body, int clientfd);



#endif

