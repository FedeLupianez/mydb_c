#pragma once
#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 1024
#define PAGE_CACHE_SIZE 5

typedef struct {
    int id;
    int offset;
    char* data;
    uint8_t dirty : 1;
} page_t;

typedef struct FileManager FileManager;

struct FileManager {
    page_t p_cache[PAGE_CACHE_SIZE];
    size_t p_cache_size;
    size_t writed;
    char* filename;
    void (*write_page)(FileManager* manager, int page_id, char* data, size_t size);
    int (*write_in)(FileManager* manager, char* data, size_t size, int page_id, int offset);
    page_t* (*get_page)(FileManager* manager, int page_id);
    void (*read_in)(FileManager* manager, int page_id, int offset, size_t size, char* buffer);
    void (*realease_page)(FileManager* manager, page_t* page);
};

FileManager* file_manager_init(char* filename);
void file_manager_close(FileManager* manager);
void write_page(FileManager* manager, int page_id, char* data, size_t size);
int write_in(FileManager* manager, char* data, size_t size, int page_id, int offset);
void read_in(FileManager* manager, int page_id, int offset, size_t size, char* buffer);
void realease_page(FileManager* manager, page_t* page);

// Page functions
void page_init(page_t* page, int id);
void page_free(page_t* page);
page_t* get_page(FileManager* manager, int page_id);
void write_in_page(page_t* page, char* data, size_t size, int offset);
char* read_in_page(page_t* page, int offset, size_t size);
void update_page_offset(page_t* page);
