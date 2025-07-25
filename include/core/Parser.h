#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <memory>
#include "core/AST.h"
#include "core/Token.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);

    std::vector<std::unique_ptr<Stmt>> parse();

private:
    const std::vector<Token>& m_tokens;
    size_t m_current;

    // Statement parsing
    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<Stmt> parseIf();
    std::unique_ptr<Stmt> parseWhile();
    std::unique_ptr<Stmt> parseFor();
    std::unique_ptr<Stmt> parseBlock();
    std::unique_ptr<Stmt> parseAssignment();
    std::unique_ptr<Stmt> parseExpressionStatement();

    // Expression parsing
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseAssignmentExpr();
    std::unique_ptr<Expr> parseBinaryExpr(int precedence = 0);
    std::unique_ptr<Expr> parseUnaryExpr();
    std::unique_ptr<Expr> parsePrimary();
    std::vector<std::unique_ptr<Expr>> parseArguments();

    // Helpers
    bool match(TokenType type);
    bool check(TokenType type) const;
    const Token& advance();
    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    void synchronize();
};

#endif // PARSER_H
