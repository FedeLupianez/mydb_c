#pragma once

#include "../DB/db.h"
#include "../client_t.h"
#include "../communication.h"
#include "../utils.h"

Response execute(Database* db, char* input);
