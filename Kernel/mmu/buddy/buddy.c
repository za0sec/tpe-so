// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <memManager.h>
#include <utils.h>
#include <videoDriver.h>

#define MIN_ALLOC 8

enum StateMem {
    FULL, EMPTY, SPLIT
};

typedef struct node {
    struct node *left;
    struct node *right;
    unsigned index;
    void *memPtr;
    unsigned size;
    enum StateMem state;
} node;

#define IS_POWER_OF_2(x) (!((x)&((x)-1)))

static node *root;
unsigned memoryAllocated = 0;

static void traverse_and_print(struct node *node, int depth, char *buf, int *offset);

static unsigned fixsize(unsigned size) {
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    return size + 1;
}

void* createSons(node *parent) {
    unsigned idx = parent->index * 2 + 1;
    parent->left = (node *)((char *)parent + idx * sizeof(node));

    if ((uint64_t) parent->left >= MEM_START) {
        return NULL;
    }
    parent->left->index = idx;
    parent->left->size = parent->size / 2;
    parent->left->memPtr = parent->memPtr;
    parent->left->state = EMPTY;

    uint64_t aux = (uint64_t)(parent->memPtr) + (parent->size / 2);


    parent->right = (node *)((char *)parent + (idx + 1) * sizeof(node));
    if ((uint64_t) parent->right >= (uint64_t)MEM_START){
        return NULL;
    }
    parent->right->index = idx + 1;
    parent->right->size = parent->size / 2;

    parent->right->memPtr = (void *) aux;
    parent->right->state = EMPTY;
    return NULL;
}

void stateUpdate(node *actual) {
    if (actual->right == NULL || actual->left == NULL) {
        actual->state = EMPTY;
        return;
    }
    if (actual->left->state == FULL && actual->right->state == FULL)
        actual->state = FULL;
    else if (actual->left->state == SPLIT || actual->right->state == SPLIT ||
             actual->left->state == FULL || actual->right->state == FULL)
        actual->state = SPLIT;
    else
        actual->state = EMPTY;
}

void *allocRec(node *actual, uint32_t size) {
    if (actual->state == FULL) {
        return NULL;
    }

    if (actual->left != NULL || actual->right != NULL) { //si tiene hijos es porque este nodo ya esta particionado
        void *aux = allocRec(actual->left, size);
        if (aux == NULL) {
            aux = allocRec(actual->right, size);
        }
        stateUpdate(actual);
        return aux;
    } else {

        if (size > actual->size) {
            return NULL;
        }

        if (actual->size / 2 >= size) {
            createSons(actual);
            void *aux = allocRec(actual->left, size);
            stateUpdate(actual);
            return aux;

        }
        actual->state = FULL;
        memoryAllocated += size;
        return actual->memPtr;
    }

    return NULL;
}


void *mem_alloc(uint32_t s) {
    if (s > root->size) {
        return NULL;
    }

    if (s < MIN_ALLOC)
        s = MIN_ALLOC;

    if (!IS_POWER_OF_2(s)) {
        s = fixsize(s);
    }

    void *ptr = allocRec(root, s);
    return ptr;
}


int freeRec(node *actual, void *block) {
    if (actual->left != NULL || actual->right != NULL) {
        int ret;
        if (actual->right!=0 && (uint64_t) actual->right->memPtr > (uint64_t) block) {
            ret = freeRec(actual->left, block);

        } else {
            ret = freeRec(actual->right, block);
        }

        stateUpdate(actual);

        if (actual->state == EMPTY) {
            actual->right = NULL;
            actual->left = NULL;
        }
        return ret;


    } else if (actual->state == FULL) {
        if (actual->memPtr == block) {
            actual->state = EMPTY;
            memoryAllocated -= actual->size;
            return 0;
        }
        return -1;
    }
    return -1;
}


void mem_free(void *ptr) {
    int i = freeRec(root, ptr);
    if (i == -1) {
         return;
     }
}

void mem_init(void * ptr, uint32_t s) {
    root = (node *)ptr;
    root->index = 0;
    root->size = s;
    root->state = EMPTY;
    root->memPtr = (void *) MEM_START;
}


char *mem_state() {
    char *buf = mem_alloc(1024 * 2); 
    if (!buf) {
        return NULL; 
    }

    int offset = 0;
    strcpy(buf + offset, "Memory State:\n", strlen("Memory State:\n"));
    offset += strlen("Memory State:\n");

    // Agrega "Total memory: "
    strcpy(buf + offset, "Total memory: ", strlen("Total memory: "));
    offset += strlen("Total memory: ");
    intToStr(root->size, buf + offset); // Convierte y agrega el tamaño total.
    offset += strlen(buf + offset);
    buf[offset++] = '\n';

    // Agrega "Memory allocated: "
    strcpy(buf + offset, "Memory allocated: ", strlen("Memory allocated: "));
    offset += strlen("Memory allocated: ");
    intToStr(memoryAllocated, buf + offset); // Convierte y agrega la memoria asignada.
    offset += strlen(buf + offset);
    buf[offset++] = '\n';

    // Agrega "Memory free: "
    strcpy(buf + offset, "Memory free: ", strlen("Memory free: "));
    offset += strlen("Memory free: ");
    intToStr(root->size - memoryAllocated, buf + offset); // Convierte y agrega la memoria libre.
    offset += strlen(buf + offset);
    buf[offset++] = '\n';

    traverse_and_print(root, 0, buf, &offset);

    return buf;
}

static void traverse_and_print(node *node, int depth, char *buf, int *offset) {
    if (!node) return;

    // Indentación para mostrar la jerarquía
    for (int i = 0; i < depth; i++) {
        buf[*offset] = ' ';
        (*offset)++;
    }

    // Agrega información del nodo
    strcpy(buf + *offset, "Node ", strlen("Node "));
    (*offset) += strlen("Node ");
    intToStr(node->index, buf + *offset);
    (*offset) += strlen(buf + *offset);

    strcpy(buf + *offset, " | Size: ", strlen(" | Size: "));
    (*offset) += strlen(" | Size: ");
    intToStr(node->size, buf + *offset);
    (*offset) += strlen(buf + *offset);

    strcpy(buf + *offset, " | State: ", strlen(" | State: "));
    (*offset) += strlen(" | State: ");
    strcpy(buf + *offset,
            node->state == EMPTY ? "EMPTY" :
            node->state == SPLIT ? "SPLIT" : "FULL", strlen(node->state == EMPTY ? "EMPTY" :
            node->state == SPLIT ? "SPLIT" : "FULL"));
    (*offset) += strlen(buf + *offset);
    buf[*offset] = '\n';
    (*offset)++;

        // Recursión en los hijos
    traverse_and_print(node->left, depth + 1, buf, offset);
    traverse_and_print(node->right, depth + 1, buf, offset);
}