#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

char* p;  // 문자열 순회용 전역 포인터

// ===================================================
// buf 내 공백 문자를 제거하고, 공백이 없는 문자열로 변환
// ===================================================
void trim(char* buf)
{
	char* src = buf;  // 읽기용 포인터
	char* dst = buf;  // 쓰기용 포인터

	while (*src != '\0')
	{
		if (!isspace((unsigned char)*src))  //공백이 아니면
		{
			// *dst = *src; dst++; 와 동일한 동작을 수행함
			*dst++ = *src;  // dst로 복사
		}
		src++;  // 다음 문자로 이동
	}
	*dst = '\0';  // 문자열 끝 표시
}

// =====================================
// 재귀적으로 트리를 파싱하며 자식 수 검사
// =====================================
int check_binary_tree()
{
	// 빈 서브트리 허용
	if (*p == '\0' || *p == ')')
	{
		return TRUE;  
	}

	p++;  // '(' 소비
	int childCount = 0; 

	while (*p != ')')  // ')' 만날 때까지 반복
	{
		if (isalpha(*p))  // 알파벳 노드
		{
			p++;  // 노드 소비
			childCount++;

			if (childCount > 2)  // 자식 3개 이상이면 이진트리 아님
			{
				return FALSE;
			}
		}
		else if (*p == '(')  // 서브트리 시작
		{
			if (!check_binary_tree())  // 재귀 호출
			{
				return FALSE;
			}

			childCount++;

			if (childCount > 2)  // 자식 3개 이상이면 이진트리 아님
			{
				return FALSE;
			}
		}
	}
	p++;  // ')' 소비

	return TRUE;  // 이진트리 조건 만족
}

// =========
// main 함수
// =========
int main()
{
	char buf[1000];
	
	if (!fgets(buf, sizeof(buf), stdin))  // 입력 받기
	{
		return 0;
	}

	trim(buf);  // 공백 제거
	p = buf;    // 전역 포인터 초기화

	// 이진트리 검사
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