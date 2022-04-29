#include <vector>
#include <string>

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

class FunctionDefnAST{
    FunctionDeclAST *Func_Decl;
    BaseAST *Body;

    public:
        FunctionDefnAST(FunctionDeclAST *proto, BaseAST *body) :
            Func_Decl(proto), Body(body) {};
};

class FunctionCallAST{
    std::string Function_Callee;
    std::vector<BaseAST*> Function_Arguments;

    public:
        FunctionCallAST(const std::string &callee,
                        std::vector<BaseAST*> &args) :
                            Function_Callee(callee),
                            Function_Arguments(args) {};
};