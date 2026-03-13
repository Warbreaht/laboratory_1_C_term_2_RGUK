/*4. Реализуйте консольное приложение для побайтовой обработки файлов. Аргументы в Ваше
приложение передаются как аргументы командной строки: первый параметр - путь ко
входному файлу; второй параметр - это флаг, который определяет действие, которое
необходимо выполнить с файлом:
○ xor8 - сложение по модулю 2 всех байтов файла;
○ xorodd - сложение по модулю 2 всех четырехбайтовых подпоследовательностей из
файла, значение хотя бы одного байта которых представляет собой простое число в
формате представления целого числа без знака;
○ mask <hex> - подсчет четырёхбайтовых целых чисел из файла, которые
соответствуют маске hex, которая задана в целого числа в системе счисления с
основанием 16 (соответствие маски вычисляется по следующему правилу: результат
поразрядной конъюнкции значения с маской равен самой маске).
Продемонстрируйте работу приложения для реализованных флагов на различных файлах.*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFFER_SIZE 4096

int is_prime(int);
unsigned int str_to_uint(const char*);
void process_xor8(FILE*);
void process_xorodd(FILE*);
void process_mask(FILE*, unsigned int);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <filename> <operation>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("Failed to open file\n");
        return -1;
    }

    unsigned int mask = 0;
    if (strcmp(argv[2], "mask") == 0) {
        if (argc < 4) {
            fclose(file);
            printf("Mask value required for operation 'mask'. Usage: %s <filename> mask <hex_mask>\n", argv[0]);
            return 1;
        }
        mask = str_to_uint(argv[3]);
        if (mask == 0) {
            fclose(file);
            printf("Invalid hexadecimal mask value.\n");
            return 1;
        }
    }

    if (strcmp(argv[2], "xor8") == 0) {
        process_xor8(file);
    } else if (strcmp(argv[2], "xorodd") == 0) {
        process_xorodd(file);
    } else if (strcmp(argv[2], "mask") == 0) {
        process_mask(file, mask);
    } else {
        fclose(file);
        printf("Unknown operation: %s\n", argv[2]);
        return 1;
    }

    fclose(file);
    return 0;
}

int is_prime(int num) {
    if (num < 2) return 0;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

unsigned int str_to_uint(const char *str) {
    unsigned int res = 0;
    while (*str) {
        char ch = toupper(*str++);
        if (!isxdigit(ch)) return 0;
        res <<= 4;
        res += (ch >= 'A' ? ch - 'A' + 10 : ch - '0');
    }
    return res;
}

void process_xor8(FILE *file) {
    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;
    unsigned char xor_result = 0;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            xor_result ^= buffer[i];
        }
    }
    printf("XOR8 Result: %d\n", xor_result);
}

void process_xorodd(FILE *file) {
    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;
    unsigned char xor_result = 0;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        for (size_t i = 0; i + 3 < bytes_read; i += 4) {
            unsigned char chunk_xor = 0;
            int has_prime = 0;
            for (int j = 0; j < 4; j++) {
                chunk_xor ^= buffer[i + j];
                if (is_prime(buffer[i + j])) has_prime = 1;
            }
            if (has_prime) xor_result ^= chunk_xor;
        }

        if (bytes_read % 4 != 0) {
            unsigned char last_chunk_xor = 0;
            int last_has_prime = 0;
            for (size_t k = bytes_read - (bytes_read % 4); k < bytes_read; k++) {
                last_chunk_xor ^= buffer[k];
                if (is_prime(buffer[k])) last_has_prime = 1;
            }
            if (last_has_prime) xor_result ^= last_chunk_xor;
        }
    }
    printf("XORODD Result: %d\n", xor_result);
}

void process_mask(FILE *file, unsigned int mask) {
    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;
    int count = 0;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        for (size_t i = 0; i + 3 < bytes_read; i += 4) {
            unsigned int word = *(unsigned int*)(buffer + i);
            if ((word & mask) == mask) count++;
        }
        if (bytes_read % 4 != 0) {
            unsigned int last_word = 0;
            for (size_t k = bytes_read - (bytes_read % 4); k < bytes_read; k++) {
                last_word |= (unsigned int)buffer[k] << ((k - (bytes_read - (bytes_read % 4))) * 8);
            }
            if ((last_word & mask) == mask) count++;
        }
    }
    printf("MASK Count: %d\n", count);
}
