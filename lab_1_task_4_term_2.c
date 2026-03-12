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

#define BUFFER 4096
#define TRUE 1
#define FALSE 0

int is_prime(int);
unsigned int str_to_uint(const char*);
void process_file(FILE*, const char*, unsigned int);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <filename> <operation>\n", argv[0]);
        return 0;
    }
    
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("Failed to open file");
        return -1;
    }

    unsigned int mask = 0;
    if (strcmp(argv[2], "mask") == 0) {
        if (argc < 4) {
            fclose(file);
            printf("Mask value required for operation 'mask'. Usage: %s <filename> mask <hex_mask>\n", argv[0]);
            return 0;
        }
        mask = str_to_uint(argv[3]);
        if (mask == 0) {
            fclose(file);
            printf("Invalid hexadecimal mask value.\n");
            return 0;
        }
    }
    process_file(file, argv[2], mask);
    fclose(file);
    return 0;
}

int is_prime(int num) {
    if (num < 2) {
        return FALSE;
    }
    int i;
    for (i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

unsigned int str_to_uint(const char *str) {
    unsigned int value = 0;
    while(*str) {
        if (isxdigit(*str)) {
            return 0;
        }
        value <<= 4;
        char ch = toupper(*str++);
        value += (ch >= 'A' ? ch - 'A' + 10 : ch - '0');
    }
    return value;
}

void process_file(FILE *file, const char* operation, unsigned int mask) {
    int buffer[BUFFER];
    size_t bytes_read;
    int xor_result;
    int count = 0;

    while ((bytes_read = fread(buffer, 1, BUFFER, file))) {
        if (strcmp(operation, "xor8") == 0) {
            int i;
            for (i = 0; i < bytes_read; i++) {
                xor_result ^= buffer[i];
            }
        } else if (strcmp(operation, "xorodd") == 0) {
            int i, j;
            for (i = 0; i + 3 < bytes_read; i += 4) {
                int chunk_xor = 0;
                int has_prime = FALSE;
                for (j = 0; j < 4; j++) {
                    chunk_xor ^= buffer[i + j];
                    if (is_prime(buffer[i + j])) {
                        has_prime = TRUE;
                    }
                }
                if (has_prime) {
                    xor_result ^= chunk_xor;
                }
            }
        } else if (strcmp(operation, "mask") == 0) {
            int i;
            for (i = 0; i + 3 < bytes_read; i += 4) {
                int word = *(int*)(buffer + i);
                if ((word & mask) == mask) {
                    count++;
                }
            }
        }
    }
    if (strcmp(operation, "xor8") == 0 || strcmp(operation, "xorodd") == 0) {
        printf("Result: %d", xor_result);
    } else if (strcmp(operation, "mask") == 0) {
        printf("Count: %d", count);
    }
}