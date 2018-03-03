#include <stdlib.h>
#include "math.h"
#include "cachelab.h"

typedef struct block block_t;
typedef struct line line_t;
typedef struct cacheset cacheset_t;
typedef struct cache cache_t;

struct block {
    int data;
};

struct line {
    int index;
    int valid;
    int tag;
    int num_blocks;
    block_t *blocks;
};

struct cacheset{
    int num_lines;
    line_t **lines;
};

struct cache {
    int num_sets;
    cacheset_t **sets;
};

cache_t *cache;

int main()
{
    printSummary(0, 0, 0);
    return 0;
}

cache_t *make_cache(int s, int e, int b) {
    // allocate cache
    cache_t *cache = malloc(sizeof(cache_t));

    // allocate cache sets
    cache->num_sets = pow(2, s);
    cache->sets = calloc(sizeof(cacheset_t), cache->num_sets);
    
    for (int i = 0; i < cache->num_sets; i++) {
        // allocate lines
        cacheset_t *set = cache->sets[i];
        set->num_lines = e;
        set->lines = calloc(sizeof(line_t), set->num_lines);

        for (int j = 0; j < set->num_lines; j++) {
            // allocate blocks
            line_t *line = set->lines[j];
            line->num_blocks = pow(2, b);
            line->blocks = calloc(sizeof(block_t), line->num_blocks);
        }
    }

    return cache;
}