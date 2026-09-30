#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main()
{
    char express[100];
    int result = 0;
    int num;
    char op = '+';

    printf("enter an expression:");
    scanf("%s", express);

    char *ptr = express;

    while (*ptr != '\0')
    {
        num = 0;
        while (isdigit(*ptr))
        {
            num = num * 10 + (*ptr - '0');
            ptr++;
        }

        if (op == '+')
        {
            result += num;
        }
        else if (op == '-')
        {
            result -= num;
        }
        else if (op == '*')
        {
            result *= num;
        }
        else if (op == '/')
        {
            if (num == 0)
            {
                printf("Error:division by zero\n");
                return 1;
            }
            result /= num;
        }

        if (*ptr != '\0')
        {
            op = *ptr;
            ptr++;
        }
    }

    printf("Result=%d\n", result);
    return 0;
}