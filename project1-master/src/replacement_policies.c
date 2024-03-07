//
// This file contains all of the implementations of the replacement_policy
// constructors from the replacement_policies.h file.
//
// It also contains stubs of all of the functions that are added to each
// replacement_policy struct at construction time.
//
// ============================================================================
// NOTE: It is recommended that you read the comments in the
// replacement_policies.h file for further context on what each function is
// for.
// ============================================================================
//

#include "replacement_policies.h"

// LRU Replacement Policy
// ============================================================================
// TODO feel free to create additional structs/enums as necessary

struct lru_metadata {
    uint32_t *last_access_times;
    uint32_t access_counter;
};

void lru_cache_access(struct replacement_policy *replacement_policy,
                      struct cache_system *cache_system, uint32_t set_idx, uint32_t tag)
{
    // update the LRU replacement policy state given a new memory access
    struct lru_metadata *metadata = (struct lru_metadata *)replacement_policy->data;
    uint32_t set_base = set_idx * cache_system->associativity;
    uint32_t access_time = ++metadata->access_counter;

    for (uint32_t i = 0; i < cache_system->associativity; ++i) {
        struct cache_line *cl = &cache_system->cache_lines[set_base + i];
        if (cl->tag == tag && cl->status != INVALID) {
            metadata->last_access_times[set_base + i] = access_time;
            break;
        }
    }
}

uint32_t lru_eviction_index(struct replacement_policy *replacement_policy,
                            struct cache_system *cache_system, uint32_t set_idx)
{
    // return the index within the set that should be evicted.
    struct lru_metadata *metadata = (struct lru_metadata *)replacement_policy->data;
    uint32_t set_base = set_idx * cache_system->associativity;
    uint32_t least_recently_used = UINT32_MAX;
    uint32_t lru_index = 0;

    for (uint32_t i = 0; i < cache_system->associativity; ++i) {
        uint32_t access_time = metadata->last_access_times[set_base + i];
        if (access_time < least_recently_used) {
            least_recently_used = access_time;
            lru_index = i;
        }
    }

    return lru_index;

}

void lru_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // TODO cleanup any additional memory that you allocated in the
    // lru_replacement_policy_new function.
    struct lru_metadata *metadata = (struct lru_metadata *)replacement_policy->data;
    if (metadata) {
        free(metadata->last_access_times);
        free(metadata);
    }
}

struct replacement_policy *lru_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    struct replacement_policy *lru_rp = calloc(1, sizeof(struct replacement_policy));
    struct lru_metadata *metadata = calloc(1, sizeof(struct lru_metadata));

    metadata->last_access_times = calloc(sets*associativity, sizeof(uint32_t));
    metadata->access_counter = 0;

    lru_rp->cache_access = &lru_cache_access;
    lru_rp->eviction_index = &lru_eviction_index;
    lru_rp->cleanup = &lru_replacement_policy_cleanup;
    lru_rp->data = metadata;

    // allocate any additional memory to store metadata here and assign to
    // lru_rp->data.

    return lru_rp;
}

// RAND Replacement Policy
// ============================================================================
void rand_cache_access(struct replacement_policy *replacement_policy,
                       struct cache_system *cache_system, uint32_t set_idx, uint32_t tag)
{
    // update the RAND replacement policy state given a new memory access
}

uint32_t rand_eviction_index(struct replacement_policy *replacement_policy,
                             struct cache_system *cache_system, uint32_t set_idx)
{
    // return the index within the set that should be evicted.

    return rand() % cache_system->associativity;
}

void rand_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // cleanup any additional memory that you allocated in the
    // rand_replacement_policy_new function.
}

struct replacement_policy *rand_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    // Seed randomness
    srand(time(NULL));

    struct replacement_policy *rand_rp = malloc(sizeof(struct replacement_policy));
    rand_rp->cache_access = &rand_cache_access;
    rand_rp->eviction_index = &rand_eviction_index;
    rand_rp->cleanup = &rand_replacement_policy_cleanup;

    // allocate any additional memory to store metadata here and assign to
    // rand_rp->data.

    return rand_rp;
}

// LRU_PREFER_CLEAN Replacement Policy
// ============================================================================
struct lru_prefer_clean_metadata {
    uint32_t *last_access_times; 
    uint8_t *is_dirty;           
    uint32_t access_counter;     
};


void lru_prefer_clean_cache_access(struct replacement_policy *replacement_policy,
                                   struct cache_system *cache_system, uint32_t set_idx,
                                   uint32_t tag)
{
    struct lru_prefer_clean_metadata *metadata = (struct lru_prefer_clean_metadata *)replacement_policy->data;
    uint32_t set_base = set_idx * cache_system->associativity;
    metadata->access_counter++;
    bool line_modified = false;

    for (uint32_t i = 0; i < cache_system->associativity; ++i) {
        struct cache_line *line = &cache_system->cache_lines[set_base + i];
        if (line->tag == tag && line->status != INVALID) {
            // Update the last access time
            metadata->last_access_times[set_base + i] = metadata->access_counter;
            // If the cache line status is MODIFIED, it means it was a 'W' operation
            line_modified = (line->status == MODIFIED);
            break;
        }
    }

    // If a line was modified, update the is_dirty array
    if (line_modified) {
        for (uint32_t i = 0; i < cache_system->associativity; ++i) {
            struct cache_line *line = &cache_system->cache_lines[set_base + i];
            if (line->tag == tag && line->status == MODIFIED) {
                metadata->is_dirty[set_base + i] = 1; // Mark the line as dirty
                break;
            }
        }
    }
}


uint32_t lru_prefer_clean_eviction_index(struct replacement_policy *replacement_policy,
                                         struct cache_system *cache_system, uint32_t set_idx) {
    struct lru_prefer_clean_metadata *metadata = (struct lru_prefer_clean_metadata *)replacement_policy->data;
    uint32_t set_base = set_idx * cache_system->associativity;

    int clean_index = -1; // Index of the oldest clean line within the set
    int dirty_index = -1; // Index of the oldest dirty line within the set
    uint32_t oldest_clean_time = UINT32_MAX; // Oldest access time among clean lines
    uint32_t oldest_dirty_time = UINT32_MAX; // Oldest access time among dirty lines

    // Iterate over all cache lines within the set to find the eviction candidate
    for (uint32_t i = 0; i < cache_system->associativity; ++i) {
        uint32_t index = i; // Local index within the set
        uint32_t last_access_time = metadata->last_access_times[set_base + index];
        uint8_t is_dirty = metadata->is_dirty[set_base + index];

        // Identify the oldest clean line
        if (!is_dirty && last_access_time < oldest_clean_time) {
            oldest_clean_time = last_access_time;
            clean_index = index;
        }
        // Separately, identify the oldest dirty line
        else if (is_dirty && last_access_time < oldest_dirty_time) {
            oldest_dirty_time = last_access_time;
            dirty_index = index;
        }
    }

    // Prefer to evict the oldest clean line if it exists
    if (clean_index != -1) {
        return clean_index;
    }
    // If no clean line is available for eviction, choose the oldest dirty line
    else if (dirty_index != -1) {
        return dirty_index;
    }

    // Should not reach here in a properly functioning cache system
    // Indicates an error or unexpected condition
    return UINT32_MAX;
}



void lru_prefer_clean_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // cleanup any additional memory that you allocated in the
    // lru_prefer_clean_replacement_policy_new function.
    struct lru_prefer_clean_metadata *metadata = (struct lru_prefer_clean_metadata *)replacement_policy->data;
    if (metadata) {
        free(metadata->last_access_times);
        free(metadata->is_dirty);
        free(metadata);
    }
}

struct replacement_policy *lru_prefer_clean_replacement_policy_new(uint32_t sets,
                                                                   uint32_t associativity)
{
    struct replacement_policy *lru_prefer_clean_rp = malloc(sizeof(struct replacement_policy));
    struct lru_prefer_clean_metadata *metadata = malloc(sizeof(struct lru_prefer_clean_metadata));

    metadata->last_access_times = calloc(sets * associativity, sizeof(uint32_t));
    metadata->is_dirty = calloc(sets * associativity, sizeof(uint8_t));
    metadata->access_counter = 0;

    lru_prefer_clean_rp->cache_access = &lru_prefer_clean_cache_access;
    lru_prefer_clean_rp->eviction_index = &lru_prefer_clean_eviction_index;
    lru_prefer_clean_rp->cleanup = &lru_prefer_clean_replacement_policy_cleanup;
    lru_prefer_clean_rp->data = metadata;
    // allocate any additional memory to store metadata here and assign to
    // lru_prefer_clean_rp->data.

    return lru_prefer_clean_rp;
}
