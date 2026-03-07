#include "Database/transactionmanager.h"
#include "printing.h"

Table* parse_to_table(TableMeta* metadata, FileManager* fm)
{
    Table* table = table_init(metadata);
    table->rows = mem_arena_alloc(&table->arena, sizeof(Row) * metadata->rows_count);
    if (table->meta->row_root_page == 0)
        return table;
    char* buffer = malloc(PAGE_SIZE);
    char* temp = malloc(PAGE_SIZE);
    page_t* page = fm->get_page(fm, table->meta->row_root_page);
    memcpy(temp, page->data, PAGE_SIZE);
    memcpy(buffer, temp, PAGE_SIZE);
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
        for (j = 0; j < metadata->columns_count; j++)
            free(row_info[j]);
        free(row_info);
    }
    for (i = 0; i < metadata->rows_count; i++)
        free(rows_info[i]);
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
    for (int i = 0; i < tm->table_cache_size; i++)
        table_free(tm->table_cache[i]);
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
        table_free(to_release);
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
    for (int i = 0; i < tm->table_cache_size; i++) {
        if (strcmp(tm->table_cache[i]->meta->name, table->meta->name) == 0) {
            tm->table_cache[i] = tm->table_cache[tm->table_cache_size - 1];
            tm->table_cache_size--;
            table_free(table);
            return;
        }
    }
    table_free(table);
}
