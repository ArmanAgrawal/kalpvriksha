#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME_LEN 50
#define MAX_INPUT_LEN 65536
#define MAX_DEPTH 256

typedef struct FreeBlock {
    int blockIndex;
    struct FreeBlock *nextBlock;
    struct FreeBlock *prevBlock;
} FreeBlock;

typedef struct FileNode {
    char nodeName[MAX_NAME_LEN + 1];
    int isFolder;
    struct FileNode *parentDir;
    struct FileNode *firstChild;
    struct FileNode *nextSibling;
    struct FileNode *prevSibling;
    int blockPointers[NUM_BLOCKS];
    int allocatedBlocks;
    int contentSize;
} FileNode;

char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
FreeBlock *freeListHead = NULL;
FreeBlock *freeListTail = NULL;
int totalFreeBlocks = 0;

FileNode *rootDir = NULL;
FileNode *currentDir = NULL;

char *trimSpaces(char *str) {
    if (str == NULL) {
        return NULL;
    }
    while (isspace((unsigned char)*str)) {
        str++;
    }
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
    return str;
}

FileNode *allocateFileNode(const char *name, int isFolder, FileNode *parent) {
    FileNode *newNode = (FileNode *)malloc(sizeof(FileNode));
    if (newNode == NULL) {
        return NULL;
    }
    strncpy(newNode->nodeName, name, MAX_NAME_LEN);
    newNode->nodeName[MAX_NAME_LEN] = '\0';
    newNode->isFolder = isFolder;
    newNode->parentDir = parent;
    newNode->firstChild = NULL;
    newNode->nextSibling = newNode;
    newNode->prevSibling = newNode;
    newNode->allocatedBlocks = 0;
    newNode->contentSize = 0;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        newNode->blockPointers[i] = -1;
    }
    return newNode;
}

void pushFreeBlockToTail(int index) {
    if (index < 0 || index >= NUM_BLOCKS) {
        return;
    }
    FreeBlock *newBlock = (FreeBlock *)malloc(sizeof(FreeBlock));
    if (newBlock == NULL) {
        return;
    }
    newBlock->blockIndex = index;
    newBlock->nextBlock = NULL;
    newBlock->prevBlock = freeListTail;

    if (freeListTail != NULL) {
        freeListTail->nextBlock = newBlock;
    } else {
        freeListHead = newBlock;
    }
    freeListTail = newBlock;
    totalFreeBlocks++;
}

int popFreeBlockFromHead() {
    if (freeListHead == NULL || totalFreeBlocks <= 0) {
        return -1;
    }
    FreeBlock *headNode = freeListHead;
    int index = headNode->blockIndex;

    freeListHead = freeListHead->nextBlock;
    if (freeListHead != NULL) {
        freeListHead->prevBlock = NULL;
    } else {
        freeListTail = NULL;
    }
    free(headNode);
    totalFreeBlocks--;
    memset(virtualDisk[index], 0, BLOCK_SIZE);
    return index;
}

void setupVirtualFileSystem() {
    if (NUM_BLOCKS < 1 || NUM_BLOCKS > 5000) {
        printf("Configuration error: NUM_BLOCKS must be between 1 and 5000.\n");
        exit(EXIT_FAILURE);
    }
    memset(virtualDisk, 0, sizeof(virtualDisk));
    for (int i = 0; i < NUM_BLOCKS; i++) {
        pushFreeBlockToTail(i);
    }
    if (totalFreeBlocks != NUM_BLOCKS) {
        printf("Initialization error: Free block list setup incomplete.\n");
        exit(EXIT_FAILURE);
    }
    rootDir = allocateFileNode("/", 1, NULL);
    if (rootDir == NULL) {
        printf("Fatal error: Could not create root directory.\n");
        exit(EXIT_FAILURE);
    }
    currentDir = rootDir;
}

void linkChildNode(FileNode *parent, FileNode *child) {
    if (parent == NULL || child == NULL) {
        return;
    }
    if (parent->firstChild == NULL) {
        parent->firstChild = child;
        child->nextSibling = child;
        child->prevSibling = child;
    } else {
        FileNode *lastChild = parent->firstChild->prevSibling;
        child->nextSibling = parent->firstChild;
        child->prevSibling = lastChild;
        lastChild->nextSibling = child;
        parent->firstChild->prevSibling = child;
    }
}

void unlinkChildNode(FileNode *parent, FileNode *target) {
    if (parent == NULL || parent->firstChild == NULL || target == NULL) {
        return;
    }
    if (target->nextSibling == target) {
        parent->firstChild = NULL;
    } else {
        target->prevSibling->nextSibling = target->nextSibling;
        target->nextSibling->prevSibling = target->prevSibling;
        if (parent->firstChild == target) {
            parent->firstChild = target->nextSibling;
        }
    }
    target->nextSibling = target;
    target->prevSibling = target;
}

FileNode *searchChildNode(FileNode *directory, const char *name) {
    if (directory == NULL || directory->firstChild == NULL || name == NULL) {
        return NULL;
    }
    FileNode *iterator = directory->firstChild;
    do {
        if (strcmp(iterator->nodeName, name) == 0) {
            return iterator;
        }
        iterator = iterator->nextSibling;
    } while (iterator != directory->firstChild);
    return NULL;
}

int validateEntryName(const char *name) {
    if (name == NULL || *name == '\0') {
        printf("Error: Name cannot be empty.\n");
        return 0;
    }
    int length = strlen(name);
    if (length > MAX_NAME_LEN) {
        printf("Error: Name exceeds %d characters.\n", MAX_NAME_LEN);
        return 0;
    }
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 || strcmp(name, "/") == 0) {
        printf("Error: '%s' is a reserved system name.\n", name);
        return 0;
    }
    for (int i = 0; i < length; i++) {
        unsigned char ch = (unsigned char)name[i];
        if (ch == '/' || ch == '\\' || ch == ':' || ch == '*' ||
            ch == '?' || ch == '"' || ch == '<' || ch == '>' ||
            ch == '|' || isspace(ch) || iscntrl(ch)) {
            printf("Error: Name contains invalid characters or whitespace.\n");
            return 0;
        }
    }
    return 1;
}

int getDirectoryDepth(FileNode *dir) {
    int depth = 0;
    FileNode *curr = dir;
    while (curr != NULL && curr != rootDir) {
        depth++;
        curr = curr->parentDir;
    }
    return depth;
}

void releaseAllFileBlocks(FileNode *fileNode) {
    if (fileNode == NULL || fileNode->isFolder) {
        return;
    }
    for (int i = 0; i < fileNode->allocatedBlocks; i++) {
        if (fileNode->blockPointers[i] != -1) {
            pushFreeBlockToTail(fileNode->blockPointers[i]);
            fileNode->blockPointers[i] = -1;
        }
    }
    fileNode->allocatedBlocks = 0;
    fileNode->contentSize = 0;
}

FileNode *navigateToPath(const char *pathStr) {
    if (pathStr == NULL || *pathStr == '\0') {
        return currentDir;
    }
    if (strcmp(pathStr, "/") == 0) {
        return rootDir;
    }
    char tempPath[4096];
    if (strlen(pathStr) >= sizeof(tempPath)) {
        return NULL;
    }
    strcpy(tempPath, pathStr);

    FileNode *tracker = (tempPath[0] == '/') ? rootDir : currentDir;
    char *segment = strtok(tempPath, "/");

    while (segment != NULL) {
        if (strcmp(segment, ".") == 0) {
        } else if (strcmp(segment, "..") == 0) {
            if (tracker->parentDir != NULL) {
                tracker = tracker->parentDir;
            }
        } else {
            FileNode *found = searchChildNode(tracker, segment);
            if (found == NULL || !found->isFolder) {
                return NULL;
            }
            tracker = found;
        }
        segment = strtok(NULL, "/");
    }
    return tracker;
}

void handleMkdir(const char *name) {
    if (!validateEntryName(name)) {
        return;
    }
    if (getDirectoryDepth(currentDir) >= MAX_DEPTH) {
        printf("Error: Maximum directory nesting depth reached.\n");
        return;
    }
    if (searchChildNode(currentDir, name) != NULL) {
        printf("Error: '%s' already exists in the current directory.\n", name);
        return;
    }
    FileNode *folderNode = allocateFileNode(name, 1, currentDir);
    if (folderNode == NULL) {
        printf("Error: System out of memory.\n");
        return;
    }
    linkChildNode(currentDir, folderNode);
    printf("Directory '%s' created successfully.\n", name);
}

void handleCreate(const char *name) {
    if (!validateEntryName(name)) {
        return;
    }
    if (searchChildNode(currentDir, name) != NULL) {
        printf("Error: '%s' already exists in the current directory.\n", name);
        return;
    }
    FileNode *fileNode = allocateFileNode(name, 0, currentDir);
    if (fileNode == NULL) {
        printf("Error: System out of memory.\n");
        return;
    }
    linkChildNode(currentDir, fileNode);
    printf("File '%s' created successfully.\n", name);
}

void handleWrite(const char *fileName, char *rawText) {
    if (fileName == NULL || *fileName == '\0' || rawText == NULL || *rawText == '\0') {
        printf("Usage: write <filename> \"<content>\"\n");
        return;
    }

    int textLen = strlen(rawText);
    if (textLen >= 2 && ((rawText[0] == '"' && rawText[textLen - 1] == '"') ||
                          (rawText[0] == '\'' && rawText[textLen - 1] == '\''))) {
        rawText[textLen - 1] = '\0';
        rawText++;
        textLen -= 2;
    } else if (rawText[0] == '"' || rawText[0] == '\'' ||
               rawText[textLen - 1] == '"' || rawText[textLen - 1] == '\'') {
        printf("Error: Mismatched or unclosed quotes in content.\n");
        return;
    }

    FileNode *targetFile = searchChildNode(currentDir, fileName);
    if (targetFile == NULL) {
        printf("Error: File '%s' does not exist.\n", fileName);
        return;
    }
    if (targetFile->isFolder) {
        printf("Error: '%s' is a directory, cannot write data to it.\n", fileName);
        return;
    }

    int requiredBlocks = (textLen == 0) ? 0 : (int)((textLen + BLOCK_SIZE - 1) / BLOCK_SIZE);
    int availableForFile = totalFreeBlocks + targetFile->allocatedBlocks;

    if (requiredBlocks > availableForFile) {
        printf("Error: Disk full. Needed %d blocks, only %d available.\n",
               requiredBlocks, availableForFile);
        return;
    }

    releaseAllFileBlocks(targetFile);

    int offset = 0;
    for (int i = 0; i < requiredBlocks; i++) {
        int blkIdx = popFreeBlockFromHead();
        if (blkIdx == -1) {
            releaseAllFileBlocks(targetFile);
            printf("Error: Unexpected block allocation failure.\n");
            return;
        }
        targetFile->blockPointers[i] = blkIdx;
        targetFile->allocatedBlocks++;
        int copyBytes = (textLen - offset > BLOCK_SIZE) ? BLOCK_SIZE : (textLen - offset);
        memcpy(virtualDisk[blkIdx], rawText + offset, copyBytes);
        offset += copyBytes;
    }
    targetFile->contentSize = textLen;
    printf("Data written successfully (size=%d bytes).\n", textLen);
}

void handleRead(const char *fileName) {
    FileNode *targetFile = searchChildNode(currentDir, fileName);
    if (targetFile == NULL) {
        printf("Error: File '%s' not found.\n", fileName);
        return;
    }
    if (targetFile->isFolder) {
        printf("Error: '%s' is a directory.\n", fileName);
        return;
    }
    if (targetFile->contentSize == 0 || targetFile->allocatedBlocks == 0) {
        printf("(empty file)\n");
        return;
    }
    int bytesLeft = targetFile->contentSize;
    for (int i = 0; i < targetFile->allocatedBlocks; i++) {
        int blkIdx = targetFile->blockPointers[i];
        if (blkIdx < 0 || blkIdx >= NUM_BLOCKS) {
            printf("\nError: Corrupted block pointer detected.\n");
            return;
        }
        int printLen = (bytesLeft > BLOCK_SIZE) ? BLOCK_SIZE : bytesLeft;
        for (int j = 0; j < printLen; j++) {
            putchar(virtualDisk[blkIdx][j]);
        }
        bytesLeft -= printLen;
    }
    printf("\n");
}

void handleDelete(const char *fileName) {
    FileNode *targetFile = searchChildNode(currentDir, fileName);
    if (targetFile == NULL) {
        printf("Error: File '%s' not found.\n", fileName);
        return;
    }
    if (targetFile->isFolder) {
        printf("Error: '%s' is a directory. Use 'rmdir' instead.\n", fileName);
        return;
    }
    releaseAllFileBlocks(targetFile);
    unlinkChildNode(currentDir, targetFile);
    free(targetFile);
    printf("File deleted successfully.\n");
}

void handleRmdir(const char *dirName) {
    if (strcmp(dirName, ".") == 0 || strcmp(dirName, "..") == 0 || strcmp(dirName, "/") == 0) {
        printf("Error: Cannot remove system directory '%s'.\n", dirName);
        return;
    }
    FileNode *targetDir = searchChildNode(currentDir, dirName);
    if (targetDir == NULL) {
        printf("Error: Directory '%s' not found.\n", dirName);
        return;
    }
    if (!targetDir->isFolder) {
        printf("Error: '%s' is a file. Use 'delete' instead.\n", dirName);
        return;
    }
    if (targetDir->firstChild != NULL) {
        printf("Error: Directory '%s' is not empty.\n", dirName);
        return;
    }
    unlinkChildNode(currentDir, targetDir);
    free(targetDir);
    printf("Directory removed successfully.\n");
}

void handleLs(const char *path) {
    FileNode *targetDir = navigateToPath(path);
    if (targetDir == NULL) {
        printf("Error: Directory '%s' not found.\n", path);
        return;
    }
    if (targetDir->firstChild == NULL) {
        printf("(empty)\n");
        return;
    }
    FileNode *iterator = targetDir->firstChild;
    do {
        if (iterator->isFolder) {
            printf("%s/\n", iterator->nodeName);
        } else {
            printf("%s\n", iterator->nodeName);
        }
        iterator = iterator->nextSibling;
    } while (iterator != targetDir->firstChild);
}

void constructFullPath(FileNode *node, char *outPath, int maxLen) {
    if (node == rootDir) {
        strcpy(outPath, "/");
        return;
    }
    FileNode *ancestors[MAX_DEPTH + 1];
    int count = 0;
    FileNode *curr = node;
    while (curr != NULL && curr != rootDir && count <= MAX_DEPTH) {
        ancestors[count++] = curr;
        curr = curr->parentDir;
    }
    outPath[0] = '\0';
    for (int i = count - 1; i >= 0; i--) {
        strncat(outPath, "/", maxLen - strlen(outPath) - 1);
        strncat(outPath, ancestors[i]->nodeName, maxLen - strlen(outPath) - 1);
    }
}

void handleCd(const char *path) {
    FileNode *destination = (path == NULL || *path == '\0') ? rootDir : navigateToPath(path);
    if (destination == NULL) {
        printf("Error: Path '%s' does not exist or is not a directory.\n", path);
        return;
    }
    currentDir = destination;
    char fullPath[4096];
    constructFullPath(currentDir, fullPath, sizeof(fullPath));
    printf("Moved to %s\n", fullPath);
}

void handlePwd() {
    char fullPath[4096];
    constructFullPath(currentDir, fullPath, sizeof(fullPath));
    printf("%s\n", fullPath);
}

void handleDf() {
    int usedBlocks = NUM_BLOCKS - totalFreeBlocks;
    double percentUsed = ((double)usedBlocks / NUM_BLOCKS) * 100.0;
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", totalFreeBlocks);
    printf("Disk Usage: %.2f%%\n", percentUsed);
}

void destroyNodeHierarchy(FileNode *node) {
    if (node == NULL) {
        return;
    }
    if (node->isFolder && node->firstChild != NULL) {
        FileNode *curr = node->firstChild;
        node->firstChild->prevSibling->nextSibling = NULL;
        while (curr != NULL) {
            FileNode *nextNode = curr->nextSibling;
            destroyNodeHierarchy(curr);
            curr = nextNode;
        }
    }
    free(node);
}

void shutdownVirtualFileSystem() {
    destroyNodeHierarchy(rootDir);
    rootDir = NULL;
    currentDir = NULL;
    FreeBlock *curr = freeListHead;
    while (curr != NULL) {
        FreeBlock *next = curr->nextBlock;
        free(curr);
        curr = next;
    }
    freeListHead = NULL;
    freeListTail = NULL;
    totalFreeBlocks = 0;
}

int dispatchCommand(char *line) {
    char *cleaned = trimSpaces(line);
    if (*cleaned == '\0') {
        return 1;
    }

    char *spacePos = strpbrk(cleaned, " \t");
    char *cmd = cleaned;
    char *args = NULL;

    if (spacePos != NULL) {
        *spacePos = '\0';
        args = trimSpaces(spacePos + 1);
    }

    if (strcmp(cmd, "exit") == 0) {
        if (args != NULL && *args != '\0') {
            printf("Error: 'exit' does not accept arguments.\n");
            return 1;
        }
        return 0;
    } else if (strcmp(cmd, "mkdir") == 0) {
        if (args == NULL || *args == '\0' || strpbrk(args, " \t") != NULL) {
            printf("Usage: mkdir <directory_name>\n");
        } else {
            handleMkdir(args);
        }
    } else if (strcmp(cmd, "create") == 0) {
        if (args == NULL || *args == '\0' || strpbrk(args, " \t") != NULL) {
            printf("Usage: create <filename>\n");
        } else {
            handleCreate(args);
        }
    } else if (strcmp(cmd, "write") == 0) {
        if (args == NULL || *args == '\0') {
            printf("Usage: write <filename> \"<content>\"\n");
            return 1;
        }
        char *contentPos = strpbrk(args, " \t");
        if (contentPos == NULL) {
            printf("Usage: write <filename> \"<content>\"\n");
            return 1;
        }
        *contentPos = '\0';
        char *content = trimSpaces(contentPos + 1);
        handleWrite(args, content);
    } else if (strcmp(cmd, "read") == 0) {
        if (args == NULL || *args == '\0' || strpbrk(args, " \t") != NULL) {
            printf("Usage: read <filename>\n");
        } else {
            handleRead(args);
        }
    } else if (strcmp(cmd, "delete") == 0) {
        if (args == NULL || *args == '\0' || strpbrk(args, " \t") != NULL) {
            printf("Usage: delete <filename>\n");
        } else {
            handleDelete(args);
        }
    } else if (strcmp(cmd, "rmdir") == 0) {
        if (args == NULL || *args == '\0' || strpbrk(args, " \t") != NULL) {
            printf("Usage: rmdir <directory_name>\n");
        } else {
            handleRmdir(args);
        }
    } else if (strcmp(cmd, "ls") == 0) {
        if (args != NULL && strpbrk(args, " \t") != NULL) {
            printf("Usage: ls [path]\n");
        } else {
            handleLs(args);
        }
    } else if (strcmp(cmd, "cd") == 0) {
        if (args != NULL && strpbrk(args, " \t") != NULL) {
            printf("Usage: cd [path]\n");
        } else {
            handleCd(args);
        }
    } else if (strcmp(cmd, "pwd") == 0) {
        if (args != NULL && *args != '\0') {
            printf("Error: 'pwd' does not accept arguments.\n");
        } else {
            handlePwd();
        }
    } else if (strcmp(cmd, "df") == 0) {
        if (args != NULL && *args != '\0') {
            printf("Error: 'df' does not accept arguments.\n");
        } else {
            handleDf();
        }
    } else {
        printf("Error: Unknown command '%s'.\n", cmd);
    }
    return 1;
}

int main() {
    char *lineBuffer = (char *)malloc(MAX_INPUT_LEN);
    if (lineBuffer == NULL) {
        printf("Fatal: Failed to allocate command buffer.\n");
        return 1;
    }

    setupVirtualFileSystem();
    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    int isRunning = 1;
    while (isRunning) {
        if (currentDir == rootDir) {
            printf("/ > ");
        } else {
            printf("%s > ", currentDir->nodeName);
        }

        if (fgets(lineBuffer, MAX_INPUT_LEN, stdin) == NULL) {
            printf("\n");
            break;
        }

        int inputLen = strlen(lineBuffer);
        if (inputLen == MAX_INPUT_LEN - 1 && lineBuffer[inputLen - 1] != '\n') {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            printf("Error: Input exceeds maximum command length.\n");
            continue;
        }

        isRunning = dispatchCommand(lineBuffer);
    }

    free(lineBuffer);
    shutdownVirtualFileSystem();
    printf("Memory released. Exiting program...\n");
    return 0;
}