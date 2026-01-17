#pragma once

#define PAGE_SIZE 4096

typedef struct {
    int id;
    int offset;
    char* data;
} page_t;

void page_init(page_t* page, int id, char* buffer);
void file_page_read(page_t* page, int id, char* filename);
void page_free(page_t* page);
