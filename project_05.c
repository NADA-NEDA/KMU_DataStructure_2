#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_SIZE 1000   // 데이터 개수
#define MAX_VALUE 10000  // 데이터 값의 최대 범위

// 배열 탐색 (선형)
int linear_search(int arr[], int n, int target) 
{
    int count = 0;                    // 비교 횟수 저장 변수
    for (int i = 0; i < n; i++)       // 배열의 처음부터 끝까지 탐색
    {
        count++;                      // 비교 1회 수행
        if (arr[i] == target) break;  // 목표 값 찾으면 반복 종료
    }
    return count;                     // 비교 횟수 반환
}
// 이진 탐색 트리 (BST)
// BST 노드 구조 정의
typedef struct BSTNode 
{
    int key;                    // 키(값)
    struct BSTNode* left;       // 왼쪽 자식 노드 포인터
    struct BSTNode* right;      // 오른쪽 자식 노드 포인터
} BSTNode;

// 새로운 BST 노드 생성 함수
BSTNode* newBSTNode(int key) 
{
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));  // 동적 메모리 할당
    node->key = key;                                    // 키 설정
    node->left = node->right = NULL;                    // 자식 노드 초기화
    return node;                                        // 생성된 노드 반환
}

// BST 삽입 함수
BSTNode* bst_insert(BSTNode* root, int key) 
{
    if (!root) return newBSTNode(key);               // 루트가 없으면 새 노드 반환
    if (key < root->key)                             // 키가 루트보다 작으면 왼쪽에 삽입
        root->left = bst_insert(root->left, key);
    else if (key > root->key)                        // 키가 루트보다 크면 오른쪽에 삽입
        root->right = bst_insert(root->right, key);
    return root;                                     // 루트 반환 (트리 유지)
}

// BST 탐색 함수
int bst_search(BSTNode* root, int target) 
{
    int count = 0;                       // 비교 횟수 카운트
    BSTNode* cur = root;                 // 현재 노드를 루트로 설정
    while (cur)                          // 노드가 존재할 동안 반복
    {
        count++;                         // 비교 수행
        if (target == cur->key) break;   // 찾으면 종료
        else if (target < cur->key)      // 더 작으면 왼쪽으로 이동
            cur = cur->left;
        else                             // 더 크면 오른쪽으로 이동
            cur = cur->right;
    }
    return count;                        // 비교 횟수 반환
}

// BST 메모리 해제 함수
void free_bst(BSTNode* root) 
{
    if (!root) return;      // 노드가 없으면 반환
    free_bst(root->left);   // 왼쪽 서브트리 해제
    free_bst(root->right);  // 오른쪽 서브트리 해제
    free(root);             // 현재 노드 해제
}

// AVL 트리
// AVL 트리의 노드 구조 정의
typedef struct AVLNode 
{
    int key;                // 키 값
    struct AVLNode* left;   // 왼쪽 자식
    struct AVLNode* right;  // 오른쪽 자식
    int height;             // 노드 높이
} AVLNode;

// 노드의 높이 반환 함수
int height(AVLNode* n) { return n ? n->height : 0; }  // 노드가 NULL이면 0 반환

// 두 수 중 큰 값 반환 함수
int maxN(int a, int b) { return (a > b) ? a : b; }

// 새로운 AVL 노드 생성 함수
AVLNode* newAVLNode(int key) 
{
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));  // 메모리 할당
    node->key = key;                                    // 키 설정
    node->left = node->right = NULL;                    // 자식 노드 초기화
    node->height = 1;                                   // 높이 초기값
    return node; 
}

// 오른쪽 회전 (LL 불균형 보정)
AVLNode* rightRotate(AVLNode* y) 
{
    AVLNode* x = y->left;    // 왼쪽 자식 저장
    AVLNode* T2 = x->right;  // x의 오른쪽 서브트리 임시 저장

    x->right = y;            // 회전 수행
    y->left = T2;

    y->height = maxN(height(y->left), height(y->right)) + 1;  // 높이 갱신
    x->height = maxN(height(x->left), height(x->right)) + 1;

    return x;                // 새 루트 반환
}

// 왼쪽 회전 (RR 불균형 보정)
AVLNode* leftRotate(AVLNode* x) 
{
    AVLNode* y = x->right;   // 오른쪽 자식 저장
    AVLNode* T2 = y->left;   // y의 왼쪽 서브트리 임시 저장

    y->left = x;             // 회전 수행
    x->right = T2;

    x->height = maxN(height(x->left), height(x->right)) + 1;  // 높이 갱신
    y->height = maxN(height(y->left), height(y->right)) + 1;

    return y;                // 새 루트 반환
}

// 균형 계수 계산
int getBalance(AVLNode* n) { return n ? height(n->left) - height(n->right) : 0; }

// AVL 삽입 함수 (균형 유지)
AVLNode* avl_insert(AVLNode* node, int key) 
{
    // 1. 일반 BST 삽입
    if (!node) return newAVLNode(key);
    if (key < node->key)
        node->left = avl_insert(node->left, key);
    else if (key > node->key)
        node->right = avl_insert(node->right, key);
    else
        return node;  // 중복 키 무시

    // 2. 높이 갱신
    node->height = 1 + maxN(height(node->left), height(node->right));

    // 3. 균형 확인
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
    return node;  // 변경 후 루트 반환
}

// AVL 탐색 함수
int avl_search(AVLNode* root, int target) 
{
    int count = 0;        // 비교 횟수 카운트
    AVLNode* cur = root;  // 현재 노드 포인터
    while (cur) 
    {
        count++;                        // 비교 수행
        if (target == cur->key) break;
        else if (target < cur->key)
            cur = cur->left;            // 왼쪽 이동
        else
            cur = cur->right;           // 오른쪽 이동
    }
    return count;                       // 비교 횟수 반환
}

// AVL 트리 메모리 해제 함수
void free_avl(AVLNode* root) 
{
    if (!root) return;
    free_avl(root->left);
    free_avl(root->right);
    free(root);
}

// 중복 없는 난수 생성 함수
void shuffle(int arr[], int n) 
{
    for (int i = n - 1; i > 0; i--) 
    {
        int j = rand() % (i + 1);  // 0~i 사이 랜덤 인덱스 선택
        int tmp = arr[i];          // swap
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

// 데이터 생성
void prepare_data(int idx, int data[]) {
    switch (idx) {
    case 1:  // (1) 0~10000 중복 없는 무작위 1000개
    {   
        int pool[MAX_VALUE + 1];               // 0~10000 숫자 배열
        for (int i = 0; i <= MAX_VALUE; i++)   // 초기화
            pool[i] = i;
        shuffle(pool, MAX_VALUE + 1);          // 무작위 섞기
        for (int i = 0; i < DATA_SIZE; i++)    // 앞 1000개 사용
            data[i] = pool[i];
    }
    break; 
    case 2:  // (2) 0~999 오름차순
        for (int i = 0; i < DATA_SIZE; i++)
            data[i] = i;
        break;
    case 3:  // (3) 999~0 내림차순
        for (int i = 0; i < DATA_SIZE; i++)
            data[i] = DATA_SIZE - 1 - i;
        break;
    case 4:  // (4) i * (i%2 + 2)
        for (int i = 0; i < DATA_SIZE; i++)
            data[i] = i * ((i % 2) + 2);
        break;
    }
}

// 메인 함수
int main() 
{
    srand((unsigned)time(NULL));  // 난수 시드 설정

    int data[4][DATA_SIZE];       // 4종류 데이터 저장 배열
    int queries[DATA_SIZE];       // 탐색용 난수 배열

    // 4가지 데이터 생성
    for (int i = 0; i < 4; i++)
        prepare_data(i + 1, data[i]);

    // 중복 없는 탐색용 난수 1000개 생성
    int pool[MAX_VALUE + 1];
    for (int i = 0; i <= MAX_VALUE; i++) pool[i] = i;
    shuffle(pool, MAX_VALUE + 1);
    for (int i = 0; i < DATA_SIZE; i++) queries[i] = pool[i];

    // 각 데이터의 이름 (출력용)
    char* labels[4] = { "데이터 (1)", "데이터 (2)", "데이터 (3)", "데이터 (4)" };

    // 각 데이터 세트별로 탐색 수행
    for (int t = 0; t < 4; t++)
    {
        long long sum_array = 0, sum_bst = 0, sum_avl = 0;  // 비교 횟수 합산 변수

        // (1) 배열 탐색 (선형 탐색)
        for (int i = 0; i < DATA_SIZE; i++)
            sum_array += linear_search(data[t], DATA_SIZE, queries[i]);
        double avg_array = (double)sum_array / DATA_SIZE;  // 평균 계산

        // (2) BST 생성 및 탐색
        BSTNode* bst = NULL;
        for (int i = 0; i < DATA_SIZE; i++) bst = bst_insert(bst, data[t][i]);       // 삽입
        for (int i = 0; i < DATA_SIZE; i++) sum_bst += bst_search(bst, queries[i]);  // 탐색
        double avg_bst = (double)sum_bst / DATA_SIZE;
        free_bst(bst);  // 메모리 해제

        // (3) AVL 생성 및 탐색
        AVLNode* avl = NULL;
        for (int i = 0; i < DATA_SIZE; i++) avl = avl_insert(avl, data[t][i]);
        for (int i = 0; i < DATA_SIZE; i++) sum_avl += avl_search(avl, queries[i]);
        double avg_avl = (double)sum_avl / DATA_SIZE;
        free_avl(avl);  // 메모리 해제

        // 결과 출력
        printf("Array: %s에서 평균 %.2f회 탐색\n", labels[t], avg_array);
        printf("BST  : %s에서 평균 %.2f회 탐색\n", labels[t], avg_bst);
        printf("AVL  : %s에서 평균 %.2f회 탐색\n\n", labels[t], avg_avl);
    }

    return 0;
}