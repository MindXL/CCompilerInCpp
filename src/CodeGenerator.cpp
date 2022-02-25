//
// Created by mind on 2022/2/25.
//

#include "CodeGenerator.h"

#include <iostream>
#include <cassert>

using namespace CCC;

void CodeGenerator::visitProgramNode(ProgramNode *p_node) {
    using std::cout, std::endl;
    cout << "\t.text" << endl;

    /* 函数开始 */
#ifdef __linux__
    /// Linux
    cout << "\t.globl prog" << endl
         << "prog:" << endl;
#else
    /// MacOS
    cout << "\t.globl _prog" << endl
              << "_prog:" << endl;
#endif
    /* rbp 是当前函数的栈空间基地址, rsp 是当前函数为被调用函数准备的栈空间的基地址 */

    // 修改rsp的地址指向rbp
    cout << "\tpush %rbp" << endl
         << "\tmov %rsp, %rbp" << endl;

    // 开辟32字节的栈空间
    cout << "\tsub $32, %rsp" << endl;

    p_node->left->accept(this);
    assert(this->stack_level == 0);

    /* 函数结束 */
    cout << "\tmov %rbp, %rsp" << endl
         << "\tpop %rbp" << endl
         << "\tret" << endl;
}

void CodeGenerator::visitBinaryNode(BinaryNode *p_node) {
    using std::cout, std::endl;

    p_node->right->accept(this);
    this->push();
    p_node->left->accept(this);
    this->pop("%rdi");

    switch (p_node->op) {
        case BinaryOperator::Add:
            cout << "\tadd %rdi, %rax" << endl;
            break;
        case BinaryOperator::Sub:
            cout << "\tsub %rdi, %rax" << endl;
            break;
        case BinaryOperator::Mul:
            cout << "\timul %rdi, %rax" << endl;
            break;
        case BinaryOperator::Div:
            cout << "\tcqo" << endl
                 << "\tidiv %rdi" << endl;
            break;
        default:
            assert(0);
    }
}

void CodeGenerator::visitConstantNode(ConstantNode *p_node) {
    std::cout << "\tmov $" << p_node->value << ", %rax" << std::endl;
}

void CodeGenerator::push() {
    std::cout << "\tpush %rax" << std::endl;
    this->stack_level++;
}

void CodeGenerator::pop(const char *reg) {
    std::cout << "\tpop " << reg << std::endl;
    this->stack_level--;
}
