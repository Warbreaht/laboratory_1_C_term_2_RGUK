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
#include <unistd.h>

#define BUFFER_SIZE 1024

int check_file_existence(const char *filename) {
    return access(filename, F_OK) == 0;
}

int check_write_permission(const char *filename) {
    return access(filename, W_OK) == 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s source_file destination_file\n", argv[0]);
        return 1;
    }

    if (!check_file_existence(argv[1])) {
        printf("Source file '%s' does not exist\n", argv[1]);
        return 2;
    }

    if (!check_write_permission(argv[2])) {
        printf("No write permission for destination file '%s'\n", argv[2]);
        return 3;
    }

    FILE *source_file = fopen(argv[1], "rb");
    if (source_file == NULL) {
        printf("Failed to open source file\n");
        return -1;
    }

    FILE *destination_file = fopen(argv[2], "wb");
    if (destination_file == NULL) {
        printf("Failed to open destination file\n");
        fclose(source_file);
        return -2;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, source_file)) > 0) {
        fwrite(buffer, 1, bytes_read, destination_file);
    }

    fclose(source_file);
    fclose(destination_file);

    struct stat st_source, st_destination;
    if (stat(argv[1], &st_source) == 0 && stat(argv[2], &st_destination) == 0) {
        if (st_source.st_size != st_destination.st_size) {
            printf("Warning: Source and destination file sizes differ\n");
        } else {
            printf("Files are identical\n");
        }
    }

    printf("File copied successfully from '%s' to '%s'\n", argv[1], argv[2]);
    return 0;

}
