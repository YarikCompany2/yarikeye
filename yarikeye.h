#ifndef YARIKEYE_H
#define YARIKEYE_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// --- STRING ---

typedef struct {
    int count;
    int capacity;
    char *buffer;
} String;

typedef struct {
    char *buffer;
    int start_index;
    int final_index;
} StringSlice;

// Initializes the String structure
String yarikeye_string_init();
// Frees the String structure
void yarikeye_string_free(String *str);
// Gets count
int yarikeye_string_count_get(String *str);
// Copies the buffer into newly created string. Need to be deleted
String yarikeye_string_assign(const char *buffer);
// Appends the buffer to the existing string
void yarikeye_string_append(String *str, const char *buffer);
// Creates new string and appends the buffer to that string
String yarikeye_string_append_and_create(String *str, const char *buffer);
// Compares two Strings, sensetive to the case
bool yarikeye_string_compare(String *str1, String *str2);
// Prints the string
void yarikeye_string_print(const String *str);

// --- FILES ---

// Reads file and fills the buffer with the text
char *yarikeye_file_read(const char *path, size_t *out_file_size);
// Writes the buffer to specified file
void yarikeye_file_write(char *path, char *buffer);

#endif // YARIKEYE_H

#ifdef YARIKEYE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>

// --- STRING ---

String yarikeye_string_init() {
    String str;

    str.buffer = NULL;
    str.capacity = 0;
    str.count = 0;

    return str;
}

void yarikeye_string_free(String *str) {
    free(str->buffer);
}

int yarikeye_string_count_get(String *str) {
    return str->count;
}

static void string_capacity_grow_to_size(String *str, size_t new_size) {
    if (str->capacity == 0) {
        str->capacity = new_size;
        str->buffer = (char*)malloc(sizeof(char) * str->capacity);
    }

    str->capacity = new_size;
    str->buffer = (char*)realloc(str->buffer, sizeof(char) * str->capacity);
}

static void string_capacity_grow(String *str) {
    if (str->capacity == 0) {
        str->capacity = 8;
        str->buffer = (char*)malloc(sizeof(char) * str->capacity);
    }

    str->capacity *= 2;
    str->buffer = (char*)realloc(str->buffer, sizeof(char) * str->capacity);
}

static void string_print_debug(String *str) {
    for (int i = 0; i < str->capacity; i++) {
        switch (str->buffer[i]) {
            case '\n':
                putchar('\\');
                putchar('n');
                break;
            case '\t':
                putchar('\\');
                putchar('t');
                break;
            case '\0':
                putchar('\\');
                putchar('0');
                break;
            default:
                putchar(str->buffer[i]);

        }
    }
    putchar('\n');
}

String yarikeye_string_assign(const char *buffer) {
    String str = yarikeye_string_init();

    size_t buffer_size = strlen(buffer);

    string_capacity_grow_to_size(&str, buffer_size + 1);

    for (int i = 0; i < buffer_size; i++)
        str.buffer[i] = buffer[i];

    str.count = buffer_size;

    str.buffer[buffer_size] = '\0';

    return str;
}

String yarikeye_string_append_and_create(String *str, const char *buffer) {
    String new_str = yarikeye_string_assign(str->buffer);

    yarikeye_string_append(&new_str, buffer);

    return new_str;
}

void yarikeye_string_append(String *str, const char *buffer){
    size_t buffer_size = strlen(buffer);
    size_t needed_capacity = str->count + buffer_size;

    if (str->capacity < needed_capacity + 1)
        string_capacity_grow_to_size(str, needed_capacity + 1);

    for (int i = str->count, j = 0; i < needed_capacity; i++, j++) {
        str->buffer[i] = buffer[j];
    }

    str->count += buffer_size;

    str->buffer[needed_capacity] = '\0';
}

bool yarikeye_string_compare(String *str1, String *str2) {
    if (str1->count != str2->count) return false;

    for (int i = 0; i < str1->count; i++) {
        if (str1->buffer[i] != str2->buffer[i])
            return false;
    }

    return true;
}

void yarikeye_string_print(const String *str) {
    printf("%s\n", str->buffer);
}

// --- STRING SLICE ---

StringSlice yarikeye_string_slice_init(char *buffer, int start, int final) {
    StringSlice string_slice;

    string_slice.buffer = buffer;
    string_slice.start_index = start;
    string_slice.final_index = final;

    return string_slice;
}

void yarikeye_string_slice_print(StringSlice *string_slice) {
    for (int i = string_slice->start_index; i < string_slice->final_index - string_slice->start_index; i++) {
        putchar(string_slice->buffer[i]);
    }
}

// --- FILES ---

char *yarikeye_file_read(const char *path, size_t *out_file_size) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Can't open the file %s\n", path);
        exit(1);
    }
    
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char *buffer = (char*)malloc(sizeof(char) * file_size + 1);

    size_t bytes_count = fread(buffer, sizeof(char), file_size, file);
    if (bytes_count < file_size) {
        fprintf(stderr, "Error was ocured during file %s reading\n", path);
        exit(1);
    }

    buffer[file_size] = '\0';

    if (out_file_size) 
        *out_file_size = bytes_count;

    fclose(file);

    return buffer;
}

void yarikeye_file_write(char *path, char *buffer) {
    FILE *file = fopen(path, "wb");
    if (!file) {
        fprintf(stderr, "Can't create the file %s\n", path);
        exit(1);
    }

    size_t buffer_size = strlen(buffer);

    size_t bytes_written = fwrite(buffer, sizeof(char), buffer_size, file);
    if (bytes_written < buffer_size) {
        fprintf(stderr, "Can't write text to the file %s\n", path);
        exit(1);
    }

    fclose(file);
}
#endif // YARIKEYE_IMPLEMENTATION
