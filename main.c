#include <stdio.h>
#include <stdlib.h>

// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

typedef struct C8_t
{
    int a;
    char *name;
} C8_t;

int main(void)
{

    // Stack allocated
    C8_t c8_stack = {1, "abc"};

    // Heap allocated
    C8_t *c8_heap = malloc(sizeof(C8_t));
    *c8_heap = (C8_t){
        .a = 0,
        .name = "abc"};

    char name[100];
    printf("enter name papi: ");
    scanf("%99s", name);

    c8_heap->name = name;

    printf("\nname (in the heap): %s", c8_heap->name);

    return 0;
}
