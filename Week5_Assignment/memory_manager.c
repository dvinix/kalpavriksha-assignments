#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define MIN_POOL_SIZE    1L
#define MAX_POOL_SIZE    1000000L
#define INITIAL_CAPACITY 8
#define INPUT_BUF_LEN    128

typedef enum { BLOCK_FREE = 0, BLOCK_ALLOCATED = 1 } BlockStatus;
typedef enum { FIRST_FIT = 1, BEST_FIT = 2, WORST_FIT = 3 } Strategy;

typedef struct {
    size_t      start;
    size_t      size;
    int         id;
    BlockStatus status;
} Block;

typedef struct {
    unsigned char *pool;
    size_t         pool_size;
    Block         *blocks;
    size_t         count;
    size_t         capacity;
    int            next_id;
    Strategy       strategy;
} MemoryManager;

static const char *strategy_name(Strategy s)
{
    switch (s) {
        case FIRST_FIT: return "First Fit";
        case BEST_FIT:  return "Best Fit";
        default:        return "Worst Fit";
    }
}

static unsigned char pattern_for(int id)
{
    return (unsigned char)('A' + (id - 1) % 26);
}

static void fill_pattern(MemoryManager *mm, size_t from, size_t len, int id)
{
    memset(mm->pool + from, pattern_for(id), len);
}

static int ensure_capacity(MemoryManager *mm, size_t needed)
{
    if (needed <= mm->capacity)
        return 1;

    size_t new_cap = mm->capacity ? mm->capacity : INITIAL_CAPACITY;
    while (new_cap < needed)
        new_cap *= 2;

    Block *tmp = realloc(mm->blocks, new_cap * sizeof(Block));
    if (tmp == NULL)
        return 0;             

    mm->blocks = tmp;
    mm->capacity = new_cap;
    return 1;
}

static void insert_block(MemoryManager *mm, size_t index, Block b)
{
    memmove(&mm->blocks[index + 1], &mm->blocks[index],
            (mm->count - index) * sizeof(Block));
    mm->blocks[index] = b;
    mm->count++;
}

static void remove_block(MemoryManager *mm, size_t index)
{
    memmove(&mm->blocks[index], &mm->blocks[index + 1],
            (mm->count - index - 1) * sizeof(Block));
    mm->count--;
}

static int find_block_by_id(const MemoryManager *mm, int id)
{
    for (size_t i = 0; i < mm->count; i++)
        if (mm->blocks[i].status == BLOCK_ALLOCATED && mm->blocks[i].id == id)
            return (int)i;
    return -1;
}

static int first_fit(const MemoryManager *mm, size_t size)
{
    for (size_t i = 0; i < mm->count; i++)
        if (mm->blocks[i].status == BLOCK_FREE && mm->blocks[i].size >= size)
            return (int)i;
    return -1;
}

static int best_fit(const MemoryManager *mm, size_t size)
{
    int best = -1;
    for (size_t i = 0; i < mm->count; i++) {
        if (mm->blocks[i].status == BLOCK_FREE && mm->blocks[i].size >= size) {
            if (best < 0 || mm->blocks[i].size < mm->blocks[best].size)
                best = (int)i;
        }
    }
    return best;
}

static int worst_fit(const MemoryManager *mm, size_t size)
{
    int worst = -1;
    for (size_t i = 0; i < mm->count; i++) {
        if (mm->blocks[i].status == BLOCK_FREE && mm->blocks[i].size >= size) {
            if (worst < 0 || mm->blocks[i].size > mm->blocks[worst].size)
                worst = (int)i;
        }
    }
    return worst;
}

static int find_free_block(const MemoryManager *mm, size_t size)
{
    switch (mm->strategy) {
        case FIRST_FIT: return first_fit(mm, size);
        case BEST_FIT:  return best_fit(mm, size);
        default:        return worst_fit(mm, size);
    }
}

static void split_and_assign(MemoryManager *mm, size_t idx, size_t size, int id)
{
    if (mm->blocks[idx].size > size) {
        Block rest;
        rest.start  = mm->blocks[idx].start + size;
        rest.size   = mm->blocks[idx].size - size;
        rest.id     = 0;
        rest.status = BLOCK_FREE;
        insert_block(mm, idx + 1, rest);
        mm->blocks[idx].size = size;
    }
    mm->blocks[idx].id     = id;
    mm->blocks[idx].status = BLOCK_ALLOCATED;
}

static void merge_free_blocks(MemoryManager *mm)
{
    size_t w = 0;
    for (size_t r = 0; r < mm->count; r++) {
        if (w > 0 && mm->blocks[w - 1].status == BLOCK_FREE &&
            mm->blocks[r].status == BLOCK_FREE) {
            mm->blocks[w - 1].size += mm->blocks[r].size;
        } else {
            if (w != r)
                mm->blocks[w] = mm->blocks[r];
            w++;
        }
    }
    mm->count = w;
}

typedef struct {
    size_t allocated_bytes, free_bytes;
    size_t allocated_blocks, free_blocks;
    size_t largest_free;
} Stats;

static Stats compute_stats(const MemoryManager *mm)
{
    Stats s = {0, 0, 0, 0, 0};
    for (size_t i = 0; i < mm->count; i++) {
        const Block *b = &mm->blocks[i];
        if (b->status == BLOCK_ALLOCATED) {
            s.allocated_bytes += b->size;
            s.allocated_blocks++;
        } else {
            s.free_bytes += b->size;
            s.free_blocks++;
            if (b->size > s.largest_free)
                s.largest_free = b->size;
        }
    }
    return s;
}

static void display_stats(const MemoryManager *mm)
{
    Stats s = compute_stats(mm);
    printf("Total memory: %zu bytes\n", mm->pool_size);
    printf("Allocated: %zu bytes\n", s.allocated_bytes);
    printf("Free: %zu bytes\n", s.free_bytes);
    printf("Allocated blocks: %zu\n", s.allocated_blocks);
    printf("Free blocks: %zu\n", s.free_blocks);
    printf("Largest free block: %zu bytes\n", s.largest_free);
}

static void display_map(const MemoryManager *mm)
{
    printf("Memory Map (%zu bytes):\n", mm->pool_size);
    for (size_t i = 0; i < mm->count; i++) {
        const Block *b = &mm->blocks[i];
        if (b->status == BLOCK_ALLOCATED)
            printf("[Allocated #%d: %zu bytes, Start: %zu]\n",
                   b->id, b->size, b->start);
        else
            printf("[Free: %zu bytes, Start: %zu]\n", b->size, b->start);
    }
}

static void report_failure_reason(const MemoryManager *mm, size_t size)
{
    Stats s = compute_stats(mm);

    if (s.free_bytes >= size) {
        printf("Note: External fragmentation detected. The request for %zu "
               "bytes cannot be satisfied even though %zu bytes are free in "
               "total.\n", size, s.free_bytes);
        printf("      Free blocks are split into: ");
        int first = 1;
        for (size_t i = 0; i < mm->count; i++) {
            if (mm->blocks[i].status == BLOCK_FREE) {
                printf("%s%zu", first ? "" : " + ", mm->blocks[i].size);
                first = 0;
            }
        }
        printf(" = %zu bytes, but the largest contiguous free block is only "
               "%zu bytes.\n", s.free_bytes, s.largest_free);
    } else {
        printf("Note: Not enough free memory (%zu bytes free, %zu requested).\n",
               s.free_bytes, size);
    }
}

static int check_consistency(const MemoryManager *mm)
{
    size_t expected_start = 0;
    for (size_t i = 0; i < mm->count; i++) {
        const Block *b = &mm->blocks[i];
        if (b->start != expected_start || b->size == 0)
            return 0;
        if (b->status == BLOCK_FREE && b->id != 0)
            return 0;
        if (b->status == BLOCK_ALLOCATED && b->id <= 0)
            return 0;
        if (i > 0 && b->status == BLOCK_FREE &&
            mm->blocks[i - 1].status == BLOCK_FREE)
            return 0;                     
        if (b->status == BLOCK_ALLOCATED) {
            unsigned char p = pattern_for(b->id);
            for (size_t k = 0; k < b->size; k++)
                if (mm->pool[b->start + k] != p)
                    return 0;               
        }
        expected_start += b->size;
    }
    return expected_start == mm->pool_size;
}

static void verify(const MemoryManager *mm)
{
    if (!check_consistency(mm))
        fprintf(stderr, "INTERNAL ERROR: memory map is inconsistent!\n");
}

static void allocate_memory(MemoryManager *mm, size_t size)
{
    if (mm->next_id == INT_MAX) {
        printf("Allocation failed: no more allocation IDs available.\n");
        return;
    }
    if (!ensure_capacity(mm, mm->count + 1)) {
        printf("Allocation failed: could not grow block metadata.\n");
        return;
    }

    int idx = find_free_block(mm, size);
    if (idx < 0) {
        printf("Allocation failed: no contiguous free block is large enough.\n");
        report_failure_reason(mm, size);
        return;
    }

    int id = mm->next_id++;
    split_and_assign(mm, (size_t)idx, size, id);
    fill_pattern(mm, mm->blocks[idx].start, size, id);
    printf("Allocation successful. ID: %d, Start: %zu, Size: %zu\n",
           id, mm->blocks[idx].start, mm->blocks[idx].size);
}

static void release_memory(MemoryManager *mm, int id)
{
    int idx = find_block_by_id(mm, id);
    if (idx < 0) {
        if (id > 0 && id < mm->next_id)
            printf("Release failed: allocation #%d was already released.\n", id);
        else
            printf("Release failed: unknown allocation ID %d.\n", id);
        return;
    }

    mm->blocks[idx].status = BLOCK_FREE;
    mm->blocks[idx].id = 0;
    merge_free_blocks(mm);
    printf("Allocation released.\n");
}

static void resize_allocation(MemoryManager *mm, int id, size_t new_size)
{
    int found = find_block_by_id(mm, id);
    if (found < 0) {
        printf("Resize failed: unknown allocation ID %d.\n", id);
        return;
    }
    size_t idx = (size_t)found;
    size_t old_size = mm->blocks[idx].size;

    if (new_size == old_size) {
        printf("Allocation #%d is already %zu bytes. Nothing to do.\n",
               id, old_size);
        return;
    }

    if (new_size < old_size) {
        size_t diff = old_size - new_size;
        int next_free = (idx + 1 < mm->count &&
                         mm->blocks[idx + 1].status == BLOCK_FREE);

        if (next_free) {
            mm->blocks[idx + 1].start -= diff;
            mm->blocks[idx + 1].size  += diff;
        } else {
            if (!ensure_capacity(mm, mm->count + 1)) {
                printf("Resize failed: could not grow block metadata. "
                       "Original allocation unchanged.\n");
                return;
            }
            Block rest;
            rest.start  = mm->blocks[idx].start + new_size;
            rest.size   = diff;
            rest.id     = 0;
            rest.status = BLOCK_FREE;
            insert_block(mm, idx + 1, rest);
        }
        mm->blocks[idx].size = new_size;
        merge_free_blocks(mm);
        printf("Allocation #%d shrunk from %zu to %zu bytes (in place).\n",
               id, old_size, new_size);
        return;
    }

    size_t extra = new_size - old_size;
    if (idx + 1 < mm->count &&
        mm->blocks[idx + 1].status == BLOCK_FREE &&
        mm->blocks[idx + 1].size >= extra) {

        size_t old_end = mm->blocks[idx].start + old_size;
        mm->blocks[idx + 1].start += extra;
        mm->blocks[idx + 1].size  -= extra;
        if (mm->blocks[idx + 1].size == 0)
            remove_block(mm, idx + 1);
        mm->blocks[idx].size = new_size;
        fill_pattern(mm, old_end, extra, id);
        printf("Allocation #%d grown from %zu to %zu bytes (in place, "
               "Start: %zu).\n", id, old_size, new_size, mm->blocks[idx].start);
        return;
    }

    if (!ensure_capacity(mm, mm->count + 1)) {
        printf("Resize failed: could not grow block metadata. "
                       "Original allocation unchanged.\n");
        return;
    }

    int target = find_free_block(mm, new_size);
    if (target < 0) {
        printf("Resize failed: cannot expand in place and no free block of "
               "%zu bytes exists. Original allocation unchanged.\n", new_size);
        report_failure_reason(mm, new_size);
        return;
    }

    size_t old_start = mm->blocks[idx].start;
    split_and_assign(mm, (size_t)target, new_size, id);   

    size_t new_start = 0;
    size_t old_idx = 0;
    for (size_t i = 0; i < mm->count; i++) {
        if (mm->blocks[i].start == old_start)
            old_idx = i;
    }
    new_start = mm->blocks[(size_t)target].start;

    memcpy(mm->pool + new_start, mm->pool + old_start, old_size);
    fill_pattern(mm, new_start + old_size, extra, id);

    mm->blocks[old_idx].status = BLOCK_FREE;
    mm->blocks[old_idx].id = 0;
    merge_free_blocks(mm);

    printf("Allocation #%d moved and grown from %zu to %zu bytes "
           "(old Start: %zu, new Start: %zu).\n",
           id, old_size, new_size, old_start, new_start);
}

static int read_long(const char *prompt, long *out)
{
    char buf[INPUT_BUF_LEN];

    for (;;) {
        printf("%s", prompt);
        fflush(stdout);
        if (fgets(buf, sizeof buf, stdin) == NULL)
            return 0;

        if (strchr(buf, '\n') == NULL) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        char *end;
        errno = 0;
        long v = strtol(buf, &end, 10);
        if (end == buf || errno == ERANGE) {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
            end++;
        if (*end != '\0') {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        *out = v;
        return 1;
    }
}

static int choose_strategy(MemoryManager *mm)
{
    long choice;
    for (;;) {
        if (!read_long("Select allocation strategy "
                       "(1. First Fit, 2. Best Fit, 3. Worst Fit): ", &choice))
            return 0;
        if (choice >= FIRST_FIT && choice <= WORST_FIT) {
            mm->strategy = (Strategy)choice;
            printf("Strategy set to %s.\n", strategy_name(mm->strategy));
            return 1;
        }
        printf("Invalid strategy. Enter 1, 2, or 3.\n");
    }
}

static void print_menu(void)
{
    printf("\nMenu:\n"
           "1. Allocate memory\n"
           "2. Release memory\n"
           "3. Resize allocation\n"
           "4. Display memory map\n"
           "5. Display memory statistics\n"
           "6. Change allocation strategy\n"
           "7. Exit\n");
}

static int init_manager(MemoryManager *mm, size_t pool_size)
{
    mm->pool      = NULL;
    mm->blocks    = NULL;
    mm->pool_size = pool_size;
    mm->count     = 0;
    mm->capacity  = 0;
    mm->next_id   = 1;
    mm->strategy  = FIRST_FIT;

    mm->pool = malloc(pool_size);
    if (mm->pool == NULL)
        return 0;
    memset(mm->pool, 0, pool_size);

    mm->blocks = malloc(INITIAL_CAPACITY * sizeof(Block));
    if (mm->blocks == NULL) {
        free(mm->pool);
        mm->pool = NULL;
        return 0;
    }
    mm->capacity = INITIAL_CAPACITY;

    mm->blocks[0].start  = 0;
    mm->blocks[0].size   = pool_size;
    mm->blocks[0].id     = 0;
    mm->blocks[0].status = BLOCK_FREE;
    mm->count = 1;
    return 1;
}

static void destroy_manager(MemoryManager *mm)
{
    free(mm->pool);
    free(mm->blocks);
    mm->pool = NULL;
    mm->blocks = NULL;
    mm->count = mm->capacity = 0;
}

int main(void)
{
    MemoryManager mm;
    long value;

    for (;;) {
        if (!read_long("Enter memory pool size: ", &value))
            return 0;
        if (value < MIN_POOL_SIZE || value > MAX_POOL_SIZE) {
            printf("Invalid pool size. Must be between %ld and %ld bytes.\n",
                   MIN_POOL_SIZE, MAX_POOL_SIZE);
            continue;
        }
        if (!init_manager(&mm, (size_t)value)) {
            printf("Error: failed to allocate memory for the pool.\n");
            return 1;
        }
        break;
    }

    if (!choose_strategy(&mm)) {
        destroy_manager(&mm);
        return 0;
    }

    for (;;) {
        print_menu();
        if (!read_long("Choice: ", &value))
            break;

        if (value == 7) {
            printf("Exiting. Goodbye!\n");
            break;
        }

        switch (value) {
        case 1: {
            long size;
            if (!read_long("Requested size: ", &size)) goto done;
            if (size <= 0 || (size_t)size > mm.pool_size) {
                printf("Invalid size. Must be between 1 and %zu bytes.\n",
                       mm.pool_size);
                break;
            }
            allocate_memory(&mm, (size_t)size);
            verify(&mm);
            break;
        }
        case 2: {
            long id;
            if (!read_long("Allocation ID: ", &id)) goto done;
            if (id <= 0 || id > INT_MAX) {
                printf("Invalid allocation ID. IDs are positive integers.\n");
                break;
            }
            release_memory(&mm, (int)id);
            verify(&mm);
            break;
        }
        case 3: {
            long id, size;
            if (!read_long("Allocation ID: ", &id)) goto done;
            if (id <= 0 || id > INT_MAX) {
                printf("Invalid allocation ID. IDs are positive integers.\n");
                break;
            }
            if (!read_long("New size: ", &size)) goto done;
            if (size <= 0 || (size_t)size > mm.pool_size) {
                printf("Invalid size. Must be between 1 and %zu bytes.\n",
                       mm.pool_size);
                break;
            }
            resize_allocation(&mm, (int)id, (size_t)size);
            verify(&mm);
            break;
        }
        case 4:
            display_map(&mm);
            break;
        case 5:
            display_stats(&mm);
            break;
        case 6:
            if (!choose_strategy(&mm)) goto done;
            break;
        default:
            printf("Invalid choice. Enter a number from 1 to 7.\n");
        }
    }

done:
    destroy_manager(&mm);
    return 0;
}