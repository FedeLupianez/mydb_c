CC = gcc
CFLAGS = -Wall -Wextra -g -I./include -fsanitize=address

SRC_DIR = src
OBJ_DIR = obj

SERVER_SRC = $(filter-out $(SRC_DIR)/client.c, $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*/*.c))
CLIENT_SRC = $(filter-out $(SRC_DIR)/server.c , $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/base/*.c))

SERVER_OBJ = $(SERVER_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
CLIENT_OBJ = $(CLIENT_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)


all: server client

# compile server
server : $(SERVER_OBJ)
	@echo "Compiling server"
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Done"

#compile client
client : $(CLIENT_OBJ)
	@echo "Compiling client"
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Done"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) server client


