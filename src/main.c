
/*
c_file_sizes

Author: John Harrington 

A program demonstrating the use of dynamic memory allocation in C. 
This program will recursively traverse a specified directory and 
record the names and file sizes of files found. 

The ten largest files and their sizes will output to std::out. 
*/


#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>

// size of array used to contain filesystem entries
const size_t fs_info_arr_size = 10;

/*
    A struct to contain the name of a filesystem entry and its size in bytes.
    An array of this type will be used to catalog all filesystem entries for
    the directory specified as command line argument.
*/
typedef struct FS_Info
{
    char name[PATH_MAX];
    long long size;
} FS_Info;

// global pointer to FS_Info array
FS_Info fs_info_arr[fs_info_arr_size];

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
FS_Info *get_smallest_entry(FS_Info *entries)
{
    long long smallest = entries[0].size;
    FS_Info *target = &entries[0];

    for (int i = 1; i < fs_info_arr_size; i++)
    {
        if (entries[i].size < smallest)
        {
            smallest = entries[i].size;
            target = &entries[i];
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
            strncpy(fs_info_arr[items_added].name, path, PATH_MAX);
            fs_info_arr[items_added].size = st.st_size;

            items_added++;
        }
        else
        // find entry having the smallest size and replace it with the current entry if it is larger
        {
            FS_Info *smallest = get_smallest_entry(fs_info_arr);
            if (st.st_size > smallest->size)
            {
                strncpy(smallest->name, path, PATH_MAX);
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
        snprintf(path_to_entry, sizeof(path_to_entry), "%s/%s", currDir, entry->d_name);

        update_fs_info_arr(path_to_entry);

        if (entry->d_type == DT_DIR)
        {
            walk(path_to_entry);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[])
{
    float startTime = (float)clock()/CLOCKS_PER_SEC;

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

    strncpy(target_dir, argv[1], PATH_MAX);

    printf("Finding the %zu largest files in: %s\n", fs_info_arr_size, target_dir);

    // recursively visit all entries in the specified directory
    walk(target_dir);

    // sort the entries descending by file size
    qsort(fs_info_arr, fs_info_arr_size, sizeof(*fs_info_arr), compare);

    float endTime = (float)clock()/CLOCKS_PER_SEC;

    printf("Program completed in %f seconds\n", endTime - startTime);

    // output ten largest files found
    for (int i = 0; i < fs_info_arr_size; i++)
    {
        printf("%s\t%lld\n", fs_info_arr[i].name, fs_info_arr[i].size);
    }

    return EXIT_SUCCESS;
}

// static allocation averages 0.2 seconds 
