#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 50
#define LINE_SIZE 8192

typedef struct FreeBlock {
    int index;
    struct FreeBlock *next, *prev;
} FreeBlock;

typedef struct FileNode {
    char name[MAX_NAME + 1];
    int isDir;
    size_t size;
    int blockCount, *blocks;
    struct FileNode *parent, *next, *children;
} FileNode;

static unsigned char disk[NUM_BLOCKS][BLOCK_SIZE];
static FreeBlock *freeHead, *freeTail;
static int freeCount;
static FileNode *root, *cwd;

void addFreeBlock(int index) {
    FreeBlock *b = malloc(sizeof(*b));
    if (!b) { perror("malloc"); exit(1); }

    b->index = index;
    b->next = NULL;
    b->prev = freeTail;

    if (freeTail) freeTail->next = b;
    else freeHead = b;

    freeTail = b;
    freeCount++;
}

int getFreeBlock(void) {
    if (!freeHead) return -1;

    FreeBlock *b = freeHead;
    int index = b->index;

    freeHead = b->next;
    if (freeHead) freeHead->prev = NULL;
    else freeTail = NULL;

    free(b);
    freeCount--;
    return index;
}

void initDisk(void) {
    memset(disk, 0, sizeof(disk));
    for (int i = 0; i < NUM_BLOCKS; i++)
        addFreeBlock(i);
}

FileNode *newNode(const char *name, int isDir, FileNode *parent) {
    FileNode *n = calloc(1, sizeof(*n));
    if (!n) { perror("calloc"); exit(1); }

    strncpy(n->name, name, MAX_NAME);
    n->isDir = isDir;
    n->parent = parent;
    return n;
}

FileNode *findChild(FileNode *dir, const char *name) {
    if (!dir->children) return NULL;

    FileNode *p = dir->children;
    do {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    } while (p != dir->children);

    return NULL;
}

void insertChild(FileNode *dir, FileNode *node) {
    if (!dir->children) {
        dir->children = node;
        node->next = node;
        return;
    }

    FileNode *last = dir->children;
    while (last->next != dir->children)
        last = last->next;

    last->next = node;
    node->next = dir->children;
}

void removeChild(FileNode *dir, FileNode *node) {
    if (node->next == node) {
        dir->children = NULL;
    } else {
        FileNode *p = dir->children;
        while (p->next != node) p = p->next;

        p->next = node->next;
        if (dir->children == node)
            dir->children = node->next;
    }
    node->next = NULL;
}

int validName(const char *name) {
    size_t len = strlen(name);
    return len > 0 && len <= MAX_NAME &&
           !strchr(name, '/') &&
           strcmp(name, ".") != 0 &&
           strcmp(name, "..") != 0;
}

void addEntry(const char *name, int isDir) {
    if (!validName(name)) {
        printf("Error: invalid name (1-%d characters, no '/').\n", MAX_NAME);
        return;
    }
    if (findChild(cwd, name)) {
        printf("Error: '%s' already exists.\n", name);
        return;
    }

    insertChild(cwd, newNode(name, isDir, cwd));
    printf("%s '%s' created successfully.\n",
           isDir ? "Directory" : "File", name);
}

void releaseBlocks(FileNode *f) {
    for (int i = 0; i < f->blockCount; i++)
        addFreeBlock(f->blocks[i]);

    free(f->blocks);
    f->blocks = NULL;
    f->blockCount = 0;
    f->size = 0;
}

void writeFile(const char *name, const char *data) {
    FileNode *f = findChild(cwd, name);
    if (!f) { printf("Error: file '%s' not found.\n", name); return; }
    if (f->isDir) { printf("Error: '%s' is a directory.\n", name); return; }

    size_t len = strlen(data);
    int need = (len + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (need > freeCount + f->blockCount) {
        printf("Error: not enough disk space.\n");
        return;
    }

    int *newBlocks = need ? malloc(need * sizeof(int)) : NULL;
    if (need && !newBlocks) { perror("malloc"); return; }

    releaseBlocks(f);

    for (int i = 0; i < need; i++) {
        int index = getFreeBlock();
        newBlocks[i] = index;

        size_t offset = (size_t)i * BLOCK_SIZE;
        size_t chunk = len - offset;
        if (chunk > BLOCK_SIZE) chunk = BLOCK_SIZE;

        memset(disk[index], 0, BLOCK_SIZE);
        memcpy(disk[index], data + offset, chunk);
    }

    f->blocks = newBlocks;
    f->blockCount = need;
    f->size = len;
    printf("Data written successfully (size=%zu bytes).\n", len);
}

void readFile(const char *name) {
    FileNode *f = findChild(cwd, name);
    if (!f) { printf("Error: file '%s' not found.\n", name); return; }
    if (f->isDir) { printf("Error: '%s' is a directory.\n", name); return; }
    if (!f->size) { printf("(empty)\n"); return; }

    size_t left = f->size;
    for (int i = 0; i < f->blockCount && left; i++) {
        size_t chunk = left < BLOCK_SIZE ? left : BLOCK_SIZE;
        fwrite(disk[f->blocks[i]], 1, chunk, stdout);
        left -= chunk;
    }
    putchar('\n');
}

void deleteFile(const char *name) {
    FileNode *f = findChild(cwd, name);
    if (!f) { printf("Error: file '%s' not found.\n", name); return; }
    if (f->isDir) { printf("Error: use rmdir for directories.\n"); return; }

    removeChild(cwd, f);
    releaseBlocks(f);
    free(f);
    printf("File deleted successfully.\n");
}

void removeDir(const char *name) {
    FileNode *d = findChild(cwd, name);
    if (!d) { printf("Error: directory '%s' not found.\n", name); return; }
    if (!d->isDir) { printf("Error: '%s' is a file.\n", name); return; }
    if (d->children) { printf("Error: directory is not empty.\n"); return; }

    removeChild(cwd, d);
    free(d);
    printf("Directory removed successfully.\n");
}

void listFiles(void) {
    if (!cwd->children) { printf("(empty)\n"); return; }

    FileNode *p = cwd->children;
    do {
        printf("%s%s\n", p->name, p->isDir ? "/" : "");
        p = p->next;
    } while (p != cwd->children);
}

void buildPath(FileNode *n, char *path, size_t size) {
    if (n == root) {
        snprintf(path, size, "/");
        return;
    }

    char parentPath[LINE_SIZE];
    buildPath(n->parent, parentPath, sizeof(parentPath));

    if (strcmp(parentPath, "/") == 0)
        snprintf(path, size, "/%s", n->name);
    else
        snprintf(path, size, "%s/%s", parentPath, n->name);
}

void changeDir(const char *name) {
    if (strcmp(name, "/") == 0) cwd = root;
    else if (strcmp(name, "..") == 0) {
        if (cwd != root) cwd = cwd->parent;
    } else if (strcmp(name, ".") != 0) {
        FileNode *d = findChild(cwd, name);
        if (!d) { printf("Error: '%s' not found.\n", name); return; }
        if (!d->isDir) { printf("Error: '%s' is not a directory.\n", name); return; }
        cwd = d;
    }

    char path[LINE_SIZE];
    buildPath(cwd, path, sizeof(path));
    printf("Moved to %s\n", path);
}

void diskUsage(void) {
    int used = NUM_BLOCKS - freeCount;
    printf("Total Blocks: %d\nUsed Blocks: %d\nFree Blocks: %d\n",
           NUM_BLOCKS, used, freeCount);
    printf("Disk Usage: %.2f%%\n", 100.0 * used / NUM_BLOCKS);
}

void destroyTree(FileNode *n) {
    if (!n) return;

    if (n->isDir && n->children) {
        FileNode *start = n->children, *p = start;
        do {
            FileNode *next = p->next;
            destroyTree(p);
            p = next;
        } while (p != start);
    }

    free(n->blocks);
    free(n);
}

void cleanup(void) {
    destroyTree(root);

    while (freeHead) {
        FreeBlock *next = freeHead->next;
        free(freeHead);
        freeHead = next;
    }

    root = cwd = NULL;
    freeTail = NULL;
    freeCount = 0;
}

char *skipSpaces(char *p) {
    while (*p == ' ' || *p == '\t') p++;
    return p;
}

char *getToken(char **p, char *out, size_t size) {
    *p = skipSpaces(*p);
    size_t i = 0;

    while (**p && **p != ' ' && **p != '\t') {
        if (i + 1 < size) out[i++] = **p;
        (*p)++;
    }
    out[i] = '\0';
    return *p;
}

int main(void) {
    initDisk();
    root = newNode("/", 1, NULL);
    cwd = root;

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    char line[LINE_SIZE], cmd[32], arg[LINE_SIZE];

    while (1) {
        char path[LINE_SIZE];
        buildPath(cwd, path, sizeof(path));
        printf("%s > ", path);
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) break;

        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        getToken(&p, cmd, sizeof(cmd));
        if (!cmd[0]) continue;

        getToken(&p, arg, sizeof(arg));

        if (strcmp(cmd, "exit") == 0) break;
        else if (strcmp(cmd, "mkdir") == 0) addEntry(arg, 1);
        else if (strcmp(cmd, "create") == 0) addEntry(arg, 0);
        else if (strcmp(cmd, "ls") == 0) listFiles();
        else if (strcmp(cmd, "pwd") == 0) { buildPath(cwd, path, sizeof(path)); puts(path); }
        else if (strcmp(cmd, "df") == 0) diskUsage();
        else if (strcmp(cmd, "cd") == 0) changeDir(arg);
        else if (strcmp(cmd, "read") == 0) readFile(arg);
        else if (strcmp(cmd, "delete") == 0) deleteFile(arg);
        else if (strcmp(cmd, "rmdir") == 0) removeDir(arg);
        else if (strcmp(cmd, "write") == 0) {
            p = skipSpaces(p);
            size_t len = strlen(p);
            if (len && p[len - 1] == '\n') p[--len] = '\0';
            if (len >= 2 && p[0] == '"' && p[len - 1] == '"') {
                p[len - 1] = '\0';
                p++;
            }
            writeFile(arg, p);
        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }

    cleanup();
    printf("Memory released. Exiting program...\n");
    return 0;
}
