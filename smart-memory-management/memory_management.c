#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<limits.h>

typedef enum{
    FIRST_FIT = 1,
    BEST_FIT,
    WORST_FIT
} AllocationStrategy;

typedef struct{
    int start, size;
    long long allocationID;
    int isAllocated;
} Block;

char *memory = NULL;
Block *blocks = NULL;
int poolSize = 0;
int blockCount = 0;
int capacity = 4;
long long nextID = 1;
int strategy = FIRST_FIT;

int readNumber(const char *message, long long *value){
    char buffer[100];
    printf("%s", message);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL){
        return 0;
    }

    if (strchr(buffer, '\n') == NULL && !feof(stdin)){
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        printf("Input is too long.\n");
        return -1;
    }

    char *p = buffer;
    while (*p == ' ' || *p == '\t'){
        p++;
    }
    if (*p == '-'){
        printf("Negative numbers are not allowed.\n");
        return -1;
    }
    if (*p < '0' || *p > '9'){
        printf("Invalid input.\n");
        return -1;
    }

    long long number = 0;
    long long maxValue = LLONG_MAX;
    while (*p >= '0' && *p <= '9'){
        int digit = *p - '0';
        if (number > (maxValue - digit) / 10){
            printf("Number is too large.\n");
            return -1;
        }
        number = number * 10 + digit;
        p++;
    }

    while (*p == ' ' || *p == '\t' || *p == '\r')
        p++;
    if (*p != '\n' && *p != '\0'){
        printf("Invalid input. Enter an integer only.\n");
        return -1;
    }

    *value = number;
    return 1;
}

int initializePool(int size){
    if (size < 1 || size > 1000000){
        printf("Invalid memory pool size.\n");
        return 0;
    }
    poolSize = size;
    memory = (char *)malloc(poolSize);
    if (!memory){
        printf("Memory pool allocation failed.\n");
        return 0;
    }
    memset(memory, 0, poolSize);

    capacity = 4;
    blocks = (Block *)malloc(capacity * sizeof(Block));
    if (!blocks){
        printf("Metadata allocation failed.\n");
        free(memory);
        memory = NULL;
        return 0;
    }

    blockCount = 1;
    blocks[0] = (Block){0, poolSize, 0, 0};
    nextID = 1;
    return 1;
}

int growMetadata(){
    if ((size_t)capacity > ((size_t)-1 / sizeof(Block)) / 2){
        return 0;
    }
    int newCapacity = capacity * 2;
    Block *temp = (Block *)realloc(blocks, newCapacity * sizeof(Block));
    if (!temp)
    {
        printf("Metadata expansion failed.\n");
        return 0;
    }
    blocks = temp;
    capacity = newCapacity;
    return 1;
}

void shiftRight(int index){
    for (int j = blockCount; j > index; j--)
        blocks[j] = blocks[j - 1];
    blockCount++;
}

void shiftLeft(int index){
    for (int j = index; j + 1 < blockCount; j++)
        blocks[j] = blocks[j + 1];
    blockCount--;
}

int firstFit(int requestSize){
    for (int i = 0; i < blockCount; i++)
        if (!blocks[i].isAllocated && blocks[i].size >= requestSize)
            return i;
    return -1;
}

int bestFit(int requestSize)
{
    int selected = -1;
    int smallest = INT_MAX;
    for (int i = 0; i < blockCount; i++) {
        if (!blocks[i].isAllocated && blocks[i].size >= requestSize) {
            if (selected == -1 || blocks[i].size < smallest) {
                smallest = blocks[i].size;
                selected = i;
            }
        }
    }
    return selected;
}

int worstFit(int requestSize)
{
    int selected = -1;
    int largest = 0;
    for (int i = 0; i < blockCount; i++){
        if (!blocks[i].isAllocated && blocks[i].size >= requestSize && blocks[i].size > largest){
            largest = blocks[i].size;
            selected = i;
        }
    }
    return selected;
}

int findSuitableBlock(int requestSize)
{
    switch (strategy)
    {
    case FIRST_FIT:
        return firstFit(requestSize);
    case BEST_FIT:
        return bestFit(requestSize);
    case WORST_FIT:
        return worstFit(requestSize);
    default:
        return -1;
    }
}

int insertAllocation(int index, int requestSize, long long id)
{
    int remaining = blocks[index].size - requestSize, oldStart = blocks[index].start;
    if (remaining > 0){
        if (blockCount == capacity)
            return 0;
        shiftRight(index + 1);
        blocks[index + 1] = (Block){oldStart + requestSize, remaining, 0, 0};
    }
    blocks[index] = (Block){oldStart, requestSize, id, 1};
    return 1;
}

void detectFragmentation(int requestSize)
{
    int totalFree = 0, largestFree = 0;
    for (int i = 0; i < blockCount; i++){
        if (!blocks[i].isAllocated){
            totalFree += blocks[i].size;
            if (blocks[i].size > largestFree)
                largestFree = blocks[i].size;
        }
    }
    if (totalFree >= requestSize && largestFree < requestSize){
        printf("Note: The request for %d bytes failed even though %d bytes are free, because the free memory is fragmented.\n", requestSize, totalFree);
    }
}

int allocateMemory(){
    long long input;
    int result = readNumber("Requested size: ", &input);
    if (result <= 0)
        return result;
    if (input == 0 || input > poolSize || nextID == 0){
        printf("Allocation failed: invalid size.\n");
        return 1;
    }

    int requestSize = input;
    int index = findSuitableBlock(requestSize);
    if (index == -1){
        printf("Allocation failed: no contiguous free block is large enough.\n");
        detectFragmentation(requestSize);
        return 1;
    }

    if (blocks[index].size > requestSize && blockCount == capacity && !growMetadata()){
        printf("Allocation failed.\n");
        return 1;
    }

    long long id = nextID;
    if (!insertAllocation(index, requestSize, id)){
        printf("Allocation failed.\n");
        return 1;
    }

    nextID = (id == LLONG_MAX) ? 0 : id + 1;
    printf("Allocation successful. ID: %lld, Start: %d, Size: %d\n", id, blocks[index].start, requestSize);
    return 1;
}

int findAllocation(long long id){
    for (int i = 0; i < blockCount; i++)
        if (blocks[i].isAllocated && blocks[i].allocationID == id)
            return i;
    return -1;
}

void mergeFreeBlocks(){
    int i = 0;
    while (i + 1 < blockCount){
        if (!blocks[i].isAllocated && !blocks[i + 1].isAllocated && blocks[i].start + blocks[i].size == blocks[i + 1].start){
            blocks[i].size += blocks[i + 1].size;
            shiftLeft(i + 1);
        }
        else
            i++;
    }
}

int releaseMemory(){
    long long id;
    int result = readNumber("Allocation ID: ", &id);
    if (result <= 0)
        return result;
    if (id == 0){
        printf("Invalid ID.\n");
        return 1;
    }

    int index = findAllocation(id);
    if (index == -1){
        printf("Invalid ID or allocation already released.\n");
        return 1;
    }

    blocks[index].isAllocated = 0;
    blocks[index].allocationID = 0;
    mergeFreeBlocks();
    printf("Allocation released.\n");
    return 1;
}

int resizeAllocation(long long id, int newSize)
{
    if (id == 0 || newSize == 0 || newSize > poolSize)
    {
        printf("Invalid resize request.\n");
        return 0;
    }
    int index = findAllocation(id);
    if (index == -1)
    {
        printf("Allocation ID not found.\n");
        return 0;
    }

    int oldStart = blocks[index].start, oldSize = blocks[index].size;
    if (newSize == oldSize){
        printf("Allocation size is unchanged.\n");
        return 1;
    }

    if (newSize < oldSize){
        int released = oldSize - newSize;
        if (index + 1 < blockCount && !blocks[index + 1].isAllocated){
            blocks[index].size = newSize;
            blocks[index + 1].start = oldStart + newSize;
            blocks[index + 1].size += released;
        }
        else
        {
            if (blockCount == capacity && !growMetadata()){
                printf("Resize failed. Original allocation unchanged.\n");
                return 0;
            }
            shiftRight(index + 1);
            blocks[index].size = newSize;
            blocks[index + 1] = (Block){oldStart + newSize, released, 0, 0};
        }
        mergeFreeBlocks();
        printf("Allocation shrunk successfully.\n");
        return 1;
    }

    int extra = newSize - oldSize;
    if (index + 1 < blockCount && !blocks[index + 1].isAllocated && blocks[index + 1].size >= extra){
        blocks[index].size = newSize;
        if (blocks[index + 1].size == extra)
            shiftLeft(index + 1);
        else{
            blocks[index + 1].start += extra;
            blocks[index + 1].size -= extra;
        }
        printf("Allocation expanded in place successfully.\n");
        return 1;
    }

    int destination = findSuitableBlock(newSize);
    if (destination == -1){
        printf("Resize failed: No suitable free block.\n");
        detectFragmentation(newSize);
        return 0;
    }
    if (blocks[destination].size > newSize && blockCount == capacity && !growMetadata()){
        printf("Resize failed. Original allocation unchanged.\n");
        return 0;
    }

    int newStart = blocks[destination].start;
    memmove(memory + newStart, memory + oldStart, oldSize);

    if (!insertAllocation(destination, newSize, id)){
        printf("Resize failed.\n");
        return 0;
    }

    int oldIndex = -1;
    for (int i = 0; i < blockCount; i++){
        if (blocks[i].isAllocated && blocks[i].allocationID == id && blocks[i].start == oldStart){
            oldIndex = i;
            break;
        }
    }
    if (oldIndex == -1){
        printf("Internal error.\n");
        return 0;
    }

    blocks[oldIndex].isAllocated = 0;
    blocks[oldIndex].allocationID = 0;
    mergeFreeBlocks();

    printf("Allocation moved and resized successfully.\nOld start: %d\nNew start: %d\nNew size: %d bytes\n", oldStart, newStart, newSize);
    return 1;
}

void displayMemoryMap()
{
    printf("Memory Map (%d bytes):\n", poolSize);
    for (int i = 0; i < blockCount; i++)
    {
        if (blocks[i].isAllocated)
            printf("[Allocated #%lld: %d bytes, Start: %d]\n", blocks[i].allocationID, blocks[i].size, blocks[i].start);
        else
            printf("[Free: %d bytes, Start: %d]\n", blocks[i].size, blocks[i].start);
    }
}

void displayStatistics()
{
    int allocated = 0, freeMemory = 0, allocatedBlocks = 0, freeBlocks = 0, largestFree = 0;
    for (int i = 0; i < blockCount; i++)
    {
        if (blocks[i].isAllocated)
        {
            allocated += blocks[i].size;
            allocatedBlocks++;
        }
        else
        {
            freeMemory += blocks[i].size;
            freeBlocks++;
            if (blocks[i].size > largestFree)
                largestFree = blocks[i].size;
        }
    }
    printf("Total memory: %d bytes\nAllocated: %d bytes\nFree: %d bytes\nFree blocks: %d\nLargest free block: %d bytes\n", poolSize, allocated, freeMemory, freeBlocks, largestFree);
}

void changeStrategy()
{
    long long choice;
    int result = readNumber("Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ", &choice);
    if (result <= 0)
        return;
    if (choice < FIRST_FIT || choice > WORST_FIT)
    {
        printf("Invalid strategy.\n");
        return;
    }
    strategy = (AllocationStrategy)choice;
    printf("Strategy set to %s Fit.\n", strategy == FIRST_FIT ? "First" : (strategy == BEST_FIT ? "Best" : "Worst"));
}

void cleanup()
{
    free(memory);
    free(blocks);
    memory = NULL;
    blocks = NULL;
    blockCount = 0;
    capacity = 0;
}

int main()
{
    long long input, choice;

    if (readNumber("Enter memory pool size: ", &input) != 1 || input < 1 || input > 1000000)
    {
        printf("Invalid pool size.\n");
        return 0;
    }

    if (!initializePool(input))
    {
        cleanup();
        return 1;
    }

    if (readNumber("Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ", &choice) != 1 || choice < FIRST_FIT || choice > WORST_FIT)
    {
        printf("Invalid strategy. First Fit selected by default.\n");
        strategy = FIRST_FIT;
    }
    else{
        strategy = choice;
    }
    printf("Strategy set to %s Fit.\n", strategy == FIRST_FIT ? "First" : (strategy == BEST_FIT ? "Best" : "Worst"));

    int running = 1;
    while (running)
    {
        printf("Menu:\n1. Allocate memory\n2. Release memory\n3. Resize allocation\n4. Display memory map\n5. Display memory statistics\n6. Change allocation strategy\n7. Exit\n");
        int result = readNumber("Choice: ", &choice);
        if (result == 0)
            break;
        if (result < 0)
            continue;

        switch (choice)
        {
        case 1:
            if (allocateMemory() == 0)
                running = 0;
            break;
        case 2:
            if (releaseMemory() == 0)
                running = 0;
            break;
        case 3:
        {
            long long id, newSize;
            result = readNumber("Allocation ID: ", &id);
            if (result == 0)
            {
                running = 0;
                break;
            }
            if (result < 0)
                break;
            result = readNumber("New size: ", &newSize);
            if (result == 0)
            {
                running = 0;
                break;
            }
            if (result < 0)
                break;
            resizeAllocation(id, newSize);
            break;
        }
        case 4:
            displayMemoryMap();
            break;
        case 5:
            displayStatistics();
            break;
        case 6:
            changeStrategy();
            break;
        case 7:
            running = 0;
            break;
        default:
            printf("Invalid choice.\n");
        }
    }

    cleanup();
    return 0;
}