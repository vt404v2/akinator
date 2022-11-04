#include "tree.h"

size_t treeCtor(Tree *tree)
{
    CHECK_NULLPTR_ERROR(tree, TREE_IS_NULLPTR)

    tree->root = nullptr;
    tree->size = 0;
    return TREE_NO_ERRORS;
}

size_t nodeCtor(Node *node)
{
    CHECK_NULLPTR_ERROR(node, NODE_IS_NULLPTR)

    node->left = nullptr;
    node->right = nullptr;

    return TREE_NO_ERRORS;
}

size_t treeDtor(Tree *tree)
{
    CHECK_NULLPTR_ERROR(tree, TREE_IS_NULLPTR)

    nodeDtor(tree->root);
    tree->size = 0;
    return TREE_NO_ERRORS;
}

size_t nodeDtor(Node *node)
{
    CHECK_NULLPTR_ERROR(node, NODE_IS_NULLPTR)

    if (node->left)
        nodeDtor(node->left);
    if (node->right)
        nodeDtor(node->right);

    free(node);
    return TREE_NO_ERRORS;
}

size_t treeDump(Tree *tree)
{
    CHECK_NULLPTR_ERROR(tree, TREE_IS_NULLPTR)

    char filename[128] = "logs.html";
    char photo_name[128] = "log.jpg";

    FILE *fp = fopen(filename, "w");

    fprintf(fp, "<pre>\n");

    createGraph(tree, photo_name);
    fprintf(fp, "<img src=%s />\n", photo_name);

    fclose(fp);
}

size_t createGraph(Tree *tree, const char *photo_name)
{
    CHECK_NULLPTR_ERROR(tree, TREE_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(photo_name, TREE_FILENAME_IS_NULLPTR)

    FILE *fp = fopen(GRAPH_FILENAME, "w");
    fprintf(fp, "digraph TREE {\n"
                "    rankdir=TB;\n");

    createGraphNodes(tree->root, fp);
    createGraphEdges(tree->root, fp);

    // close graph with }
    fprintf(fp, "}\n");

    char command[128] = "";
    fclose(fp);
    sprintf(command, "dot %s -T jpg -o %s", GRAPH_FILENAME, photo_name);
    system(command);
    return TREE_NO_ERRORS;
}

size_t createGraphNodes(Node *node, FILE *fp)
{
    if (!node)
        return TREE_NO_ERRORS;

    size_t error = TREE_NO_ERRORS;

    fprintf(fp,
            "    node_%p[shape=\"record\", \n"
            "        color=%s,"
            "        fillcolor=%s, \n"
            "        style=\"rounded, filled\", \n"
            "        label=\"\n"
            "            {%s? |\n"
            "            {{LEFT | %p} | {INDEX | %p} | {RIGHT | %p}}}\"]\n",
            node,
            GREEN_COLOR,
            LIGHT_GREEN_COLOR,
            node->value,
            node->left,
            node,
            node->right);

    if (node->left)
        error = createGraphNodes(node->left, fp);
    if (error)
        return error;

    if (node->right)
        error = createGraphNodes(node->right, fp);
    if (error)
        return error;

    return TREE_NO_ERRORS;
}

size_t createGraphEdges(Node *node, FILE *fp)
{
    if (!node)
        return TREE_NO_ERRORS;

    size_t error = TREE_NO_ERRORS;

    if (node->left)
    {
        fprintf(fp,
                "    node_%p->node_%p[label=\"YES\", color=%s, fontcolor=%s];",
                node,
                node->left,
                PURPLE_COLOR,
                GREEN_COLOR);
        error = createGraphEdges(node->left, fp);
        if (error)
            return error;
    }

    if (node->right)
    {
        fprintf(fp,
                "    node_%p->node_%p[label=\"NO\",  color=%s, fontcolor=%s];",
                node,
                node->right,
                PURPLE_COLOR,
                RED_COLOR);
        error = createGraphEdges(node->right, fp);
        if (error)
            return error;
    }

    return TREE_NO_ERRORS;
}

size_t treePrint(Tree *tree, FILE *fp)
{
    CHECK_NULLPTR_ERROR(tree, TREE_IS_NULLPTR)

    return nodePreOrderPrint(tree->root, fp, 0);
}

size_t nodePreOrderPrint(Node *node, FILE *fp, size_t num_spaces)
{
    CHECK_NULLPTR_ERROR(node, NODE_IS_NULLPTR)

    size_t error = TREE_NO_ERRORS;

    for (size_t i = 0; i < num_spaces; i++)
    {
        fprintf(fp, " ");
    }

    if (node->left == nullptr and node->right == nullptr)
    {
        fprintf(fp, "{ %s }\n", node->value);
        return TREE_NO_ERRORS;
    }
    else
        fprintf(fp, "{ %s \n", node->value);

    if (node->left)
        error = nodePreOrderPrint(node->left, fp, num_spaces + 4);
    if (error)
        return error;

    if (node->right)
        error = nodePreOrderPrint(node->right, fp, num_spaces + 4);
    if (error)
        return error;

    for (size_t i = 0; i < num_spaces; i++)
    {
        fprintf(fp, " ");
    }
    fprintf(fp, "}\n");

    return TREE_NO_ERRORS;
}

size_t nodeInOrderPrint(Node *node)
{
    if (!node)
        return TREE_NO_ERRORS;

    size_t error = TREE_NO_ERRORS;

    if (node->left)
        error = nodeInOrderPrint(node->left);
    if (error)
        return error;

    printf("%s\n", node->value);

    if (node->right)
        error = nodeInOrderPrint(node->right);
    if (error)
        return error;

    return TREE_NO_ERRORS;
}

size_t nodePostOrderPrint(Node *node)
{
    if (!node)
        return TREE_NO_ERRORS;

    size_t error = TREE_NO_ERRORS;

    if (node->left)
        error = nodePostOrderPrint(node->left);
    if (error)
        return error;

    if (node->right)
        error = nodePostOrderPrint(node->right);
    if (error)
        return error;

    printf("%s\n", node->value);

    return TREE_NO_ERRORS;
}

size_t readTree(Tree *tree, const char *filename)
{
    CHECK_NULLPTR_ERROR(tree, TREE_IS_NULLPTR)

    FILE *fp = fopen(filename, "r");
    if (fp == nullptr)
        return CANT_OPEN_TREE_FILE;

    long lenOfFile = 0;
    char *tree_buffer = readFileToBuf(fp, &lenOfFile);
    fclose(fp);

    Node *new_node = (Node *) calloc(1, sizeof(tree->root[0]));
    if (new_node == nullptr)
        return TREE_CANT_ALLOCATE_MEMORY;
    tree->root = new_node;

    char *readPtr = tree_buffer;

    addNode(tree, tree->root, &tree_buffer, &readPtr, lenOfFile);

    return TREE_NO_ERRORS;
}

size_t addNode(Tree *tree,
               Node *node,
               char **buffer,
               char **readPtr,
               long lenOfFile)
{
    size_t error = TREE_NO_ERRORS;

    bool isToken = false;
    char *startTokenPtr = nullptr;
    char *endTokenPtr = nullptr;
    while (*readPtr < *buffer + lenOfFile)
    {
        if (**readPtr == '}' or **readPtr == EOF)
        {
            if (isToken > 0)
            {
                Node *new_node = nullptr;
                if (node == tree->root and tree->size == 0)
                    new_node = tree->root;
                else
                    new_node = (Node *) calloc(1, sizeof(Node));

                new_node->left = nullptr;
                new_node->right = nullptr;

                if (tree->root->value != nullptr)
                {
                    if (node->left == nullptr)
                        node->left = new_node;
                    else if (node->right == nullptr)
                        node->right = new_node;
                    else
                        fprintf(stderr, "Both is not NULLPTR\n");
                }

                if (*startTokenPtr == '\"')
                    startTokenPtr++;
                if (*endTokenPtr == '\"')
                    endTokenPtr--;

                new_node->value =
                    (char *) calloc(endTokenPtr - startTokenPtr + 1,
                                    sizeof(new_node->value[0]));
                CHECK_NULLPTR_ERROR(new_node->value,
                                    TREE_CANT_ALLOCATE_MEMORY)
                memcpy(new_node->value,
                       startTokenPtr,
                       endTokenPtr - startTokenPtr + 1);
                tree->size++;

                isToken = false;
                if (node->right != nullptr)
                    break;
            }
            (*readPtr)++;
        }
        else if (**readPtr == '{')
        {
            if (isToken > 0)
            {
                Node *new_node = nullptr;
                if (node == tree->root and tree->size == 0)
                    new_node = tree->root;
                else
                    new_node = (Node *) calloc(1, sizeof(Node));
                isToken = false;

                if (tree->root->value != nullptr)
                {
                    if (node->left == nullptr)
                        node->left = new_node;
                    else if (node->right == nullptr)
                        node->right = new_node;
                    else
                        fprintf(stderr, "Both is not NULLPTR\n");
                }

                if (*startTokenPtr == '\"')
                    startTokenPtr++;
                if (*endTokenPtr == '\"')
                    endTokenPtr--;

                new_node->value =
                    (char *) calloc(endTokenPtr - startTokenPtr + 1,
                                    sizeof(new_node->value[0]));
                CHECK_NULLPTR_ERROR(new_node->value,
                                    TREE_CANT_ALLOCATE_MEMORY)
                memcpy(new_node->value,
                       startTokenPtr,
                       endTokenPtr - startTokenPtr + 1);
                tree->size++;

                error = addNode(tree,
                                new_node,
                                buffer,
                                readPtr,
                                lenOfFile);

                if (error)
                    return error;
            }
            (*readPtr)++;
        }
        else
        {
            if (!isToken and **readPtr != ' ' and **readPtr != '\n')
            {
                startTokenPtr = *readPtr;
                isToken = true;
            }
            if (**readPtr != ' ' and **readPtr != '\n')
                endTokenPtr = *readPtr;
            (*readPtr)++;
        }
    }
    return TREE_NO_ERRORS;
}

size_t insertNode(Node *node,
                  char *value,
                  size_t value_size,
                  char *delimiter,
                  size_t delimiter_size)
{
    CHECK_NULLPTR_ERROR(node, NODE_IS_NULLPTR)

    node->left = (Node *) calloc(1, sizeof(Node));
    CHECK_NULLPTR_ERROR(node->left, TREE_CANT_ALLOCATE_MEMORY)
    nodeCtor(node->left);

    node->right = (Node *) calloc(1, sizeof(Node));
    CHECK_NULLPTR_ERROR(node->right, TREE_CANT_ALLOCATE_MEMORY)
    nodeCtor(node->right);

    node->right->value = node->value;

    node->left->value = (char *) calloc(1, value_size);
    memcpy(node->left->value, value, value_size);
    node->value = (char *) calloc(1, delimiter_size);
    memcpy(node->value, delimiter, delimiter_size);

    return TREE_NO_ERRORS;
}
