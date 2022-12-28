#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "fs_info.h"

// size of array used to contain filesystem entries
const size_t fs_info_arr_size = 10;

// global pointer to FS_Info array
FS_Info *fs_info_arr[fs_info_arr_size];

// used to sort fs_entries array descending in terms of entry size
static int compare(const void *a, const void *b)
{
    const struct FS_Info *entryA = (FS_Info *)a;
    const struct FS_Info *entryB = (FS_Info *)b;

    return (entryB->size - entryA->size) - (entryA->size - entryB->size);
}

/*
Iterates over an array of FS_Info structs and returns a pointer to the struct 
having the smallest size member. 
*/
FS_Info *get_smallest_entry(FS_Info **entries)
{
    long long smallest = entries[0]->size;
    FS_Info *target;

    for (int i = 1; i < fs_info_arr_size * sizeof(FS_Info); i += sizeof(FS_Info))
    {
        if (entries[i]->size < smallest)
        {
            smallest = entries[i]->size;
            target = entries[i];
        }
    }
    return target;
}

/*
Add entires to the array. If the array is full, use the above function to find the
struct having the smallest file size, and if the current file size is larger, replace it.
*/
void update_fs_info_arr(char *path)
{
    static int items_added = 0;

    struct stat st;
    if (stat(path, &st) == 0)
    {
        if (items_added < fs_info_arr_size) // if array capacity will not be exceeded
        {
            strncpy(fs_info_arr[items_added]->name, path, sizeof(fs_info_arr[items_added]->name)/sizeof(fs_info_arr[items_added]->name[0]) - 1);
            fs_info_arr[items_added]->size = st.st_size;

            items_added++;
        }
        else 
        // find entry having the smallest size and replace it with the current entry if it is larger
        {
            FS_Info *smallest = get_smallest_entry(fs_info_arr); 
            if (st.st_size > smallest->size)
            {
                strncpy(smallest->name, path, sizeof(smallest->name)/sizeof(smallest->name[0]) - 1);
                smallest->size = st.st_size;
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

    printf("Finding the %zu largest files in: %s\n", fs_info_arr_size, target_dir);

    // recursively visit all entries in the specified directory
    walk(target_dir);

    // sort the entries descending by file size
    qsort(fs_info_arr, fs_info_arr_size, sizeof(*fs_info_arr), compare);

    // output ten largest files found
    for (int i = 0; i < fs_info_arr_size; i++)
    {
        printf("%s\t%lld\n", fs_info_arr[i]->name, fs_info_arr[i]->size);
    }

    return EXIT_SUCCESS;
}
