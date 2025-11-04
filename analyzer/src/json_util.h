#pragma once
#include <cJSON.h>

cJSON *make_function_json(const char *name);
void add_int(cJSON *obj, const char *k, int v);
