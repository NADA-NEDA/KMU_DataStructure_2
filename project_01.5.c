#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

char* p;  // ���ڿ� ��ȸ�� ���� ������

// ===================================================
// buf �� ���� ���ڸ� �����ϰ�, ������ ���� ���ڿ��� ��ȯ
// ===================================================
void trim(char* buf)
{
	char* src = buf;  // �б�� ������
	char* dst = buf;  // ����� ������

	while (*src != '\0')
	{
		if (!isspace((unsigned char)*src))  //������ �ƴϸ�
		{
			// *dst = *src; dst++; �� ������ ������ ������
			*dst++ = *src;  // dst�� ����
		}
		src++;  // ���� ���ڷ� �̵�
	}
	*dst = '\0';  // ���ڿ� �� ǥ��
}

// =====================================
// ��������� Ʈ���� �Ľ��ϸ� �ڽ� �� �˻�
// =====================================
int check_binary_tree()
{
	// �� ����Ʈ�� ���
	if (*p == '\0' || *p == ')')
	{
		return TRUE;  
	}

	p++;  // '(' �Һ�
	int childCount = 0; 

	while (*p != ')')  // ')' ���� ������ �ݺ�
	{
		if (isalpha(*p))  // ���ĺ� ���
		{
			p++;  // ��� �Һ�
			childCount++;

			if (childCount > 2)  // �ڽ� 3�� �̻��̸� ����Ʈ�� �ƴ�
			{
				return FALSE;
			}
		}
		else if (*p == '(')  // ����Ʈ�� ����
		{
			if (!check_binary_tree())  // ��� ȣ��
			{
				return FALSE;
			}

			childCount++;

			if (childCount > 2)  // �ڽ� 3�� �̻��̸� ����Ʈ�� �ƴ�
			{
				return FALSE;
			}
		}
	}
	p++;  // ')' �Һ�

	return TRUE;  // ����Ʈ�� ���� ����
}

// =========
// main �Լ�
// =========
int main()
{
	char buf[1000];
	
	if (!fgets(buf, sizeof(buf), stdin))  // �Է� �ޱ�
	{
		return 0;
	}

	trim(buf);  // ���� ����
	p = buf;    // ���� ������ �ʱ�ȭ

	// ����Ʈ�� �˻�
	if (check_binary_tree())  
	{
		printf("TRUE\n");
	}
	else
	{
		printf("FALSE\n");
	}
	return 0;
}