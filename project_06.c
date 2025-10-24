/*
 * ����: �׷��� ǥ�� ���(���� ��� vs ���� ����Ʈ) ���� ��
 *
 * �䱸����:
 * 1. ����(V) 100��
 * 2. ��� �׷��� (���� E=100), ���� �׷��� (���� E=4000)
 * 3. 4���� ���̽� �׽�Ʈ:
 * - ��� + ���� ���
 * - ��� + ���� ����Ʈ
 * - ���� + ���� ���
 * - ���� + ���� ����Ʈ
 * 4. ���� �׸�:
 * - �޸� ��뷮
 * - ���� ����/���� �� �� Ƚ��
 * - �� ���� ���� Ȯ�� �� �� Ƚ��
 * - �� ����� ���� ��� ��� �� �� Ƚ��
 */

#include <stdio.h>   // ǥ�� ����� (printf ��)
#include <stdlib.h>  // ���� �޸� �Ҵ� (malloc, free, calloc)
#include <time.h>    // ���� �õ� ������ ���� time()

 // --- ��� ���� ---
 // �ڵ��� ������ �� ���������� ���� ����� �����մϴ�.
#define VERTICES 100      // ������ ���� (V)
#define E_SPARSE 100      // ��� �׷����� ���� ����
#define E_DENSE 4000      // ���� �׷����� ���� ����
#define SUPPRESS_PRINT 1  // 1�� ���� ��, ���� ��� ��� �׽�Ʈ���� ���� ����� ����
                          // (���� ������ �����ϱ� ����)

// --- ���� ������ ����ü ---
// �� �׽�Ʈ ���̽����� �߻��ϴ� �� Ƚ���� �����ϱ� ���� ����ü�Դϴ�.
typedef struct Metrics {
    long long insert_delete_comps;  // ���� ����/���� �� ���� �� Ƚ��
    long long check_edge_comps;     // �� ���� ���� Ȯ�� �� ���� �� Ƚ��
    long long print_neighbors_comps; // ���� ��� ��� �� ���� �� Ƚ��
} Metrics;


/*
 * ===================================================================
 * ���� 1: ���� ����Ʈ (Adjacency List) ����
 * ===================================================================
 * - O(V+E)�� ���� ���⵵.
 * - ��� �׷����� �����մϴ�.
 * - ���� Ž��(Ȯ��, ����) �� ���� ��� ����� �ش� ������ ����(degree)�� ����մϴ�.
 */

 // ���� ����Ʈ�� ��� ����ü (�ϳ��� '����'�� ��Ÿ��)
typedef struct AdjListNode {
    int dest;                    // ������ ���� (destination)
    struct AdjListNode* next;    // ���� ���� ��带 ����Ű�� ������
} AdjListNode;

// ���� ����Ʈ (Ư�� ������ ����� �̿����� ����Ʈ)
typedef struct AdjList {
    AdjListNode* head;           // ����Ʈ�� ����(���) ������
} AdjList;

// ���� ����Ʈ �׷��� ��ü�� ��Ÿ���� ����ü
typedef struct GraphAdjList {
    int V;                       // �׷����� �� ���� ����
    AdjList* array;              // �� ������ ���� ����Ʈ(��� ������)�� ��� �迭
    long long memory_usage;      // �׷��� ���� �� ���� �߰� �� ���� �� �޸�
} GraphAdjList;

// �� ���� ����Ʈ ���(����)�� �����ϴ� ���� �Լ�
AdjListNode* newAdjListNode(int dest) {
    // ��� ũ�⸸ŭ ���� �޸� �Ҵ�
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL; // ���� ���� ���� ����
    return newNode;
}

// ���� ����Ʈ ����� �׷����� �����ϴ� �Լ�
GraphAdjList* createGraph_AdjList(int V) {
    // 1. �׷��� ����ü ��ü�� �޸� �Ҵ�
    GraphAdjList* graph = (GraphAdjList*)malloc(sizeof(GraphAdjList));
    graph->V = V;

    // 2. V���� ������ŭ '���� ����Ʈ ���' �迭 �Ҵ�
    graph->array = (AdjList*)malloc(V * sizeof(AdjList));

    // 3. �⺻ �޸� ��뷮 ���
    // (�׷��� ����ü ũ�� + V���� ����Ʈ ��� �迭 ũ��)
    graph->memory_usage = sizeof(GraphAdjList) + (V * sizeof(AdjList));

    // 4. ��� ������ ����Ʈ ��带 NULL�� �ʱ�ȭ (���� ���� ����)
    for (int i = 0; i < V; ++i) {
        graph->array[i].head = NULL;
    }
    return graph;
}

// (���� ����Ʈ) ���� ���� �Լ� (�� Ƚ�� ī��Ʈ)
// m: ���� ������ ���� Metrics ����ü ������
void insertEdge_AdjList(GraphAdjList* graph, int src, int dest, Metrics* m) {

    // --- 1. (src -> dest) ���� �߰� ---
    // �ߺ� Ȯ��: ���� ��, �̹� ������ �����ϴ��� Ȯ���մϴ�.
    AdjListNode* checker = graph->array[src].head;
    while (checker) {
        if (m) m->insert_delete_comps++; // [�� 1ȸ] ����Ʈ ���� dest ��
        if (checker->dest == dest) return; // �̹� �����ϹǷ� �Լ� ����
        checker = checker->next;
    }
    // (m�� NULL�� �ƴ϶��) ������ NULL üũ�� �� Ƚ���� ����
    if (m) m->insert_delete_comps++; // [�� 1ȸ] while�� ���� (NULL)

    // �� ��� ���� �� ����Ʈ�� �� �տ� ���� (O(1))
    AdjListNode* newNode = newAdjListNode(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // ��尡 �߰��Ǿ����Ƿ� �޸� ��뷮 ����
    if (m != NULL) { // (���� ���� �ÿ��� m=NULL�̹Ƿ� ���� �� ��)
        graph->memory_usage += sizeof(AdjListNode);
    }

    // --- 2. (dest -> src) ���� �߰� (������ �׷����̹Ƿ�) ---
    // ���� ������ ������ dest -> src ���� ����
    checker = graph->array[dest].head;
    while (checker) {
        if (m) m->insert_delete_comps++; // [�� 1ȸ]
        if (checker->dest == src) return; // (�̹� ������ src->dest�� �־����� ���� �ɸ��� �ȵ�)
        checker = checker->next;
    }
    if (m) m->insert_delete_comps++; // [�� 1ȸ]

    newNode = newAdjListNode(src);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
    if (m != NULL) {
        graph->memory_usage += sizeof(AdjListNode);
    }
}

// (���� ����Ʈ) ���� ���� �Լ� (�� Ƚ�� ī��Ʈ)
void deleteEdge_AdjList(GraphAdjList* graph, int src, int dest, Metrics* m) {

    // --- 1. (src -> dest) ���� ���� ---
    AdjListNode* curr = graph->array[src].head;
    AdjListNode* prev = NULL; // ������ ����� ���� ��带 ����

    // ������ ���(dest) Ž��
    while (curr) {
        m->insert_delete_comps++; // [�� 1ȸ] dest�� ��ġ�ϴ��� ��
        if (curr->dest == dest) break; // ã����
        prev = curr;
        curr = curr->next;
    }
    if (m && curr == NULL) m->insert_delete_comps++; // [�� 1ȸ] (�� ã�� NULL ����)

    if (curr) { // ��带 ã�Ҵٸ�
        if (prev == NULL) { // ������ ��尡 ��� ����� ���
            graph->array[src].head = curr->next;
        }
        else { // ������ ��尡 �߰� ����� ���
            prev->next = curr->next;
        }
        free(curr); // ��� �޸� ����
        graph->memory_usage -= sizeof(AdjListNode); // �޸� ��뷮 ����
    }

    // --- 2. (dest -> src) ���� ���� (������ �׷����̹Ƿ�) ---
    // ���� ������ ���� ����
    curr = graph->array[dest].head;
    prev = NULL;
    while (curr) {
        m->insert_delete_comps++; // [�� 1ȸ]
        if (curr->dest == src) break;
        prev = curr;
        curr = curr->next;
    }
    if (m && curr == NULL) m->insert_delete_comps++; // [�� 1ȸ]

    if (curr) {
        if (prev == NULL) graph->array[dest].head = curr->next;
        else prev->next = curr->next;
        free(curr);
        graph->memory_usage -= sizeof(AdjListNode);
    }
}

// (���� ����Ʈ) �� ���� ���� Ȯ�� �Լ� (�� Ƚ�� ī��Ʈ)
int checkEdge_AdjList(GraphAdjList* graph, int src, int dest, Metrics* m) {
    AdjListNode* curr = graph->array[src].head;

    // src�� ���� ����Ʈ�� ��ȸ�ϸ� dest�� �ִ��� Ž��
    while (curr) {
        m->check_edge_comps++; // [�� 1ȸ] ���� ��尡 dest���� ��
        if (curr->dest == dest) return 1; // ã���� (�����)
        curr = curr->next;
    }
    // ������ NULL üũ
    if (m) m->check_edge_comps++; // [�� 1ȸ] (�� ã�� NULL ����)

    return 0; // �� ã���� (���� �� ��)
}

// (���� ����Ʈ) Ư�� ����� ���� ��� ��� (�� Ƚ�� ī��Ʈ)
void printNeighbors_AdjList(GraphAdjList* graph, int v, Metrics* m) {
    AdjListNode* curr = graph->array[v].head;

    // SUPPRESS_PRINT �÷��װ� 0�� ���� ���� ���
    if (!SUPPRESS_PRINT) printf("���� %d�� ���� ���: ", v);

    // �ش� ������ ���� ����Ʈ�� ������ ��ȸ
    while (curr) {
        m->print_neighbors_comps++; // [�� 1ȸ] ���� ����(curr != NULL) Ȯ��

        if (!SUPPRESS_PRINT) printf("%d ", curr->dest);
        curr = curr->next;
    }

    // ������ NULL üũ
    if (m) m->print_neighbors_comps++; // [�� 1ȸ] (���� ����)

    if (!SUPPRESS_PRINT) printf("\n");
}

// (���� ����Ʈ) �׷��� �޸� ����
void freeGraph_AdjList(GraphAdjList* graph) {
    // ��� ������ ��ȸ
    for (int v = 0; v < graph->V; ++v) {
        AdjListNode* curr = graph->array[v].head;
        // �� ������ ���� ����Ʈ�� �ִ� ��� ���(����)���� ����
        while (curr) {
            AdjListNode* temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
    // ���� ��� ������ �迭 ����
    free(graph->array);
    // �׷��� ����ü ��ü ����
    free(graph);
}


/*
 * ===================================================================
 * ���� 2: ���� ��� (Adjacency Matrix) ����
 * ===================================================================
 * - O(V^2)�� ���� ���⵵.
 * - ���� �׷����� �����մϴ�.
 * - ���� ����/����/Ȯ���� O(1)�� �ſ� �����ϴ�.
 * - ���� ��� ����� O(V)��, �׻� ��� ������ Ȯ���ؾ� �մϴ�.
 */

 // ���� ��� �׷��� ����ü
typedef struct GraphAdjMatrix {
    int V;        // �� ���� ����
    int** matrix; // V x V ũ���� 2���� �迭�� ����Ű�� ������
} GraphAdjMatrix;

// ���� ��� ����� �׷����� �����ϴ� �Լ�
GraphAdjMatrix* createGraph_Matrix(int V) {
    // 1. �׷��� ����ü ��ü �޸� �Ҵ�
    GraphAdjMatrix* graph = (GraphAdjMatrix*)malloc(sizeof(GraphAdjMatrix));
    graph->V = V;

    // 2. 2D �迭 �Ҵ� (1�ܰ�: �� ������ �迭 �Ҵ�)
    // int* (������) V���� ������ ���� �Ҵ�
    graph->matrix = (int**)malloc(V * sizeof(int*));

    // 3. 2D �迭 �Ҵ� (2�ܰ�: �� ���� ���� ������ ���� �Ҵ�)
    for (int i = 0; i < V; i++) {
        // V���� int�� ������ ���� �Ҵ�
        // calloc�� ����Ͽ� ��� ���� 0 (���� ����)���� �ڵ� �ʱ�ȭ
        graph->matrix[i] = (int*)calloc(V, sizeof(int));
    }
    return graph;
}

// (���� ���) ���� ���� �Լ� (�� Ƚ�� ī��Ʈ)
void insertEdge_Matrix(GraphAdjMatrix* graph, int src, int dest, Metrics* m) {
    // O(1) ����
    // ���� ��, 0���� Ȯ�� (�� Ȯ�� ������ '��'�� ����)
    m->insert_delete_comps++; // [�� 1ȸ]
    if (graph->matrix[src][dest] == 0) {
        graph->matrix[src][dest] = 1; // (src -> dest) ����
        graph->matrix[dest][src] = 1; // (dest -> src) ���� (������)
    }
}

// (���� ���) ���� ���� �Լ� (�� Ƚ�� ī��Ʈ)
void deleteEdge_Matrix(GraphAdjMatrix* graph, int src, int dest, Metrics* m) {
    // O(1) ����
    // ���� ��, 1���� Ȯ�� (�� Ȯ�� ������ '��'�� ����)
    m->insert_delete_comps++; // [�� 1ȸ]
    if (graph->matrix[src][dest] == 1) {
        graph->matrix[src][dest] = 0; // (src -> dest) ���� ����
        graph->matrix[dest][src] = 0; // (dest -> src) ���� ���� (������)
    }
}

// (���� ���) �� ���� ���� Ȯ�� �Լ� (�� Ƚ�� ī��Ʈ)
int checkEdge_Matrix(GraphAdjMatrix* graph, int src, int dest, Metrics* m) {
    // O(1) ����
    // matrix[src][dest] ���� 1���� Ȯ���ϴ� �� ��ü�� '��'�� ����
    m->check_edge_comps++; // [�� 1ȸ]
    return graph->matrix[src][dest] == 1;
}

// (���� ���) Ư�� ����� ���� ��� ��� (�� Ƚ�� ī��Ʈ)
void printNeighbors_Matrix(GraphAdjMatrix* graph, int v, Metrics* m) {
    if (!SUPPRESS_PRINT) printf("���� %d�� ���� ���: ", v);

    // �ش� ����(v)�� '��(row)'�� ��� ��ȸ�ؾ� �� (O(V))
    for (int i = 0; i < graph->V; i++) {
        // v�� i�� ����Ǿ� �ִ��� (���� 1����) Ȯ��
        m->print_neighbors_comps++; // [�� 1ȸ] (�� �������� 1���� Ȯ��)
        if (graph->matrix[v][i] == 1) {
            if (!SUPPRESS_PRINT) printf("%d ", i);
        }
    }
    if (!SUPPRESS_PRINT) printf("\n");
}

// (���� ���) �׷��� �޸� ��뷮 ���
long getMemory_Matrix(int V) {
    long mem = 0;
    mem += sizeof(GraphAdjMatrix);      // 1. �׷��� ����ü ��ü ũ��
    mem += V * sizeof(int*);            // 2. �� ������ �迭 ũ�� (V��)
    mem += (long)V * V * sizeof(int);   // 3. V*V ���� ���� ������(int) ũ��
    return mem;
}

// (���� ���) �׷��� �޸� ����
void freeGraph_Matrix(GraphAdjMatrix* graph) {
    // �Ҵ��� �������� ����
    // 1. �� ���� ���� ������ ���� ����
    for (int i = 0; i < graph->V; i++) {
        free(graph->matrix[i]);
    }
    // 2. �� ������ �迭 ����
    free(graph->matrix);
    // 3. �׷��� ����ü ��ü ����
    free(graph);
}


/*
 * ===================================================================
 * ���� 3: ���� �׷��� ���� �� �׽�Ʈ ����
 * ===================================================================
 */

 // (���� ���� ����) ���� ���� ���� Ȯ���ϴ� ���� �Լ�
 // (���� �׷��� ���� �� �ߺ� ������ ���ϱ� ���� ���)
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

// E���� ������ ���� ���� �׷��� ����
// (void* graph: ���� ��� �Ǵ� ���� ����Ʈ �׷��� ������)
// (isMatrix: 1�̸� ���, 0�̸� ����Ʈ)
void generateRandomGraph(int V, int E, void* graph, int isMatrix) {
    int edges_added = 0;

    // ���� �ߺ� �߰��� ���� ���� �ӽ� V x V �迭
    // (�޸𸮸� �� ������ ��Ȯ�� E���� ������ �����ϱ� ����)
    int** added = (int**)malloc(V * sizeof(int*));
    for (int i = 0; i < V; i++) {
        added[i] = (int*)calloc(V, sizeof(int));
    }

    // E���� ������ �߰��� ������ �ݺ�
    while (edges_added < E) {
        int u = rand() % V; // ���� ���� u
        int v = rand() % V; // ���� ���� v

        // (u != v) : �ڱ� �ڽ��� ����Ű�� ���� ����
        // (added[u][v] == 0) : �̹� �߰��� �������� Ȯ��
        if (u != v && added[u][v] == 0) {

            // (isMatrix ���ο� ���� �׷��� Ÿ�� ĳ����)
            if (isMatrix) {
                // (���� ���� ���� �ܼ� ����)
                GraphAdjMatrix* g = (GraphAdjMatrix*)graph;
                g->matrix[u][v] = 1;
                g->matrix[v][u] = 1;
            }
            else {
                // (���� ������ ���� NULL ����)
                insertEdge_AdjList((GraphAdjList*)graph, u, v, NULL);
            }

            // �ߺ� ���� �迭�� ���
            added[u][v] = 1;
            added[v][u] = 1;
            edges_added++; // �߰��� ���� �� ����
        }
    }

    // �ӽ� �ߺ� ���� �迭 ����
    for (int i = 0; i < V; i++) {
        free(added[i]);
    }
    free(added);
}

// 4���� ���̽��� �����ϴ� ���� �׽�Ʈ �Լ�
void runTestCase(int V, int E, int isMatrix) {

    // 1. ���� ������ ���� Metrics ���� �ʱ�ȭ
    Metrics metrics = { 0, 0, 0 }; // ��� ī���͸� 0����
    void* graph;                 // �׷��� ������ (��� Ÿ���̵� ����ų �� �ְ�)
    long memory = 0;             // �޸� ��뷮

    // --- 1. �׷��� ���� �� ���� ���� ä��� ---
    if (isMatrix) {
        graph = createGraph_Matrix(V);
        generateRandomGraph(V, E, graph, 1);
        // (���� �׸� 1) �޸� ��뷮 ���
        memory = getMemory_Matrix(V);
    }
    else {
        graph = createGraph_AdjList(V);
        generateRandomGraph(V, E, graph, 0);
        // (���� �׸� 1) �޸� ��뷮 ��� (���� �Ҵ�� ��� ����)
        // (����: ���� ���� �� ���� insertEdge_AdjList(m=NULL)�� �޸𸮸� ������� �ʾ����Ƿ�,
        //  ��Ȯ�� ����� ���ؼ� generateRandomGraph���� memory_usage�� �����ؾ� ��.
        //  (���� �ڵ�� ���� �߰�/���� �׽�Ʈ ���� �޸𸮸� ���� -> �ʱ� �޸𸮷� ����))

        // (����) generateRandomGraph�� ���� ������ ��Ȯ�� �޸� ���
        GraphAdjList* g_list = (GraphAdjList*)graph;
        g_list->memory_usage = sizeof(GraphAdjList) + (V * sizeof(AdjList)) + (2LL * E * sizeof(AdjListNode));
        memory = g_list->memory_usage;
    }

    // --- 2. ���� ���� ���� (�׸� 1ȸ ���� ����) ---

    // (���� �׸� 2) �߰� ���� ����/���� ��
    int u, v;
    // �׽�Ʈ�� ����, �׷����� '�������� �ʴ�' �� ������ �����ϰ� ã��
    do {
        u = rand() % V;
        v = rand() % V;
    } while (u == v || (isMatrix ? checkEdge_Matrix_no_metric(graph, u, v)
        : checkEdge_AdjList_no_metric(graph, u, v)));

    if (isMatrix) {
        insertEdge_Matrix(graph, u, v, &metrics); // ���� (�� Ƚ�� ����)
        deleteEdge_Matrix(graph, u, v, &metrics); // ���� (�� Ƚ�� ����)
    }
    else {
        insertEdge_AdjList(graph, u, v, &metrics); // ���� (�� Ƚ�� ����)
        deleteEdge_AdjList(graph, u, v, &metrics); // ���� (�� Ƚ�� ����)
    }

    // (���� �׸� 3) �� ������ ���� ���� Ȯ�� ��
    // (�� �׽�Ʈ�� ������ �ֵ� ���� �������)
    u = rand() % V;
    v = rand() % V;
    if (isMatrix) {
        checkEdge_Matrix(graph, u, v, &metrics);
    }
    else {
        checkEdge_AdjList(graph, u, v, &metrics);
    }

    // (���� �׸� 4) Ư�� ��忡 ����� ���� ��� ��� ��
    v = rand() % V; // ���� ���� v ����
    if (isMatrix) {
        printNeighbors_Matrix(graph, v, &metrics);
    }
    else {
        printNeighbors_AdjList(graph, v, &metrics);
    }

    // --- 3. ��� ��� ---
    // (�䱸������ ���� ��� ����)
    printf("�޸� %ld Bytes\n", memory);
    printf("���� ����/���� �� %lld��\n", metrics.insert_delete_comps);
    printf("�� ������ ���� Ȯ�� �� %lld��\n", metrics.check_edge_comps);
    printf("�� ����� ���� ��� ��� �� %lld��\n", metrics.print_neighbors_comps);

    // --- 4. �޸� ���� ---
    if (isMatrix) {
        freeGraph_Matrix(graph);
    }
    else {
        freeGraph_AdjList(graph);
    }
}


// --- ���� �Լ� (���α׷� ������) ---
int main() {
    // ���� �õ� �ʱ�ȭ (�Ź� �ٸ� ���� ���� ��� ����)
    srand((unsigned int)time(NULL));

    printf("���� 100��, ��� �׷���(���� 100��), ���� �׷���(���� 4000��) ���� ��\n");
    printf("-------------------------------------------------------------\n\n");

    // ���̽� 1: ��ұ׷��� (E=100) - �������
    printf("���̽� 1: ��ұ׷��� (V=%d, E=%d) - �������\n", VERTICES, E_SPARSE);
    runTestCase(VERTICES, E_SPARSE, 1); // 1 = isMatrix (true)
    printf("\n");

    // ���̽� 2: ��ұ׷��� (E=100) - ��������Ʈ
    printf("���̽� 2: ��ұ׷��� (V=%d, E=%d) - ��������Ʈ\n", VERTICES, E_SPARSE);
    runTestCase(VERTICES, E_SPARSE, 0); // 0 = isMatrix (false)
    printf("\n");

    // ���̽� 3: ���� �׷��� (E=4000) - �������
    printf("���̽� 3: ���� �׷��� (V=%d, E=%d) - �������\n", VERTICES, E_DENSE);
    runTestCase(VERTICES, E_DENSE, 1);
    printf("\n");

    // ���̽� 4: ���� �׷��� (E=4000) - ��������Ʈ
    printf("���̽� 4: ���� �׷��� (V=%d, E=%d) - ��������Ʈ\n", VERTICES, E_DENSE);
    runTestCase(VERTICES, E_DENSE, 0);
    printf("\n");

    return 0; // ���α׷� ���� ����
}