#include "Tokenizer.h"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

static const std::unordered_map<std::string, TokenType> keywords = {
    {"for", TokenType::For},
    {"while", TokenType::While},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"in", TokenType::In},
    {"def", TokenType::Def},
    {"return", TokenType::Return},
    {"break", TokenType::Break},
    {"continue", TokenType::Continue},
    {"True", TokenType::True},
    {"False", TokenType::False},
    {"None", TokenType::None},
    {"and", TokenType::And},
    {"or", TokenType::Or},
    {"not", TokenType::Not},
};

Tokenizer::Tokenizer(const std::string& input)
    : m_input(input), m_pos(0), m_line(1), m_col(1), m_peekedToken(TokenType::EndOfInput, ""), m_hasPeeked(false), m_atLineStart(true) {
    m_indentStack.push_back(0);
}

bool Tokenizer::isAtEnd() const {
    return m_pos >= m_input.size();
}

char Tokenizer::peekChar() const {
    if (isAtEnd()) return '\0';
    return m_input[m_pos];
}

char Tokenizer::getChar() {
    if (isAtEnd()) return '\0';
    char c = m_input[m_pos++];
    if (c == '\n') {
        m_line++;
        m_col = 1;
        m_atLineStart = true;
    } else {
        m_col++;
    }
    return c;
}

bool Tokenizer::matchChar(char expected) {
    if (peekChar() == expected) {
        getChar();
        return true;
    }
    return false;
}

// Only skip spaces, tabs, and carriage returns (not newlines)
void Tokenizer::skipWhitespace() {
    while (!isAtEnd() && (peekChar() == ' ' || peekChar() == '\t' || peekChar() == '\r')) {
        getChar();
    }
}

void Tokenizer::processIndentation() {
    // Count leading spaces/tabs
    size_t start = m_pos;
    int indent = 0;
    while (!isAtEnd()) {
        char c = peekChar();
        if (c == ' ') { indent++; getChar(); }
        else if (c == '\t') { indent += 4; getChar(); } // treat tab as 4 spaces
        else break;
    }
    // If line is empty or comment, don't emit INDENT/DEDENT
    if (peekChar() == '\n' || isAtEnd()) return;
    int prevIndent = m_indentStack.back();
    if (indent > prevIndent) {
        m_indentStack.push_back(indent);
        m_pendingIndents.push(Token(TokenType::Indent, "<INDENT>"));
    } else if (indent < prevIndent) {
        while (indent < m_indentStack.back()) {
            m_indentStack.pop_back();
            m_pendingIndents.push(Token(TokenType::Dedent, "<DEDENT>"));
        }
        if (indent != m_indentStack.back()) {
            throw std::runtime_error("Inconsistent indentation");
        }
    }
}

Token Tokenizer::nextToken() {
    if (m_hasPeeked) {
        m_hasPeeked = false;
        return m_peekedToken;
    }
    // Emit any pending INDENT/DEDENT tokens first
    if (!m_pendingIndents.empty()) {
        Token t = m_pendingIndents.front();
        m_pendingIndents.pop();
        return t;
    }
    // At start of line, process indentation
    if (m_atLineStart && !isAtEnd()) {
        processIndentation();
        m_atLineStart = false;
        if (!m_pendingIndents.empty()) {
            Token t = m_pendingIndents.front();
            m_pendingIndents.pop();
            return t;
        }
    }
    skipWhitespace();
    if (isAtEnd()) {
        // At EOF, emit DEDENTs for any remaining indentation
        while (m_indentStack.size() > 1) {
            m_indentStack.pop_back();
            m_pendingIndents.push(Token(TokenType::Dedent, "<DEDENT>"));
        }
        if (!m_pendingIndents.empty()) {
            Token t = m_pendingIndents.front();
            m_pendingIndents.pop();
            return t;
        }
        return Token(TokenType::EndOfInput, "");
    }
    char c = peekChar();
    if (c == '\n') {
        getChar();
        return Token(TokenType::Newline, "\\n");
    }
    if (c == '#') {
        while (!isAtEnd() && peekChar() != '\n') getChar();
        return nextToken();
    }
    if (std::isdigit(c) || (c == '.' && m_pos + 1 < m_input.size() && std::isdigit(m_input[m_pos + 1]))) {
        return lexNumber();
    }
    if (std::isalpha(c) || c == '_') {
        return lexIdentifierOrKeyword();
    }
    if (c == '"' || c == '\'') {
        return lexString();
    }
    return lexOperatorOrDelimiter();
}

Token Tokenizer::peekToken() {
    if (!m_hasPeeked) {
        m_peekedToken = nextToken();
        m_hasPeeked = true;
    }
    return m_peekedToken;
}

Token Tokenizer::lexNumber() {
    size_t start = m_pos;
    bool hasDot = false;
    while (!isAtEnd() && (std::isdigit(peekChar()) || peekChar() == '.')) {
        if (peekChar() == '.') {
            if (hasDot) break;
            hasDot = true;
        }
        getChar();
    }
    std::string numStr = m_input.substr(start, m_pos - start);
    double value = std::stod(numStr);
    return Token(TokenType::Number, numStr, value);
}

Token Tokenizer::lexIdentifierOrKeyword() {
    size_t start = m_pos;
    while (!isAtEnd() && (std::isalnum(peekChar()) || peekChar() == '_')) {
        getChar();
    }
    std::string ident = m_input.substr(start, m_pos - start);
    auto it = keywords.find(ident);
    if (it != keywords.end()) {
        return Token(it->second, ident);
    }
    return Token(TokenType::Identifier, ident, ident);
}

Token Tokenizer::lexString() {
    char quote = getChar(); // consume opening quote
    std::string value;
    while (!isAtEnd() && peekChar() != quote) {
        if (peekChar() == '\\') {
            getChar(); // consume backslash
            if (!isAtEnd()) {
                char next = getChar();
                switch (next) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    case '\\': value += '\\'; break;
                    default: value += next; break;
                }
            } else {
                value += '\\';
            }
        } else {
            value += getChar();
        }
    }
    if (peekChar() == quote) getChar(); // consume closing quote
    return Token(TokenType::String, value, value);
}

Token Tokenizer::lexOperatorOrDelimiter() {
    char c = getChar();
    // Two-char operators
    if (!isAtEnd()) {
        char next = peekChar();
        std::string two = std::string(1, c) + next;
        if (two == "+=" || two == "-=" || two == "==" || two == "!=" || two == "<=" || two == ">=" || two == "**") {
            getChar();
            if (two == "+=") return Token(TokenType::PlusAssign, two);
            if (two == "-=") return Token(TokenType::MinusAssign, two);
            if (two == "==") return Token(TokenType::Equal, two);
            if (two == "!=") return Token(TokenType::NotEqual, two);
            if (two == "<=") return Token(TokenType::LessEqual, two);
            if (two == ">=") return Token(TokenType::GreaterEqual, two);
            if (two == "**") return Token(TokenType::Power, two);
        }
    }
    // Single-char tokens
    switch (c) {
        case '+': return Token(TokenType::Plus, "+");
        case '-': return Token(TokenType::Minus, "-");
        case '*': return Token(TokenType::Star, "*");
        case '/': return Token(TokenType::Slash, "/");
        case '%': return Token(TokenType::Percent, "%");
        case '=': return Token(TokenType::Assign, "=");
        case '<': return Token(TokenType::Less, "<");
        case '>': return Token(TokenType::Greater, ">");
        case ':': return Token(TokenType::Colon, ":");
        case ',': return Token(TokenType::Comma, ",");
        case '.': return Token(TokenType::Dot, ".");
        case ';': return Token(TokenType::Semicolon, ";");
        case '(': return Token(TokenType::LeftParen, "(");
        case ')': return Token(TokenType::RightParen, ")");
        case '[': return Token(TokenType::LeftBracket, "[");
        case ']': return Token(TokenType::RightBracket, "]");
        case '{': return Token(TokenType::LeftBrace, "{");
        case '}': return Token(TokenType::RightBrace, "}");
        default:
            throw std::runtime_error(std::string("Unknown character: ") + c);
    }
}
