enum Token_type {
    EOF_TOKEN = 0,    //States the end of file
    NUMERIC_TOKEN,    //Current token is a numeric
    IDENTIFIER_TOKEN, //Current token is identifier
    PARAN_TOKEN,      //Token is a parenthesis
    DEF_TOKEN         //This token states that what is next is a func definition
};