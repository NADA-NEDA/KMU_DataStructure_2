#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

// ================= 기본 설정 =================

// [수정 1] 데이터 개수(약 3.2만 개) + 삽입 테스트 여유분을 고려하여 50,000으로 증량
#define MAX_SIZE 50000    

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
    // 중복 여부 확인
    int idx = seq_search(arr, *size, key);
    if (idx != -1) return 0; // 이미 존재

    // [수정 2] 배열 용량 초과 방지 안전장치
    if (*size >= MAX_SIZE) {
        printf("[Error] 배열 용량이 가득 찼습니다.\n");
        return 0;
    }

    arr[*size] = key;
    (*size)++;
    return 1;
}

int unsorted_delete(int arr[], int* size, int key) {
    int idx = seq_search(arr, *size, key);
    if (idx == -1) return 0; // 삭제 실패

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
            return 0; // 이미 존재
        }
        else if (arr[mid] < key) {
            low = mid + 1;
        }
        else {
            high = mid - 1;
        }
    }

    // [수정 2] 배열 용량 초과 방지
    if (*size >= MAX_SIZE) {
        printf("[Error] 배열 용량이 가득 찼습니다.\n");
        return 0;
    }

    // Shift 연산 (데이터 밀기)
    for (int i = *size; i > low; i--) {
        arr[i] = arr[i - 1];
    }
    arr[low] = key;
    (*size)++;
    return 1;
}

int sorted_delete(int arr[], int* size, int key) {
    int idx = bin_search(arr, *size, key);
    if (idx == -1) return 0; // 삭제 실패

    // Shift 연산 (데이터 당기기)
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

AVLNode* avl_insert_node(AVLNode* node, int key, long long* comp, int* inserted) {
    if (!node) {
        *inserted = 1;
        return avl_new_node(key);
    }

    (*comp)++;
    if (key < node->key) {
        node->left = avl_insert_node(node->left, key, comp, inserted);
    }
    else if (key > node->key) {
        node->right = avl_insert_node(node->right, key, comp, inserted);
    }
    else {
        *inserted = 0; // 중복
        return node;
    }

    node->height = 1 + avl_max(avl_height(node->left), avl_height(node->right));
    int balance = avl_get_balance(node);

    // 회전 로직 (LL, RR, LR, RL)
    if (balance > 1 && key < node->left->key)
        return avl_right_rotate(node);
    if (balance < -1 && key > node->right->key)
        return avl_left_rotate(node);
    if (balance > 1 && key > node->left->key) {
        node->left = avl_left_rotate(node->left);
        return avl_right_rotate(node);
    }
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
    while (cur && cur->left) cur = cur->left;
    return cur;
}

AVLNode* avl_delete_node(AVLNode* root, int key, long long* comp, int* deleted) {
    if (!root) {
        *deleted = 0;
        return root;
    }

    (*comp)++;
    if (key < root->key) {
        root->left = avl_delete_node(root->left, key, comp, deleted);
    }
    else if (key > root->key) {
        root->right = avl_delete_node(root->right, key, comp, deleted);
    }
    else {
        *deleted = 1;
        if (!root->left || !root->right) {
            AVLNode* temp = root->left ? root->left : root->right;
            if (!temp) {
                temp = root;
                root = NULL;
            }
            else {
                *root = *temp;
            }
            free(temp);
        }
        else {
            AVLNode* temp = avl_min_node(root->right);
            root->key = temp->key;
            root->right = avl_delete_node(root->right, temp->key, comp, deleted);
        }
    }

    if (!root) return root;

    root->height = 1 + avl_max(avl_height(root->left), avl_height(root->right));
    int balance = avl_get_balance(root);

    if (balance > 1 && avl_get_balance(root->left) >= 0)
        return avl_right_rotate(root);
    if (balance > 1 && avl_get_balance(root->left) < 0) {
        root->left = avl_left_rotate(root->left);
        return avl_right_rotate(root);
    }
    if (balance < -1 && avl_get_balance(root->right) <= 0)
        return avl_left_rotate(root);
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
        g_comparisons++;
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
        fprintf(stderr, "Error: '데이터.csv' 파일을 찾을 수 없습니다.\n");
        return 0;
    }

    char line[1024];
    int count = 0;

    // 헤더 스킵
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return 0;
    }

    while (fgets(line, sizeof(line), file)) {
        // [수정 2] 파일이 MAX_SIZE보다 크면 중단 (Buffer Overflow 방지)
        if (count >= MAX_SIZE) {
            printf("Warning: 설정된 최대 데이터 개수(%d)에 도달하여 로드를 중단합니다.\n", MAX_SIZE);
            break;
        }
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
    shuffle(unsorted, size);
    qsort(sorted, size, sizeof(int), compare_ints);
}

// ================= 메인 함수 =================

int main(void) {
    srand((unsigned)time(NULL));

    // [수정 1] Stack Overflow 방지를 위해 대용량 배열은 Heap(malloc)에 할당
    int* original = (int*)malloc(sizeof(int) * MAX_SIZE);
    int* unsorted_arr = (int*)malloc(sizeof(int) * MAX_SIZE);
    int* sorted_arr = (int*)malloc(sizeof(int) * MAX_SIZE);

    // 메모리 할당 실패 체크
    if (!original || !unsorted_arr || !sorted_arr) {
        fprintf(stderr, "메모리 할당 실패\n");
        return 1;
    }

    int n = load_data(original);
    if (n == 0) {
        // 종료 전 메모리 해제
        free(original); free(unsorted_arr); free(sorted_arr);
        return 1;
    }

    // 존재하는 키: 랜덤 선택
    int random_index = rand() % n;
    int key_exist = original[random_index];

    // 존재하지 않는 키: 로드된 데이터 중 최댓값 + 1000으로 설정
    int max_val = 0;
    for (int i = 0; i < n; i++) {
        if (original[i] > max_val) max_val = original[i];
    }
    int key_absent = max_val + 1000;

    setup_arrays(original, n, unsorted_arr, sorted_arr);
    int n_uns = n;
    int n_srt = n;

    printf("데이터 개수           = %d\n", n);
    printf("존재하는 키 (key_exist)   = %d\n", key_exist);
    printf("존재하지 않는 키 (key_absent) = %d\n\n", key_absent);

    // ---------------- 1. 비정렬 배열 ----------------
    printf("=== [1] 비정렬 배열 (순차 탐색) ===\n");
    seq_search(unsorted_arr, n_uns, key_exist);
    printf("검색 성공 (존재)   : %lld 회 비교\n", g_comparisons);
    seq_search(unsorted_arr, n_uns, key_absent);
    printf("검색 실패 (미존재) : %lld 회 비교\n\n", g_comparisons);
    unsorted_insert(unsorted_arr, &n_uns, key_exist);
    printf("삽입 실패 (존재)   : %lld 회 비교\n", g_comparisons);
    unsorted_insert(unsorted_arr, &n_uns, key_absent);
    printf("삽입 성공 (새 키)  : %lld 회 비교\n\n", g_comparisons);
    unsorted_delete(unsorted_arr, &n_uns, key_absent);
    printf("삭제 성공 (존재)   : %lld 회 비교\n", g_comparisons);
    unsorted_delete(unsorted_arr, &n_uns, key_absent);
    printf("삭제 실패 (미존재) : %lld 회 비교\n\n", g_comparisons);

    // ---------------- 2. 정렬 배열 ----------------
    printf("\n=== [2] 정렬 배열 (이진 탐색) ===\n");
    bin_search(sorted_arr, n_srt, key_exist);
    printf("검색 성공 (존재)   : %lld 회 비교\n", g_comparisons);
    bin_search(sorted_arr, n_srt, key_absent);
    printf("검색 실패 (미존재) : %lld 회 비교\n\n", g_comparisons);
    sorted_insert(sorted_arr, &n_srt, key_exist);
    printf("삽입 실패 (존재)   : %lld 회 비교\n", g_comparisons);
    sorted_insert(sorted_arr, &n_srt, key_absent);
    printf("삽입 성공 (새 키)  : %lld 회 비교\n\n", g_comparisons);
    sorted_delete(sorted_arr, &n_srt, key_absent);
    printf("삭제 성공 (존재)   : %lld 회 비교\n", g_comparisons);
    sorted_delete(sorted_arr, &n_srt, key_absent);
    printf("삭제 실패 (미존재) : %lld 회 비교\n\n", g_comparisons);

    // ---------------- 3. AVL Tree ----------------
    printf("\n=== [3] AVL Tree ===\n");
    AVLNode* root = NULL;
    for (int i = 0; i < n; i++) avl_insert(&root, original[i]);

    avl_search(root, key_exist);
    printf("검색 성공 (존재)   : %lld 회 비교\n", g_comparisons);
    avl_search(root, key_absent);
    printf("검색 실패 (미존재) : %lld 회 비교\n\n", g_comparisons);
    avl_insert(&root, key_exist);
    printf("삽입 실패 (존재)   : %lld 회 비교\n", g_comparisons);
    avl_insert(&root, key_absent);
    printf("삽입 성공 (새 키)  : %lld 회 비교\n\n", g_comparisons);
    avl_delete(&root, key_absent);
    printf("삭제 성공 (존재)   : %lld 회 비교\n", g_comparisons);
    avl_delete(&root, key_absent);
    printf("삭제 실패 (미존재) : %lld 회 비교\n", g_comparisons);

    // [수정 3] 메모리 정리 (필수)
    avl_free(root);
    free(original);
    free(unsorted_arr);
    free(sorted_arr);

    return 0;
}
