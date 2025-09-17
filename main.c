#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

typedef int AnyTemp;

enum AnyType
{
    TYPE_NONE,
    TYPE_INT,
    TYPE_STRING
};

//void pointers, scary!
typedef struct
{
    enum AnyType type;
    void *value;
} Any;

enum TokenType
{
    TOKEN_ADD,
    TOKEN_MUL,
    TOKEN_INT,
    TOKEN_CONSTANT_DEF,
    TOKEN_SYMBOL
};

typedef struct
{
    enum TokenType type;
    Any data;
} Token;

enum OpType
{
    OP_ADD,
    OP_MUL,
    OP_PUSH_INT,
    OP_PRINT
};

typedef struct
{
    enum OpType type;
    Any data;
} Op;

/*int main()
{

    Any things[2] = {0};
    Any thing = {0};

    int val = 123;
    thing.type = TYPE_INT;
    thing.value = &val;
    things[0] = thing;

    char *str = "I love C!";
    thing.type = TYPE_STRING;
    thing.value = str;
    things[1] = thing;
    Any_getString()
    Any_getInt()

    printf("%d%s\n", *(int*)things[0].value, (char*)things[1].value);

    return 0;
}*/

int main()
{
        //"constant pi 31415"
        //"pi 69 + 2 *"

    char sourceCode[] =
    (
        "1 2 +"
    );
    int sourceCodeLen = strlen(sourceCode);

    //tokenization
    const int tokenStackCap = 256;
    Token tokenStack[256] = {0};
    int tokenStackIdx = 0;

    Token curToken;

    int tokenStartIdx = 0; //TODO: account for leading spaces

    bool tokenIsNumeric = true;
    const int textualTokenCap = 32;
    char textualToken[32] = {0};
    int textualTokenIdx = 0;

    //shitty linear dict
    int symbolLookupCap = 256;
    char *symbolStringLookup[256] = {0};
    int symbolValueLookup[256] = {0};
    int symbolLookupIdx = 0;

    symbolStringLookup[symbolLookupIdx] = "pi";
    symbolValueLookup[symbolLookupIdx] = 31415;
    symbolLookupIdx++;

    Any thingTemp = {0};

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
                int intTemp = atoi(textualToken);
                thingTemp.type = TYPE_INT;
                thingTemp.value = &intTemp;

                curToken.type = TOKEN_INT;
                curToken.data = thingTemp;
                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
                printf("atoi\n");
            }
            else if(strcmp(textualToken, "+") == 0)
            {
                thingTemp.type = TYPE_NONE;
                thingTemp.value = NULL;

                curToken.type = TOKEN_ADD;
                curToken.data = thingTemp;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "*") == 0)
            {
                thingTemp.type = TYPE_NONE;
                thingTemp.value = NULL;

                curToken.type = TOKEN_MUL;
                curToken.data = thingTemp;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "constant") == 0)
            {
                thingTemp.type = TYPE_NONE;
                thingTemp.value = NULL;

                curToken.type = TOKEN_CONSTANT_DEF;
                curToken.data = thingTemp;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else
            {
                //curToken.type = TOKEN_SYMBOL;
                //curToken.data = 0;
                //tokenStack[tokenStackIdx] = curToken;
                //tokenStackIdx++;
                printf("symbol found instead of int or keyword at: %d\n", tokenStartIdx);

                /*for(int s = 0; s < symbolLookupIdx; s++)
                {
                    if(strcmp(symbolStringLookup[s], textualToken) == 0)
                    {
                        curToken.type = TOKEN_INT;
                        curToken.data = symbolValueLookup[s];
                        tokenStack[tokenStackIdx] = curToken;
                        tokenStackIdx++;
                        break;
                    }
                }*/
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

            case TOKEN_CONSTANT_DEF:
                textualType = "constant";
                break;

            default:
                textualType = "unreachable";
                break;
        }
        if(tokenStack[i].data.type == TYPE_INT)
        {
            printf("%d: (%s, %d)\n", i, textualType, *(int*)tokenStack[i].data.value);
        }
        else
        {
            printf("%d: (%s)\n", i, textualType);
        }
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
                curOp.data = curToken.data;
                break;

            case TOKEN_MUL:
                curOp.type = OP_MUL;
                curOp.data = curToken.data;
                break;

            case TOKEN_INT:
                curOp.type = OP_PUSH_INT;
                curOp.data = curToken.data;
                break;

            default:
                printf("unknown token '%d' found during parsing\n", curToken.type);
                break;
        }

        program[programIdx] = curOp;
        programIdx++;
    }


    thingTemp.type = TYPE_NONE;
    thingTemp.value = NULL;

    curOp.type = OP_PRINT;
    curOp.data = thingTemp;
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
        if(program[i].data.type == TYPE_INT)
        {
            printf("%d: (%s, %d)\n", i, textualType, *(int*)program[i].data.value);
        }
        else
        {
            printf("%d: (%s)\n", i, textualType);
        }
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
                    simStack[simStackIdx] = *(int*)curOp.data.value;
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
