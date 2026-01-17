#include "../../include/serverdeps/page.h"
#include "../../include/base/utils.h"
#include <stdio.h>

void page_init(page_t* page, int id, char* buffer)
{
    page->id = id;
    page->data = buffer;
    page->offset = strlen(buffer);
}

void file_page_read(page_t* page, int id, char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
        return;
    fseek(file, id * PAGE_SIZE, SEEK_SET);
    fread(page->data, PAGE_SIZE, 1, file);
    page->offset = strlen(page->data);
    fclose(file);
}

void page_free(page_t* page)
{
    free(page->data);
}
