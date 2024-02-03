#pragma once

#include "Defs.hpp"
#include "InstructionSet.hpp"

namespace LemonVM {

const std::array<std::uint8_t, 2> binary_password = {25, 01}; /*The net is vast and infinite*/

template<typename T>
std::size_t stream_bytes(std::vector<std::uint8_t>& stream, const T t) {
    std::uint8_t* tdata = static_cast<std::uint8_t>(t);
    std::size_t i;
    for (i = 0; i < sizeof(T); i++)
        stream.push_back(tdata[i]);
    return i;
}

std::vector<std::uint8_t> generate_bytecode(const InstructionSet& iset) {
    std::vector<std::uint8_t> stream{};
    std::size_t i;
    /*Insert Password*/
    for (i = 0; i < binary_password.size(); i++)
        stream_bytes(stream, binary_password[i]);

    /*Insert Version*/
    stream_bytes(stream, std::uint8_t(1));
    
    /*Insert Program Size*/
    for (i = 0; i < sizeof(std::size_t); i++)
        stream_bytes(stream, std::uint8_t(0));
 
    /*Insert Program Size*/
    std::size_t size = 0;
    for (auto ins: iset) {
        auto bytecode = instruction_to_bytecode(ins);
        stream.push_back(bytecode);

    }

    return stream;
}

}//ns
