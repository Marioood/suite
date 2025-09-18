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
    TOKEN_SYMBOL,
    TOKEN_PRIM_PRINT,
    TOKEN_END,
    TOKEN_DUP,
    TOKEN_SWAP
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
    OP_PRINT,
    OP_DUP,
    OP_SWAP
};

typedef struct
{
    enum OpType type;
    Any data;
} Op;

int main(void)
{
    const int sourceCodeCap = 256;
    char sourceCode[256] = {};
    int sourceCodeIdx = 0;

    FILE *fp = fopen("guinea.suite", "r");

    if(fp == NULL)
    {
        printf("could not locate source code\n");
        return 1;
    }

    bool isEof = false;

    while(!isEof)
    {
        char curChar = getc(fp);

        if(curChar == EOF)
        {
            isEof = true;
        }
        else
        {
            sourceCode[sourceCodeIdx] = curChar;
            sourceCodeIdx++;
        }
    }
    fclose(fp);

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
    //this memory HAS to be dynamically allocated, because the Any type uses *void
    //it works just like the other stacks
    const int intDumpsterCap = 256;
    int *intDumpster = malloc(sizeof(int) * intDumpsterCap);
    int intDumpsterIdx = 0;

    const int symbolDumpsterCap = 256;
    char *symbolDumpster = malloc(sizeof(char) * symbolDumpsterCap);
    int symbolDumpsterIdx = 0;
    //intDumpster[0] = 2;
    //intDumpster[1] = 69;
    //printf("%d %d\n", *intDumpster, *(intDumpster + 1));

    for(int c = 0; c < sourceCodeIdx; c++)
    {
        char curChar = sourceCode[c];
        bool isLastChar = c == sourceCodeIdx - 1;

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
                int *intp = intDumpster + intDumpsterIdx;
                *intp = atoi(textualToken);
                intDumpsterIdx++;

                curToken.type = TOKEN_INT;
                curToken.data.type = TYPE_INT;
                curToken.data.value = intp;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
                printf("atoi\n");
            }
            else if(strcmp(textualToken, "+") == 0)
            {
                curToken.type = TOKEN_ADD;
                curToken.data.type = TYPE_NONE;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "*") == 0)
            {
                curToken.type = TOKEN_MUL;
                curToken.data.type = TYPE_NONE;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "constant") == 0)
            {
                curToken.type = TOKEN_CONSTANT_DEF;
                curToken.data.type = TYPE_NONE;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "print") == 0)
            {
                curToken.type = TOKEN_PRIM_PRINT;
                curToken.data.type = TYPE_NONE;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "end") == 0)
            {
                curToken.type = TOKEN_END;
                curToken.data.type = TYPE_NONE;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "dup") == 0)
            {
                curToken.type = TOKEN_DUP;
                curToken.data.type = TYPE_NONE;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "swap") == 0)
            {
                curToken.type = TOKEN_SWAP;
                curToken.data.type = TYPE_NONE;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else
            {
                int textualTokenLen = strlen(textualToken);
                char *textualSymbol = NULL;

                if(textualTokenLen + symbolDumpsterIdx <= symbolDumpsterCap)
                {
                    textualSymbol = symbolDumpster + symbolDumpsterIdx;
                    textualSymbol = strcpy(textualSymbol, textualToken);
                    //add 1 to account for null character
                    symbolDumpsterIdx += textualTokenLen + 1;
                }
                else
                {
                    printf("stack overflow in symbol dumpster!\n");
                    return 1;
                }

                curToken.type = TOKEN_SYMBOL;
                curToken.data.type = TYPE_STRING;
                curToken.data.value = textualSymbol;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;

                printf("symbol found instead of int or keyword at: %d\n", tokenStartIdx);
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

            case TOKEN_SYMBOL:
                textualType = "symbol";
                break;

            case TOKEN_PRIM_PRINT:
                textualType = "print";
                break;

            case TOKEN_END:
                textualType = "end";
                break;

            case TOKEN_DUP:
                textualType = "dup";
                break;

            case TOKEN_SWAP:
                textualType = "swap";
                break;

            default:
                textualType = "unreachable";
                break;
        }
        switch(tokenStack[i].data.type)
        {
            case TYPE_INT:
                printf("%d: (%s, %d)\n", i, textualType, *(int*)tokenStack[i].data.value);
                break;

            case TYPE_STRING:
                printf("%d: (%s, %s)\n", i, textualType, (char*)tokenStack[i].data.value);
                break;

            case TYPE_NONE:
                printf("%d: (%s)\n", i, textualType);
                break;

            default:
                printf("%d: (%s, ???unknown type)\n");
                break;
        }
    }

    //parsing
    const int programCap = 256;
    Op program[256] = {0};
    int programIdx = 0;
    Op curOp;

    //shitty linear dict
    int symbolLookupCap = 256;
    char *symbolStringLookup[256] = {0};
    int symbolValueLookup[256] = {0};
    int symbolLookupIdx = 0;

    bool isDefiningConstant = false;

    for(int i = 0; i < tokenStackIdx; i++)
    {
        Token curToken = tokenStack[i];

        switch(curToken.type)
        {
            case TOKEN_ADD:
                curOp.type = OP_ADD;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_MUL:
                curOp.type = OP_MUL;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_DUP:
                curOp.type = OP_DUP;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_SWAP:
                curOp.type = OP_SWAP;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_INT:
            {
                if(isDefiningConstant) //constant is being defined
                {
                    symbolValueLookup[symbolLookupIdx] = *(int*)curToken.data.value;
                }
                else
                {
                    curOp.type = OP_PUSH_INT;
                    curOp.data = curToken.data;

                    program[programIdx] = curOp;
                    programIdx++;
                }
                break;
            }
            case TOKEN_PRIM_PRINT:
                curOp.type = OP_PRINT;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_CONSTANT_DEF:
                //this is used in TOKEN_SYMBOL, TOKEN_INT, and TOKEN_END cases
                isDefiningConstant = true;
                break;

            case TOKEN_SYMBOL:
            {
                char *textualSymbol = (char*)curToken.data.value;

                if(isDefiningConstant) //constant is being defined
                {
                    symbolStringLookup[symbolLookupIdx] = textualSymbol;
                }
                else //constant is used in an equation
                {
                    bool symbolIsDefined = false;

                    for(int s = 0; s < symbolLookupIdx; s++)
                    {
                        if(strcmp(symbolStringLookup[s], textualSymbol) == 0)
                        {
                            int *intp = intDumpster + intDumpsterIdx;
                            *intp = symbolValueLookup[s];
                            intDumpsterIdx++;

                            curOp.type = OP_PUSH_INT;
                            curOp.data.type = TYPE_INT;
                            curOp.data.value = intp;

                            symbolIsDefined = true;

                            program[programIdx] = curOp;
                            programIdx++;
                            break;
                        }
                    }

                    if(!symbolIsDefined)
                    {
                        printf("symbol '%s' has not been defined\n", textualSymbol);
                    }
                }
                break;
            }
            case TOKEN_END:
                symbolLookupIdx++;
                isDefiningConstant = false;
                break;

            default:
                printf("unknown token '%d' found during parsing\n", curToken.type);
                break;
        }
    }

    printf("-- defined symbols --\n");
    for(int i = 0; i < symbolLookupIdx; i++)
    {
        printf("constant %s = %d\n", symbolStringLookup[i], symbolValueLookup[i]);
    }

    //symbols are not used in simulation, so free that memory
    free(symbolDumpster);

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

            case OP_DUP:
                textualType = "dup";
                break;

            case OP_SWAP:
                textualType = "swap";
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
            case OP_DUP:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStack[simStackIdx] = a;
                simStackIdx++;
                simStack[simStackIdx] = a;
                simStackIdx++;
                break;
            }
            case OP_SWAP:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStackIdx--;
                int b = simStack[simStackIdx];
                simStackIdx++;
                simStack[simStackIdx] = a;
                simStackIdx++;
                simStack[simStackIdx] = b;
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
    //final cleanup
    free(intDumpster);

    return 0;
}
