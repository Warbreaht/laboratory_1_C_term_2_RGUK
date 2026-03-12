/*2. Напишите примитивную оболочку для командной строки. При запуске вашей программы
должно быть выведено приглашение к авторизации пользователя. У каждого пользователя
есть уникальный в рамках системы login (длиной не более 6 символов, который состоит из
символов латинского алфавита и символов цифр) и PIN-код (целое число в системе
счисления c основанием 10, значение числа варьируется в диапазоне 0 до 100000). Для
работы с оболочкой пользователь должен авторизоваться или зарегистрироваться
(интерфейс взаимодействия с пользователем для его регистрации продумайте
самостоятельно) в приложении. После удачной авторизации пользователя ему доступен
следующий набор команд:
○ Time - вывод в консоль текущего времени в стандартном формате “чч:мм:сс”;
○ Date - вывод в консоль текущей даты в стандартном формате “дд:ММ:гггг”;
○ Howmuch <datetime> flag - запрос прошедшего времени с указанного момента в
параметре datetime в формате “дд:ММ:гггг чч:мм:сс”, параметр flag определяет тип
представления результата (“-s” - в секундах, “-m” - в минутах, “-h” - в часах, “-y” - в
годах)
○ Logout - выйти в меню регистрации/авторизации
○ Sanctions <username> - команда позволяет ввести ограничения на работу с оболочкой
для пользователя с логином username, а именно: данный пользователь не может в
одном сеансе выполнить более запросов. Для подтверждения ограничений после
ввода команды необходимо ввести значение 52.*/
//USER_DATA_lab_1_task_2_term_2.txt

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define MAX_COMMAND_LENGHT 100
#define FILENAME "USER_DATA_lab_1_task_2_term_2.txt"

typedef struct User {
    char* login;
    char* password;
    int access;
    struct User* next;
} User;

typedef struct List_user {
    User* head;
} List;

int get_user_list(List*);
void write_users_to_file(List*);

int primary_interaction_window();
void authorization_function(List*, User*);
void registration_function(List*, User*);

int validate_login(char*);
int validate_password(const char*);
int validate_check(char*, char*);
int find_user_by_login(List*, char*);
User* get_user_by_login(List*, const char*);

char* get_password(List*, char*);
char* trim_password(char*);

void add_user_to_list(List*, char*, char*);
void print_user_list(List* users_list);
void destroy_user_list(List*);

void sanctions_function(List*, User*, char*);
char* extract_login_from_command(const char*);
void Time_function();
void Data_function();
//void Howmuch_function(const char*); не работает

int main() {
    printf("The primitive shell is running!\n");

    List* User_list = (List*)malloc(sizeof(List));
    if (User_list == NULL) {
        printf("Error: no memory allocated");
        return 1;
    }
    User_list->head = NULL;

    print_user_list(User_list);
    int ANSWER_get_user_list = get_user_list(User_list);
    switch (ANSWER_get_user_list) {
        case 0:
            printf("The user database file has been read\n==========\n");
            break;
        case -1:
            printf("User database file not found, file created\n==========\n");
            break;
        case -2:
            printf("User database file not found, file not created\n==========\n");
            return -2;
        case -3:
            printf("Failed to access the user database file\n==========\n");
        default:
            printf("Unknow error\n==========\n");
            return 0;
    }

    User* Current_user = (User*)malloc(sizeof(User));
    if (Current_user == NULL) {
        printf("Couldn't allocate memory for the user\n");
        return -1;
    }
    
    int ANSWER_primary_interaction_window;
    char ANSWER_current_user[256];
    int count = 0;
    printf("List of commands\n==========\n"
        "logout - log out of the user\nEXIT - end the program\n"
        "Sanctions <username> - restrict username rights until the end of the session\n"
        "Time - displays the current time.\n"
        "Date - displays the current date\n"
        "==========\n");

    while (strcmp(ANSWER_current_user, "EXIT") != 0) {
        if (count == 0) {
            do {
                ANSWER_primary_interaction_window = primary_interaction_window();
                if (ANSWER_primary_interaction_window == 1) {
                    authorization_function(User_list, Current_user);
                } else if (ANSWER_primary_interaction_window == 2) {
                    registration_function(User_list, Current_user);
                    printf("The user is registered\n");
                } else {
                    printf("Invalid command\n");
                }
                printf("==========\n");
            } while (ANSWER_primary_interaction_window == 0);
            count = 1;
        }
        //: сообщение дублируется
        //         |
        printf("Users/%s/>", Current_user->login);
        fgets(ANSWER_current_user, sizeof(ANSWER_current_user), stdin);
        ANSWER_current_user[strcspn(ANSWER_current_user, "\n")] = '\0';

        if (strstr(ANSWER_current_user, "Sanctions") != NULL) {
            if (Current_user->access == 0) {
                sanctions_function(User_list, Current_user, ANSWER_current_user);
            } else {
                printf("User %s is restricted until the end of the session\n", Current_user->login);
            }
        }
        if (strcmp(ANSWER_current_user, "Time") == 0) {
            if (Current_user->access == 0) {
                Time_function();
            } else {
                printf("User %s is restricted until the end of the session\n", Current_user->login);
            }
        }
        if (strcmp(ANSWER_current_user, "Data") == 0) {
            if (Current_user->access == 0) {
                Data_function();
            } else {
                printf("User %s is restricted until the end of the session\n", Current_user->login);
            }
        }
        /*if (strstr(ANSWER_current_user, "Howmuch") != NULL) {
            //НЕ работает
            if (Current_user->access == 0) {
                Howmuch_function(ANSWER_current_user);
            } else {
                printf("User %s is restricted until the end of the session\n", Current_user->login);
            }
        }*/
        if (strcmp(ANSWER_current_user, "logout") == 0) {
            count = 0;
        }
    }

    print_user_list(User_list);
    write_users_to_file(User_list);
    destroy_user_list(User_list);

    free(Current_user->login);
    free(Current_user->password);
    free(Current_user->next);
    free(Current_user);
    return 0;
}

int primary_interaction_window() {
    char user_request[MAX_COMMAND_LENGHT];
    printf("'-AUT': authorization\n"
        "'-REG': registration\n"
        "Enter the command:");
    
    scanf("%s", user_request);
    printf("==========\n");

    
    if (strcmp(user_request, "-AUT") == 0) {
        return 1;
    } else if (strcmp(user_request, "-REG") == 0) {
        return 2;
    } else {
        return 0;
    }
}

//===========================================================================

int get_user_list(List* user_list) {
      if (access(FILENAME, F_OK) != 0) {
          return 0;
      }
      FILE* file = fopen(FILENAME, "r");
      if (file == NULL) {
          return -3;
      }
      char buffer_line[256];
      while (fgets(buffer_line, sizeof(buffer_line), file)) {
          char* token = strtok(buffer_line, ":");
          if (token != NULL) {
              char* login = strdup(token);

              token = strtok(NULL, "\n");
              if (token != NULL) {
                  char* password = strdup(token);
                  add_user_to_list(user_list, login, password);
              }
          }
      }
      fclose(file);
      return 0;
}

void write_users_to_file(List* users_list) {
    FILE* file = fopen(FILENAME, "w");
    if (file == NULL) {
        return;
    }
    User* current = users_list->head;
    do {
        fprintf(file, "%s:%s\n", current->login, current->password);
        current = current->next;
    } while (current != NULL);

    fclose(file);
}

//===========================================================================

void registration_function(List* user_list, User* Current_user) {
    printf("Registration\n==========\n");
    printf("login (no more than 6 characters long, consisting of Latin alphabet characters and numeric characters) \nand a PIN code (a whole number in the base 10 number system, with a value ranging from 0 to 100000).\n==========\n");
    int ANSWER_validate_check = 1, ANSWER_find_user_by_login = 1;
    char new_login[256];
    char new_password[256];
    while (ANSWER_validate_check != 0) {
        printf("Enter new login: ");
        scanf("%s", new_login);
        printf("Enter new password: ");
        scanf("%s", new_password);
        printf("==========\n");
        ANSWER_validate_check = validate_check(new_login, new_password);
        ANSWER_find_user_by_login = find_user_by_login(user_list, new_login);
        if (ANSWER_find_user_by_login == 1) {
            printf("This username has already been registered\n==========\n");
            continue;
        }
    }
    add_user_to_list(user_list, new_login, new_password);

    Current_user->login = strdup(new_login);
    Current_user->password = strdup(new_password);
    Current_user->access = 0;
    Current_user->next = NULL;
}

void authorization_function(List* user_list, User* Current_user) {
    if (user_list->head == NULL) {
        printf("Not a single user is registered\n==========\n");
    }
    printf("Authorization\n==========\n");
    char login[6];
    int ANSWER_find_user_by_login = 0;
    while(ANSWER_find_user_by_login != 1) {
        printf("Enter login: ");
        scanf("%s", login);
        ANSWER_find_user_by_login = find_user_by_login(user_list, login);
        if (ANSWER_find_user_by_login == 0) {
            printf("There is no such login\n==========\n");
        }
    }
    User* authorization_user = get_user_by_login(user_list, login);
    if (authorization_user == NULL) {
        printf("User not found\n");
        return;
    }

    char* password = authorization_user->password;
    printf("%s\n", password);
    char user_password[8];
    while (1) {
        printf("Enter password: ");
        scanf("%s", user_password);
        if (strcmp(user_password, password) == 0) {
            printf("Password is correct\n");
            break;
        } else {
            printf("Password is not correct\n");
        }
    }
    Current_user->access = authorization_user->access;
    Current_user->login = strdup(authorization_user->login);
    Current_user->password = strdup(password);
    Current_user->next = NULL;

    free(Current_user->login);
    free(Current_user->password);
}

char* trim_password(char* password) {
    if (password == NULL || *password == '\0')
        return password;

    char* start = password;
    while (*start != '\0' && isspace(*start)) {
        start++;
    }
    if (*start == '\0')
        return start;
    char* end = start + strlen(start) - 1;
    while (end >= start && isspace(*end)) {
        end--;
    }

    *(end + 1) = '\0';
    return start;
}
//=============================================================================

//======================================
void sanctions_function(List* user_list, User* current_user, char* command) {
    if (user_list == NULL) {
        printf("There is no list of users\n");
        return;
    }
    if (user_list->head == NULL) {
        printf("There are no users in the system\n");
        return;
    }
    if (current_user == NULL) {
        printf("The current user does not exist\n");
        return;
    }
    if (command == NULL) {
        printf("Invalid command\n");
        return;
    }
    char* login = extract_login_from_command(command);
    if (login == NULL) {
        printf("Invalid format. Valid format: sanctions <username>\n");
        return;
    }

    User* sanctioned_user = get_user_by_login(user_list, login);
    if (sanctioned_user == NULL) {
        printf("The '%s' user does not exist\n", login);
        free(login);
        return;
    }

    printf("The '%s' user has been found. Confirm the command by typing 52\nEnter the confirmation: ", login);
    int ANSWER_current_user;
    scanf("%d", &ANSWER_current_user);
    if (ANSWER_current_user == 52) {
        sanctioned_user->access = 1;
        printf("The '%s' user is restricted until the end of the session\n", login);
    } else {
        printf("The command has been cancelled\n");
    }
    free(login);
    return;
}

char* extract_login_from_command(const char* command) {
    const char* space_pos = strchr(command, ' ');
    if (space_pos == NULL) {
        return NULL;
    }
    int  len = strlen(space_pos + 1) + 1;
    char* login_copy = (char*)malloc(len);
    if (login_copy == NULL) {
        return NULL;
    }
    strcpy(login_copy, space_pos + 1);
    return login_copy;
}
//======================================

//TODO: разобраться как работает

void Time_function() {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (timeinfo == NULL) {
        printf("Error getting local time\n");
        return;
    }

    char buffer[80];
    if (strftime(buffer, sizeof(buffer),  "%H:%M:%S", timeinfo) == 0) {
        printf("Error formatting time\n");
        return;
    }

    printf("Current time: %s\n", buffer);
}

void Data_function() {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (timeinfo == NULL) {
        printf("Error getting local time\n");
        return;
    }

    char buffer[80];
    if (strftime(buffer, sizeof(buffer),  "%Y-%m-%d", timeinfo) == 0) {
        printf("Error formatting time\n");
        return;
    }

    printf("Current time: %s\n", buffer);
}

/*void Howmuch_function(const char *command) {
    printf("1: %s\n", command);
    // Перемещаемся после слова "Howmuch "
    const char *data = command + 8;
    printf("2: %s\n", data);
    // Ищем положение пробела, разделяющего дату и флаг
    const char *space_pos = strchr(data, ' ');
    if (space_pos == NULL) {
        printf("Error: Invalid command format.\n");
        return;
    }

    // Длина даты-времени
    size_t dt_len = space_pos - data;

    // Выделяем дату-время и флаг
    char datetime_str[sizeof("DD:MM:YYYY HH:MM:SS")]; // Увеличенный буфер
    strncpy(datetime_str, data, dt_len);
    datetime_str[dt_len] = '\0';
    printf("3: %s\n", datetime_str);
    char flag[3];
    strncpy(flag, space_pos + 1, sizeof(flag) - 1); // Забираем остаток строки после пробела
    flag[sizeof(flag) - 1] = '\0';  
    printf("4: %s\n", flag);               // Завершаем строку

    // Проверка формата даты-времени
    struct tm tm_input = {0};
    if (sscanf(datetime_str, "%d:%d:%d %d:%d:%d",
               &tm_input.tm_mday, &tm_input.tm_mon, &tm_input.tm_year,
               &tm_input.tm_hour, &tm_input.tm_min, &tm_input.tm_sec) != 6) {
        printf("Error: Invalid date-time format.\n");
        return;
    }

    // Корректировка индексов
    tm_input.tm_mon--;           // Месяцы нумеруются с 0
    tm_input.tm_year -= 1900;    // Годы отсчитываются с 1900

    // Проверка корректности флага
    if (strcmp(flag, "-s") &&
        strcmp(flag, "-m") &&
        strcmp(flag, "-h") &&
        strcmp(flag, "-y")) {
        printf("Error: Unknown output flag.\n");
        return;
    }

    // Преобразуем дату-время в timestamp
    time_t start_time = mktime(&tm_input);
    if (start_time == -1) {
        printf("Error: Invalid or out-of-range date-time.\n");
        return;
    }

    // Получаем текущее время
    time_t current_time = time(NULL);

    // Рассчитываем разницу в секундах
    double delta_seconds = difftime(current_time, start_time);

    // Выводим результат в зависимости от флага
    if (strcmp(flag, "-s") == 0) {
        printf("%.0lf\n", delta_seconds);          // Выводим секунды
    } else if (strcmp(flag, "-m") == 0) {
        printf("%.0lf\n", delta_seconds / 60);    // Минуты
    } else if (strcmp(flag, "-h") == 0) {
        printf("%.0lf\n", delta_seconds / 3600);  // Часы
    } else if (strcmp(flag, "-y") == 0) {
        printf("%.8lf\n", delta_seconds / (365.25 * 24 * 3600)); // Приблизительно годы
    }
}*/


//=============================================================================

void add_user_to_list(List* users_list, char* login, char* password) {
    User* new_user = (User*)malloc(sizeof(User)); //TODO: поменять
    if (new_user == NULL) {
        perror("Ошибка выделения памяти");
        return;
    }
    new_user->login = strdup(login);
    new_user->password = strdup(password);
    new_user->access = 0;
    new_user->next = NULL;

    if (users_list->head == NULL) {
        users_list->head = new_user;
    } else {
        User* last = users_list->head;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = new_user;
    }
}

void print_user_list(List* users_list) {
    if (users_list->head == NULL) {
        return;
    }
    printf("Print list\n");
      User* current = users_list->head;
      while (current != NULL) {
          printf("Login: %s, Password: %s, Access %d\n", current->login, current->password, current->access);
          current = current->next;
      }
}

void destroy_user_list(List* users_list) {
    User* current = users_list->head;
    User* temp;

    while (current != NULL) {
        temp = current;
        current = current->next;

        free(temp->login);
        free(temp->password);
        free(temp);
    }
    users_list->head = NULL;
}

//===============================================================

int validate_login(char* login) {
    int len = strlen(login);
    if (len < 1 || len > 6) {
        return 1;
    }
    int i;
    for (i = 0; i < len; i++) {
        if (!isalnum(login[i])) {
            return 1;
        }
    }
    return 0;
}

int validate_password(const char* password) {
    int i;
    for (i = 0; password[i] != '\0'; i++) {
        if (!isdigit((unsigned char)password[i])) {
            return 1;
        }
    }

    unsigned int num = strtoul(password, NULL, 10);
    if (num >= 100000) {
        return 1;
    }
    return 0;
}

int validate_check(char* login, char* password) {
    if (validate_login(login) == 0 && validate_password(password) == 0) {
        return 0;
    }
    printf("1\n");
    return 1;
}

int find_user_by_login(List* user_list, char* login) {
    User* current = user_list->head;

    while (current != NULL) {
        if (strcmp(current->login, login) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

User* get_user_by_login(List* user_list, const char* login) {
    User* current = user_list->head;

    while (current != NULL) {
        if (strcmp(current->login, login) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

char* get_password(List* user_list, char* login) {
    User* current = user_list->head;

    while (current != NULL) {
        if (strcmp(current->login, login) == 0) {
            return strdup(current->password);
        }
        current = current->next;
    }
}
