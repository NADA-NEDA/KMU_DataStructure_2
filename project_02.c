#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Input: (A (B (C D) E (G H (I J (K (L) M (N)))))) 
// Output: 5, 13, 6 �̷��� ��µž� �ϴµ� ����� 5,13,7�� ��µ�
// �� �ܸ� ����� ���� �ϳ� �� �����°���...

// �迭 ũ�� (Ʈ�� ��带 ������ �ִ� ũ��)
#define MAX 1000

// ���� ����Ʈ ����� Ʈ�� ��� ����ü
typedef struct Node
{
    char data;                // ��忡 ����� ���� ������
    struct Node* firstChild;  // ù ��° �ڽ� ��带 ����Ű�� ������
    struct Node* nextSibling; // ���� ���� ��带 ����Ű�� ������
} Node;

char arr[MAX];     // Ʈ���� �迭�� ǥ���� ���� (�ε��� 1���� ���)
int arrIndex = 1;  // arr �迭�� ���� ��尡 ����� ��ġ
const char* p;     // �Է� ���ڿ��� ��ȸ�ϴ� �� ���� ���� ������

// ���� ���ڸ� �ǳʶٴ� �Լ�
void skipSpaces()
{
    while (*p && isspace((unsigned char)*p))
    {
        p++;
    }
}

// ���ο� ��带 �������� �Ҵ��ϰ� �ʱ�ȭ�ϴ� �Լ�
Node* createNode(char data)
{
    Node* node = (Node*)malloc(sizeof(Node));  // Node ũ�⸸ŭ �޸� �Ҵ�

    // �޸� �Ҵ� ���� �� ����
    if (!node)
    {
        exit(EXIT_FAILURE);   // ���α׷� ���� ����
    }
    node->data = data;        // ��忡 ������ ����
    node->firstChild = NULL;  // ù �ڽ� ������ �ʱ�ȭ
    node->nextSibling = NULL; // ���� ���� ������ �ʱ�ȭ
    return node;              // ������ ����� ������ ��ȯ
}

// ��ȣ ������ ���ڿ��� �Ľ��Ͽ� Ʈ���� �����ϴ� �Լ�
Node* parse()
{
    skipSpaces();  // �Ľ� ���� �� ���� ����

    // '('�� �������� ������ ���� 
    if (*p != '(')
    {
        return NULL;
    }
    p++;           // '(' ���� �Һ�
    skipSpaces();  // ���� ���� �ǳʶٱ�

    // �� ��ȣ "()" �� ���
    if (*p == ')')
    {
        p++;  // ')' �Һ�
        return NULL;
    }

    // ��� �����ʹ� ���ĺ��� �ƴ� ��� ���� 
    if (!isalpha(*p))
    {
        return NULL;
    }
    Node* root = createNode(*p);  // ���� ���(��Ʈ) ����
    arr[arrIndex] = *p;           // �迭�� ��� ������ ����
    //arrIndex++;                   // �迭 �ε��� ����
    p++;                          // ��� ���� �Һ�
    skipSpaces();                 // ���� ���� �ǳʶٱ�

    Node* prevChild = NULL;       // ���� �ڽ� ��� ������ (���� �����)

    // ')'�� ������ ������ �ڽ� ������ ��ȸ�ϸ� �Ľ�
    while (*p != ')' && *p != '\0')
    {
        Node* child = NULL;

        // �ڽ��� ����Ʈ���� ��� ��������� parse() ȣ��
        if (*p == '(')
        {
            child = parse();  // ����Ʈ�� ��ȯ
        }

        // �ڽ� ��尡 ���� ������ ���
        else if (isalpha(*p))
        {
            // ���� ���ڰ� �ƴ϶�� ��� ������ ����
            while (isalpha(*p))
            {
                Node* leaf = createNode(*p);  // ���ο� ��� ����
                arr[arrIndex] = *p;           // �迭�� ������ ����
                //arrIndex++;                 // �迭 �ε��� ����
                p++;
                skipSpaces();                 // ���� �ǳʶٱ�

                if (!root->firstChild)
                {
                    root->firstChild = leaf;
                }
                else
                {
                    prevChild->nextSibling = leaf;
                }
                prevChild = leaf;
            }
            continue; // �̹� ���� �Ϸ������Ƿ� �Ʒ� if(child) ��� �ǳʶ�
        }
        // ����ġ ���� ���� �߰� �� ���� 
        else
        {
            return NULL;
        }

        // ��ȿ�� �ڽ� ��尡 �Ľ̵� ���
        if (child)
        {
            if (!root->firstChild)
            {
                root->firstChild = child;  // ù ��° �ڽ����� ����
            }
            else
            {
                prevChild->nextSibling = child;  // ���� �ڽĵ��� ���� �ڽ��� ������ ����
            }
            prevChild = child;  // ���� �ڽ��� ���� �ݺ��� ���� '���� �ڽ�'���� ������Ʈ
        }
        skipSpaces();  // ���� ���� �ǳʶٱ�
    }

    // �ݴ� ')'�� ������ ���� 
    if (*p != ')')
    {
        return NULL;
    }
    p++;          // ')' ���� �Һ�
    return root;  // �Ľ̵� Ʈ���� ��Ʈ ��� ��ȯ
}

// Ʈ���� ��� ��� �޸𸮸� ��������� �����ϴ� �Լ�
void freeTree(Node* root)
{
    // ��尡 NULL�̸� ��ȯ
    if (!root)
    {
        return;
    }
    freeTree(root->firstChild);   // ù ��° �ڽ� ����Ʈ�� �޸� ����
    freeTree(root->nextSibling);  // ���� ���� ����Ʈ�� �޸� ����
    free(root);                   // ���� ��� �޸� ����
}

// Ʈ���� ���̸� ��������� ����ϴ� �Լ� (���� ��� ���� = 0)
int getHeight(Node* node)
{
    // �� Ʈ���� ���̴� -1
    if (!node)
    {
        return 0;
    }
    int maxChildHeight = 0;

    for (Node* c = node->firstChild; c; c = c->nextSibling)
    {
        int h = getHeight(c);

        if (h > maxChildHeight)
        {
            maxChildHeight = h;
        }
        return maxChildHeight + 1;
    }
}

// Ʈ���� ��ü ��� ���� ��������� ����ϴ� �Լ�
int countNodes(Node* node)
{
    // ��尡 ������ 0 ��ȯ
    if (!node)
    {
        return 0;
    }
    int count = 1;  // ���� ��� 1��

    for (Node* c = node->firstChild; c; c = c->nextSibling)
    {
        count += countNodes(c);
    }
    return count;
}

// Ʈ���� �ܸ� ���(���� ���) ���� ��������� ����ϴ� �Լ�
int countLeaves(Node* node)
{
    // ��尡 ������ 0 ��ȯ
    if (!node)
    {
        return 0;
    }

    // �ڽ��� ������ �ܸ� ����̹Ƿ� 1 ��ȯ
    if (!node->firstChild)
    {
        return 1;
    }
    int count = 0;

    // ù ��° �ڽ� ��常 ��ȸ
    for (Node* c = node->firstChild; c; c = c->nextSibling)
    {
        count += countLeaves(c);
    }
    return count;
}

int main(void)
{
    char input[1000];

    // �� �� ��ü�� �Է¹ޱ�
    if (scanf(" %[^\n]", input) != 1)
    {
        return 1;  // �Է� ���� �� ����
    }

    p = input;     // ���� �����͸� �Է� ���ڿ� ���������� ����
    arrIndex = 1;  // �迭 �ε��� �ʱ�ȭ (��Ʈ�� 1�� �ε���)

    Node* root = parse(); // Ʈ���� �Ľ�

    // �Ľ̿� ������ ���
    if (!root)
    {
        printf("0,0,0\n"); // 0,0,0 ���
        return 0;
    }

    // �Ľ̵� Ʈ���� �������� ����, ��� ��, �ܸ� ��� �� ���
    int height = getHeight(root);
    int nodes = countNodes(root);
    int leaves = countLeaves(root);

    printf("%d,%d,%d\n", height, nodes, leaves);

    freeTree(root); // ���α׷� ���� �� ���� �Ҵ�� �޸� ����

    return 0; // ���� ����
}