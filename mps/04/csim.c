#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "math.h"
#include "cachelab.h"

typedef struct {
    int valid;
    long tag;
    int lru;
} line_t;

typedef struct {
    int num_lines;
    line_t *lines;
} cacheset_t;

typedef struct {
    int num_sets;
    cacheset_t *sets;
} cache_t;

int verbose = 0;
int s = 0, e = 0, b = 0;
char *t = "";
cache_t *cache;

cache_t *make_cache(int s, int e, int b);
void process_input(int argc, char **argv);
void usage();

int main(int argc, char **argv)
{
    int hit_count = 0, miss_count = 0, eviction_count = 0;

    process_input(argc, argv);

    // initialize and allocate cache
    cache = make_cache(s, e, b);

    // read file
    FILE *file;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char instr;
    unsigned address;
    int size;

    file = fopen(t, "r");
    if (file) {
        while ((read = getline(&line, &len, file)) != -1){
            if (line[0] != ' ') {
                // instruction load
                // don't do anything
                continue;
            }

            strcpy(line, &line[1]);
            
            if (verbose)
                printf("%s ", line);

            if ((sscanf(line, " %c %u,%d", &instr, &address, &size)) == 3) {
                if (instr == 'L') {
                    printf("load\n");
                } else if (instr == 'S') {
                    printf("store\n");
                } else if (instr == 'M') {
                    printf("modify\n");
                }

                printf("%s", line);
            }
        }
        fclose(file);
    }

    if (line)
        free(line);

    // print summary
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}


void process_input(int argc, char **argv) {
    char c;

    while ((c = getopt(argc, argv, "hvs:E:b:t:")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
            break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            e = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            t = optarg;
            break;
        default:
            usage();
            break;
        }
    }

    if (c == 0 || e == 0 || b == 0 || !strcmp(t, "")) {
        // invalid inputs
        usage();
    }
}

cache_t *make_cache(int s, int e, int b) {
    cache_t *cache = malloc(sizeof(cache_t *));

    cache->num_sets = pow(2, s);
    cache->sets = malloc(sizeof(cacheset_t) * cache->num_sets);
    
    for (int i = 0; i < cache->num_sets; i++) {
        cache->sets[i].num_lines = e;
        cache->sets[i].lines = malloc(sizeof(line_t) * e);
    }

    return cache;
}

int check_hit(char *address) {
//    int tagSize = 64 - s - b;
//    unsigned addr = strtoul(address, NULL, 16);
//    unsigned tag = addr >> (64 - tagSize);
//    unsigned block = addr << t >> (tagSize + b);

    return 0;
//    return cache->sets[index].lines[block].tag == tag;
}

void usage(void) 
{
  printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n");
  printf("Options:\n");
  printf("  -h         Print this help message.\n");
  printf("  -v         OPtional verbose flag,\n");
  printf("  -s <num>   Number of set index bits.\n");
  printf("  -E <num>   Number of lines per set.\n");
  printf("  -b <num>   Number of block offset bits.\n");
  printf("  -t <file>  Trace file.\n");
  printf("\n");
  printf("Examples:\n");
  printf("  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
  printf("  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
  exit(1);
}
