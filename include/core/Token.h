#ifndef TOKEN_H
#define TOKEN_H

#include <ostream>
#include <string>
#include <variant>

enum class TokenType {
    // Literals
    Number,
    String,

    // Identifiers
    Identifier,

    // Keywords
    For,
    While,
    If,
    Else,
    In,
    Def,
    Return,
    Break,
    Continue,
    True,
    False,
    None,

    // Operators
    Plus,       // +
    Minus,      // -
    Star,       // *
    Slash,      // /
    Percent,    // %
    Power,      // **
    Assign,     // =
    PlusAssign, // +=
    MinusAssign,// -=
    Equal,      // ==
    NotEqual,   // !=
    Less,       // <
    Greater,    // >
    LessEqual,  // <=
    GreaterEqual,// >=
    And,        // and
    Or,         // or
    Not,        // not

    // Delimiters
    Colon,      // :
    Comma,      // ,
    Dot,        // .
    Semicolon,  // ;

    // Brackets
    LeftParen,  // (
    RightParen, // )
    LeftBracket,// [
    RightBracket,// ]
    LeftBrace,  // {
    RightBrace, // }

    // Special
    Indent,
    Dedent,
    Newline,
    EndOfInput,
};


struct Token {
    TokenType type;
    std::string text; // The actual text from input
    std::variant<std::monostate, double, std::string> value; // For numbers, strings, identifiers

    Token(TokenType t, const std::string& txt);
    Token(TokenType t, const std::string& txt, double num);
    Token(TokenType t, const std::string& txt, const std::string& str);
};


const char* tokenTypeToString(TokenType type);

std::ostream& operator<<(std::ostream& os, const Token& token);

#endif // TOKEN_H
