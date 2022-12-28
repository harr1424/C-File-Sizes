#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "fs_info.h"

// initial size of array used to contain filesystem entries
size_t fs_info_arr_size = 100;

// number of largest entries to output
const int num_entries = 10;

// global pointer to current FS_Info array
// will be updated as the array is resized using dynamic memory allocation
FS_Info *curr_fs_info_ptr;

// used to sort fs_entries array descending in terms of entry size
static int compare(const void *a, const void *b)
{
    const struct FS_Info *entryA = (FS_Info *)a;
    const struct FS_Info *entryB = (FS_Info *)b;

    return (entryB->size - entryA->size) - (entryA->size - entryB->size);
}

void update_fs_info_arr(char *path)
{
    static int items_added = 0;

    struct stat st;
    if (stat(path, &st) == 0)
    {
        if (items_added < fs_info_arr_size) // if array capacity will not be exceeded
        {
            strncpy(curr_fs_info_ptr[items_added].name, path, sizeof(curr_fs_info_ptr[items_added].name) - 1);
            curr_fs_info_ptr[items_added].size = st.st_size;

            items_added++;
        }
        else // double the size of the containing array
        {
            fs_info_arr_size *= 2;
            FS_Info *resized_fs_entries = realloc(curr_fs_info_ptr, fs_info_arr_size * sizeof(*curr_fs_info_ptr));
            if (resized_fs_entries)
            {
                curr_fs_info_ptr = resized_fs_entries;

                strncpy(curr_fs_info_ptr[items_added].name, path, sizeof(curr_fs_info_ptr[items_added].name) - 1);
                curr_fs_info_ptr[items_added].size = st.st_size;

                items_added++;
            }
            else
            {
                perror("An error occurred when attempting to resize the array!");
                exit(EXIT_FAILURE);
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

    if (dir == NULL)
    {
        printf("%s could not be opened", currDir);
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        // if directory is current dir or parent dir
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char path_to_entry[PATH_MAX];
        snprintf(path_to_entry, sizeof(path_to_entry) - 1, "%s/%s", currDir, entry->d_name);
        path_to_entry[sizeof(path_to_entry) - 1] = '\0';

        // use path_to_entry to call stats on the entry
        update_fs_info_arr(path_to_entry); // fs_entries will only be used on first call

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

    // a char array to hold a filesystem path
    char target_dir[PATH_MAX];

    if (argc > 2)
    { // more than one argument passed at invocation 
        char *error_message = "Usage: %s <target directory>\n", argv[0];
        fprintf(stderr, "%s", error_message);
        return EXIT_FAILURE;
    }

    else if (argc == 1)
    { // no argument passed at invocation, default to current working directory 
        if (getcwd(target_dir, sizeof(target_dir)) != NULL)
        {
            printf("Defaulting to current directory\n");
        }
        else
        {
            perror("Unable to detect current working directory, try passing a directory as command line argument");
            exit(EXIT_FAILURE);
        }
    }
    else
    { // exactly one path specified as argument at invocation 
        strncpy(target_dir, argv[1], PATH_MAX);
    };

    printf("Finding %d largest files in: %s\n", num_entries, target_dir);

    /* Create a pointer to the start of the FS_Info array and set the global
        variable curr_fs_info_ptr to this memory address
    */
    FS_Info *fs_entries = calloc(fs_info_arr_size, sizeof(*fs_entries));
    if (!fs_entries)
    {
        fprintf(stderr, "Malloc of fs_entries failed in main\n");
        return EXIT_FAILURE;
    }
    curr_fs_info_ptr = fs_entries;

    // recursively visit all entries in the specified directory
    walk(target_dir);

    // sort the entries descending by file size
    qsort(curr_fs_info_ptr, fs_info_arr_size, sizeof(*curr_fs_info_ptr), compare);

    // output ten largest files found
    for (int i = 0; i < num_entries; i++)
    {
        printf("%s\t%lld\n", curr_fs_info_ptr[i].name, curr_fs_info_ptr[i].size);
    }

    free(curr_fs_info_ptr); // good practice, but program is over and OS will reclaim anyways

    return EXIT_SUCCESS;
}
