/*
 * 과제: 그래프 표현 방식(인접 행렬 vs 인접 리스트) 성능 비교
 *
 * 요구사항:
 * 1. 정점(V) 100개
 * 2. 희소 그래프 (간선 E=100), 밀집 그래프 (간선 E=4000)
 * 3. 4가지 케이스 테스트:
 * - 희소 + 인접 행렬
 * - 희소 + 인접 리스트
 * - 밀집 + 인접 행렬
 * - 밀집 + 인접 리스트
 * 4. 측정 항목:
 * - 메모리 사용량
 * - 간선 삽입/삭제 시 비교 횟수
 * - 두 정점 연결 확인 시 비교 횟수
 * - 한 노드의 인접 노드 출력 시 비교 횟수
 */

#include <stdio.h>   // 표준 입출력 (printf 등)
#include <stdlib.h>  // 동적 메모리 할당 (malloc, free, calloc)
#include <time.h>    // 랜덤 시드 생성을 위한 time()

 // --- 상수 정의 ---
 // 코드의 가독성 및 유지보수를 위해 상수를 정의합니다.
#define VERTICES 100      // 정점의 개수 (V)
#define E_SPARSE 100      // 희소 그래프의 간선 개수
#define E_DENSE 4000      // 밀집 그래프의 간선 개수
#define SUPPRESS_PRINT 1  // 1로 설정 시, 인접 노드 출력 테스트에서 실제 출력을 억제
                          // (성능 측정에 집중하기 위함)

// --- 성능 측정용 구조체 ---
// 각 테스트 케이스에서 발생하는 비교 횟수를 저장하기 위한 구조체입니다.
typedef struct Metrics {
    long long insert_delete_comps;  // 간선 삽입/삭제 시 누적 비교 횟수
    long long check_edge_comps;     // 두 정점 연결 확인 시 누적 비교 횟수
    long long print_neighbors_comps; // 인접 노드 출력 시 누적 비교 횟수
} Metrics;


/*
 * ===================================================================
 * 섹션 1: 인접 리스트 (Adjacency List) 구현
 * ===================================================================
 * - O(V+E)의 공간 복잡도.
 * - 희소 그래프에 유리합니다.
 * - 간선 탐색(확인, 삭제) 및 인접 노드 출력은 해당 정점의 차수(degree)에 비례합니다.
 */

 // 인접 리스트의 노드 구조체 (하나의 '간선'을 나타냄)
typedef struct AdjListNode {
    int dest;                    // 목적지 정점 (destination)
    struct AdjListNode* next;    // 다음 인접 노드를 가리키는 포인터
} AdjListNode;

// 인접 리스트 (특정 정점에 연결된 이웃들의 리스트)
typedef struct AdjList {
    AdjListNode* head;           // 리스트의 시작(헤드) 포인터
} AdjList;

// 인접 리스트 그래프 전체를 나타내는 구조체
typedef struct GraphAdjList {
    int V;                       // 그래프의 총 정점 개수
    AdjList* array;              // 각 정점의 인접 리스트(헤드 포인터)를 담는 배열
    long long memory_usage;      // 그래프 생성 및 간선 추가 시 사용된 총 메모리
} GraphAdjList;

// 새 인접 리스트 노드(간선)를 생성하는 헬퍼 함수
AdjListNode* newAdjListNode(int dest) {
    // 노드 크기만큼 동적 메모리 할당
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL; // 다음 노드는 아직 없음
    return newNode;
}

// 인접 리스트 방식의 그래프를 생성하는 함수
GraphAdjList* createGraph_AdjList(int V) {
    // 1. 그래프 구조체 자체의 메모리 할당
    GraphAdjList* graph = (GraphAdjList*)malloc(sizeof(GraphAdjList));
    graph->V = V;

    // 2. V개의 정점만큼 '인접 리스트 헤드' 배열 할당
    graph->array = (AdjList*)malloc(V * sizeof(AdjList));

    // 3. 기본 메모리 사용량 계산
    // (그래프 구조체 크기 + V개의 리스트 헤드 배열 크기)
    graph->memory_usage = sizeof(GraphAdjList) + (V * sizeof(AdjList));

    // 4. 모든 정점의 리스트 헤드를 NULL로 초기화 (아직 간선 없음)
    for (int i = 0; i < V; ++i) {
        graph->array[i].head = NULL;
    }
    return graph;
}

// (인접 리스트) 간선 삽입 함수 (비교 횟수 카운트)
// m: 성능 측정을 위한 Metrics 구조체 포인터
void insertEdge_AdjList(GraphAdjList* graph, int src, int dest, Metrics* m) {

    // --- 1. (src -> dest) 간선 추가 ---
    // 중복 확인: 삽입 전, 이미 간선이 존재하는지 확인합니다.
    AdjListNode* checker = graph->array[src].head;
    while (checker) {
        if (m) m->insert_delete_comps++; // [비교 1회] 리스트 노드와 dest 비교
        if (checker->dest == dest) return; // 이미 존재하므로 함수 종료
        checker = checker->next;
    }
    // (m이 NULL이 아니라면) 마지막 NULL 체크도 비교 횟수에 포함
    if (m) m->insert_delete_comps++; // [비교 1회] while문 종료 (NULL)

    // 새 노드 생성 및 리스트의 맨 앞에 삽입 (O(1))
    AdjListNode* newNode = newAdjListNode(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // 노드가 추가되었으므로 메모리 사용량 갱신
    if (m != NULL) { // (랜덤 생성 시에는 m=NULL이므로 갱신 안 함)
        graph->memory_usage += sizeof(AdjListNode);
    }

    // --- 2. (dest -> src) 간선 추가 (무방향 그래프이므로) ---
    // 위와 동일한 로직을 dest -> src 에도 적용
    checker = graph->array[dest].head;
    while (checker) {
        if (m) m->insert_delete_comps++; // [비교 1회]
        if (checker->dest == src) return; // (이미 위에서 src->dest를 넣었으면 여기 걸리면 안됨)
        checker = checker->next;
    }
    if (m) m->insert_delete_comps++; // [비교 1회]

    newNode = newAdjListNode(src);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
    if (m != NULL) {
        graph->memory_usage += sizeof(AdjListNode);
    }
}

// (인접 리스트) 간선 삭제 함수 (비교 횟수 카운트)
void deleteEdge_AdjList(GraphAdjList* graph, int src, int dest, Metrics* m) {

    // --- 1. (src -> dest) 간선 삭제 ---
    AdjListNode* curr = graph->array[src].head;
    AdjListNode* prev = NULL; // 삭제할 노드의 이전 노드를 추적

    // 삭제할 노드(dest) 탐색
    while (curr) {
        m->insert_delete_comps++; // [비교 1회] dest와 일치하는지 비교
        if (curr->dest == dest) break; // 찾았음
        prev = curr;
        curr = curr->next;
    }
    if (m && curr == NULL) m->insert_delete_comps++; // [비교 1회] (못 찾고 NULL 종료)

    if (curr) { // 노드를 찾았다면
        if (prev == NULL) { // 삭제할 노드가 헤드 노드인 경우
            graph->array[src].head = curr->next;
        }
        else { // 삭제할 노드가 중간 노드인 경우
            prev->next = curr->next;
        }
        free(curr); // 노드 메모리 해제
        graph->memory_usage -= sizeof(AdjListNode); // 메모리 사용량 갱신
    }

    // --- 2. (dest -> src) 간선 삭제 (무방향 그래프이므로) ---
    // 위와 동일한 로직 적용
    curr = graph->array[dest].head;
    prev = NULL;
    while (curr) {
        m->insert_delete_comps++; // [비교 1회]
        if (curr->dest == src) break;
        prev = curr;
        curr = curr->next;
    }
    if (m && curr == NULL) m->insert_delete_comps++; // [비교 1회]

    if (curr) {
        if (prev == NULL) graph->array[dest].head = curr->next;
        else prev->next = curr->next;
        free(curr);
        graph->memory_usage -= sizeof(AdjListNode);
    }
}

// (인접 리스트) 두 정점 연결 확인 함수 (비교 횟수 카운트)
int checkEdge_AdjList(GraphAdjList* graph, int src, int dest, Metrics* m) {
    AdjListNode* curr = graph->array[src].head;

    // src의 인접 리스트를 순회하며 dest가 있는지 탐색
    while (curr) {
        m->check_edge_comps++; // [비교 1회] 현재 노드가 dest인지 비교
        if (curr->dest == dest) return 1; // 찾았음 (연결됨)
        curr = curr->next;
    }
    // 마지막 NULL 체크
    if (m) m->check_edge_comps++; // [비교 1회] (못 찾고 NULL 종료)

    return 0; // 못 찾았음 (연결 안 됨)
}

// (인접 리스트) 특정 노드의 인접 노드 출력 (비교 횟수 카운트)
void printNeighbors_AdjList(GraphAdjList* graph, int v, Metrics* m) {
    AdjListNode* curr = graph->array[v].head;

    // SUPPRESS_PRINT 플래그가 0일 때만 실제 출력
    if (!SUPPRESS_PRINT) printf("정점 %d의 인접 노드: ", v);

    // 해당 정점의 인접 리스트를 끝까지 순회
    while (curr) {
        m->print_neighbors_comps++; // [비교 1회] 루프 조건(curr != NULL) 확인

        if (!SUPPRESS_PRINT) printf("%d ", curr->dest);
        curr = curr->next;
    }

    // 마지막 NULL 체크
    if (m) m->print_neighbors_comps++; // [비교 1회] (루프 종료)

    if (!SUPPRESS_PRINT) printf("\n");
}

// (인접 리스트) 그래프 메모리 해제
void freeGraph_AdjList(GraphAdjList* graph) {
    // 모든 정점을 순회
    for (int v = 0; v < graph->V; ++v) {
        AdjListNode* curr = graph->array[v].head;
        // 각 정점의 인접 리스트에 있는 모든 노드(간선)들을 해제
        while (curr) {
            AdjListNode* temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
    // 정점 헤드 포인터 배열 해제
    free(graph->array);
    // 그래프 구조체 자체 해제
    free(graph);
}


/*
 * ===================================================================
 * 섹션 2: 인접 행렬 (Adjacency Matrix) 구현
 * ===================================================================
 * - O(V^2)의 공간 복잡도.
 * - 밀집 그래프에 유리합니다.
 * - 간선 삽입/삭제/확인은 O(1)로 매우 빠릅니다.
 * - 인접 노드 출력은 O(V)로, 항상 모든 정점을 확인해야 합니다.
 */

 // 인접 행렬 그래프 구조체
typedef struct GraphAdjMatrix {
    int V;        // 총 정점 개수
    int** matrix; // V x V 크기의 2차원 배열을 가리키는 포인터
} GraphAdjMatrix;

// 인접 행렬 방식의 그래프를 생성하는 함수
GraphAdjMatrix* createGraph_Matrix(int V) {
    // 1. 그래프 구조체 자체 메모리 할당
    GraphAdjMatrix* graph = (GraphAdjMatrix*)malloc(sizeof(GraphAdjMatrix));
    graph->V = V;

    // 2. 2D 배열 할당 (1단계: 행 포인터 배열 할당)
    // int* (포인터) V개를 저장할 공간 할당
    graph->matrix = (int**)malloc(V * sizeof(int*));

    // 3. 2D 배열 할당 (2단계: 각 행의 실제 데이터 공간 할당)
    for (int i = 0; i < V; i++) {
        // V개의 int를 저장할 공간 할당
        // calloc을 사용하여 모든 값을 0 (간선 없음)으로 자동 초기화
        graph->matrix[i] = (int*)calloc(V, sizeof(int));
    }
    return graph;
}

// (인접 행렬) 간선 삽입 함수 (비교 횟수 카운트)
void insertEdge_Matrix(GraphAdjMatrix* graph, int src, int dest, Metrics* m) {
    // O(1) 접근
    // 삽입 전, 0인지 확인 (이 확인 과정을 '비교'로 간주)
    m->insert_delete_comps++; // [비교 1회]
    if (graph->matrix[src][dest] == 0) {
        graph->matrix[src][dest] = 1; // (src -> dest) 연결
        graph->matrix[dest][src] = 1; // (dest -> src) 연결 (무방향)
    }
}

// (인접 행렬) 간선 삭제 함수 (비교 횟수 카운트)
void deleteEdge_Matrix(GraphAdjMatrix* graph, int src, int dest, Metrics* m) {
    // O(1) 접근
    // 삭제 전, 1인지 확인 (이 확인 과정을 '비교'로 간주)
    m->insert_delete_comps++; // [비교 1회]
    if (graph->matrix[src][dest] == 1) {
        graph->matrix[src][dest] = 0; // (src -> dest) 연결 해제
        graph->matrix[dest][src] = 0; // (dest -> src) 연결 해제 (무방향)
    }
}

// (인접 행렬) 두 정점 연결 확인 함수 (비교 횟수 카운트)
int checkEdge_Matrix(GraphAdjMatrix* graph, int src, int dest, Metrics* m) {
    // O(1) 접근
    // matrix[src][dest] 값이 1인지 확인하는 것 자체를 '비교'로 간주
    m->check_edge_comps++; // [비교 1회]
    return graph->matrix[src][dest] == 1;
}

// (인접 행렬) 특정 노드의 인접 노드 출력 (비교 횟수 카운트)
void printNeighbors_Matrix(GraphAdjMatrix* graph, int v, Metrics* m) {
    if (!SUPPRESS_PRINT) printf("정점 %d의 인접 노드: ", v);

    // 해당 정점(v)의 '행(row)'을 모두 순회해야 함 (O(V))
    for (int i = 0; i < graph->V; i++) {
        // v와 i가 연결되어 있는지 (값이 1인지) 확인
        m->print_neighbors_comps++; // [비교 1회] (매 루프마다 1인지 확인)
        if (graph->matrix[v][i] == 1) {
            if (!SUPPRESS_PRINT) printf("%d ", i);
        }
    }
    if (!SUPPRESS_PRINT) printf("\n");
}

// (인접 행렬) 그래프 메모리 사용량 계산
long getMemory_Matrix(int V) {
    long mem = 0;
    mem += sizeof(GraphAdjMatrix);      // 1. 그래프 구조체 자체 크기
    mem += V * sizeof(int*);            // 2. 행 포인터 배열 크기 (V개)
    mem += (long)V * V * sizeof(int);   // 3. V*V 개의 실제 데이터(int) 크기
    return mem;
}

// (인접 행렬) 그래프 메모리 해제
void freeGraph_Matrix(GraphAdjMatrix* graph) {
    // 할당의 역순으로 해제
    // 1. 각 행의 실제 데이터 공간 해제
    for (int i = 0; i < graph->V; i++) {
        free(graph->matrix[i]);
    }
    // 2. 행 포인터 배열 해제
    free(graph->matrix);
    // 3. 그래프 구조체 자체 해제
    free(graph);
}


/*
 * ===================================================================
 * 섹션 3: 랜덤 그래프 생성 및 테스트 실행
 * ===================================================================
 */

 // (성능 측정 없이) 간선 존재 여부 확인하는 헬퍼 함수
 // (랜덤 그래프 생성 시 중복 간선을 피하기 위해 사용)
int checkEdge_Matrix_no_metric(GraphAdjMatrix* graph, int src, int dest) {
    return graph->matrix[src][dest] == 1;
}
int checkEdge_AdjList_no_metric(GraphAdjList* graph, int src, int dest) {
    AdjListNode* curr = graph->array[src].head;
    while (curr) {
        if (curr->dest == dest) return 1;
        curr = curr->next;
    }
    return 0;
}

// E개의 간선을 가진 랜덤 그래프 생성
// (void* graph: 인접 행렬 또는 인접 리스트 그래프 포인터)
// (isMatrix: 1이면 행렬, 0이면 리스트)
void generateRandomGraph(int V, int E, void* graph, int isMatrix) {
    int edges_added = 0;

    // 간선 중복 추가를 막기 위한 임시 V x V 배열
    // (메모리를 더 쓰지만 정확히 E개의 간선을 생성하기 위함)
    int** added = (int**)malloc(V * sizeof(int*));
    for (int i = 0; i < V; i++) {
        added[i] = (int*)calloc(V, sizeof(int));
    }

    // E개의 간선이 추가될 때까지 반복
    while (edges_added < E) {
        int u = rand() % V; // 랜덤 정점 u
        int v = rand() % V; // 랜덤 정점 v

        // (u != v) : 자기 자신을 가리키는 루프 방지
        // (added[u][v] == 0) : 이미 추가된 간선인지 확인
        if (u != v && added[u][v] == 0) {

            // (isMatrix 여부에 따라 그래프 타입 캐스팅)
            if (isMatrix) {
                // (성능 측정 없는 단순 삽입)
                GraphAdjMatrix* g = (GraphAdjMatrix*)graph;
                g->matrix[u][v] = 1;
                g->matrix[v][u] = 1;
            }
            else {
                // (성능 측정을 위해 NULL 전달)
                insertEdge_AdjList((GraphAdjList*)graph, u, v, NULL);
            }

            // 중복 방지 배열에 기록
            added[u][v] = 1;
            added[v][u] = 1;
            edges_added++; // 추가된 간선 수 증가
        }
    }

    // 임시 중복 방지 배열 해제
    for (int i = 0; i < V; i++) {
        free(added[i]);
    }
    free(added);
}

// 4가지 케이스를 실행하는 메인 테스트 함수
void runTestCase(int V, int E, int isMatrix) {

    // 1. 성능 측정을 위한 Metrics 변수 초기화
    Metrics metrics = { 0, 0, 0 }; // 모든 카운터를 0으로
    void* graph;                 // 그래프 포인터 (어느 타입이든 가리킬 수 있게)
    long memory = 0;             // 메모리 사용량

    // --- 1. 그래프 생성 및 랜덤 간선 채우기 ---
    if (isMatrix) {
        graph = createGraph_Matrix(V);
        generateRandomGraph(V, E, graph, 1);
        // (측정 항목 1) 메모리 사용량 계산
        memory = getMemory_Matrix(V);
    }
    else {
        graph = createGraph_AdjList(V);
        generateRandomGraph(V, E, graph, 0);
        // (측정 항목 1) 메모리 사용량 계산 (실제 할당된 노드 포함)
        // (참고: 랜덤 생성 시 사용된 insertEdge_AdjList(m=NULL)는 메모리를 계산하지 않았으므로,
        //  정확한 계산을 위해선 generateRandomGraph에서 memory_usage를 갱신해야 함.
        //  (현재 코드는 간선 추가/삭제 테스트 시의 메모리만 갱신 -> 초기 메모리로 수정))

        // (수정) generateRandomGraph가 끝난 시점의 정확한 메모리 계산
        GraphAdjList* g_list = (GraphAdjList*)graph;
        g_list->memory_usage = sizeof(GraphAdjList) + (V * sizeof(AdjList)) + (2LL * E * sizeof(AdjListNode));
        memory = g_list->memory_usage;
    }

    // --- 2. 성능 측정 시작 (항목별 1회 랜덤 실행) ---

    // (측정 항목 2) 추가 간선 삽입/삭제 비교
    int u, v;
    // 테스트를 위해, 그래프에 '존재하지 않는' 새 간선을 랜덤하게 찾음
    do {
        u = rand() % V;
        v = rand() % V;
    } while (u == v || (isMatrix ? checkEdge_Matrix_no_metric(graph, u, v)
        : checkEdge_AdjList_no_metric(graph, u, v)));

    if (isMatrix) {
        insertEdge_Matrix(graph, u, v, &metrics); // 삽입 (비교 횟수 누적)
        deleteEdge_Matrix(graph, u, v, &metrics); // 삭제 (비교 횟수 누적)
    }
    else {
        insertEdge_AdjList(graph, u, v, &metrics); // 삽입 (비교 횟수 누적)
        deleteEdge_AdjList(graph, u, v, &metrics); // 삭제 (비교 횟수 누적)
    }

    // (측정 항목 3) 두 정점의 연결 여부 확인 비교
    // (이 테스트는 간선이 있든 없든 상관없음)
    u = rand() % V;
    v = rand() % V;
    if (isMatrix) {
        checkEdge_Matrix(graph, u, v, &metrics);
    }
    else {
        checkEdge_AdjList(graph, u, v, &metrics);
    }

    // (측정 항목 4) 특정 노드에 연결된 인접 노드 출력 비교
    v = rand() % V; // 랜덤 정점 v 선택
    if (isMatrix) {
        printNeighbors_Matrix(graph, v, &metrics);
    }
    else {
        printNeighbors_AdjList(graph, v, &metrics);
    }

    // --- 3. 결과 출력 ---
    // (요구사항의 예시 출력 포맷)
    printf("메모리 %ld Bytes\n", memory);
    printf("간선 삽입/삭제 비교 %lld번\n", metrics.insert_delete_comps);
    printf("두 정점의 연결 확인 비교 %lld번\n", metrics.check_edge_comps);
    printf("한 노드의 인접 노드 출력 비교 %lld번\n", metrics.print_neighbors_comps);

    // --- 4. 메모리 해제 ---
    if (isMatrix) {
        freeGraph_Matrix(graph);
    }
    else {
        freeGraph_AdjList(graph);
    }
}


// --- 메인 함수 (프로그램 시작점) ---
int main() {
    // 랜덤 시드 초기화 (매번 다른 랜덤 값을 얻기 위함)
    srand((unsigned int)time(NULL));

    printf("정점 100개, 희소 그래프(간선 100개), 밀집 그래프(간선 4000개) 성능 비교\n");
    printf("-------------------------------------------------------------\n\n");

    // 케이스 1: 희소그래프 (E=100) - 인접행렬
    printf("케이스 1: 희소그래프 (V=%d, E=%d) - 인접행렬\n", VERTICES, E_SPARSE);
    runTestCase(VERTICES, E_SPARSE, 1); // 1 = isMatrix (true)
    printf("\n");

    // 케이스 2: 희소그래프 (E=100) - 인접리스트
    printf("케이스 2: 희소그래프 (V=%d, E=%d) - 인접리스트\n", VERTICES, E_SPARSE);
    runTestCase(VERTICES, E_SPARSE, 0); // 0 = isMatrix (false)
    printf("\n");

    // 케이스 3: 밀집 그래프 (E=4000) - 인접행렬
    printf("케이스 3: 밀집 그래프 (V=%d, E=%d) - 인접행렬\n", VERTICES, E_DENSE);
    runTestCase(VERTICES, E_DENSE, 1);
    printf("\n");

    // 케이스 4: 밀집 그래프 (E=4000) - 인접리스트
    printf("케이스 4: 밀집 그래프 (V=%d, E=%d) - 인접리스트\n", VERTICES, E_DENSE);
    runTestCase(VERTICES, E_DENSE, 0);
    printf("\n");

    return 0; // 프로그램 정상 종료
}