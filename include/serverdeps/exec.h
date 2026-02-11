#pragma once

#include "../DB/db.h"
#include "../base/communication.h"
#include "../base/socket_t.h"
#include "../base/utils.h"

typedef struct {
    Database* db;
    mem_arena* arena;
} ServerContext;

Response execute(ServerContext* ctx, char* input);
