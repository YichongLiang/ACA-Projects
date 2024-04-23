1. Describe how your prefetcher works.
The custom prefetcher implements a two-level adaptive strategy using both local and global histories. It monitors the recent strides (local history) and identifies the most effective common stride over a longer period (global history). Based on these histories, it dynamically selects which stride to use for prefetching:
Local History: tracks the last few accessed addresses to adapt to immediate changes in access patterns.
Global History: maintains a record of the most successful prefetch stride observed over time. When a cache miss occurs, the prefetcher calculates the current stride, updates the histories, and uses either the local average stride or the global stride to prefetch the next address, depending on their past success rates.

2. Explain how you chose that prefetch strategy.
The chosen strategy enhances the prefetcher's adaptability by using both local and global histories, allowing it to quickly respond to changes in application behavior across both short and long-term patterns. This dual-history approach not only improves responsiveness but also aims to minimize cache pollution by adjusting prefetching decisions based on the effectiveness of previously prefetched data. This method effectively reduces unnecessary data loads, maintaining cache efficiency and performance.

3. Discuss the pros and cons of your prefetch strategy.
Pros:
Adaptive: Adjusts to different access patterns using both recent and long-term data, improving accuracy in diverse scenarios.
Reduced Miss Rates: Potentially lowers cache miss rates by using historically successful prefetch strategies.
Flexibility: Capable of handling both regular and irregular access patterns more effectively than simple stride prefetching.
Cons:
Complexity: More complex to implement and integrate due to the dual-history mechanism.
Resource Intensive: Requires additional memory and computational resources to track and analyze access histories.
Potential for Overhead: The need to frequently update and evaluate histories can introduce overhead, especially in environments with highly volatile access patterns.

4. Demonstrate that the prefetcher could be implemented in hardware (this can be
   as simple as pointing to an existing hardware prefetcher using the strategy
   or a paper describing a hypothetical hardware prefetcher which implements
   your strategy).
The concept of using multiple history tables to guide prefetch decisions has been explored in hardware prefetching technologies. For instance, some advanced prefetchers use similar concepts to adjust their prefetching decisions based on observed access patterns, such as the IP Prefetcher which also dynamically adjusts its strategy based on effectiveness. The feasibility of implementing such adaptive strategies in hardware is supported by their presence in both academic research and commercial processors, indicating that a two-level adaptive prefetcher could realistically be implemented in modern CPUs.

5. Cite any additional sources that you used to develop your prefetcher.
"Data Prefetch Mechanisms" by Steven P and David J.