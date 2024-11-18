// Personal academic work. Dear PVS-Studio, analyze this, please.
// PVS-Studio Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <memManager.h>
#include <utils.h>
#include <videoDriver.h>

#define MIN_ALLOC_UNIT 8

typedef enum {
    MEM_FULL, MEM_EMPTY, MEM_SPLIT
} MemState;

typedef struct MemoryNode {
    struct MemoryNode *leftChild;
    struct MemoryNode *rightChild;
    unsigned nodeIndex;
    void *memoryBlock;
    unsigned blockSize;
    MemState currentState;
} MemoryNode;

#define POWER_OF_2(x) (((x) & ((x) - 1)) == 0)

static MemoryNode *rootNode;
static unsigned allocatedMemory = 0;

static void recursive_print(MemoryNode *current, int depth, char *buffer, int *offset);

static unsigned align_size(unsigned size) {
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    return size + 1;
}

void* split_node(MemoryNode *parent) {
    unsigned childIdx = parent->nodeIndex * 2 + 1;
    parent->leftChild = (MemoryNode *)((char *)parent + childIdx * sizeof(MemoryNode));

    if ((uint64_t) parent->leftChild >= MEM_START) {
        return NULL;
    }
    parent->leftChild->nodeIndex = childIdx;
    parent->leftChild->blockSize = parent->blockSize / 2;
    parent->leftChild->memoryBlock = parent->memoryBlock;
    parent->leftChild->currentState = MEM_EMPTY;

    uint64_t offset = (uint64_t)(parent->memoryBlock) + (parent->blockSize / 2);

    parent->rightChild = (MemoryNode *)((char *)parent + (childIdx + 1) * sizeof(MemoryNode));
    if ((uint64_t) parent->rightChild >= MEM_START) {
        return NULL;
    }
    parent->rightChild->nodeIndex = childIdx + 1;
    parent->rightChild->blockSize = parent->blockSize / 2;
    parent->rightChild->memoryBlock = (void *)offset;
    parent->rightChild->currentState = MEM_EMPTY;
    return NULL;
}

void update_node_state(MemoryNode *node) {
    if (!node->leftChild || !node->rightChild) {
        node->currentState = MEM_EMPTY;
        return;
    }
    if (node->leftChild->currentState == MEM_FULL && node->rightChild->currentState == MEM_FULL) {
        node->currentState = MEM_FULL;
    } else if (node->leftChild->currentState != MEM_EMPTY || node->rightChild->currentState != MEM_EMPTY) {
        node->currentState = MEM_SPLIT;
    } else {
        node->currentState = MEM_EMPTY;
    }
}

void *allocate_recursive(MemoryNode *node, uint32_t size) {
    if (node->currentState == MEM_FULL) {
        return NULL;
    }

    if (node->leftChild || node->rightChild) {
        void *allocated = allocate_recursive(node->leftChild, size);
        if (!allocated) {
            allocated = allocate_recursive(node->rightChild, size);
        }
        update_node_state(node);
        return allocated;
    }

    if (size > node->blockSize) {
        return NULL;
    }

    if (node->blockSize / 2 >= size) {
        split_node(node);
        void *allocated = allocate_recursive(node->leftChild, size);
        update_node_state(node);
        return allocated;
    }

    node->currentState = MEM_FULL;
    allocatedMemory += size;
    return node->memoryBlock;
}

void *mem_alloc(uint32_t size) {
    if (size > rootNode->blockSize) {
        return NULL;
    }

    if (size < MIN_ALLOC_UNIT) {
        size = MIN_ALLOC_UNIT;
    }

    if (!POWER_OF_2(size)) {
        size = align_size(size);
    }

    return allocate_recursive(rootNode, size);
}

int free_recursive(MemoryNode *node, void *block) {
    if (node->leftChild || node->rightChild) {
        int result;
        if (node->rightChild && (uint64_t)node->rightChild->memoryBlock > (uint64_t)block) {
            result = free_recursive(node->leftChild, block);
        } else {
            result = free_recursive(node->rightChild, block);
        }

        update_node_state(node);

        if (node->currentState == MEM_EMPTY) {
            node->leftChild = NULL;
            node->rightChild = NULL;
        }
        return result;
    }

    if (node->currentState == MEM_FULL && node->memoryBlock == block) {
        node->currentState = MEM_EMPTY;
        allocatedMemory -= node->blockSize;
        return 0;
    }
    return -1;
}

void mem_free(void *ptr) {
    free_recursive(rootNode, ptr);
}

void mem_init(void *base, uint32_t size) {
    rootNode = (MemoryNode *)base;
    rootNode->nodeIndex = 0;
    rootNode->blockSize = size;
    rootNode->currentState = MEM_EMPTY;
    rootNode->memoryBlock = (void *)MEM_START;
}

char *mem_state() {
    char *stateBuffer = mem_alloc(2048);
    if (!stateBuffer) {
        return NULL;
    }

    int offset = 0;
    strcpy(stateBuffer + offset, "Memory Status Report:\n", strlen("Memory Status Report:\n"));
    offset += strlen("Memory Status Report:\n");

    strcpy(stateBuffer + offset, "Total: ", strlen("Total: "));
    offset += strlen("Total: ");
    intToStr(rootNode->blockSize, stateBuffer + offset);
    offset += strlen(stateBuffer + offset);
    stateBuffer[offset++] = '\n';

    strcpy(stateBuffer + offset, "Used: ", strlen("Used: "));
    offset += strlen("Used: ");
    intToStr(allocatedMemory, stateBuffer + offset);
    offset += strlen(stateBuffer + offset);
    stateBuffer[offset++] = '\n';

    strcpy(stateBuffer + offset, "Free: ", strlen("Free: "));
    offset += strlen("Free: ");
    intToStr(rootNode->blockSize - allocatedMemory, stateBuffer + offset);
    offset += strlen(stateBuffer + offset);
    stateBuffer[offset++] = '\n';

    recursive_print(rootNode, 0, stateBuffer, &offset);
    return stateBuffer;
}

static void recursive_print(MemoryNode *node, int depth, char *buffer, int *offset) {
    if (!node) return;

    for (int i = 0; i < depth; i++) {
        buffer[(*offset)++] = ' ';
    }

    strcpy(buffer + *offset, "Node ", strlen("Node "));
    (*offset) += strlen("Node ");
    intToStr(node->nodeIndex, buffer + *offset);
    (*offset) += strlen(buffer + *offset);

    strcpy(buffer + *offset, " | BlockSize: ", strlen(" | BlockSize: "));
    (*offset) += strlen(" | BlockSize: ");
    intToStr(node->blockSize, buffer + *offset);
    (*offset) += strlen(buffer + *offset);

    strcpy(buffer + *offset, " | State: ", strlen(" | State: "));
    (*offset) += strlen(" | State: ");
    strcpy(buffer + *offset,
           node->currentState == MEM_EMPTY ? "EMPTY" :
           node->currentState == MEM_SPLIT ? "SPLIT" : "FULL", 
           strlen(node->currentState == MEM_EMPTY ? "EMPTY" :
                  node->currentState == MEM_SPLIT ? "SPLIT" : "FULL"));
    (*offset) += strlen(buffer + *offset);
    buffer[(*offset)++] = '\n';

    recursive_print(node->leftChild, depth + 1, buffer, offset);
    recursive_print(node->rightChild, depth + 1, buffer, offset);
}
