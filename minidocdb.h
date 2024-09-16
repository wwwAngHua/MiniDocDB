#ifndef MINIDOCDB_H
#define MINIDOCDB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define PAGE_SIZE 4096
#define MAX_KEY_LEN 256
#define MAX_VALUE_LEN 4096
#define DB_FILENAME "minidocdb.dat"
#define HASH_TABLE_SIZE 1024

typedef struct
{
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} Document;

typedef struct
{
    FILE *db_file;
    uint32_t num_pages;
    uint32_t free_page;
    uint32_t hash_table[HASH_TABLE_SIZE]; // Hash table for indexing documents
} MiniDocDB;

// API Functions
MiniDocDB *open_db(const char *filename);
int insert_document(MiniDocDB *db, const char *key, const char *json_value);
char *find_document(MiniDocDB *db, const char *key);
int delete_document(MiniDocDB *db, const char *key);
int close_db(MiniDocDB *db);

#endif // MINIDOCDB_H