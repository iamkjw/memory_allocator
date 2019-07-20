//FIRST_FIT finds first available memory block that is at least as large as the requested size.
//BEST_FIT finds memory block that results in the smallest remainder fragment.
//WORST_FIT finds memory block that results in the largest remainder fragment.
enum allocation_algorithm {FIRST_FIT, BEST_FIT, WORST_FIT};

//initializing contiguous memory allocation
void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm);

//compact the memory chunk, relocating and merging contiguous free blocks
int compact_allocation(void** _before, void** _after);

//prints detailed statistics
void print_statistics(void);

//allocate memory
void* kalloc(int_size);

//free memory
void kfree(void* _ptr);

//returns the available memory size
int available_memory(void);

//release any dynamically allocated memory
void destroy_allocator();
