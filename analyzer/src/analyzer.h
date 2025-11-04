#pragma once
#include <cJSON.h>

cJSON *analyze_file_to_json(const char *path, const char **parse_args, int parse_args_count);
