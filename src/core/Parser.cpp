#include "core/Parser.h"
#include <stdexcept>
#include <iostream>
#include <unordered_map>

// --- Pratt Parser Parse Rules Table ---
// Precedence levels (low -> high):
// 1: or
// 2: and
// 3: equality (==, !=)
// 4: comparisons (<, <=, >, >=)
// 5: additive (+, -)
// 6: multiplicative (*, /, %)
// 7: unary (right-binding power used inside parseUnary for '-' only)
// 8: power (right-associative)
// 9: postfix (call, index, member)
const std::unordered_map<TokenType, Parser::ParseRule> Parser::s_parseRules = {
    // Literals
    {TokenType::Number, {&Parser::parseNumber, nullptr, 0}},
    {TokenType::String, {&Parser::parseString, nullptr, 0}},
    {TokenType::Identifier, {&Parser::parseVariable, nullptr, 0}},

    {TokenType::True, {&Parser::parseBoolean, nullptr, 0}},
    {TokenType::False, {&Parser::parseBoolean, nullptr, 0}},
    {TokenType::None, {&Parser::parseNone, nullptr, 0}},

    // Grouping and function calls (highest postfix precedence)
    {TokenType::LeftParen, {&Parser::parseGrouping, &Parser::parseCall, 9}},
    {TokenType::LeftBracket, {&Parser::parseList, &Parser::parseIndex, 9}},

    // Member access (postfix)
    {TokenType::Dot, {nullptr, &Parser::parseMemberAccess, 9}},

    // Unary operators (prefix only here; precedence handled in parseUnary)
    // Note: '-' also has an infix rule below; combine into a single entry
    {TokenType::Minus, {&Parser::parseUnary, &Parser::parseBinary, 5}}, // unary '-', binary '-' (additive)
    {TokenType::Not, {&Parser::parseUnary, nullptr, 0}},

    // Binary operators
    {TokenType::Power, {nullptr, &Parser::parseBinary, 8}},      // ** (right associative)
    {TokenType::Star, {nullptr, &Parser::parseBinary, 6}},       // *
    {TokenType::Slash, {nullptr, &Parser::parseBinary, 6}},      // /
    {TokenType::Percent, {nullptr, &Parser::parseBinary, 6}},    // %
    {TokenType::Plus, {nullptr, &Parser::parseBinary, 5}},       // +

    // Comparison operators
    {TokenType::Less, {nullptr, &Parser::parseBinary, 4}},       // <
    {TokenType::LessEqual, {nullptr, &Parser::parseBinary, 4}},  // <=
    {TokenType::Greater, {nullptr, &Parser::parseBinary, 4}},    // >
    {TokenType::GreaterEqual, {nullptr, &Parser::parseBinary, 4}}, // >=
    {TokenType::Equal, {nullptr, &Parser::parseBinary, 3}},      // ==
    {TokenType::NotEqual, {nullptr, &Parser::parseBinary, 3}},   // !=

    // Logical operators
    {TokenType::And, {nullptr, &Parser::parseBinary, 2}},        // and
    {TokenType::Or, {nullptr, &Parser::parseBinary, 1}},         // or
};

// --- Parser implementation ---
Parser::Parser(const std::vector<Token>& tokens)
    : m_tokens(tokens), m_current(0) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        skipNewlines();
        auto stmt = parseStatement();
        if (stmt) statements.push_back(std::move(stmt));
        if (match(TokenType::Newline) || match(TokenType::Semicolon)) {}
    }
    return statements;
}

// --- Statement parsing ---
std::unique_ptr<Stmt> Parser::parseStatement() {
    if (match(TokenType::Def)) return parseFunctionDef();
    if (match(TokenType::If)) return parseIf();
    if (match(TokenType::While)) return parseWhile();
    if (match(TokenType::For)) return parseFor();
    if (match(TokenType::Break)) return std::make_unique<BreakStmt>();
    if (match(TokenType::Continue)) return std::make_unique<ContinueStmt>();
    if (match(TokenType::Return)) return parseReturn();
    
    // Check for assignment: identifier = expression
    if (check(TokenType::Identifier) && m_current + 1 < m_tokens.size() && m_tokens[m_current + 1].type == TokenType::Assign) {
        return parseAssignment();
    }
    
    return parseExpressionStatement();
}

std::unique_ptr<Stmt> Parser::parseIf() {
    auto condition = parseExpression();
    if (!match(TokenType::Colon)) throw std::runtime_error("Expected ':' after if condition");
    skipNewlines();
    if (!match(TokenType::Indent)) throw std::runtime_error("Expected indentation after ':'");
    auto thenBlock = parseBlock();
    std::vector<std::unique_ptr<Stmt>> thenBranch;
    if (auto block = dynamic_cast<BlockStmt*>(thenBlock.get())) {
        thenBranch = std::move(block->statements);
    }
    std::vector<std::unique_ptr<Stmt>> elseBranch;
    if (match(TokenType::Else)) {
        if (!match(TokenType::Colon)) throw std::runtime_error("Expected ':' after else");
        skipNewlines();
        if (!match(TokenType::Indent)) throw std::runtime_error("Expected indentation after else ':'");
        auto elseBlock = parseBlock();
        if (auto block = dynamic_cast<BlockStmt*>(elseBlock.get())) {
            elseBranch = std::move(block->statements);
        }
    }
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::parseWhile() {
    auto condition = parseExpression();
    if (!match(TokenType::Colon)) throw std::runtime_error("Expected ':' after while condition");
    skipNewlines();
    if (!match(TokenType::Indent)) throw std::runtime_error("Expected indentation after ':'");
    auto bodyBlock = parseBlock();
    std::vector<std::unique_ptr<Stmt>> body;
    if (auto block = dynamic_cast<BlockStmt*>(bodyBlock.get())) {
        body = std::move(block->statements);
    }
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseFor() {
    if (!match(TokenType::Identifier)) throw std::runtime_error("Expected variable name in for loop");
    std::string var = previous().text;
    if (!match(TokenType::In)) throw std::runtime_error("Expected 'in' in for loop");
    auto iterable = parseExpression();
    if (!match(TokenType::Colon)) throw std::runtime_error("Expected ':' after for loop");
    skipNewlines();
    if (!match(TokenType::Indent)) throw std::runtime_error("Expected indentation after ':'");
    auto bodyBlock = parseBlock();
    std::vector<std::unique_ptr<Stmt>> body;
    if (auto block = dynamic_cast<BlockStmt*>(bodyBlock.get())) {
        body = std::move(block->statements);
    }
    return std::make_unique<ForStmt>(var, std::move(iterable), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseBlock() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd() && !check(TokenType::Dedent)) {
        skipNewlines();
        if (check(TokenType::Dedent)) break;
        auto stmt = parseStatement();
        if (stmt) statements.push_back(std::move(stmt));
    }
    if (match(TokenType::Dedent)) {}
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::parseExpressionStatement() {
    auto expr = parseExpression();
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

// --- Pratt Parser Expression Parsing ---
std::unique_ptr<Expr> Parser::parseExpression(int precedence) {
    auto left = parsePrecedence();
    
    while (precedence <= getPrecedence(peek().type)) {
        auto rule = s_parseRules.find(peek().type);
        if (rule == s_parseRules.end() || !rule->second.infix) {
            break;
        }
        
        advance();
        left = (this->*rule->second.infix)(std::move(left));
    }
    
    return left;
}

std::unique_ptr<Expr> Parser::parsePrecedence() {
    auto rule = s_parseRules.find(peek().type);
    if (rule == s_parseRules.end() || !rule->second.prefix) {
        throw std::runtime_error("Unexpected token: " + peek().text);
    }
    
    advance();
    return (this->*rule->second.prefix)();
}

// --- Prefix Parsers ---
std::unique_ptr<Expr> Parser::parseNumber() {
    return std::make_unique<NumberExpr>(std::get<double>(previous().value));
}

std::unique_ptr<Expr> Parser::parseString() {
    return std::make_unique<StringExpr>(std::get<std::string>(previous().value));
}

std::unique_ptr<Expr> Parser::parseVariable() {
    return std::make_unique<VariableExpr>(previous().text);
}

std::unique_ptr<Expr> Parser::parseBoolean() {
    return std::make_unique<NumberExpr>(previous().type == TokenType::True ? 1.0 : 0.0);
}

std::unique_ptr<Expr> Parser::parseNone() {
    return std::make_unique<NumberExpr>(0.0); // None is represented as 0
}

std::unique_ptr<Expr> Parser::parseGrouping() {
    auto expr = parseExpression();
    if (!match(TokenType::RightParen)) {
        throw std::runtime_error("Expected ')' after expression");
    }
    return expr;
}

std::unique_ptr<Expr> Parser::parseList() {
    std::vector<std::unique_ptr<Expr>> elements;
    
    if (!check(TokenType::RightBracket)) {
        do {
            elements.push_back(parseExpression());
        } while (match(TokenType::Comma));
    }
    
    if (!match(TokenType::RightBracket)) {
        throw std::runtime_error("Expected ']' after list elements");
    }
    
    return std::make_unique<ListExpr>(std::move(elements));
}

std::unique_ptr<Expr> Parser::parseUnary() {
    std::string op = previous().text;
    // Use different binding powers for logical vs arithmetic unary:
    // - For "not": lower than equality to allow "not a == b" -> not (a == b)
    // - For numeric negation: higher than multiplicative, lower than power and postfix
    int operandBindingPower = (op == "not") ? 2 : 7;
    auto operand = parseExpression(operandBindingPower);
    return std::make_unique<UnaryExpr>(op, std::move(operand));
}

// --- Infix Parsers ---
std::unique_ptr<Expr> Parser::parseBinary(std::unique_ptr<Expr> left) {
    std::string op = previous().text;
    int precedence = getPrecedence(previous().type);
    
    int nextPrecedence;
    if (op == "**") {
        // Power is right associative, so use same precedence
        nextPrecedence = precedence;
    } else {
        // Other operators are left associative, so use higher precedence
        nextPrecedence = precedence + 1;
    }
    
    auto right = parseExpression(nextPrecedence);
    return std::make_unique<BinaryExpr>(std::move(left), op, std::move(right));
}

std::unique_ptr<Expr> Parser::parseCall(std::unique_ptr<Expr> left) {
    auto args = parseArguments();
    if (!match(TokenType::RightParen)) {
        throw std::runtime_error("Expected ')' after function arguments");
    }
    return std::make_unique<CallExpr>(std::move(left), std::move(args));
}

std::unique_ptr<Expr> Parser::parseIndex(std::unique_ptr<Expr> left) {
    auto index = parseExpression();
    if (!match(TokenType::RightBracket)) {
        throw std::runtime_error("Expected ']' after index expression");
    }
    return std::make_unique<IndexExpr>(std::move(left), std::move(index));
}

std::unique_ptr<Expr> Parser::parseMemberAccess(std::unique_ptr<Expr> left) {
    if (!match(TokenType::Identifier)) {
        throw std::runtime_error("Expected identifier after '.'");
    }
    std::string member = previous().text;
    return std::make_unique<MemberAccessExpr>(std::move(left), member);
}

std::unique_ptr<Stmt> Parser::parseAssignment() {
    if (!match(TokenType::Identifier)) {
        throw std::runtime_error("Expected identifier for assignment");
    }
    std::string name = previous().text;
    
    if (!match(TokenType::Assign)) {
        throw std::runtime_error("Expected '=' after identifier");
    }
    
    auto value = parseExpression();
    return std::make_unique<AssignStmt>(name, std::move(value));
}

std::vector<std::unique_ptr<Expr>> Parser::parseArguments() {
    std::vector<std::unique_ptr<Expr>> args;
    if (check(TokenType::RightParen)) return args;
    do {
        args.push_back(parseExpression());
    } while (match(TokenType::Comma));
    return args;
}

std::unique_ptr<Stmt> Parser::parseFunctionDef() {
    if (!match(TokenType::Identifier)) {
        throw std::runtime_error("Expected function name after 'def'");
    }
    std::string name = previous().text;
    
    if (!match(TokenType::LeftParen)) {
        throw std::runtime_error("Expected '(' after function name");
    }
    
    std::vector<std::string> parameters;
    if (!check(TokenType::RightParen)) {
        do {
            if (!match(TokenType::Identifier)) {
                throw std::runtime_error("Expected parameter name");
            }
            parameters.push_back(previous().text);
        } while (match(TokenType::Comma));
    }
    
    if (!match(TokenType::RightParen)) {
        throw std::runtime_error("Expected ')' after function parameters");
    }
    
    if (!match(TokenType::Colon)) {
        throw std::runtime_error("Expected ':' after function parameters");
    }
    
    skipNewlines();
    
    if (!match(TokenType::Indent)) {
        throw std::runtime_error("Expected indentation after function definition");
    }
    
    auto bodyBlock = parseBlock();
    std::vector<std::unique_ptr<Stmt>> body;
    if (auto block = dynamic_cast<BlockStmt*>(bodyBlock.get())) {
        body = std::move(block->statements);
    }
    
    return std::make_unique<FunctionDefStmt>(name, std::move(parameters), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseReturn() {
    std::unique_ptr<Expr> value = nullptr;
    if (!check(TokenType::Newline)) {
        value = parseExpression();
    }
    return std::make_unique<ReturnStmt>(std::move(value));
}

// --- Helpers ---
int Parser::getPrecedence(TokenType type) const {
    auto it = s_parseRules.find(type);
    return it != s_parseRules.end() ? it->second.precedence : 0;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

const Token& Parser::advance() {
    if (!isAtEnd()) m_current++;
    return previous();
}

const Token& Parser::peek() const {
    return m_tokens[m_current];
}

const Token& Parser::previous() const {
    return m_tokens[m_current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfInput;
}

void Parser::skipNewlines() {
    while (match(TokenType::Newline)) {}
}
