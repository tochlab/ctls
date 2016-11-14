#ifndef _CONFIG_H_
#define _CONFIG_H_

void config_load(char *filename);

int config_get_int(char *param, int default_value);
char *config_get_string(char *param, char *default_value);

#endif /* _CONFIG_H_ */