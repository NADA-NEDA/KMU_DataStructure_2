#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <psapi.h>

// ---------- 상수 정의 ----------
#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 200

// 반복 횟수: 비교/시간/메모리 모두 이 횟수만큼 반복 후 평균 (과제 A용)
#define REPEAT_COUNT 10

// 과제 B용 반복 횟수: 비교 횟수 평균을 위한 반복 (Assignment B 전용)
#define REPEAT_COUNT_B 1000

// --------------
// Student 구조체
// --------------
typedef struct {
    int  id;
    char name[MAX_NAME_LEN];
    char gender;
    int  korean_grade;
    int  english_grade;
    int  math_grade;
    int  total;
} Student;

// 비교 횟수 카운트 (전역)
long long g_comparison_count = 0;

// 타입 정의
typedef int  (*CmpFunc)(const Student*, const Student*);
typedef void (*SortFunc)(Student*, int, CmpFunc);

// ---------- 전방 선언 ----------
void swap_student(Student* a, Student* b);

// 정렬 알고리즘 선언
void bubble_sort(Student* arr, int n, CmpFunc cmp);
void selection_sort(Student* arr, int n, CmpFunc cmp);
void insertion_sort(Student* arr, int n, CmpFunc cmp);
void shell_sort(Student* arr, int n, CmpFunc cmp);
void quick_sort(Student* arr, int n, CmpFunc cmp);
void heap_sort(Student* arr, int n, CmpFunc cmp);
void merge_sort(Student* arr, int n, CmpFunc cmp);
void radix_sort(Student* arr, int n, CmpFunc cmp);
void tree_sort(Student* arr, int n, CmpFunc cmp);

// ===== 과제 B용 개선 정렬 전방 선언 =====
void shell_sort_improved(Student* arr, int n, CmpFunc cmp);
void quick_sort_improved(Student* arr, int n, CmpFunc cmp);
void tree_sort_avl(Student* arr, int n, CmpFunc cmp);
void run_assignment_B(Student* students, int n);
// ======================================

// CSV 로딩
Student* load_students(const char* filename, int* out_count);

// 비교 함수들
int cmp_id_asc(const Student* a, const Student* b);
int cmp_id_desc(const Student* a, const Student* b);
int cmp_name_asc(const Student* a, const Student* b);
int cmp_name_desc(const Student* a, const Student* b);
int cmp_gender_asc(const Student* a, const Student* b);
int cmp_gender_desc(const Student* a, const Student* b);
int cmp_total_asc(const Student* a, const Student* b);
int cmp_total_desc(const Student* a, const Student* b);

// ----------------
// enum 및 헬퍼
// ----------------
typedef enum {
    SORT_BUBBLE,
    SORT_SELECTION,
    SORT_INSERTION,
    SORT_SHELL,
    SORT_QUICK,
    SORT_HEAP,
    SORT_MERGE,
    SORT_RADIX,
    SORT_TREE,
    SORT_ALGO_COUNT
} SortAlgo;

typedef enum {
    KEY_ID,
    KEY_NAME,
    KEY_GENDER,
    KEY_TOTAL,
    KEY_COUNT
} SortKey;

typedef enum {
    ORDER_ASC,
    ORDER_DESC,
    ORDER_COUNT
} SortOrder;

const char* get_algo_name(SortAlgo algo) {
    switch (algo) {
    case SORT_BUBBLE:    return "Bubble";
    case SORT_SELECTION: return "Selection";
    case SORT_INSERTION: return "Insertion";
    case SORT_SHELL:     return "Shell";
    case SORT_QUICK:     return "Quick";
    case SORT_HEAP:      return "Heap";
    case SORT_MERGE:     return "Merge";
    case SORT_RADIX:     return "Radix";
    case SORT_TREE:      return "Tree";
    default:             return "Unknown";
    }
}

SortFunc get_sort_func(SortAlgo algo) {
    switch (algo) {
    case SORT_BUBBLE:    return bubble_sort;
    case SORT_SELECTION: return selection_sort;
    case SORT_INSERTION: return insertion_sort;
    case SORT_SHELL:     return shell_sort;
    case SORT_QUICK:     return quick_sort;
    case SORT_HEAP:      return heap_sort;
    case SORT_MERGE:     return merge_sort;
    case SORT_RADIX:     return radix_sort;
    case SORT_TREE:      return tree_sort;
    default:             return NULL;
    }
}

CmpFunc get_cmp_func(SortKey key, SortOrder order) {
    switch (key) {
    case KEY_ID:     return (order == ORDER_ASC) ? cmp_id_asc : cmp_id_desc;
    case KEY_NAME:   return (order == ORDER_ASC) ? cmp_name_asc : cmp_name_desc;
    case KEY_GENDER: return (order == ORDER_ASC) ? cmp_gender_asc : cmp_gender_desc;
    case KEY_TOTAL:  return (order == ORDER_ASC) ? cmp_total_asc : cmp_total_desc;
    default:         return NULL;
    }
}

// -----------------
// swap 함수
// -----------------
void swap_student(Student* a, Student* b) {
    Student tmp = *a;
    *a = *b;
    *b = tmp;
}

// -----------------
// CSV 로딩 함수
// -----------------
Student* load_students(const char* filename, int* out_count) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open file (데이터.csv 확인)");
        return NULL;
    }

    char line[MAX_LINE_LEN];
    int capacity = 10;
    int count = 0;
    int bad_lines = 0;

    Student* arr = (Student*)malloc(sizeof(Student) * capacity);
    if (!arr) {
        perror("Memory allocation failed");
        fclose(fp);
        return NULL;
    }

    // 첫 줄 헤더 스킵
    if (fgets(line, sizeof(line), fp) == NULL) {
        fprintf(stderr, "File is empty or cannot read header\n");
        free(arr);
        fclose(fp);
        return NULL;
    }

    // 데이터 줄 읽기
    while (fgets(line, sizeof(line), fp)) {
        if (count >= capacity) {
            capacity *= 2;
            Student* temp = (Student*)realloc(arr, sizeof(Student) * capacity);
            if (!temp) {
                perror("Reallocation failed");
                free(arr);
                fclose(fp);
                return NULL;
            }
            arr = temp;
        }

        Student s;
        int ok = 1;

        char* token = strtok(line, ",");
        if (!token) { ok = 0; }
        else { s.id = atoi(token); }

        token = strtok(NULL, ",");
        if (!token) { ok = 0; }
        else {
            token[strcspn(token, "\r\n")] = '\0';
            strncpy(s.name, token, MAX_NAME_LEN - 1);
            s.name[MAX_NAME_LEN - 1] = '\0';
        }

        token = strtok(NULL, ",");
        if (!token) { ok = 0; }
        else { s.gender = token[0]; }

        token = strtok(NULL, ",");
        if (!token) { ok = 0; }
        else { s.korean_grade = atoi(token); }

        token = strtok(NULL, ",");
        if (!token) { ok = 0; }
        else { s.english_grade = atoi(token); }

        token = strtok(NULL, ",\r\n");
        if (!token) { ok = 0; }
        else { s.math_grade = atoi(token); }

        if (ok) {
            s.total = s.korean_grade + s.english_grade + s.math_grade;
            arr[count++] = s;
        }
        else {
            bad_lines++;
        }
    }

    fclose(fp);

    Student* tight = (Student*)realloc(arr, sizeof(Student) * count);
    if (!tight) {
        fprintf(stderr, "Warning: tight realloc failed. Using original buffer.\n");
        *out_count = count;
        if (bad_lines > 0) {
            fprintf(stderr, "Warning: %d invalid data line(s) skipped.\n", bad_lines);
        }
        return arr;
    }

    *out_count = count;
    if (bad_lines > 0) {
        fprintf(stderr, "Warning: %d invalid data line(s) skipped.\n", bad_lines);
    }
    return tight;
}

// -----------------
// 비교 함수들
// -----------------

// 1) ID 기준
int cmp_id_asc(const Student* a, const Student* b) {
    g_comparison_count++;
    if (a->id < b->id) return -1;
    if (a->id > b->id) return 1;
    return 0;
}
int cmp_id_desc(const Student* a, const Student* b) {
    g_comparison_count++;
    if (a->id > b->id) return -1;
    if (a->id < b->id) return 1;
    return 0;
}

// 2) NAME 기준
int cmp_name_asc(const Student* a, const Student* b) {
    g_comparison_count++;
    return strcmp(a->name, b->name);
}
int cmp_name_desc(const Student* a, const Student* b) {
    g_comparison_count++;
    return -strcmp(a->name, b->name);
}

// 3) GENDER 기준
int cmp_gender_asc(const Student* a, const Student* b) {
    g_comparison_count++;
    if (a->gender < b->gender) return -1;
    if (a->gender > b->gender) return 1;
    return 0;
}
int cmp_gender_desc(const Student* a, const Student* b) {
    g_comparison_count++;
    if (a->gender > b->gender) return -1;
    if (a->gender < b->gender) return 1;
    return 0;
}

// 4) TOTAL 기준 + tie-break
int cmp_total_asc(const Student* a, const Student* b) {
    g_comparison_count++;
    if (a->total < b->total) return -1;
    if (a->total > b->total) return 1;

    // 동점 시: Korean > English > Math 점수 큰 순
    if (a->korean_grade != b->korean_grade)
        return b->korean_grade - a->korean_grade;
    if (a->english_grade != b->english_grade)
        return b->english_grade - a->english_grade;
    if (a->math_grade != b->math_grade)
        return b->math_grade - a->math_grade;

    return 0;
}

int cmp_total_desc(const Student* a, const Student* b) {
    g_comparison_count++;
    if (a->total > b->total) return -1;
    if (a->total < b->total) return 1;

    if (a->korean_grade != b->korean_grade)
        return b->korean_grade - a->korean_grade;
    if (a->english_grade != b->english_grade)
        return b->english_grade - a->english_grade;
    if (a->math_grade != b->math_grade)
        return b->math_grade - a->math_grade;

    return 0;
}

// -----------------
// 정렬 알고리즘 (과제 A 기본 버전)
// -----------------

// 1) Bubble Sort (Stable)
void bubble_sort(Student* arr, int n, CmpFunc cmp) {
    for (int i = 0; i < n - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            if (cmp(&arr[j], &arr[j + 1]) > 0) {
                swap_student(&arr[j], &arr[j + 1]);
                swapped = 1;
            }
        }
        if (!swapped) break;
    }
}

// 2) Selection Sort
void selection_sort(Student* arr, int n, CmpFunc cmp) {
    for (int i = 0; i < n - 1; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++) {
            if (cmp(&arr[j], &arr[best]) < 0) {
                best = j;
            }
        }
        if (best != i) {
            swap_student(&arr[i], &arr[best]);
        }
    }
}

// 3) Insertion Sort (Stable)
void insertion_sort(Student* arr, int n, CmpFunc cmp) {
    for (int i = 1; i < n; i++) {
        Student key = arr[i];
        int j = i - 1;
        while (j >= 0) {
            if (cmp(&arr[j], &key) > 0) {
                arr[j + 1] = arr[j];
                j--;
            }
            else {
                break;
            }
        }
        arr[j + 1] = key;
    }
}

// 4) Shell Sort (기본)
void shell_sort(Student* arr, int n, CmpFunc cmp) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            Student temp = arr[i];
            int j = i;
            while (j >= gap && cmp(&arr[j - gap], &temp) > 0) {
                arr[j] = arr[j - gap];
                j -= gap;
            }
            arr[j] = temp;
        }
    }
}

// 5) Quick Sort (기본)
int partition(Student* arr, int left, int right, CmpFunc cmp) {
    int mid = (left + right) / 2;
    Student pivot = arr[mid];

    swap_student(&arr[mid], &arr[right]);

    int i = left - 1;
    for (int j = left; j < right; j++) {
        if (cmp(&arr[j], &pivot) <= 0) {
            swap_student(&arr[++i], &arr[j]);
        }
    }
    swap_student(&arr[i + 1], &arr[right]);
    return i + 1;
}

void quick_sort_rec(Student* arr, int left, int right, CmpFunc cmp) {
    while (left < right) {
        int p = partition(arr, left, right, cmp);

        if (p - left < right - p) {
            if (left < p - 1)
                quick_sort_rec(arr, left, p - 1, cmp);
            left = p + 1;
        }
        else {
            if (p + 1 < right)
                quick_sort_rec(arr, p + 1, right, cmp);
            right = p - 1;
        }
    }
}

void quick_sort(Student* arr, int n, CmpFunc cmp) {
    if (n > 1)
        quick_sort_rec(arr, 0, n - 1, cmp);
}

// 6) Merge Sort (Stable)
void merge(Student* arr, int left, int mid, int right, CmpFunc cmp) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Student* L = (Student*)malloc(sizeof(Student) * n1);
    Student* R = (Student*)malloc(sizeof(Student) * n2);
    if (!L || !R) {
        fprintf(stderr, "merge: memory allocation failed\n");
        if (L) free(L);
        if (R) free(R);
        exit(1);
    }

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (cmp(&L[i], &R[j]) <= 0) {
            arr[k++] = L[i++];
        }
        else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void merge_sort_rec(Student* arr, int left, int right, CmpFunc cmp) {
    if (left < right) {
        int mid = (left + right) / 2;
        merge_sort_rec(arr, left, mid, cmp);
        merge_sort_rec(arr, mid + 1, right, cmp);
        merge(arr, left, mid, right, cmp);
    }
}

void merge_sort(Student* arr, int n, CmpFunc cmp) {
    if (n > 1)
        merge_sort_rec(arr, 0, n - 1, cmp);
}

// 7) Heap Sort
void heapify(Student* arr, int n, int i, CmpFunc cmp) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && cmp(&arr[left], &arr[largest]) > 0)
        largest = left;
    if (right < n && cmp(&arr[right], &arr[largest]) > 0)
        largest = right;

    if (largest != i) {
        swap_student(&arr[i], &arr[largest]);
        heapify(arr, n, largest, cmp);
    }
}

void heap_sort(Student* arr, int n, CmpFunc cmp) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i, cmp);

    for (int i = n - 1; i > 0; i--) {
        swap_student(&arr[0], &arr[i]);
        heapify(arr, i, 0, cmp);
    }
}

// 8) Radix Sort (ID 기준, Stable, 비교 카운트 사용 X)
void radix_sort(Student* arr, int n, CmpFunc cmp) {
    (void)cmp; // 사용하지 않음 (ID 기반 정렬)

    if (n <= 0) return;

    int max_id = arr[0].id;
    for (int i = 1; i < n; i++) {
        if (arr[i].id > max_id) max_id = arr[i].id;
    }

    Student* output = (Student*)malloc(sizeof(Student) * n);
    if (!output) {
        fprintf(stderr, "radix_sort: memory allocation failed\n");
        exit(1);
    }

    int exp = 1;
    while (max_id / exp > 0) {
        int count[10] = { 0 };

        for (int i = 0; i < n; i++) {
            int digit = (arr[i].id / exp) % 10;
            count[digit]++;
        }

        for (int i = 1; i < 10; i++) {
            count[i] += count[i - 1];
        }

        for (int i = n - 1; i >= 0; i--) {
            int digit = (arr[i].id / exp) % 10;
            output[--count[digit]] = arr[i];
        }

        for (int i = 0; i < n; i++) {
            arr[i] = output[i];
        }

        exp *= 10;
    }

    free(output);
}

// 9) Tree Sort (BST) - 재귀 제거 버전
typedef struct TreeNode {
    Student data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// 반복(iterative) BST 삽입
TreeNode* bst_insert_iter(TreeNode* root, const Student* s, CmpFunc cmp) {
    TreeNode* parent = NULL;
    TreeNode* curr = root;

    // 적절한 위치 찾기
    while (curr != NULL) {
        parent = curr;
        g_comparison_count++;
        if (cmp(s, &curr->data) <= 0) {
            curr = curr->left;
        }
        else {
            curr = curr->right;
        }
    }

    // 새 노드 생성
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) {
        fprintf(stderr, "bst_insert_iter: memory allocation failed\n");
        exit(1);
    }
    node->data = *s;
    node->left = node->right = NULL;

    // 트리가 비어있던 경우
    if (parent == NULL) {
        return node;
    }

    // 부모의 어느 쪽에 붙일지 결정
    if (cmp(s, &parent->data) <= 0)
        parent->left = node;
    else
        parent->right = node;

    return root;
}

// 반복(iterative) 중위 순회 - Tree 를 정렬된 순서로 배열에 복사
void bst_inorder_iter(TreeNode* root, Student* arr, int* index, int n) {
    if (!root) return;

    TreeNode** stack = (TreeNode**)malloc(sizeof(TreeNode*) * n);
    if (!stack) {
        fprintf(stderr, "bst_inorder_iter: memory allocation failed\n");
        exit(1);
    }

    int top = -1;
    TreeNode* curr = root;

    while (curr != NULL || top >= 0) {
        while (curr != NULL) {
            stack[++top] = curr;
            curr = curr->left;
        }

        curr = stack[top--];
        arr[(*index)++] = curr->data;
        curr = curr->right;
    }

    free(stack);
}

// 반복(iterative) 방식으로 트리 전체 free
void bst_free_iter(TreeNode* root, int n) {
    if (!root) return;

    TreeNode** stack = (TreeNode**)malloc(sizeof(TreeNode*) * n);
    if (!stack) {
        fprintf(stderr, "bst_free_iter: memory allocation failed\n");
        exit(1);
    }

    int top = -1;
    stack[++top] = root;

    while (top >= 0) {
        TreeNode* node = stack[top--];

        if (node->left)
            stack[++top] = node->left;
        if (node->right)
            stack[++top] = node->right;

        free(node);
    }

    free(stack);
}

// Tree Sort 본체 - 재귀 없는 버전 (Assignment A용 기본 트리)
void tree_sort(Student* arr, int n, CmpFunc cmp) {
    TreeNode* root = NULL;

    // 1) BST 구성
    for (int i = 0; i < n; i++) {
        root = bst_insert_iter(root, &arr[i], cmp);
    }

    // 2) 중위 순회로 정렬된 결과를 arr에 다시 채움
    int idx = 0;
    bst_inorder_iter(root, arr, &idx, n);

    // 3) 트리 메모리 해제
    bst_free_iter(root, n);
}

// -----------------
// 중복 데이터 체크
// -----------------

int check_duplicate(Student* arr, int n, SortKey key) {
    if (n <= 1) return 0;

    Student* tmp = (Student*)malloc(sizeof(Student) * n);
    if (!tmp) {
        fprintf(stderr, "check_duplicate: memory allocation failed\n");
        exit(1);
    }
    memcpy(tmp, arr, sizeof(Student) * n);

    CmpFunc cmp = NULL;
    switch (key) {
    case KEY_ID:     cmp = cmp_id_asc;     break;
    case KEY_NAME:   cmp = cmp_name_asc;   break;
    case KEY_GENDER: cmp = cmp_gender_asc; break;
    case KEY_TOTAL:  cmp = cmp_total_asc;  break;
    default:
        free(tmp);
        return 0;
    }

    // 삽입 정렬로 정렬
    for (int i = 1; i < n; i++) {
        Student key_s = tmp[i];
        int j = i - 1;
        while (j >= 0 && cmp(&tmp[j], &key_s) > 0) {
            tmp[j + 1] = tmp[j];
            j--;
        }
        tmp[j + 1] = key_s;
    }

    int has_dup = 0;
    for (int i = 0; i < n - 1; i++) {
        if (cmp(&tmp[i], &tmp[i + 1]) == 0) {
            has_dup = 1;
            break;
        }
    }

    free(tmp);
    return has_dup;
}

// -----------------------
// 정렬 성능 측정 결과 구조체
// -----------------------
typedef struct {
    double avg_comparisons; // REPEAT_COUNT회 평균 비교 횟수
    double time_ms;         // REPEAT_COUNT회 평균 정렬 시간 (ms)
    double mem_kb;          // REPEAT_COUNT회 평균 working set 증가량 (KB, 대략)
} SortStats;

// -----------------------
// REPEAT_COUNT회 반복 + 시간/메모리 평균 측정 (과제 A용)
// -----------------------
SortStats measure_sort(Student* original, int n,
    SortAlgo algo, SortKey key, SortOrder order)
{
    SortStats stats;
    stats.avg_comparisons = -1.0;
    stats.time_ms = -1.0;
    stats.mem_kb = -1.0;

    SortFunc sort_func = get_sort_func(algo);
    CmpFunc  cmp_func = get_cmp_func(key, order);

    if (!sort_func || !cmp_func) {
        return stats;
    }

    Student* work = (Student*)malloc(sizeof(Student) * n);
    if (!work) {
        fprintf(stderr, "measure_sort: memory allocation failed\n");
        exit(1);
    }

    double total_time_ms = 0.0;
    double total_mem_kb = 0.0;

    g_comparison_count = 0;

    for (int t = 0; t < REPEAT_COUNT; t++) {
        memcpy(work, original, sizeof(Student) * n);

        PROCESS_MEMORY_COUNTERS pmc_before, pmc_after;
        GetProcessMemoryInfo(GetCurrentProcess(), &pmc_before, sizeof(pmc_before));

        clock_t start = clock();
        sort_func(work, n, cmp_func);
        clock_t end = clock();

        GetProcessMemoryInfo(GetCurrentProcess(), &pmc_after, sizeof(pmc_after));

        double elapsed = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
        SIZE_T diff_ws = 0;
        if (pmc_after.WorkingSetSize > pmc_before.WorkingSetSize)
            diff_ws = pmc_after.WorkingSetSize - pmc_before.WorkingSetSize;

        total_time_ms += elapsed;
        total_mem_kb += (double)diff_ws / 1024.0;
    }

    stats.avg_comparisons = (double)g_comparison_count / (double)REPEAT_COUNT;
    stats.time_ms = total_time_ms / (double)REPEAT_COUNT;
    stats.mem_kb = total_mem_kb / (double)REPEAT_COUNT;

    free(work);
    return stats;
}

// -----------------
// 출력 도우미
// -----------------
void print_table_header(int stable_only) {
    if (stable_only)
        printf("  [Stable 알고리즘별 성능 측정 결과] (%d회 실행 평균)\n", REPEAT_COUNT);
    else
        printf("  [알고리즘별 성능 측정 결과] (%d회 실행 평균)\n", REPEAT_COUNT);

    printf("  ------------------------------------------------------------------------------\n");
    printf("  %-12s %18s %18s %12s\n",
        "Algorithm", "Avg Comparisons", "Mem Usage (KB)", "Time (ms)");
    printf("  ------------------------------------------------------------------------------\n");
}

void print_result_row(SortAlgo algo, SortStats stats) {
    const char* algo_name = get_algo_name(algo);

    printf("  %-12s %18.2f %18.2f %12.2f\n",
        algo_name,
        stats.avg_comparisons,
        stats.mem_kb,
        stats.time_ms);
}

void print_skip_row(SortAlgo algo) {
    printf("  %-12s %18s %18s %12s\n",
        get_algo_name(algo),
        "SKIP", "SKIP", "SKIP");
}

// =======================
// ===== 과제 B 부분 =====
// =======================

// --- B용: 개선 Shell Sort (gap = Knuth 수열) ---
void shell_sort_improved(Student* arr, int n, CmpFunc cmp) {
    int gap = 1;
    while (gap < n / 3) {
        gap = 3 * gap + 1; // 1, 4, 13, ...
    }

    for (; gap > 0; gap /= 3) {
        for (int i = gap; i < n; i++) {
            Student temp = arr[i];
            int j = i;
            while (j >= gap && cmp(&arr[j - gap], &temp) > 0) {
                arr[j] = arr[j - gap];
                j -= gap;
            }
            arr[j] = temp;
        }
    }
}

// --- B용: 개선 Quick Sort (median-of-three pivot) ---
static int median_of_three_idx(Student* arr, int left, int right, CmpFunc cmp) {
    int mid = (left + right) / 2;
    // 비교 횟수 카운트는 cmp 내부에서
    if (cmp(&arr[left], &arr[mid]) > 0) {
        swap_student(&arr[left], &arr[mid]);
    }
    if (cmp(&arr[left], &arr[right]) > 0) {
        swap_student(&arr[left], &arr[right]);
    }
    if (cmp(&arr[mid], &arr[right]) > 0) {
        swap_student(&arr[mid], &arr[right]);
    }
    return mid;
}

static void quick_sort_improved_rec(Student* arr, int left, int right, CmpFunc cmp) {
    while (left < right) {
        int pivot_idx = median_of_three_idx(arr, left, right, cmp);
        Student pivot = arr[pivot_idx];
        // 피봇을 맨 오른쪽으로 이동
        swap_student(&arr[pivot_idx], &arr[right]);

        int i = left - 1;
        for (int j = left; j < right; j++) {
            if (cmp(&arr[j], &pivot) <= 0) {
                swap_student(&arr[++i], &arr[j]);
            }
        }
        swap_student(&arr[i + 1], &arr[right]);
        int p = i + 1;

        // 재귀 깊이 최소화 (tail recursion 제거)
        if (p - left < right - p) {
            if (left < p - 1)
                quick_sort_improved_rec(arr, left, p - 1, cmp);
            left = p + 1;
        }
        else {
            if (p + 1 < right)
                quick_sort_improved_rec(arr, p + 1, right, cmp);
            right = p - 1;
        }
    }
}

void quick_sort_improved(Student* arr, int n, CmpFunc cmp) {
    if (n > 1)
        quick_sort_improved_rec(arr, 0, n - 1, cmp);
}

// --- B용: AVL Tree 기반 Tree Sort ---
typedef struct AVLNode {
    Student data;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

static int avl_height(AVLNode* node) { return node ? node->height : 0; }
static int avl_max(int a, int b) { return (a > b) ? a : b; }

static int avl_get_balance(AVLNode* node) {
    if (!node) return 0;
    return avl_height(node->left) - avl_height(node->right);
}

static AVLNode* avl_right_rotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = 1 + avl_max(avl_height(y->left), avl_height(y->right));
    x->height = 1 + avl_max(avl_height(x->left), avl_height(x->right));
    return x;
}

static AVLNode* avl_left_rotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = 1 + avl_max(avl_height(x->left), avl_height(x->right));
    y->height = 1 + avl_max(avl_height(y->left), avl_height(y->right));
    return y;
}

// toggle을 이용해 중복 키 시 왼/오 번갈아 삽입
static AVLNode* avl_insert_internal(AVLNode* node, const Student* s, CmpFunc cmp, int* toggle) {
    if (node == NULL) {
        AVLNode* newNode = (AVLNode*)malloc(sizeof(AVLNode));
        if (!newNode) {
            fprintf(stderr, "AVL insert: memory allocation failed\n");
            exit(1);
        }
        newNode->data = *s;
        newNode->left = newNode->right = NULL;
        newNode->height = 1;
        return newNode;
    }

    int c = cmp(s, &node->data);  // 비교 횟수는 cmp 안에서 카운트

    if (c < 0) {
        node->left = avl_insert_internal(node->left, s, cmp, toggle);
    }
    else if (c > 0) {
        node->right = avl_insert_internal(node->right, s, cmp, toggle);
    }
    else { // c == 0: 중복 키
        if (*toggle == 0) {
            node->left = avl_insert_internal(node->left, s, cmp, toggle);
            *toggle = 1;
        }
        else {
            node->right = avl_insert_internal(node->right, s, cmp, toggle);
            *toggle = 0;
        }
    }

    node->height = 1 + avl_max(avl_height(node->left), avl_height(node->right));
    int balance = avl_get_balance(node);

    // LL
    if (balance > 1 && avl_get_balance(node->left) >= 0)
        return avl_right_rotate(node);

    // LR
    if (balance > 1 && avl_get_balance(node->left) < 0) {
        node->left = avl_left_rotate(node->left);
        return avl_right_rotate(node);
    }

    // RR
    if (balance < -1 && avl_get_balance(node->right) <= 0)
        return avl_left_rotate(node);

    // RL
    if (balance < -1 && avl_get_balance(node->right) > 0) {
        node->right = avl_right_rotate(node->right);
        return avl_left_rotate(node);
    }

    return node;
}

static void avl_inorder(AVLNode* root, Student* arr, int* idx) {
    if (!root) return;
    avl_inorder(root->left, arr, idx);
    arr[(*idx)++] = root->data;
    avl_inorder(root->right, arr, idx);
}

static void avl_free(AVLNode* root) {
    if (!root) return;
    avl_free(root->left);
    avl_free(root->right);
    free(root);
}

void tree_sort_avl(Student* arr, int n, CmpFunc cmp) {
    AVLNode* root = NULL;
    int toggle = 0;
    for (int i = 0; i < n; i++) {
        root = avl_insert_internal(root, &arr[i], cmp, &toggle);
    }
    int idx = 0;
    avl_inorder(root, arr, &idx);
    avl_free(root);
}

// --- B용: REPEAT_COUNT_B(1000회) 반복 평균 비교 횟수 측정 전용 ---

static double measure_avg_comparisons_B(Student* original, int n,
    void (*sort_func)(Student*, int, CmpFunc),
    CmpFunc cmp) {
    Student* work = (Student*)malloc(sizeof(Student) * n);
    if (!work) {
        fprintf(stderr, "measure_avg_comparisons_B: malloc failed\n");
        exit(1);
    }

    g_comparison_count = 0;

    for (int t = 0; t < REPEAT_COUNT_B; t++) {   // 과제 B는 1000회 반복
        memcpy(work, original, sizeof(Student) * n);
        sort_func(work, n, cmp);
    }

    double avg = (double)g_comparison_count / (double)REPEAT_COUNT_B;
    free(work);
    return avg;
}

// --- B용: 실행 함수 ---
void run_assignment_B(Student* students, int n) {
    printf("\n\n");
    printf("\n\n");
    printf("==============================================================================\n");
    printf("                 Student Sorting Performance Test (Assignment B)\n");
    printf("        (Improved Shell / Quick / Tree sort - Minimize comparisons)\n");
    printf("==============================================================================\n");
    printf("  - 각 정렬은 %d회 반복 수행 후, 평균 비교 횟수를 출력합니다.\n", REPEAT_COUNT_B);
    printf("==============================================================================\n\n");

    double shell_asc, quick_asc, tree_asc;
    double shell_desc, quick_desc, tree_desc;

    // [ID ASC]
    printf("[ID ASC]\n");
    printf("  Algorithm            Avg Comparisons\n");
    printf("  ---------------------------------------------\n");
    shell_asc = measure_avg_comparisons_B(students, n,
        shell_sort_improved, cmp_id_asc);
    quick_asc = measure_avg_comparisons_B(students, n,
        quick_sort_improved, cmp_id_asc);
    tree_asc = measure_avg_comparisons_B(students, n,
        tree_sort_avl, cmp_id_asc);
    printf("  Shell_Imp               %.2f\n", shell_asc);
    printf("  Quick_Imp               %.2f\n", quick_asc);
    printf("  Tree_AVL                %.2f\n\n", tree_asc);

    // [ID DESC]
    printf("[ID DESC]\n");
    printf("  Algorithm            Avg Comparisons\n");
    printf("  ---------------------------------------------\n");
    shell_desc = measure_avg_comparisons_B(students, n,
        shell_sort_improved, cmp_id_desc);
    quick_desc = measure_avg_comparisons_B(students, n,
        quick_sort_improved, cmp_id_desc);
    tree_desc = measure_avg_comparisons_B(students, n,
        tree_sort_avl, cmp_id_desc);
    printf("  Shell_Imp               %.2f\n", shell_desc);
    printf("  Quick_Imp               %.2f\n", quick_desc);
    printf("  Tree_AVL                %.2f\n\n", tree_desc);

    // [NAME ASC]
    printf("[NAME ASC]\n");
    printf("  Algorithm            Avg Comparisons\n");
    printf("  ---------------------------------------------\n");
    shell_asc = measure_avg_comparisons_B(students, n,
        shell_sort_improved, cmp_name_asc);
    quick_asc = measure_avg_comparisons_B(students, n,
        quick_sort_improved, cmp_name_asc);
    tree_asc = measure_avg_comparisons_B(students, n,
        tree_sort_avl, cmp_name_asc);
    printf("  Shell_Imp               %.2f\n", shell_asc);
    printf("  Quick_Imp               %.2f\n", quick_asc);
    printf("  Tree_AVL                %.2f\n\n", tree_asc);

    // [NAME DESC]
    printf("[NAME DESC]\n");
    printf("  Algorithm            Avg Comparisons\n");
    printf("  ---------------------------------------------\n");
    shell_desc = measure_avg_comparisons_B(students, n,
        shell_sort_improved, cmp_name_desc);
    quick_desc = measure_avg_comparisons_B(students, n,
        quick_sort_improved, cmp_name_desc);
    tree_desc = measure_avg_comparisons_B(students, n,
        tree_sort_avl, cmp_name_desc);
    printf("  Shell_Imp               %.2f\n", shell_desc);
    printf("  Quick_Imp               %.2f\n", quick_desc);
    printf("  Tree_AVL                %.2f\n\n", tree_desc);

    // [GENDER ASC]
    printf("[GENDER ASC]\n");
    printf("  Algorithm            Avg Comparisons\n");
    printf("  ---------------------------------------------\n");
    shell_asc = measure_avg_comparisons_B(students, n,
        shell_sort_improved, cmp_gender_asc);
    quick_asc = measure_avg_comparisons_B(students, n,
        quick_sort_improved, cmp_gender_asc);
    tree_asc = measure_avg_comparisons_B(students, n,
        tree_sort_avl, cmp_gender_asc);
    printf("  Shell_Imp                %.2f\n", shell_asc);
    printf("  Quick_Imp                %.2f\n", quick_asc);
    printf("  Tree_AVL                 %.2f\n\n", tree_asc);

    // [GENDER DESC]
    printf("[GENDER DESC]\n");
    printf("  Algorithm            Avg Comparisons\n");
    printf("  ---------------------------------------------\n");
    shell_desc = measure_avg_comparisons_B(students, n,
        shell_sort_improved, cmp_gender_desc);
    quick_desc = measure_avg_comparisons_B(students, n,
        quick_sort_improved, cmp_gender_desc);
    tree_desc = measure_avg_comparisons_B(students, n,
        tree_sort_avl, cmp_gender_desc);
    printf("  Shell_Imp                %.2f\n", shell_desc);
    printf("  Quick_Imp                %.2f\n", quick_desc);
    printf("  Tree_AVL                 %.2f\n\n", tree_desc);

    // [TOTAL ASC]
    printf("[TOTAL ASC]\n");
    printf("  Algorithm            Avg Comparisons\n");
    printf("  ---------------------------------------------\n");
    shell_asc = measure_avg_comparisons_B(students, n,
        shell_sort_improved, cmp_total_asc);
    quick_asc = measure_avg_comparisons_B(students, n,
        quick_sort_improved, cmp_total_asc);
    tree_asc = measure_avg_comparisons_B(students, n,
        tree_sort_avl, cmp_total_asc);
    printf("  Shell_Imp               %.2f\n", shell_asc);
    printf("  Quick_Imp               %.2f\n", quick_asc);
    printf("  Tree_AVL                %.2f\n\n", tree_asc);

    // [TOTAL DESC]
    printf("[TOTAL DESC]\n");
    printf("  Algorithm            Avg Comparisons\n");
    printf("  ---------------------------------------------\n");
    shell_desc = measure_avg_comparisons_B(students, n,
        shell_sort_improved, cmp_total_desc);
    quick_desc = measure_avg_comparisons_B(students, n,
        quick_sort_improved, cmp_total_desc);
    tree_desc = measure_avg_comparisons_B(students, n,
        tree_sort_avl, cmp_total_desc);
    printf("  Shell_Imp               %.2f\n", shell_desc);
    printf("  Quick_Imp               %.2f\n", quick_desc);
    printf("  Tree_AVL                %.2f\n\n", tree_desc);
}

// -----------------
// main (과제 A + B)
// -----------------
int main(void) {
    const char* filename = "데이터.csv";
    int n = 0;

    Student* students = load_students(filename, &n);
    if (!students) {
        fprintf(stderr, "Failed to load students.\n");
        return 1;
    }

    // ===== Assignment A 배너 =====
    printf("==============================================================================\n");
    printf("                             Assignment A\n");
    printf("                   Student Sorting Performance Test\n");
    printf("==============================================================================\n");
    printf(" 학생 수     : %d명\n", n);
    printf(" 반복 횟수   : %d회/알고리즘\n", REPEAT_COUNT);
    printf("------------------------------------------------------------------------------\n");
    printf(" ※ 각 알고리즘에 대해\n");
    printf("    - 비교 횟수 : %d회 실행 후 평균\n", REPEAT_COUNT);
    printf("    - 시간(ms)  : %d회 정렬 시간 평균 (clock 기반)\n", REPEAT_COUNT);
    printf("    - 메모리(KB): %d회 정렬 전/후 Working Set 차이 평균\n", REPEAT_COUNT);
    printf("------------------------------------------------------------------------------\n");
    printf(" ※ 각 알고리즘을 1000회 반복하여 평균을 구하는 것이지만, 데이터 개수가 많아\n");
    printf("   실행 시간이 과도하게 길어져, 전반적으로  %d회로 조정하여 측정 \n", REPEAT_COUNT);
    printf(" ※ 메모리 사용량(Mem Usage)은 Windows Working Set 크기 변화를 이용해 측정하며,\n");
    printf("   정렬로 인한 추가 메모리 사용이 작아 대부분 0.00KB로 나타날 수 있음\n");
    printf(" ※ 기수 정렬(Radix Sort)은 정수형 ID 기준으로만 구현하였으며, NAME/GENDER/\n");
    printf("   TOTAL 기준에서는 비교 기반 정렬 알고리즘만 사용\n");
    printf(" ※ Tree Sort는 재귀 제거 버전으로 구현하여, 스택 오버플로우 없이 성능 비교에 포함\n");
    printf(" ※ 중복 키가 존재하는 경우 Heap Sort는 성능 비교에서 제외하여 SKIP으로 표시\n");
    printf("==============================================================================\n");

    // 기준별 중복 여부 체크
    int dup_id = check_duplicate(students, n, KEY_ID);
    int dup_name = check_duplicate(students, n, KEY_NAME);
    int dup_gender = check_duplicate(students, n, KEY_GENDER);
    int dup_total = check_duplicate(students, n, KEY_TOTAL);

    // 1. ID 오름차순
    printf("\n[정렬 기준 1] ID 오름차순 (Ascending by ID)\n");
    printf("  중복 ID 존재 여부 : %s\n\n", dup_id ? "YES" : "NO");
    print_table_header(0);
    for (int algo = 0; algo < SORT_ALGO_COUNT; algo++) {
        if (dup_id && ((SortAlgo)algo == SORT_HEAP)) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        SortStats stats = measure_sort(students, n, (SortAlgo)algo, KEY_ID, ORDER_ASC);
        if (stats.avg_comparisons < 0.0) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        print_result_row((SortAlgo)algo, stats);
    }
    printf("  ------------------------------------------------------------------------------\n");

    // 2. ID 내림차순
    printf("\n[정렬 기준 2] ID 내림차순 (Descending by ID)\n");
    printf("  중복 ID 존재 여부 : %s\n\n", dup_id ? "YES" : "NO");
    print_table_header(0);
    for (int algo = 0; algo < SORT_ALGO_COUNT; algo++) {
        if (dup_id && ((SortAlgo)algo == SORT_HEAP)) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        SortStats stats = measure_sort(students, n, (SortAlgo)algo, KEY_ID, ORDER_DESC);
        if (stats.avg_comparisons < 0.0) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        print_result_row((SortAlgo)algo, stats);
    }
    printf("  ------------------------------------------------------------------------------\n");

    // 3. NAME 오름차순
    printf("\n[정렬 기준 3] NAME 오름차순 (Ascending by NAME)\n");
    printf("  중복 NAME 존재 여부 : %s\n\n", dup_name ? "YES" : "NO");
    print_table_header(0);
    for (int algo = 0; algo < SORT_ALGO_COUNT; algo++) {
        if (dup_name && ((SortAlgo)algo == SORT_HEAP || (SortAlgo)algo == SORT_TREE)) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        SortStats stats = measure_sort(students, n, (SortAlgo)algo, KEY_NAME, ORDER_ASC);
        if (stats.avg_comparisons < 0.0) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        print_result_row((SortAlgo)algo, stats);
    }
    printf("  ------------------------------------------------------------------------------\n");

    // 4. NAME 내림차순
    printf("\n[정렬 기준 4] NAME 내림차순 (Descending by NAME)\n");
    printf("  중복 NAME 존재 여부 : %s\n\n", dup_name ? "YES" : "NO");
    print_table_header(0);
    for (int algo = 0; algo < SORT_ALGO_COUNT; algo++) {
        if (dup_name && ((SortAlgo)algo == SORT_HEAP || (SortAlgo)algo == SORT_TREE)) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        SortStats stats = measure_sort(students, n, (SortAlgo)algo, KEY_NAME, ORDER_DESC);
        if (stats.avg_comparisons < 0.0) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        print_result_row((SortAlgo)algo, stats);
    }
    printf("  ------------------------------------------------------------------------------\n");

    // 5. GENDER 오름차순 (Stable 정렬만 사용: Bubble, Insertion, Merge)
    printf("\n[정렬 기준 5] GENDER 오름차순 (F < M, 문자열 기준)\n");
    printf("  중복 GENDER 존재 여부 : %s\n", dup_gender ? "YES" : "NO");
    printf("  - Stable 정렬 알고리즘만 사용: Bubble, Insertion, Merge\n\n");
    print_table_header(1);
    for (int algo = 0; algo < SORT_ALGO_COUNT; algo++) {
        // Tree는 제외: Bubble, Insertion, Merge만 사용
        if (!((SortAlgo)algo == SORT_BUBBLE ||
            (SortAlgo)algo == SORT_INSERTION ||
            (SortAlgo)algo == SORT_MERGE))
            continue;

        SortStats stats = measure_sort(students, n, (SortAlgo)algo, KEY_GENDER, ORDER_ASC);
        if (stats.avg_comparisons < 0.0) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        print_result_row((SortAlgo)algo, stats);
    }
    printf("  ------------------------------------------------------------------------------\n");

    // 6. GENDER 내림차순 (Stable 정렬만 사용: Bubble, Insertion, Merge)
    printf("\n[정렬 기준 6] GENDER 내림차순 (M < F, 문자열 기준)\n");
    printf("  중복 GENDER 존재 여부 : %s\n", dup_gender ? "YES" : "NO");
    printf("  - Stable 정렬 알고리즘만 사용: Bubble, Insertion, Merge\n\n");
    print_table_header(1);
    for (int algo = 0; algo < SORT_ALGO_COUNT; algo++) {
        // Tree는 제외: Bubble, Insertion, Merge만 사용
        if (!((SortAlgo)algo == SORT_BUBBLE ||
            (SortAlgo)algo == SORT_INSERTION ||
            (SortAlgo)algo == SORT_MERGE))
            continue;

        SortStats stats = measure_sort(students, n, (SortAlgo)algo, KEY_GENDER, ORDER_DESC);
        if (stats.avg_comparisons < 0.0) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        print_result_row((SortAlgo)algo, stats);
    }
    printf("  ------------------------------------------------------------------------------\n");

    // 7. TOTAL 오름차순
    printf("\n[정렬 기준 7] TOTAL GRADE 오름차순\n");
    printf("  중복 TOTAL 존재 여부 : %s\n", dup_total ? "YES" : "NO");
    printf("  - 동점 시: Korean > English > Math 점수 큰 순\n\n");
    print_table_header(0);
    for (int algo = 0; algo < SORT_ALGO_COUNT; algo++) {
        if (dup_total && ((SortAlgo)algo == SORT_HEAP || (SortAlgo)algo == SORT_TREE)) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        SortStats stats = measure_sort(students, n, (SortAlgo)algo, KEY_TOTAL, ORDER_ASC);
        if (stats.avg_comparisons < 0.0) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        print_result_row((SortAlgo)algo, stats);
    }
    printf("  ------------------------------------------------------------------------------\n");

    // 8. TOTAL 내림차순
    printf("\n[정렬 기준 8] TOTAL GRADE 내림차순\n");
    printf("  중복 TOTAL 존재 여부 : %s\n", dup_total ? "YES" : "NO");
    printf("  - 동점 시: Korean > English > Math 점수 큰 순\n\n");
    print_table_header(0);
    for (int algo = 0; algo < SORT_ALGO_COUNT; algo++) {
        if (dup_total && ((SortAlgo)algo == SORT_HEAP || (SortAlgo)algo == SORT_TREE)) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        SortStats stats = measure_sort(students, n, (SortAlgo)algo, KEY_TOTAL, ORDER_DESC);
        if (stats.avg_comparisons < 0.0) {
            print_skip_row((SortAlgo)algo);
            continue;
        }
        print_result_row((SortAlgo)algo, stats);
    }
    printf("  ------------------------------------------------------------------------------\n");

    // ==== 여기까지 Assignment A ====

    // ==== Assignment B 수행 (1000회 평균 비교 횟수) ====
    run_assignment_B(students, n);

    free(students);
    return 0;
}