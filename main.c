#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

typedef int AnyTemp;

enum AnyType
{
    TYPE_VOID,
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
    //raw data
    TOKEN_INT,
    //numeric operations
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_REM,
    //boolean operations
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_EQ,
    TOKEN_NOT,
    TOKEN_AND,
    TOKEN_OR,
    //stack operations
    TOKEN_DUPE,
    TOKEN_SWAP,
    TOKEN_DROP,
    //control
    TOKEN_PRIM_PRINT,
    TOKEN_RETURN,
    TOKEN_GOTO,
    TOKEN_DEBUG_PRINT_STACK,
    //syntax "sugar" (not preserved when tokens are converted to operations)
    TOKEN_SYMBOL,
    TOKEN_END,
    TOKEN_CONSTANT_DEF,
    TOKEN_LABEL_DEF,
    TOKEN_FUNCTION_DEF,
    TOKEN_FUNCTION_ARROW,
    TOKEN_IF_DEF,
    TOKEN_THEN
};

typedef struct
{
    enum TokenType type;
    Any data;
} Token;

enum OpType
{
    //raw data
    OP_PUSH_INT,
    //numeric operations
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_REM,
    //boolean operations
    OP_LT,
    OP_GT,
    OP_EQ,
    OP_NOT,
    OP_AND,
    OP_OR,
    //stack operations
    OP_DUPE,
    OP_SWAP,
    OP_DROP,
    //control
    OP_PRINT,
    OP_RETURN,
    OP_GOTO,
    OP_GOTO_IF_NOT,
    OP_DEBUG_PRINT_STACK
};

typedef struct
{
    enum OpType type;
    Any data;
} Op;

#define SUITE_SIM_TRUE 1
#define SUITE_SIM_FALSE 0

int main(void)
{
    const int sourceCodeCap = 256;
    char sourceCode[256] = {};
    int sourceCodeIdx = 0;

    FILE *fp = fopen("input/guinea.suite", "r");

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
    //hack because some tokens would disappear if they were at the end of the file
    //TODO: find the cause of that!
    sourceCode[sourceCodeIdx] = ' ';
    sourceCodeIdx++;

    fclose(fp);

    //tokenization
    const int tokenStackCap = 256;
    Token tokenStack[256] = {0};


    int tokenStackIdx = 0;

    Token curToken;

    int tokenStartIdx = 0;

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
    bool isStartingComment = false;
    bool skipCode = false;

    for(int c = 0; c < sourceCodeIdx; c++)
    {
        char curChar = sourceCode[c];
        char prevChar = ' ';

        if(c > 0)
            prevChar = sourceCode[c - 1];

        if(skipCode)
        {
            //stop ignoring code when we reach a line break
            if(curChar == '\n')
            {
                skipCode = false;
            }
            continue;
        }

        bool isLastChar = c == sourceCodeIdx - 1;

        if(isLastChar && !isspace(curChar))
        {
            tokenIsNumeric = tokenIsNumeric && (isdigit(curChar) || curChar == '-');
            textualToken[textualTokenIdx] = curChar;
        }
        if(curChar == ';')
        {
            //comments
            skipCode = true;
        }
        else if(isspace(curChar) || isLastChar)
        {
            //support for                     space between tokens
            if(isspace(prevChar))
                continue;

            printf("token: '%s'\n", textualToken);

            //HACK: check subtraction first, because a lone '-' is considered a number right now
            if(strcmp(textualToken, "-") == 0)
            {
                curToken.type = TOKEN_SUB;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(tokenIsNumeric)
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
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "*") == 0)
            {
                curToken.type = TOKEN_MUL;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "/") == 0)
            {
                curToken.type = TOKEN_DIV;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "rem") == 0)
            {
                curToken.type = TOKEN_REM;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "mod") == 0)
            {
                curToken.type = TOKEN_MOD;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "constant") == 0)
            {
                curToken.type = TOKEN_CONSTANT_DEF;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "print") == 0)
            {
                curToken.type = TOKEN_PRIM_PRINT;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "end") == 0)
            {
                curToken.type = TOKEN_END;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "dupe") == 0)
            {
                curToken.type = TOKEN_DUPE;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "swap") == 0)
            {
                curToken.type = TOKEN_SWAP;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "return") == 0)
            {
                curToken.type = TOKEN_RETURN;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "goto") == 0)
            {
                curToken.type = TOKEN_GOTO;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "label") == 0)
            {
                curToken.type = TOKEN_LABEL_DEF;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "function") == 0)
            {
                curToken.type = TOKEN_FUNCTION_DEF;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "=>") == 0)
            {
                curToken.type = TOKEN_FUNCTION_ARROW;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "=") == 0)
            {
                curToken.type = TOKEN_EQ;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, ">") == 0)
            {
                curToken.type = TOKEN_GT;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "<") == 0)
            {
                curToken.type = TOKEN_LT;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "and") == 0)
            {
                curToken.type = TOKEN_AND;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "or") == 0)
            {
                curToken.type = TOKEN_OR;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "not") == 0)
            {
                curToken.type = TOKEN_NOT;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "if") == 0)
            {
                curToken.type = TOKEN_IF_DEF;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "then") == 0)
            {
                curToken.type = TOKEN_THEN;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "drop") == 0)
            {
                curToken.type = TOKEN_DROP;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else if(strcmp(textualToken, "stack???") == 0)
            {
                curToken.type = TOKEN_DEBUG_PRINT_STACK;
                curToken.data.type = TYPE_VOID;
                curToken.data.value = NULL;

                tokenStack[tokenStackIdx] = curToken;
                tokenStackIdx++;
            }
            else
            {
                //add 1 to account for null character
                int textualTokenLen = strlen(textualToken) + 1;
                char *textualSymbol = NULL;

                if(textualTokenLen + symbolDumpsterIdx <= symbolDumpsterCap)
                {
                    textualSymbol = symbolDumpster + symbolDumpsterIdx;
                    textualSymbol = strcpy(textualSymbol, textualToken);
                    symbolDumpsterIdx += textualTokenLen;
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
            tokenIsNumeric = tokenIsNumeric && (isdigit(curChar) || curChar == '-');
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
                textualType = "+";
                break;

            case TOKEN_SUB:
                textualType = "-";
                break;

            case TOKEN_MUL:
                textualType = "*";
                break;

            case TOKEN_DIV:
                textualType = "/";
                break;

            case TOKEN_MOD:
                textualType = "mod";
                break;

            case TOKEN_REM:
                textualType = "rem";
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

            case TOKEN_DUPE:
                textualType = "dupe";
                break;

            case TOKEN_SWAP:
                textualType = "swap";
                break;

            case TOKEN_RETURN:
                textualType = "return";
                break;

            case TOKEN_GOTO:
                textualType = "goto";
                break;

            case TOKEN_LABEL_DEF:
                textualType = "label";
                break;

            case TOKEN_FUNCTION_DEF:
                textualType = "function";
                break;

            case TOKEN_FUNCTION_ARROW:
                textualType = "=>";
                break;

            case TOKEN_EQ:
                textualType = "=";
                break;

            case TOKEN_GT:
                textualType = ">";
                break;

            case TOKEN_LT:
                textualType = "<";
                break;

            case TOKEN_AND:
                textualType = "and";
                break;

            case TOKEN_OR:
                textualType = "or";
                break;

            case TOKEN_NOT:
                textualType = "not";
                break;

            case TOKEN_IF_DEF:
                textualType = "if";
                break;

            case TOKEN_THEN:
                textualType = "then";
                break;

            case TOKEN_DROP:
                textualType = "drop";
                break;

            case TOKEN_DEBUG_PRINT_STACK:
                textualType = "stack???";
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

            case TYPE_VOID:
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
    bool isDefiningLabel = false;
    bool isDefiningFunction = false;
    bool inFunctionBody = false;
    bool isDefiningIf = false;
    //for functions
    //they are fancy labels with gotos before them, and the address AFTER the function needs to be found.
    //and so to avoid complicated BS, just assign them when we know the address after the function

    //TODO: this will not work if an if statement is inside of a function
    int *toBeAssigned = NULL;

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

            case TOKEN_SUB:
                curOp.type = OP_SUB;
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

            case TOKEN_DIV:
                curOp.type = OP_DIV;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_MOD:
                curOp.type = OP_MOD;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_REM:
                curOp.type = OP_REM;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_DUPE:
                curOp.type = OP_DUPE;
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

            case TOKEN_RETURN:
                curOp.type = OP_RETURN;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_GOTO:
                curOp.type = OP_GOTO;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_PRIM_PRINT:
                curOp.type = OP_PRINT;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_EQ:
                curOp.type = OP_EQ;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_GT:
                curOp.type = OP_GT;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_LT:
                curOp.type = OP_LT;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_AND:
                curOp.type = OP_AND;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_OR:
                curOp.type = OP_OR;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_NOT:
                curOp.type = OP_NOT;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_DROP:
                curOp.type = OP_DROP;
                curOp.data = curToken.data;

                program[programIdx] = curOp;
                programIdx++;
                break;

            case TOKEN_DEBUG_PRINT_STACK:
                curOp.type = OP_DEBUG_PRINT_STACK;
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

            case TOKEN_CONSTANT_DEF:
                //this is used in TOKEN_SYMBOL, TOKEN_INT, and TOKEN_END cases
                isDefiningConstant = true;
                break;

            case TOKEN_LABEL_DEF:
                isDefiningLabel = true;
                break;

            case TOKEN_FUNCTION_DEF:
                isDefiningFunction = true;
                break;

            case TOKEN_IF_DEF:
            {
                break;
            }

            case TOKEN_THEN:
            {
                isDefiningIf = true;

                int *intp = intDumpster + intDumpsterIdx;
                toBeAssigned = intp;
                intDumpsterIdx++;

                curOp.type = OP_PUSH_INT;
                curOp.data.type = TYPE_INT;
                curOp.data.value = intp;

                program[programIdx] = curOp;
                programIdx++;

                curOp.type = OP_GOTO_IF_NOT;
                curOp.data.type = TYPE_VOID;
                curOp.data.value = NULL;

                program[programIdx] = curOp;
                programIdx++;
                break;
            }

            case TOKEN_SYMBOL:
            {
                char *textualSymbol = (char*)curToken.data.value;

                if(isDefiningConstant) //constant is being defined
                {
                    symbolStringLookup[symbolLookupIdx] = textualSymbol;
                }
                else if(isDefiningLabel)
                {
                    symbolStringLookup[symbolLookupIdx] = textualSymbol;
                    //program idx is already the address of the first instruction after the label definition
                    symbolValueLookup[symbolLookupIdx] = programIdx;
                    symbolLookupIdx++;
                    isDefiningLabel = false;
                }
                else if(isDefiningFunction && !inFunctionBody)
                {
                    int *intp = intDumpster + intDumpsterIdx;
                    toBeAssigned = intp;
                    intDumpsterIdx++;

                    curOp.type = OP_PUSH_INT;
                    curOp.data.type = TYPE_INT;
                    curOp.data.value = intp;

                    program[programIdx] = curOp;
                    programIdx++;

                    curOp.type = OP_GOTO;
                    curOp.data.type = TYPE_VOID;
                    curOp.data.value = NULL;

                    program[programIdx] = curOp;
                    programIdx++;

                    //TODO: separate tables for constants, labels, and functions??
                    symbolStringLookup[symbolLookupIdx] = textualSymbol;
                    //program idx is already the address of the first instruction after the label definition
                    symbolValueLookup[symbolLookupIdx] = programIdx;
                    symbolLookupIdx++;
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

                            program[programIdx] = curOp;
                            programIdx++;

                            symbolIsDefined = true;
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
                if(isDefiningConstant)
                {
                    symbolLookupIdx++;
                    isDefiningConstant = false;
                }
                else if(isDefiningFunction)
                {
                    //address of the instruction after the function body
                    *toBeAssigned = programIdx;
                    isDefiningFunction = false;
                }
                else if(isDefiningIf)
                {
                    //address of the instruction after the function body
                    *toBeAssigned = programIdx;
                    isDefiningIf = false;
                }
                else
                {
                    printf("unexpected end statement\n");
                }
                break;

            case TOKEN_FUNCTION_ARROW:
                inFunctionBody = false;
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
                textualType = "+";
                break;

            case OP_SUB:
                textualType = "-";
                break;

            case OP_MUL:
                textualType = "*";
                break;

            case OP_DIV:
                textualType = "/";
                break;

            case OP_MOD:
                textualType = "mod";
                break;

            case OP_REM:
                textualType = "rem";
                break;

            case OP_PUSH_INT:
                textualType = "push int";
                break;

            case OP_PRINT:
                textualType = "print";
                break;

            case OP_DUPE:
                textualType = "dupe";
                break;

            case OP_SWAP:
                textualType = "swap";
                break;

            case OP_RETURN:
                textualType = "return";
                break;

            case OP_GOTO:
                textualType = "goto";
                break;

            case OP_EQ:
                textualType = "=";
                break;

            case OP_GT:
                textualType = ">";
                break;

            case OP_LT:
                textualType = "<";
                break;

            case OP_AND:
                textualType = "and";
                break;

            case OP_OR:
                textualType = "or";
                break;

            case OP_NOT:
                textualType = "not";
                break;

            case OP_DROP:
                textualType = "drop";
                break;

            case OP_GOTO_IF_NOT:
                textualType = "goto if not";
                break;

            case OP_DEBUG_PRINT_STACK:
                textualType = "stack???";
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

    for(int curAddress = 0; curAddress < programIdx; curAddress++)
    {
        Op curOp = program[curAddress];

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
            case OP_SUB:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStackIdx--;
                int b = simStack[simStackIdx];
                simStack[simStackIdx] = b - a;
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
            case OP_DIV:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStackIdx--;
                int b = simStack[simStackIdx];
                simStack[simStackIdx] = b / a;
                simStackIdx++;
                break;
            }
            case OP_MOD:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStackIdx--;
                int b = simStack[simStackIdx];
                simStack[simStackIdx] = abs(b % a);
                simStackIdx++;
                break;
            }
            case OP_REM:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStackIdx--;
                int b = simStack[simStackIdx];
                simStack[simStackIdx] = b % a;
                simStackIdx++;
                break;
            }
            case OP_DUPE:
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
            case OP_EQ:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStackIdx--;
                int b = simStack[simStackIdx];
                simStack[simStackIdx] = (a == b) ? SUITE_SIM_TRUE : SUITE_SIM_FALSE;
                simStackIdx++;
                break;
            }
            case OP_GT:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStackIdx--;
                int b = simStack[simStackIdx];
                simStack[simStackIdx] = (b > a) ? SUITE_SIM_TRUE : SUITE_SIM_FALSE;
                simStackIdx++;
                break;
            }
            case OP_LT:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStackIdx--;
                int b = simStack[simStackIdx];
                simStack[simStackIdx] = (b < a) ? SUITE_SIM_TRUE : SUITE_SIM_FALSE;
                simStackIdx++;
                break;
            }
            case OP_AND:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStackIdx--;
                int b = simStack[simStackIdx];
                simStack[simStackIdx] = (a == SUITE_SIM_TRUE && b == SUITE_SIM_TRUE) ? SUITE_SIM_TRUE : SUITE_SIM_FALSE;
                simStackIdx++;
                break;
            }
            case OP_OR:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStackIdx--;
                int b = simStack[simStackIdx];
                simStack[simStackIdx] = (a == SUITE_SIM_TRUE || b == SUITE_SIM_TRUE) ? SUITE_SIM_TRUE : SUITE_SIM_FALSE;
                simStackIdx++;
                break;
            }
            case OP_NOT:
            {
                simStackIdx--;
                int a = simStack[simStackIdx];
                simStack[simStackIdx] = (a == SUITE_SIM_FALSE) ? SUITE_SIM_TRUE : SUITE_SIM_FALSE;
                simStackIdx++;
                break;
            }
            case OP_DROP:
            {
                simStackIdx--;
                break;
            }
            case OP_GOTO:
            {
                simStackIdx--;
                int gotoAddress = simStack[simStackIdx];
                curAddress = gotoAddress - 1;
                printf("goto: %d\n", gotoAddress);
                break;
            }
            case OP_GOTO_IF_NOT:
            {
                //condition address --
                //the condition HAS to come before the address, otherwise dupe will duplicate addresses (CAUSES INFINITE LOOPS!!!)
                simStackIdx--;
                int gotoAddress = simStack[simStackIdx];
                simStackIdx--;
                int condition = simStack[simStackIdx];

                if(condition != SUITE_SIM_TRUE)
                {
                    curAddress = gotoAddress - 1;
                }
                printf("goto if not: %d, %s\n", gotoAddress, (condition == SUITE_SIM_TRUE) ? "continued" : "jumped");
                break;
            }
            case OP_RETURN:
                goto endOfSimulation;

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

            case OP_DEBUG_PRINT_STACK:
            {
                printf("stack during address %d:\n", curAddress);
                printf("  .\n");
                printf("  .\n");
                printf("  .\n");

                for(int i = simStackIdx - 1; i >= 0; i--)
                {
                    printf("%2X|%8d\n", i, simStack[i]);
                }
                printf("  +--------");
                break;
            }

            default:
                printf("unreachable op in simulation\n");
                return 1;
        }
    }
    //for the 'return' keyword
    endOfSimulation:

    //final cleanup
    free(intDumpster);

    return 0;
}
