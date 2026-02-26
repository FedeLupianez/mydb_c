#include "serverdeps/filemanager.h"
#include "base/utils.h"
#include <stdio.h>

FileManager* file_manager_init(char* filename)
{
    FileManager* manager = malloc(sizeof(FileManager));
    manager->filename = malloc(strlen(filename) + 1);
    memcpy(manager->filename, filename, strlen(filename) + 1);
    for (size_t i = 0; i < PAGE_CACHE_SIZE; i++) {
        manager->p_cache[i].data = NULL;
        manager->p_cache[i].id = -1;
    }
    manager->p_cache_size = 0;
    manager->writed = 0;
    manager->write = file_manager_write;
    manager->read = file_manager_read;
    return manager;
}

void file_manager_close(FileManager* manager)
{
    for (size_t i = 0; i < manager->p_cache_size; i++) {
        free(manager->p_cache[i].data);
    }
    free(manager->filename);
    free(manager);
}

int file_manager_write(FileManager* manager, char* data, size_t size)
{
    FILE* file = fopen(manager->filename, "a");
    if (file == NULL)
        return -1;
    fseek(file, manager->writed, SEEK_SET);
    fwrite((uint8_t*)data, size, 1, file);
    fclose(file);
    manager->writed += size;
    return manager->writed / PAGE_SIZE;
}

page_t* load_page(FileManager* manager, int page_id)
{
    for (size_t i = 0; i < manager->p_cache_size; i++) {
        if (manager->p_cache[i].id == page_id)
            return &manager->p_cache[i];
    }
    if (manager->p_cache_size == PAGE_CACHE_SIZE) {
        page_free(&manager->p_cache[0]);
        for (size_t i = 1; i < PAGE_CACHE_SIZE; i++) {
            manager->p_cache[i - 1] = manager->p_cache[i];
        }
        manager->p_cache_size--;
    }
    page_t* page = &manager->p_cache[manager->p_cache_size];
    page_init(page, page_id);
    FILE* file = fopen(manager->filename, "r");
    if (file) {
        fseek(file, page_id * PAGE_SIZE, SEEK_SET);
        fread(page->data, 1, PAGE_SIZE, file);
        fclose(file);
    }
    manager->p_cache_size++;
    return page;
}

void page_free(page_t* page)
{
    free(page->data);
    page->data = NULL;
}

char* file_manager_read(FileManager* manager, char* buffer, int page_id, size_t size)
{
    page_t* page = load_page(manager, page_id);
    memcpy(buffer, page->data, size);
    return buffer;
}

void page_init(page_t* page, int id)
{
    if (page->data != NULL)
        free(page->data);
    page->id = id;
    page->data = malloc(PAGE_SIZE);
    page->offset = 0;
}

void file_page_read(page_t* page, int id, char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
        return;
    fseek(file, id * PAGE_SIZE, SEEK_SET);
    page->offset = fread(page->data, PAGE_SIZE, 1, file);
    fclose(file);
}

void file_page_write(page_t* page, char* filename, char* data, size_t size)
{
    FILE* file = fopen(filename, "r+b");
    if (file == NULL)
        return;
    fseek(file, page->id * PAGE_SIZE, SEEK_SET);
    fwrite(data, size, 1, file);
    fclose(file);
}
