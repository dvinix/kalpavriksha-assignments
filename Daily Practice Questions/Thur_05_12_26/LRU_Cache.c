#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int key;
    int value;
    struct Node* prev;
    struct Node* next;
} Node;

typedef struct HashEntry {
    int key;
    Node* node;
    struct HashEntry* next;
} HashEntry;

typedef struct {
    int capacity;
    int count;
    int hashSize;
    HashEntry** table;
    Node* head;
    Node* tail;
} LRUCache;

Node* createNode(int key, int value) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = key;
    node->value = value;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

void addToHead(LRUCache* cache, Node* node) {
    node->next = cache->head->next;
    node->next->prev = node;
    cache->head->next = node;
    node->prev = cache->head;
}

void removeNode(Node* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void moveToHead(LRUCache* cache, Node* node) {
    removeNode(node);
    addToHead(cache, node);
}

int hashFunc(int key, int hashSize) {
    int h = key % hashSize;
    return h < 0 ? h + hashSize : h;
}

HashEntry* findHash(LRUCache* cache, int key) {
    int idx = hashFunc(key, cache->hashSize);
    HashEntry* curr = cache->table[idx];
    while (curr) {
        if (curr->key == key) return curr;
        curr = curr->next;
    }
    return NULL;
}

void insertHash(LRUCache* cache, int key, Node* node) {
    int idx = hashFunc(key, cache->hashSize);
    HashEntry* entry = (HashEntry*)malloc(sizeof(HashEntry));
    entry->key = key;
    entry->node = node;
    entry->next = cache->table[idx];
    cache->table[idx] = entry;
}

void removeHash(LRUCache* cache, int key) {
    int idx = hashFunc(key, cache->hashSize);
    HashEntry* curr = cache->table[idx];
    HashEntry* prev = NULL;
    while (curr) {
        if (curr->key == key) {
            if (prev == NULL) {
                cache->table[idx] = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

LRUCache* lRUCacheCreate(int capacity) {
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->count = 0;
    cache->hashSize = capacity * 2 + 1;
    cache->table = (HashEntry**)calloc(cache->hashSize, sizeof(HashEntry*));
    cache->head = createNode(-1, -1);
    cache->tail = createNode(-1, -1);
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;
    return cache;
}

int lRUCacheGet(LRUCache* cache, int key) {
    HashEntry* entry = findHash(cache, key);
    if (!entry) return -1;
    
    Node* node = entry->node;
    moveToHead(cache, node);
    return node->value;
}

void lRUCachePut(LRUCache* cache, int key, int value) {
    HashEntry* entry = findHash(cache, key);
    
    if (entry) {
        Node* node = entry->node;
        node->value = value;
        moveToHead(cache, node);
    } else {
        Node* node = createNode(key, value);
        insertHash(cache, key, node);
        addToHead(cache, node);
        cache->count++;
        
        if (cache->count > cache->capacity) {
            Node* lruNode = cache->tail->prev;
            removeNode(lruNode);
            removeHash(cache, lruNode->key);
            free(lruNode);
            cache->count--;
        }
    }
}

void lRUCacheFree(LRUCache* cache) {
    Node* currNode = cache->head;
    while (currNode) {
        Node* nextNode = currNode->next;
        free(currNode);
        currNode = nextNode;
    }
    
    for (int i = 0; i < cache->hashSize; i++) {
        HashEntry* currEntry = cache->table[i];
        while (currEntry) {
            HashEntry* nextEntry = currEntry->next;
            free(currEntry);
            currEntry = nextEntry;
        }
    }
    free(cache->table);
    free(cache);
}

int main() {
    LRUCache* cache = lRUCacheCreate(2);

    lRUCachePut(cache, 1, 1);
    lRUCachePut(cache, 2, 2);
    
    printf("get(1): %d\n", lRUCacheGet(cache, 1));
    
    lRUCachePut(cache, 3, 3);
    
    printf("get(2): %d\n", lRUCacheGet(cache, 2));
    
    lRUCachePut(cache, 4, 4);
    
    printf("get(1): %d\n", lRUCacheGet(cache, 1));
    printf("get(3): %d\n", lRUCacheGet(cache, 3));
    printf("get(4): %d\n", lRUCacheGet(cache, 4));

    lRUCacheFree(cache);
    return 0;
}
