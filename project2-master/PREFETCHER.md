1. Describe how your prefetcher works.
The custom prefetcher utilizes stride-based prefetching. It monitors the difference between consecutive memory addresses accessed during cache misses (the stride) and uses this stride to predict and prefetch the next likely address to be accessed. Upon a cache miss, the prefetcher:
Calculates the current stride from the last accessed address.
Prefetches data from the address that is one stride away.
Updates the stored last address and last stride for future reference.

2. Explain how you chose that prefetch strategy.
This strategy was selected for its simplicity and effectiveness in applications with predictable, linear data access patterns. Stride prefetching is particularly adept at improving performance in scenarios such as iterating over arrays or processing large data blocks sequentially, where subsequent data accesses often occur at regular address intervals.

3. Discuss the pros and cons of your prefetch strategy.
pros:
Efficiency in Predictable Patterns: Highly effective in environments with regular access patterns, improving cache hit rates significantly.
Simplicity: Relatively easy to implement and integrate into existing systems.
Adaptivity: Capable of dynamically adjusting to detect and follow the stride pattern as it changes over time.
Cons:
Limited by Pattern Variability: Less effective in handling irregular, non-linear access patterns, which can lead to mispredictions and unnecessary prefetches.
Resource Utilization: Although simpler than more complex schemes, it still requires tracking and updating of strides, which involves some computational overhead.
Cache Pollution Risk: Incorrect stride predictions can lead to prefetching of unwanted data, potentially evicting useful data from the cache.

4. Demonstrate that the prefetcher could be implemented in hardware (this can be
   as simple as pointing to an existing hardware prefetcher using the strategy
   or a paper describing a hypothetical hardware prefetcher which implements
   your strategy).
Stride prefetching techniques have been successfully implemented in hardware by various processor manufacturers. For example, Intel and AMD utilize stride prefetching mechanisms in their modern CPUs to anticipate and load subsequent cache lines based on observed access patterns. These implementations validate the practicality and utility of stride-based prefetching in enhancing processor performance and demonstrate that such techniques can be efficiently realized in hardware.

5. Cite any additional sources that you used to develop your prefetcher.
"Data Prefetch Mechanisms" by Steven P and David J.