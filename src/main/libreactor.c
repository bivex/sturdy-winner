/**
 * @file libreactor.c
 * @brief Main program for basic libreactor server
 */

#include <stdio.h>
#include <stdlib.h>

#include "../../include/infrastructure/server_infrastructure.h"
#include "../../include/platform/log.h"

static core_status simple_server_handler(core_event *event)
{
    server *s = event->state;
    server_context *context = (server_context *) event->data;

    if (event->type == SERVER_REQUEST){
        log_info("Processing HTTP request for: %.*s", (int)context->request.target.size, (char*)context->request.target.base);

        if (segment_equal(context->request.target, segment_string("/plaintext"))){
            log_debug("Serving plaintext response");
            server_ok(context, segment_string("text/plain"), segment_string("Hello, World!"));
        }
        else if (segment_equal(context->request.target, segment_string("/json"))){
            log_debug("Serving JSON response");
            server_ok(context, segment_string("application/json"), segment_string("{\"message\":\"Hello, World!\"}"));
        }
        else{
            log_debug("Serving 404 for unknown route");
            server_ok(context, segment_string("text/plain"), segment_string("Not Found"));
        }
        return CORE_OK;
    }
    else {
        log_error("Unexpected event type: %d", event->type);
        server_destruct(s);
        return CORE_ABORT;
    }
}

int main(void)
{
    log_info("Starting simple test server on port 2342");

    core_construct(NULL);

    server s;
    server_construct(&s, simple_server_handler, &s);
    server_open(&s, 0, 2342);

    log_info("Server ready, starting event loop");
    core_loop(NULL);
    core_destruct(NULL);

    log_info("Server shutdown complete");
    return EXIT_SUCCESS;
}
