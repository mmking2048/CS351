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
int s = 0, e = 0, b = 0, lineNum = 0;
char *t = "";
cache_t *cache;
int hit_count = 0, miss_count = 0, eviction_count = 0;

cache_t *make_cache(int s, int e, int b);
void process_input(int argc, char **argv);
void free_cache(cache_t* cache);
int check_hit(unsigned long long int address);
void load_line(unsigned tag, unsigned setIndex);
void usage();

int main(int argc, char **argv)
{
    process_input(argc, argv);

    // initialize and allocate cache
    cache = make_cache(s, e, b);

    // read file
    FILE *file;
    char *line = NULL;
    
    size_t len = 0;
    ssize_t read;
    char instr;
    unsigned long long int address;
    int size;

    file = fopen(t, "r");
    if (file) {
        while ((read = getline(&line, &len, file)) != -1){
            if (line[0] != ' ') {
                // instruction load
                // don't do anything
                continue;
            }

            memmove(line, line+1, strlen(line));
            char *line2 = line;

            if (verbose)
                printf("%s ", strtok(line2, "\n"));

            if ((sscanf(line, " %c %llx,%d", &instr, &address, &size)) == 3) {
                if (instr == 'L') {
                    check_hit(address);
                } else if (instr == 'S') {
                    check_hit(address);
                } else if (instr == 'M') {
                    check_hit(address);
                    check_hit(address);
                }

                if (verbose)
                    printf("\n");
            }

            lineNum++;
        }
        fclose(file);
    }

    if (line)
        free(line);

    // print summary
    printSummary(hit_count, miss_count, eviction_count);

    free_cache(cache);
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

        for (int j = 0; j < cache->sets[i].num_lines; j++) {
            cache->sets[i].lines[j].valid = 0;
            cache->sets[i].lines[j].lru = 0;
        }
    }

    return cache;
}

void free_cache(cache_t *cache) {
    for (int i = 0; i < cache->num_sets; i++) {
        free(cache->sets[i].lines);
    }

    free(cache->sets);
    free(cache);
}

int check_hit(unsigned long long int address) {
    int tagSize = 64 - (s + b);
    unsigned tag = address >> (s + b);
    unsigned setIndex = address << tagSize >> (tagSize + b);

    cacheset_t *set =  &(cache->sets[setIndex]);

    for (int i = 0; i < set->num_lines; i++) {
        line_t *line = &(set->lines[i]);
        if (line->valid == 1 && line->tag == tag) {
            // hit
            if (verbose)
                printf("hit ");

            hit_count++;
            line->lru = lineNum;

            printf("tag %u, set %u ", tag, setIndex);

            return 1;
        }
    }

    // missed
    if (verbose)
        printf("miss ");

    miss_count++;
    load_line(tag, setIndex);

    printf("tag %u, set %u", tag, setIndex);
    return 0;
}

void load_line(unsigned tag, unsigned setIndex) {
    cacheset_t *set = &(cache->sets[setIndex]);
    int min = 0;

    for (int i = 0; i < set->num_lines; i++) {
        if (set->lines[i].lru < set->lines[min].lru)
        min = i;
    }

    if (set->lines[min].valid) {
        // evicted
        if (verbose)
            printf("eviction ");
        eviction_count++;
    }

    set->lines[min].tag = tag;
    set->lines[min].valid = 1;
    set->lines[min].lru = lineNum;
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
