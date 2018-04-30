/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <limits.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
#define HEADER_SIZE ALIGN(sizeof(header_t))
#define STRIPPED_HEADER ALIGN(sizeof(size_t))
#define FOOTER_SIZE ALIGN(sizeof(footer_t))
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

typedef struct header header_t;
typedef struct footer footer_t;
header_t * find_fit(size_t size);

struct header{
   size_t size;
   header_t *next;
   header_t *prev;
};
struct footer{
   header_t *head;
};

int freecount = 0;

#define NUM_BINS 10
struct header heads[NUM_BINS];
//int cutoff[NUM_BINS]={16,64,112,128,160,1620,4072,4095,8190,INT_MAX};
int cutoff[NUM_BINS] = {17,65,113,129,449,1621,4073,4096,8191,INT_MAX};

int getIndex(size_t size){
   int index;
   for(index = 0; index < NUM_BINS; ++index){
      if(size < cutoff[index])
         break;
   }
   return index;
}

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
   freecount = 0;
   header_t *p = mem_sbrk(HEADER_SIZE + FOOTER_SIZE);
   p->next = p;
   p->prev = p;
   p->size = HEADER_SIZE + FOOTER_SIZE+1;
   footer_t *q = (footer_t *)((char *)p + HEADER_SIZE);
   q->head = p;
   int i;
   for(i = 0; i < NUM_BINS;++i){
      heads[i].next = &heads[i];
      heads[i].prev = &heads[i];
      heads[i].size = HEADER_SIZE + FOOTER_SIZE + 1;
   }

   return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
   size_t newsize = ALIGN(size + STRIPPED_HEADER + FOOTER_SIZE);
   header_t *p;
   if(freecount > 0 && (p = find_fit(newsize)) != NULL){
      if(((p->size) & ~1) - newsize >= 7 + HEADER_SIZE + FOOTER_SIZE){
         //split off extra, put it on end
         size_t oldsize = p->size;
         p->size = newsize;
         ((footer_t *)((char *)p + ((p->size) & ~1) - FOOTER_SIZE))->head = p;
         header_t *split = (header_t *)((char *)p + (p->size & ~1));
         split->size = oldsize - p->size; 
         ((footer_t *)((char *)split + ((split->size) & ~1) - FOOTER_SIZE))->head = split;
         int index = getIndex(split->size - STRIPPED_HEADER - FOOTER_SIZE);
         split->next = heads[index].next;
         heads[index].next = split;
         split->next->prev = split;
         split->prev = &heads[index];
         ++freecount;
      }
      if(freecount>0){
         --freecount;
      }
      p->prev->next = p->next;
      p->next->prev = p->prev;
      p->size = p->size | 1;
   }else{
      header_t *last=(((footer_t *)((char *)mem_heap_hi() + 1 - FOOTER_SIZE))->head);
      if((freecount > 0)&&(!(last->size & 1))){
         --freecount;
         mem_sbrk(newsize - last->size);
         last->size = newsize | 1;
         ((footer_t *)((char *)last + ((last->size) & ~1) - FOOTER_SIZE))->head = last;
         last->prev->next = last->next;
         last->next->prev = last->prev;
         p = last;
      } else {
         p = mem_sbrk(newsize);
         if ((long)p == -1){
            return NULL;
         }
         else {
            p->size = newsize | 1;
            footer_t *q = (footer_t *)((char *)p + ((p->size) & ~1) - FOOTER_SIZE);
            q->head = p;
         }
      }
   }

   return (void *)((char *)p + sizeof(size_t));
}


header_t *find_fit(size_t size){
   int index = getIndex(size);
   header_t *p ;
   for(; index < NUM_BINS; ++index){
      for(p = heads[index].next; (p != &heads[index]) && (p->size < size + STRIPPED_HEADER + FOOTER_SIZE); p = p->next);
      if(p != &heads[index])
         return p;
   }
   return NULL;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr){
   ++freecount;
   //Get the header of the current block
   header_t *p = (header_t *)((char *)ptr - STRIPPED_HEADER);
   //back up and get the previous block's header
   header_t *prev = ((footer_t *)((char *)p - FOOTER_SIZE))->head;
   //set the current block to free
   p->size &= ~1;
   //if the previous block is also free
   if(!((prev->size) & 1)){
	   //link the list around the current block
      prev->prev->next = prev->next;
      prev->next->prev = prev->prev;
      prev->size += p->size;
      ((footer_t *)((char *)p + ((p->size) & ~1) - FOOTER_SIZE))->head = prev;
      p = prev;
      --freecount;
   }
   header_t *next=(header_t *)((char *)p + (p->size &= ~1));
   if(((void *)((char *)p + p->size) <= mem_heap_hi()) && !((next->size) & 1)){
      p->size += next->size;
      ((footer_t *)((char *)p + ((p->size) & ~1) - FOOTER_SIZE))->head = p;
      next->prev->next = next->next;
      next->next->prev = next->prev;
      --freecount;
   }
   header_t * front = &heads[getIndex(p->size-STRIPPED_HEADER-FOOTER_SIZE)];
   p->prev = front;
   p->next = front->next;
   front->next->prev = p;
   front->next = p;
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
   size_t newsize = ALIGN(size + STRIPPED_HEADER + FOOTER_SIZE);
   header_t *p = (header_t *)((char *)ptr - sizeof(size_t));
   header_t *next=(header_t *)((char *)p + (p->size &= ~1));
   if((p->size & ~1)>newsize){
      return ptr;
   }
   if(((void *)((char *)p + p->size) <= mem_heap_hi()) && !((next->size) & 1) && (next->size + (p->size & ~1) > newsize)){
      p->size = (p->size + next->size) | 1;
      ((footer_t *)((char *)p + ((p->size) & ~1) - FOOTER_SIZE))->head = p;
      next->prev->next = next->next;
      next->next->prev = next->prev;

      return ptr;
   }
   if(((void *)((char *)p + p->size) >= mem_heap_hi())){
      mem_sbrk(newsize - p->size);
      p->size = newsize;
      ((footer_t *)((char *)p + ((p->size) & ~1) - FOOTER_SIZE))->head = p;
      return ptr;
   }
   void * new = mm_malloc(size);
   memcpy(new, ptr, p->size - STRIPPED_HEADER - FOOTER_SIZE);
   mm_free(ptr);
   return new;
}