#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 이진 탐색 트리 노드 구조체 (연결 자료형)
typedef struct TreeNode
{
	int data;                // 노드가 저장할 정수 데이터
	struct TreeNode* left;   // 왼쪽 자식 노드를 가리키는 포인터
	struct TreeNode* right;  // 오른쪽 자식 노드를 가리키는 포인터
} TreeNode;

// 함수 선언
TreeNode* createNode(int value);                                      // 새 노드 생성
TreeNode* insertNode(TreeNode* root, int value);                      // 트리에 노드 삽입 (BST 규칙에 따라)
int treeSearch(TreeNode* root, int target, int* searchCount);         // 트리에서 탐색 
int linearSearch(int arr[], int size, int target, int* searchCount);  // 배열 선형 탐색
void freeTree(TreeNode* root);                                        // 트리 메모리 해제

// 노드 생성
TreeNode* createNode(int value)
{
	TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));  // 메모리 동적 할당
	newNode->data = value;                                    // 데이터 저장
	newNode->left = newNode->right = NULL;                    // 자식 포인터 초기화
	return newNode;                                           // 새 노드 주소 반환
} 

// 트리 삽입 (이진 탐색 트리 규칙 적용)
TreeNode* insertNode(TreeNode* root, int value)
{
	if (root == NULL) return createNode(value);        // 비어 있으면 새 노드 생성
	if (value < root->data)                            // 현재 노드보다 작으면 왼쪽으로
		root->left = insertNode(root->left, value);    // 왼쪽 하위 트리에 재귀 삽입
	else if (value > root->data)                       // 현재 노드보다 크면 오른쪽으로
		root->right = insertNode(root->right, value);  // 오른쪽 하위 트리에 재귀 삽입
	return root;                                       // 루트 노드 반환 (연결 유지)
}

// 트리 탐색
int treeSearch(TreeNode* root, int target, int* searchCount)
{
	if (root == NULL) return 0;                                // 트리 끝까지 가면 실패(0 반환)
	(*searchCount)++;                                          // 비교 횟수 1 증가
	if (target == root->data) return 1;                        // 찾으면 성공(1 반환)
	else if (target < root->data)                              // 찾는 값이 작으면 왼쪽 탐색
		return treeSearch(root->left, target, searchCount);    // 왼쪽 재귀 호출
	else                                                       // 찾는 값이 크면 오른쪽 탐색
		return treeSearch(root->right, target, searchCount);   // 오른쪽 재귀 호출
}

// 선형 탐색
int linearSearch(int arr[], int size, int target, int* searchCount)
{
	for (int i = 0; i < size; i++)        // 배열의 처음부터 끝까지 순회
	{
		(*searchCount)++;                 // 비교 횟수 증가
		if (arr[i] == target) return 1;   // 값이 일치하면 성공(1 반환)
	}
	return 0;                             // 끝까지 못 찾으면 실패(0 반환)
}

// 트리 메모리 해제
void freeTree(TreeNode* root)
{
	if (root == NULL) return;  // NULL이면 반환
	freeTree(root->left);      // 왼쪽 서브트리 먼저 해제
	freeTree(root->right);     // 오른쪽 서브트리 해제
	free(root);                // 현재 노드 메모리 반환
} 

// 메인 함수
int main(void)
{
	int size = 100;         // 배열 크기 설정
	int arr[100];           // 0~1000 난수를 저장할 배열
	TreeNode* root = NULL;  // 트리 루트 포인터 (처음엔 NULL)
		
	srand((unsigned int)time(NULL));  // 현재 시간을 기준으로 난수 시드 설정

	// 1. 0~1000 사이의 난수를 100개 생성하여 배열과 트리에 저장
	for (int i = 0; i < size; i++)        // 배열 크기(100)만큼 반복
	{
		arr[i] = rand() % 1001;           // 0~1000 범위의 난수 생성
		root = insertNode(root, arr[i]);  // 트리에 같은 값 삽입 (연결형 구조)
	}

	// 2.배열 중 임의의 인덱스 선택 → 해당 값을 탐색 대상으로 설정
	int target = arr[rand() % size];        // 배열 안에서 랜덤하게 하나 선택

	int repetitions = 10000; // 평균 소요 시간 측정용 반복 횟수

	// 3. 선형 탐색 (성공 케이스)
	int linearCount = 0;       
	int foundLinear = 0;
	clock_t start1 = clock();
	for (int i = 0; i < repetitions; i++)
	{
		linearCount = 0;  // 반복마다 탐색 횟수 초기화
		foundLinear = linearSearch(arr, size, target, &linearCount);
	}
	clock_t end1 = clock();
	double linearTime = (double)(end1 - start1) / CLOCKS_PER_SEC / repetitions;

	// 4. 트리 탐색 (성공 케이스)
	int treeCount = 0;
	int foundTree = 0;                                 
	clock_t start2 = clock();
	for (int i = 0; i < repetitions; i++)
	{
		treeCount = 0;  // 반복마다 탐색 횟수 초기화
		foundTree = treeSearch(root, target, &treeCount);
	}
	clock_t end2 = clock();
	double treeTime = (double)(end2 - start2) / CLOCKS_PER_SEC / repetitions;

	// 5. 존재하지 않는 숫자 테스트
	int targetNotExist;

	while (1)
	{
		targetNotExist = rand() % 1001;
		int found = 0;
		for (int i = 0; i < size; i++)
		{
			if (arr[i] == targetNotExist) { found = 1; break; }
		}
		if (!found) break;  // 배열에 없는 숫자 찾으면 종료
	}

	// 6. 선형 탐색 (실패 케이스)
	int notFoundLinearCount = 0;
	int foundLinearFail = 0;
	clock_t start3 = clock();
	for (int i = 0; i < repetitions; i++)
	{
		notFoundLinearCount = 0;  // 반복마다 탐색 횟수 초기화
		foundLinearFail = linearSearch(arr, size, targetNotExist, &notFoundLinearCount);
	}
	clock_t end3 = clock();
	double linearTimeFail = (double)(end3 - start3) / CLOCKS_PER_SEC / repetitions;

	// 7. 트리 탐색 (실패 케이스)
	int notFoundTreeCount = 0;
	int foundTreeFail = 0;
	clock_t start4 = clock();
	for (int i = 0; i < repetitions; i++)
	{
		notFoundTreeCount = 0;  // 반복마다 탐색 횟수 초기화
		foundTreeFail = treeSearch(root, targetNotExist, &notFoundTreeCount);
	}
	clock_t end4 = clock();
	double treeTimeFail = (double)(end4 - start4) / CLOCKS_PER_SEC / repetitions;

	// 8. 탐색 성공 결과 비교 출력
	printf("\n===== 탐색 성공 결과 비교 =====\n");
	printf("\n탐색 대상 숫자: %d\n", target);
	printf("\n선형 탐색 결과: %s\n", foundLinear ? "성공" : "실패");
	printf("탐색 횟수: %d\n", linearCount);
	printf("평균 소요 시간: %.8f초\n", linearTime);

	printf("\n이진 탐색 트리 탐색 결과: %s\n", foundTree ? "성공" : "실패");
	printf("탐색 횟수: %d\n", treeCount);
	printf("평균 소요 시간: %.8f초\n\n", treeTime);

	// 9. 탐색 실패 결과 비교 출력
	printf("\n===== 탐색 실패 결과 비교 =====\n");
	printf("\n탐색 대상 숫자: %d\n", targetNotExist);
	printf("\n선형 탐색 결과: %s\n", foundLinearFail ? "성공" : "실패");
	printf("탐색 횟수: %d\n", notFoundLinearCount);
	printf("평균 소요 시간: %.8f초\n", linearTimeFail);

	printf("\n이진 탐색 트리 탐색 결과: %s\n", foundTreeFail ? "성공" : "실패");
	printf("탐색 횟수: %d\n", notFoundTreeCount);
	printf("평균 소요 시간: %.8f초\n", treeTimeFail);

	// 10. 메모리 해제
	freeTree(root);

	return 0;
}