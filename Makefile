CFLAGS = -Wall -Werror -Wextra -g

# compile server
server : src/base/socket_t.c src/base/mem_arena.c src/DB/cell.c src/DB/row.c src/DB/table.c src/DB/db.c src/base/exec.c src/utils.c src/server.c
	@echo "Compiling server"
	gcc $(CFLAGS) -o server src/base/socket_t.c src/base/mem_arena.c src/DB/cell.c src/DB/row.c src/DB/table.c src/DB/db.c src/base/exec.c src/utils.c src/server.c
	@echo "Done"

#compile client
client : src/base/socket_t.c src/base/client_t.c src/client.c
	@echo "Compiling client"
	gcc $(CFLAGS) -g -o client src/base/socket_t.c src/base/client_t.c src/client.c
	@echo "Done"


