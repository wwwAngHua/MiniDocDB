#include "minidocdb.h"
#include <string.h>

// Hash function to map keys to hash table slots
uint32_t hash_key(const char *key)
{
    uint32_t hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_TABLE_SIZE;
}

// Open the database file
MiniDocDB *open_db(const char *filename)
{
    MiniDocDB *db = (MiniDocDB *)malloc(sizeof(MiniDocDB));
    db->db_file = fopen(filename, "r+b");
    if (!db->db_file)
    {
        db->db_file = fopen(filename, "w+b"); // Create if not exists
        db->num_pages = 0;
        db->free_page = 0;
        memset(db->hash_table, 0xFF, sizeof(db->hash_table)); // Initialize hash table to -1 (empty)
        fwrite(db, sizeof(MiniDocDB), 1, db->db_file);
    }
    else
    {
        // Load existing database metadata
        fread(db, sizeof(MiniDocDB), 1, db->db_file);
    }
    return db;
}

// Insert a document into the database
int insert_document(MiniDocDB *db, const char *key, const char *json_value)
{
    uint32_t hash_index = hash_key(key);

    // Check if the key already exists
    if (db->hash_table[hash_index] != 0xFFFFFFFF)
    {
        printf("Error: Key already exists\n");
        return -1;
    }

    // Create the document
    Document doc;
    strncpy(doc.key, key, MAX_KEY_LEN);
    strncpy(doc.value, json_value, MAX_VALUE_LEN);

    // Write the document to the database
    fseek(db->db_file, 0, SEEK_END);
    uint32_t page_number = db->num_pages++;
    fwrite(&doc, sizeof(Document), 1, db->db_file);

    // Update the hash table
    db->hash_table[hash_index] = page_number;

    // Write the updated DB metadata and hash table to file
    fseek(db->db_file, 0, SEEK_SET);
    fwrite(db, sizeof(MiniDocDB), 1, db->db_file);

    return 0;
}

// Find a document by key
char *find_document(MiniDocDB *db, const char *key)
{
    uint32_t hash_index = hash_key(key);

    // Check if the document exists
    if (db->hash_table[hash_index] == 0xFFFFFFFF)
    {
        printf("Error: Document not found\n");
        return NULL;
    }

    // Seek to the page where the document is stored
    uint32_t page_number = db->hash_table[hash_index];
    fseek(db->db_file, sizeof(MiniDocDB) + page_number * sizeof(Document), SEEK_SET);

    // Read the document
    Document doc;
    fread(&doc, sizeof(Document), 1, db->db_file);

    // Return the document value
    char *result = (char *)malloc(MAX_VALUE_LEN);
    strncpy(result, doc.value, MAX_VALUE_LEN);
    return result;
}

// Delete a document by key
int delete_document(MiniDocDB *db, const char *key)
{
    uint32_t hash_index = hash_key(key);

    // Check if the document exists
    if (db->hash_table[hash_index] == 0xFFFFFFFF)
    {
        printf("Error: Document not found\n");
        return -1;
    }

    // Invalidate the document in the hash table
    db->hash_table[hash_index] = 0xFFFFFFFF;

    // Write the updated DB metadata and hash table to file
    fseek(db->db_file, 0, SEEK_SET);
    fwrite(db, sizeof(MiniDocDB), 1, db->db_file);

    return 0;
}

// Close the database
int close_db(MiniDocDB *db)
{
    fclose(db->db_file);
    free(db);
    return 0;
}