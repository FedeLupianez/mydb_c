#ifndef EXEC_INPUT_H
#define EXEC_INPUT_H

#include "../DB/db.h"
#include "../base/socket_t.h"
#include "../client_t.h"
#include "../utils.h"

void execute(Database* db, char* input, int* client);

#endif
