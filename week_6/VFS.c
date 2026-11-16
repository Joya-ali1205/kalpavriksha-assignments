#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024

struct FreeBlock {
    int index;
    struct FreeBlock *prev;
    struct FreeBlock *next;
};

struct FileNode {
    char name[51];
    int isDirectory;
    int contentSize;
    struct FileNode *parent;
    struct FileNode *children;
    struct FileNode *next;
    struct FileNode *prev;
    int blockPointers[NUM_BLOCKS];
    int blockCount;
};

unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
struct FreeBlock *freeHead = NULL;
struct FreeBlock *freeTail = NULL;
struct FileNode *root = NULL;
struct FileNode *cwd = NULL;

void initFreeList() {
    for(int i = 0; i < NUM_BLOCKS; i++) {
        struct FreeBlock *newNode = malloc(sizeof(struct FreeBlock));
        newNode->index = i;
        newNode->next = NULL;
        newNode->prev = freeTail;
        if(freeHead == NULL) {
            freeHead = newNode;
        }
        else {
            freeTail->next = newNode;
        }
        freeTail = newNode;
    }
}

void initRoot() {
    root = malloc(sizeof(struct FileNode));
    strcpy(root->name, "/");
    root->isDirectory = 1;
    root->parent = NULL;
    root->children = NULL;
    root->next = NULL;
    root->prev = NULL;
    root->blockCount = 0;
    cwd = root;
    root->contentSize = 0;
}

void init_vfs() {
    memset(virtualDisk, 0, sizeof(virtualDisk));
    initFreeList();
    initRoot();
    printf("Compact VFS - ready. Type 'exit' to quit.\n");
}

struct FileNode* searchEntry(char name[]) {
    if(cwd->children == NULL) {
        return NULL;
    }
    struct FileNode *temp = cwd->children;
    do {
        if(strcmp(temp->name, name) == 0) {
            return temp;
        }
        temp = temp->next;
    } while(temp != cwd->children);
    return NULL;
}

void mkdir(char name[]) {
    if(searchEntry(name) != NULL) {
        printf("Entry already exists.\n");
        return;
    }
    struct FileNode *newNode = malloc(sizeof(struct FileNode));
    if(newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    if (strlen(name) == 0 || strlen(name) > 50) {
        printf("Name must contain 1 to 50 characters.\n");
        return;
    }
    strcpy(newNode->name, name);

    newNode->isDirectory = 1;
    newNode->parent = cwd;
    newNode->children = NULL;
    newNode->blockCount = 0;
    newNode->contentSize = 0;
    if(cwd->children == NULL) {
        newNode->next = newNode;
        newNode->prev = newNode;
        cwd->children = newNode;
    }
    else {
        struct FileNode *head = cwd->children;
        struct FileNode *tail = head->prev;
        newNode->next = head;
        newNode->prev = tail;
        tail->next = newNode;
        head->prev = newNode;
    }
    printf("Directory '%s' created successfully.\n", name);
}

void create(char name[]) {
    if(searchEntry(name) != NULL) {
        printf("Entry already exists.\n");
        return;
    }
    struct FileNode *newNode = malloc(sizeof(struct FileNode));
    if(newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    if (strlen(name) == 0 || strlen(name) > 50) {
        printf("Name must contain 1 to 50 characters.\n");
        return;
    }
    strcpy(newNode->name, name);

    newNode->isDirectory = 0;
    newNode->parent = cwd;
    newNode->children = NULL;
    newNode->blockCount = 0;
    newNode->contentSize = 0;
    if(cwd->children == NULL) {
        newNode->next = newNode;
        newNode->prev = newNode;
        cwd->children = newNode;
    }
    else {
        struct FileNode *head = cwd->children;
        struct FileNode *tail = head->prev;
        newNode->next = head;
        newNode->prev = tail;
        tail->next = newNode;
        head->prev = newNode;
    }
    printf("File '%s' created successfully.\n", name);
}

void ls() {
    if(cwd->children == NULL) {
        printf("(empty)\n");
        return;
    }
    struct FileNode *temp = cwd->children;
    do {
        if(temp->isDirectory == 1) {
            printf("%s/\n", temp->name);
        }
        else {
            printf("%s\n", temp->name);
        }
        temp = temp->next;
    } while(temp != cwd->children);
}


void cd(char name[]) {
    if(strcmp(name, "..") == 0) {
        if(cwd->parent == NULL) {
            printf("Already in root directory.\n");
            return;
        }
        cwd = cwd->parent;
        printf("Moved to parent directory.\n");
        return;
    }
    struct FileNode *temp = searchEntry(name);
    if(temp == NULL || temp->isDirectory == 0) {
        printf("Directory not found.\n");
        return;
    }
    cwd = temp;
    printf("Moved to directory '%s'.\n", name);
}

void getCurrentPath(char path[]) {
    char *names[100];
    int count = 0;
    struct FileNode *temp = cwd;
    while (temp != NULL && temp != root) {
        names[count] = temp->name;
        count++;
        temp = temp->parent;
    }
    strcpy(path, "");
    for (int i = count - 1; i >= 0; i--) {
        strcat(path, names[i]);
        if (i != 0) {
            strcat(path, "/");
        }
    }
}

void pwd() {
    char path[1000];
    getCurrentPath(path);
    if (cwd == root) {
        printf("/\n");
    }
    else {
        printf("/%s\n", path);
    }
}

void writeFile(char filename[], char content[]) {
    struct FileNode *file = searchEntry(filename);

    if (file == NULL) {
        printf("File not found.\n");
        return;
    }
    if (file->isDirectory == 1) {
        printf("Cannot write into a directory.\n");
        return;
    }
    if (file->blockCount > 0) {
        printf("File already contains data.\n");
        return;
    }

    int contentSize = strlen(content);
    int requiredBlocks = (contentSize + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int availableBlocks = 0;
    struct FreeBlock *temp = freeHead;

    while (temp != NULL) {
        availableBlocks++;
        temp = temp->next;
    }
    if (requiredBlocks > availableBlocks) {
        printf("Not enough disk space.\n");
        return;
    }
    int offset = 0;

    for (int i = 0; i < requiredBlocks; i++) {
        int blockIndex = freeHead->index;
        file->blockPointers[i] = blockIndex;

        struct FreeBlock *oldHead = freeHead;
        freeHead = freeHead->next;

        if (freeHead != NULL) {
            freeHead->prev = NULL;
        }
        else {
            freeTail = NULL;
        }

        free(oldHead);

        memset(virtualDisk[blockIndex], 0, BLOCK_SIZE);
        int bytesToCopy = contentSize - offset;
        if (bytesToCopy > BLOCK_SIZE) {
            bytesToCopy = BLOCK_SIZE;
        }
        memcpy(
            virtualDisk[blockIndex],
            content + offset,
            bytesToCopy
        );

        offset += bytesToCopy;
    }
    file->blockCount = requiredBlocks;
    file->contentSize = contentSize;
    printf("Data written successfully.\n");
}

void readFile(char filename[]) {
    struct FileNode *file = searchEntry(filename);
    if (file == NULL) {
        printf("File not found.\n");
        return;
    }
    if (file->isDirectory == 1) {
        printf("Cannot read a directory.\n");
        return;
    }
    if (file->contentSize == 0) {
        printf("File is empty.\n");
        return;
    }
    int remaining = file->contentSize;
    for (int i = 0; i < file->blockCount; i++) {
        int blockIndex = file->blockPointers[i];
        int bytesToRead = remaining;
        if (bytesToRead > BLOCK_SIZE) {
            bytesToRead = BLOCK_SIZE;
        }
        fwrite(
            virtualDisk[blockIndex],
            1,
            bytesToRead,
            stdout
        );
        remaining -= bytesToRead;
    }
    printf("\n");
}

void freeBlock(int index) {
    struct FreeBlock *newNode = (struct FreeBlock*)malloc(sizeof(struct FreeBlock));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    newNode->index = index;
    newNode->next = NULL;
    newNode->prev = freeTail;
    if (freeHead == NULL) {
        freeHead = newNode;
        freeTail = newNode;
    }
    else {
        freeTail->next = newNode;
        freeTail = newNode;
    }
}

void deleteFile(char filename[]) {
    struct FileNode *file = searchEntry(filename);
    if (file == NULL) {
        printf("File not found.\n");
        return;
    }
    if (file->isDirectory == 1) {
        printf("Cannot delete a directory using delete.\n");
        return;
    }
    for (int i = 0; i < file->blockCount; i++) {
        int blockIndex = file->blockPointers[i];
        freeBlock(blockIndex);
    }
    if (file->next == file) {

        cwd->children = NULL;
    }
    else {
        file->prev->next = file->next;
        file->next->prev = file->prev;
        if (cwd->children == file) {
            cwd->children = file->next;
        }
    }
    free(file);
    printf("File deleted successfully.\n");
}

void rmdir(char name[]) {
    struct FileNode *dir = searchEntry(name);
    if (dir == NULL) {
        printf("Directory not found.\n");
        return;
    }
    if (dir->isDirectory == 0) {
        printf("This is a file, not a directory.\n");
        return;
    }
    if (dir->children != NULL) {
        printf("Directory is not empty.\n");
        return;
    }
    if (dir->next == dir) {
        cwd->children = NULL;
    }
    else {
        dir->prev->next = dir->next;
        dir->next->prev = dir->prev;
        if (cwd->children == dir) {
            cwd->children = dir->next;
        }
    }
    free(dir);
    printf("Directory removed successfully.\n");
}

void df() {
    int totalSpace = NUM_BLOCKS * BLOCK_SIZE;
    int freeBlocks = 0;
    struct FreeBlock *temp = freeHead;

    while (temp != NULL) {
        freeBlocks++;
        temp = temp->next;
    }
    int usedBlocks = NUM_BLOCKS-freeBlocks;
    int usedSpace = usedBlocks*BLOCK_SIZE;
    int freeSpace = freeBlocks*BLOCK_SIZE;
    float usage = ((float)usedSpace/totalSpace)*100;

    printf("Total Blocks     : %d\n", NUM_BLOCKS);
    printf("Used Blocks      : %d\n", usedBlocks);
    printf("Free Blocks      : %d\n", freeBlocks);
    printf("Disk Usage : %.2f%%\n", usage);
}

void freeDirectory(struct FileNode *dir) {
    if (dir == NULL) {
        return;
    }
    struct FileNode *child = dir->children;
    if (child != NULL) {
        struct FileNode *current = child;
        do {
            struct FileNode *next = current->next;
            if (current->isDirectory == 1) {
                freeDirectory(current);
            }
            free(current);
            current = next;
        } while (current != child);
    }
    dir->children = NULL;
}

void cleanupVFS() {
    freeDirectory(root);
    free(root);
    root = NULL;
    cwd = NULL;
    struct FreeBlock *temp = freeHead;
    while (temp != NULL) {
        struct FreeBlock *next = temp->next;
        free(temp);
        temp = next;
    }
    freeHead = NULL;
    freeTail = NULL;
    printf("Memory released.\n");
}

int main() {
    char command[600];
    char arg1[100];
    char content[512];
    init_vfs();

    while (1){
        char currentPath[1000];
        getCurrentPath(currentPath);
        if (cwd == root) {
            printf("/> ");
        }
        else {
            printf("%s> ", currentPath);
        }
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) {
            break;
        }
        else if (strcmp(command, "ls") == 0) {
            ls();
        }
        else if (strcmp(command, "pwd") == 0) {
            pwd();
        }
        else if (strcmp(command, "df") == 0) {
            df();
        }
        else if (strncmp(command, "write ", 6) == 0) {
            char filename[100];
            char *start = strchr(command + 6, '"');
            if (start == NULL) {
                printf("Use: write filename \"content\"\n");
            }
            else {
                *start = '\0';
                sscanf(command + 6, "%99s", filename);
                char *end = strrchr(start + 1, '"');
                if (end == NULL) {
                    printf("Missing closing quotation mark.\n");
                }
                else {
                    *end = '\0';

                    writeFile(filename, start + 1);
                }
            }
        }
        else if (sscanf(command, "%99s %99s", arg1, content) >= 1) {
            if (strcmp(arg1, "mkdir") == 0) {
                if (sscanf(command, "%*s %99s", content) == 1)
                    mkdir(content);
                else
                    printf("Usage: mkdir dirname\n");
            }
            else if (strcmp(arg1, "create") == 0) {
                if (sscanf(command, "%*s %99s", content) == 1)
                    create(content);
                else
                    printf("Usage: create filename\n");
            }
            else if (strcmp(arg1, "cd") == 0) {
                if (sscanf(command, "%*s %99s", content) == 1)
                    cd(content);
                else
                    printf("Usage: cd dirname\n");
            }
            else if (strcmp(arg1, "read") == 0) {
                if (sscanf(command, "%*s %99s", content) == 1)
                    readFile(content);
                else
                    printf("Usage: read filename\n");
            }
            else if (strcmp(arg1, "delete") == 0) {
                if (sscanf(command, "%*s %99s", content) == 1)
                    deleteFile(content);
                else
                    printf("Usage: delete filename\n");
            }
            else if (strcmp(arg1, "rmdir") == 0) {
                if (sscanf(command, "%*s %99s", content) == 1)
                    rmdir(content);
                else
                    printf("Usage: rmdir dirname\n");
            }
            else {
                printf("Invalid command.\n");
            }
        }
    }
    cleanupVFS();
    printf("Exiting program...\n");
    return 0;
}