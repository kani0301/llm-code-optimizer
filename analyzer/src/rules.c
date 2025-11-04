#include "rules.h"
#include <cJSON.h>
#include <string.h>

#define LARGE_FUNC_LOC 120
#define INLINE_SMALL_LOC 8
#define CALLS_IN_LOOP_THRESHOLD 2

void apply_rules(cJSON *root, cJSON *rules_out_array) {
    cJSON *functions = cJSON_GetObjectItemCaseSensitive(root, "functions");
    if (!functions) return;
    cJSON *fn = NULL;
    cJSON_ArrayForEach(fn, functions) {
        cJSON *name = cJSON_GetObjectItemCaseSensitive(fn, "name");
        cJSON *loc = cJSON_GetObjectItemCaseSensitive(fn, "loc");
        cJSON *calls = cJSON_GetObjectItemCaseSensitive(fn, "calls");
        cJSON *calls_in_loops = cJSON_GetObjectItemCaseSensitive(fn, "calls_in_loops");
        const char *fname = name ? name->valuestring : "";
        int iloc = loc ? (int)loc->valuedouble : 0;
        int icalls = calls ? (int)calls->valuedouble : 0;
        int icalls_in_loops = calls_in_loops ? (int)calls_in_loops->valuedouble : 0;

        if (iloc >= LARGE_FUNC_LOC) {
            cJSON *r = cJSON_CreateObject();
            cJSON_AddStringToObject(r, "rule", "refactor-large");
            cJSON_AddStringToObject(r, "function", fname);
            cJSON_AddStringToObject(r, "message", "Function exceeds LOC threshold; consider breaking into smaller functions.");
            cJSON_AddNumberToObject(r, "loc", iloc);
            cJSON_AddItemToArray(rules_out_array, r);
        }
        if (iloc > 0 && iloc <= INLINE_SMALL_LOC) {
            cJSON *r = cJSON_CreateObject();
            cJSON_AddStringToObject(r, "rule", "inline-small");
            cJSON_AddStringToObject(r, "function", fname);
            cJSON_AddStringToObject(r, "message", "Small function candidate for inlining if ABI/semantics allow.");
            cJSON_AddNumberToObject(r, "loc", iloc);
            cJSON_AddItemToArray(rules_out_array, r);
        }
        if (icalls_in_loops >= CALLS_IN_LOOP_THRESHOLD) {
            cJSON *r = cJSON_CreateObject();
            cJSON_AddStringToObject(r, "rule", "hoist-calls-in-loops");
            cJSON_AddStringToObject(r, "function", fname);
            cJSON_AddStringToObject(r, "message", "Multiple calls inside loops; consider hoisting invariant calls.");
            cJSON_AddNumberToObject(r, "calls_in_loops", icalls_in_loops);
            cJSON_AddNumberToObject(r, "calls", icalls);
            cJSON_AddItemToArray(rules_out_array, r);
        }
    }
}
