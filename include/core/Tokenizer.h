#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <string>
#include <vector>
#include <queue>
#include "core/Token.h"

class Tokenizer {
public:
    Tokenizer(const std::string& input);

    // Get the next token (consumes it)
    Token nextToken();

    // Peek at the next token without consuming
    Token peekToken();

    // Check if at end of input
    bool isAtEnd() const;

private:
    std::string m_input;
    size_t m_pos;
    size_t m_line;
    size_t m_col;
    Token m_peekedToken;
    bool m_hasPeeked;

    // Indentation handling
    std::vector<int> m_indentStack; // stack of indentation levels
    std::queue<Token> m_pendingIndents; // queue of INDENT/DEDENT tokens to emit
    bool m_atLineStart = true; // are we at the start of a new line?

    void skipWhitespace();
    char peekChar() const;
    char getChar();
    bool matchChar(char expected);
    Token lexNumber();
    Token lexIdentifierOrKeyword();
    Token lexString();
    Token lexOperatorOrDelimiter();
    void processIndentation();
};

#endif // TOKENIZER_H
