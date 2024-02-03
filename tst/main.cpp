#include <iostream>
#include <cassert>
#include "testlib.h"
#include "../LemonVM.hpp"

using namespace LemonVM;

char test_top(VM& vm, int v) {
    if (vm.stack.size() == 0)
        return false;
    int last = vm.stack.back();
    return last == v;
}

void
print_iset(const std::vector<Instruction>& iset)
{
    std::cout << "== InstructionSet Start ==\n";
    std::cout << ISet_disasemble(iset);
    std::cout << "== InstructionSet End ==" << std::endl;
}

void
print_stack(VM& vm)
{
    std::cout << "== Program Start ==\n";
    std::cout << stack_dump(vm);
    std::cout << "== Program End ==" << std::endl;
}

void
print_labels(LabelMap& labels)
{
    std::cout << "== LabelMap Start ==\n";
    for (auto [label, idx]: labels) {
        std::cout << label << " -> " << idx << "\n";
    }
    std::cout << "== LabelMap End ==" << std::endl;
}

void test(void) {
    VM vm;
    State state;
    LabelMap labels{};

    InstructionSet a = {
        ins_put(3),
        ins_put(4),
    };
    print_iset(a);
    state = iset_eval(vm, labels, a);
    print_stack(vm);
    TL_TEST(state == State::OK);

    InstructionSet b = {
        ins_plus(),
    };
    print_iset(b);
    state = iset_eval(vm, labels, b);
    print_stack(vm);
    TL_TEST(state == State::OK);
}

void test_exit(void) {
    VM vm;
    State state;
    LabelMap labels{};

    InstructionSet a = {
        ins_put(1),
        ins_exit(),
        ins_put(2),
    };

    print_iset(a);
    state = iset_eval(vm, labels, a);
    print_stack(vm);
    TL_TEST(state == State::EXIT);
}

void test_math(void) {
    VM vm;
    State state;
    LabelMap labels{};

    InstructionSet a = {
        ins_put(3),
        ins_put(4),
        ins_plus(),
        ins_duplast(),
        ins_write(),
        // 7
    };

    print_iset(a);
    state = iset_eval(vm, labels, a);
    print_stack(vm);
    TL_TEST(test_top(vm, 7));

    InstructionSet b = {
        ins_put(2),
        ins_multiply(),
        ins_duplast(),
        ins_write(),
        // 14
    };
    print_iset(b);
    state = iset_eval(vm, labels, b);
    stack_dump(vm);
    TL_TEST(test_top(vm, 14));


    InstructionSet c = {
        ins_put(4),
        ins_minus(),
        ins_duplast(),
        ins_write(),
        // 10
    };
    print_iset(c);
    state = iset_eval(vm, labels, c);
    print_stack(vm);
    TL_TEST(test_top(vm, 10));

    InstructionSet d = {
        ins_put(2),
        ins_divide(),
        ins_duplast(),
        ins_write(),
        // 5
    };
    print_iset(d);
    state = iset_eval(vm, labels, d);
    print_stack(vm);
    TL_TEST(test_top(vm, 5));
}

void test_eq(void) {
    VM vm;
    State state;
    LabelMap labels{};

    InstructionSet a = {
        ins_put(3),
        ins_put(4),
        ins_eq(),
        // 0
    };
    print_iset(a);
    state = iset_eval(vm, labels, a);
    print_stack(vm);
    TL_TEST(test_top(vm, 0));
    vm = VM{};

    InstructionSet b = {
        ins_put(3),
        ins_put(3),
        ins_eq(),
        // 1
    };
    print_iset(b);
    state = iset_eval(vm, labels, b);
    print_stack(vm);
    TL_TEST(test_top(vm, 1));
}

void test_cmp(void) {
    VM vm;
    State state;
    LabelMap labels{};

    InstructionSet a = {
        ins_put(3),
        ins_put(4),
        ins_cmp(),
        // 1
    };
    print_iset(a);
    state = iset_eval(vm, labels, a);
    print_stack(vm);
    TL_TEST(test_top(vm, 1));

    InstructionSet b = {
        ins_put(3),
        ins_put(3),
        ins_cmp(),
        // 0
    };
    vm = VM{};
    print_iset(b);
    state = iset_eval(vm, labels, b);
    print_stack(vm);
    TL_TEST(test_top(vm, 0));

    InstructionSet c = {
        ins_put(3),
        ins_put(1),
        ins_cmp(),
        // -1
    };
    vm = VM{};
    print_iset(c);
    state = iset_eval(vm, labels, c);
    print_stack(vm);
    TL_TEST(test_top(vm, -1));
}

void test_labelmap_create(void) {
    LabelMap labels{};

    InstructionSet a = {
        /*0*/ ins_label("fn-plus"),
        ins_plus(),

        /*2*/ ins_label("fn-scale"),
        ins_multiply(),
        ins_plus(),
    };

    print_iset(a);
    labels = extract_labels(a);
    print_labels(labels);

    TL_TEST(labels["fn-plus"] == 0);
    TL_TEST(labels["fn-scale"] == 2);

}

void test_jmpif(void) {
    VM vm;
    State state;
    LabelMap labels{};

    InstructionSet a = {
        ins_put(0),
        ins_jmpif("fn1"),

        ins_label("fn0"),
        ins_put(0),
        ins_exit(),
        
        ins_label("fn1"),
        ins_put(1),
        ins_exit(),
    };

    labels = extract_labels(a);
    state = iset_eval(vm, labels, a);
    print_iset(a);
    print_labels(labels);
    print_stack(vm);
    TL_TEST(test_top(vm, 0));

    InstructionSet b = {
        ins_put(1),
        ins_jmpif("fn1"),

        ins_label("fn0"),
        ins_put(0),
        ins_exit(),
        
        ins_label("fn1"),
        ins_put(1),
        ins_exit(),
    };

    labels = extract_labels(b);
    vm = VM();
    state = iset_eval(vm, labels, b);
    print_iset(a);
    print_labels(labels);
    print_stack(vm);
    TL_TEST(test_top(vm, 1));
}


void test_call_return(void) {
    VM vm;
    State state;
    LabelMap labels{};

    InstructionSet a = {
        ins_call("main"),
        ins_exit(),

        ins_label("cube"),
        ins_duplast(),
        ins_duplast(),
        ins_multiply(),
        ins_multiply(),
        ins_return(),
        
        ins_label("main"),
        ins_put(7),
        ins_call("cube"),
        ins_return(),
    };

    labels = extract_labels(a);
    print_iset(a);
    print_labels(labels);
    state = iset_eval(vm, labels, a);
    print_stack(vm);
    TL_TEST(test_top(vm, 7*7*7));
}

void test_tokenization(void) {
    VM vm;
    State state;
    LabelMap labels{};
    InstructionSet iset;

    const std::string program =
        "call main\n"
        "exit\n"

        "label cube\n"
        "duplast\n"
        "duplast\n"
        "multiply\n"
        "multiply\n"
        "return\n"

        "label main\n"
        "put 7\n"
        "call cube\n"
        "return\n";
    std::cout << program << std::endl;

    Tokens tokens = tokenize(program);
    print_tokens(tokens);
    TL_TEST(tokens.size() == 17);
}

void test_assemble(void) {
    LabelMap labels{};
    InstructionSet iset;
    Tokens tokens;

    const std::string program =
        "call main\n"
        "exit\n"

        "label cube\n"
        "duplast\n"
        "duplast\n"
        "multiply\n"
        "multiply\n"
        "return\n"

        "label main\n"
        "put 7\n"
        "call cube\n"
        "return\n";
    std::cout << program << std::endl;

    tokens = tokenize(program);
    print_tokens(tokens);
    TL_TEST(tokens.size() == 17);
    iset = assemble(tokens);
    print_iset(iset);
    TL_TEST(iset.size() == 12);
}

void test_cube_function(void) {
    VM vm{};
    State state = State::OK;

    const std::string program = "call main\n"
                                "exit\n"

                                "label main\n"
                                "put 7\n"
                                "call cube\n"
                                "return\n"

                                "label cube\n"
                                "duplast\n"
                                "duplast\n"
                                "multiply\n"
                                "multiply\n"
                                "return\n"
        ;
    std::cout << program << std::endl;

    state = eval(vm, program);
    print_stack(vm);
    TL_TEST(test_top(vm, 7*7*7));
}

void test_comment(void) {
    VM vm{};
    State state = State::OK;

    const std::string program = "call main\n"
                                "exit\n"

                                "# this is a comment\n"
                                "label main\n"
                                "  # this is the main entry point of the program\n"
                                "  put 4\n"
                                "          put 2\n"
                                "  put 7\n"
                                " call scale\n"
                                "  return\n"

                                "# Scaling function\n"
                                "label scale\n"
                                "  multiply\n"
                                "  plus\n"
                                "  return\n"
        ;
    std::cout << program << std::endl;

    state = eval(vm, program);
    print_stack(vm);
    TL_TEST(test_top(vm, 2*7+4));
}

void test_file(void) {
    VM vm{};
    State state = State::OK;
    const std::string program = file_slurp("../../code/cube-of-7.hl");
    TL_TEST(program != "");
    std::cout << program << std::endl;
    state = eval(vm, program);
    TL_TEST(test_top(vm, 7*7*7));
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test());
	TL(test_exit());
	TL(test_math());
	TL(test_eq());
	TL(test_cmp());
	TL(test_labelmap_create());
	TL(test_jmpif());
	TL(test_call_return());

	TL(test_tokenization());
	TL(test_assemble());
	TL(test_cube_function());
	TL(test_comment());
	//TL(test_file());


	tl_summary();
	return 0;
}
