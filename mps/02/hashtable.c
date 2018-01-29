#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

/* Daniel J. Bernstein's "times 33" string hash function, from comp.lang.C;
   See https://groups.google.com/forum/#!topic/comp.lang.c/lSKWXiuNOAk */
unsigned long hash(char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash;
}

hashtable_t *make_hashtable(unsigned long size) {
  hashtable_t *ht = malloc(sizeof(hashtable_t));
  ht->size = size;
  ht->buckets = calloc(sizeof(bucket_t *), size);
  return ht;
}

void ht_put(hashtable_t *ht, char *key, void *val) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = malloc(sizeof(bucket_t));
  b->key = key;
  b->val = val;
  b->next = ht->buckets[idx];
  ht->buckets[idx] = b;

  // Look for existing item and remove it
  bucket_t *bprev = b;
  b = b->next;

  while (b) {
    if (strcmp(b->key, key) == 0) {
      bprev->next = b->next;
      free(b->key);
      free(b->val);
      free(b);
      return;
    }
    bprev = b;
    b = b->next;
  }
}

void *ht_get(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = ht->buckets[idx];
  while (b) {
    if (strcmp(b->key, key) == 0) {
      return b->val;
    }
    b = b->next;
  }
  return NULL;
}

void ht_iter(hashtable_t *ht, int (*f)(char *, void *)) {
  bucket_t *b;
  unsigned long i;
  for (i=0; i<ht->size; i++) {
    b = ht->buckets[i];
    while (b) {
      if (!f(b->key, b->val)) {
        return ; // abort iteration
      }
      b = b->next;
    }
  }
}

void free_hashtable(hashtable_t *ht) {
  bucket_t *tmp;
  bucket_t *b;

  unsigned long i;
  for (i=0; i<ht->size; i++) {
    b = ht->buckets[i];
    while (b) {
      tmp = b;
      b = b->next;
      free(tmp->key);
      free(tmp->val);
      free(tmp);
    }
  }

  free(ht->buckets);
  free(ht);
}

void  ht_del(hashtable_t *ht, char *key) {
  unsigned int idx = hash(key) % ht->size;
  bucket_t *b = ht->buckets[idx];

  // check if first item matches
  if (strcmp(b->key, key) == 0) {
    free(b->key);
    free(b->val);
    free(b);
    ht->buckets[idx] = b->next;
    return;
  }

  // check through linked list for correct item
  bucket_t *bprev = b;
  b = b->next;

  while (b) {
    if (strcmp(b->key, key) == 0) {
      // remove item
      bprev->next = b->next;
      free(b->key);
      free(b->val);
      free(b);
      return;
    }

    bprev = b;
    b = b->next;
  }
}

void  ht_rehash(hashtable_t *ht, unsigned long newsize) {
  // create new hashtable
  hashtable_t *new_ht = make_hashtable(newsize);

  // iterate through old hashtable and add values to new one
  bucket_t *b;
  unsigned long i;
  for (i=0; i<ht->size; i++) {
    b = ht->buckets[i];
    while (b) {
      ht_put(new_ht, b->key, b->val);
      b = b->next;
    }
  }

  // delete old hashtable
  bucket_t *tmp;

  for (i=0; i<ht->size; i++) {
    b = ht->buckets[i];
    while (b) {
      tmp = b;
      b = b->next;
      free(tmp);
    }
  }

  free(ht->buckets);
  free(ht);

  // pointer magic
  *ht = *new_ht;
  
  // free pointers
  free(new_ht);
}
