#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "math.h"
#include "cachelab.h"

typedef struct {
    int index;
    int valid;
    int tag;
} line_t;

typedef struct {
    int num_lines;
    line_t **lines;
} cacheset_t;

typedef struct {
    int num_sets;
    cacheset_t **sets;
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
    process_input(argc, argv);

    // initialize and allocate cache
    cache = make_cache(1, 1, 1);

    printSummary(0, 0, 0);
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
}

cache_t *make_cache(int s, int e, int b) {
    cache_t *cache = malloc(sizeof(cache_t *));

    cache->num_sets = pow(2, s);
    cache->sets = calloc(sizeof(cacheset_t *), cache->num_sets);
    
    for (int i = 0; i < cache->num_sets; i++) {
        cacheset_t *set = malloc(sizeof(cacheset_t *));
        cache->sets[i] = set;
        set->num_lines = e;
        set->lines = calloc(sizeof(line_t *), e);

        for (int j = 0; j < e; j++) {
            line_t *line = malloc(sizeof(line_t *));
            set->lines[j] = line;
        }
    }

    return cache;
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