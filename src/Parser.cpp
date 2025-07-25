#include "Parser.h"
#include <stdexcept>
#include <iostream>

// --- Operator precedence table ---
static int getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::Or: return 1;
        case TokenType::And: return 2;
        case TokenType::Equal:
        case TokenType::NotEqual: return 3;
        case TokenType::Less:
        case TokenType::LessEqual:
        case TokenType::Greater:
        case TokenType::GreaterEqual: return 4;
        case TokenType::Plus:
        case TokenType::Minus: return 5;
        case TokenType::Star:
        case TokenType::Slash:
        case TokenType::Percent: return 6;
        case TokenType::Power: return 7;
        default: return 0;
    }
}

// --- Parser implementation ---
Parser::Parser(const std::vector<Token>& tokens)
    : m_tokens(tokens), m_current(0) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        if (check(TokenType::Newline)) { advance(); continue; }
        // std::cout << "[parse] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
        auto stmt = parseStatement();
        if (stmt) statements.push_back(std::move(stmt));
        // Consume statement separator
        if (match(TokenType::Newline) || match(TokenType::Semicolon)) {}
    }
    return statements;
}

// --- Statement parsing ---
std::unique_ptr<Stmt> Parser::parseStatement() {
    // std::cout << "[parseStatement] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    if (match(TokenType::If)) return parseIf();
    if (match(TokenType::While)) return parseWhile();
    if (match(TokenType::For)) return parseFor();
    if (check(TokenType::Identifier) && (m_current + 1 < m_tokens.size()) && m_tokens[m_current+1].type == TokenType::Assign) {
        return parseAssignment();
    }
    if (match(TokenType::Break)) return std::make_unique<BreakStmt>();
    if (match(TokenType::Continue)) return std::make_unique<ContinueStmt>();
    return parseExpressionStatement();
}

std::unique_ptr<Stmt> Parser::parseAssignment() {
    // identifier = expr
    // std::cout << "[parseAssignment] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    std::string name = advance().text;
    if (!match(TokenType::Assign)) throw std::runtime_error("Expected '=' in assignment");
    auto value = parseExpression();
    return std::make_unique<AssignStmt>(name, std::move(value));
}

std::unique_ptr<Stmt> Parser::parseIf() {
    // std::cout << "[parseIf] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    auto condition = parseExpression();
    if (!match(TokenType::Colon)) throw std::runtime_error("Expected ':' after if condition");
    while (match(TokenType::Newline) || match(TokenType::Semicolon)) {}
    // Require INDENT before block
    auto thenBlock = parseBlock();
    std::vector<std::unique_ptr<Stmt>> thenBranch;
    if (auto block = dynamic_cast<BlockStmt*>(thenBlock.get())) {
        thenBranch = std::move(block->statements);
    }
    std::vector<std::unique_ptr<Stmt>> elseBranch;
    if (match(TokenType::Else)) {
        while (match(TokenType::Newline) || match(TokenType::Semicolon)) {}
        if (!match(TokenType::Colon)) throw std::runtime_error("Expected ':' after else");
        while (match(TokenType::Newline) || match(TokenType::Semicolon)) {}
        auto elseBlock = parseBlock();
        if (auto block = dynamic_cast<BlockStmt*>(elseBlock.get())) {
            elseBranch = std::move(block->statements);
        }
    }
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::parseWhile() {
    // std::cout << "[parseWhile] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    auto condition = parseExpression();
    while (match(TokenType::Newline) || match(TokenType::Semicolon)) {}
    if (!match(TokenType::Colon)) throw std::runtime_error("Expected ':' after while condition");
    while (match(TokenType::Newline) || match(TokenType::Semicolon)) {}
    // Require INDENT before block
    auto bodyBlock = parseBlock();
    std::vector<std::unique_ptr<Stmt>> body;
    if (auto block = dynamic_cast<BlockStmt*>(bodyBlock.get())) {
        body = std::move(block->statements);
    }
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseFor() {
    // std::cout << "[parseFor] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    if (!match(TokenType::Identifier)) throw std::runtime_error("Expected variable name in for");
    std::string var = previous().text;
    if (!match(TokenType::In)) throw std::runtime_error("Expected 'in' in for");
    auto iterable = parseExpression();
    while (match(TokenType::Newline) || match(TokenType::Semicolon)) {}
    if (!match(TokenType::Colon)) throw std::runtime_error("Expected ':' after for iterable");
    while (match(TokenType::Newline) || match(TokenType::Semicolon)) {}
    // Require INDENT before block
    auto bodyBlock = parseBlock();
    std::vector<std::unique_ptr<Stmt>> body;
    if (auto block = dynamic_cast<BlockStmt*>(bodyBlock.get())) {
        body = std::move(block->statements);
    }
    return std::make_unique<ForStmt>(var, std::move(iterable), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseBlock() {
    // std::cout << "[parseBlock] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    if (!match(TokenType::Indent)) {
        throw std::runtime_error("Expected INDENT at start of block");
    }
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd() && !check(TokenType::Dedent) && !check(TokenType::EndOfInput)) {
        // Skip newlines/semicolons between statements
        while (match(TokenType::Newline) || match(TokenType::Semicolon)) {}
        if (isAtEnd() || check(TokenType::Dedent) || check(TokenType::EndOfInput)) break;
        auto stmt = parseStatement();
        if (stmt) statements.push_back(std::move(stmt));
    }
    if (!match(TokenType::Dedent)) {
        throw std::runtime_error("Expected DEDENT at end of block");
    }
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::parseExpressionStatement() {
    // std::cout << "[parseExpressionStatement] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    auto expr = parseExpression();
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

// --- Expression parsing ---
std::unique_ptr<Expr> Parser::parseExpression() {
    // std::cout << "[parseExpression] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    return parseAssignmentExpr();
}

std::unique_ptr<Expr> Parser::parseAssignmentExpr() {
    // std::cout << "[parseAssignmentExpr] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    auto expr = parseBinaryExpr();
    // Assignment handled as statement, not as expression for now
    return expr;
}

std::unique_ptr<Expr> Parser::parseBinaryExpr(int precedence) {
    // std::cout << "[parseBinaryExpr] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    auto left = parseUnaryExpr();
    while (true) {
        TokenType opType = peek().type;
        // Stop if at a statement separator or end of expression
        if (opType == TokenType::Newline || opType == TokenType::Semicolon || opType == TokenType::Colon ||
            opType == TokenType::RightParen || opType == TokenType::EndOfInput || opType == TokenType::Comma) {
            break;
        }
        int opPrec = getPrecedence(opType);
        if (opPrec < precedence) break;
        std::string opText = peek().text;
        advance();
        auto right = parseBinaryExpr(opPrec + 1);
        left = std::make_unique<BinaryExpr>(std::move(left), opText, std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::parseUnaryExpr() {
    // std::cout << "[parseUnaryExpr] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    if (match(TokenType::Minus) || match(TokenType::Not)) {
        std::string op = previous().text;
        auto operand = parseUnaryExpr();
        return std::make_unique<UnaryExpr>(op, std::move(operand));
    }
    return parsePrimary();
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    // std::cout << "[parsePrimary] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    if (match(TokenType::Number)) {
        return std::make_unique<NumberExpr>(std::get<double>(previous().value));
    }
    if (match(TokenType::String)) {
        return std::make_unique<StringExpr>(std::get<std::string>(previous().value));
    }
    if (match(TokenType::Identifier)) {
        std::string name = previous().text;
        // Function call or variable
        if (match(TokenType::LeftParen)) {
            auto args = parseArguments();
            if (!match(TokenType::RightParen)) throw std::runtime_error("Expected ')' after function arguments");
            return std::make_unique<CallExpr>(std::make_unique<VariableExpr>(name), std::move(args));
        }
        return std::make_unique<VariableExpr>(name);
    }
    if (match(TokenType::LeftParen)) {
        auto expr = parseExpression();
        if (!match(TokenType::RightParen)) {
            throw std::runtime_error("Expected ')' after expression");
        }
        return expr;
    }
    throw std::runtime_error("Unexpected token in expression: " + peek().text + tokenTypeToString(peek().type));
}

std::vector<std::unique_ptr<Expr>> Parser::parseArguments() {
    // std::cout << "[parseArguments] At token: '" << peek().text << "' (" << tokenTypeToString(peek().type) << ")\n";
    std::vector<std::unique_ptr<Expr>> args;
    if (check(TokenType::RightParen)) return args;
    do {
        args.push_back(parseExpression());
    } while (match(TokenType::Comma));
    return args;
}

// --- Helpers (already implemented) ---
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

void Parser::synchronize() {
    // TODO: Error recovery (skip tokens until a statement boundary)
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::Semicolon) return;
        switch (peek().type) {
            case TokenType::If:
            case TokenType::For:
            case TokenType::While:
            case TokenType::Def:
            case TokenType::Return:
                return;
            default:
                break;
        }
        advance();
    }
}
