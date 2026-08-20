#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stddef.h>
#include<stdbool.h>
#include<unistd.h>
#include<sys/mman.h>
#include<string.h>

typedef struct{
unsigned char *buffer;
size_t reserve_size;
size_t commit_size;
size_t pos;
size_t commit_pos;
}arena;

typedef struct{
arena *arena;
size_t old_pos;
}temp_arena;

size_t get_pagesize(void);
void *memory_reserve(size_t size); // this is for reseving virtual address space of some size.
bool memory_commit(void *ptr,size_t size); // this make reserved virtual range readable and writable.
bool memory_release(void *ptr ,size_t size); // this is for releasing reserve memory.
arena *create_arena(size_t reservesize);
void destroy_arena(arena *a);
void *push_arena(arena *a,size_t size,size_t align);
bool is_power_of_two(size_t x);
void arena_clear(arena *a);
void arena_pop(arena *a,size_t old_pos);
temp_arena temp_arena_begin(arena *a);
void temp_arena_end(temp_arena *a);
void *push_arena_zero(arena *a,size_t size , size_t align);

int main(){
printf("hello my arena_vm\n");
size_t one_GiB = 1024*1024*1024;
arena *a = create_arena(one_GiB);
if(a==NULL){
printf("arena creation is faild");
return 0;
}
printf("reserve_size %zu\n",a->reserve_size);
printf("commit_size %zu\n",a->commit_size);
printf("pos is %zu\n",a->pos);
printf("commit_pos %zu\n",a->commit_pos);

//size_t b = a->commit_size + 100;
//void *x = push_arena(a,b,4);
//if(x!=NULL){
//printf("the big allocation succesful\n");
//printf("the new pos %zu\n",a->pos);
//printf("new commit_pos %zu\n",a->commit_pos);
//}else{
//printf("failed big size");
//}
//void *first = push_arena(a,1,1);
//printf("first allocation %p\n",first);
//printf("pos after first %zu\n",a->pos);
//size_t bs = a->pos;
//void *second = push_arena(a,8,8);
//printf("second allocation %p\n",second);
//printf("pos after the second %zu\n",a->pos);
//arena_pop(a,bs);
//printf("pos after pop %zu\n",a->pos);
//printf("second address mod 8 %zu\n",(size_t)second%8);
//printf("before clear pos %zu\n",a->pos);
//arena_clear(a);
//printf("after clear arena the pos become %zu\n",a->pos);
//printf("after clear arena commit_pos %zu\n",a->commit_pos);
//void *p = push_arena(a,100,8);
//printf("pos bfore temp arena is %zun",a->pos);
//temp_arena temp = temp_arena_begin(a);
//void *x = push_arena(a,200,8);
//void *y = push_arena(a,300,8);
//printf("pos inside temp = %zu\n",a->pos);
//temp_arena_end(&temp);
//printf("pos after temp %zu\n",a->pos);
//void *x = push_arena(a,16,8);
//memset(x,0xAA,16);
//arena_clear(a);
void *y = push_arena_zero(a,16,8);
unsigned char *b = y;
for(size_t i=0;i<16;i++){
printf("%02x ",b[i]);
}
printf("\n");
destroy_arena(a);
return 0;
}


bool is_power_of_two(size_t x){
return x!=0 && ((x&(x-1))==0);
}

size_t get_pagesize(void){
return (size_t)sysconf(_SC_PAGESIZE);
}
void *memory_reserve(size_t size){
void *ptr = mmap(NULL,size,PROT_NONE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);// PROT_NONE is flag for no read/write, MAP_PRIVATE is for private mapping for the process. mmap() resturn starting memory address of the maping.
if(ptr==MAP_FAILED){
return NULL;
}
return ptr;
}

bool memory_commit(void *ptr ,size_t size){
int mp = mprotect(ptr,size,PROT_READ|PROT_WRITE);
if(mp==-1){
return false;
}
return true;
}

bool memory_release(void *ptr ,size_t size){
// you can read this doc for munmap(): man7.org/man-pages/man3/munmap.3p.html.
int mr = munmap(ptr,size);
if(mr == -1){
return false;
}
return true;
}

arena *create_arena(size_t reservesize){
arena *a = malloc(sizeof(arena));
if(a==NULL){
return NULL;
}
size_t pagesize = get_pagesize();
a->buffer = memory_reserve(reservesize);
if(a->buffer==NULL){
free(a);
return NULL;
}
a->reserve_size = reservesize;
a->commit_size  = pagesize;
if(!memory_commit(a->buffer,a->commit_size)){
memory_release(a->buffer,a->reserve_size);
free(a);
return NULL;
}
a->pos = 0;
a->commit_pos = a->commit_size;
return a;
}
void destroy_arena(arena *a){
memory_release(a->buffer,a->reserve_size);
free(a);
}
void *push_arena(arena *a,size_t size,size_t align){
size_t old_pos = a->pos;
if(!is_power_of_two(align)){return NULL;}
if(align -1 > a->reserve_size - old_pos){return NULL;}
size_t aligned_pos = (old_pos+align-1)& ~(align-1);
if(aligned_pos > a->reserve_size){return NULL;}
if(size>a->reserve_size -aligned_pos){
return NULL;
}
size_t new_pos = aligned_pos + size;
if(new_pos<=a->commit_pos){
void *result = a->buffer + aligned_pos;
a->pos = new_pos;
return result;
}
size_t new_commit_pos = new_pos;
new_commit_pos += a->commit_size - 1;
new_commit_pos -= new_commit_pos % a->commit_size;
if(new_commit_pos>a->reserve_size){
return NULL;
}
size_t commit_amount = new_commit_pos - a->commit_pos;
if(!memory_commit(a->buffer+a->commit_pos,commit_amount)){
return NULL;
}
a->commit_pos = new_commit_pos;
void *result = a->buffer + aligned_pos;
a->pos = new_pos;
return result;
}
void arena_clear(arena *a){
a->pos = 0;
}
void arena_pop(arena *a,size_t old_pos){
if(old_pos>a->pos){return;}
a->pos =old_pos;
}
temp_arena temp_arena_begin(arena *a){
temp_arena temp;
temp.arena = a;
temp.old_pos = a->pos;
return temp;
}

void temp_arena_end(temp_arena *temp){
temp->arena->pos = temp->old_pos;
}
void *push_arena_zero(arena *a,size_t size,size_t align){
void *ptr = push_arena(a,size,align);
if(ptr==NULL){
return NULL;
}
memset(ptr,0,size);
return ptr;
}



