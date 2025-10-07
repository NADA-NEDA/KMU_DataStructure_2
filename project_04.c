#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ���� Ž�� Ʈ�� ��� ����ü (���� �ڷ���)
typedef struct TreeNode
{
	int data;                // ��尡 ������ ���� ������
	struct TreeNode* left;   // ���� �ڽ� ��带 ����Ű�� ������
	struct TreeNode* right;  // ������ �ڽ� ��带 ����Ű�� ������
} TreeNode;

// �Լ� ����
TreeNode* createNode(int value);                                      // �� ��� ����
TreeNode* insertNode(TreeNode* root, int value);                      // Ʈ���� ��� ���� (BST ��Ģ�� ����)
int treeSearch(TreeNode* root, int target, int* searchCount);         // Ʈ������ Ž�� 
int linearSearch(int arr[], int size, int target, int* searchCount);  // �迭 ���� Ž��
void freeTree(TreeNode* root);                                        // Ʈ�� �޸� ����

// ��� ����
TreeNode* createNode(int value)
{
	TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));  // �޸� ���� �Ҵ�
	newNode->data = value;                                    // ������ ����
	newNode->left = newNode->right = NULL;                    // �ڽ� ������ �ʱ�ȭ
	return newNode;                                           // �� ��� �ּ� ��ȯ
} 

// Ʈ�� ���� (���� Ž�� Ʈ�� ��Ģ ����)
TreeNode* insertNode(TreeNode* root, int value)
{
	if (root == NULL) return createNode(value);        // ��� ������ �� ��� ����
	if (value < root->data)                            // ���� ��庸�� ������ ��������
		root->left = insertNode(root->left, value);    // ���� ���� Ʈ���� ��� ����
	else if (value > root->data)                       // ���� ��庸�� ũ�� ����������
		root->right = insertNode(root->right, value);  // ������ ���� Ʈ���� ��� ����
	return root;                                       // ��Ʈ ��� ��ȯ (���� ����)
}

// Ʈ�� Ž��
int treeSearch(TreeNode* root, int target, int* searchCount)
{
	if (root == NULL) return 0;                                // Ʈ�� ������ ���� ����(0 ��ȯ)
	(*searchCount)++;                                          // �� Ƚ�� 1 ����
	if (target == root->data) return 1;                        // ã���� ����(1 ��ȯ)
	else if (target < root->data)                              // ã�� ���� ������ ���� Ž��
		return treeSearch(root->left, target, searchCount);    // ���� ��� ȣ��
	else                                                       // ã�� ���� ũ�� ������ Ž��
		return treeSearch(root->right, target, searchCount);   // ������ ��� ȣ��
}

// ���� Ž��
int linearSearch(int arr[], int size, int target, int* searchCount)
{
	for (int i = 0; i < size; i++)        // �迭�� ó������ ������ ��ȸ
	{
		(*searchCount)++;                 // �� Ƚ�� ����
		if (arr[i] == target) return 1;   // ���� ��ġ�ϸ� ����(1 ��ȯ)
	}
	return 0;                             // ������ �� ã���� ����(0 ��ȯ)
}

// Ʈ�� �޸� ����
void freeTree(TreeNode* root)
{
	if (root == NULL) return;  // NULL�̸� ��ȯ
	freeTree(root->left);      // ���� ����Ʈ�� ���� ����
	freeTree(root->right);     // ������ ����Ʈ�� ����
	free(root);                // ���� ��� �޸� ��ȯ
} 

// ���� �Լ�
int main(void)
{
	int size = 100;         // �迭 ũ�� ����
	int arr[100];           // 0~1000 ������ ������ �迭
	TreeNode* root = NULL;  // Ʈ�� ��Ʈ ������ (ó���� NULL)
		
	srand((unsigned int)time(NULL));  // ���� �ð��� �������� ���� �õ� ����

	// 1. 0~1000 ������ ������ 100�� �����Ͽ� �迭�� Ʈ���� ����
	for (int i = 0; i < size; i++)        // �迭 ũ��(100)��ŭ �ݺ�
	{
		arr[i] = rand() % 1001;           // 0~1000 ������ ���� ����
		root = insertNode(root, arr[i]);  // Ʈ���� ���� �� ���� (������ ����)
	}

	// 2.�迭 �� ������ �ε��� ���� �� �ش� ���� Ž�� ������� ����
	int target = arr[rand() % size];        // �迭 �ȿ��� �����ϰ� �ϳ� ����

	int repetitions = 10000; // ��� �ҿ� �ð� ������ �ݺ� Ƚ��

	// 3. ���� Ž�� (���� ���̽�)
	int linearCount = 0;       
	int foundLinear = 0;
	clock_t start1 = clock();
	for (int i = 0; i < repetitions; i++)
	{
		linearCount = 0;  // �ݺ����� Ž�� Ƚ�� �ʱ�ȭ
		foundLinear = linearSearch(arr, size, target, &linearCount);
	}
	clock_t end1 = clock();
	double linearTime = (double)(end1 - start1) / CLOCKS_PER_SEC / repetitions;

	// 4. Ʈ�� Ž�� (���� ���̽�)
	int treeCount = 0;
	int foundTree = 0;                                 
	clock_t start2 = clock();
	for (int i = 0; i < repetitions; i++)
	{
		treeCount = 0;  // �ݺ����� Ž�� Ƚ�� �ʱ�ȭ
		foundTree = treeSearch(root, target, &treeCount);
	}
	clock_t end2 = clock();
	double treeTime = (double)(end2 - start2) / CLOCKS_PER_SEC / repetitions;

	// 5. �������� �ʴ� ���� �׽�Ʈ
	int targetNotExist;

	while (1)
	{
		targetNotExist = rand() % 1001;
		int found = 0;
		for (int i = 0; i < size; i++)
		{
			if (arr[i] == targetNotExist) { found = 1; break; }
		}
		if (!found) break;  // �迭�� ���� ���� ã���� ����
	}

	// 6. ���� Ž�� (���� ���̽�)
	int notFoundLinearCount = 0;
	int foundLinearFail = 0;
	clock_t start3 = clock();
	for (int i = 0; i < repetitions; i++)
	{
		notFoundLinearCount = 0;  // �ݺ����� Ž�� Ƚ�� �ʱ�ȭ
		foundLinearFail = linearSearch(arr, size, targetNotExist, &notFoundLinearCount);
	}
	clock_t end3 = clock();
	double linearTimeFail = (double)(end3 - start3) / CLOCKS_PER_SEC / repetitions;

	// 7. Ʈ�� Ž�� (���� ���̽�)
	int notFoundTreeCount = 0;
	int foundTreeFail = 0;
	clock_t start4 = clock();
	for (int i = 0; i < repetitions; i++)
	{
		notFoundTreeCount = 0;  // �ݺ����� Ž�� Ƚ�� �ʱ�ȭ
		foundTreeFail = treeSearch(root, targetNotExist, &notFoundTreeCount);
	}
	clock_t end4 = clock();
	double treeTimeFail = (double)(end4 - start4) / CLOCKS_PER_SEC / repetitions;

	// 8. Ž�� ���� ��� �� ���
	printf("\n===== Ž�� ���� ��� �� =====\n");
	printf("\nŽ�� ��� ����: %d\n", target);
	printf("\n���� Ž�� ���: %s\n", foundLinear ? "����" : "����");
	printf("Ž�� Ƚ��: %d\n", linearCount);
	printf("��� �ҿ� �ð�: %.8f��\n", linearTime);

	printf("\n���� Ž�� Ʈ�� Ž�� ���: %s\n", foundTree ? "����" : "����");
	printf("Ž�� Ƚ��: %d\n", treeCount);
	printf("��� �ҿ� �ð�: %.8f��\n\n", treeTime);

	// 9. Ž�� ���� ��� �� ���
	printf("\n===== Ž�� ���� ��� �� =====\n");
	printf("\nŽ�� ��� ����: %d\n", targetNotExist);
	printf("\n���� Ž�� ���: %s\n", foundLinearFail ? "����" : "����");
	printf("Ž�� Ƚ��: %d\n", notFoundLinearCount);
	printf("��� �ҿ� �ð�: %.8f��\n", linearTimeFail);

	printf("\n���� Ž�� Ʈ�� Ž�� ���: %s\n", foundTreeFail ? "����" : "����");
	printf("Ž�� Ƚ��: %d\n", notFoundTreeCount);
	printf("��� �ҿ� �ð�: %.8f��\n", treeTimeFail);

	// 10. �޸� ����
	freeTree(root);

	return 0;
}