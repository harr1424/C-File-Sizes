#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "fs_info.h"

// size of array used to contain filesystem entries
const int N = 100000;

// number of largest entries to output
const int num_entries = 10;

// declare an array of type info to contain information about filesystem entries
struct info fs_entries[N];

// determine the size of the info struct, used for qsort
const int info_size = sizeof(struct info);

// used to sort fs_entries array descending in terms of entry size
int compare(const void *a, const void *b)
{
    struct info *entryA = (struct info *)a;
    struct info *entryB = (struct info *)b;

    return (entryB->size - entryA->size);
}

void get_size(char *path, struct info fs_info[N], int info_size)
{
    static int items_added = 0;

    struct stat st;
    if (stat(path, &st) == 0)
    {
        if (items_added < N) // if array capacity will not be exceeded
        {
            strcpy(fs_info[items_added].name, path);
            fs_info[items_added].size = st.st_size;

            items_added++;
        }
        else
        {
            puts("ERROR: Number of filesystem entries exceeds array");
        }
    }
    else
    {
        printf("Error getting stat for entry %s: %d\n", path, stat(path, &st));
    }
}

void walk(const char *currDir, struct info fs_entries[N], int info_size)
{
    DIR *dir = opendir(currDir);
    struct dirent *entry;

    if (dir == NULL)
    {
        // directory could not be opened
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            // if directory is current dir or parent dir
            continue;
        }

        char path_to_entry[1024];
        snprintf(path_to_entry, sizeof(path_to_entry), "%s/%s", currDir, entry->d_name);

        // use path_to_entry to call stats on the entry
        get_size(path_to_entry, fs_entries, info_size);

        if (entry->d_type == DT_DIR)
        {
            // recursively visit subdirectories
            walk(path_to_entry, fs_entries, info_size);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <target directory>\n", argv[0]);
    }

    const char *target_dir = argv[1];

    printf("Finding %d largest files in: %s\n", num_entries, target_dir);

    // recursively visit all entries in the specified directory
    walk(target_dir, fs_entries, info_size);

    // sort the entries descending by file size
    qsort(fs_entries, N, info_size, compare);

    // output ten largest files found
    for (int i = 0; i < num_entries; i++)
    {
        printf("%s\t%d\n", fs_entries[i].name, fs_entries[i].size);
    }

    return 0;
}
