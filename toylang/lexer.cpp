#include <cstdio>
#include <string>

// static std::string file="FUNCDEF foo (x, y)\nx + y * 16";
// _IO_FILE* file;

enum Token_type {
    EOF_TOKEN = 0,    //States the end of file
    NUMERIC_TOKEN,    //Current token is a numeric
    IDENTIFIER_TOKEN, //Current token is identifier
    PARAN_TOKEN,      //Token is a parenthesis
    DEF_TOKEN         //This token states that what is next is a func definition
};

static int Numeric_Val; //To hold numeric values

static std::string Identifier_string; //Holds the identifier string name

static int get_token(){
    static int LastChar = ' ';

    while(isspace(LastChar))
        LastChar = fgetc(file);

    if(isalpha(LastChar)){
        Identifier_string = LastChar;

        while(isalnum((LastChar = fgetc(file))))
            Identifier_string += LastChar;

        if(Identifier_string == "FUNCDEF")
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