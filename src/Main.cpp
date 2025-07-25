#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "core/Interpreter.h"
#include "core/Parser.h"
#include "core/Tokenizer.h"
#include "core/Token.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename> [--timing]" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    bool timing = false;
    for (int i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "--timing") timing = true;
    }

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open " << filename << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();

    try {
        using clock = std::chrono::high_resolution_clock;
        auto t0 = clock::now();

        // Tokenize
        Tokenizer tokenizer(code);
        std::vector<Token> tokens;
        while (true) {
            Token token = tokenizer.nextToken();
            tokens.push_back(token);
            if (token.type == TokenType::EndOfInput) break;
        }
        
        // Parse
        Parser parser(tokens);
        auto statements = parser.parse();
        auto t1 = clock::now();

        // Interpret
        Interpreter interpreter;
        interpreter.run(statements);
        auto t2 = clock::now();

        if (timing) {
            auto compile_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
            auto interpret_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
            std::cout << "\n[Compilation time]: " << compile_ms << " ms\n";
            std::cout << "[Interpretation time]: " << interpret_ms << " ms\n";
        }
        std::cout << "\n[Program finished successfully]" << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}