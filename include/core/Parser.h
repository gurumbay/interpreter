#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <memory>
#include <unordered_map>
#include "core/AST.h"
#include "core/Token.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);

    std::vector<std::unique_ptr<Stmt>> parse();

private:
    // Pratt parser function types
    using PrefixParseFn = std::unique_ptr<Expr> (Parser::*)();
    using InfixParseFn = std::unique_ptr<Expr> (Parser::*)(std::unique_ptr<Expr>);
    
    struct ParseRule {
        PrefixParseFn prefix;
        InfixParseFn infix;
        int precedence;
    };
    
    static const std::unordered_map<TokenType, ParseRule> s_parseRules;
    
    const std::vector<Token>& m_tokens;
    size_t m_current;

    // Statement parsing
    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<Stmt> parseIf();
    std::unique_ptr<Stmt> parseWhile();
    std::unique_ptr<Stmt> parseFor();
    std::unique_ptr<Stmt> parseBlock();
    std::unique_ptr<Stmt> parseExpressionStatement();
    std::unique_ptr<Stmt> parseAssignment();

    // Expression parsing with Pratt parser
    std::unique_ptr<Expr> parseExpression(int precedence = 0);
    std::unique_ptr<Expr> parsePrecedence();
    
    // Prefix parsers
    std::unique_ptr<Expr> parseNumber();
    std::unique_ptr<Expr> parseString();
    std::unique_ptr<Expr> parseVariable();
    std::unique_ptr<Expr> parseGrouping();
    std::unique_ptr<Expr> parseList();
    std::unique_ptr<Expr> parseUnary();
    
    // Infix parsers
    std::unique_ptr<Expr> parseBinary(std::unique_ptr<Expr> left);
    std::unique_ptr<Expr> parseCall(std::unique_ptr<Expr> left);
    std::unique_ptr<Expr> parseIndex(std::unique_ptr<Expr> left);
    std::unique_ptr<Expr> parseMemberAccess(std::unique_ptr<Expr> left);
    
    std::vector<std::unique_ptr<Expr>> parseArguments();

    // Helpers
    bool match(TokenType type);
    bool check(TokenType type) const;
    const Token& advance();
    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    int getPrecedence(TokenType type) const;
};

#endif // PARSER_H
