#include <dmm.h>
#include <dmm_instance.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *small = NULL;
static void *large = NULL;

void panic(const char *msg, const char *func, const char *filename, size_t line) {
    fprintf(stderr, "function '%s' panicked at '%s', %s:%zu\n", func, msg, filename, line);
    exit(1);
}

void do_something(void) {
    char *a = dmm_instance_malloc(small, 20);
    char *b = dmm_instance_malloc(large, 5 * 1024); // 5KB

    strncpy(a, "hello, world!", 14);
    memset(b, 'a', 5 * 1024);
    b[5 * 1024 - 1] = '\0';

    printf("a = '%s'\n", a);

    // Oh no, we "forgot" to free `a`.
    // But that's okay, because it'll get freed when we free() small_region in main().
    dmm_instance_free(large, b);
}

int main(void) {
    // Get memory chunks from system allocator.
    size_t small_region_size = 10 * 1024 * 1024; // 10MB
    void *small_region = malloc(small_region_size);

    size_t large_region_size = 100 * 1024 * 1024; // 100MB
    void *large_region = malloc(large_region_size);

    // Set up the panic function.
    dmm_init(&panic);

    // Set up memory regions.
    small = dmm_instance_add_memory_region(small, small_region, small_region_size);
    large = dmm_instance_add_memory_region(large, large_region, large_region_size);

    // Usage
    do_something();

    // Clean up everything.
    free(small_region);
    free(large_region);

    return 0;
}
