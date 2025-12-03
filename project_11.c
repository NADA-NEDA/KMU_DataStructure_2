#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// ================= 기본 설정 =================

#define MAX_SIZE 32033    // 데이터셋 최대 크기

// 전역 비교 횟수
long long g_comparisons = 0;

// ================= 1. 비정렬 배열 (순차 탐색) =================

int seq_search(int arr[], int size, int key) {
    g_comparisons = 0;
    for (int i = 0; i < size; i++) {
        g_comparisons++;          // arr[i] == key 비교
        if (arr[i] == key) {
            return i;
        }
    }
    return -1;
}

int unsorted_insert(int arr[], int* size, int key) {
    // 중복 여부 확인: 있으면 실패, 없으면 삽입
    int idx = seq_search(arr, *size, key);    // 비교 수 카운트
    if (idx != -1) {
        // 이미 존재 → 삽입 실패
        return 0;
    }

    // 존재하지 않으면: 비교 수(g_comparisons)는 "중복 검사"에 대한 것
    if (*size >= MAX_SIZE) return 0;
    arr[*size] = key;
    (*size)++;
    return 1;
}

int unsorted_delete(int arr[], int* size, int key) {
    int idx = seq_search(arr, *size, key);    // 여기서 비교 수 카운트
    if (idx == -1) {
        // 삭제 실패 (없는 키)
        return 0;
    }
    // 마지막 원소로 덮어씌워서 O(1) 삭제
    arr[idx] = arr[*size - 1];
    (*size)--;
    return 1;
}

// ================= 2. 정렬 배열 (이진 탐색) =================

int bin_search(int arr[], int size, int key) {
    g_comparisons = 0;
    int low = 0, high = size - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        g_comparisons++;          // arr[mid] == key 비교
        if (arr[mid] == key) {
            return mid;
        }
        else if (arr[mid] < key) {
            low = mid + 1;
        }
        else {
            high = mid - 1;
        }
    }
    return -1;
}

int sorted_insert(int arr[], int* size, int key) {
    int low = 0, high = *size - 1;
    g_comparisons = 0;

    // 이진 탐색으로 위치/중복 여부 확인
    while (low <= high) {
        int mid = low + (high - low) / 2;

        g_comparisons++;          // arr[mid] == key 비교
        if (arr[mid] == key) {
            // 이미 존재 → 삽입 실패
            return 0;
        }
        else if (arr[mid] < key) {
            low = mid + 1;
        }
        else {
            high = mid - 1;
        }
    }
    // low = 삽입 위치
    if (*size >= MAX_SIZE) return 0;

    for (int i = *size; i > low; i--) {
        arr[i] = arr[i - 1];
    }
    arr[low] = key;
    (*size)++;
    return 1;
}

int sorted_delete(int arr[], int* size, int key) {
    int idx = bin_search(arr, *size, key);    // 여기서 비교 수 카운트
    if (idx == -1) {
        // 삭제 실패 (없는 키)
        return 0;
    }

    for (int i = idx; i < *size - 1; i++) {
        arr[i] = arr[i + 1];
    }
    (*size)--;
    return 1;
}

// ================= 3. AVL 트리 =================

typedef struct AVLNode {
    int key;
    int height;
    struct AVLNode* left;
    struct AVLNode* right;
} AVLNode;

int avl_height(AVLNode* node) {
    return node ? node->height : 0;
}

int avl_max(int a, int b) {
    return (a > b) ? a : b;
}

AVLNode* avl_new_node(int key) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) {
        fprintf(stderr, "메모리 부족(AVLNode)\n");
        exit(1);
    }
    node->key = key;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

AVLNode* avl_right_rotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;

    return x;
}

AVLNode* avl_left_rotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;

    return y;
}

int avl_get_balance(AVLNode* node) {
    if (!node) return 0;
    return avl_height(node->left) - avl_height(node->right);
}

// 재귀 삽입 (중복이면 실패)
AVLNode* avl_insert_node(AVLNode* node, int key, long long* comp, int* inserted) {
    if (!node) {
        *inserted = 1;
        return avl_new_node(key);
    }

    (*comp)++;                          // key vs node->key 비교
    if (key < node->key) {
        node->left = avl_insert_node(node->left, key, comp, inserted);
    }
    else if (key > node->key) {
        node->right = avl_insert_node(node->right, key, comp, inserted);
    }
    else {
        *inserted = 0;                  // 중복 → 삽입 실패
        return node;
    }

    node->height = 1 + avl_max(avl_height(node->left), avl_height(node->right));

    int balance = avl_get_balance(node);

    // LL
    if (balance > 1 && key < node->left->key)
        return avl_right_rotate(node);
    // RR
    if (balance < -1 && key > node->right->key)
        return avl_left_rotate(node);
    // LR
    if (balance > 1 && key > node->left->key) {
        node->left = avl_left_rotate(node->left);
        return avl_right_rotate(node);
    }
    // RL
    if (balance < -1 && key < node->right->key) {
        node->right = avl_right_rotate(node->right);
        return avl_left_rotate(node);
    }
    return node;
}

int avl_insert(AVLNode** root, int key) {
    int inserted = 0;
    g_comparisons = 0;
    *root = avl_insert_node(*root, key, &g_comparisons, &inserted);
    return inserted;
}

AVLNode* avl_min_node(AVLNode* node) {
    AVLNode* cur = node;
    while (cur && cur->left) {
        cur = cur->left;
    }
    return cur;
}

// 재귀 삭제
AVLNode* avl_delete_node(AVLNode* root, int key, long long* comp, int* deleted) {
    if (!root) {
        *deleted = 0;
        return root;
    }

    (*comp)++;                          // key vs root->key 비교
    if (key < root->key) {
        root->left = avl_delete_node(root->left, key, comp, deleted);
    }
    else if (key > root->key) {
        root->right = avl_delete_node(root->right, key, comp, deleted);
    }
    else {
        *deleted = 1;
        // 0개 또는 1개 자식
        if (!root->left || !root->right) {
            AVLNode* temp = root->left ? root->left : root->right;
            if (!temp) {        // leaf
                temp = root;
                root = NULL;
            }
            else {              // 한 자식
                *root = *temp;
            }
            free(temp);
        }
        else {
            // 2개 자식: 오른쪽 서브트리에서 최소 노드
            AVLNode* temp = avl_min_node(root->right);
            root->key = temp->key;
            root->right = avl_delete_node(root->right, temp->key, comp, deleted);
        }
    }

    if (!root) return root;

    root->height = 1 + avl_max(avl_height(root->left), avl_height(root->right));
    int balance = avl_get_balance(root);

    // LL
    if (balance > 1 && avl_get_balance(root->left) >= 0)
        return avl_right_rotate(root);
    // LR
    if (balance > 1 && avl_get_balance(root->left) < 0) {
        root->left = avl_left_rotate(root->left);
        return avl_right_rotate(root);
    }
    // RR
    if (balance < -1 && avl_get_balance(root->right) <= 0)
        return avl_left_rotate(root);
    // RL
    if (balance < -1 && avl_get_balance(root->right) > 0) {
        root->right = avl_right_rotate(root->right);
        return avl_left_rotate(root);
    }
    return root;
}

int avl_delete(AVLNode** root, int key) {
    int deleted = 0;
    g_comparisons = 0;
    *root = avl_delete_node(*root, key, &g_comparisons, &deleted);
    return deleted;
}

AVLNode* avl_search(AVLNode* root, int key) {
    g_comparisons = 0;
    AVLNode* cur = root;
    while (cur) {
        g_comparisons++;                          // key vs cur->key 비교
        if (key == cur->key) return cur;
        else if (key < cur->key) cur = cur->left;
        else cur = cur->right;
    }
    return NULL;
}

void avl_free(AVLNode* root) {
    if (!root) return;
    avl_free(root->left);
    avl_free(root->right);
    free(root);
}

// ================= 파일 로드 & 셔플 & 정렬 =================

int load_data(int arr[]) {
    FILE* file = fopen("데이터.csv", "r");
    if (!file) {
        fprintf(stderr, "Error: '데이터.csv' 파일을 열 수 없습니다.\n");
        return 0;
    }

    char line[1024];
    int count = 0;

    // 헤더 스킵
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return 0;
    }

    while (fgets(line, sizeof(line), file) && count < MAX_SIZE) {
        if (sscanf(line, "%d,", &arr[count]) == 1) {
            count++;
        }
    }
    fclose(file);
    return count;
}

int compare_ints(const void* a, const void* b) {
    int x = *(const int*)a;
    int y = *(const int*)b;
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}

void shuffle(int arr[], int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void setup_arrays(int original[], int size, int unsorted[], int sorted[]) {
    for (int i = 0; i < size; i++) {
        unsorted[i] = original[i];
        sorted[i] = original[i];
    }
    shuffle(unsorted, size);                      // 비정렬 배열
    qsort(sorted, size, sizeof(int), compare_ints);     // 정렬 배열
}

// ================= 메인: 성공/실패 × 삽입/삭제/검색 비교 횟수 =================

int main(void) {
    srand((unsigned)time(NULL)); // 실행 시마다 다른 시드로 초기화하여 랜덤성 확보

    int original[MAX_SIZE];
    int unsorted_arr[MAX_SIZE];
    int sorted_arr[MAX_SIZE];

    int n = load_data(original);
    if (n == 0) {
        return 1;
    }

    // 존재하는 키: original 데이터 중 랜덤으로 선정
    int random_index = rand() % n;
    int key_exist = original[random_index];

    // 존재하지 않는 키: 범위 밖 큰 값으로 가정
    // (original 배열은 CSV 로드 순서이므로, 마지막 요소가 가장 큰 ID일 확률이 높음)
    int max_id = original[n - 1];
    int key_absent = max_id + 1000;

    setup_arrays(original, n, unsorted_arr, sorted_arr);
    int n_uns = n;
    int n_srt = n;

    printf("데이터 개수                   = %d\n", n);
    printf("존재하는 키 (key_exist)       = %d (random)\n", key_exist);
    printf("존재하지 않는 키 (key_absent) = %d\n\n", key_absent);

    // ---------------- 1. 비정렬 배열 ----------------
    printf("=== [1] 비정렬 배열 (순차 탐색) ===\n");

    // 검색 성공
    seq_search(unsorted_arr, n_uns, key_exist);
    printf("검색 성공 (존재)   : %lld 회 비교\n", g_comparisons);

    // 검색 실패
    seq_search(unsorted_arr, n_uns, key_absent);
    printf("검색 실패 (미존재) : %lld 회 비교\n\n", g_comparisons);

    // 삽입 실패 (이미 있는 키)
    unsorted_insert(unsorted_arr, &n_uns, key_exist);
    printf("삽입 실패 (존재)   : %lld 회 비교\n", g_comparisons);

    // 삽입 성공 (없는 키)
    unsorted_insert(unsorted_arr, &n_uns, key_absent);
    printf("삽입 성공 (새 키)  : %lld 회 비교\n\n", g_comparisons);

    // 삭제 성공 (방금 넣은 key_absent 삭제)
    unsorted_delete(unsorted_arr, &n_uns, key_absent);
    printf("삭제 성공 (존재)   : %lld 회 비교\n", g_comparisons);

    // 삭제 실패 (이미 삭제된 key_absent 한 번 더 삭제 시도)
    unsorted_delete(unsorted_arr, &n_uns, key_absent);
    printf("삭제 실패 (미존재) : %lld 회 비교\n\n", g_comparisons);


    // ---------------- 2. 정렬 배열 ----------------
    printf("\n=== [2] 정렬 배열 (이진 탐색) ===\n");

    // 검색 성공
    bin_search(sorted_arr, n_srt, key_exist);
    printf("검색 성공 (존재)   : %lld 회 비교\n", g_comparisons);

    // 검색 실패
    bin_search(sorted_arr, n_srt, key_absent);
    printf("검색 실패 (미존재) : %lld 회 비교\n\n", g_comparisons);

    // 삽입 실패 (이미 있는 키)
    sorted_insert(sorted_arr, &n_srt, key_exist);
    printf("삽입 실패 (존재)   : %lld 회 비교\n", g_comparisons);

    // 삽입 성공 (없는 키)
    sorted_insert(sorted_arr, &n_srt, key_absent);
    printf("삽입 성공 (새 키)  : %lld 회 비교\n\n", g_comparisons);

    // 삭제 성공 (방금 넣은 key_absent 삭제)
    sorted_delete(sorted_arr, &n_srt, key_absent);
    printf("삭제 성공 (존재)   : %lld 회 비교\n", g_comparisons);

    // 삭제 실패 (이미 삭제된 key_absent 다시 삭제)
    sorted_delete(sorted_arr, &n_srt, key_absent);
    printf("삭제 실패 (미존재) : %lld 회 비교\n\n", g_comparisons);


    // ---------------- 3. AVL Tree ----------------
    printf("\n=== [3] AVL Tree ===\n");
    AVLNode* root = NULL;

    // 초기 데이터 전체 삽입 (초기 비교 횟수는 실험에서 제외)
    for (int i = 0; i < n; i++) {
        avl_insert(&root, original[i]);
    }

    // 검색 성공
    avl_search(root, key_exist);
    printf("검색 성공 (존재)   : %lld 회 비교\n", g_comparisons);

    // 검색 실패
    avl_search(root, key_absent);
    printf("검색 실패 (미존재) : %lld 회 비교\n\n", g_comparisons);

    // 삽입 실패 (이미 있는 키)
    avl_insert(&root, key_exist);
    printf("삽입 실패 (존재)   : %lld 회 비교\n", g_comparisons);

    // 삽입 성공 (없는 키)
    avl_insert(&root, key_absent);
    printf("삽입 성공 (새 키)  : %lld 회 비교\n\n", g_comparisons);

    // 삭제 성공 (방금 넣은 key_absent 삭제)
    avl_delete(&root, key_absent);
    printf("삭제 성공 (존재)   : %lld 회 비교\n", g_comparisons);

    // 삭제 실패 (이미 삭제된 key_absent 다시 삭제)
    avl_delete(&root, key_absent);
    printf("삭제 실패 (미존재) : %lld 회 비교\n", g_comparisons);

    avl_free(root);
    return 0;
}