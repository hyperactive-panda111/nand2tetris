#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include "command_lookup_table.h"
#include "parser.h"
#include "assembly_conversion.h"

void verify_output_filename(char *out_name);
void translate_file(FILE* vm_file, FILE* out_file, int* counter, char* base_name);
void translate_directory(const char* dir_path, FILE* out_file, int* counter);

int main(int argc, char* argv[]) {
    int counter = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file.vm | directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* determine if input is a file or directory */
    struct stat path_stat;
    if (stat(argv[1], &path_stat) != 0) {
        perror("Error accessing input path");
        return EXIT_FAILURE;
    }

    char out_name[256];
    char base_name[256];

    if (S_ISDIR(path_stat.st_mode)) {
        /* --- directory mode --- */
        char* dir_path = argv[1];

        /* strip trailing slash if present */
        size_t dir_len = strlen(dir_path);
        if (dir_path[dir_len - 1] == '/')
            dir_path[dir_len - 1] = '\0';

        /* extract directory stem for output filename */
        char* slash = strrchr(dir_path, '/');
        char* stem_start = slash ? slash + 1 : dir_path;

        if (snprintf(base_name, sizeof(base_name), "%s", stem_start) 
                >= (int)sizeof(base_name)) {
            fprintf(stderr, "Error: directory name too long\n");
            return EXIT_FAILURE;
        }

        verify_output_filename(base_name);

        /* output file: DirectoryName/DirectoryName.asm */
        if (snprintf(out_name, sizeof(out_name), "%s/%s.asm", 
                dir_path, base_name) >= (int)sizeof(out_name)) {
            fprintf(stderr, "Error: output path too long\n");
            return EXIT_FAILURE;
        }

        FILE* out_file = fopen(out_name, "w");
        if (!out_file) {
            perror("Error creating output file");
            return EXIT_FAILURE;
        }

        create_lookup_table();

        /* emit bootstrap code - only in directory mode */
        write_bootstrap(out_file, &counter);

        /* translate all .vm files in directory */
        translate_directory(dir_path, out_file, &counter);

        free_lookup_table();
        fclose(out_file);

    } else {
        /* --- single file mode --- */
        if (snprintf(out_name, sizeof(out_name), "%s", argv[1]) 
                >= (int)sizeof(out_name)) {
            fprintf(stderr, "Input path too long\n");
            return EXIT_FAILURE;
        }

        char* slash = strrchr(out_name, '/');
        char* dot   = strrchr(out_name, '.');

        if (!dot || (slash && dot < slash)) {
            fprintf(stderr, "Error: input file must have .vm extension\n");
            return EXIT_FAILURE;
        }

        char* stem_start = slash ? slash + 1 : out_name;
        size_t stem_len  = (size_t)(dot - stem_start);

        if (stem_len >= sizeof(base_name)) {
            fprintf(stderr, "Error: base name too long\n");
            return EXIT_FAILURE;
        }

        memcpy(base_name, stem_start, stem_len);
        base_name[stem_len] = '\0';

        /* replace .vm with .asm */
        snprintf(dot, sizeof(out_name) - (size_t)(dot - out_name), ".asm");

        verify_output_filename(base_name);

        FILE* vm_file = fopen(argv[1], "r");
        if (!vm_file) {
            perror("Error opening input file");
            return EXIT_FAILURE;
        }

        FILE* out_file = fopen(out_name, "w");
        if (!out_file) {
            perror("Error creating output file");
            fclose(vm_file);
            return EXIT_FAILURE;
        }

        create_lookup_table();
        parser(vm_file, out_file, &counter, base_name);
        free_lookup_table();

        fclose(vm_file);
        fclose(out_file);
    }

    return EXIT_SUCCESS;
}

/* translate all .vm files found in a directory */
void translate_directory(const char* dir_path, FILE* out_file, int* counter) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        char* name = entry->d_name;
        size_t name_len = strlen(name);

        /* skip entries that don't end in .vm */
        if (name_len < 4 || strcmp(name + name_len - 3, ".vm") != 0)
            continue;

        /* build full path to .vm file */
        char vm_path[512];
        if (snprintf(vm_path, sizeof(vm_path), "%s/%s", dir_path, name) 
                >= (int)sizeof(vm_path)) {
            fprintf(stderr, "Error: vm file path too long: %s\n", name);
            exit(EXIT_FAILURE);
        }

        /* extract base name from filename for static segment labels */
        char base_name[256];
        size_t stem_len = name_len - 3;  /* strip .vm */
        memcpy(base_name, name, stem_len);
        base_name[stem_len] = '\0';

        verify_output_filename(base_name);

        FILE* vm_file = fopen(vm_path, "r");
        if (!vm_file) {
            fprintf(stderr, "Error opening file: %s\n", vm_path);
            exit(EXIT_FAILURE);
        }

        parser(vm_file, out_file, counter, base_name);
        fclose(vm_file);
    }

    closedir(dir);
}

void verify_output_filename(char* out_name) {
    if (*out_name >= 'a' && *out_name <= 'z')
        *out_name = toupper((unsigned char)*out_name);
}
