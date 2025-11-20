#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>     
#include <stdlib.h>   
#include <time.h>      
#include <string.h>    

#define MAX_NAME_LEN 20   // 학생 이름 최대 길이(문자 수)
#define MAX_LINE_LEN 200  // 한 줄에 읽을 수 있는 최대 CSV 문자열 길이

// 학생 정보를 저장하기 위한 구조체 정의
typedef struct {
    int id;                       // 학생 ID
    char name[MAX_NAME_LEN];      // 학생 이름 (최대 19자 + '\0')
    char gender[2];               // 성별: 'M' 또는 'F' 한 글자 + 문자열 종료 문자 '\0'
    int korean_grade;             // 국어 점수
    int english_grade;            // 영어 점수
    int math_grade;               // 수학 점수
    long long product;            // 세 과목 점수의 곱을 저장할 필드
} Student;

// 비교 횟수를 저장하는 전역 변수 (순차 탐색, 이진 탐색에서 공용으로 사용)
int comparison_count = 0;

// 1. CSV 파일에서 학생 데이터를 모두 읽어 동적으로 메모리에 저장하는 함수
Student* load_students_from_file(const char* filename, int* out_count) {
    FILE* fp = fopen(filename, "r");  // 파일 읽기 모드로 열기
    if (!fp) {                        // 파일 포인터가 NULL이면(열기 실패)
        perror("파일 열기 실패 (데이터.csv가 같은 경로에 있는지 확인하세요)");
        return NULL;                  // 실패 시 NULL 반환
    }

    char line[MAX_LINE_LEN];          // 한 줄씩 읽어올 버퍼
    int capacity = 10;                // 초기 배열 용량(학생 10명 분)
    int count = 0;                    // 현재까지 읽은 학생 수
    Student* arr = malloc(sizeof(Student) * capacity);  // Student 배열 동적 할당

    if (!arr) {                       // 메모리 할당 실패 시
        perror("메모리 할당 실패");
        fclose(fp);                   // 파일 닫기
        return NULL;                  // NULL 반환
    }

    // 첫 줄 헤더 스킵 (열 이름이 있는 줄을 버림)
    if (fgets(line, sizeof(line), fp) == NULL) {  // 헤더를 읽는데 실패하면(빈 파일 등)
        fprintf(stderr, "경고: 파일이 비어 있습니다.\n");
        free(arr);                    // 이미 할당한 메모리 해제
        fclose(fp);                   // 파일 닫기
        return NULL;                  // NULL 반환
    }

    // 파일 끝(EOF)까지 모든 데이터 줄을 반복해서 읽기
    while (fgets(line, sizeof(line), fp)) {
        // 현재 학생 수가 capacity 이상이면 배열 크기 2배로 증가
        if (count >= capacity) {
            capacity *= 2;                                            // 용량 2배 증가
            Student* temp = realloc(arr, sizeof(Student) * capacity); // 메모리 재할당
            if (!temp) {                                              // 재할당 실패 시
                perror("메모리 재할당 실패");
                // 지금까지 읽은 데이터 개수만 out_count에 기록
                *out_count = count;
                fclose(fp);                                       // 파일 닫기
                return arr;                                       // 기존 arr 반환(부분 데이터)
            }
            arr = temp;                                           // 재할당된 포인터로 교체
        }

        // strtok에 사용할 복사본 (strtok이 원본 문자열을 변경하므로 복사본 사용)
        char line_copy[MAX_LINE_LEN];
        strncpy(line_copy, line, MAX_LINE_LEN);       // line 내용을 line_copy로 복사
        line_copy[MAX_LINE_LEN - 1] = '\0';           // 마지막에 '\0'을 강제로 넣어 안전하게

        Student s;                                    // 한 학생 정보를 임시로 담을 구조체
        char* token;                                  // 구분자 기준으로 자를 문자열 토큰 포인터

        // 1. ID (첫 번째 컬럼)
        token = strtok(line_copy, ",");               // 콤마(,) 기준으로 첫 토큰 분리
        if (token) {                                  // 토큰이 존재하면
            s.id = atoi(token);                       // 문자열을 정수로 변환하여 id에 저장
        }
        else {
            continue;                                 // 데이터 형식이 잘못되면 이 줄은 건너뜀
        }

        // 2. Name (두 번째 컬럼)
        token = strtok(NULL, ",");                    // 다음 토큰(이름) 분리
        if (token) {
            strncpy(s.name, token, MAX_NAME_LEN - 1); // 이름을 s.name에 최대 길이-1 까지 복사
            s.name[MAX_NAME_LEN - 1] = '\0';          // 마지막에 '\0'을 넣어 문자열 종료 보장
        }
        else {
            continue;                                 // 없으면 이 레코드 무시
        }

        // 3. Gender (세 번째 컬럼)
        token = strtok(NULL, ",");                   // 다음 토큰(성별) 분리
        if (token && token[0] != '\0') {             // 토큰이 있고 비어 있지 않다면
            s.gender[0] = token[0];                  // 첫 글자만 사용 (예: 'M' 또는 'F')
            s.gender[1] = '\0';                      // 문자열 종료 문자
        }
        else {
            s.gender[0] = '?';                       // 성별 정보가 없으면 '?'로 표시
            s.gender[1] = '\0';
        }

        // 4. Korean Grade (네 번째 컬럼)
        token = strtok(NULL, ",");                   // 다음 토큰(국어 점수) 분리
        if (token) {
            s.korean_grade = atoi(token);            // 문자열을 정수로 변환
        }
        else {
            continue;                                // 없으면 잘못된 줄이므로 건너뜀
        }

        // 5. English Grade (다섯 번째 컬럼)
        token = strtok(NULL, ",");                   // 다음 토큰(영어 점수) 분리
        if (token) {
            s.english_grade = atoi(token);           // 문자열을 정수로 변환
        }
        else {
            continue;                                // 없으면 이 줄 건너뜀
        }

        // 6. Math Grade (마지막 컬럼, 개행 문자 포함 가능)
        token = strtok(NULL, ",\n\r");               // 콤마, 줄바꿈(\n, \r) 모두 구분자로 처리
        if (token) {
            s.math_grade = atoi(token);              // 문자열을 정수로 변환
        }
        else {
            continue;                                // 마지막 토큰 없으면 줄 건너뜀
        }

        // 완성된 Student 구조체를 배열에 저장하고, 학생 수 증가
        arr[count++] = s;
    }

    fclose(fp);                                      // 파일 닫기
    *out_count = count;                              // 읽은 전체 학생 수를 out_count에 저장

    // 실제 학생 수만큼 메모리 재조정 (남는 용량 제거해서 메모리 절약)
    Student* tight = realloc(arr, sizeof(Student) * count);
    if (tight) {                                     // 재조정 성공하면
        arr = tight;                                 // arr 포인터를 재조정된 메모리로 변경
    }

    return arr;                                      // 학생 배열의 시작 주소 반환
}

// 2. 세 가지 성적의 곱을 계산하여 product 필드에 저장하는 함수
void calculate_product(Student arr[], int size) {
    for (int i = 0; i < size; i++) {                 // 모든 학생에 대해 반복
        // 각 과목 점수를 long long으로 형 변환 후 곱해서 product에 저장
        arr[i].product = (long long)arr[i].korean_grade
            * (long long)arr[i].english_grade
            * (long long)arr[i].math_grade;
    }
}

// 3. 순차 탐색 (Sequential Search) 함수
//    key와 각 학생의 product를 비교하면서, 비교 횟수를 전역 변수 comparison_count에 기록
int sequential_search(long long key, Student arr[], int size) {
    comparison_count = 0;                            // 비교 횟수 초기화
    for (int i = 0; i < size; i++) {                // 배열의 처음부터 끝까지 순차적으로 탐색
        comparison_count++;                          // product와 key를 비교한 횟수 1 증가
        if (arr[i].product == key) {                // 현재 학생의 곱이 key와 같으면
            return i;                               // 해당 인덱스 반환 (탐색 성공)
        }
    }
    return -1;                                      // 끝까지 못 찾으면 -1 반환 (탐색 실패)
}

// 4. 선택 정렬 (Selection Sort) 함수 - product 기준 오름차순 정렬
//    정렬 과정에서의 비교 횟수를 sort_comparisons에 저장
void selection_sort(Student arr[], int size, int* sort_comparisons) {
    *sort_comparisons = 0;                          // 정렬 비교 횟수 초기화
    for (int i = 0; i < size - 1; i++) {            // 배열의 0번째부터 (size-2)번째까지
        int min_idx = i;                            // 현재 구간에서의 최소값 인덱스를 i로 초기화
        for (int j = i + 1; j < size; j++) {        // i 다음 요소부터 끝까지 탐색
            (*sort_comparisons)++;                  // product 비교 1회 수행
            if (arr[j].product < arr[min_idx].product) { // 더 작은 값 발견 시
                min_idx = j;                        // 최소값 인덱스 갱신
            }
        }
        // 찾은 최소값(min_idx)을 현재 위치(i)와 교환
        if (min_idx != i) {                         // 최소값 위치가 i와 다를 때만 교환
            Student temp = arr[i];                  // 임시 변수에 arr[i] 저장
            arr[i] = arr[min_idx];                  // arr[i]에 최소값 넣기
            arr[min_idx] = temp;                    // 원래 arr[i] 값을 최소값 위치로 이동
        }
    }
}

// 5. 이진 탐색 (Binary Search) 함수 - 정렬된 배열에서만 사용 가능
//    key와 arr[mid].product 비교 시마다 comparison_count 증가
int binary_search(long long key, Student arr[], int size) {
    comparison_count = 0;                           // 비교 횟수 초기화
    int low = 0;                                    // 탐색 구간의 시작 인덱스
    int high = size - 1;                            // 탐색 구간의 끝 인덱스

    while (low <= high) {                           // 탐색 구간이 유효한 동안 반복
        int mid = low + (high - low) / 2;           // 중간 인덱스 계산 (오버플로 방지용 형식)
        comparison_count++;                         // key와 arr[mid].product 비교 횟수 1 증가

        if (arr[mid].product == key) {              // 중간 값이 key와 같으면
            return mid;                             // 해당 인덱스 반환 (탐색 성공)
        }
        else if (arr[mid].product < key) {        // 중간 값이 key보다 작으면
            low = mid + 1;                          // 탐색 구간을 오른쪽 절반으로 이동
        }
        else {                                    // 중간 값이 key보다 크면
            high = mid - 1;                         // 탐색 구간을 왼쪽 절반으로 이동
        }
    }
    return -1;                                      // 못 찾으면 -1 반환 (탐색 실패)
}

// 결과 출력 함수 
void print_results(const char* title,
    int index,
    long long key,
    int comparisons,
    int total_comparisons,   // 이진 탐색일 때만 의미 있음
    Student* arr,
    int total_data_count) {
    printf("\n======================================================\n");
    printf("  %s 결과\n", title);                     // 탐색 종류 출력
    printf("======================================================\n");
    printf("총 데이터 개수: %d개\n", total_data_count); // 전체 학생 수 출력
    printf("찾고자 했던 임의의 곱 값: %lld\n", key);   // 찾으려던 product 값 출력

    if (index != -1) {                                // 인덱스가 -1이 아니면 탐색 성공
        printf("탐색 성공! 인덱스: %d, 학생 ID: %d, 실제 곱 값: %lld\n",
            index, arr[index].id, arr[index].product);
    }
    else {                                          // -1이면 탐색 실패
        printf("탐색 실패: 해당 곱 값을 가진 학생을 찾을 수 없습니다.\n");
    }

    printf("탐색 시 비교 횟수: %d회\n", comparisons);  // 탐색 과정의 비교 횟수 출력
    if (total_comparisons > 0) {                      // total_comparisons가 0보다 크면 (이진 탐색일 때)
        printf("정렬과 탐색을 합한 총 비교 횟수: %d회\n", total_comparisons);
    }
    printf("======================================================\n");
}

int main(void) {
    // 0. CSV 파일에서 모든 학생 데이터 로드
    int student_count = 0;                            // 읽은 학생 수를 저장할 변수
    Student* students = load_students_from_file("데이터.csv", &student_count); // 파일에서 학생 데이터 읽기

    if (!students) {                                  // students가 NULL이면 로드 실패
        fprintf(stderr, "학생 데이터를 불러오지 못했습니다. 프로그램을 종료합니다.\n");
        return 1;                                     // 비정상 종료 코드 반환
    }

    const int actual_data_size = student_count;       // 실제 학생 수를 상수처럼 사용
    printf("--- 총 %d명의 학생 데이터를 로드했습니다. ---\n", actual_data_size);

    // 난수 생성기 초기화 (현재 시간을 시드로 사용)
    srand((unsigned int)time(NULL));

    // 1단계: 세 가지 성적의 곱을 계산하여 product 필드에 저장
    calculate_product(students, actual_data_size);    // 모든 학생의 product 계산
    printf("\n1. 세 가지 성적의 곱(product) 계산 완료\n");
 
    /*printf("   ID   |   이름   |  국어  |  영어  |  수학  |    곱 값\n");
    printf("--------------------------------------------------------\n");

    // 데이터가 너무 많을 수 있으니 처음 10개 + 마지막 1개만 예시로 출력
    int print_limit = (actual_data_size > 10) ? 10 : actual_data_size; // 최대 10개까지만 출력
    for (int i = 0; i < print_limit; i++) {         // 처음 print_limit 개 학생만 출력
        printf("%6d | %6s | %4d | %4d | %4d | %8lld\n",
            students[i].id, 
            students[i].name,
            students[i].korean_grade, 
            students[i].english_grade,
            students[i].math_grade, 
            students[i].product);
    } 
    if (actual_data_size > 10) {                    // 전체 데이터가 10개를 넘으면
        printf("  ... (중략) ...\n");                // 중간 생략 표시
        // 마지막 학생 정보도 예시로 출력
        printf("%6d | %6s | %4d | %4d | %4d | %8lld\n",
            students[actual_data_size - 1].id,
            students[actual_data_size - 1].name,
            students[actual_data_size - 1].korean_grade,
            students[actual_data_size - 1].english_grade,
            students[actual_data_size - 1].math_grade,
            students[actual_data_size - 1].product);
    }*/

    // 2단계: 0 ~ 1,000,000 사이의 임의의 값을 선택 (탐색 대상 product 값)
    long long random_key = rand() % 1000001;        // 0 이상 1,000,000 이하의 난수 생성
    printf("\n2. 0 ~ 1,000,000 사이의 임의의 값 선택\n");
    printf("   [선택된 탐색 대상 값 (random_key) = %lld]\n", random_key);

    // 3단계: 순차 탐색 수행 (정렬하지 않은 원본 배열 기준)
    int sequential_index = sequential_search(random_key, students, actual_data_size);
    int sequential_comparisons = comparison_count;  // 순차 탐색 중 비교 횟수 저장
    print_results("순차 탐색", sequential_index, random_key,
        sequential_comparisons, 0,        // total_comparisons는 0 (정렬 없음)
        students, actual_data_size);

    // 4단계: 이진 탐색을 위한 정렬 + 이진 탐색
    // 원본 데이터를 보호하기 위해 별도의 배열에 복사 후 정렬
    Student* students_for_binary_search = malloc(sizeof(Student) * actual_data_size);
    if (!students_for_binary_search) {              // 메모리 할당 실패 시
        perror("이진 탐색용 배열 메모리 할당 실패");
        free(students);                             // 원본 배열 해제
        return 1;                                   // 종료
    }
    memcpy(students_for_binary_search, students, sizeof(Student) * actual_data_size);
    // 원본 배열 내용을 복사

    int sort_comparisons = 0;                       // 정렬 중 비교 횟수를 저장할 변수
    selection_sort(students_for_binary_search, actual_data_size, &sort_comparisons);
    // product 기준으로 선택 정렬 수행

    printf("\n3. 성적 곱(product) 기준 선택 정렬 완료\n");
    printf("   [정렬 시 비교 횟수: %d회]\n", sort_comparisons);
    /*
    printf("정렬된 배열의 앞부분 예시 (최대 10개):\n");
    for (int i = 0; i < print_limit; i++) {         // 정렬된 배열의 앞부분 일부 출력
        printf("%6d | %6s | %8lld\n",
            students_for_binary_search[i].id,
            students_for_binary_search[i].name,
            students_for_binary_search[i].product);
    }
    if (actual_data_size > 10) {                    // 데이터가 많으면 중간 생략 표시
        printf("  ... (중략) ...\n");
    }
    */

    // 정렬된 배열을 사용하여 이진 탐색 수행
    int binary_index = binary_search(random_key, students_for_binary_search, actual_data_size);
    int search_comparisons = comparison_count;      // 이진 탐색에서의 비교 횟수 저장
    int total_comparisons = sort_comparisons + search_comparisons;
    // 정렬 + 이진 탐색 전체 비교 횟수

// 이진 탐색 결과 출력 (정렬된 배열 기준)
    print_results("이진 탐색", binary_index, random_key,
        search_comparisons, total_comparisons,
        students_for_binary_search, actual_data_size);

    // 동적 메모리 해제 (사용을 마친 배열 반환)
    free(students);                                 // 원본 학생 배열 메모리 해제
    free(students_for_binary_search);               // 정렬/이진 탐색용 배열 해제

    return 0;                                       // 정상 종료
}