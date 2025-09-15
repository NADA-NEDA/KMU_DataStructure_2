#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Input: (A (B (C D) E (G H (I J (K (L) M (N)))))) 
// Output: 5, 13, 6 이렇게 출력돼야 하는데 현재는 5,13,7로 출력됨
// 왜 단말 노드의 수가 하나 더 나오는거지...

// 배열 크기 (트리 노드를 저장할 최대 크기)
#define MAX 1000

// 연결 리스트 기반의 트리 노드 구조체
typedef struct Node
{
    char data;                // 노드에 저장될 문자 데이터
    struct Node* firstChild;  // 첫 번째 자식 노드를 가리키는 포인터
    struct Node* nextSibling; // 다음 형제 노드를 가리키는 포인터
} Node;

char arr[MAX];     // 트리를 배열로 표현할 공간 (인덱스 1부터 사용)
int arrIndex = 1;  // arr 배열에 다음 노드가 저장될 위치
const char* p;     // 입력 문자열을 순회하는 데 사용될 전역 포인터

// 공백 문자를 건너뛰는 함수
void skipSpaces()
{
    while (*p && isspace((unsigned char)*p))
    {
        p++;
    }
}

// 새로운 노드를 동적으로 할당하고 초기화하는 함수
Node* createNode(char data)
{
    Node* node = (Node*)malloc(sizeof(Node));  // Node 크기만큼 메모리 할당

    // 메모리 할당 실패 시 오류
    if (!node)
    {
        exit(EXIT_FAILURE);   // 프로그램 강제 종료
    }
    node->data = data;        // 노드에 데이터 저장
    node->firstChild = NULL;  // 첫 자식 포인터 초기화
    node->nextSibling = NULL; // 다음 형제 포인터 초기화
    return node;              // 생성된 노드의 포인터 반환
}

// 괄호 형식의 문자열을 파싱하여 트리를 구성하는 함수
Node* parse()
{
    skipSpaces();  // 파싱 시작 전 공백 제거

    // '('로 시작하지 않으면 오류 
    if (*p != '(')
    {
        return NULL;
    }
    p++;           // '(' 문자 소비
    skipSpaces();  // 공백 문자 건너뛰기

    // 빈 괄호 "()" 인 경우
    if (*p == ')')
    {
        p++;  // ')' 소비
        return NULL;
    }

    // 노드 데이터는 알파벳이 아닐 경우 오류 
    if (!isalpha(*p))
    {
        return NULL;
    }
    Node* root = createNode(*p);  // 현재 노드(루트) 생성
    arr[arrIndex] = *p;           // 배열에 노드 데이터 저장
    //arrIndex++;                   // 배열 인덱스 증가
    p++;                          // 노드 문자 소비
    skipSpaces();                 // 공백 문자 건너뛰기

    Node* prevChild = NULL;       // 이전 자식 노드 포인터 (형제 연결용)

    // ')'가 나오기 전까지 자식 노드들을 순회하며 파싱
    while (*p != ')' && *p != '\0')
    {
        Node* child = NULL;

        // 자식이 서브트리인 경우 재귀적으로 parse() 호출
        if (*p == '(')
        {
            child = parse();  // 서브트리 반환
        }

        // 자식 노드가 단일 문자인 경우
        else if (isalpha(*p))
        {
            // 단일 문자가 아니라면 계속 형제로 연결
            while (isalpha(*p))
            {
                Node* leaf = createNode(*p);  // 새로운 노드 생성
                arr[arrIndex] = *p;           // 배열에 데이터 저장
                //arrIndex++;                 // 배열 인덱스 증가
                p++;
                skipSpaces();                 // 공백 건너뛰기

                if (!root->firstChild)
                {
                    root->firstChild = leaf;
                }
                else
                {
                    prevChild->nextSibling = leaf;
                }
                prevChild = leaf;
            }
            continue; // 이미 연결 완료했으므로 아래 if(child) 블록 건너뜀
        }
        // 예상치 못한 문자 발견 시 오류 
        else
        {
            return NULL;
        }

        // 유효한 자식 노드가 파싱된 경우
        if (child)
        {
            if (!root->firstChild)
            {
                root->firstChild = child;  // 첫 번째 자식으로 연결
            }
            else
            {
                prevChild->nextSibling = child;  // 이후 자식들은 이전 자식의 형제로 연결
            }
            prevChild = child;  // 현재 자식을 다음 반복을 위한 '이전 자식'으로 업데이트
        }
        skipSpaces();  // 공백 문자 건너뛰기
    }

    // 닫는 ')'가 없으면 오류 
    if (*p != ')')
    {
        return NULL;
    }
    p++;          // ')' 문자 소비
    return root;  // 파싱된 트리의 루트 노드 반환
}

// 트리의 모든 노드 메모리를 재귀적으로 해제하는 함수
void freeTree(Node* root)
{
    // 노드가 NULL이면 반환
    if (!root)
    {
        return;
    }
    freeTree(root->firstChild);   // 첫 번째 자식 서브트리 메모리 해제
    freeTree(root->nextSibling);  // 다음 형제 서브트리 메모리 해제
    free(root);                   // 현재 노드 메모리 해제
}

// 트리의 높이를 재귀적으로 계산하는 함수 (단일 노드 높이 = 0)
int getHeight(Node* node)
{
    // 빈 트리의 높이는 -1
    if (!node)
    {
        return 0;
    }
    int maxChildHeight = 0;

    for (Node* c = node->firstChild; c; c = c->nextSibling)
    {
        int h = getHeight(c);

        if (h > maxChildHeight)
        {
            maxChildHeight = h;
        }
        return maxChildHeight + 1;
    }
}

// 트리의 전체 노드 수를 재귀적으로 계산하는 함수
int countNodes(Node* node)
{
    // 노드가 없으면 0 반환
    if (!node)
    {
        return 0;
    }
    int count = 1;  // 현재 노드 1개

    for (Node* c = node->firstChild; c; c = c->nextSibling)
    {
        count += countNodes(c);
    }
    return count;
}

// 트리의 단말 노드(리프 노드) 수를 재귀적으로 계산하는 함수
int countLeaves(Node* node)
{
    // 노드가 없으면 0 반환
    if (!node)
    {
        return 0;
    }

    // 자식이 없으면 단말 노드이므로 1 반환
    if (!node->firstChild)
    {
        return 1;
    }
    int count = 0;

    // 첫 번째 자식 노드만 순회
    for (Node* c = node->firstChild; c; c = c->nextSibling)
    {
        count += countLeaves(c);
    }
    return count;
}

int main(void)
{
    char input[1000];

    // 한 줄 전체를 입력받기
    if (scanf(" %[^\n]", input) != 1)
    {
        return 1;  // 입력 실패 시 종료
    }

    p = input;     // 전역 포인터를 입력 문자열 시작점으로 설정
    arrIndex = 1;  // 배열 인덱스 초기화 (루트는 1번 인덱스)

    Node* root = parse(); // 트리를 파싱

    // 파싱에 실패한 경우
    if (!root)
    {
        printf("0,0,0\n"); // 0,0,0 출력
        return 0;
    }

    // 파싱된 트리를 바탕으로 높이, 노드 수, 단말 노드 수 계산
    int height = getHeight(root);
    int nodes = countNodes(root);
    int leaves = countLeaves(root);

    printf("%d,%d,%d\n", height, nodes, leaves);

    freeTree(root); // 프로그램 종료 전 동적 할당된 메모리 해제

    return 0; // 정상 종료
}