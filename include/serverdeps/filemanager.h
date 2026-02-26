#pragma once

#include <stddef.h>
#define PAGE_SIZE 1024
#define PAGE_CACHE_SIZE 5

typedef struct {
    int id;
    int offset;
    char* data;
} page_t;

typedef struct FileManager FileManager;

struct FileManager {
    page_t p_cache[PAGE_CACHE_SIZE];
    size_t p_cache_size;
    size_t writed;
    int (*write)(FileManager* manager, char* data, size_t size);
    char* (*read)(FileManager* manager, char* buffer, int page_id, size_t size);
    char* filename;
};

FileManager* file_manager_init(char* filename);
void file_manager_close(FileManager* manager);
int file_manager_write(FileManager* manager, char* data, size_t size);
char* file_manager_read(FileManager* manager, char* buffer, int page_id, size_t size);
void page_init(page_t* page, int id);
void file_page_read(page_t* page, int id, char* filename);
void file_page_write(page_t* page, char* filename, char* data, size_t size);
void page_free(page_t* page);
