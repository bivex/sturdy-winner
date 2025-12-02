/**
 * @file http_server.c
 * @brief Implementation of HTTP server business logic
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _GNU_SOURCE
#include <dynamic.h>
#include <reactor.h>
#include <clo.h>

#include "../../include/domain/http_server.h"
#include "../../include/domain/http_response.h"
#include "../../include/platform/log.h"

http_server_error_t http_server_init(void)
{
    http_response_error_t resp_err = http_response_init();
    if (resp_err != HTTP_RESPONSE_OK) {
        return HTTP_SERVER_ERROR_INVALID_PARAM;
    }

    return HTTP_SERVER_OK;
}

void http_server_cleanup(void)
{
    http_response_cleanup();
}

http_server_error_t http_server_create(http_server_t *server,
                                         const http_server_config_t *config)
{
    if (!server || !config) {
        return HTTP_SERVER_ERROR_INVALID_PARAM;
    }

    memset(server, 0, sizeof(*server));

    /* Copy configuration */
    server->config = *config;

    /* Prepare JSON buffer */
    if (config->json_message) {
        /* Manually create JSON object: {"message": "value"} */
        /* Format: {"message":"Hello, World!"} */
        int written = snprintf(server->json_buffer, sizeof(server->json_buffer),
                              "{\"message\":\"%s\"}", config->json_message);
        if (written < 0 || (size_t)written >= sizeof(server->json_buffer)) {
            return HTTP_SERVER_ERROR_MEMORY;
        }

        server->json_buffer_size = (size_t)written;
    }

    return HTTP_SERVER_OK;
}

void http_server_destroy(http_server_t *server)
{
    if (server) {
        memset(server, 0, sizeof(*server));
    }
}

http_server_error_t http_server_handle_request(http_server_t *server,
                                                 server_context *context)
{
    if (!server || !context) {
        return HTTP_SERVER_ERROR_INVALID_PARAM;
    }

    /* Parse the route from the request */
    http_route_t route = http_server_parse_route(&context->request.target);

    /* Handle routes and send responses directly */
    switch (route) {
        case ROUTE_PLAINTEXT:
            server_ok(context, 
                     segment_string("text/plain"), 
                     segment_string(server->config.plaintext_response));
            break;

        case ROUTE_JSON:
            server_ok(context, 
                     segment_string("application/json"), 
                     segment_string(server->json_buffer));
            break;

        case ROUTE_UNKNOWN:
        default:
            server_ok(context, 
                     segment_string("text/plain"), 
                     segment_string("Not Found"));
            break;
    }

    return HTTP_SERVER_OK;
}

http_route_t http_server_parse_route(const segment *target)
{
    if (!target || !target->base) {
        return ROUTE_UNKNOWN;
    }

    /* Simple route matching */
    if (segment_equal(*target, segment_string("/plaintext"))) {
        return ROUTE_PLAINTEXT;
    } else if (segment_equal(*target, segment_string("/json"))) {
        return ROUTE_JSON;
    }

    return ROUTE_UNKNOWN;
}

http_server_error_t http_server_generate_response(const http_server_t *server,
                                                    http_route_t route,
                                                    http_response_config_t *response_config)
{
    if (!server || !response_config) {
        return HTTP_SERVER_ERROR_INVALID_PARAM;
    }

    /* Initialize with defaults */
    response_config->status_code = HTTP_STATUS_OK;
    response_config->include_date_header = server->config.enable_date_headers;

    switch (route) {
        case ROUTE_PLAINTEXT:
            response_config->content_type = CONTENT_TYPE_TEXT_PLAIN;
            response_config->body = server->config.plaintext_response;
            response_config->body_length = server->config.plaintext_response ?
                strlen(server->config.plaintext_response) : 0;
            break;

        case ROUTE_JSON:
            response_config->content_type = CONTENT_TYPE_APPLICATION_JSON;
            response_config->body = server->json_buffer;
            response_config->body_length = server->json_buffer_size;
            break;

        case ROUTE_UNKNOWN:
        default:
            response_config->status_code = HTTP_STATUS_NOT_FOUND;
            response_config->content_type = CONTENT_TYPE_TEXT_PLAIN;
            response_config->body = "Not Found";
            response_config->body_length = strlen("Not Found");
            break;
    }

    return HTTP_SERVER_OK;
}
