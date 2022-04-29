#include "lexer.cpp"
#include "ast.cpp"

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
        while(1){
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

static BastAST* expression_parser(){
    BastAST *LHS = Base_Parser();
    if(!LHS) return 0;
    return binary_op_parser(0, LHS);
}