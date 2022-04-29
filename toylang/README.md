# Writing a Frontend 

## 1. Defining a toy language

TODO : define syntax 
<br></br>

## 2. Implementing a lexer

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

Then, the lexer will read the file char by char and eventually will return a
`Token_type` if the stream of charaacters have a meaning (e.g. char 'd', 'e' and
'f' will return a `DEF_TOKEN` from a function definition). This work is done in
the `get_token()` function.

For a real example, see
[the clang lexer](https://clang.llvm.org/doxygen/Lexer_8cpp_source.html).
<br></br>

## 3. Defining the Abstract Syntax Tree

[AST](https://en.wikipedia.org/wiki/Abstract_syntax_tree) is a tree
representation of the syntactic structure of the source code, with abstraction
of the language's syntax. ASTs ignore programming elements such as punctuations
or delimiters. 

Generally, ASTs contain additional properties of every elements for further
compilation phases, such as the location of the source code to throw an error
with a line number. Other examples are :
- Line number
- Column number
- File location
- ...

The AST is intensively used during semantic analysis : a complete traversal of
the tree allows to check the corectness of the program. After this phase, the
AST is used as a base for **code generation**.
<br></br>

## 4. Implementing a parser

The parser analyzes the code syntactically according to the rules of the
language's grammar. The parsing phase determines if the input code can be used
to form a string of tokens according to the grammar. The parser defines
functions to organize the language into an AST. There are many parsing
techniques, here we will use a
[recursive descent top-down](https://www.geeksforgeeks.org/recursive-descent-parser/)
parser technique.