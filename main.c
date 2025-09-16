#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

enum TokenType
{
    TOKEN_ADD,
    TOKEN_MUL,
    TOKEN_INT
};

enum OpType
{
    OP_ADD,
    OP_MUL,
    OP_PUSH_INT,
    OP_PRINT
};

typedef struct
{
    enum TokenType type;
    int value;
} Token;

typedef struct
{
    enum OpType type;
    int value;
} Op;

int main()
{
    char sourceCode[] =
    (
        "1 2 + "
        "69 *"
    );
    int sourceCodeLen = strlen(sourceCode);

    const int tokenStackCap = 256;
    Token tokenStack[256] = {0};
    int tokenStackIdx = 0;

    Token curToken;
    //"1 2 +"
    //| |
    int tokenStartIdx = 0; //TODO: account for leading spaces

    bool tokenIsNumeric = true;
    const int textualTokenCap = 32;
    char textualToken[32] = {0};
    int textualTokenIdx = 0;

    for(int c = 0; c < sourceCodeLen; c++)
    {
        char curChar = sourceCode[c];
        bool isLastChar = c == sourceCodeLen - 1;

        if(isLastChar && !isspace(curChar))
        {
            tokenIsNumeric = tokenIsNumeric && isdigit(curChar);
            textualToken[textualTokenIdx] = curChar;
        }

        if(isspace(curChar) || isLastChar)
        {
            printf("token: '%s'\n", textualToken);

            if(tokenIsNumeric)
            {
                curToken.type = TOKEN_INT;
                curToken.value = atoi(textualToken);
                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
                printf("atoi\n");
            }
            else if(strcmp(textualToken, "+") == 0)
            {
                curToken.type = TOKEN_ADD;
                curToken.value = 0;
                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "*") == 0)
            {
                curToken.type = TOKEN_MUL;
                curToken.value = 0;
                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else
            {
                printf("unknown token starting at index: %d\n", tokenStartIdx);
                //return 1;
            }

            tokenStartIdx = c;
            memset(textualToken, 0, textualTokenCap);
            textualTokenIdx = 0;
            tokenIsNumeric = true;
        }
        else
        {
            tokenIsNumeric = tokenIsNumeric && isdigit(curChar);
            textualToken[textualTokenIdx] = curChar;
            textualTokenIdx++;
        }
    }

    printf("-- tokens --\n");

    for(int i = 0; i < tokenStackIdx; i++)
    {
        char *textualType;

        switch(tokenStack[i].type)
        {
            case TOKEN_ADD:
                textualType = "add";
                break;

            case TOKEN_MUL:
                textualType = "mul";
                break;

            case TOKEN_INT:
                textualType = "int";
                break;

            default:
                textualType = "unreachable";
                break;
        }
        printf("%d: (%s, %d)\n", i, textualType, tokenStack[i].value);
    }

    //parsing
    const int programCap = 256;
    Op program[256] = {0};
    int programIdx = 0;
    Op curOp;

    for(int i = 0; i < tokenStackIdx; i++)
    {
        Token curToken = tokenStack[i];

        switch(curToken.type)
        {
            case TOKEN_ADD:
                curOp.type = OP_ADD;
                curOp.value = 0;
                break;

            case TOKEN_MUL:
                curOp.type = OP_MUL;
                curOp.value = 0;
                break;

            case TOKEN_INT:
                curOp.type = OP_PUSH_INT;
                curOp.value = curToken.value;
                break;

            default:
                printf("unknown token '%d' found during parsing", curToken.type);
                break;
        }

        program[programIdx] = curOp;
        programIdx++;
    }

    curOp.type = OP_PRINT;
    curOp.value = 0;
    program[programIdx] = curOp;
    programIdx++;

    printf("-- program operations --\n");

    for(int i = 0; i < programIdx; i++)
    {
        char *textualType;

        switch(program[i].type)
        {
            case OP_ADD:
                textualType = "add";
                break;

            case OP_MUL:
                textualType = "mul";
                break;

            case OP_PUSH_INT:
                textualType = "push int";
                break;

            case OP_PRINT:
                textualType = "print";
                break;

            default:
                textualType = "unreachable";
                break;
        }
        printf("%d: (%s, %d)\n", i, textualType, program[i].value);
    }

    //run program
    const int simStackCap = 256;
    int simStack[256] = {0};
    int simStackIdx = 0;

    printf("-- simulation output --\n");

    for(int i = 0; i < programIdx; i++)
    {
        Op curOp = program[i];

        switch(curOp.type)
        {
            case OP_ADD:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStackIdx--;
                int b = simStack[simStackIdx];
                simStack[simStackIdx] = a + b;
                simStackIdx++;
                break;
            }

            case OP_MUL:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStackIdx--;
                int b = simStack[simStackIdx];
                simStack[simStackIdx] = a * b;
                simStackIdx++;
                break;
            }

            case OP_PUSH_INT:
                if(simStackIdx > simStackCap)
                {
                    printf("simulation stack overflow\n");
                    return 1;
                }
                else
                {
                    simStack[simStackIdx] = curOp.value;
                    simStackIdx++;
                }
                break;

            case OP_PRINT:
                simStackIdx--;
                printf("%d\n", simStack[simStackIdx]);
                break;

            default:
                printf("unreachable op in simulation\n");
                return 1;
        }
    }

    return 0;
}
