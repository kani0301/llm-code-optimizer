#include "json_util.h"
#include <cJSON.h>

cJSON *make_function_json(const char *name) {
    cJSON *o = cJSON_CreateObject();
    cJSON_AddStringToObject(o, "name", name ? name : "");
    cJSON_AddNumberToObject(o, "loc", 0);
    cJSON_AddNumberToObject(o, "branches", 0);
    cJSON_AddNumberToObject(o, "loops", 0);
    cJSON_AddNumberToObject(o, "returns", 0);
    cJSON_AddNumberToObject(o, "calls", 0);
    cJSON_AddNumberToObject(o, "calls_in_loops", 0);
    return o;
}

void add_int(cJSON *obj, const char *key, int val) {
    cJSON_DeleteItemFromObjectCaseSensitive(obj, key);
    cJSON_AddNumberToObject(obj, key, val);
}
