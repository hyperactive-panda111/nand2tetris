#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "parser.h"

// Helper to check if a filename ends with .jack
int is_jack_file(const char *filename) {
    size_t len = strlen(filename);
    return (len > 5 && strcmp(filename + len - 5, ".jack") == 0);
}

// Helper to read an entire file into a null-terminated buffer
char* read_file_to_buffer(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Fatal Error: Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Fatal Error: Failed to seek file %s\n", filename);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    long length = ftell(file);
    if (length < 0) {
        fprintf(stderr, "Fatal Error: Failed to determine file size of %s\n", filename);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Fatal Error: Failed to rewind file %s\n", filename);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    char* buffer = malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Fatal Error: Memory allocation failed for file buffer\n");
        exit(EXIT_FAILURE);
    }

    size_t expected = (size_t)length;
    size_t read_bytes = fread(buffer, 1, length, file);
    if (read_bytes == 0) {
        fprintf(stderr, "Fatal Error: Failed to read file %s\n");
        free(buffer);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    buffer[read_bytes] = '\0';

    fclose(file);
    return buffer;
}

// The core compilation sequence for a single file pairing
void compile_single_file(const char *input_path, const char *output_path) {
    char *source_buffer = read_file_to_buffer(input_path);

    TokenTable table;
    table.capacity = 256;
    table.count = 0;
    table.tokens = malloc(sizeof(Token) * table.capacity);
    if (!table.tokens) {
        fprintf(stderr, "Fatal Error: Failed to allocate Token table\n");
        free(source_buffer);
        exit(EXIT_FAILURE);
    }

    FILE *out_file = fopen(output_path, "w");
    if (!out_file) {
        fprintf(stderr, "Fatal Error: Could not open output file %s\n", output_path);
        free(table.tokens);
        free(source_buffer);
        exit(EXIT_FAILURE);
    }

    ParserState p;
    p.table = &table;
    p.out = out_file;

    printf("Compiling: %s -> %s\n", input_path, output_path);

    // Execute tokenizer & compiler loop
    parser(&p, source_buffer);

    fclose(out_file);
    free(table.tokens);
    free(source_buffer);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file.jack | input_directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *target_path = argv[1];
    struct stat path_stat;

    // Get metadata about the provided path
    if (stat(target_path, &path_stat) != 0) {
        fprintf(stderr, "Fatal Error: Path '%s' does not exist.\n", target_path);
        return EXIT_FAILURE;
    }

    // Case 1: Target is a single file
    if (S_ISREG(path_stat.st_mode)) {
        if (!is_jack_file(target_path)) {
            fprintf(stderr, "Error: Target file must have a .jack extension.\n");
            return EXIT_FAILURE;
        }

        // Generate output filename by replacing .jack with .xml
        char output_path[512];
        size_t len = strlen(target_path);
        snprintf(output_path, sizeof(output_path), "%.*s.xml", (int)(len - 5), target_path);

        compile_single_file(target_path, output_path);
    }
    // Case 2: Target is a directory
    else if (S_ISDIR(path_stat.st_mode)) {
        DIR *dir = opendir(target_path);
        if (!dir) {
            fprintf(stderr, "Fatal Error: Could not open directory %s\n", target_path);
            return EXIT_FAILURE;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (is_jack_file(entry->d_name)) {
                char input_filepath[1024];
                char output_filepath[1024];

                // Construct full path for input file
                snprintf(input_filepath, sizeof(input_filepath), "%s/%s", target_path, entry->d_name);

                // Construct full path for output XML file
                size_t file_len = strlen(entry->d_name);
                char raw_name[256];
                strncpy(raw_name, entry->d_name, file_len - 5);
                raw_name[file_len - 5] = '\0';

                snprintf(output_filepath, sizeof(output_filepath), "%s/%s.xml", target_path, raw_name);

                compile_single_file(input_filepath, output_filepath);
            }
        }
        closedir(dir);
    } else {
        fprintf(stderr, "Error: Unsupported path type.\n");
        return EXIT_FAILURE;
    }

    printf("All files processed successfully.\n");
    return EXIT_SUCCESS;
}
