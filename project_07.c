#include <stdio.h>
#include <stdlib.h> 
#include <time.h>   
#include <limits.h> 

#define V 10 // 정점(Vertex)의 개수를 10으로 정의
#define E 20 // 간선(Edge)의 개수를 20으로 정의

#define INF 99999 // 경로가 없음을 나타내는 무한대(Infinity) 값.

void printMatrix(int dist[V][V], const char* title) {
    printf("\n--- %s ---\n", title);  // 전달받은 제목 출력
    printf("     ");                  // 초기 정렬용 공백
    for (int i = 0; i < V; i++) {
        printf("%5d", i);             // 5칸 간격으로 정점 번호 출력
    }
    printf("\n----");                 // 구분선 시작
    for (int i = 0; i < V; i++) {
        printf("-----");              // 5칸 간격으로 구분선 출력
    }
    printf("\n");

    // 행(Row) 및 데이터 출력
    for (int i = 0; i < V; i++) {      // 바깥 루프: 행 (i)
        printf("%3d |", i);            // 행 번호 출력 (예: "  0 |")
        for (int j = 0; j < V; j++) {  // 안쪽 루프: 열 (j)
            if (dist[i][j] == INF) {   // 만약 (i, j) 거리가 INF라면
                printf("%5s", "INF");  // "INF" 문자열 출력
            }
            else {                          // 거리가 INF가 아니라면 (유효한 값)
                printf("%5d", dist[i][j]);  // 해당 거리(가중치) 값 출력
            }
        }
        printf("\n");                       // 한 행 출력이 끝나면 줄바꿈
    }
}

void floydWarshall(int graph[V][V]) {
    int dist[V][V];      // 최종 최단 거리를 저장할 2D 배열
    int next_node[V][V]; // 경로 복원을 위한 2D 배열 (i에서 j로 갈 때 '다음' 정점)
    int i, j, k;         // 루프(반복문)에서 사용할 인덱스 변수


    // 1. dist 및 next_node 배열 초기화
    for (i = 0; i < V; i++) {          // 모든 정점 i (출발)   
        for (j = 0; j < V; j++) {      // 모든 정점 j (도착)
            dist[i][j] = graph[i][j];  // dist 배열은 원본 그래프(graph)의 값으로 초기화

            // 경로 복원 배열(next_node) 초기화:
            // i와 j가 다르고, (i,j) 간선이 존재한다면 (INF가 아님)
            if (i != j && graph[i][j] != INF) {
                next_node[i][j] = j;   // i에서 j로 가는 '다음' 노드는 j 자신 (직접 연결)
            }
            else {
                // 자기 자신(i==j)이거나, 간선이 없는 경우(INF)
                next_node[i][j] = -1; // -1은 '다음 노드 없음'을 의미
            }
        }
    }

    // 2. 플로이드-워셜 알고리즘 수행 
    // k: 중간에 거쳐갈 노드
    for (k = 0; k < V; k++) {
        // i: 출발 노드
        for (i = 0; i < V; i++) {
            // j: 도착 노드
            for (j = 0; j < V; j++) {

                // k를 거쳐가는 경로(i -> k -> j)가 기존 경로(i -> j)보다 짧은지 확인
                // INF 값으로 인한 오버플로우 방지
                // dist[i][k] + dist[k][j] 계산 전에 둘 다 INF가 아닌지 먼저 확인
                if (dist[i][k] != INF && dist[k][j] != INF &&
                    dist[i][k] + dist[k][j] < dist[i][j])      // i->k->j 경로가 더 짧다면
                {
                    dist[i][j] = dist[i][k] + dist[k][j];      // 최단 거리를 갱신

                    // 경로 갱신:
                    // i에서 j로 가는 최단 경로는 k를 거쳐감.
                    // 따라서 i에서 j로 가기 위한 '다음' 노드는
                    // i에서 k로 가기 위한 '다음' 노드와 같아짐.
                    next_node[i][j] = next_node[i][k];
                }
            }
        }
    }

    // 3. 최종 최단 경로 및 경로(45쌍) 출력
    printf("\n--- 45개의 모든 노드 쌍 최단 경로 (i < j) ---\n");
    int pair_count = 0;  // 출력된 쌍의 개수를 세는 변수

    // (i, j) 쌍을 중복 없이 (j > i) 출력하기 위한 루프
    for (i = 0; i < V; i++) {           // 출발 노드 i
        for (j = i + 1; j < V; j++) {   // 도착 노드 j (j는 항상 i보다 큼)
            pair_count++;               // 쌍의 개수 1 증가
            printf("\n[%2d번째 쌍] %d -> %d\n", pair_count, i, j);

            if (dist[i][j] == INF) {        // i에서 j까지의 최단 거리가 INF라면
                printf("  >> 경로 없음\n");  // 경로가 없다고 출력
            }
            else {  // 경로가 있다면
                printf("  >> 최단 거리: %d\n", dist[i][j]);  // 계산된 최단 거리 출력
                printf("  >> 경로: ");

                int curr = i;       // 경로 추적을 위한 현재 위치 변수 (출발지 i에서 시작)
                printf("%d", curr); // 시작 노드(i) 출력

                // curr가 최종 목적지 j에 도달할 때까지 반복
                while (curr != j) {
                    // (안전 장치) 만약 '다음 노드'가 -1이면 (경로가 없으면) 중단
                    if (next_node[curr][j] == -1) {
                        printf("... (경로 복원 오류)");
                        break;  // while 루프 탈출
                    }
                    // 현재 위치(curr)를 'curr에서 j로 가는 다음 노드'로 갱신
                    curr = next_node[curr][j];
                    printf(" -> %d", curr);  // 갱신된 '다음 노드' 출력
                }
                printf("\n");  // 경로 출력이 끝나면 줄바꿈
            }
        }
    }
    printf("--------------------------------------------\n");
    printf("총 %d 쌍의 경로를 출력했습니다.\n", pair_count);  // 총 45쌍인지 확인
}

int main() {
    // V x V (10 x 10) 크기의 2D 배열을 생성.
    int graph[V][V];

    // 난수 생성기 초기화 (프로그램 실행 시마다 다른 난수 생성)
    // 현재 시간을 시드(seed) 값으로 사용
    srand(time(NULL));

    // 1. 그래프 행렬 초기화
    for (int i = 0; i < V; i++) {              // 행 (i)
        for (int j = 0; j < V; j++) {          // 열 (j)
            // (i == j)인 경우 (자기 자신) 거리는 0,
            // 그 외 모든 경우는 INF (경로 없음)로 초기화
            graph[i][j] = (i == j) ? 0 : INF;
        }
    }

    // 2. 무작위 간선 20개 생성 (무방향 그래프)
    int edges_added = 0;  // 현재까지 추가된 간선의 수
    printf("--- 생성된 무작위 간선 (u, v, weight) ---\n");

    // edges_added가 E(20)개가 될 때까지 반복
    while (edges_added < E) {
        int u = rand() % V;  // 무작위 출발 정점 (0 ~ V-1)
        int v = rand() % V;  // 무작위 도착 정점 (0 ~ V-1)

        // (u != v) : 자기 자신을 연결하는 간선(self-loop) 방지
        // (graph[u][v] == INF) : 이미 간선이 있는 경우(중복) 방지
        if (u != v && graph[u][v] == INF) {
            int weight = (rand() % 10) + 1;  // 가중치 1 ~ 10 사이의 난수 생성

            graph[u][v] = weight;  // (u, v)에 가중치 할당
            graph[v][u] = weight;  // 무방향 그래프이므로 (v, u)에도 동일한 가중치 할당

            // 생성된 간선 정보 출력
            printf("%2d. (%d, %2d, %2d)\n", edges_added + 1, u, v, weight);
            edges_added++;  // 추가된 간선 수 1 증가
        }
        // (if 조건 불만족 시: self-loop이거나 중복 간선이므로 아무것도 안 하고 while 루프 계속)
    }

    // 3. 생성된 초기 그래프(인접 행렬) 출력
    printMatrix(graph, "초기 무작위 가중치 그래프 (인접 행렬)");

    // 4. 플로이드-워셜 알고리즘 실행 및 결과 출력
    floydWarshall(graph);  // graph 배열을 인자로 전달

    return 0;  // 프로그램 정상 종료
}