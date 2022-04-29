# Writing a Frontend 

## Defining a toy language

## Implementing a lexer

Lexer is the first phase in compiling a program. A lexer tokenizes a stream of
input into a program, then the parser will consume these tokens to construct an
[AST](https://en.wikipedia.org/wiki/Abstract_syntax_tree).

A **token** is a string of one or more characters that have a significant
meaning as a group. **Tokenization** is the process of forming tokens given an
input stream of characters.

When implementing a lexer, we define types of tokens using an
**enumeration (enum)** type :

```cpp
enum Token_type {
    EOF_TOKEN = 0,    //States the end of file
    NUMERIC_TOKEN,    //Current token is a numeric
    IDENTIFIER_TOKEN, //Current token is identifier
    PARAN_TOKEN,      //Token is a parenthesis
    DEF_TOKEN         //What is next will be a func definition
};
```