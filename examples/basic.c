#include <dmm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void _panic(const char *message, const char *function,
            const char *filename, size_t line) {
    // Loosely based on Rust's panic message format.
    fprintf(stderr, "function '%s' panicked at '%s', %s:%zu\n", function, message, filename, line);
    exit(1);
}

int main(void) {
    dmm_init(&_panic);

    char *region1 = malloc(200);
    char *region2 = malloc(512);
    char *region3 = malloc(2048);

    dmm_add_memory_region(region1, 200);
    dmm_add_memory_region(region2, 512);
    dmm_add_memory_region(region3, 2048);

    char *a = dmm_malloc(9);   // can be from any region
    if (a == NULL)
        dmm_panic("a is NULL");
    char *b = dmm_malloc(100); // can be from region2 or region3
    if (b == NULL)
        dmm_panic("b is NULL");
    char *c = dmm_malloc(600); // can only be from region3.
    if (c == NULL)
        dmm_panic("c is NULL");

    strncpy(a, "hello", 6);
    strncpy(b, "world", 6);
    strncpy(c, "!", 2);

    printf("%s %s%s\n", a, b, c);

    // Clean up after ourselves.
    free(region1);
    free(region2);
    free(region3);

    return 0;
}
