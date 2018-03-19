#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "cachelab.h"

typedef struct block block_t;
typedef struct line line_t;
typedef struct cacheset cacheset_t;
typedef struct cache cache_t;

cache_t *cache;

struct line {
    int index;
    int valid;
    int tag;
};

struct cacheset{
    int num_lines;
    line_t *lines;
};

struct cache {
    int num_sets;
    cacheset_t *sets;
};

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
    // allocate cache
    cache_t *cache = malloc(sizeof(cache_t));

    // allocate cache sets
    cache->sets = malloc(sizeof(cacheset_t *) * pow(2, s));
    
    for (int i = 0; i < pow(2, s); i++) {
        // allocate lines
        cacheset_t *set = cache->sets;
        set->lines = malloc(sizeof(line_t *) * e);
    }

    return cache;
}