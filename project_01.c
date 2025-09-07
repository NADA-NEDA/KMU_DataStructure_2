#include <stdio.h>
#include <ctype.h>

const char* p; // �Է� ���ڿ� ������

// ���� ���� �ǳʶٱ�
void skipSpaces()
{
    while (*p && isspace((unsigned char)*p))
    {
        p++;
    }
}

// parse �Լ�: ���� ��ġ���� ����Ʈ���� �Ľ�
// ��ȯ: 1 = ����, 0 = ERROR
// isBinary: �ڽ� 2�� �ʰ� �� 0(FALSE)
int parse(int* isBinary)
{
    skipSpaces();

    if (*p != '(')
    {
        return 0; // '(' ������ ERROR
    }

    p++; // '(' �Һ�
    skipSpaces();

    if (*p == ')')
    {
        // �� ��� ó��
        p++;
        return 1;
    }

    if (!isalpha(*p))
    {
        return 0; // ���ĺ��� �ƴϸ� ERROR
    }

    p++; // ��� ���� �Һ�
    skipSpaces();

    int childCount = 0;

    // �ڽ� ��� �Ľ� (�ִ� 2�������� ���)
    while (*p != ')' && *p != '\0')
    {
        skipSpaces();

        if (*p == '(')
        {
            // '('�� �����ϸ� ��� ȣ��� ����Ʈ�� ó��
            if (!parse(isBinary))
            {
                return 0;
            }
            childCount++; // ����Ʈ�� �ϳ� �Ϸ� �� childCount ����
        }
        else if (isalpha(*p))
        {
            // ���� ���ĺ��� �ϳ��� ����Ʈ���� ó��
            while (isalpha(*p))
            {
                p++; // ������ �ڽı��� ���� ó��
            }
            childCount++; // ���� ���ĺ� �� �ϳ��� ����Ʈ��
        }
        else
        {
            return 0; // �߸��� ���� �� ERROR
        }

        if (childCount > 2)
        {
            *isBinary = 0; // �ڽ��� 2�� �ʰ� �� FALSE
        }

        skipSpaces();
    }

    if (*p != ')')
    {
        return 0; // �ݴ� ��ȣ Ȯ��
    }

    p++; // ')' �Һ�
    return 1; // ���� �Ľ� �Ϸ�
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

    // ���ڿ� ������ �о����� Ȯ��
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
