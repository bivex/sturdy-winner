# Compiler and flags
CC       = gcc
CFLAGS   = -std=gnu11 -Wall -Wextra -Wpedantic -O3 -g -march=native -flto -MMD -MP
CPPFLAGS = -Isrc/include -Isrc/include/platform -Isrc/include/domain -Isrc/include/infrastructure
LDADD    = -lreactor -ldynamic -lclo

# Build directory
BUILD_DIR = build

# Source files by module
PLATFORM_SRCS = \
	src/platform/system.c \
	src/platform/process.c \
	src/platform/socket.c \
	src/platform/log.c \
	src/platform/signals.c

DOMAIN_SRCS = \
	src/domain/http_response.c \
	src/domain/http_server.c

INFRASTRUCTURE_SRCS = \
	src/infrastructure/server_infrastructure.c

MAIN_SRCS = \
	src/main/libreactor.c \
	src/main/libreactor-server.c

# Object files (in build directory)
PLATFORM_OBJS = $(PLATFORM_SRCS:src/%.c=$(BUILD_DIR)/%.o)
DOMAIN_OBJS = $(DOMAIN_SRCS:src/%.c=$(BUILD_DIR)/%.o)
INFRASTRUCTURE_OBJS = $(INFRASTRUCTURE_SRCS:src/%.c=$(BUILD_DIR)/%.o)
MAIN_OBJS = $(MAIN_SRCS:src/%.c=$(BUILD_DIR)/%.o)

# All objects
ALL_OBJS = $(PLATFORM_OBJS) $(DOMAIN_OBJS) $(INFRASTRUCTURE_OBJS) $(MAIN_OBJS)

# Build targets
.PHONY: all clean

all: libreactor libreactor-server

# Main executables
libreactor: $(PLATFORM_OBJS) $(DOMAIN_OBJS) $(INFRASTRUCTURE_OBJS) $(BUILD_DIR)/main/libreactor.o
	$(CC) -o $@ $^ $(LDADD)

libreactor-server: $(PLATFORM_OBJS) $(DOMAIN_OBJS) $(INFRASTRUCTURE_OBJS) $(BUILD_DIR)/main/libreactor-server.o
	$(CC) -o $@ $^ $(LDADD)

# Compilation rules
$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Dependencies (automatically handled by gcc -MMD)

clean:
	rm -rf $(BUILD_DIR) libreactor libreactor-server *.a
