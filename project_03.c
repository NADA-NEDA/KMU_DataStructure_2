#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// =============================
// 배열 기반 이진트리 구조체 정의
// =============================
typedef struct 
{
    char* a;       // 1-based 배열: 노드 값을 저장 (인덱스 1부터 루트)
    size_t cap;    // 배열 용량
    size_t max_i;  // 현재 사용 중인 최대 인덱스
} ArrBT;

// ===========================================
// 인덱스 계산 (배열 기반 트리의 자식 위치 계산)
// ===========================================
static inline size_t left(size_t i) { return 2 * i; }       // 왼쪽 자식 노드 인덱스
static inline size_t right(size_t i) { return 2 * i + 1; }  // 오른쪽 자식 노드 인덱스

// =================================
// 배열 용량 확보: 필요 시 동적 재할당
// =================================
static void ensure_cap(ArrBT* t, size_t need) 
{
    if (need < t->cap) return;          // 이미 충분하면 종료
    size_t ncap = t->cap ? t->cap : 2;  // 초기 용량 설정
    while (ncap <= need) ncap <<= 1;    // 필요한 만큼 2배씩 증가
    t->a = realloc(t->a, ncap);         // 메모리 재할당
    memset(t->a + t->cap, 0, ncap - t->cap);  // 새로 할당한 영역 0 초기화
    t->cap = ncap;                      // 용량 갱신
}

// =================
// 트리 생성 및 해제
// =================
ArrBT* bt_create(size_t initial_cap) 
{
    ArrBT* t = malloc(sizeof(ArrBT));
    t->a = calloc(initial_cap, 1);     // 배열 0 초기화
    t->cap = initial_cap;
    t->max_i = 0;
    return t;
}
void bt_free(ArrBT* t) { free(t->a); free(t); }

// =========
// 노드 삽입
// =========
void insert_node(ArrBT* t, size_t i, char c) 
{
    ensure_cap(t, i);  // 배열 용량 확보
    t->a[i] = c;       // 노드 값 삽입
    if (i > t->max_i) t->max_i = i;  // 최대 인덱스 갱신
}

// 전위 순회: 루트 -> 왼쪽 -> 오른쪽
void preorder_iter(const ArrBT* t) 
{
    size_t stack[1024], top = 0;  // 스택 초기화
    stack[top++] = 1;             // 루트 인덱스 1부터 시작
    while (top) 
    {
        size_t i = stack[--top];
        if (i == 0 || i > t->max_i || t->a[i] == 0) continue;  // 유효 범위 체크
        printf("%c ", t->a[i]);
        stack[top++] = right(i);  // 오른쪽 먼저 넣어야 왼쪽이 먼저 처리됨
        stack[top++] = left(i);
    }
    printf("\n");
}

// 중위 순회: 왼쪽 -> 루트 -> 오른쪽
void inorder_iter(const ArrBT* t) 
{
    size_t stack[1024], top = 0;
    size_t i = 1;      // 루트
    while (top || (i <= t->max_i && t->a[i])) 
    {
        while (i <= t->max_i && t->a[i]) 
        { 
            stack[top++] = i;  // 스택에 현재 노드 저장
            i = left(i);       // 왼쪽으로 이동
        }  
        i = stack[--top];
        printf("%c ", t->a[i]);
        i = right(i);  // 오른쪽 서브트리 탐색
    }
    printf("\n");
}

// 후위 순회: 왼쪽 -> 오른쪽 -> 루트
void postorder_iter(const ArrBT* t) 
{
    size_t stack[1024], top = 0, last = 0;
    size_t i = 1;
    while (top || (i <= t->max_i && t->a[i])) 
    {
        if (i && i <= t->max_i && t->a[i]) 
        { 
            stack[top++] = i;  // 스택에 현재 노드 저장
            i = left(i);       // 왼쪽으로 이동
        }
        else {
            size_t peek = stack[top - 1];  // 스택 최상단 확인
            // 오른쪽 자식이 있고 아직 처리 안 했으면 이동
            if (right(peek) && right(peek) <= t->max_i && t->a[right(peek)] && last != right(peek)) i = right(peek);
            else 
            { 
                printf("%c ", t->a[peek]);  // 루트 처리
                last = stack[--top];        // 처리 완료
                i = 0;                      // 다음 노드
            }
        }
    }
    printf("\n");
}

// ================================
// 괄호형 입력 -> 배열 기반 트리 변환
// ================================
typedef struct 
{ 
    size_t idx; int next_child; 
} StackItem;

void parse_tree_input(ArrBT* t, const char* s) 
{
    StackItem stack[1024]; 
    size_t top = 0; 
    size_t current = 0;  // 현재 부모 노드 인덱스

    for (size_t k = 0; s[k]; k++)
    {
        char c = s[k];

        if (c == '(')
        {
            if (current)
            {
                stack[top++] = (StackItem){ current, 0 };  // 새 부모 노드 push, next_child 초기화
            }
        }
        else if (c == ')')
        {
            if (top)
            {
                top--;  // 현재 부모 종료 -> pop
            }
        }
        else if (isupper(c))  // 대문자 노드
        {
            size_t i;

            if (top)
            {
                // 첫 번째 자식은 left, 두 번째 자식은 right
                if (stack[top - 1].next_child == 0)
                    i = left(stack[top - 1].idx);
                else
                    i = right(stack[top - 1].idx);

                stack[top - 1].next_child++; // 자식 처리 완료 후 증가
            }
            else
            {
                i = 1; // 루트 노드
            }

            insert_node(t, i, c); // 트리에 노드 삽입
            current = i;           // 현재 노드 갱신
        }
    }
}

// ====
// 메인
// ====
int main(void) 
{
    ArrBT* t = bt_create(32);  // 초기 용량 32
    char input[512];

    // 입력 받기
    if (!fgets(input, sizeof(input), stdin)) return 1;
    input[strcspn(input, "\n")] = 0;  // 개행 제거

    // 괄호형 문자열 -> 배열 트리
    parse_tree_input(t, input);

    // 순회 출력
    printf("\npre-order: "); preorder_iter(t);
    printf("in-order: "); inorder_iter(t);
    printf("post-order: "); postorder_iter(t);

    bt_free(t);  // 메모리 해제
    return 0;
}
