// Implementation for json_util.h — include the real cJSON header here so
// the public header doesn't force consumers to have the cJSON includePath.
#include "json_util.h"
#include <cJSON.h>

cJSON *make_function_json(const char *name) {
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return nullptr;
    // Add a name field to identify the function
    cJSON_AddStringToObject(obj, "name", name ? name : "");
    return obj;
}

void add_int(cJSON *obj, const char *k, int v) {
    if (!obj || !k) return;
    cJSON_AddNumberToObject(obj, k, v);
}
