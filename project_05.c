#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_SIZE 1000   // ������ ����
#define MAX_VALUE 10000  // ������ ���� �ִ� ����

// �迭 Ž�� (����)
int linear_search(int arr[], int n, int target) 
{
    int count = 0;                    // �� Ƚ�� ���� ����
    for (int i = 0; i < n; i++)       // �迭�� ó������ ������ Ž��
    {
        count++;                      // �� 1ȸ ����
        if (arr[i] == target) break;  // ��ǥ �� ã���� �ݺ� ����
    }
    return count;                     // �� Ƚ�� ��ȯ
}
// ���� Ž�� Ʈ�� (BST)
// BST ��� ���� ����
typedef struct BSTNode 
{
    int key;                    // Ű(��)
    struct BSTNode* left;       // ���� �ڽ� ��� ������
    struct BSTNode* right;      // ������ �ڽ� ��� ������
} BSTNode;

// ���ο� BST ��� ���� �Լ�
BSTNode* newBSTNode(int key) 
{
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));  // ���� �޸� �Ҵ�
    node->key = key;                                    // Ű ����
    node->left = node->right = NULL;                    // �ڽ� ��� �ʱ�ȭ
    return node;                                        // ������ ��� ��ȯ
}

// BST ���� �Լ�
BSTNode* bst_insert(BSTNode* root, int key) 
{
    if (!root) return newBSTNode(key);               // ��Ʈ�� ������ �� ��� ��ȯ
    if (key < root->key)                             // Ű�� ��Ʈ���� ������ ���ʿ� ����
        root->left = bst_insert(root->left, key);
    else if (key > root->key)                        // Ű�� ��Ʈ���� ũ�� �����ʿ� ����
        root->right = bst_insert(root->right, key);
    return root;                                     // ��Ʈ ��ȯ (Ʈ�� ����)
}

// BST Ž�� �Լ�
int bst_search(BSTNode* root, int target) 
{
    int count = 0;                       // �� Ƚ�� ī��Ʈ
    BSTNode* cur = root;                 // ���� ��带 ��Ʈ�� ����
    while (cur)                          // ��尡 ������ ���� �ݺ�
    {
        count++;                         // �� ����
        if (target == cur->key) break;   // ã���� ����
        else if (target < cur->key)      // �� ������ �������� �̵�
            cur = cur->left;
        else                             // �� ũ�� ���������� �̵�
            cur = cur->right;
    }
    return count;                        // �� Ƚ�� ��ȯ
}

// BST �޸� ���� �Լ�
void free_bst(BSTNode* root) 
{
    if (!root) return;      // ��尡 ������ ��ȯ
    free_bst(root->left);   // ���� ����Ʈ�� ����
    free_bst(root->right);  // ������ ����Ʈ�� ����
    free(root);             // ���� ��� ����
}

// AVL Ʈ��
// AVL Ʈ���� ��� ���� ����
typedef struct AVLNode 
{
    int key;                // Ű ��
    struct AVLNode* left;   // ���� �ڽ�
    struct AVLNode* right;  // ������ �ڽ�
    int height;             // ��� ����
} AVLNode;

// ����� ���� ��ȯ �Լ�
int height(AVLNode* n) { return n ? n->height : 0; }  // ��尡 NULL�̸� 0 ��ȯ

// �� �� �� ū �� ��ȯ �Լ�
int maxN(int a, int b) { return (a > b) ? a : b; }

// ���ο� AVL ��� ���� �Լ�
AVLNode* newAVLNode(int key) 
{
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));  // �޸� �Ҵ�
    node->key = key;                                    // Ű ����
    node->left = node->right = NULL;                    // �ڽ� ��� �ʱ�ȭ
    node->height = 1;                                   // ���� �ʱⰪ
    return node; 
}

// ������ ȸ�� (LL �ұ��� ����)
AVLNode* rightRotate(AVLNode* y) 
{
    AVLNode* x = y->left;    // ���� �ڽ� ����
    AVLNode* T2 = x->right;  // x�� ������ ����Ʈ�� �ӽ� ����

    x->right = y;            // ȸ�� ����
    y->left = T2;

    y->height = maxN(height(y->left), height(y->right)) + 1;  // ���� ����
    x->height = maxN(height(x->left), height(x->right)) + 1;

    return x;                // �� ��Ʈ ��ȯ
}

// ���� ȸ�� (RR �ұ��� ����)
AVLNode* leftRotate(AVLNode* x) 
{
    AVLNode* y = x->right;   // ������ �ڽ� ����
    AVLNode* T2 = y->left;   // y�� ���� ����Ʈ�� �ӽ� ����

    y->left = x;             // ȸ�� ����
    x->right = T2;

    x->height = maxN(height(x->left), height(x->right)) + 1;  // ���� ����
    y->height = maxN(height(y->left), height(y->right)) + 1;

    return y;                // �� ��Ʈ ��ȯ
}

// ���� ��� ���
int getBalance(AVLNode* n) { return n ? height(n->left) - height(n->right) : 0; }

// AVL ���� �Լ� (���� ����)
AVLNode* avl_insert(AVLNode* node, int key) 
{
    // 1. �Ϲ� BST ����
    if (!node) return newAVLNode(key);
    if (key < node->key)
        node->left = avl_insert(node->left, key);
    else if (key > node->key)
        node->right = avl_insert(node->right, key);
    else
        return node;  // �ߺ� Ű ����

    // 2. ���� ����
    node->height = 1 + maxN(height(node->left), height(node->right));

    // 3. ���� Ȯ��
    int balance = getBalance(node);

    // LL
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    // RR
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    // LR
    if (balance > 1 && key > node->left->key) 
    {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // RL
    if (balance < -1 && key < node->right->key) 
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;  // ���� �� ��Ʈ ��ȯ
}

// AVL Ž�� �Լ�
int avl_search(AVLNode* root, int target) 
{
    int count = 0;        // �� Ƚ�� ī��Ʈ
    AVLNode* cur = root;  // ���� ��� ������
    while (cur) 
    {
        count++;                        // �� ����
        if (target == cur->key) break;
        else if (target < cur->key)
            cur = cur->left;            // ���� �̵�
        else
            cur = cur->right;           // ������ �̵�
    }
    return count;                       // �� Ƚ�� ��ȯ
}

// AVL Ʈ�� �޸� ���� �Լ�
void free_avl(AVLNode* root) 
{
    if (!root) return;
    free_avl(root->left);
    free_avl(root->right);
    free(root);
}

// �ߺ� ���� ���� ���� �Լ�
void shuffle(int arr[], int n) 
{
    for (int i = n - 1; i > 0; i--) 
    {
        int j = rand() % (i + 1);  // 0~i ���� ���� �ε��� ����
        int tmp = arr[i];          // swap
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

// ������ ����
void prepare_data(int idx, int data[]) {
    switch (idx) {
    case 1:  // (1) 0~10000 �ߺ� ���� ������ 1000��
    {   
        int pool[MAX_VALUE + 1];               // 0~10000 ���� �迭
        for (int i = 0; i <= MAX_VALUE; i++)   // �ʱ�ȭ
            pool[i] = i;
        shuffle(pool, MAX_VALUE + 1);          // ������ ����
        for (int i = 0; i < DATA_SIZE; i++)    // �� 1000�� ���
            data[i] = pool[i];
    }
    break; 
    case 2:  // (2) 0~999 ��������
        for (int i = 0; i < DATA_SIZE; i++)
            data[i] = i;
        break;
    case 3:  // (3) 999~0 ��������
        for (int i = 0; i < DATA_SIZE; i++)
            data[i] = DATA_SIZE - 1 - i;
        break;
    case 4:  // (4) i * (i%2 + 2)
        for (int i = 0; i < DATA_SIZE; i++)
            data[i] = i * ((i % 2) + 2);
        break;
    }
}

// ���� �Լ�
int main() 
{
    srand((unsigned)time(NULL));  // ���� �õ� ����

    int data[4][DATA_SIZE];       // 4���� ������ ���� �迭
    int queries[DATA_SIZE];       // Ž���� ���� �迭

    // 4���� ������ ����
    for (int i = 0; i < 4; i++)
        prepare_data(i + 1, data[i]);

    // �ߺ� ���� Ž���� ���� 1000�� ����
    int pool[MAX_VALUE + 1];
    for (int i = 0; i <= MAX_VALUE; i++) pool[i] = i;
    shuffle(pool, MAX_VALUE + 1);
    for (int i = 0; i < DATA_SIZE; i++) queries[i] = pool[i];

    // �� �������� �̸� (��¿�)
    char* labels[4] = { "������ (1)", "������ (2)", "������ (3)", "������ (4)" };

    // �� ������ ��Ʈ���� Ž�� ����
    for (int t = 0; t < 4; t++)
    {
        long long sum_array = 0, sum_bst = 0, sum_avl = 0;  // �� Ƚ�� �ջ� ����

        // (1) �迭 Ž�� (���� Ž��)
        for (int i = 0; i < DATA_SIZE; i++)
            sum_array += linear_search(data[t], DATA_SIZE, queries[i]);
        double avg_array = (double)sum_array / DATA_SIZE;  // ��� ���

        // (2) BST ���� �� Ž��
        BSTNode* bst = NULL;
        for (int i = 0; i < DATA_SIZE; i++) bst = bst_insert(bst, data[t][i]);       // ����
        for (int i = 0; i < DATA_SIZE; i++) sum_bst += bst_search(bst, queries[i]);  // Ž��
        double avg_bst = (double)sum_bst / DATA_SIZE;
        free_bst(bst);  // �޸� ����

        // (3) AVL ���� �� Ž��
        AVLNode* avl = NULL;
        for (int i = 0; i < DATA_SIZE; i++) avl = avl_insert(avl, data[t][i]);
        for (int i = 0; i < DATA_SIZE; i++) sum_avl += avl_search(avl, queries[i]);
        double avg_avl = (double)sum_avl / DATA_SIZE;
        free_avl(avl);  // �޸� ����

        // ��� ���
        printf("Array: %s���� ��� %.2fȸ Ž��\n", labels[t], avg_array);
        printf("BST  : %s���� ��� %.2fȸ Ž��\n", labels[t], avg_bst);
        printf("AVL  : %s���� ��� %.2fȸ Ž��\n\n", labels[t], avg_avl);
    }

    return 0;
}