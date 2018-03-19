#include <stdio.h>
#include <stdlib.h>
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

cache_t *cache;

cache_t *make_cache(int s, int e, int b);

int main(int argc, char **argv)
{
    // read input, parse

    // initialize and allocate cache
    cache = make_cache(1, 1, 1);

    printSummary(0, 0, 0);
    return 0;
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