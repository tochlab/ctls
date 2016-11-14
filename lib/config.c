#include "config.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *section_name;
    char *name;
    char *value;
} config_param_t;

static config_param_t **param_list;
static int param_list_size;
static char *current_section_name;

static void config_init( void ) {
    param_list = NULL;
    param_list_size = 0;
    current_section_name = NULL;
}

static char* string_duplicate(char *str) {   
    if( str == NULL ) {
        return NULL;
    }

    int str_size = strlen(str)+1;
    char *result = malloc(str_size);
    memcpy(result, str, str_size-1);
    result[str_size-1] = '\0';
    return result;
}

static void handle_section_start(char *str) {
    free(current_section_name);
    int len = strlen(str);

    current_section_name = calloc(len-2,1);

    char *end = strstr(str,"]");
    if( end == NULL ) {
        fprintf(stderr,"Bad name of section '%s'\n", str);
        exit(1);
    }
    strncpy(current_section_name,&str[ 1 ], len-2);
}

void config_load(char *config_file_name) {
    const int max_line_len = 1024;
    config_init();
    FILE *config_file = fopen(config_file_name, "r");
    if(config_file == NULL) {
        perror("config_load");
        exit(1);
    }

    char readed_line[max_line_len];
    int line_num = 0;
    while(!feof(config_file)) {
        line_num++;
        char *res = fgets(&readed_line[ 0 ], max_line_len, config_file);
        if( res == NULL ) {
            break;
        }
        if(res[0] == '#') {
            continue;
        }

        int len = strlen(res);        
        if( len == 1 ) {
            continue;
        }

        res[len-1] = '\0';
        if( res[0] == '[' ) {
            handle_section_start(res);
            continue;
        }
        char *_value = strstr(res, "=");
        if( _value == NULL ) {
            fprintf(stderr,"Error at string %d\n", line_num);
        }
        *_value = '\0';
        _value++;        

        config_param_t *param = malloc(sizeof(config_param_t));      
        param->name = string_duplicate(res);        
        param->value = string_duplicate(_value);
        param->section_name = string_duplicate(current_section_name);
        param_list_size++;
        config_param_t **new_param_list = realloc(param_list, param_list_size * sizeof(config_param_t*));
        if( new_param_list == NULL ) {
            fprintf(stderr,"realloc failed! Exiting...\n");
        } 
        param_list = new_param_list; 
        param_list[param_list_size-1] = param;
    }

    fclose(config_file);
}

static char *config_find_param(char *param) {
    for(int i = 0;i<param_list_size;i++) {
        if( strcmp(param, param_list[i]->name) == 0 ) {
            return param_list[i]->value;
        }
    }
    return NULL;
}

int config_get_int(char *param, int default_value) {
    char *res = config_find_param(param);
    if( res != NULL ) {
        int result = atoi(res);
        return result; 
    }
    else {
        return default_value;
    }
}

char *config_get_string(char *param, char *default_value) {
    char *res = config_find_param(param);
    if( res != NULL ) {
        char *result = string_duplicate(res);
        return result; 
    }
    else {
        return string_duplicate(default_value);
    }
}
