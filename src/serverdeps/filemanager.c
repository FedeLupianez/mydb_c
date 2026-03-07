#include "serverdeps/filemanager.h"
#include "base/utils.h"
#include <stdio.h>

FileManager* file_manager_init(char* filename)
{
    FILE* file = fopen(filename, "a+");
    if (file == NULL)
        return NULL;
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fclose(file);

    if (file_size == 0) {
        file = fopen(filename, "w");
        char* empty_page = calloc(PAGE_SIZE, 1);
        for (int i = 0; i < PAGE_CACHE_SIZE; i++) {
            fwrite(empty_page, PAGE_SIZE, 1, file);
        }
        free(empty_page);
        fclose(file);
    }

    FileManager* manager = malloc(sizeof(FileManager));
    manager->filename = malloc(strlen(filename) + 1);
    memcpy(manager->filename, filename, strlen(filename) + 1);
    for (size_t i = 0; i < PAGE_CACHE_SIZE; i++) {
        manager->p_cache[i].data = NULL;
        manager->p_cache[i].id = -1;
    }
    manager->p_cache_size = 0;
    manager->writed = 0;
    manager->write_page = write_page;
    manager->write_in = write_in;
    manager->get_page = get_page;
    manager->read_in = read_in;
    manager->realease_page = realease_page;
    return manager;
}

void file_manager_close(FileManager* manager)
{
    for (size_t i = 0; i < manager->p_cache_size; i++) {
        page_t* page = &manager->p_cache[i];
        printf("Realeasing page %d\n", page->id);
        realease_page(manager, page);
        page_free(page);
        page = NULL;
    }
    free(manager->filename);
    free(manager);
}

int write_in(FileManager* manager, char* data, size_t size, int page_id, int offset)
{
    FILE* file = fopen(manager->filename, "r+");
    if (file == NULL)
        return 0;
    fseek(file, page_id * PAGE_SIZE + offset, SEEK_SET);
    fwrite(data, 1, size, file);
    fclose(file);
    return size;
}

void write_page(FileManager* manager, int page_id, char* data, size_t size)
{
    FILE* file = fopen(manager->filename, "r+");
    if (file == NULL)
        return;
    fseek(file, page_id * PAGE_SIZE, SEEK_SET);
    fwrite(data, sizeof(char), size, file);
    fclose(file);
}

page_t* load_page(FileManager* manager, int page_id)
{
    for (size_t i = 0; i < manager->p_cache_size; i++) {
        if (manager->p_cache[i].id == page_id)
            return &manager->p_cache[i];
    }
    if (manager->p_cache_size == PAGE_CACHE_SIZE) {
        for (size_t i = 1; i < PAGE_CACHE_SIZE; i++) {
            page_free(&manager->p_cache[i - 1]);
            manager->p_cache[i - 1].id = manager->p_cache[i].id;
            manager->p_cache[i - 1].data = manager->p_cache[i].data;
            manager->p_cache[i - 1].offset = manager->p_cache[i].offset;
        }
        manager->p_cache_size--;
    }
    page_t* page = &manager->p_cache[manager->p_cache_size];
    page_init(page, page_id);
    FILE* file = fopen(manager->filename, "r");
    fseek(file, page_id * PAGE_SIZE, SEEK_SET);
    fread(page->data, 1, PAGE_SIZE, file);
    fclose(file);
    update_page_offset(page);
    manager->p_cache_size++;
    return page;
}

page_t* get_page(FileManager* manager, int page_id)
{
    return load_page(manager, page_id);
}

void read_in(FileManager* manager, int page_id, int offset, size_t size, char* buffer)
{
    page_t* page = load_page(manager, page_id);
    memcpy(buffer, page->data + offset, size);
}

void page_init(page_t* page, int id)
{
    if (page->data != NULL)
        free(page->data);
    page->id = id;
    page->data = calloc(PAGE_SIZE, 1);
    page->offset = 0;
    page->dirty = 0;
}

void page_free(page_t* page)
{
    free(page->data);
    page->data = NULL;
}

void write_in_page(page_t* page, char* data, size_t size, int offset)
{
    char* buffer = page->data;
    memcpy(buffer + offset, data, size);
    update_page_offset(page);
    if (!page->dirty)
        page->dirty = 1;
}

char* read_in_page(page_t* page, int offset, size_t size)
{
    char* buffer = malloc(size);
    memcpy(buffer, page->data + offset, size);
    return buffer;
}

void realease_page(FileManager* manager, page_t* page)
{
    if (page->dirty) {
        printf("DEBUG realease: writing page %d (dirty=1)\n", page->id);
        manager->write_page(manager, page->id, page->data, PAGE_SIZE);
        page->dirty = 0;
    }
    update_page_offset(page);
    if (manager->p_cache_size < PAGE_CACHE_SIZE)
        return;
    for (size_t i = 0; i < manager->p_cache_size; i++) {
        if (manager->p_cache[i].id == page->id) {
            manager->p_cache[i].id = -1;
            manager->p_cache[i].offset = 0;
            manager->p_cache[i].dirty = 0;
            page_free(page);
        }
    }
}

void update_page_offset(page_t* page)
{
    page->offset = 0;
    for (int i = 0; i < PAGE_SIZE; i++)
        if (page->data[i] != '\0')
            page->offset++;
}
