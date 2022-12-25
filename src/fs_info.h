#ifndef FS_INFO_H_
#define FS_INFO_H_

/*
    A struct to contain the name of a filesystem entry and its size in bytes. 
    An array of this type will be used to catalog all filesystem entries for 
    the directory specified as command line argument.
*/
struct FS_Info {
    char name[1024];
    int size;
};

#endif