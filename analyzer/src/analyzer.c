#define _CRT_SECURE_NO_WARNINGS
#include "analyzer.h"
#include "json_util.h"
#include <clang-c/Index.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define APP_VERSION "1.0.0"

/* Deterministic cursor-kind helpers */
static int is_loop_kind(enum CXCursorKind k) {
    return k == CXCursor_ForStmt || k == CXCursor_WhileStmt || k == CXCursor_DoStmt;
}
static int is_branch_kind(enum CXCursorKind k) {
    return k == CXCursor_IfStmt || k == CXCursor_SwitchStmt;
}
static int is_return_kind(enum CXCursorKind k) {
    return k == CXCursor_ReturnStmt;
}
static int is_call_kind(enum CXCursorKind k) {
    return k == CXCursor_CallExpr;
}
static int is_function_decl(enum CXCursorKind k) {
    return k == CXCursor_FunctionDecl;
}

/* Estimate lines of code for a cursor (function). */
static int estimate_loc(CXCursor cursor) {
    CXSourceRange range = clang_getCursorExtent(cursor);
    CXSourceLocation start = clang_getRangeStart(range);
    CXSourceLocation end = clang_getRangeEnd(range);
    unsigned sl=0, sc=0, el=0, ec=0;
    clang_getSpellingLocation(start, NULL, &sl, &sc, NULL);
    clang_getSpellingLocation(end, NULL, &el, &ec, NULL);
    if (el >= sl) return (int)(el - sl + 1);
    return 0;
}

/* Context used while traversing inside a function */
typedef struct {
    cJSON *functions_array; // top-level functions array
    cJSON *current_func;    // function JSON when inside
    int loop_depth;
} TraverseCtx;

/* Utility to increment field */
static void incr(cJSON *obj, const char *field) {
    cJSON *it = cJSON_GetObjectItemCaseSensitive(obj, field);
    if (!it) { cJSON_AddNumberToObject(obj, field, 1); return; }
    it->valuedouble = it->valuedouble + 1;
}

/* Recursive visitor */
enum CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
    TraverseCtx *ctx = (TraverseCtx *)client_data;
    enum CXCursorKind kind = clang_getCursorKind(cursor);

    if (is_function_decl(kind)) {
        /* New function: create JSON and push children with a new context */
        CXString name = clang_getCursorSpelling(cursor);
        const char *cname = clang_getCString(name);
        cJSON *f = make_function_json(cname ? cname : "");
        int loc = estimate_loc(cursor);
        add_int(f, "loc", loc);
        cJSON_AddItemToArray(ctx->functions_array, f);

        TraverseCtx sub = { ctx->functions_array, f, 0 };
        clang_visitChildren(cursor, visitor, &sub);
        clang_disposeString(name);
        return CXChildVisit_Continue;
    }

    /* If not inside a function, continue (we only count inside functions) */
    if (!ctx->current_func) {
        return CXChildVisit_Recurse;
    }

    /* handle nodes */
    if (is_loop_kind(kind)) {
        incr(ctx->current_func, "loops");
        ctx->loop_depth++;
        clang_visitChildren(cursor, visitor, ctx);
        ctx->loop_depth--;
        return CXChildVisit_Continue;
    }

    if (is_branch_kind(kind)) {
        incr(ctx->current_func, "branches");
        return CXChildVisit_Recurse;
    }

    if (is_return_kind(kind)) {
        incr(ctx->current_func, "returns");
        return CXChildVisit_Recurse;
    }

    if (is_call_kind(kind)) {
        incr(ctx->current_func, "calls");
        if (ctx->loop_depth > 0) incr(ctx->current_func, "calls_in_loops");
        return CXChildVisit_Recurse;
    }

    return CXChildVisit_Recurse;
}

cJSON *analyze_file_to_json(const char *path, const char **parse_args, int parse_args_count) {
    CXIndex idx = clang_createIndex(0, 0);
    CXTranslationUnit tu = clang_parseTranslationUnit(idx, path, parse_args, parse_args_count, NULL, 0, CXTranslationUnit_None);
    if (!tu) {
        fprintf(stderr, "Failed to parse %s\n", path);
        clang_disposeIndex(idx);
        return NULL;
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "file", path);
    cJSON *meta = cJSON_CreateObject();
    cJSON_AddStringToObject(meta, "analyzer_version", APP_VERSION);
    cJSON_AddItemToObject(root, "meta", meta);

    cJSON *functions = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "functions", functions);

    TraverseCtx ctx = { functions, NULL, 0 };
    CXCursor cursor = clang_getTranslationUnitCursor(tu);
    clang_visitChildren(cursor, visitor, &ctx);

    clang_disposeTranslationUnit(tu);
    clang_disposeIndex(idx);
    return root;
}
