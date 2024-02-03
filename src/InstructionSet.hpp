#pragma once

#include "Defs.hpp"

namespace LemonVM {

enum Opcode : std::uint8_t {
    OPCODE_INVALID = 00,
    OPCODE_EXIT    = 01,
    OPCODE_NOP     = 02,
    OPCODE_PUT     = 03,
    OPCODE_POP     = 04,
    OPCODE_DUP     = 05,
    OPCODE_DUPLAST = 06,
    OPCODE_SWAP    = 07,

    OPCODE_LABEL  = 20,
    //OPCODE_JMP,
    OPCODE_JMPIF  = 21,
    OPCODE_CALL   = 22,
    OPCODE_RETURN = 23,

    OPCODE_PLUS     = 30,
    OPCODE_MINUS    = 31,
    OPCODE_MULTIPLY = 32,
    OPCODE_DIVIDE   = 33,

    OPCODE_VAR   = 40,
    OPCODE_LOAD  = 41,
    OPCODE_STORE = 42,

    OPCODE_CMP = 50,
    OPCODE_EQ  = 51,
    //OPCODE_IF,
    OPCODE_WRITE = 60,

    OPCODE_COUNT
};

using Arg = int;

struct Instruction {
    Opcode opcode{OPCODE_NOP};
    Arg arg1{0};
    std::string label{};
};
using InstructionSet = std::vector<Instruction>;

inline const Instruction ins_new(Opcode op)                     { return {op, 0, ""}; }
inline const Instruction ins_new(Opcode op, Arg arg)            { return {op, arg, ""}; }
inline const Instruction ins_new(Opcode op, std::string label)  { return {op, 0, label}; }

inline Instruction ins_exit()        { return ins_new(OPCODE_EXIT); }
inline Instruction ins_nop()         { return ins_new(OPCODE_NOP); }
inline Instruction ins_swap()        { return ins_new(OPCODE_SWAP); }
inline Instruction ins_pop()         { return ins_new(OPCODE_POP); }
inline Instruction ins_put(Arg arg1) { return ins_new(OPCODE_PUT, arg1); }
inline Instruction ins_dup(Arg arg1) { return ins_new(OPCODE_DUP, arg1); }
inline Instruction ins_duplast()     { return ins_new(OPCODE_DUPLAST); }
inline Instruction ins_eq()          { return ins_new(OPCODE_EQ); }
inline Instruction ins_cmp()         { return ins_new(OPCODE_CMP); }
inline Instruction ins_write()       { return ins_new(OPCODE_WRITE); }
inline Instruction ins_plus()        { return ins_new(OPCODE_PLUS); }
inline Instruction ins_minus()       { return ins_new(OPCODE_MINUS); }
inline Instruction ins_multiply()    { return ins_new(OPCODE_MULTIPLY); }
inline Instruction ins_divide()      { return ins_new(OPCODE_DIVIDE); }

inline Instruction ins_label(std::string label) { return ins_new(OPCODE_LABEL, label); }
inline Instruction ins_jmpif(std::string label) { return ins_new(OPCODE_JMPIF, label); }
inline Instruction ins_call(std::string label)  { return ins_new(OPCODE_CALL, label); }
inline Instruction ins_return()                 { return ins_new(OPCODE_RETURN); }

inline Instruction ins_var(std::string name)   { return ins_new(OPCODE_VAR, name); }
inline Instruction ins_load(std::string name)  { return ins_new(OPCODE_LOAD, name); }
inline Instruction ins_store(std::string name) { return ins_new(OPCODE_STORE, name); }

static const std::string
str(const Instruction& ins)
{
    switch (ins.opcode) {
    case OPCODE_EXIT:     return "exit";
    case OPCODE_NOP:      return "nop";
    case OPCODE_SWAP:     return "swap";
    case OPCODE_POP:      return "pop";
    case OPCODE_PUT:      return "put " + std::to_string(ins.arg1);
    case OPCODE_PLUS:     return "plus";
    case OPCODE_MINUS:    return "minus";
    case OPCODE_MULTIPLY: return "multiply";
    case OPCODE_DIVIDE:   return "divide";
    case OPCODE_DUP:      return "dup";
    case OPCODE_DUPLAST:  return "duplast";
    case OPCODE_WRITE:    return "write";
    case OPCODE_EQ:       return "eq";
    case OPCODE_CMP:      return "cmp";
    case OPCODE_LABEL:    return "label " + ins.label;
    case OPCODE_JMPIF:    return "jmpif " + ins.label;
    case OPCODE_CALL:     return "call "  + ins.label;
    case OPCODE_RETURN:   return "return";
    case OPCODE_VAR:      return "var"   + ins.label;
    case OPCODE_LOAD:     return "load"  + ins.label;
    case OPCODE_STORE:    return "store" + ins.label;

    case OPCODE_INVALID:
    case OPCODE_COUNT: 
        break;
    };
    assert(1 && "unknown opcode");
    return "unreachable opcode";
}

std::string
ISet_disasemble(const InstructionSet& iset)
{
    std::stringstream ss{};
    for (auto it : iset) {
        ss << str(it);
        ss << "\n";
    }
    return ss.str();
}

Opcode
get_opcode(const std::string& str)
{
    /*TODO: Use a map for quicker access*/
    if (str == "exit")     return OPCODE_EXIT;
    if (str == "nop")      return OPCODE_NOP;
    if (str == "swap")     return OPCODE_SWAP;
    if (str == "put")      return OPCODE_PUT;
    if (str == "plus")     return OPCODE_PLUS;
    if (str == "minus")    return OPCODE_MINUS;
    if (str == "multiply") return OPCODE_MULTIPLY;
    if (str == "divide")   return OPCODE_DIVIDE;
    if (str == "dup")      return OPCODE_DUP;
    if (str == "duplast")  return OPCODE_DUPLAST;
    if (str == "write")    return OPCODE_WRITE;
    if (str == "eq")       return OPCODE_EQ;
    if (str == "cmp")      return OPCODE_CMP;
    if (str == "label")    return OPCODE_LABEL;
    if (str == "jmpif")    return OPCODE_JMPIF;
    if (str == "call")     return OPCODE_CALL;
    if (str == "var")      return OPCODE_VAR;
    if (str == "load")     return OPCODE_LOAD;
    if (str == "store")    return OPCODE_STORE;
    if (str == "return")   return OPCODE_RETURN;
    return OPCODE_INVALID;
}

bool is_opcode(const std::string& str) {
    if (get_opcode(str) == OPCODE_INVALID)
        return false;
    return true;
}

}//ns
