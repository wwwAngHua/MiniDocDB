#include "minidocdb.h"
#include <stdio.h>

int main()
{
    // Open the database
    MiniDocDB *db = open_db(DB_FILENAME);

    // Insert a document
    insert_document(db, "name", "{\"first\": \"Alice\", \"last\": \"Doe\"}");
    insert_document(db, "age", "{\"value\": 30}");

    // Retrieve and print a document
    char *doc = find_document(db, "name");
    if (doc)
    {
        printf("Document: %s\n", doc);
        free(doc);
    }

    // Update a document (for demonstration, we delete and insert new one)
    delete_document(db, "name");
    insert_document(db, "name", "{\"first\": \"Bob\", \"last\": \"Smith\"}");

    // Retrieve the updated document
    doc = find_document(db, "name");
    if (doc)
    {
        printf("Updated Document: %s\n", doc);
        free(doc);
    }

    // Delete a document
    delete_document(db, "age");

    // Attempt to retrieve a deleted document
    doc = find_document(db, "age");
    if (!doc)
    {
        printf("Document 'age' not found (it was deleted).\n");
    }

    // Close the database
    close_db(db);

    return 0;
}