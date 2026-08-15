#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<stdint.h>
#include<string.h>
#include<stdbool.h>

#define PUSH_STRUCT(a,T) ((T *)arena_push(a,sizeof(T),_Alignof(T)))
#define ARENA_PUSH_ARRAY(a,T,n) ((T *)arena_push_array(a,sizeof(T),n,_Alignof(T)))
#define PUSH_STRUCT_ZERO(a,T) ((T *)arena_push_zero(a,sizeof(T),_Alignof(T)))

typedef struct{
unsigned char *buffer; // this is pointer who store the add of memory
size_t capacity; // this how big arena you want to create
size_t pos; // this is for position like a allocate kiya 50byte ke to pos to change kr ke 50 kr denge and go on.
}arena;

typedef struct{
arena *temp_arena; 
size_t temp_pos;
}arena_temp;

bool is_pow2(size_t x){
if((x&(x-1))==0 && x!=0){
return true;
}
return false;
}
arena *arena_create(size_t capacity){
arena *ptr = malloc(sizeof(arena));
if(ptr == NULL) return NULL;
ptr->capacity = capacity;
ptr->buffer = malloc(capacity);
if(ptr->buffer == NULL) {
free(ptr);
return NULL;
}
ptr->pos = 0;
return ptr;
}
size_t arena_get_pos(arena *a){
return a->pos;
}
void *arena_push(arena *a,size_t size,size_t alignment){
if(is_pow2(alignment)==0){
return NULL;
}
size_t rem = (a->pos)%alignment;
size_t padding = 0;
if(rem!=0){
padding= alignment - rem;
}
if(padding>a->capacity-a->pos){
return NULL;
}
size_t alig_p = a->pos + padding;
if(size> a->capacity - alig_p){
return NULL; // arena full
}
size_t new_p = alig_p + size;
a->pos = new_p;
return &a->buffer[alig_p];
}
void arena_destroy(arena *a){
free(a->buffer);
free(a);
}
void arena_pop(arena *a,size_t size){
if(size>a->pos){
a->pos = 0;
}else{
size_t new_p = a->pos-size;
a->pos = new_p;
}
}
void arena_pop_to(arena *a,size_t req_pos){
if(req_pos<a->pos && a->pos<=a->capacity){
a->pos = req_pos;
}
}
void arena_clear(arena *a){
a->pos = 0;
}

void *arena_push_array(arena *a,size_t element_size,size_t count,size_t alignment){
if(element_size==0||count>(SIZE_MAX/element_size)){
return NULL;
}
size_t total_size = element_size*count;
return arena_push(a,total_size,alignment);
}

arena_temp arena_temp_begin(arena *a){
arena_temp temp;
temp.temp_arena = a;
temp.temp_pos = arena_get_pos(a);
return temp;
}

void arena_temp_end(arena_temp *temp){
arena_pop_to(temp->temp_arena,temp->temp_pos);
}
void *arena_push_zero(arena *a,size_t size,size_t alignment){
void *ptr = arena_push(a,size,alignment);
if(ptr==NULL){
return NULL;
}
memset(ptr,0,size);
return ptr;
}
int main(){
printf("hello arena\n");
arena *a = arena_create(100);
if(a==NULL) {
printf("creating arena return null\n");
 return 0;
}
//void *first = arena_push(a,20,_Alignof(int));
int *first  = PUSH_STRUCT(a,int); 
char *c = arena_push(a,1,1);
size_t mark = arena_get_pos(a);
double *d = PUSH_STRUCT(a,double);
printf("mark=%zu\n",mark);
printf("pos after double %zu\n",a->pos);
arena_pop_to(a,mark);
printf("pos after the rollback %zu\n",a->pos);
int *temp1  = PUSH_STRUCT(a,int);
int *temp2  = PUSH_STRUCT(a,int);
size_t mark2 = arena_get_pos(a);
printf("the pos of mark is %zu and the after mark2 %zu\n",mark,mark2);
arena_temp temp = arena_temp_begin(a);
int *t1 = PUSH_STRUCT(a,int);
//int *t2 = PUSH_ARRAY(a,int,10);
printf("before temp is %zu\n",mark2);
printf("inside the temp %zu\n",arena_get_pos(a));
arena_temp_end(&temp);
printf("after temp end is %zu\n",arena_get_pos(a));
int *zeo = PUSH_STRUCT_ZERO(a,int);
printf("zero value %d\n",*zeo);
*zeo = 42;
printf("after write zeo %d\n",*zeo);
int *numbers = ARENA_PUSH_ARRAY(a,int,5);
if(numbers==NULL){
return 0;
}
numbers[0] = 1;
numbers[1] = 2;
numbers[2] = 3;
numbers[3] = 4;
numbers[4] = 4;
for(int i=0;i<5;i++){
printf("%d ",numbers[i]);
}
if(first==NULL){
printf("arena to push hi nhi hua\n");
return 0;
}
printf("the capacity after allocate%zu\n",a->capacity); // %zu is for size_t 
printf("the new position of pos is %zu and the first addr is %p\n",a->pos,first);
//int *x = arena_push(a,sizeof(int),_Alignof(int));
int *x = PUSH_STRUCT(a,int);
if(x==NULL){
printf("second push fail");
return 0;
}
*x = 42; // store value in it
printf("the x is %d\n",*x);
printf("the new2 postiton of pos is %zu and the x addr is %p\n",a->pos,(void *)x);
//arena_push(a,6,_Alignof(int));
int *y = PUSH_STRUCT(a,int);
printf("thire 6 %zu\n",a->pos);
//arena_pop(a,6);
printf("check pos %zu\n",a->pos);
size_t mark3 = arena_get_pos(a);
printf("the pos at last is %zu\n",mark3);
arena_pop_to(a,mark2);
printf("after the arena_pop_to %zu\n",a->pos);
arena_clear(a);
arena_destroy(a);
return 0;
}


