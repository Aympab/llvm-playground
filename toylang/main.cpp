#include "parser.cpp"
#include "lexer.cpp"
#include <llvm-10/llvm/IR/LLVMContext.h>
#include <llvm-10/llvm/IR/Module.h>

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