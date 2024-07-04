#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include "token.h"
using namespace std;


// Token types
/*enum TokenType {
    KEYWORD,
    IDENTIFIER,
    DELIMITER,
    OPERATOR,
    INTEGER_LITERAL,
    CHAR_LITERAL,
    STRING_LITERAL,
    INVALID
};*/

// Token structure
/*struct Token {
    TokenType type;
    string value;
    int line;
    int position;
};*/

// Global Symbol Table
vector<Token>SymbolTable;

// Lexical analyzer class
class Lexer {
public:
    Lexer(const string& filename) : line(1), position(1) {
        inputFile.open(filename);
        if (!inputFile.is_open()) {
            cerr << "Error: Unable to open file " << filename << endl;
            exit(EXIT_FAILURE);
        }
    }

    ~Lexer() {
        inputFile.close();
    }

    // Get next token
    Token getNextToken() {
        string value;
        char currentChar;

        if (inputFile.eof()) {
            return {INVALID, "", line, position};
        }

        while(inputFile.get(currentChar)){
            value = "";
            value += currentChar;
            position++;

            if (currentChar == '\n') {
                line++;
                position = 1;
            }

            if (!isspace(currentChar)) {
                if (isdigit(currentChar)) {
                    return getIntegerLiteral(value);
                }

                if (isalpha(currentChar)) {
                    return getIdentifierOrKeyword(value);
                }

                if (isDelimiter(currentChar)) {
                    return getDelimiter(value);
                }

                if (isOperator(currentChar)) {
                    return getOperator(value);
                }

                if (currentChar == '\'') {
                    return getCharLiteral(value);
                }

                if (currentChar == '"') {
                    return getStringLiteral(value);
                }
                return {INVALID, value, line, position};
            }
        }
        
        return {INVALID, "", line, position};
    }
    bool addToken(Token t){
        SymbolTable.push_back(t);
        return 1;
    }

private:
    ifstream inputFile;
    int line;
    int position;
    

    // Check if a character is a delimiter
    bool isDelimiter(char c) {
        return c == ';' || c == '(' || c == ')' || c == '{' || c == '}';
    }

    // Get delimiter token
    Token getDelimiter(string& value) {
        return {DELIMITER, value, line, int(position - value.length())};
    }

    // Check if a character is an operator
    bool isOperator(char c) {
        return c == '+' || c == '-' || c == '*' || c == '^' || c == '=' || c == '<' || c == '>';
    }

    // Get operator token
    Token getOperator( string& value) {
        return {OPERATOR, value, line, int(position - value.length())};
    }

    // Get integer literal token
    Token getIntegerLiteral(string& value) {
        char currentChar;
        while (inputFile.get(currentChar) && isdigit(currentChar)) {
            value += currentChar;
            position++;
        }
        inputFile.unget(); // Put back the non-digit character
        
        return {INTEGER_LITERAL, value, line, int(position - value.length())};
    }

    // Get character literal token
    Token getCharLiteral(string& value) {
        char currentChar;
        value = "";
        while (inputFile.get(currentChar) && currentChar != '\'') {
            value += currentChar;
            position++;
        }
        
        return {CHAR_LITERAL, value, line, int(position - value.length())};
    }

    // Get string literal token
    Token getStringLiteral(string& value) {
        char currentChar;
        value = "";
        while (inputFile.get(currentChar) && currentChar != '"') {
            value += currentChar;
            position++;
        }
        
        return {STRING_LITERAL, value, line, int(position - value.length())};
    }

    // Get identifier or keyword token
    Token getIdentifierOrKeyword(string& value) {
        char currentChar;
        while (inputFile.get(currentChar) && (isalnum(currentChar) || currentChar == '_')) {
            value += currentChar;
            position++;
        }
        inputFile.unget(); // Put back the non-alphanumeric character
        
        if (value =="void" || value =="main" || value == "int" || value == "char" || value == "string" || value == "if" || value == "else" || value == "for") {
            return {KEYWORD, value, line, int(position - value.length())};
        } else {
            return {IDENTIFIER, value, line, int(position - value.length())};
        }
    }

    // Create token
    Token createToken(const string& value, int startPosition) {
        return {IDENTIFIER, value, line, startPosition};
    }
};

// Function to generate tokens and add them to the Symbol Table
vector<string> getTokens(string filename){
    vector<string>tokens;
    
    Lexer lexer(filename);
    Token token;
    do {
        token = lexer.getNextToken();
        if (token.type != INVALID) {
            
            switch (token.type) {
                case KEYWORD:
                    
                    tokens.push_back(token.value);
                    break;
                case IDENTIFIER:
                    
                    tokens.push_back("id");
                    break;
                case DELIMITER:
                    
                    tokens.push_back(token.value);
                    break;
                case OPERATOR:
                    
                    tokens.push_back(token.value);
                    break;
                case INTEGER_LITERAL:
                    
                    tokens.push_back("int_l");
                    break;
                case CHAR_LITERAL:
                    
                    tokens.push_back("char_l");
                    break;
                case STRING_LITERAL:
                    
                    tokens.push_back("str_l");
                    break;
                case INVALID:
                    
                    break;
            }
            lexer.addToken(token);
            
        }
    } while (token.type != INVALID);
    
    return tokens;
}



