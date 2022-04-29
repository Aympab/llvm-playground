#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include "llvm-10/llvm/IR/LLVMContext.h"
#include "llvm-10/llvm/IR/Module.h"
#include "llvm-c-10/llvm-c/Types.h"

// static std::string file="FUNCDEF foo (x, y)\nx + y * 16";
FILE* file;

enum Token_type {
    EOF_TOKEN = 0,    //States the end of file
    NUMERIC_TOKEN,    //Current token is a numeric
    IDENTIFIER_TOKEN, //Current token is identifier
    PARAN_TOKEN,      //Token is a parenthesis
    DEF_TOKEN         //This token states that what is next is a func definition
};

static int Numeric_Val; //To hold numeric values
static std::string Identifier_String; //Holds the identifier string name

static int get_token(){
    static int LastChar = ' ';

    while(isspace(LastChar))
        LastChar = fgetc(file);

    if(isalpha(LastChar)){
        Identifier_String = LastChar;

        while(isalnum((LastChar = fgetc(file))))
            Identifier_String += LastChar;

        if(Identifier_String == "FUNCDEF")
            return DEF_TOKEN;
        return IDENTIFIER_TOKEN;
    }

    if(isdigit(LastChar)){
        std::string NumStr;
        do{
            NumStr += LastChar;
            LastChar = fgetc(file);
        } while(isdigit(LastChar));

        Numeric_Val = strtod(NumStr.c_str(), 0);
        return NUMERIC_TOKEN;
    }

    if(LastChar == '#'){
        do LastChar = fgetc(file);
            while(LastChar != EOF  &&
                  LastChar != '\n' &&
                  LastChar != '\r');

        if(LastChar == EOF) return get_token();
    }

    if(LastChar == EOF) return EOF_TOKEN;

    int ThisChar = LastChar;
    LastChar = fgetc(file);
    return ThisChar;
}

class BaseAST{
    public:
        virtual ~BaseAST();
};

class VariableAST : public BaseAST {
    std::string Var_Name; //str object to store name of the variable

    public:
        VariableAST(std::string &name) : Var_Name(name) {};
};

class NumericAST : public BaseAST {
    int Numeric_Val; //str object to store name of the variable

    public:
        NumericAST(int val) : Numeric_Val(val) {};
};

class BinaryAST : public BaseAST {
    std::string Bin_Operator;

    BaseAST *LHS, *RHS;

    public:
        BinaryAST(std::string op, BaseAST* lhs, BaseAST* rhs):
            Bin_Operator(op),
            LHS(lhs),
            RHS(rhs) {};
};

class FunctionDeclAST {
    std::string Func_Name;
    std::vector<std::string> Arguments;

    public:
        FunctionDeclAST(const std::string &name,
                        const std::vector<std::string> &args) : 
                            Func_Name(name), Arguments(args) {};
};

class FunctionDefnAST {
    FunctionDeclAST *Func_Decl;
    BaseAST *Body;

    public:
        FunctionDefnAST(FunctionDeclAST *proto, BaseAST *body) :
            Func_Decl(proto), Body(body) {};
};

class FunctionCallAST : BaseAST {
    std::string Function_Callee;
    std::vector<BaseAST*> Function_Arguments;

    public:
        FunctionCallAST(const std::string &callee,
                        std::vector<BaseAST*> &args) :
                            Function_Callee(callee),
                            Function_Arguments(args) {};
};


static int Current_Token;

static void next_token(){
    Current_Token = get_token();
}

static BaseAST* Base_Parser(){
    switch (Current_Token){
        default : return 0;
        case IDENTIFIER_TOKEN : return identifier_parser();
        case NUMERIC_TOKEN :    return numeric_parser();
        case '(' :              return paran_parser();
    }
}

static BaseAST* numeric_parser(){
    BaseAST *Result = new NumericAST(Numeric_Val);
    next_token();
    return Result;
}

static BaseAST* identifier_parser(){
    std::string IdName = Identifier_String;

    next_token();

    if(Current_Token != '(')
        return new VariableAST(IdName);
    
    next_token();

    std::vector<BaseAST*> Args;
    if(Current_Token != ')'){
        while(true){
            BaseAST* Arg = expression_parser();
            if(!Arg) return 0;
            Args.push_back(Arg);

            if(Current_Token == ')') break;

            if(Current_Token != ',') return 0;
            next_token();
        }
    }
    next_token();

    return new FunctionCallAST(IdName, Args);
}

static FunctionDeclAST *func_decl_parser(){
    if(Current_Token != IDENTIFIER_TOKEN)
        return 0;

    std::string FnName = Identifier_String;
    next_token();

    if(Current_Token != '(')
        return 0;

    std::vector<std::string> Function_Arguments_Names;
    
    while(next_token() == IDENTIFIER_TOKEN)
        Function_Arguments_Names.push_back(Identifier_String);

    if(Current_Token != ')')
        return 0;
    
    next_token();

    return new FunctionDeclAST(FnName, Function_Arguments_Names);
}

static FunctionDefnAST* func_defn_parser(){
    next_token();
    FunctionDeclAST *Decl = func_decl_parser();
    if(Decl == 0) return 0;

    if(BaseAST *Body = expression_parser())
        return new FunctionDefnAST(Decl, Body);
    
    return 0;
}

static BaseAST* expression_parser(){
    BaseAST *LHS = Base_Parser();
    if(!LHS) return 0;
    return binary_op_parser(0, LHS);
}

// Parsing binary expressions
static std::map<char, int> Operator_Precedence;

static void init_precedence(){
    Operator_Precedence['-'] = 1;
    Operator_Precedence['+'] = 2;
    Operator_Precedence['/'] = 3;
    Operator_Precedence['*'] = 4;
}

static int getBinOpPrecedence(){
    if(!isascii(Current_Token)) return -1;

    int TokPrec = Operator_Precedence[Current_Token];
    if(TokPrec <= 0) return -1;
    return TokPrec;
}

static BaseAST* binary_op_parser(int Old_Prec, BaseAST *LHS){
    while (true)
    {
        int Operator_Prec = getBinOpPrecedence();

        if(Operator_Prec < Old_Prec) return LHS;

        int BinOp = Current_Token;
        next_token();

        BaseAST* RHS = Base_Parser();
        if(!RHS) return 0;

        int Next_Prec = getBinOpPrecedence();
        if(Operator_Prec < Next_Prec){
            RHS = binary_op_parser(Operator_Prec+1, RHS);
            if(RHS == 0) return 0;
        }

        LHS = new BinaryAST(std::to_string(BinOp), LHS, RHS);
    }
}

static BaseAST* paran_parser(){
    next_token();
    BaseAST* V = expression_parser();
    if(!V) return 0;

    if(Current_Token != ')')
        return 0;

    return V;
}

static void HandleDefn(){
    if(FunctionDefnAST *F = func_defn_parser()){
        if(Function* LF = F->Codegen()){

        }
    }
    else
    {
        next_token();
    }
}

static void HandleTopExpression(){
    if(FunctionDefnAST* F = top_level_parser()){
        if(Function *LF = F->Codegen()){

        }
    }
    else{
        next_token();
    }
}

static void Driver(){
    while(true){
        switch (Current_Token)
        {
        case EOF_TOKEN : return;
        case ';' : next_token(); break;
        case DEF_TOKEN : HandleDefn(); break;   
        default : HandleTopExpression(); break;
        }
    }
}

int main(int argc, char* argv[]){
    LLVMContext &Context = getGlobalContext();
    init_precedence();
    file = fopen(argv[1], "r");
    if(file == 0){
        printf("Could not open file\n");
    }
    next_token();
    Module_Ob = new Module("my compiler", Context);
    Driver();
    Module_Ob->dump();
    return 0;
}