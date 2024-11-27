#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_malloc()
{
    printf("Testing malloc allocation\n");

    size_t size = 1024;
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "malloc failed for size %zu\n", size);
        exit(1);
    }

    memset(ptr, 0, size);
    printf("Allocated %zu bytes\n", size);

    free(ptr);
}

int main()
{
    printf("Starting memory tests\n");

    test_malloc();

    printf("Memory tests completed\n");
    return 0;
}
