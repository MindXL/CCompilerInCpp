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

    int stack_size = 0;
    for (const auto &local:p_node->locals) {
        stack_size += 8;
        local->offset = stack_size * -1;
    }

    /* rbp 栈基指针, rsp 栈顶指针 */

    // 修改rsp的地址指向rbp
    cout << "\tpush %rbp" << endl
         << "\tmov %rsp, %rbp" << endl;

    // 开辟32字节的栈空间
    cout << "\tsub $" << stack_size << ", %rsp" << endl;

    for (const auto &statement:p_node->statements) {
        statement->accept(this);
        assert(this->stack_level == 0);
    }

    /* 函数结束 */
    cout << "\tmov %rbp, %rsp" << endl
         << "\tpop %rbp" << endl
         << "\tret" << endl;
}

void CodeGenerator::visitStatementNode(StatementNode *p_node) {
    p_node->left->accept(this);
}

void CodeGenerator::visitAssignmentNode(AssignmentNode *p_node) {
    using std::cout, std::endl;

    /// x86
    assert(p_node->left != nullptr);
    cout << "\tlea " << p_node->left->local->offset << "(%rbp), %rax" << endl;    // 取变量地址至rax
    this->pushRAX();    // rax（变量的地址）入栈
    p_node->right->accept(this);    // 计算赋值运算符的右操作数并取至rax
    this->popTo("%rdi");    // 变量的地址弹出至rdi
    cout << "\tmov %rax, (%rdi)" << endl;    // 右操作数存入变量的地址处
}

void CodeGenerator::visitBinaryNode(BinaryNode *p_node) {
    using std::cout, std::endl;

    p_node->right->accept(this);
    this->pushRAX();
    p_node->left->accept(this);
    this->popTo("%rdi");

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

void CodeGenerator::pushRAX() {
    std::cout << "\tpush %rax" << std::endl;
    this->stack_level++;
}

void CodeGenerator::popTo(const char *reg) {
    std::cout << "\tpop " << reg << std::endl;
    this->stack_level--;
}

void CodeGenerator::visitIdentifierNode(IdentifierNode *p_node) {
    using std::cout, std::endl;

    /// x86
    cout << "\tlea " << p_node->local->offset << "(%rbp), %rax" << endl    // 取变量地址至rax
         << "\tmov (%rax), %rax" << endl;    // 间接寻址
}
