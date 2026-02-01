#pragma once

#include "../DB/db.h"
#include "../base/communication.h"
#include "../base/socket_t.h"
#include "../base/utils.h"

Response execute(Database* db, char* input, mem_arena* exec_arena);
