//
// This file defines the function signatures necessary for creating the three
// cache systems and defines the prefetcher struct.
//

#include "prefetchers.h"

// Null Prefetcher
// ============================================================================
uint32_t null_handle_mem_access(struct prefetcher *prefetcher, struct cache_system *cache_system,
                                uint32_t address, bool is_miss)
{
    return 0; // No lines prefetched
}

void null_cleanup(struct prefetcher *prefetcher) {}

struct prefetcher *null_prefetcher_new()
{
    struct prefetcher *null_prefetcher = calloc(1, sizeof(struct prefetcher));
    null_prefetcher->handle_mem_access = &null_handle_mem_access;
    null_prefetcher->cleanup = &null_cleanup;
    return null_prefetcher;
}

// Sequential Prefetcher
// ============================================================================
// TODO feel free to create additional structs/enums as necessary

struct sequential_data {
    int n;  
};

uint32_t sequential_handle_mem_access(struct prefetcher *prefetcher,
                                      struct cache_system *cache_system, uint32_t address,
                                      bool is_miss)
{
    // TODO: Return the number of lines that were prefetched.
    if(((struct sequential_data*) prefetcher->data)->n > 0){
	    for(int i = 1; i <= ((struct sequential_data*) prefetcher->data)->n; i++){
		    uint32_t fetch_address = address + i*cache_system->line_size;
    		cache_system_mem_access(cache_system, fetch_address, 'R', true);
	    }
    }
    return ((struct sequential_data*) prefetcher->data)->n;;
}

void sequential_cleanup(struct prefetcher *prefetcher)
{
    // TODO cleanup any additional memory that you allocated in the
    // sequential_prefetcher_new function.
    free(prefetcher->data);
}

struct prefetcher *sequential_prefetcher_new(uint32_t prefetch_amount)
{
    struct prefetcher *sequential_prefetcher = calloc(1, sizeof(struct prefetcher));
    sequential_prefetcher->handle_mem_access = &sequential_handle_mem_access;
    sequential_prefetcher->cleanup = &sequential_cleanup;

    // TODO allocate any additional memory needed to store metadata here and
    // assign to sequential_prefetcher->data.
    struct sequential_data *new_data = malloc(sizeof(struct sequential_data));
    new_data->n = prefetch_amount;
    sequential_prefetcher->data = new_data;

    return sequential_prefetcher;
}


// Adjacent Prefetcher
// ============================================================================
uint32_t adjacent_handle_mem_access(struct prefetcher *prefetcher,
                                    struct cache_system *cache_system, uint32_t address,
                                    bool is_miss)
{
    // TODO perform the necessary prefetches for the adjacent strategy.
    
    // TODO: Return the number of lines that were prefetched.
    uint32_t prefetch_address = address + cache_system->line_size;
    cache_system_mem_access(cache_system, prefetch_address, 'R', true);
    return 1; 
}

void adjacent_cleanup(struct prefetcher *prefetcher)
{
    // TODO cleanup any additional memory that you allocated in the
    // adjacent_prefetcher_new function.
    free(prefetcher->data);
}

struct prefetcher *adjacent_prefetcher_new()
{
    struct prefetcher *adjacent_prefetcher = calloc(1, sizeof(struct prefetcher));
    adjacent_prefetcher->handle_mem_access = &adjacent_handle_mem_access;
    adjacent_prefetcher->cleanup = &adjacent_cleanup;

    // TODO allocate any additional memory needed to store metadata here and
    // assign to adjacent_prefetcher->data.

    return adjacent_prefetcher;
}

// Custom Prefetcher
// ============================================================================
struct adaptive_prefetch_data {
    int32_t last_address;
    int32_t local_strides[3];  // Local history buffer
    int32_t global_stride;     // Most effective stride observed
    int local_index;           // Index for local strides
    uint32_t local_hits;       // Successful prefetch count based on local stride
    uint32_t global_hits;      // Successful prefetch count based on global stride
};



uint32_t custom_handle_mem_access(struct prefetcher *prefetcher, struct cache_system *cache_system,
                                  uint32_t address, bool is_miss)
{
    struct adaptive_prefetch_data *data = (struct adaptive_prefetch_data *)prefetcher->data;
    if (is_miss) {
        int32_t current_stride = address - data->last_address;
        data->last_address = address;

        // Update local stride history
        data->local_strides[data->local_index % 3] = current_stride;
        data->local_index++;

        // Determine most successful stride
        int32_t stride_to_use = data->global_stride;
        if (data->local_hits > data->global_hits && data->local_index >= 3) {
            stride_to_use = (data->local_strides[0] + data->local_strides[1] + data->local_strides[2]) / 3;
        }

        // Prefetch using the selected stride
        uint32_t prefetch_address = address + stride_to_use;
        cache_system_mem_access(cache_system, prefetch_address, 'R', true);

        // Update hit statistics
        if (stride_to_use == current_stride) {
            if (stride_to_use == data->global_stride) {
                data->global_hits++;
            } else {
                data->local_hits++;
            }
        }

        return 1;  // One line prefetched
    }
    return 0;
}



void custom_cleanup(struct prefetcher *prefetcher)
{
    // TODO cleanup any additional memory that you allocated in the
    // custom_prefetcher_new function.
    free(prefetcher->data);
}

struct prefetcher *custom_prefetcher_new()
{
    struct prefetcher *custom_prefetcher = calloc(1, sizeof(struct prefetcher));
    struct adaptive_prefetch_data *data = calloc(1, sizeof(struct adaptive_prefetch_data));

    // TODO allocate any additional memory needed to store metadata here and
    // assign to custom_prefetcher->data.

    data->last_address = -1;
    memset(data->local_strides, 0, sizeof(data->local_strides));
    data->global_stride = 0;
    data->local_index = 0;
    data->local_hits = 0;
    data->global_hits = 0;

    custom_prefetcher->data = data;
    custom_prefetcher->handle_mem_access = &custom_handle_mem_access;
    custom_prefetcher->cleanup = &custom_cleanup;

    return custom_prefetcher;
}


