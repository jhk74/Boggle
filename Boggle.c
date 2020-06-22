//Jason Kim jhk74
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FALSE	0;
#define TRUE	1;

typedef struct NODE
{
    struct NODE * child[26];
    int endOfWord;
    int count;
    char str[30];
} Node;

typedef struct WORDS
{
    char str[30];
    int count;
} Words;


int isNumber(char * str);
void printUsage();
void lowerCase(char * str);
int isValidWord(char * str);
Node * CreateNode();
void insertWordIntoTrie(char * str, Node * root);
int inRange(int i, int j, int nRows, int nCols);
int available(int i, int j, int nRows, int nCols, int ** visited, int isTFlag);
void boggleWords(Node * node, char ** board, int nRows, int nCols, int i, int j, int ** visited, char * str, int isTFlag);
void traverseTrie(Node * root, Words * words, int * cnt, int isCFlag);
void sortWords(Words * words, int cnt);
void printWords(Words * words, int cnt, int isCFlag);
void freeTrie(Node * root);

int main(int argc, char ** argv)
{
    int nRows;
    int nCols;
    int index;
    int isTFlag = FALSE;
    int isCFlag = FALSE;
    int i, j;
    char ** board;
    char str[30];
    Node * root;
    int ** visited;
    int cnt;
    Node * node;
    Words * words;	// array of words for printing
    index = argc - 1;

    // Check for valid numbers
    if (isNumber(argv[argc - 2]) && isNumber(argv[argc - 3]))
    {
        nCols = atoi(argv[argc - 2]);
        nRows = atoi(argv[argc - 3]);
    }
    else
    {
        printf("Illegal arguments");
        exit(1);
    }
    if (nRows < 1) {
        printf("Illegal Arguments");
        exit(1);
    }
    else if (nCols < 1) {
        printf("Illegal Arguments");
        exit(1);
    }

    // Check for valid flags
    switch (argc)
    {
        case 4:
            break;
        case 5:
            if (!strcmp(argv[1], "-t"))
            {
                isTFlag = TRUE;
            }
            else if (!strcmp(argv[1], "-c"))
            {
                isCFlag = TRUE;
            }
            else
            {
                printf("Illegal arguments");
                exit(1);
            }
            nRows = atoi(argv[2]);
            nCols = atoi(argv[3]);
            index = 4;
            break;
        case 6:
            if ((!strcmp(argv[1], "-c")) && (!strcmp(argv[2], "-t")))
            {
                isCFlag = TRUE;
                isTFlag = TRUE;
            }
            else
            {
                printf("Illegal arguments");
                exit(1);
            }
            nRows = atoi(argv[3]);
            nCols = atoi(argv[4]);
            index = 5;
            break;
        default:
            printf("Illegal arguments");
            exit(1);
            break;
    }
    if (nRows < 1) {
        printf("Illegal Arguments");
        exit(1);
    }
    else if (nCols < 1) {
        printf("Illegal Arguments");
        exit(1);
    }
    // Check if board and dimensions fit
    if (nRows * nCols != strlen(argv[index]))
    {
        printf("Dimensions don't match with board.\n");
        exit(1);
    }

    // Allocate memory for board an visited flags in 2d arrays
    board = (char **)malloc(nRows * sizeof(char*));
    visited = (int **)malloc(nRows * sizeof(int *));
    for (i = 0; i < nRows; i++)
    {
        board[i] = (char *)malloc(nCols * sizeof(char));
        visited[i] = (int *)malloc(nCols * sizeof(int));
    }

    // transform board letters to lowercase
    for (i = 0; i < nRows; i++)
    {
        for (j = 0; j < nCols; j++)
        {
            board[i][j] = tolower(argv[index][i * nCols + j]);
            visited[i][j] = FALSE;
        }
    }

    // build Trie from stdin inputs
    root = CreateNode();
    cnt = 0;
    while (gets(str))
    {
        lowerCase(str);
        if (isValidWord(str))
        {
            insertWordIntoTrie(str, root);
        }
        cnt++;
    }

    // Allocate memory for words for print
    words = (Words *)malloc(cnt * sizeof(Words));

    // solve boggle board by visiting every field as a start letter
    str[0] = '\000';
    cnt = 0;
    node = root;
    char c;
    for (i = 0; i < nRows; i++)
    {
        for (j = 0; j < nCols; j++)
        {
            if (board[i][j] != '_') // check if underscore
            {
                index = board[i][j] - 0x61;
                if (node->child[index])
                {
                    str[0] = board[i][j];
                    str[1] = '\000';
                    boggleWords(node->child[index], board, nRows, nCols, i, j, visited, str, isTFlag);
                    str[0] = '\000';
                }
            }
            else
            {
                // Replace '_' with every character
                for (c = 'a'; c <= 'z'; c++)
                {
                    index = c - 0x61;
                    if (node->child[index])
                    {
                        str[0] = c;
                        str[1] = '\000';
                        boggleWords(node->child[index], board, nRows, nCols, i, j, visited, str, isTFlag);
                        str[0] = '\000';
                    }
                }
            }
        }
    }

    // Traverse Trie to prepare for printing
    cnt = 0;
    traverseTrie(root, words, &cnt, isCFlag);

    // Sort the words
    sortWords(words, cnt);

    // Print the words
    printWords(words, cnt, isCFlag);

    // Free the allocated memory
    freeTrie(root);

    for (i = 0; i < nRows; i++)
    {
        free(board[i]);
        free(visited[i]);
    }
    free(words);
    free(board);
    free(visited);

    exit(0);
}


// Check if the string is from digits only
int isNumber(char * str)
{
    int len = strlen(str);
    int i;
    int isNum = TRUE;
    for (i = 0; i < len; i++)
    {
        if ((str[i] < '0') || (str[i] > '9'))
        {
            isNum = FALSE;
        }
    }
    return isNum;
}

// Prints the board
void printBoard(char ** board, int nRows, int nCols)
{
    int i;
    int j;
    for (i = 0; i < nRows; i++)
    {
        for (j = 0; j < nCols; j++)
        {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}

// Converts string to lowercase
void lowerCase(char * str)
{
    int i;
    int len = strlen(str);
    for (i = 0; i < len; i++)
    {
        str[i] = tolower(str[i]);
    }
}

// Check if character is valid
int isValidWord(char * str)
{
    int valid = TRUE;
    int len = strlen(str);
    int i = 0;
    while ((valid) && (i < len))
    {
        if ((!isalpha(str[i])) || (str[i] == ' '))
        {
            valid = FALSE;
        }
        i++;
    }
    return valid;
}

// Allocate memory and create a node in Trie
Node * CreateNode()
{
    int i;
    Node * n = (Node *)malloc(sizeof(Node));
    n->endOfWord = FALSE;
    n->count = 0;
    for (i = 0; i < 26; i++)
    {
        n->child[i] = NULL;
    }
    return n;
}

// Insert string into Trie
void insertWordIntoTrie(char * str, Node * root)
{
    int len = strlen(str);
    Node * n = root;
    int i = 0;
    int index;
    while (i < len)
    {
        index = str[i] - 0x61;
        if (!n->child[index])
        {
            n->child[index] = CreateNode();
        }
        n = n->child[index];
        i++;
    }
    n->endOfWord = TRUE;
    strcpy(n->str, str);
}

// Check if indices are in board range
int inRange(int i, int j, int nRows, int nCols)
{
    int in = FALSE;
    if ((i >= 0) && (i < nRows) && (j >= 0) && (j < nCols))
    {
        in = TRUE;
    }
    return in;
}

// Check if cell is available - it is in range and not yet visited if -t
int available(int i, int j, int nRows, int nCols, int ** visited, int isTFlag)
{
    if (inRange(i, j, nRows, nCols) && ((!isTFlag) || ((isTFlag) && (!visited[i][j]))))
    {
        return TRUE;
    }
    return FALSE;
}

// boggle words from a cell recursively
void boggleWords(Node * node, char ** board, int nRows, int nCols, int i, int j, int ** visited, char * str, int isTFlag)
{
    char c;
    int cnt;
    char copy[30] = "";
    // Check if there is an end of word
    if (node->endOfWord)
    {
        node->count++;
    }

    // if cell is available
    if (available(i,  j, nRows, nCols, visited, isTFlag))
    {
        visited[i][j] = TRUE;
        // check all existing children
        for (cnt = 0; cnt < 26; cnt++)
        {
            if (node->child[cnt])
            {
                c = (char)cnt + 0x61;
                strcpy(copy, str);
                copy[strlen(copy)] = c;
                copy[strlen(copy) + 1] = '\000';

                // Recursively check adjacent cells
                if (available(i, j + 1, nRows, nCols, visited, isTFlag) && ((board[i][j + 1] == c) || (board[i][j + 1] == '_')))
                {
                    boggleWords(node->child[cnt], board, nRows, nCols, i, j + 1, visited, copy, isTFlag);
                }
                if (available(i, j - 1, nRows, nCols, visited, isTFlag) && ((board[i][j - 1] == c) || (board[i][j - 1] == '_')))
                {
                    boggleWords(node->child[cnt], board, nRows, nCols, i, j - 1, visited, copy, isTFlag);
                }
                if (available(i + 1, j, nRows, nCols, visited, isTFlag) && ((board[i + 1][j] == c) || (board[i + 1][j] == '_')))
                {
                    boggleWords(node->child[cnt], board, nRows, nCols, i + 1, j, visited, copy, isTFlag);
                }
                if (available(i - 1, j, nRows, nCols, visited, isTFlag) && ((board[i - 1][j] == c) || (board[i - 1][j] == '_')))
                {
                    boggleWords(node->child[cnt], board, nRows, nCols, i - 1, j, visited, copy, isTFlag);
                }
                if (available(i + 1, j + 1, nRows, nCols, visited, isTFlag) && ((board[i + 1][j + 1] == c) || (board[i + 1][j + 1] == '_')))
                {
                    boggleWords(node->child[cnt], board, nRows, nCols, i + 1, j + 1, visited, copy, isTFlag);
                }
                if (available(i + 1, j - 1, nRows, nCols, visited, isTFlag) && ((board[i + 1][j - 1] == c) || (board[i + 1][j - 1] == '_')))
                {
                    boggleWords(node->child[cnt], board, nRows, nCols, i + 1, j - 1, visited, copy, isTFlag);
                }
                if (available(i - 1, j + 1, nRows, nCols, visited, isTFlag) && ((board[i - 1][j + 1] == c) || (board[i - 1][j + 1] == '_')))
                {
                    boggleWords(node->child[cnt], board, nRows, nCols, i - 1, j + 1, visited, copy, isTFlag);
                }
                if (available(i - 1, j - 1, nRows, nCols, visited, isTFlag) && ((board[i - 1][j - 1] == c) || (board[i - 1][j - 1] == '_')))
                {
                    boggleWords(node->child[cnt], board, nRows, nCols, i - 1, j - 1, visited, copy, isTFlag);
                }
            }
        }
        visited[i][j] = FALSE;
    }
}

// Visit all nodes in Trie to find words and boggled words and put them in an array for printing
void traverseTrie(Node * root, Words * words, int * cnt, int isCFlag)
{
    int i;
    if (root->endOfWord)
    {
        if (((isCFlag) && (root->count == 0)) || ((!isCFlag) && (root->count > 0)))
        {
            strcpy(words[*cnt].str, root->str);
            words[*cnt].count = root->count;
            (*cnt)++;
        }
    }
    for (i = 0; i < 26; i++)
    {
        if (root->child[i])
        {
            traverseTrie(root->child[i], words, cnt, isCFlag);
        }
    }
}

// Sort the words in array ascending using bubble sort
void sortWords(Words * words, int cnt)
{
    int swap = TRUE;
    int i = 0;
    int j = 0;
    char str[30];
    while ((swap) && (i < cnt - 1))
    {
        swap = FALSE;
        for (j = 0; j < cnt - 1 - i; j++)
        {
            if (strcmp(words[j].str, words[j + 1].str) > 0)
            {
                strcpy(str, words[j].str);
                strcpy(words[j].str, words[j + 1].str);
                strcpy(words[j + 1].str, str);
                swap = TRUE;
            }
        }
    }
}

// Print words from result array
void printWords(Words * words, int cnt, int isCFlag)
{
    int i;
    for (i = 0; i < cnt; i++)
    {
        if (isCFlag)
        {
            printf("%s\n", words[i].str);
        }
        else
        {
            printf("%s: %d\n", words[i].str, words[i].count);
        }
    }
}

// Free memory allocated for Trie recursively
void freeTrie(Node * root)
{
    int i;
    for (i = 0; i < 26; i++)
    {
        if (root->child[i])
        {
            freeTrie(root->child[i]);
        }
    }
    free(root);
    root = NULL;
}