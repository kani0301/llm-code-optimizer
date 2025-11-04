#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "analyzer.h"
#include "rules.h"
#include "json_util.h"
#include "version.h"
#include <cJSON.h>

#ifdef HAVE_CURL
#include <curl/curl.h>
#endif

void usage(const char *prog) {
    printf("Usage: %s [--json out.json] [--llm endpoint] <file.c>\n", prog);
    puts("  --json <file>   Write JSON output to file (otherwise stdout)");
    puts("  --llm <url>     (optional) send to LLM endpoint");
    puts("  --version       Print version");
}

int main(int argc, char **argv) {
    char *json_out = NULL;
    char *llm_endpoint = NULL;
    int print_version = 0;

    static struct option long_opts[] = {
        {"json", required_argument, NULL, 'j'},
        {"llm", required_argument, NULL, 'l'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {0,0,0,0}
    };
    int opt;
    while ((opt = getopt_long(argc, argv, "j:l:vh", long_opts, NULL)) != -1) {
        switch(opt) {
            case 'j': json_out = strdup(optarg); break;
            case 'l': llm_endpoint = strdup(optarg); break;
            case 'v': print_version = 1; break;
            case 'h': usage(argv[0]); return 0;
            default: usage(argv[0]); return 1;
        }
    }

    if (print_version) {
        printf("%s %s\n", APP_NAME, APP_VERSION);
        return 0;
    }

    if (optind >= argc) {
        fprintf(stderr, "Input C file required\n");
        usage(argv[0]);
        return 1;
    }

    const char *input_file = argv[optind];

    const char *parse_args[] = { "-std=c11" };
    cJSON *root = analyze_file_to_json(input_file, parse_args, 1);
    if (!root) { fprintf(stderr, "Analysis failed\n"); return 1; }

    /* apply rules */
    cJSON *rules_arr = cJSON_CreateArray();
    apply_rules(root, rules_arr);
    cJSON_AddItemToObject(root, "rules", rules_arr);

    /* deterministic print */
    char *out = cJSON_PrintUnformatted(root);
    if (json_out) {
        FILE *f = fopen(json_out, "wb");
        if (!f) { perror("fopen"); free(out); return 1; }
        fputs(out, f); fclose(f);
    } else {
        puts(out);
    }

    free(out);
    cJSON_Delete(root);
    if (json_out) free(json_out);
    if (llm_endpoint) free(llm_endpoint);
    return 0;
}
