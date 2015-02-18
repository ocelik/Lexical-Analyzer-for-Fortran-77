#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Charachter classes */
#define LETTER 0
#define DIGIT 1
#define OTHER 2

/* Token codes */
#define KEYWORD 10
#define IDENT 11
#define INT_LIT 12
#define REAL_LIT 13
#define DOUBLE_LIT 14
#define STR_LIT 15
#define ASSIGN_OP 16
#define LEFT_PAREN 17
#define RIGHT_PAREN 18
#define ADD_OP 19
#define SUB_OP 20
#define DIV_OP 21
#define MULT_OP 22
#define ASTERIKS 23
#define COMMA 24
#define COMP_OP 25
#define EXP_OP 26
#define CURR_SYM 27
#define COLON_SYM 28
#define SLASH_SYM 29
#define LOGIC_OP 30

/*Exception Codes*/
#define EX_LONG_ID 100
#define EX_STRING_UNCLOSED 101
#define EX_ID_DIGIT 102
#define EX_UNKNOWN 103

/* Global Declarations */
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int nextToken;
int token;
int lineNo = 1;
int double_flag=0;
int comm_data_flag=0;
long int position;
FILE *x_lex, *x_for, *fopen();

/*Function Declarations */
void addChar();
void getChar();
void getNonBlank();
int lookUp(char ch);
int lex();
int isKeyword();
void exceptionHandler(int exception);
void comment();

typedef char *string;
string keyword_list[24]= {"PROGRAM","INTEGER","EXTERNAL","COMMON","PRINT","READ","CALL","END","SUBROUTINE","DATA","IF","THEN","END IF",
                          "REAL","CONTINUE","STOP","DOUBLE PRECISION","FUNCTION","ELSE","RETURN","DO","END DO","FORMAT","COMPLEX"
                         };
string token_list[21]= {"KEYWORD","IDENT","INT_LIT","REAL_LIT","DOUBLE_LIT","STR_LIT","ASSIGN_OP","LEFT_PAREN","RIGHT_PAREN","ADD_OP",
                        "SUB_OP","DIV_OP","MULT_OP","ASTERIKS","COMMA","COMP_OP","EXP_OP","CURR_SYM","COLON_SYM","SLASH_SYM","LOGIC_OP"
                       };

int main()
{
    char dosya_adi[20];
    printf("Lutfen '.for' dosyasinin ismini uzantisiz olarak giriniz: ");
    gets(dosya_adi);

    strcat(dosya_adi,".for");

    if((x_for=fopen(dosya_adi,"r"))== NULL)
        printf("ERROR - cannot open fort.txt \n");
    else
    {
        x_lex=fopen("x.lex","w");

        getChar();
        if(nextChar=='C' || nextChar=='*' || nextChar=='c')
            comment();
        do
        {
            lex();
        }
        while(nextToken!=EOF);

        fclose(x_for);
        fclose(x_lex);

        x_lex=fopen("lexs.txt","r");
        fclose(x_lex);
    }

    return 0;
}

void comment() //Yorumları tanıyor ve atlıyor.
{
    do
    {
        do
        {
            nextChar=getc(x_for);
        }
        while(nextChar!='\n');
        if(nextChar=='\n')
            lineNo++;
        nextChar=getc(x_for);
    }
    while(nextChar=='C' || nextChar=='*' || nextChar=='c');
}
void addChar() //lexeme'e o karakteri ekliyor.
{
    if(lexLen<=98)
    {
        lexeme[lexLen++] = nextChar;
            lexeme[lexLen]=0;
    }
    else printf("Error - too long");
}

void getChar() //Bir sonraki karakteri alıyor.
{
    if ((nextChar=getc(x_for))!=EOF)
    {
        if(isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else
            charClass = OTHER;
    }
    else charClass = EOF;

    if(nextChar=='\n')
    {
        lineNo++;
        comm_data_flag=0;
    }
}

void getNonBlank() //Boşlukları geçiyor.
{
    while(isspace(nextChar))
        getChar();
}

void exceptionHandler(int exception) //Hataları belirtiyor.
{
    switch(exception)
    {
    case EX_LONG_ID:
        printf("Error > Line %d: Identifier is too long! (%s)\n",lineNo,lexeme);
        break;

    case EX_ID_DIGIT:
        printf("Error > Line %d: Identifiers MUST start with a letter!\n",lineNo);
        break;

    case EX_STRING_UNCLOSED:
        printf("Error > Line %d: Unclosed String Literal!\n",lineNo);
        nextToken = 0;
        break;

    case EX_UNKNOWN:
        printf("Error > Line %d: Unknown charachter! (%s)\n",lineNo,lexeme);
        nextToken = 0;
        break;
    }
}

int isKeyword() //Keyword olup olmadığına bakıyor ve ona göre atama yapıyor.
{
    int i,j,flag=0;
    char tempChar;
    for(i=0; i<24; i++)
    {
        if(strcmp(lexeme,keyword_list[i])==0)
        {
            flag=1;
            if(strcmp(lexeme,"COMMON")==0 || strcmp(lexeme,"DATA")==0)
                comm_data_flag=1;

            if(strcmp(lexeme,"END")==0)
            {
                position=ftell(x_for);
                tempChar=getc(x_for);
                if(tempChar=='I' || tempChar=='D')
                {
                    addChar();
                    fseek(x_for,position,SEEK_SET);
                    for(j=0; j<2; j++)
                    {
                        getChar();
                        addChar();
                    }
                    getChar();
                }
                else fseek(x_for,position,SEEK_SET);
            }
            return KEYWORD;
        }
    }
    if(flag==0 && strlen(lexeme)>9)
    {
        exceptionHandler(EX_LONG_ID);
        return 0;
    }
    if(strcmp(lexeme,"DOUBLE")==0)
    {
        position=ftell(x_for);
        tempChar=getc(x_for);
        if(tempChar=='P')
        {
            addChar();
            fseek(x_for,position,SEEK_SET);
            for(j=0; j<9; j++)
            {
                getChar();
                addChar();
            }
            getChar();
            return KEYWORD;
        }
        else fseek(x_for,position,SEEK_SET);
    }
    return IDENT;
}

int lookUp(char ch) //Sembol,Operatör türünü belirliyor.
{
    switch (ch)
    {
    case '(':
        nextToken=LEFT_PAREN;
        break;

    case ')':
        nextToken=RIGHT_PAREN;
        break;

    case '*':
        position=ftell(x_for);
        getChar();
        if(nextChar==',' || nextChar==')')
        {
            nextToken=ASTERIKS;
            fseek(x_for,position,SEEK_SET);
        }
        else if (isdigit(nextChar) || isalpha(nextChar) || nextChar=='(')
        {
            nextToken=MULT_OP;
            fseek(x_for,position,SEEK_SET);
        }

        else if (nextChar=='*')
        {
            addChar();
            nextToken=EXP_OP;
        }
        break;

    case ',':
        nextToken=COMMA;
        break;

    case '=':
        nextToken=ASSIGN_OP;
        break;

    case '+':
        nextToken=ADD_OP;
        break;

    case '-':
        nextToken=SUB_OP;
        break;

    case '/':
        if(comm_data_flag==1)
            nextToken=SLASH_SYM;
        else
        {
            position=ftell(x_for);
            getChar();
            if(nextChar=='\n')
                nextToken=SLASH_SYM;
            else nextToken=DIV_OP;
            fseek(x_for,position,SEEK_SET);
        }
        break;

    case '.':
        position=ftell(x_for);

        do
        {
            getChar();
            addChar();
        }
        while(nextChar!='.' && strlen(lexeme)<8);

        if(nextChar=='.' && strlen(lexeme)==4)
            nextToken=COMP_OP;
        else if(nextChar=='.' && strlen(lexeme)>=5)
            nextToken=LOGIC_OP;
        else exceptionHandler(EX_UNKNOWN);

        break;

    case '\'':
        position=ftell(x_for);
        do
        {
            getChar();
            addChar();
            if(nextChar=='\'')
                break;
        }
        while(nextChar!='\n');

        if(nextChar=='\'')
        {
            nextToken=STR_LIT;
        }
        else if (nextChar=='\n')
        {
            lineNo--;
            exceptionHandler(EX_STRING_UNCLOSED);
            lineNo++;
        }
        break;

    case '$':
        nextToken=CURR_SYM;
        break;

    case ':':
        nextToken=COLON_SYM;
        break;

    default:
        exceptionHandler(EX_UNKNOWN);
        break;

    }
    return nextToken;
}

int lex () //Tokenları belirliyor,dosyaya ve ekrana yazıyor.
{
    int dot_flag=0;
    int let_flag=0;
    double_flag=0;
    lexLen=0;
    getNonBlank();

    switch(charClass)
    {
    case LETTER:
        addChar();
        getChar();
        while(charClass==LETTER || charClass == DIGIT || nextChar=='_')
        {
            addChar();
            getChar();
        }
        nextToken=isKeyword();
        break;

    case DIGIT:
        addChar();
        getChar();
        while(charClass == DIGIT || nextChar=='.' || charClass==LETTER)
        {
            if(nextChar=='.')
                dot_flag++;
            if(charClass==LETTER)
            {
                let_flag++;
                if(nextChar=='D')
                    double_flag=1;
            }
            addChar();
            getChar();
        }

        if(let_flag>1 || dot_flag>1)
        {
            nextToken=0;
            exceptionHandler(EX_ID_DIGIT);
        }
        else if(dot_flag==0 && let_flag==0)
            nextToken=INT_LIT;
        else if(dot_flag==1 && let_flag==1)
        {
            if(double_flag==0)
                nextToken=REAL_LIT;
            else nextToken=DOUBLE_LIT;
        }
        else if(dot_flag==1 && let_flag==0)
            nextToken=REAL_LIT;
        break;

    case OTHER:
        addChar();
        lookUp(nextChar);
        getChar();
        break;

    case EOF:
        nextToken = EOF;
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = 0;
        break;
    }

    if(nextToken!=0 && nextToken!=-1)
    {
        printf("Next token: %s \b\t\t  Next lexeme: %s\n",token_list[nextToken-10], lexeme); //ekrana token ve lexeme'leri yazdırma..

        //dosyaya token kodlarıyla birlikte token'ları ve lexeme'leri yazdırma..
        fprintf(x_lex,"%d Next Token: ",nextToken);
        fprintf(x_lex,"%-18s \t Next Lexeme: ",token_list[nextToken-10]);
        fprintf(x_lex,"%s\n",lexeme);
    }

    if(nextToken==-1) //EOF-dosya sonunu ekrana yazdırma..
        printf("Next token: NULL \b\t\t  Next lexeme: %s\n", lexeme);

    return nextToken;
}
