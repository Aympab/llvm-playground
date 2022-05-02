#include "lexer.cpp"
#include "ast.cpp"
#include <map>

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