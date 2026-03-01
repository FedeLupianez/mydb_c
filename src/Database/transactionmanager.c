#include "Database/transactionmanager.h"
#include "printing.h"

Table* parse_to_table(TableMeta* metadata, FileManager* fm)
{
    Table* table = table_init(metadata);
    table->rows = mem_arena_alloc(&table->arena, sizeof(Row) * metadata->rows_count);
    int root_page = metadata->root_row_offset % PAGE_SIZE;
    int last_page = metadata->root_row_offset % PAGE_SIZE;
    char* buffer = malloc(PAGE_SIZE * (last_page - root_page + 1));
    char* temp = malloc(PAGE_SIZE);
    for (int i = root_page; i <= last_page; i++) {
        page_t* page = fm->read_page(fm, i);
        memcpy(temp, page->data, PAGE_SIZE);
        memcpy(buffer + (i - root_page) * PAGE_SIZE, temp, PAGE_SIZE);
    }
    free(temp);
    Row* rows = table->rows;
    size_t i = 0;
    size_t j = 0;
    char** rows_info = split(buffer, "\n");
    for (i = 0; i < metadata->rows_count; i++) {
        char** row_info = split(rows_info[i], "\t");
        rows[i] = row_init(i, metadata->columns_count);
        Row* row = &rows[i];
        row->cells_count = metadata->columns_count;
        for (j = 0; j < metadata->columns_count; j++)
            row->cells[j] = cell_init_from_string(metadata->columns[j].type, row_info[j]);
        free(row_info);
    }
    if (rows_info)
        free(rows_info);
    free(buffer);
    return table;
}

TransactionManager* tm_init()
{
    TransactionManager* tm = malloc(sizeof(TransactionManager));
    tm->table_cache_size = 0;
    tm->load_table = load_table;
    tm->get_table = get_table;
    tm->realease_table = realease_table;
    return tm;
}

void tm_free(TransactionManager* tm)
{
    free(tm);
}

Table* load_table(TransactionManager* tm, TableMeta* metadata, FileManager* fm)
{
    Table* table = parse_to_table(metadata, fm);
    Table* to_release = NULL;
    if (tm->table_cache_size >= TABLE_CACHE_SIZE) {

        to_release = tm->table_cache[0];
        for (int i = 1; i < tm->table_cache_size; i++)
            tm->table_cache[i - 1] = tm->table_cache[i];
    }
    tm->table_cache[tm->table_cache_size] = table;
    tm->table_cache_size++;
    if (to_release)
        tm->realease_table(tm, to_release);
    print_debug("table loaded");
    return table;
}

Table* get_table(TransactionManager* tm, TableMeta* metadata, FileManager* fm)
{
    for (int i = 0; i < tm->table_cache_size; i++)
        if (strcmp(tm->table_cache[i]->meta->name, metadata->name) == 0)
            return tm->table_cache[i];
    print_debug("table not in cache");
    return tm->load_table(tm, metadata, fm);
}

void realease_table(TransactionManager* tm, Table* table)
{
    char* name = strdup(table->meta->name);
    table_free(table);
    for (int i = 0; i < tm->table_cache_size; i++)
        if (strcmp(tm->table_cache[i]->meta->name, name) == 0) {
            tm->table_cache[i] = tm->table_cache[tm->table_cache_size - 1];
            tm->table_cache_size--;
            free(name);
            return;
        }
    if (name != NULL)
        free(name);
}
