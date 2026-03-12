/*1. В этом задании используйте функционал: FILE *, fopen, fclose, fwrite, fread, fseek.
Создайте файл (путь к файлу передаётся как аргумент командной строки), в котором
содержится следующая последовательность байт: 3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5. После
создания файла откройте и прочитайте этот файл с побайтовым выводом содержимого
этого файла на консоль. На каждой итерации побайтового чтения также выводите в
консоль состояние всех полей структуры FILE, через указатель на которую
взаимодействуете с файлом. Закройте файл и откройте его на чтение снова. С помощью
функции fseek выполните перемещение указателя чтения записи на 3 относительно начала
файла (флаг SEEK_SET). С помощью функции fread считайте 4 байта из файла в байтовый
буфер. Что будет содержать буфер, когда завершится работа функции fread?
*/

#include <stdio.h>
#include <stdlib.h>

void print_file_status(FILE *fp);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
        return 1;
    }

    const unsigned char byte_sequence[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    const char *filename = argv[1];

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        perror("Error opening file for writing");
        return 1;
    }

    fwrite(byte_sequence, sizeof(unsigned char), sizeof(byte_sequence), fp);
    fclose(fp);

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error opening file for reading");
        return 1;
    }

    unsigned char byte;
    while (fread(&byte, sizeof(unsigned char), 1, fp) > 0) {
        printf("Byte: %d\n", byte);
        print_file_status(fp);
    }

    fclose(fp);

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error reopening file");
        return 1;
    }

    fseek(fp, 3, SEEK_SET);

    unsigned char buffer[4];
    fread(buffer, sizeof(unsigned char), 4, fp);

    printf("Buffer content after fseek and fread: ");
    for (int i = 0; i < 4; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");

    fclose(fp);
    return 0;
}

void print_file_status(FILE *fp) {
    long pos = ftell(fp);
    if (pos == -1L) {
        printf("Error getting file position");
        return;
    }
    printf("File position: %ld\n", pos);
}
