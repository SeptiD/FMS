//
// Created by sd on 26.02.2018.
//

#ifndef FMS_FILES_HANDLING_H
#define FMS_FILES_HANDLING_H
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int create_directory_structure_file(char *file_name,char *path_to_folder);
#endif //FMS_FILES_HANDLING_H
