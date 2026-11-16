#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MemoryRegion
{
    int start;
    int size;
    int is_free;
    int allocation_id;
};

struct MemoryRegion *regions;
unsigned char *memory_pool;

int region_count = 1;
int capacity = 10;
int pool_size;
int next_id = 1;

int first_fit(int request)
{
    for (int i = 0; i < region_count; i++)
    {
        if (regions[i].is_free && regions[i].size >= request)
        {
            return i;
        }
    }
    return -1;
}

int best_fit(int request)
{
    int best = -1;
    for (int i = 0; i < region_count; i++)
    {
        if (regions[i].is_free && regions[i].size >= request)
        {
            if (best == -1 || regions[i].size < regions[best].size)
            {
                best = i;
            }
        }
    }
    return best;
}

int worst_fit(int request)
{
    int worst = -1;
    for (int i = 0; i < region_count; i++)
    {
        if (regions[i].is_free && regions[i].size >= request)
        {
            if (worst == -1 || regions[i].size > regions[worst].size)
            {
                worst = i;
            }
        }
    }
    return worst;
}

void allocate_memory(int strategy)
{
    int request;

    printf("Requested Size: ");
    scanf("%d", &request);

    if (request <= 0 || request > pool_size)
    {
        printf("Invalid request size!\n");
        return;
    }

    int index = -1;

    if (strategy == 1)
    {
        index = first_fit(request);
    }
    else if (strategy == 2)
    {
        index = best_fit(request);
    }
    else if (strategy == 3)
    {
        index = worst_fit(request);
    }
    if (index == -1)
    {
        printf("Allocation failed\n");
        return;
    }

    int old_size = regions[index].size;
    if (old_size > request)
    {
        if (region_count == capacity)
        {
            int new_capacity = capacity * 2;

            struct MemoryRegion *temp = realloc(regions, new_capacity * sizeof(struct MemoryRegion));

            if (temp == NULL)
            {
                printf("Metadata expansion failed\n");
                return;
            }

            regions = temp;
            capacity = new_capacity;
        }

        for (int i = region_count; i > index + 1; i--)
        {
            regions[i] = regions[i - 1];
        }

        regions[index + 1].start = regions[index].start + request;
        regions[index + 1].size = old_size - request;
        regions[index + 1].is_free = 1;
        regions[index + 1].allocation_id = -1;
        region_count++;
    }

    regions[index].size = request;
    regions[index].is_free = 0;
    regions[index].allocation_id = next_id;

    printf("Allocation successful. ID: %d, Start: %d, Size: %d\n", next_id, regions[index].start, request);
    next_id++;
}

void merge_block()
{
    for (int i = 0; i < region_count - 1; i++)
    {
        if (regions[i].is_free && regions[i + 1].is_free)
        {
            regions[i].size += regions[i + 1].size;
            for (int j = i + 1; j < region_count - 1; j++)
            {
                regions[j] = regions[j + 1];
            }
            region_count--;
            i--;
        }
    }
}

void release_memory()
{
    int id;
    printf("Enter allocation ID to release: ");
    scanf("%d", &id);
    if (id <= 0)
    {
        printf("Invalid allocation ID\n");
        return;
    }

    int index = -1;
    for (int i = 0; i < region_count; i++)
    {
        if (!regions[i].is_free &&
            regions[i].allocation_id == id)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        printf("Invalid ID or allocation already released\n");
        return;
    }
    regions[index].is_free = 1;
    regions[index].allocation_id = -1;

    printf("Allocation released.\n");
    merge_block();
}

int ensure_capacity()
{
    if (region_count < capacity)
        return 1;

    int new_capacity = capacity * 2;

    struct MemoryRegion *temp = realloc(regions, new_capacity * sizeof(struct MemoryRegion));

    if (temp == NULL)
    {
        printf("Metadata expansion failed\n");
        return 0;
    }

    regions = temp;
    capacity = new_capacity;

    return 1;
}

void resize_allocation(int strategy)
{
    int id, new_size;
    printf("Enter allocation ID: ");
    scanf("%d", &id);
    if (id <= 0)
    {
        printf("Invalid allocation ID\n");
        return;
    }

    printf("Enter new size: ");
    scanf("%d", &new_size);

    if (new_size <= 0 || new_size > pool_size)
    {
        printf("Invalid size\n");
        return;
    }

    int index = -1;

    for (int i = 0; i < region_count; i++)
    {
        if (!regions[i].is_free && regions[i].allocation_id == id)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        printf("Invalid allocation ID\n");
        return;
    }

    int old_size = regions[index].size;
    int old_start = regions[index].start;

    if (new_size == old_size)
    {
        printf("Allocation size is unchanged.\n");
        return;
    }

    if (new_size < old_size)
    {
        if (!ensure_capacity())
            return;

        for (int i = region_count; i > index + 1; i--)
        {
            regions[i] = regions[i - 1];
        }

        regions[index].size = new_size;

        regions[index + 1].start = old_start + new_size;
        regions[index + 1].size = old_size - new_size;
        regions[index + 1].is_free = 1;
        regions[index + 1].allocation_id = -1;

        region_count++;

        merge_block();

        printf("Allocation shrunk successfully\n");
        return;
    }

    int extra = new_size - old_size;

    if (index + 1 < region_count && regions[index + 1].is_free && regions[index + 1].size >= extra)
    {
        if (regions[index + 1].size == extra)
        {
            for (int i = index + 1; i < region_count - 1; i++)
            {
                regions[i] = regions[i + 1];
            }

            region_count--;
        }
        else
        {
            regions[index + 1].start += extra;
            regions[index + 1].size -= extra;
        }

        regions[index].size = new_size;

        printf("Allocation expanded in place\n");
        return;
    }

    int target = -1;

    if (strategy == 1)
    {
        target = first_fit(new_size);
    }
    else if (strategy == 2)
    {
        target = best_fit(new_size);
    }
    else if (strategy == 3)
    {
        target = worst_fit(new_size);
    }
    if (target == -1)
    {
        printf("Resize failed: insufficient contiguous memory\n");
        return;
    }

    int target_start = regions[target].start;
    int target_size = regions[target].size;

    if (target_size > new_size && !ensure_capacity())
    {
        return;
    }

    memmove(memory_pool + target_start, memory_pool + old_start, old_size);
    if (target_size > new_size)
    {
        for (int i = region_count; i > target + 1; i--)
        {
            regions[i] = regions[i - 1];
        }

        regions[target + 1].start = target_start + new_size;
        regions[target + 1].size = target_size - new_size;
        regions[target + 1].is_free = 1;
        regions[target + 1].allocation_id = -1;

        region_count++;
    }

    regions[target].size = new_size;
    regions[target].is_free = 0;
    regions[target].allocation_id = id;

    for (int i = 0; i < region_count; i++)
    {
        if (!regions[i].is_free && regions[i].allocation_id == id && regions[i].start == old_start)
        {
            regions[i].is_free = 1;
            regions[i].allocation_id = -1;
            break;
        }
    }

    merge_block();

    printf("Allocation moved and resized successfully\n");
}

void display_memory_map()
{
    printf("MEMORY MAP (%d bytes):\n", pool_size);

    for (int i = 0; i < region_count; i++)
    {
        if (regions[i].is_free)
        {
            printf("[Free: %d bytes, Start: %d]\n", regions[i].size, regions[i].start);
        }
        else
        {
            printf("[Allocated #%d: %d bytes, Start: %d]\n", regions[i].allocation_id, regions[i].size, regions[i].start);
        }
    }
}

void display_memory_statistics()
{
    int allocated = 0;
    int free_memory = 0;
    int free_blocks = 0;
    int largest_free = 0;

    for (int i = 0; i < region_count; i++)
    {
        if (regions[i].is_free)
        {
            free_memory += regions[i].size;
            free_blocks++;

            if (regions[i].size > largest_free)
            {
                largest_free = regions[i].size;
            }
        }
        else
        {
            allocated += regions[i].size;
            allocated_blocks++;
        }
    }

    printf("Total memory: %d bytes\n", pool_size);
    printf("Allocated: %d bytes\n", allocated);
    printf("Free: %d bytes\n", free_memory);
    printf("Free blocks: %d\n", free_blocks);
    printf("Largest free block: %d bytes\n", largest_free);
}

void change_allocation(int *strategy)
{
    int choice;
    printf("Choose allocation strategy:\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");
    printf("Enter choice: ");

    scanf("%d", &choice);

    if (choice >= 1 && choice <= 3)
    {
        *strategy = choice;
        printf("Allocation strategy changed successfully.\n");
    }
    else
    {
        printf("Invalid strategy\n");
    }
}

int main()
{
    printf("enter memory pool size:\n");
    scanf("%d", &pool_size);
    if (pool_size < 1 || pool_size > 1000000)
    {
        printf("Invalid memory pool size!\n");
        return 1;
    }
    memory_pool = malloc(pool_size);
    regions = malloc(capacity * sizeof(struct MemoryRegion));
    if (memory_pool == NULL || regions == NULL)
    {
        printf("Memory allocation failed\n");
        free(memory_pool);
        free(regions);
        return 1;
    }
    regions[0].start = 0;
    regions[0].size = pool_size;
    regions[0].is_free = 1;
    regions[0].allocation_id = -1;
    printf("Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit):\n");
    int strategy;
    scanf("%d", &strategy);
    int choice;
    do
    {
        printf("Menu:\n1. Allocate memory\n2. Release memory\n3. Resize allocation\n4. Display memory map\n5. Display memory statistics\n6. Change allocation strategy\n7. Exit\n\nChoice:");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            allocate_memory(strategy);
            break;
        case 2:
            release_memory();
            break;
        case 3:
            resize_allocation();
            break;
        case 4:
            display_memory_map();
            break;
        case 5:
            display_memory_statistics();
            break;
        case 6:
            change_allocation(&strategy);
            break;
        case 7:
            free(memory_pool);
            free(regions);
            printf("Program ended.\n");
            break;
        default:
            printf("Invalid Choice.\n");
            break;
        }
    } while (choice != 7);
    return 0;
}