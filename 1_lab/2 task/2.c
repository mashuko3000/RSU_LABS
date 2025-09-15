#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define OK                    0
#define ERR_MALLOC            -1
#define ERR_INVALID_LOGIN     -2
#define ERR_LOGIN_EXISTS      -3
#define ERR_INVALID_PIN       -4
#define ERR_USER_NOT_FOUND    -5
#define ERR_INPUT             -6
#define ERR_DATETIME          -7
#define ERR_MKTINE            -8
#define ERR_REALLOC           -9
#define ERR_CANCEL            -10

volatile sig_atomic_t exit_flag = 0;

typedef struct
{
    char *login;
    int pin;
    int command_limit;
    int commands_used;
} User, user;

typedef struct
{
    User *users;
    int user_count;
    int user_capacity;
    int current_user_index;
} System;


void clear_input_buffer()
{
    int c;
    while((c = getchar()) != '\n' && c!=EOF);
}

void signal_handler(int signum)
{
    if(signum == SIGINT)
    {
        printf("\n Programm was interrupted by user. Exiting... \n");
        exit_flag = 1;
    }
}

char *read_string()
{
    int capacity = 20;
    char *str = (char*)malloc(sizeof(char) * capacity);
    int size = 0;
    int c;
    char* new;

    if (str == NULL)
    {
        return NULL;
    }

    while ((c = getchar()) != '\n' && c!= EOF)
    {
        if (c == '\x03')
        {
            free(str);
            return NULL;
        }
        if (size + 1 == capacity)
        {
            new = (char*)realloc(str, sizeof(char) * capacity * 2);
            if (new == NULL)
            {
                free(str);
                return NULL;
            }
            capacity *= 2;
            str = new;
        }
        str[size++] = c;
    }
    str[size] = '\0';
    return str;
}

int is_valid_login(const char* login)
{
    int i = 0;
    int len = strlen(login);

    if (login == NULL) return ERR_INVALID_LOGIN;

    if(len == 0 || len > 6)
    {
        return ERR_INVALID_LOGIN;
    }
    for (i; i < len; ++i)
    {
        if(!isalnum(login[i]) || !isascii(login[i])) return ERR_INVALID_LOGIN;
    }
    return OK;
}

int find_user(System* state, const char* login)
{
    int i;

    if(state == NULL || login == NULL) return ERR_INPUT;
    for(i = 0; i < state->user_count; i++)
    {
        if(strcmp(state->users[i].login, login) == 0) return i;
    }
    return -1;
}

int expand_users(System *state)
{
    User* temp;

    if(state == NULL) return ERR_INPUT;
    if(state->user_count >= state->user_capacity)
    {
        if(state->user_capacity == 0)
        {
            state->user_capacity = 1;
            state->users = (user*)malloc(sizeof(user) * state->user_capacity);
            if(state->users == NULL)
            {
                return ERR_MALLOC;
            }
            return OK;
        }
        else
        {
            temp = (user*)realloc(state->users, sizeof(user) * state->user_capacity * 2);
            if (temp == NULL)
            {
                return ERR_REALLOC;
            }
            state->user_capacity *= 2;
            state->users = temp;
        }
    }
    return OK;
}

int register_new_user(System* state)
{
    char *login = NULL;
    int result;
    int pin;

    if(state == NULL) return ERR_INPUT;
    printf("Registration: ");

    do
    {
        if (login != NULL)
        {
            free(login);
        }

        printf("Enter login (max 6 charecters, 0 to 9, a to z): ");
        login = read_string();
        if (login == NULL)
        {
            return ERR_CANCEL;
        }

        result = is_valid_login(login);

        if(result != OK){
            printf("Invalid login, please try again.");
        }
    } while(result != OK);

    while(find_user(state, login) != -1)
    {
        printf("Buddy, this login was already used, please choose another one( \n)");
        free(login);
        login = read_string();
        if(login == NULL)
        {
            return ERR_CANCEL;
        }
    }


    printf("Buddy, enter PIN (0 to 100000): ");
    


    if(scanf("%d", &pin) != 1)
    {
        printf("Invalid input for PIN.\n");
        clear_input_buffer();
        free(login);
        return ERR_INVALID_PIN;
    }
    clear_input_buffer();
    
    while (pin < 0 || pin > 100000)
    {
        printf("Dont make me repeat again! ENTER PIN FROM 0 TO 100000 ONLY DIGITS! \n");
        scanf("%d", &pin);
        if(scanf("%d", &pin) != 1)
        {
            printf("Invalid input for PIN.\n");
            clear_input_buffer();
            free(login);
            return ERR_INVALID_PIN;
        }
        clear_input_buffer();
    }
    
    if (expand_users(state) != OK)
    {
        free(login);
        return ERR_REALLOC;
    }

    state->users[state->user_count].login = login;
    state->users[state->user_count].pin = pin;
    state->users[state->user_count].command_limit = 0;
    state->users[state->user_count].commands_used = 0;
    state->user_count++;

    printf("Registration successful!\n");
    return OK;
}

int login_user(System* state)
{
    char* login;
    int idx;
    int pin;

    if (state == NULL) return ERR_INPUT;
    printf("Login: \n");
    login = read_string();
    if (login == NULL)
    {
        return ERR_CANCEL;
    }

    idx = find_user(state, login);
    while (idx == -1)
    {
        printf("User not found!");
        free(login);
        return ERR_USER_NOT_FOUND;
    }

    while(1)
    {
        if(scanf("%d", &pin) != 1)
        {
            printf("Invalid input for PIN, try again, enter a number from 0 to 100000");
            clear_input_buffer();
            free(login);
            return ERR_INPUT;
        }
        clear_input_buffer();

        if(state->users[idx].pin == pin)
        {
            break;
        }
        else
        {
            printf("Incorrect PIN. \n");
        }
    }

    state->current_user_index = idx;
    state->users[idx].commands_used = 0;
    printf("Login successful!\n");
    free(login);
    return 0;
}

void print_time() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t == NULL)
    {
        perror("Localtime failed");
        return;
    }
    printf("Current time: %02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
}

void print_date() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t == NULL)
    {
        perror("Localtime failed");
        return;
    }
    printf("Current date: %02d:%02d:%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
}

int parse_datetime(const char *datetime, struct tm *tm) {
    if (datetime == NULL || tm == NULL) return ERR_DATETIME;

    int day, month, year, hour, min, sec;
    if (sscanf(datetime, "%d:%d:%d %d:%d:%d", &day, &month, &year, &hour, &min, &sec) != 6) {
        return ERR_DATETIME;
    }
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1970 ||
        hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 || sec > 59) {
        return ERR_DATETIME;
    }

    tm->tm_mday = day;
    tm->tm_mon = month - 1;
    tm->tm_year = year - 1900;
    tm->tm_hour = hour;
    tm->tm_min = min;
    tm->tm_sec = sec;
    tm->tm_isdst = -1;
    return OK;
}

void howmuch(const char *datetime, const char *flag) {
    struct tm input_time = {0};
    time_t now = time(NULL);
    time_t input = mktime(&input_time);
    double diff;

    if (datetime == NULL || flag == NULL) return;

    
    if (!parse_datetime(datetime, &input_time)) {
        printf("Invalid datetime format! Use dd:MM:yyyy hh:mm:ss\n");
        return;
    }

    if(input == (time_t) - 1)
    {
        perror("mktine failed");
        return;
    }
    diff = difftime(now, input);

    if (strcmp(flag, "-s") == 0) {
        printf("Time passed: %.0f seconds\n", diff);
    } else if (strcmp(flag, "-m") == 0) {
        printf("Time passed: %.0f minutes\n", diff / 60);
    } else if (strcmp(flag, "-h") == 0) {
        printf("Time passed: %.0f hours\n", diff / 3600);
    } else if (strcmp(flag, "-y") == 0) {
        printf("Time passed: %.2f years\n", diff / (3600 * 24 * 365.25));
    } else {
        printf("Invalid flag! Use -s, -m, -h, or -y\n");
    }
}

void sanctions(System *state, const char *username) {
    int idx;
    int code;
    int limit;


    if (state == NULL || username == NULL)
    {
        return;
    }
    idx = find_user(state, username);
    if (idx == -1) {
        printf("User %s not found!\n", username);
        return;
    }

    printf("Enter confirmation code: ");
    
    if (scanf("%d", &code) != 1)
    {
        printf("Invalid input. \n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();
    
    if (code != 52) {
        printf("Incorrect confirmation code!\n");
        return;
    }

    printf("Enter command limit for %s: ", username);
    
    if (scanf("%d", &limit) != 1)
    {
        printf("Invalid input. \n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    if (limit < 0) {
        printf("Invalid limit! Must be non-negative.\n");
        return;
    }

    state->users[idx].command_limit = limit;
    printf("Sanctions applied: %s can execute %d commands per session.\n", username, limit);
}

int check_command_limit(System *state) {
    if (state->current_user_index == -1) return 1;
    if (state->users[state->current_user_index].command_limit == 0) return 1;
    if (state->users[state->current_user_index].commands_used >= state->users[state->current_user_index].command_limit) {
        printf("Command limit reached for this session!\n");
        return 0;
    }
    state->users[state->current_user_index].commands_used++;
    return 1;
}

void shell_loop(System *state) {
    char *command;
    char *datetime;
    char *flag;
    char *username;


    if (state == NULL) return;
    while (1) {
        if (exit_flag)
        {
            break;
        }

        if(!check_command_limit(state))
        {
            state->current_user_index = -1;
            return;
        }


        printf("> ");
        command = read_string();
        if (command == NULL)
        {
            state->current_user_index = -1;
            return;
        }


        if (strcmp(command, "Time") == 0) 
        {
            print_time();
        } 
        
        else if (strcmp(command, "Date") == 0) 
        {
            print_date();
        }

        else if (strcmp(command, "Howmuch") == 0)
        {
            datetime = read_string();
            flag = read_string();
            if(datetime != NULL && flag!=NULL)
            {
                howmuch(datetime, flag);
                free(datetime);
                free(flag);
            }
            else
            {
                printf("Error reading arguments for Howmuch. \n");
                if(datetime != NULL) free(datetime);
                if(flag != NULL) free(flag);
            }
            
        } else if (strcmp(command, "Logout") == 0) {
            printf("Logged out.\n");
            free(command);
            state->current_user_index = -1;
            return;

        } else if (strcmp(command, "Sanctions") == 0) {
            username = read_string();
            if (username != NULL)
            {
                sanctions(state, username);
                free(username);
            } 
            else
            {
                printf("Error reading usernames foe sanctions");
            }
            
        } else {
            printf("Unknown command! Available: Time, Date, Howmuch, Logout, Sanctions\n");
        }
        free(command);
    }
}

void free_System_state(System *state) {
    int i;

    if(state == NULL) return;
    if(state->users != NULL)
    {
        for (i = 0; i < state -> user_count; i++)
        {
            if (state->users[i].login != NULL)
            {
                free(state->users[i].login);
            }
        }
        free(state->users);
    }
    free(state);
}

System* init_System()
{
    System* state = (System*)malloc(sizeof(System));
    if (state == NULL)
    {
        return NULL;
    }
    state->users = NULL;
    state->user_count = 0;
    state->user_capacity = 0;
    state->current_user_index = -1;
    return state;
}

int main() {
    System *state = init_System();
    int choice;
    
    if (state == NULL)
    {
        return 1;
    }

    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        perror("Error sitting up signal helper");
        free_System_state(state);
        return 1;
    }

    while (1) {
        if (exit_flag)
        {
            break;
        }
        if (state->current_user_index == -1) {
            printf("\n1. Login\n2. Register\n3. Exit\nChoose an option: ");
            
            if (scanf("%d", &choice) != 1) {
                printf("Invalid input. Exiting.\n");
                clear_input_buffer();
                break;
            }
            clear_input_buffer();

            switch (choice) {
                case 1:
                    if (login_user(state)) {
                        shell_loop(state);
                    }
                    break;
                case 2:
                    register_new_user(state);
                    break;
                case 3:
                    printf("Goodbye!\n");
                    free_System_state(state);
                    return 0;
                default:
                    printf("Invalid option!\n");
            }
        } else {
            shell_loop(state);
        }
    }
    free_System_state(state);
    return 0;
}