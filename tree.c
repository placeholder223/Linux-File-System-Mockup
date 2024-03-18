#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

int verify_folder(TreeNode *currentNode, char *folderName)
{
    if (!currentNode->content)
        return 0;
    List *list = ((FolderContent *)currentNode->content)->children;
    ListNode *tmp = list->head;
    while (tmp)
    {
        if (tmp->info)
        {
            if (strcmp(folderName, tmp->info->name) == 0)
            {
                if (tmp->info->type == FOLDER_NODE)
                    return 1;
            }
        }
        tmp = tmp->next;
    }
    return 0;
}

int verify_file(TreeNode *currentNode, char *fileName, int print)
{
    if (!currentNode->content)
        return 0;
    List *list = ((FolderContent *)currentNode->content)->children;
    ListNode *tmp = list->head;
    while (tmp)
    {
        if (tmp->info)
        {
            if (strcmp(fileName, tmp->info->name) == 0)
            {
                if (tmp->info->type == FILE_NODE)
                {
                    FileContent *content = tmp->info->content;
                    if (content->text != NULL && print == 1)
                        printf("%s", content->text);
                    return 1;
                }
            }
        }
        tmp = tmp->next;
    }
    return 0;
}

FileTree createFileTree(char *rootFolderName)
{
    FileTree tree;
    tree.root = malloc(sizeof(TreeNode));
    tree.root->parent = NULL;
    tree.root->type = FOLDER_NODE;
    tree.root->name = malloc(strlen(rootFolderName) + 1);
    memcpy(tree.root->name, rootFolderName, strlen(rootFolderName) + 1);
    FolderContent *content = malloc(sizeof(FolderContent));
    content->children = malloc(sizeof(List));
    content->children->head = NULL;
    tree.root->content = malloc(sizeof(FolderContent));
    memcpy(tree.root->content, content, sizeof(FolderContent));
    free(content);
    return tree;
}

void free_simple_node(TreeNode **currentNode)
{
    (*currentNode)->parent = NULL;
    free((*currentNode)->name);
    if ((*currentNode)->type == FILE_NODE)
    {
        TreeNode *aux = *currentNode;
        FileContent *content = aux->content;
        free(content->text);
        free((*currentNode)->content);
    }
    free(*currentNode);
}

void freeTree_recursive(TreeNode *currentNode)
{
    if (!currentNode->content || currentNode->type == FILE_NODE)
    {
        free_simple_node(&currentNode);
        return;
    }
    FolderContent *aux = currentNode->content;
    List *list = aux->children;
    ListNode *tmp = list->head;
    while (tmp)
    {
        freeTree_recursive(tmp->info);
        ListNode *aux = tmp->next;
        free(tmp);
        tmp = aux;
    }
    free(list);
    free(aux);
    free_simple_node(&currentNode);
}

void freeTree(FileTree fileTree)
{
    // TODO
    freeTree_recursive(fileTree.root);
}

void ls(TreeNode *currentNode, char *arg)
{
    if (!currentNode->content)
    {
        return;
    }

    if (strlen(arg) != 0)
    {
        if (verify_folder(currentNode, arg) == 1)
        {
            currentNode = cd(currentNode, arg);
            if (!currentNode->content)
            {
                return;
            }
            FolderContent *aux = currentNode->content;
            List *list = aux->children;
            ListNode *tmp = list->head;
            while (tmp)
            {
                printf("%s\n", tmp->info->name);
                tmp = tmp->next;
            }
        }
        else if (verify_file(currentNode, arg, 0) == 1)
        {
            printf("%s: ", arg);
            verify_file(currentNode, arg, 1);
        }
        else
        {
            printf("ls: cannot access '%s': No such file or directory", arg);
        }
    }
    else
    {
        FolderContent *aux = currentNode->content;
        List *list = aux->children;
        ListNode *tmp = list->head;
        while (tmp)
        {
            printf("%s\n", tmp->info->name);
            tmp = tmp->next;
        }
    }
}

void pwd(TreeNode *treeNode)
{
    // TODO
    if (treeNode->parent)
    {
        pwd(treeNode->parent);
        printf("/");
    }
    printf("%s", treeNode->name);
}

TreeNode
    *
    cd_recursive(TreeNode *currentNode, TreeNode *original_node,
                 char *original_path, char *curr_path, char *last_dir)
{
    if (curr_path == NULL)
        return currentNode;
    if (!currentNode->content && strcmp(curr_path, "..") != 0)
    {
        printf("cd: no such file or directory: %s", original_path);
        return original_node;
    }

    if (strcmp(curr_path, "..") == 0)
    {
        fflush(stdout);
        if (currentNode->parent == NULL)
        {
            printf("cd: no such file or directory: %s", original_path);
            return original_node;
        }
        curr_path = strtok(NULL, "/");
        return cd_recursive(currentNode->parent, original_node,
                            original_path, curr_path, last_dir);
    }
    FolderContent *aux = currentNode->content;
    List *list = aux->children;
    ListNode *tmp = list->head;
    int ok = 0;
    while (tmp)
    {
        if (strcmp(curr_path, tmp->info->name) == 0)
        {
            ok = 1;
            if (strcmp(curr_path, last_dir) == 0)
            {
                return tmp->info;
            }
            else
            {
                curr_path = strtok(NULL, "/");
                return cd_recursive(tmp->info, original_node,
                                    original_path, curr_path, last_dir);
            }
        }

        tmp = tmp->next;
    }
    if (ok == 0)
    {
        printf("cd: no such file or directory: %s\n", original_path);
        return original_node;
    }
}

TreeNode *cd(TreeNode *currentNode, char *path)
{
    // TODO
    char *original_path = malloc(strlen(path) + 1);
    memcpy(original_path, path, strlen(path) + 1);
    char *resizable_path = malloc(strlen(path) + 2);
    memcpy(resizable_path, path, strlen(path) + 1);
    resizable_path[strlen(path)] = '/';
    resizable_path[strlen(path) + 1] = '\0';
    TreeNode *original_node = currentNode;
    char *curr_path = strtok(resizable_path, "/");
    char last_dir[100];
    while (curr_path)
    {
        memcpy(last_dir, curr_path, strlen(curr_path) + 1);
        curr_path = strtok(NULL, "/");
    }
    free(resizable_path);
    resizable_path = malloc(strlen(path) + 2);
    memcpy(resizable_path, path, strlen(path) + 1);
    resizable_path[strlen(path)] = '/';
    curr_path = strtok(resizable_path, "/");
    resizable_path[strlen(path) + 1] = '\0';
    TreeNode *aux = cd_recursive(currentNode, original_node,
                                 original_path, curr_path, last_dir);
    free(original_path);
    free(resizable_path);
    return aux;
}

int verify_nestedFolder_recursive(TreeNode *currentNode,
                                  TreeNode *original_node, char *original_path,
                                  char *curr_path, char *last_dir)
{
    if (currentNode->type == FILE_NODE)
        return 0;
    if (curr_path == NULL)
        return 1;
    if (!currentNode->content && strcmp(curr_path, "..") != 0)
    {
        return 0;
    }

    if (strcmp(curr_path, "..") == 0)
    {
        fflush(stdout);
        if (currentNode->parent == NULL)
        {
            return 0;
        }
        curr_path = strtok(NULL, "/");
        return verify_nestedFolder_recursive(currentNode->parent,
                                             original_node, original_path,
                                             curr_path, last_dir);
    }
    FolderContent *aux = currentNode->content;
    List *list = aux->children;
    ListNode *tmp = list->head;
    int ok = 0;
    while (tmp)
    {
        if (strcmp(curr_path, tmp->info->name) == 0)
        {
            ok = 1;
            if (strcmp(curr_path, last_dir) == 0)
            {
                if (tmp->info->type == FOLDER_NODE)
                    return 1;
                else
                    return 0;
            }
            else
            {
                curr_path = strtok(NULL, "/");
                return verify_nestedFolder_recursive(tmp->info,
                                                     original_node,
                                                     original_path, curr_path,
                                                     last_dir);
            }
        }

        tmp = tmp->next;
    }
    if (ok == 0)
    {
        return 0;
    }
}

int verify_nestedFolder(TreeNode *currentNode, char *path)
{
    // TODO
    char *original_path = malloc(strlen(path) + 1);
    memcpy(original_path, path, strlen(path) + 1);
    char *resizable_path = malloc(strlen(path) + 2);
    memcpy(resizable_path, path, strlen(path) + 1);
    resizable_path[strlen(path)] = '/';
    resizable_path[strlen(path) + 1] = '\0';
    TreeNode *original_node = currentNode;
    char *curr_path = strtok(resizable_path, "/");
    char last_dir[100];
    while (curr_path)
    {
        memcpy(last_dir, curr_path, strlen(curr_path) + 1);
        curr_path = strtok(NULL, "/");
    }
    free(resizable_path);
    resizable_path = malloc(strlen(path) + 2);
    memcpy(resizable_path, path, strlen(path) + 1);
    resizable_path[strlen(path)] = '/';
    curr_path = strtok(resizable_path, "/");
    resizable_path[strlen(path) + 1] = '\0';
    int aux;
    aux = verify_nestedFolder_recursive(currentNode,
                                        original_node, original_path,
                                        curr_path, last_dir);
    free(original_path);
    free(resizable_path);
    return aux;
}

void tree_recursive(TreeNode *currentNode,
                    int number_of_tabs, int *dir_nr, int *file_nr)
{
    for (int i = 0; i < number_of_tabs - 1; i++)
        printf("\t");
    if (number_of_tabs != 0)
        printf("%s\n", currentNode->name);
    if (!currentNode->content || currentNode->type == FILE_NODE)
    {
        return;
    }
    FolderContent *aux = currentNode->content;
    List *list = aux->children;
    ListNode *tmp = list->head;
    while (tmp)
    {
        tree_recursive(tmp->info, number_of_tabs + 1, dir_nr, file_nr);
        if (tmp->info->type == FILE_NODE)
        {
            *file_nr = *file_nr + 1;
            // printf("ajunge\n");
        }
        else if (tmp->info->type == FOLDER_NODE)
        {
            *dir_nr = *dir_nr + 1;
        }
        tmp = tmp->next;
    }
}

void tree(TreeNode *currentNode, char *arg)
{
    // TODO
    int *dir_nr = malloc(sizeof(int));
    *dir_nr = 0;
    int *file_nr = malloc(sizeof(int));
    *file_nr = 0;

    if (strlen(arg) > 0)
    {
        if (verify_nestedFolder(currentNode, arg) == 1)
        {
            TreeNode *auxNode = cd(currentNode, arg);
            tree_recursive(auxNode, 0, dir_nr, file_nr);
            printf("%d directories, %d files\n", *dir_nr, *file_nr);
        }
        else
        {
            printf("%s [error opening dir]\n\n0 directories, 0 files\n", arg);
        }
    }
    else
    {
        tree_recursive(currentNode, 0, dir_nr, file_nr);
        printf("%d directories, %d files\n", *dir_nr, *file_nr);
    }
    free(dir_nr);
    free(file_nr);
}

void mkdir(TreeNode *currentNode, char *folderName)
{
    // TODO
    if (verify_folder(currentNode, folderName) == 1)
    {
        printf("mkdir: cannot create directory ");
        printf("'%s': File exists\n", folderName);
        return;
    }
    if (!currentNode->content)
    {
        currentNode->content = malloc(sizeof(FolderContent));
        FolderContent *aux = currentNode->content;
        aux->children = malloc(sizeof(List));
        aux->children->head = NULL;
    }

    FolderContent *aux = currentNode->content;
    List *list = aux->children;
    ListNode *tmp = list->head;
    ListNode *new_node = malloc(sizeof(ListNode));
    new_node->info = malloc(sizeof(TreeNode));
    new_node->info->name = malloc(strlen(folderName) + 1);
    memcpy(new_node->info->name, folderName, strlen(folderName) + 1);
    new_node->info->content = NULL;
    new_node->info->type = FOLDER_NODE;
    new_node->info->parent = currentNode;
    new_node->next = tmp;
    list->head = new_node;
}

void rmrec(TreeNode *currentNode, char *resourceName)
{
    // TODO
}

void rm(TreeNode *currentNode, char *fileName)
{
    // TODO
}

void rmdir(TreeNode *currentNode, char *folderName)
{
    // TODO
}

void touch(TreeNode *currentNode, char *fileName, char *fileContent)
{
    // TODO
    if (verify_file(currentNode, fileName, 0) == 1)
    {
        return;
    }
    if (!currentNode->content)
    {
        currentNode->content = malloc(sizeof(FolderContent));
        FolderContent *aux = currentNode->content;
        aux->children = malloc(sizeof(List));
        aux->children->head = NULL;
    }

    FolderContent *aux = currentNode->content;
    List *list = aux->children;
    ListNode *tmp = list->head;
    ListNode *new_node = malloc(sizeof(ListNode));
    new_node->info = malloc(sizeof(TreeNode));
    new_node->info->name = malloc(strlen(fileName) + 1);
    memcpy(new_node->info->name, fileName, strlen(fileName) + 1);
    new_node->info->content = malloc(sizeof(FileContent));
    FileContent *new_file = new_node->info->content;
    new_file->text = malloc(strlen(fileContent) + 1);
    memcpy(new_file->text, fileContent, strlen(fileContent) + 1);
    new_node->info->type = FILE_NODE;
    new_node->info->parent = currentNode;
    new_node->next = tmp;
    list->head = new_node;
}

void cp(TreeNode *currentNode, char *source, char *destination)
{
    // TODO
}

void mv(TreeNode *currentNode, char *source, char *destination)
{
    // TODO
}
