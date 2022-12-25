#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include "fs_info.h"

// initial size of array used to contain filesystem entries
int N = 100;

// number of largest entries to output
const int num_entries = 10;

// global pointer to current FS_Info array
// will be updated as the array is resized using dynamic memory allocation
struct FS_Info *curr_fs_info_ptr;

// determine the size of the info struct, used for qsort
const int info_size = sizeof(struct FS_Info);

// used to sort fs_entries array descending in terms of entry size
int compare(const void *a, const void *b)
{
    struct FS_Info *entryA = (struct FS_Info *)a;
    struct FS_Info *entryB = (struct FS_Info *)b;

    return (entryB->size - entryA->size);
}

void get_size(char *path)
{
    static int items_added = 0;

    struct stat st;
    if (stat(path, &st) == 0)
    {
        if (items_added < N) // if array capacity will not be exceeded
        {
            strcpy(curr_fs_info_ptr[items_added].name, path);
            curr_fs_info_ptr[items_added].size = st.st_size;

            items_added++;
        }
        else // double the size of the containing array
        {
            // puts("Re-allocating array to fit additional fs_entries");

            N *= 2;
            struct FS_Info *resized_fs_entries = realloc(curr_fs_info_ptr, N * sizeof(*curr_fs_info_ptr));
            if (resized_fs_entries)
            {
                curr_fs_info_ptr = resized_fs_entries;

                strcpy(curr_fs_info_ptr[items_added].name, path);
                curr_fs_info_ptr[items_added].size = st.st_size;

                items_added++;
            }
            else
            {
                puts("An error occurred when attempting to resize the array!");
                exit(-1);
            }
        }
    }
    else
    {
        printf("Error getting stat for entry %s: %d\n", path, stat(path, &st));
    }
}

void walk(const char *currDir)
{
    DIR *dir = opendir(currDir);
    struct dirent *entry;

    if (dir == NULL) // directory could not be opened
    {
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        // if directory is current dir or parent dir
        {
            continue;
        }

        char path_to_entry[1024];
        snprintf(path_to_entry, sizeof(path_to_entry), "%s/%s", currDir, entry->d_name);

        // use path_to_entry to call stats on the entry
        get_size(path_to_entry); // fs_entries will only be used on first call

        if (entry->d_type == DT_DIR)
        {
            // recursively visit subdirectories
            walk(path_to_entry);
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

    /* Create a pointer to the start of the FS_Info array and set the global
        variable curr_fs_info_ptr to this memory address
    */
    struct FS_Info *fs_entries = malloc(N * sizeof(struct FS_Info));
    curr_fs_info_ptr = fs_entries;

    // recursively visit all entries in the specified directory
    walk(target_dir);

    // sort the entries descending by file size
    qsort(curr_fs_info_ptr, N, info_size, compare);

    // output ten largest files found
    for (int i = 0; i < num_entries; i++)
    {
        printf("%s\t%d\n", curr_fs_info_ptr[i].name, curr_fs_info_ptr[i].size);
    }

    free(curr_fs_info_ptr); // good practice, but program is over and OS will reclaim anyways 

    return 0;
}
