#pragma once

#include "Defs.hpp"
#include "InstructionSet.hpp"

namespace LemonVM {

struct Token {
    std::string str{};
    std::size_t line{0}; 
};

using Tokens = std::vector<Token>;

void print_tokens(Tokens tokens) {
    std::size_t i = 0;
    for (auto tok: tokens)
        std::cout << i++ << ") =" << tok.str << "=\n";
    std::cout << std::endl;
}

void trim_left(std::string::iterator& curr, const std::string::iterator eof) {
    auto is_whitespace = [](char c) { return (c == ' ' || c == '\t'); };
    auto is_comment    = [](char c) { return (c == '#'); };
    auto is_endline    = [](char c) { return (c == '\n'); };

    while (curr != eof) {
        if (is_whitespace(*curr)) {
            curr++;
        }
        else if (is_comment(*curr)) {
            while (curr != eof && !is_endline(*curr))
                curr++;
            curr++;
            curr++;
        }
        else {
            break;
        }
    }
}

Token extract_token(std::string::iterator start, const std::string::iterator eof) {
    auto is_whitespace = [](char c) { return (c == ' ' || c == '\t'); };
    auto is_comment    = [](char c) { return (c == '#'); };
    auto is_endline    = [](char c) { return (c == '\n'); };

    std::string::iterator end = start;
    while (end != eof) {
        if (is_whitespace(*end) || is_endline(*end) || is_comment(*end))
            break;
        end++;
    }
    return Token{std::string(start, end), 0};
}

Tokens tokenize(std::string prg) {
    Tokens tokens{};
    std::string::iterator curr = prg.begin();
    std::string::iterator eof = prg.end();
    while (curr != eof) {
        trim_left(curr, eof);
        if (curr == eof)
            return tokens;
        tokens.emplace_back(extract_token(curr, eof));
        curr += tokens.back().str.size() + 1;
    }
    return tokens;
}

InstructionSet assemble(const Tokens& tokens) {
    InstructionSet iset{};
    std::size_t i = 0;
    while (i < tokens.size()) {
        Instruction ins;
        ins.opcode = get_opcode(tokens[i].str);
        if (ins.opcode == OPCODE_PUT || ins.opcode == OPCODE_DUP) {
            i++;
            assert(!is_opcode(tokens[i].str));
            ins.arg1 = std::stoi(tokens[i].str);
        }
        else if (ins.opcode == OPCODE_LABEL || ins.opcode == OPCODE_JMPIF ||
            ins.opcode == OPCODE_CALL) {
            i++;
            assert(!is_opcode(tokens[i].str));
            ins.label = tokens[i].str;
        }
        iset.push_back(ins);
        i++;
    }
    return iset;
}

}//ns
