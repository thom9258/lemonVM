#pragma once

#include "InstructionSet.hpp"
#include "Lexer.hpp"

namespace LemonVM {

using LabelMap = std::map<std::string, std::size_t>;
using MemoryStack = std::vector<Arg>;
using ReturnStack = std::vector<std::size_t>;
using Scope = std::map<std::string, Arg>;
using ScopeStack = std::vector<Scope>;

enum class State {
    ERR,
    OK,
    EXIT,
};

struct VM {
    std::size_t ip{0};
    Arg a{0};
    Arg b{0};

    MemoryStack stack{};

    ReturnStack returnstack{};

    ScopeStack scopestack{};
};

std::string stack_dump(VM& vm, int width=80) {
    std::stringstream ss{};
    ss << "== VM Stack Dump Start ==";
    for (auto it = vm.stack.cbegin(); it != vm.stack.cend(); it++) {
        if ((std::distance(vm.stack.cbegin(), it) % width) == 0)
            ss << "\n";
        ss << *it;
    }
    ss << "\n== VM Stack Dump End ==\n";
    return ss.str();
}

State ins_eval(VM& vm, const LabelMap& labels, const Instruction& ins)
{
    switch (ins.opcode) {

    case OPCODE_COUNT:
    case OPCODE_INVALID:
    case OPCODE_EXIT:
        return State::EXIT;

    case OPCODE_LABEL: 
    case OPCODE_NOP: 
        break;

    case OPCODE_PUT: 
        vm.stack.push_back(ins.arg1);
        break;

    case OPCODE_POP:
        vm.a = vm.stack.back();
        vm.stack.pop_back();
        break;

    case OPCODE_JMPIF:
        vm.a = vm.stack.back();
        vm.stack.pop_back();
        if (vm.a != 0) {
            vm.ip = labels.at(ins.label);
            goto CONTEXT_CHANGE;
        }
        break;

    case OPCODE_CALL:
        vm.returnstack.push_back(vm.ip);
        vm.ip = labels.at(ins.label);
        goto CONTEXT_CHANGE;

    case OPCODE_RETURN:
        if (vm.stack.empty())
            return State::EXIT;
        vm.a = vm.returnstack.back();
        vm.returnstack.pop_back();
        vm.ip = vm.a;
        break;
        goto CONTEXT_CHANGE;

    case OPCODE_VAR:
        vm.scopestack.back().insert({ins.label, 0});
        break;

    case OPCODE_STORE:
        vm.a = vm.stack.back();
        vm.stack.pop_back();
        vm.scopestack.back().insert({ins.label, vm.a});
        break;

    case OPCODE_LOAD:
        vm.a = vm.scopestack.back().at(ins.label);
        vm.stack.push_back(vm.a);
        break;

    case OPCODE_EQ:
        vm.a = vm.stack.back();
        vm.stack.pop_back();
        vm.b = vm.stack.back();
        vm.stack.pop_back();
        if (vm.a == vm.b)
            vm.stack.push_back(1);
        else
            vm.stack.push_back(0);
        break;

    case OPCODE_CMP:
        vm.a = vm.stack.back();
        vm.stack.pop_back();
        vm.b = vm.stack.back();
        vm.stack.pop_back();
        if (vm.b == vm.a)
            vm.stack.push_back(0);
        else if (vm.b < vm.a)
            vm.stack.push_back(1);
        else
            vm.stack.push_back(-1);
        break;

   case OPCODE_SWAP: 
        vm.a = vm.stack.back();
        vm.stack.pop_back();
        vm.b = vm.stack.back();
        vm.stack.pop_back();
        vm.stack.push_back(vm.a);
        vm.stack.push_back(vm.b);
        break;

    case OPCODE_PLUS: 
        vm.a = vm.stack.back();
        vm.stack.pop_back();
        vm.b = vm.stack.back();
        vm.stack.pop_back();
        vm.stack.push_back(vm.b+vm.a);
        break;

    case OPCODE_MINUS: 
        vm.a = vm.stack.back();
        vm.stack.pop_back();
        vm.b = vm.stack.back();
        vm.stack.pop_back();
        vm.stack.push_back(vm.b-vm.a);
        break;

    case OPCODE_MULTIPLY: 
        vm.a = vm.stack.back();
        vm.stack.pop_back();
        vm.b = vm.stack.back();
        vm.stack.pop_back();
        vm.stack.push_back(vm.b*vm.a);
        break;

    case OPCODE_DIVIDE:
        vm.a = vm.stack.back();
        vm.stack.pop_back();
        vm.b = vm.stack.back();
        vm.stack.pop_back();
        vm.stack.push_back(vm.b/vm.a);
        break;

    case OPCODE_DUPLAST:
        vm.a = vm.stack.back();
        vm.stack.pop_back();
        vm.stack.push_back(vm.a);
        vm.stack.push_back(vm.a);
        break;

    case OPCODE_DUP:
        vm.a = vm.stack[ins.arg1];
        vm.stack.push_back(vm.a);
        break;

    case OPCODE_WRITE:
        vm.a = vm.stack.back();
        vm.stack.pop_back();
        printf("[stdout] -> %d\n", vm.a);
        break;

    };
    vm.ip++;
    return State::OK;

 CONTEXT_CHANGE:
    return State::OK;
}

State iset_eval(VM& vm, const LabelMap& labels, InstructionSet& iset) {
    vm.ip = 0;
    State state = State::OK; 
    while (state == State::OK && vm.ip < iset.size())
        state = ins_eval(vm, labels, iset[vm.ip]);
    return state;
}

LabelMap extract_labels(const InstructionSet& iset) {
    LabelMap labels{};
    std::size_t idx = 0;
    for (auto ins: iset) {
        if (ins.opcode == OPCODE_LABEL)
            labels[ins.label] = idx;
        idx++;
    }
    return labels;
}

State eval(VM& vm, const std::string& program) {
    Tokens tokens{};
    InstructionSet iset{};
    LabelMap labels{};
    tokens = tokenize(program);
    iset = assemble(tokens);
    labels = extract_labels(iset);
    return iset_eval(vm, labels, iset);
}

std::string file_slurp(const std::string& path) {
    std::ifstream f(path);
    std::string str{};
    f.seekg(0, std::ios::end);   
    str.reserve(f.tellg());
    f.seekg(0, std::ios::beg);
    str.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
    return str;
}

}//ns
