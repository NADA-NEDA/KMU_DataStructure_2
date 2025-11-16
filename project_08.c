#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include <string.h> 
#include <limits.h>

#define NUM_ELEMENTS 10000 // 정렬할 데이터의 개수
#define MAX_VALUE 1000000  // 생성할 난수의 최대값 (0부터 1,000,000까지)
#define NUM_RUNS 100       // 평균을 내기 위해 반복 실행할 횟수

// 단순 삽입 정렬
long long insertion_sort(int* arr, int n) {
    long long comparisons = 0;         // 비교 횟수를 저장할 변수 (long long으로 오버플로우 방지)
    for (int i = 1; i < n; i++) {      // 배열의 두 번째 요소(인덱스 1)부터 마지막 요소까지 순회
        int key = arr[i];              // 현재 삽입하려는 요소를 'key' 변수에 저장
        int j;                         // 'key'와 비교할 왼쪽(정렬된) 부분의 인덱스를 위한 변수

        // key를 삽입할 위치를 찾습니다. (arr[j-1] > key)
        for (j = i; j > 0; j--) {      // 현재 위치 'i'부터 배열의 시작(인덱스 0) 방향으로 이동
            comparisons++;             // arr[j-1]과 key의 비교 횟수 1 증가
            if (arr[j - 1] > key) {    // 만약 'key'보다 큰 요소를 만나면
                arr[j] = arr[j - 1];   // 해당 요소를 오른쪽으로 한 칸 이동시킴
            }
            else {                     // 'key'보다 작거나 같은 요소를 만나면
                break;                 // 그 요소의 바로 뒤가 'key'의 삽입 위치이므로 내부 루프 종료
            }
        }
        arr[j] = key;                  // 찾은 위치(j)에 'key'를 삽입
    }
    return comparisons;                // 총 비교 횟수 반환
}

//기본적인 쉘 정렬
long long shell_sort_basic(int* arr, int n) {
    long long comparisons = 0; // 비교 횟수를 저장할 변수

    // 간격(gap)을 n/2부터 시작하여, 1이 될 때까지 2로 나눠가며 줄입니다.
    for (int gap = n / 2; gap > 0; gap /= 2) {

        // 각 간격(gap)에 대해 부분 리스트의 삽입 정렬을 수행합니다.
        for (int i = gap; i < n; i++) {          // 'gap' 위치부터 배열 끝까지 순회
            int temp = arr[i];                   // 현재 정렬할 요소(삽입될 값)를 'temp'에 저장
            int j;                               // 'temp'와 비교할 'gap'만큼 떨어진 앞쪽 요소의 인덱스

            // gap만큼 떨어진 요소들과 비교하며 삽입 위치를 찾습니다.
            for (j = i; j >= gap; j -= gap) {    // 현재 위치 'i'에서 'gap'씩 빼면서 앞쪽으로 이동
                comparisons++;                   // arr[j - gap]과 temp의 비교 횟수 1 증가
                if (arr[j - gap] > temp) {       // 만약 'gap'만큼 앞선 요소가 'temp'보다 크면
                    arr[j] = arr[j - gap];       // 해당 요소를 'gap'만큼 뒤(현재 j 위치)로 이동
                }
                else {                           // 'temp'보다 작거나 같은 요소를 만나면
                    break;                       // 그 요소의 뒤가 'temp'의 삽입 위치이므로 내부 루프 종료
                }
            }
            arr[j] = temp;                       // 찾은 위치(j)에 'temp'를 삽입
        }
    }
    return comparisons;                          // 총 비교 횟수 반환
}

// 개선된 쉘 정렬(Ciura 간격 수열)
long long shell_sort_optimal(int* arr, int n) {
    long long comparisons = 0;                       // 비교 횟수를 저장할 변수

    // Ciura 간격 수열 (N=10000에 맞춰 8861까지만 사용). 큰 값부터 역순으로 배열에 저장.
    int gaps[] = { 8861, 3938, 1750, 701, 301, 132, 57, 23, 10, 4, 1 };
    int num_gaps = sizeof(gaps) / sizeof(gaps[0]);   // 간격 수열의 개수 계산

    for (int k = 0; k < num_gaps; k++) {             // 정의된 간격 수열을 순서대로(큰 값부터) 순회
        int gap = gaps[k];                           // 현재 사용할 간격(gap)을 가져옴

        // 간격이 배열 크기(n)보다 크거나 같으면 해당 간격은 의미가 없으므로 건너뜁니다.
        if (gap >= n) continue;

        // 기본 쉘 정렬과 동일하게, 현재 'gap'에 대해 삽입 정렬을 수행합니다.
        for (int i = gap; i < n; i++) {
            int temp = arr[i];                       // 현재 정렬할 요소를 'temp'에 저장
            int j;
            for (j = i; j >= gap; j -= gap) {        // 'gap'씩 앞쪽으로 이동하며 비교
                comparisons++;                       // arr[j - gap]과 temp의 비교 횟수 1 증가
                if (arr[j - gap] > temp) {           // 'gap' 앞의 요소가 더 크면
                    arr[j] = arr[j - gap];           // 뒤로 'gap'만큼 이동
                }
                else {                               // 'temp'보다 작거나 같은 요소를 만나면
                    break;                           // 삽입 위치 찾음
                }
            }
            arr[j] = temp;                           // 찾은 위치에 'temp' 삽입
        }
    }
    return comparisons;                              // 총 비교 횟수 반환
}

int main() {
    // 난수 시드 초기화: 현재 시간을 기준으로 시드를 설정하여 매번 다른 난수 생성
    srand((unsigned int)time(NULL));

    // 원본 데이터 및 정렬용 복사본 배열을 힙(heap)에 동적 할당
    int* original_data = (int*)malloc(NUM_ELEMENTS * sizeof(int));  // 난수 원본 저장용
    int* data_to_sort = (int*)malloc(NUM_ELEMENTS * sizeof(int));   // 정렬 시 사용할 복사본

    // 메모리 할당이 실패했는지 확인
    if (original_data == NULL || data_to_sort == NULL) {
        printf("메모리 할당 오류!\n");   // 오류 메시지 출력
        return 1;                       // 비정상 종료
    }

    // 각 정렬 방식의 총 비교 횟수를 누적할 변수 (long long 타입)
    long long total_insertion_comps = 0;
    long long total_shell_basic_comps = 0;
    long long total_shell_optimal_comps = 0;

    // 각 정렬 방식의 최소/최대 비교 횟수를 저장할 변수
    long long min_insertion_comps = LLONG_MAX;  // 최소값은 가장 큰 값(LLONG_MAX)으로 초기화
    long long max_insertion_comps = 0;          // 최대값은 0으로 초기화
    long long min_shell_basic_comps = LLONG_MAX;
    long long max_shell_basic_comps = 0;
    long long min_shell_optimal_comps = LLONG_MAX;
    long long max_shell_optimal_comps = 0;

    // 각 정렬 방식의 총 실행 시간을 누적할 변수 (double 타입)
    double total_insertion_time = 0.0;
    double total_shell_basic_time = 0.0;
    double total_shell_optimal_time = 0.0;

    // 시간 측정을 위한 변수
    clock_t start, end;    // clock()의 반환값(CPU tick)을 저장할 변수
    double cpu_time_used;  // 측정된 CPU 시간을 초 단위로 저장할 변수

    printf("%d개 데이터, %d회 실행 시작...\n", NUM_ELEMENTS, NUM_RUNS);  // 진행 상황 안내 출력

    for (int i = 0; i < NUM_RUNS; i++) { // NUM_RUNS(100회)만큼 반복 실행

        // 1. 새로운 난수 데이터 생성 (0 ~ 1,000,000)
        for (int j = 0; j < NUM_ELEMENTS; j++) {
            original_data[j] = rand() % (MAX_VALUE + 1);  // 0 ~ MAX_VALUE 사이의 난수 생성
        }

        long long current_comps; // 현재 실행의 비교 횟수를 저장할 임시 변수

        // 2. 단순 삽입 정렬
        memcpy(data_to_sort, original_data, NUM_ELEMENTS * sizeof(int));  // 원본 데이터 복사
        start = clock();                                                  // 시간 측정 시작
        current_comps = insertion_sort(data_to_sort, NUM_ELEMENTS);       // 정렬 수행
        end = clock();                                                    // 시간 측정 종료
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;         // 초 단위로 변환 (CPU tick / 초당 tick 수)

        total_insertion_comps += current_comps;                                       // 총 비교 횟수 누적
        total_insertion_time += cpu_time_used;                                        // 총 시간 누적
        if (current_comps < min_insertion_comps) min_insertion_comps = current_comps; // 최소값 갱신
        if (current_comps > max_insertion_comps) max_insertion_comps = current_comps; // 최대값 갱신

        // 3. 기본 쉘 정렬 (n/2)
        memcpy(data_to_sort, original_data, NUM_ELEMENTS * sizeof(int));  // 원본 데이터 복사
        start = clock();                                                  // 시간 측정 시작
        current_comps = shell_sort_basic(data_to_sort, NUM_ELEMENTS);     // 정렬 수행
        end = clock();                                                    // 시간 측정 종료
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;         // 초 단위로 변환

        total_shell_basic_comps += current_comps;                                          // 총 비교 횟수 누적
        total_shell_basic_time += cpu_time_used;                                           // 총 시간 누적
        if (current_comps < min_shell_basic_comps) min_shell_basic_comps = current_comps;  // 최소값 갱신
        if (current_comps > max_shell_basic_comps) max_shell_basic_comps = current_comps;  // 최대값 갱신

        // 4. 개선 쉘 정렬 (Ciura)
        memcpy(data_to_sort, original_data, NUM_ELEMENTS * sizeof(int));  // 원본 데이터 복사
        start = clock();                                                  // 시간 측정 시작
        current_comps = shell_sort_optimal(data_to_sort, NUM_ELEMENTS);   // 정렬 수행
        end = clock();                                                    // 시간 측정 종료
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;         // 초 단위로 변환

        total_shell_optimal_comps += current_comps;                                            // 총 비교 횟수 누적
        total_shell_optimal_time += cpu_time_used;                                             // 총 시간 누적
        if (current_comps < min_shell_optimal_comps) min_shell_optimal_comps = current_comps;  // 최소값 갱신
        if (current_comps > max_shell_optimal_comps) max_shell_optimal_comps = current_comps;  // 최대값 갱신

        // 10회 실행할 때마다 중간 진행 상황 출력
        if ((i + 1) % 10 == 0) {
            printf("%d회 실행 완료...\n", i + 1);
        }
    }

    printf("\n--- %d회 실행 후 성능 분석 (데이터: %d개) ---\n", NUM_RUNS, NUM_ELEMENTS);  // 최종 결과 헤더 출력

    // 최종 결과 출력 (시간은 ms 단위로 변환하여 출력)
    printf("\n[1. 단순 삽입 정렬]\n");
    printf(" - 평균 실행 시간  : %.2fms\n", (total_insertion_time / NUM_RUNS) * 1000.0);  // 초(s)를 밀리초(ms)로 변환
    printf(" - 평균 비교 횟수  : %.2f회\n", (double)total_insertion_comps / NUM_RUNS);    // 평균 계산
    printf(" - 최소 비교 횟수  : %lld회\n", min_insertion_comps);                         // 100회 중 최소 비교 횟수
    printf(" - 최대 비교 횟수  : %lld회\n", max_insertion_comps);                         // 100회 중 최대 비교 횟수

    printf("\n[2. 기본 쉘 정렬 (n/2)]\n");
    printf(" - 평균 실행 시간  : %.2fms\n", (total_shell_basic_time / NUM_RUNS) * 1000.0);
    printf(" - 평균 비교 횟수  : %.2f회\n", (double)total_shell_basic_comps / NUM_RUNS);
    printf(" - 최소 비교 횟수  : %lld회\n", min_shell_basic_comps);
    printf(" - 최대 비교 횟수  : %lld회\n", max_shell_basic_comps);

    printf("\n[3. 개선 쉘 정렬 (Ciura)]\n");
    printf(" - 평균 실행 시간  : %.2fms\n", (total_shell_optimal_time / NUM_RUNS) * 1000.0);
    printf(" - 평균 비교 횟수  : %.2f회\n", (double)total_shell_optimal_comps / NUM_RUNS);
    printf(" - 최소 비교 횟수  : %lld회\n", min_shell_optimal_comps);
    printf(" - 최대 비교 횟수  : %lld회\n", max_shell_optimal_comps);

    // --- 사용자가 요청한 최종 요약 부분 ---
    printf("\n--- [최종 요약: 평균 비교 횟수] ---\n");
    printf("%s : %.2f회\n", "1. 단순 삽입 정렬", (double)total_insertion_comps / NUM_RUNS);
    printf("%s : %.2f회\n", "2. 기본 쉘 정렬 (n/2)", (double)total_shell_basic_comps / NUM_RUNS);
    printf("%s : %.2f회\n", "3. 개선 쉘 정렬 (Ciura)", (double)total_shell_optimal_comps / NUM_RUNS);

    // 동적 할당했던 메모리 해제
    free(original_data);  // 원본 데이터 배열 메모리 해제
    free(data_to_sort);   // 정렬용 복사본 배열 메모리 해제

    return 0;  // 정상 종료
}