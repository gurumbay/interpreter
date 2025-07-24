#include "Token.h"

Token::Token(TokenType t, const std::string& txt)
: type(t), text(txt), value(std::monostate{}) {}

Token::Token(TokenType t, const std::string& txt, double num)
: type(t), text(txt), value(num) {}

Token::Token(TokenType t, const std::string& txt, const std::string& str)
: type(t), text(txt), value(str) {}


const char* tokenTypeToString(TokenType type) {
    switch (type) {
        // Literals
        case TokenType::Number: return "Number";
        case TokenType::String: return "String";

        // Identifiers
        case TokenType::Identifier: return "Identifier";

        // Keywords
        case TokenType::For: return "For";
        case TokenType::While: return "While";
        case TokenType::If: return "If";
        case TokenType::Else: return "Else";
        case TokenType::In: return "In";
        case TokenType::Def: return "Def";
        case TokenType::Return: return "Return";
        case TokenType::Break: return "Break";
        case TokenType::Continue: return "Continue";
        case TokenType::True: return "True";
        case TokenType::False: return "False";
        case TokenType::None: return "None";

        // Operators
        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Star: return "Star";
        case TokenType::Slash: return "Slash";
        case TokenType::Percent: return "Percent";
        case TokenType::Power: return "Power";
        case TokenType::Assign: return "Assign";
        case TokenType::PlusAssign: return "PlusAssign";
        case TokenType::MinusAssign: return "MinusAssign";
        case TokenType::Equal: return "Equal";
        case TokenType::NotEqual: return "NotEqual";
        case TokenType::Less: return "Less";
        case TokenType::Greater: return "Greater";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::GreaterEqual: return "GreaterEqual";
        case TokenType::And: return "And";
        case TokenType::Or: return "Or";
        case TokenType::Not: return "Not";

        // Delimiters
        case TokenType::Colon: return "Colon";
        case TokenType::Comma: return "Comma";
        case TokenType::Dot: return "Dot";
        case TokenType::Semicolon: return "Semicolon";

        // Brackets
        case TokenType::LeftParen: return "LeftParen";
        case TokenType::RightParen: return "RightParen";
        case TokenType::LeftBracket: return "LeftBracket";
        case TokenType::RightBracket: return "RightBracket";
        case TokenType::LeftBrace: return "LeftBrace";
        case TokenType::RightBrace: return "RightBrace";

        // Special
        case TokenType::Indent: return "Indent";
        case TokenType::Dedent: return "Dedent";
        case TokenType::Newline: return "Newline";
        case TokenType::EndOfInput: return "EndOfInput";

        default: return "Unknown";
    }
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Type: " << tokenTypeToString(token.type)
       << ", Text: \"" << token.text << "\"";
    if (std::holds_alternative<double>(token.value)) {
        os << ", Value: " << std::get<double>(token.value);
    } else if (std::holds_alternative<std::string>(token.value)) {
        os << ", Value: " << std::get<std::string>(token.value);
    }
    return os;
}
