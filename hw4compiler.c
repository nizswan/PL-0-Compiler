//Cristian McGee (Only Member) COP3402 PL/0 Compiler Assignment 4
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//general libraries, stdio.h for necessaties, stdlib for dynamic memory allocation, string library for comparing strings getting string lengths and copying strings

FILE *ofp;
int stopCompiling = 0;
typedef enum {
oddsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
multsym = 6, slashsym = 7, fisym = 8, eqsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, procsym = 30, writesym = 31,
readsym = 32} token_type;
//given enumerator which identifies a bunch of the symbols to their respected token

typedef struct instructions{
    char op[5]; //name of operation
    int numOp; //number of operation
    int l; //number of level
    int m; //m value
} instructions;

typedef struct tokenPointer{
    char identity[12]; //identity
    token_type token; //token
    struct tokenPointer* nextToken; //used for the lexemes
} tokenPointer;
//structure used for dynamic memory to help make a linked list

typedef struct symbol {
    int kind; // const = 1, var = 2, proc = 3
    char name[12]; // name up to 11 chars and null operator
    int val; // number
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted
} symbol;

instructions ia[500]; //instruction array
int ic = 0; //instruction count
int currentLevel = 0;
int currentAddress = 3;

symbol st[500]; //symbol array or symbol table
int sc = 0; //symbol count

tokenPointer *ta; //token array
int tc = 0; //token count
int ct = 0; //current token

void Error(int num);

tokenPointer getToken(){
    if(ct >= tc){
        Error(28);
    }
    return ta[++ct];
}

void dPrint(char *str){
    //double prints both to file and terminal
    printf("%s",str);
    fprintf(ofp, "%s", str);
}

void printELF(){
    //prints all of the instructions in numeric values
    if(!stopCompiling){
        FILE *elf = fopen("elf.txt", "w");
        for(int i = 0; i < ic; i++){
            fprintf(elf, "%d %d %d\n", ia[i].numOp, ia[i].l, ia[i].m);
        }
        fclose(elf);
    }
}

void finalPrint(char *str){
    //prints the input, no errors and the necessary lines and values
    if(!stopCompiling){
        dPrint(str);
        dPrint("\n\nNo errors, program is syntactically correct\n\nLine\tOP\tL\tM\n");
        for(int i = 0; i < ic; i++){
            printf("%d\t%s\t%d\t%d\n", i, ia[i].op, ia[i].l, ia[i].m);
            fprintf(ofp, "%d\t%s\t%d\t%d\n", i, ia[i].op, ia[i].l, ia[i].m);
        }
    }
}

void ClearLevel(int final){
    //if its not the end we only clear the current level which is used after procedures, and if it is final we mark everything as not usable
    if(!final){
        for(int i = sc - 1; i >= 0; i--){
            if(st[i].level== currentLevel){
                st[i].mark = 1;
            }
        }
    } else {
        for(int i = sc - 1; i >= 0; i--){
            st[i].mark = 1;
        }
    }
}

int GetInt(char *str) {
    int total = 0;
    int sign = 1;
    //total is abs value sign is positive or negative
    if (str == NULL) return 0;

    if (str[0] == '-') {
        sign = -1;
        str++; //checks for negative sign
    }

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            total = total * 10 + (str[i] - '0');
        } else {
            return 0;
        }
        //simply does a check of all of the values
    }
    return total * sign; //returns positive or negative total
}

int checkForTable(char *check, int lev, int type) {
    if(type){
        //this is used to see if we have a variable in the current level, this is for declarations
        for(int i = sc - 1; i >= 0; i--){
            if(strcmp(check, st[i].name) == 0 && st[i].mark == 0 && st[i].level == lev){
                return i;
            }
        }
    } else {
        //this is used to see if we have a variable in all levels that are usable, this is for operations
        for(int i = sc - 1; i >= 0; i--){
            if(strcmp(check, st[i].name) == 0 && st[i].mark == 0 && st[i].level <= lev){
                return i;
            }
        }
    }
    return -1; // No copy found
}

void Error(int num){
    stopCompiling = 1;
    //we are done compiling and print a specific error value using a switch case
    switch(num){
        case(1):
            dPrint("Error number 1, use = instead of :=\n");
            break;
        case(2):
            dPrint("Error number 2, = must be followed by a number\n");
            break;
        case(3):
            dPrint("Error number 3, identifier must be followed by =\n");
            break;
        case(4):
            dPrint("Error number 4, const, var, procedure must be followed by identifier\n");
            break;
        case(5):
            dPrint("Error number 5, semicolon or comma missing\n");
            break;
        case(6):
            dPrint("Error number 6, incorrect symbol after procedure declaration\n");
            break;
        case(7):
            dPrint("Error number 7, statement expected\n");
            break;
        case(8):
            dPrint("Error number 8, incorrect symbol after statement part in block\n");
            break;
        case(9):
            dPrint("Error number 9, period expected\n");
            break;
        case(10):
            dPrint("Error number 10, semicolon between statements missing\n");
            break;
        case(11):
            dPrint("Error number 11, undeclared identifier\n");
            break;
        case(12):
            dPrint("Error number 12, assignment to constant or procedure is not allowed\n");
            break;
        case(13):
            dPrint("Error number 13, assignment operator expected\n");
            break;
        case(14):
            dPrint("Error number 14, call must be followed by an identifier\n");
            break;
        case(15):
            dPrint("Error number 15, call of a constant or variable is meaningless\n");
            break;
        case(16):
            dPrint("Error number 16, then expected\n");
            break;
        case(17):
            dPrint("Error number 17, semicolon or end expected\n");
            break;
        case(18):
            dPrint("Error number 18, do expected\n");
            break;
        case(19):
            dPrint("Error number 19, incorrect symbol following statement\n");
            break;
        case(20):
            dPrint("Error number 20, relational operator expected\n");
            break;
        case(21):
            dPrint("Error number 21, expression must not contain a procedure identifier\n");
            break;
        case(22):
            dPrint("Error number 22, right paranthesis missing\n");
            break;
        case(23):
            dPrint("Error number 23, the preceding factor cannot begin with this symbol\n");
            break;
        case(24):
            dPrint("Error number 24, an expression cannot begin with this symbol\n");
            break;
        case(25):
            dPrint("Error number 25, this number is too large\n");
            break;
        case(26):
            dPrint("Error number 26, identifer too long\n");
            break;
        case(27):
            dPrint("Error number 27, invalid symbol\n");
            break;
        case (28):
            dPrint("Error number 28, invalid syntax required to go over total amount of tokens\n");
            break;
        case (29):
            dPrint("Error number 29, symbol already exists on same scope");
            break;
    }
    //we also force quit the program
    exit(1);
}

void AddToTable(char* name, int v, int k){
    //depending on the type of varibale we use either the current address, value passed by and a bunch of detilas, essentially the only three values immportant is the name value and kind, for k=3 the memory is passed in the value
    strcpy(st[sc].name, name);
    st[sc].kind = k;
    st[sc].mark = 0;
    if (k == 1){
        st[sc].val = v;
        st[sc].addr = 0;
        st[sc].level = 0;
    } else if (k == 2){
        st[sc].val = 0;
        st[sc].addr = currentAddress++;
        st[sc].level = currentLevel;
    } else if (k == 3){
        st[sc].val = 0;
        st[sc].level = currentLevel;
        st[sc].addr = v;
    }
    sc++;
}

void checkFor(int k, int e, int checkFor){
    //sometimes we can check for two kinds, other times we check for one, fortunatey since we work with 3 types we can check if we don't have something its an error, or if we do have a kind its an error
    int inde = checkForTable(ta[ct].identity, currentLevel, 0);
    if(inde != -1){
        if(checkFor){
            if(st[inde].kind != k){
                Error(e);
            }
        } else {
            if(st[inde].kind == k){
                Error(e);
            }
        }
    } else {
        Error(11);
    }
}

int Expression();
int Term();
int Factor();
int RelOp();
int Condition();
void ConstDeclaration(int v);
void VarDeclaration(int v);
void ProcedureDeclaration();
void Statement();
void Block();
void Program();
//bunch of functions

int Condition(){
    if(ta[ct].token == oddsym){
        getToken();
        if(Expression()){
            //we check a bunch of values sunch as if we have an odd symbol and an expression then add the current instruction based off the grammar
            strcpy(ia[ic].op, "ODD");
            ia[ic].numOp = 2;
            ia[ic].l = 0;
            ia[ic++].m = 11;
            return 1;
        } else{
            Error(24); //error will come from expression, this won't be called
            return 0;
        }
    } else if (Expression()){
        //if we don't have odd then we need an expression and if we don't have an expression we get an error
        getToken();
        token_type tok = ta[ct].token;
        if(RelOp()){
            //we check for a relationship operator if not we get error and then check for expression
            getToken();
            if(Expression()){
                //prints necessary relationship operator
                ia[ic].l = 0;
                ia[ic].numOp = 2;
                if(tok == eqsym){
                    strcpy(ia[ic].op, "EQL");
                    ia[ic++].m = 5;
                } else if (tok == neqsym){
                    strcpy(ia[ic].op, "NEQ");
                    ia[ic++].m = 6;
                } else if (tok == leqsym){
                    strcpy(ia[ic].op, "LEQ");
                    ia[ic++].m = 8;
                } else if (tok == lessym){
                    strcpy(ia[ic].op, "LSS");
                    ia[ic++].m = 7;
                } else if (tok == geqsym){
                    strcpy(ia[ic].op, "GEQ");
                    ia[ic++].m = 10;
                } else {
                    strcpy(ia[ic].op, "GTR");
                    ia[ic++].m = 9;
                }
                return 1;
            } else{
                Error(24); //this won't be called error will be called in expression
                return 0;
            }
        } else {
            Error(20);
            return 0;
        }
    }
    return 0;
    Error(24);//won't get here, we will get the error from expression
}

int RelOp(){
    //just checks to see if we have a relationship operator
    token_type tok = ta[ct].token;
    if(tok == eqsym || tok == neqsym || tok == lessym || tok == leqsym || tok == gtrsym || tok == geqsym){
        return 1;
    }
    Error(20);
    return 0;
}

int Expression(){
    //checks to see if we have a term, and if we have a term we can check the next identity to see if we have a plus or minus if we do we progress two values to check if the next value is a term and use a while loop until we run out of operators
    if (Term()){
        while(ct + 1 < tc && (ta[ct + 1].token == plussym || ta[ct + 1].token == minussym)){
            getToken();
            int type = ta[ct].token == plussym ? 1 : 0;
            tokenPointer tok = getToken();
            if(!Term()){
                Error(23); //irrelevant will eventually end when we get to factor()
                return 0;
            }
             if(type){
                strcpy(ia[ic].op, "ADD");
                ia[ic].numOp = 2;
                ia[ic].l = 0;
                ia[ic++].m = 1;
            } else {
                strcpy(ia[ic].op, "SUB");
                ia[ic].numOp = 2;
                ia[ic].l = 0;
                ia[ic++].m = 2;
            }
        }
        return 1;
    }
    Error(24); //irrelevant will eventually end when we get to factor()
    return 0;
}

int Term(){
    if (Factor()){
        //check to see if we have a factor and similarly to expression we use a while loop to see how many multiplicative operators we have until we don't have any more
        while(ct + 1 < tc && (ta[ct + 1].token == multsym || ta[ct + 1].token == slashsym)){
            getToken();
            int type = ta[ct].token == multsym ? 1 : 0;
            tokenPointer tok = getToken();
            if(!Factor()){
                Error(23); //irrelevant we won't call this because factor will release an error first
                return 0;
            }
            if(type){
                strcpy(ia[ic].op, "MUL");
                ia[ic].numOp = 2;
                ia[ic].l = 0;
                ia[ic++].m = 3;
            } else {
                strcpy(ia[ic].op, "DIV");
                ia[ic].numOp = 2;
                ia[ic].l = 0;
                ia[ic++].m = 4;
            }
        }
        return 1;
    }
    Error(23); //irrelevant we won't call this because factor will release an error first
    return 0;
}

int Factor(){
    //we check to see if we have a value if so then we simply return that value
    tokenPointer tok = ta[ct];
    if(tok.token == numbersym || tok.token == identsym){
        if(tok.token == identsym){
            checkFor(3, 21, 0); //if we have a procedure identifier we can't use factor operations with it, this also calls error 21
            int ind = checkForTable(tok.identity, currentLevel, 0);
            if(st[ind].kind == 1){
                strcpy(ia[ic].op, "LIT"); //lit for numeric values
                ia[ic].numOp = 1;
                ia[ic].l = 0;
                ia[ic++].m = st[ind].val;
            } else {
                strcpy(ia[ic].op, "LOD"); //lod for variables
                ia[ic].numOp = 3;
                ia[ic].l = currentLevel - st[ind].level;
                ia[ic++].m = st[ind].addr;
            }
        }
        if(tok.token == numbersym){
            strcpy(ia[ic].op, "LIT"); //lit for number
            ia[ic].numOp = 1;
            ia[ic].l = 0;
            ia[ic++].m = GetInt(tok.identity); //gets int value
        }
        return 1;
    } else if (tok.token == lparentsym){
        //if we get a lparent we call an entire expression and treat it as a factor and we need a right parent afterwards or else we get an error
        getToken();
        if(Expression()){
            if(getToken().token != rparentsym){
                Error(22);
                return 0;
            } else {
                return 1;
            }
        } else {
            Error(24);
        }
    }
    Error(24);
    return 0;
}

void ConstDeclaration(int v){
    if (v){
        if(ta[ct].token == constsym){
            //we check for constant symbol if we don't have it we move on
            if(getToken().token == identsym){
                char name[12];
                strcpy(name, ta[ct].identity);
                if(checkForTable(name, 0, 1) != -1){
                    Error(29);
                }
                //we check our table and add the constant if it doesn't already have another identifier on the same level with the same name (all constants are glopbal so we can't have any other constants or identities on level 1 with the same name)
                if (getToken().token == eqsym){
                    if(getToken().token == numbersym){
                        AddToTable(name, GetInt(ta[ct].identity), 1);
                        tokenPointer tok = getToken();
                        if(tok.token == commasym){
                            //if we get comma we use recursive pattern if we get semicolon we return and move on
                            ConstDeclaration(0);
                        } else if(tok.token == semicolonsym){
                            getToken();
                            return;
                        } else {
                            Error(5);
                        }
                    } else {
                        Error(2);
                    }
                } else {
                    Error(3);
                }
            } else {
                Error(4);
            }
        }
    } else {
        //similar to above we just skip the constsym step, but everything else is the same and we keep using a recursive pattern until we get semicolon or error
        if(getToken().token == identsym){
            char name[12];
            strcpy(name, ta[ct].identity);
            if(checkForTable(name, 0, 1) != -1){
                Error(29);
            }
            if (getToken().token == eqsym){
                if(getToken().token == numbersym){
                    AddToTable(name, GetInt(ta[ct].identity), 1);
                    tokenPointer tok = getToken();
                    if(tok.token == commasym){
                        ConstDeclaration(0);
                    } else if(tok.token == semicolonsym){
                        getToken();
                        return;
                    } else {
                        Error(5);
                    }
                } else {
                    Error(2);
                }
            } else {
                Error(3);
            }
        } else {
            Error(4);
        }
    }
    
}

void VarDeclaration(int v){
    //if v is one we need the varsym at first, if we don't have the varsym its over and skip, additoinally if v is 0 then we are using a recursive pattern and constantly callling vardeclaration until we get a semicolon or error
    if(v){
        if(ta[ct].token == varsym){
            if(getToken().token == identsym){
                char name[12];
                strcpy(name, ta[ct].identity);
                //checks table for value to add
                if(checkForTable(name, currentLevel, 1) != -1){
                    Error(29);
                }
                AddToTable(name, 0, 2);
                tokenPointer tok = getToken();
                if(tok.token == commasym){
                    VarDeclaration(0); //calls vardeclaration again for the next part
                } else if(tok.token == semicolonsym){
                    getToken(); //if we get a semicolon we move on
                    return;
                } else {
                    Error(5);
                }
            } else {
                Error(4);
            }
        }
    } else {
        //similar logic to above but skips the varsym step
        if(getToken().token == identsym){
            char name[12];
            strcpy(name, ta[ct].identity);
            if(checkForTable(name, currentLevel, 1) != -1){
                Error(29);
            }
            AddToTable(name, 0, 2);
            tokenPointer tok = getToken();
            if(tok.token == commasym){
                VarDeclaration(0);
            } else if(tok.token == semicolonsym){
                getToken();
                return;
            } else {
                Error(5);
            }
        } else {
            Error(4);
        }
    }
}

void ProcedureDeclaration(){
    if(ta[ct].token == procsym){
        //if we get a procsymbol we keep going and if we don't we simply just skip this part
        if(getToken().token == identsym){
            //if we get an identity we check to see if we can add one and add it to our symbol table
            char name[12];
            strcpy(name, ta[ct].identity);
            if(checkForTable(name, currentLevel, 1) != -1){
                Error(29);
            }
            AddToTable(name, ic, 3);
            int prevIndex = sc;
            if(getToken().token == semicolonsym){
                //we need a semicolon then we call block and when we get our second semicolon we are done with the procedure declaration
                int previousLevel = currentLevel;
                int previousAddress = currentAddress;
                currentLevel ++;
                currentAddress = 3;
                getToken();
                Block();
                ClearLevel(0);
                currentLevel = previousLevel;
                currentAddress = previousAddress;
                if(ta[ct].token == semicolonsym){
                    getToken();
                    strcpy(ia[ic].op, "RTN"); //we return back
                    ia[ic].numOp = 2;
                    ia[ic].l = 0;
                    ia[ic++].m = 0;
                    ProcedureDeclaration(); //we call procedure declaration to see if we have another procedure or not
                } else {
                    Error(10);
                }
            } else {
                Error(10);
            }
        } else {
            Error(4);
        }
    } 
}

void Statement(){
    if(ta[ct].token == beginsym){
        //when we get a begin we keep getting statements until we are done with semicolons, since statements allow to be empty we can also not end with a semicolon as well or end with a semicolon
        getToken();
        Statement();
        while(ta[ct].token == semicolonsym){
            getToken();
            Statement();
        }
        if(ta[ct].token == endsym){
            //we must end begin with an end symbol
            getToken();
            return;
        } else {
            Error(17);
        }
    } else if (ta[ct].token == identsym){
        //when we have an identity we need to check to see that its a variable type identity
        checkFor(2, 12, 1);
        char *name = malloc(sizeof(char) * (strlen(ta[ct].identity) + 1));
        strcpy(name, ta[ct].identity);
        if(getToken().token == becomessym){
            getToken();
            //we check for a become symbol and then an expression and add a store instruction when its over
            if(Expression()){
                strcpy(ia[ic].op, "STO");
                ia[ic].numOp = 4;
                int ind = checkForTable(name, currentLevel, 0);
                ia[ic].l = currentLevel - st[ind].level;
                ia[ic++].m = st[ind].addr;
                free(name);
                getToken();
                return;
            }else{
                Error(24);//won't call this
            }
        } else {
            Error(1);
        }
    } else if (ta[ct].token == callsym){
        //we get a call symbol and check for an identity that is only a procedure if it isn't a procedure we call the 15th error
        if(getToken().token == identsym){
            checkFor(3, 15, 1);
            int ind = checkForTable(ta[ct].identity, currentLevel, 0);
            //we get neccessary call instructions
            strcpy(ia[ic].op, "CAL");
            ia[ic].numOp = 5;
            ia[ic].l = currentLevel - st[ind].level;
            ia[ic++].m = st[ind].addr * 3 + 10;
            getToken();
            return;
        } else {
            Error(14);
        }
    } else if (ta[ct].token == ifsym){
        //if we have an ifsym we check for if conditions
        getToken();
        if(Condition()){
            //we check for a condition and use a conditional jump and then use a statement
            int jpcIDX = ic;
            strcpy(ia[jpcIDX].op, "JPC");
            ia[ic].numOp = 8;
            ia[ic++].l = 0;
            if(getToken().token == thensym){
                getToken();
                Statement();
                ia[jpcIDX].m = ic * 3 + 10;
                if(ta[ct].token == fisym){
                    getToken();
                    return;
                } else {
                    Error(23);
                }
            } else {
                Error(16);
            }
        } else {
            Error(23); // will call error first
        }
    } else if (ta[ct].token == whilesym){
        getToken();
        //if we get while we get a condition check for dosym and get appropriate instructions given from the instructions
        int loopIdx = ic;
        if(Condition()){
            if(getToken().token == dosym){
                getToken();
                int jpcIDX = ic;
                strcpy(ia[ic].op, "JPC");
                //using instructions from assignment 3 we are able to determine the right jpc and jmp instruction values
                ia[ic].numOp = 8;
                ia[ic++].l = 0;
                Statement();
                strcpy(ia[ic].op, "JMP");
                ia[ic].numOp = 7;
                ia[ic].l = 0;
                ia[ic++].m = loopIdx * 3 + 10;
                ia[jpcIDX].m = ic * 3 + 10;
                return;
            } else {
                Error(18);
            }
        } else {
            Error(23);
        }
    } else if (ta[ct].token == readsym){
        //with read we need an identity next
        if(getToken().token == identsym){
            checkFor(2, 12, 1);
            strcpy(ia[ic].op, "SYS");
            ia[ic].numOp = 9;
            ia[ic].l = 0;
            ia[ic++].m = 2;
            strcpy(ia[ic].op, "STO");
            ia[ic].numOp = 4;
            //simple instructions needed for the system call and the store call
            int ind = checkForTable(ta[ct].identity, currentLevel, 0);
            ia[ic].l = currentLevel - st[ind].level;
            ia[ic++].m = st[ind].addr;
            getToken();
            return;
        } else {
            Error(23);
        }
    } else if (ta[ct].token == writesym){
        getToken();
        //we need write and then just need an expression
        if(Expression()){
            getToken();
            strcpy(ia[ic].op, "SYS");
            ia[ic].numOp = 9;
            ia[ic].l = 0;
            ia[ic++].m = 1;
            return;
        } else {
            Error(23); //won't call this
        }
    }
}

void Block(){
    int idx = ic;
    ia[ic].l = 0;
    ia[ic].numOp = 7;
    strcpy(ia[ic++].op, "JMP"); //we jump to where we need to inc
    //calls constant declaration, variable declaration, procedure declaration, and statement
    ConstDeclaration(1);
    VarDeclaration(1);
    ProcedureDeclaration();
    ia[idx].m = ic * 3 + 10;
    strcpy(ia[ic].op, "INC"); //gets increment
    ia[ic].numOp = 6;
    ia[ic].l = 0;
    ia[ic++].m = currentAddress;
    Statement();
}

void Program(){
    if(ta[tc - 1].token != periodsym){
        Error(9);
    }
    Block();
    if(ta[ct].token == periodsym && ct== (tc - 1)){
        strcpy(ia[ic].op, "SYS");
        ia[ic].numOp = 9;
        ia[ic].l = 0;
        ia[ic++].m = 3;
        ClearLevel(1);
        return;
    } else {
        Error(8);
    }
    
}

void PLCompiler(tokenPointer *head){
    //below is just to make an array
    tokenPointer* temp = head->nextToken;
    while(temp->nextToken != NULL){
        tc ++; //token count
        temp = temp->nextToken;
    }
    ta = malloc(sizeof(tokenPointer) * tc); //token array
    temp = head->nextToken;
    int index = 0;
    while(temp->nextToken != NULL){
        ta[index] = *temp;
        temp = temp->nextToken;
        index ++;
    }
    temp = head->nextToken;
    while(temp != NULL){
        free(head);
        head = temp;
        temp = temp->nextToken;
    }
    free(head);


    Program(); //we call the program after we get an array
}

void Lexemes(char* source){
    //elementNum is the current index in the source file, startindex and endindex are used to help hold locations for entire strings, skip is used to determine if we are in a comment, skip type is helped to determine if we are in a line comment or a block comment and skiptoken input is used to determine if we add an element to my token linked list
    int elementNum = 0, startIndex = 0, endIndex = 0, skip = 0, skipTokenInput = 0;
    //currentword helps let the program know if we are currently in a word, similarly currentnum helps let the program know if we are currently in a number
    int currentWord = 0, currentNum = 0;
    //simple print statement
    //head of our linked list
    tokenPointer* head = (tokenPointer*)(malloc)(sizeof(tokenPointer));
    head->nextToken = NULL;
    tokenPointer* next = head;
    //string holder used for our tokenpointer linked list later, 12 elements because if a string has more it cannot be a token
    char stringHolder[12];
    token_type currentToken; //our current token that we are using to help print and add to our tokenpointer

    while(elementNum < strlen(source) && !stopCompiling){
        //until we go through all of the elements we keep going
        if (!skip){
            //if we aren't skipping we come here
            skipTokenInput = 0;
            //we decide to not skip the token input
            if(elementNum != strlen(source) - 1){
                //if our element is one less than our entire length we don't really keep going but this is balanced out with an additional space we declared earlier
                if (source[elementNum] == '/' && source[elementNum + 1] == '*'){
                    //if we get a /* sign we assign skip value which means we will keep skipping in the future until we get our closing comment, we won't have a token this time so we skip
                    skipTokenInput = 1;
                    currentToken = oddsym;
                    skip = 1;
                    elementNum ++;
                    startIndex = elementNum; //we hold a startindex for later when returning in case of error
                } else {
                    if(source[elementNum] >= '0' && source[elementNum] <= '9'){
                        //if our current element is a number we check if we are currently in a word
                        if(!currentWord){
                            //if we are not in a word then we are either in a number or we are about to start a number
                            if(!currentNum){
                                //if we aren't already in a number we start a number and tell the program we are in a number, currentNum = 1 indicates we are working on a number
                                startIndex = elementNum;
                                currentNum = 1;
                            }
                            if(source[elementNum + 1] < '0' || source[elementNum + 1] > '9'){
                                endIndex = elementNum + 1;
                                currentNum = 0;
                                currentToken = numbersym;
                                int j = 0;
                                //if our next element isn't a number we can essentially declare that our number is over so we get our endindex and declare that we have a currenttoken
                                for(int i = startIndex; i < endIndex; i++){
                                    if(j < 5){
                                        stringHolder[j] = source[i];
                                        j++;
                                    }
                                    stringHolder[j] = '\0';
                                    //we go through all of the indexes and add them to our string and our printed terminal and if we have more than 5 elements we don't add it to stringholder because it won't go to our token dynamic link
                                }
                                if(endIndex - startIndex > 5){
                                    //we print an error if our number has more than 5 digits so we skip our on printing a token
                                    //printf("\t\tError: Number is too large\n");
                                    Error(25);
                                    skipTokenInput = 1;
                                } else {
                                    //this condition allows for us to print out a token
                                }
                            } else {
                                skipTokenInput = 1;
                                //if our next character is another digit we don't need to worry about printing a token until our number is complete
                            }
                        }else{
                            if(((source[elementNum + 1] < '0' || source[elementNum + 1] > '9')) && (source[elementNum + 1] < 'a' || source[elementNum + 1] > 'z') && (source[elementNum + 1] < 'A' || source[elementNum + 1] > 'Z')){
                                //here we are checking and we are currently in a word, if our next element is neither a number or a letter than the word is complete
                                endIndex = elementNum + 1;
                                currentWord = 0;
                                currentToken = identsym;
                                //we get our end index and identify our token as an identity and tell the program we are no longer working on a word
                                int j = 0;
                                for(int i = startIndex; i < endIndex; i++){
                                    //printf("%c", source[i]);
                                    if(j < 11){
                                        stringHolder[j] = source[i];
                                        j++;
                                    }
                                    //we copy the word to our stringholder but if its more than 11 elements we can't
                                }
                                stringHolder[j] = '\0';
                                if(endIndex - startIndex > 11){
                                    Error(26);
                                    skipTokenInput = 1;
                                    //here we declare if the word has more than 11 characters than the word is too long and we skip making a token and declare an error
                                } else {
                                    if(endIndex - startIndex > 7){
                                        //printf("\t%d\n", currentToken);
                                        //this is only for formatting purposes
                                    } else {
                                        //printf("\t\t%d\n", currentToken);
                                        //this is only for formatting purposes
                                    }
                                }
                            } else {
                                skipTokenInput = 1; //if we have another letter or number next we do not have to worry about making a token yet so we may skip making a token
                            }
                        }
                        
                    } else if (source[elementNum] == '.'){
                        currentToken = periodsym;
                        //printf(".\t\t%d\n", currentToken);
                        //if our current character is a period it is our next token
                    } else if ((source[elementNum] >= 'a' && source[elementNum] <= 'z') || (source[elementNum] >= 'A' && source[elementNum] <= 'Z')){
                        //here we are checking for all letters
                        if(!currentWord){
                            //if we aren't currently in a word then we start a new word
                            startIndex = elementNum;
                            currentWord = 1;
                        }
                        if((source[elementNum + 1] < 'a' || source[elementNum + 1] > 'z') && (source[elementNum + 1] < 'A' || source[elementNum + 1] > 'Z') && (source[elementNum + 1] < '0' || source[elementNum + 1] > '9')){
                            //here we check to see if our next character is either a letter or a number, if it isn't then it is the end of the word and we declare the end index and that we are no longer in a word
                            endIndex = elementNum + 1;
                            currentWord = 0;
                            char* currentString = (char*)malloc(endIndex - startIndex + 1 * sizeof(char));
                            //we mallocate space for our word
                            int j = 0;
                            for(int i = startIndex; i < endIndex; i++){
                                currentString[j] = source[i];
                                j++;
                                //we get our currentstring from our source's index
                            }
                            currentString[j] = '\0'; //we add a null terminator to declare end of string
                            if(strcmp(currentString, "if") == 0){
                                currentToken = ifsym; // we check for an if token
                            } else if (strcmp(currentString, "fi") == 0){
                                currentToken = fisym; //we check for a fi token
                            } else if (strcmp(currentString, "begin") == 0){
                                currentToken = beginsym; //we check for a begin token
                            } else if (strcmp(currentString, "then") == 0){
                                currentToken = thensym; //we check for a then token
                            } else if (strcmp(currentString, "write") == 0){
                                currentToken = writesym; //we check for a write token
                            } else if (strcmp(currentString, "var") == 0){
                                currentToken = varsym; //we check for a var token
                            } else if (strcmp(currentString, "const") == 0){
                                currentToken = constsym; //we check for a const token
                            } else if (strcmp(currentString, "while") == 0){
                                currentToken = whilesym; //we check for a while token
                            } else if (strcmp(currentString, "do") == 0){
                                currentToken = dosym; //we check for a do token
                            } else if(strcmp(currentString, "read") == 0){
                                currentToken = readsym; //we check for a read token
                            } else if (strcmp(currentString, "end") == 0){
                                currentToken = endsym; //we check for an end token
                            } else if (strcmp(currentString, "odd") == 0){
                                currentToken = oddsym; //we check for an end token
                            } else if(strcmp(currentString, "procedure") == 0){
                                currentToken = procsym;
                            } else if (strcmp(currentString, "call") == 0){
                                currentToken = callsym;
                            } else {
                                currentToken = identsym; //if we've checked all other word based tokens the only option left is an identifier token
                            }
                            if(endIndex - startIndex > 11){
                                Error(26);
                                skipTokenInput = 1;
                                //if our word is more than 11 characters long than it is an error and we do not make a token
                            }else {
                                if(endIndex - startIndex > 7){
                                        //printf("%s\t%d\n", currentString, currentToken);
                                        //formatting purposes
                                    } else {
                                        //printf("%s\t\t%d\n", currentString, currentToken);
                                        //formatting purposes
                                    }
                                strcpy(stringHolder, currentString);
                                //copies the current string to the string holder for later when we add our identity to our token pointer
                            }
                            free(currentString); //frees the current string
                        } else {
                            skipTokenInput = 1; //if the next symbol is a valid symbol for a word we do not need to make a token yet
                        }
                    } else if (source[elementNum] == ','){
                        currentToken = commasym;
                        //printf(",\t\t%d\n", currentToken);
                        //we check for a comma token
                    } else if (source[elementNum] == ':'){
                        if(source[elementNum + 1] == '='){
                            elementNum ++;
                            currentToken = becomessym;
                            //printf(":=\t\t%d\n", currentToken);
                            //we check for a become token if not we skip it because we print an error due to ":" alone being an invalid symbol
                        } else {
                            Error(27);
                            skipTokenInput = 1;
                        }
                    } else if (source[elementNum] == '<'){
                        if(source[elementNum + 1] == '='){
                            //checks to see if we have a less equal token and not equal token and a simple less than token, we equate the right token to the right value and if we have two symbols such as "<=" or "<>" our currentelement ticker increases
                            elementNum ++;
                            currentToken = leqsym;
                            //printf("<=\t\t%d\n", currentToken);
                        } else if(source[elementNum + 1] == '>'){
                            elementNum ++;
                            currentToken = neqsym;
                            //printf("<>\t\t%d\n", currentToken);
                        } else{
                            currentToken = lessym;
                            //printf("<\t\t%d\n", currentToken);
                        }
                    } else if(source[elementNum] == '>'){
                        //we check for greater than equal to or just greater than by determining if the following character is an = symbol if it is we increment the elementnumber but if not we acknowledge it is a simple greater than
                        if(source[elementNum + 1] == '='){
                            elementNum ++;
                            currentToken = geqsym;
                            //printf(">=\t\t%d\n", currentToken);
                        } else {
                            currentToken = gtrsym;
                            //printf(">\t\t%d\n", currentToken);
                        }
                    } else if (source[elementNum] == '='){
                        currentToken = eqsym;
                        //printf("=\t\t%d\n", currentToken);
                        //checks for equality, this works alone because we have already checked for := <= and >= so any other = alone would be a simple equal symbol
                    } else if (source[elementNum] == '*'){
                        //checks for multiplication token
                        currentToken = multsym;
                        //printf("*\t\t%d\n", currentToken);
                    } else if (source[elementNum] == '+'){
                        //checks for plus token
                        currentToken = plussym;
                        //printf("+\t\t%d\n", currentToken);
                    } else if (source[elementNum] == '-'){
                        //checks for minus token
                        currentToken = minussym;
                        //printf("-\t\t%d\n", currentToken);
                    } else if (source[elementNum] == '('){
                        //checks for left parenthesis token
                        currentToken = lparentsym;
                        //printf("(\t\t%d\n", currentToken);
                    } else if (source[elementNum] == ')'){
                        //checks for right paranthesis token
                        currentToken = rparentsym;
                        //printf(")\t\t%d\n", currentToken);
                    } else if (source[elementNum] == '/'){
                        //checks for slash token
                        currentToken = slashsym;
                        //printf("/\t\t%d\n", currentToken);
                    } else if (source[elementNum] == ';'){
                        //checks for semicolon token
                        currentToken = semicolonsym;
                        //printf(";\t\t%d\n", currentToken);
                    } else {
                        //if we haven't gotten through any other tokens then we are skipping tokens
                        skipTokenInput = 1;
                        if (source[elementNum] == ' ' || source[elementNum] == '\n' || source[elementNum] == '\t' || source[elementNum] == '\0' || source[elementNum] == '\r'){

                        } else {
                            //additionally if we get any symbols not already checked for earlier than we have an invalid symbol
                            Error(27);
                        }
                    }
                }
            } else {
                if(source[elementNum] == '.'){
                    currentToken = periodsym;
                    //printf(".\t\t%d\n", currentToken);
                }
            }
        } else {
            //if we are currently skipping then we have no token to make
            skipTokenInput = 1;
            if (source[elementNum] == '/' && source[elementNum - 1] == '*'){
                skip = 0;
                //we check to see if we ever close out the skipping and end skipping here
            }
            if(elementNum == (strlen(source) - 1)){
                //if we are at the end and still haven't found any close comment then we treat this as a / and * instead of a /* as a whole
                if(skip == 1){
                    //here we check if we are still skipping since we could have ended skipping in our previous if statement
                    //printf("Error: Never closed comment\n");
                    //printf("/\t\t%d\n*\t\t%d\n", slashsym, multsym);
                    //fprintf(ofp, "Error: Never closed comment\n");
                    //we declare we are currently not skipping anymore
                    skip = 0;
                    elementNum = startIndex;
                    //we go back to our startindex and add a / and a * to our token linked list
                    next->nextToken = (tokenPointer*)malloc(sizeof(tokenPointer));
                    next->nextToken->nextToken = (tokenPointer*)malloc(sizeof(tokenPointer));
                    next->nextToken->nextToken->nextToken = NULL;
                    next->nextToken->token = slashsym;
                    next->nextToken->nextToken->token = multsym;
                    next = next->nextToken->nextToken;
                }
            }
        }

        if(!skipTokenInput){
            //after we've determined if we are adding an element to our linked list we then decide to add our element
            next->nextToken = (tokenPointer*)malloc(sizeof(tokenPointer));
            next->nextToken->nextToken = NULL;
            next->nextToken->token = currentToken;
            //we create a new token
            if(currentToken == identsym || currentToken == numbersym){
                strcpy(next->nextToken->identity, stringHolder);
                //if we have an identity token or a current token we add an identity to it
            }
            next = next->nextToken; //we transverse and go to our next token
        }
        elementNum ++; //we increment our current index in our source
    }
    //printf("\nToken List:\n");

    if(!stopCompiling){
        PLCompiler(head);
    }
}

int main(int argc, char *argv[]){
    if(argc < 2 || argc > 3){
        if (argc < 2){
            printf("Not enough arguments\n"); //makes sure we dont have too little arguments
        } else {
            printf("Too many arguments\n"); //makes sure we dont have too many arguments
        }
        return 1; //ends program early since we don't have the right amount of arguments going into the program
    }

    
    if (argc == 2) {
        ofp = fopen("output.txt", "w");
    } else {
        ofp = fopen(argv[2], "w");
    }

    FILE *ifp = fopen(argv[1], "r");
    int elementCount = 0;
    char *sourceProgram;
    char v = fgetc(ifp);
    while(v != EOF){
        elementCount += 1; //calculates the proper amount of elements needed in our source program
        v = fgetc(ifp); //essentailly increments the pointer in the file
    }
    sourceProgram = (char*)(malloc)((elementCount + 2) * sizeof(char)); //adds two spaces one for a blank space and one for a null terminator
    fclose(ifp);
    fopen(argv[1], "r");
    for(int i = 0; i < elementCount; i++){
        sourceProgram[i] = fgetc(ifp); //gets the element at the correpsonding spot
    }
    sourceProgram[elementCount] = '\0'; //adds the null terminator to the string
    //printf("Source Program:\n%s\n\n", sourceProgram); //prints the source program
    //fprintf(ofp, "Source Program:\n%s\n\n", sourceProgram);
    sourceProgram[elementCount] = ' '; //saves a bunch of check forward logic later in my printLexemes function
    sourceProgram[elementCount + 1] = '\0'; //adds the null terminator again
    fclose(ifp); // closes the file holding the source program
    Lexemes(sourceProgram); //prints the lexemes and does the rest of the program
    printELF(); //prints the elf file
    finalPrint(sourceProgram); //prints the final statement
    return 0;
}
