/*3. Реализуйте консольное приложение для копирования файлов. Аргументы в Ваше
приложение передаются как аргументы командной строки: первый параметр — это имя
файла, который является источником данных, второй параметр - это имя файла, в который
необходимо выполнить копирование. Данные во входном файле могут быть произвольной
структуры; структура содержимого во входном файле не должна влиять на процесс
копирования; входной и выходной файл должны быть идентичны после выполнения
операции копирования.*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <windows.h>
#include <string.h>

#define BUFFER_SIZE 4096

int check_file_existence(const char *);
int check_write_permission(const char *);
char* convert_to_absolute_path(const char *);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s source_file destination_file\n", argv[0]);
        return 1;
    }

    char *abs_src_path = convert_to_absolute_path(argv[1]);
    char *abs_dst_path = convert_to_absolute_path(argv[2]);

    if (strcmp(abs_src_path, abs_dst_path) == 0) {
        printf("Error: Cannot copy a file onto itself\n");
        free(abs_src_path);
        free(abs_dst_path);
        return 2;
    }

    if (!check_file_existence(abs_src_path)) {
        printf("Source file '%s' does not exist\n", abs_src_path);
        free(abs_src_path);
        free(abs_dst_path);
        return 3;
    }

    if (!check_write_permission(abs_dst_path)) {
        printf("No write permission for destination file '%s'\n", abs_dst_path);
        free(abs_src_path);
        free(abs_dst_path);
        return 4;
    }

    FILE *source_file = fopen(abs_src_path, "rb");
    if (source_file == NULL) {
        printf("Failed to open source file\n");
        free(abs_src_path);
        free(abs_dst_path);
        return -1;
    }

    FILE *destination_file = fopen(abs_dst_path, "wb");
    if (destination_file == NULL) {
        printf("Failed to open destination file\n");
        fclose(source_file);
        free(abs_src_path);
        free(abs_dst_path);
        return -2;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, source_file)) > 0) {
        fwrite(buffer, 1, bytes_read, destination_file);
    }

    fclose(source_file);
    fclose(destination_file);

    free(abs_src_path);
    free(abs_dst_path);

    printf("File copied successfully from '%s' to '%s'\n", argv[1], argv[2]);
    return 0;
}

int check_file_existence(const char *filename) {
    return access(filename, F_OK) == 0;
}

int check_write_permission(const char *filename) {
    return access(filename, W_OK) == 0;
}

char *convert_to_absolute_path(const char *path) {
    int len = GetFullPathNameA(path, 0, NULL, NULL); // Узнаем необходимый размер буфера
    if (len == 0) {
        printf("Failed to convert path to absolute\n");
        return NULL;
    }

    char *absolute_path = malloc(len * sizeof(char));
    if (absolute_path == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    if (GetFullPathNameA(path, len, absolute_path, NULL) == 0) {
        free(absolute_path);
        printf("Failed to convert path to absolute\n");
        return NULL;
    }

    return absolute_path;
}

    printf("File copied successfully from '%s' to '%s'\n", argv[1], argv[2]);
    return 0;

}
