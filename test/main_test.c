#include <tinker.h>
#include <stddef.h>
#include <dmm.h>
#include "../src/main.h"

// FIXME: dmm can't handle >634K chunk sizes for some reason.
#define ALLOCATE_THE_UNIVERSE_CHUNK_SIZE (634 * 1024) // 634K
//#define ALLOCATE_THE_UNIVERSE_CHUNK_SIZE (1024 * 1024) // 1M

void test_dmm_malloc(void)
{
    void *region = dmm_malloc(10);
    tinker_assert(region != NULL);

    DMM_MallocHeader *header = ((DMM_MallocHeader*)region) - 1;

    tinker_assert(header->size == 10);
    tinker_assert(header->used == 1);
    tinker_assert(header->data == region);
}

void test_dmm_free_sets_header(void)
{
    void *region = dmm_malloc(10);
    tinker_assert(region != NULL);

    DMM_MallocHeader *header = ((DMM_MallocHeader*)region) - 1;

    tinker_assert(header->used == 1);

    dmm_free(region);

    tinker_assert(header->used == 0);
}

void test_dmm_allocate_the_universe(void)
{
    DMM_MallocHeader *header;
    size_t allocated_chunks = 0;

    // Allocate everything
    while (1) {
        void *region = dmm_malloc(ALLOCATE_THE_UNIVERSE_CHUNK_SIZE);
        if (region == NULL) {
            break;
        }
        allocated_chunks += 1;

        header = ((DMM_MallocHeader*)region) - 1;
        header->flags = DMM_HEADER_FLAG_TEST;
    }

    if (allocated_chunks == 0) {
        // If you encounter this error, you need to give DMM bigger chunks.
        // The chunks should be at least the ALLOCATE_THE_UNIVERSE_CHUNK_SIZE
        // bytes in length.
        tinker_fail("Allocated no chunks -- universe is too small.");
    }

    // Check that all the allocations with the test flag set are of the size
    // we expect them to be
    header = dmm_global_instance;
    while (1) {
        if (header == NULL) {
            break;
        }

        if ((header->flags & DMM_HEADER_FLAG_TEST) == 1) {
            if (header->size != (ALLOCATE_THE_UNIVERSE_CHUNK_SIZE - sizeof(DMM_MallocHeader))) {
                tinker_fail("An allocated test chunk was not of the expected size.");
            }
        }

        header = header->next;
    }

    // Free all the test allocations
    header = dmm_global_instance;
    while (1) {
        if (header == NULL) {
            break;
        }

        // Save the value of header->next in case the header disappears when we
        // free() the region (when DMM actually merges free regions)
        DMM_MallocHeader *next = header->next;
        if ((header->flags & DMM_HEADER_FLAG_TEST) != 0) {
            header->flags = 0;
            dmm_free(header->data);
        }

        header = next;
    }

    tinker_pass();
}
