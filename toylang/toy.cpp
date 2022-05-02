#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
// clang++ toy.cpp -O3 -o toy
// clang++ -O3 toy.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o toy
// g++ -O3 toy.cpp -g `llvm-config --cxxflags --ldflags --system-libs --libs core` -o toy

// static std::string file="FUNCDEF foo (x, y)\nx + y * 16";
FILE* file;

static std::unique_ptr<llvm::LLVMContext> Context;
static std::unique_ptr<llvm::IRBuilder<>> Builder;
static std::unique_ptr<llvm::Module> Module_Ob;
static std::map<std::string, llvm::Value *> Named_Values;

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
        printf("%c\n", LastChar);

        while(isalnum((LastChar = fgetc(file))))
            Identifier_String += LastChar;

        if(Identifier_String == "def")
            return DEF_TOKEN;
        // if (IdentifierStr == "extern")
        // return tok_extern;
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
        ~BaseAST();
        virtual llvm::Value* Codegen() = 0;
};

class VariableAST : public BaseAST {
    std::string Var_Name; //str object to store name of the variable

    public:
        VariableAST(std::string &name) : Var_Name(name) {};
        virtual llvm::Value* Codegen();
};

llvm::Value* VariableAST::Codegen(){
    llvm::Value *V = Named_Values[Var_Name];
    return V ? V : 0;
}

class NumericAST : public BaseAST {
    int Numeric_Val; //str object to store name of the variable

    public:
        NumericAST(int val) : Numeric_Val(val) {};
        virtual llvm::Value* Codegen();

};
llvm::Value* NumericAST::Codegen(){
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*Context), Numeric_Val);
}

class BinaryAST : public BaseAST {
    std::string Bin_Operator;

    BaseAST *LHS, *RHS;

    public:
        BinaryAST(std::string op, BaseAST* lhs, BaseAST* rhs):
            Bin_Operator(op),
            LHS(lhs),
            RHS(rhs) {};
        
        virtual llvm::Value* Codegen();
        
};
llvm::Value* BinaryAST::Codegen(){
    llvm::Value *L = LHS->Codegen();
    llvm::Value *R = RHS->Codegen();
    if(L == 0 || R == 0) return 0;

    switch (atoi(Bin_Operator.c_str()))
    {
    case '+' : return Builder->CreateAdd(L, R, "addtmp");
    case '-' : return Builder->CreateSub(L, R, "subtmp");
    case '*' : return Builder->CreateMul(L, R, "multmp");
    case '/' : return Builder->CreateUDiv(L, R, "divtmp");
    default: return 0;
    }
}


class FunctionDeclAST {
    std::string Func_Name;
    std::vector<std::string> Arguments;

    public:
        const std::string &getName(){ return Func_Name; };
        FunctionDeclAST(const std::string &name,
                        const std::vector<std::string> &args) : 
                            Func_Name(name), Arguments(args) {};

        virtual llvm::Value* Codegen();
};
llvm::Value* FunctionDeclAST::Codegen(){
    std::vector<llvm::Type*>Integers(Arguments.size(), llvm::Type::getInt32Ty(*Context));
    llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(*Context), Integers, false);
    llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, Func_Name, Module_Ob.get());

    if(F->getName() != Func_Name){
        F->eraseFromParent();
        F = Module_Ob->getFunction(Func_Name);

        if(!F->empty()) return 0;
        if(F->arg_size() != Arguments.size()) return 0;
    }

    unsigned Idx = 0;
    for(llvm::Function::arg_iterator Arg_It = F->arg_begin(); 
        Idx != Arguments.size(); ++Arg_It, ++Idx){

        Arg_It->setName(Arguments[Idx]);
        Named_Values[Arguments[Idx]] = Arg_It;
    }

    return F;
}

class FunctionDefnAST {
    BaseAST *Body;

    public:
        FunctionDeclAST *Func_Decl;
        FunctionDefnAST(FunctionDeclAST *proto, BaseAST *body) :
            Func_Decl(proto), Body(body) {};

        llvm::Value* Codegen();
};
llvm::Value* FunctionDefnAST::Codegen(){
    Named_Values.clear();

    std::cout << "Before seg fault" << std::endl;
    // std::cout << "FNAME IS : " << Func_Decl->getName();
    // llvm::Function *TheFunction = Module_Ob->getFunction(Func_Decl->getName());
    std::cout << "After seg fault" << std::endl;
    llvm::Function *TheFunction = (llvm::Function*)Func_Decl-> Codegen();
    if(TheFunction == 0) return 0;

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*Context, "entry", TheFunction);
    Builder->SetInsertPoint(BB);

    if(llvm::Value *RetVal = Body->Codegen()){
        Builder->CreateRet(RetVal);
        llvm::verifyFunction(*TheFunction);
        return TheFunction;
    }

    TheFunction->eraseFromParent();
    return 0;
}

class FunctionCallAST : public BaseAST {
    std::string Function_Callee;
    std::vector<BaseAST*> Function_Arguments;

    public:
        FunctionCallAST(const std::string &callee,
                        std::vector<BaseAST*> &args) :
                            Function_Callee(callee),
                            Function_Arguments(args) {};
        virtual llvm::Value* Codegen();
};

llvm::Value* FunctionCallAST::Codegen(){
    llvm::Function *CalleF = Module_Ob->getFunction(Function_Callee);
    std::vector<llvm::Value*> ArgsV;

    for(unsigned i = 0, e = Function_Arguments.size(); i != e; ++i){
        ArgsV.push_back(Function_Arguments[i]->Codegen());
        if(ArgsV.back() == 0) return 0;
    }
    return Builder->CreateCall(CalleF, ArgsV, "calltmp");
}

static int Current_Token;

static BaseAST* identifier_parser();
static BaseAST* numeric_parser();
static BaseAST* paran_parser();

static int next_token(){
    Current_Token = get_token();
    return Current_Token;
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

static BaseAST* expression_parser();

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
    // printf("TOKEN IS : %d\n", Current_Token);
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
    // std::cout << "In func defn parser" << std::endl;
    FunctionDeclAST *Decl = func_decl_parser();

    if(Decl == 0) return 0;

    if(BaseAST *Body = expression_parser()){
        std::cout << "My name : " << Decl->getName() << std::endl;
        // std::cout << "My name : " << Decl-> << std::endl;
        return new FunctionDefnAST(Decl, Body);
    }
    
    return 0;
}

//if we have a top level expression
static FunctionDefnAST* top_level_parser(){
    if(BaseAST* F = expression_parser()){
        auto Decl = new FunctionDeclAST("", std::vector<std::string>());  //we make an empty func decl
        return new FunctionDefnAST(Decl, F);
    }
    return nullptr;
    
    // next_token();
    // FunctionDeclAST *Decl = func_decl_parser();
    // if(Decl == 0) return 0;

    // if(BaseAST *Body = expression_parser())
    //     return new FunctionDefnAST(Decl, Body);
}

// /// toplevelexpr ::= expression
// static std::unique_ptr<FunctionAST> ParseTopLevelExpr() {
//   if (auto E = ParseExpression()) {
//     // Make an anonymous proto.
//     auto Proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
//     return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
//   }
//   return nullptr;
// }


static BaseAST* binary_op_parser(int Old_Prec, BaseAST *LHS);

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
    std::cout << "In HandleDefn" << std::endl;
    if(FunctionDefnAST *F = func_defn_parser()){
        std::cout << "FNAME = " << F->Func_Decl->getName()<< std::endl;
        if(auto* LF = F->Codegen()){

        }
    }
    else
    {
        next_token();
    }
}

static void HandleTopExpression(){
    if(FunctionDefnAST* F = top_level_parser()){
        if(auto *LF = F->Codegen()){

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
        // default : next_token(); break;
        }
    }
}

// Value *LogErrorV(const char *Str) {
//   LogError(Str);
//   return nullptr;
// }
// static llvm::LLVMContext *Context;

int main(int argc, char* argv[]){
    init_precedence();
    file = fopen(argv[1], "r");
    if(file == 0){
        printf("Could not open file\n");
    }
    next_token();

    Context = std::make_unique<llvm::LLVMContext>();
    auto Module_Ob = new llvm::Module("my compiler", *Context);
    Builder = std::make_unique<llvm::IRBuilder<>>(*Context);

    Driver();
    Module_Ob->print(llvm::errs(), nullptr);

    return 0;
}
