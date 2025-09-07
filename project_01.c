#include <stdio.h>
#include <ctype.h>

const char* p; // 입력 문자열 포인터

// 공백 문자 건너뛰기
void skipSpaces()
{
    while (*p && isspace((unsigned char)*p))
    {
        p++;
    }
}

// parse 함수: 현재 위치에서 서브트리를 파싱
// 반환: 1 = 정상, 0 = ERROR
// isBinary: 자식 2개 초과 시 0(FALSE)
int parse(int* isBinary)
{
    skipSpaces();

    if (*p != '(')
    {
        return 0; // '(' 없으면 ERROR
    }

    p++; // '(' 소비
    skipSpaces();

    if (*p == ')')
    {
        // 빈 노드 처리
        p++;
        return 1;
    }

    if (!isalpha(*p))
    {
        return 0; // 알파벳이 아니면 ERROR
    }

    p++; // 노드 문자 소비
    skipSpaces();

    int childCount = 0;

    // 자식 노드 파싱 (최대 2개까지만 허용)
    while (*p != ')' && *p != '\0')
    {
        skipSpaces();

        if (*p == '(')
        {
            // '('로 시작하면 재귀 호출로 서브트리 처리
            if (!parse(isBinary))
            {
                return 0;
            }
            childCount++; // 서브트리 하나 완료 → childCount 증가
        }
        else if (isalpha(*p))
        {
            // 연속 알파벳도 하나의 서브트리로 처리
            while (isalpha(*p))
            {
                p++; // 오른쪽 자식까지 연속 처리
            }
            childCount++; // 연속 알파벳 → 하나의 서브트리
        }
        else
        {
            return 0; // 잘못된 문자 → ERROR
        }

        if (childCount > 2)
        {
            *isBinary = 0; // 자식이 2개 초과 → FALSE
        }

        skipSpaces();
    }

    if (*p != ')')
    {
        return 0; // 닫는 괄호 확인
    }

    p++; // ')' 소비
    return 1; // 정상 파싱 완료
}

int main(void)
{
    char input[100];

    if (!fgets(input, sizeof(input), stdin))
    {
        printf("ERROR\n");
        return 0;
    }

    p = input;
    int isBinary = 1;

    if (!parse(&isBinary))
    {
        printf("ERROR\n");
        return 0;
    }

    // 문자열 끝까지 읽었는지 확인
    while (*p)
    {
        if (!isspace((unsigned char)*p))
        {
            printf("ERROR\n");
            return 0;
        }
        p++;
    }

    if (isBinary)
    {
        printf("TRUE\n");
    }
    else
    {
        printf("FALSE\n");
    }

    return 0;
}
