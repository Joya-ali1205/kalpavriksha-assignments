#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main()
{
    char express[100];
    int numbers[50];
    char operators[50];

    int n = 0, opCount = 0;
    int i = 0;
    int num;
    int sign = 1;

    printf("Enter an expression: ");
    scanf("%s", express);

    if (express[0] == '-')
    {
        sign = -1;
        i++;
    }

    if (!isdigit(express[i]))
    {
        printf("Invalid expression\n");
        return 0;
    }

    while (express[i] != '\0')
    {
        num = 0;

        if (!isdigit(express[i]))
        {
            printf("Invalid expression\n");
            return 0;
        }

        while (isdigit(express[i]))
        {
            num = num * 10 + (express[i] - '0');
            i++;
        }

        numbers[n++] = num * sign;

        sign = 1;

        if (express[i] == '\0')
            break;

        if (express[i] != '+' &&
            express[i] != '-' &&
            express[i] != '*' &&
            express[i] != '/')
        {
            printf("Invalid expression\n");
            return 0;
        }

        operators[opCount++] = express[i];
        i++;

        if (express[i] == '\0')
        {
            printf("Invalid expression\n");
            return 0;
        }
    }

    for (i = 0; i < opCount; i++)
    {
        if (operators[i] == '*' || operators[i] == '/')
        {
            if (operators[i] == '/' && numbers[i + 1] == 0)
            {
                printf("Error: Division by zero\n");
                return 0;
            }

            if (operators[i] == '*')
                numbers[i] = numbers[i] * numbers[i + 1];
            else
                numbers[i] = numbers[i] / numbers[i + 1];

            for (int j = i + 1; j < n - 1; j++)
                numbers[j] = numbers[j + 1];

            for (int j = i; j < opCount - 1; j++)
                operators[j] = operators[j + 1];

            n--;
            opCount--;
            i--;
        }
    }

    int result = numbers[0];

    for (i = 0; i < opCount; i++)
    {
        if (operators[i] == '+')
            result += numbers[i + 1];

        else if (operators[i] == '-')
            result -= numbers[i + 1];
    }

    printf("Result = %d\n", result);

    return 0;
}
