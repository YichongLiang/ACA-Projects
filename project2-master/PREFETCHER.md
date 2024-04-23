1. Describe how your prefetcher works.
The custom prefetcher uses stride-based prefetching, where it predicts the next memory address to prefetch based on the difference between the current and the last accessed addresses. Upon a cache miss, it prefetches the data at an address that is one stride away from the current address and updates the last accessed address for the next prediction.

2. Explain how you chose that prefetch strategy.
This strategy was chosen for its effectiveness in predictable, linear data access patterns, commonly seen in array processing and sequential data traversals, where it significantly reduces cache miss rates.

3. Discuss the pros and cons of your prefetch strategy.
Pros:
Effective for linear and regular access patterns.
Simple to implement and integrate.
Dynamically adapts to changing strides.
Cons:
Less effective for irregular, non-linear access patterns.
Risk of cache pollution if the predicted stride is incorrect.
Involves computational overhead for managing the stride calculation.

4. Demonstrate that the prefetcher could be implemented in hardware (this can be
   as simple as pointing to an existing hardware prefetcher using the strategy
   or a paper describing a hypothetical hardware prefetcher which implements
   your strategy).
Stride prefetching is a validated approach used in hardware prefetchers like Intel’s Next-Line and AMD’s Stride Prefetcher. These implementations show that stride-based prefetching is feasible, practical, and beneficial for enhancing cache performance in hardware systems.

5. Cite any additional sources that you used to develop your prefetcher.
”Reducing Memory Latency by Improving Resource Utilization“ by Marius Grannæs