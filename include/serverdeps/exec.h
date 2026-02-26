#pragma once

#include "Database/database.h"
#include "base/communication.h"
#include "base/socket_t.h"
#include "base/utils.h"
#include "serverdeps/filemanager.h"

typedef struct {
    Database* db;
    mem_arena* arena;
    FileManager* filemanager;
} ServerContext;

Response execute(ServerContext* ctx, char* input);
