//
// Created by mind on 2022/2/25.
//

#include "CodeGenerator.h"

using namespace CCC;

void CodeGenerator::visitProgramNode(ProgramNode *p_node) {
    for (const auto &f:p_node->functions) {
        f->accept(this);
    }
}

std::string CodeGenerator::wrapFunctionName(std::string &name) {
#ifdef __linux__
    /// Linux
    return name;
#else
    /// MacOS
    return '_' + name;
#endif
}

void CodeGenerator::visitFunctionDefinitionNode(FunctionDefinitionNode *p_node) {
    using std::cout, std::endl;

    func_name = wrapFunctionName(p_node->name);
    cout << "\t.text" << endl;
    /* 函数开始 */
    cout << "\t.globl " << func_name << endl
         << func_name << ':' << endl;

    int stack_size = 0;
    for (const auto &local:p_node->locals) {
        stack_size += 8;
        local->offset = stack_size * -1;
    }
    stack_size = align(stack_size, 16);

    /* rbp 栈基指针, rsp 栈顶指针 */
    // 修改rsp的地址指向rbp
    cout << "\tpush %rbp" << endl
         << "\tmov %rsp, %rbp" << endl;

    // 开辟32字节的栈空间
    cout << "\tsub $" << stack_size << ", %rsp" << endl;

    /* parameters */
    for (int i = 0; i < p_node->parameters.size(); i++) {
        cout << "\tmov " << registers[i] << ", " << p_node->parameters.at(i)->offset << "(%rbp)" << endl;
    }

    for (const auto &s:p_node->statements) {
        s->accept(this);
        assert(stack_level == 0);
    }

    /* 函数结束 */
    cout << ".L." << func_name << ".Return:" << endl
         << "\tmov %rbp, %rsp" << endl
         << "\tpop %rbp" << endl
         << "\tret" << endl;
}

void CodeGenerator::visitFunctionCallNode(FunctionCallNode *p_node) {
    auto &&name{wrapFunctionName(p_node->name)};

    // pass arguments through the registers
    for (const auto &arg:p_node->arguments) {
        arg->accept(this);
        pushRAX();
    }
    // TODO: is this extravagant?
    for (auto i = p_node->arguments.size() - 1; i != decltype(p_node->arguments.size())(0 - 1); i--) {
        popTo(registers[i]);
    }
    std::cout << "\tcall " << name << std::endl;
}

void CodeGenerator::visitStatementNode(StatementNode *p_node) {
    if (p_node->left)
        p_node->left->accept(this);
}

void CodeGenerator::visitBlockStatementNode(BlockStatementNode *p_node) {
    for (const auto &stmt:p_node->statements) {
        stmt->accept(this);
    }
}

void CodeGenerator::visitIfStatementNode(IfStatementNode *p_node) {
    using std::cout, std::endl;

    const int n = n_mnemonic++;

    p_node->condition_expr->accept(this);
    cout << "\tcmp $0, %rax" << endl;
    if (p_node->else_stmt) {
        // 如果条件为假且有else语句则先跳else语句
        cout << "\tje .L" << n << ".else" << endl;
    } else {
        // 如果条件为假且无else语句则跳if语句结束
        cout << "\tje .L" << n << ".end" << endl;
    }
    p_node->then_stmt->accept(this);
    cout << "\tjmp .L" << n << ".end" << endl;
    if (p_node->else_stmt) {
        cout << ".L" << n << ".else:" << endl;
        p_node->else_stmt->accept(this);
    }
    cout << ".L" << n << ".end:" << endl;
}

void CodeGenerator::visitWhileStatementNode(WhileStatementNode *p_node) {
    using std::cout, std::endl;

    const int n = n_mnemonic++;

    cout << ".L" << n << ".begin:" << endl;
    p_node->condition_expr->accept(this);
    cout << "\tcmp $0, %rax" << endl
         << "\tje .L" << n << ".end" << endl;
    p_node->then_stmt->accept(this);
    cout << "\tjmp .L" << n << ".begin" << endl
         << ".L" << n << ".end:" << endl;
}

void CodeGenerator::visitDoWhileStatementNode(DoWhileStatementNode *p_node) {
    using std::cout, std::endl;

    const int n = n_mnemonic++;

    cout << ".L" << n << ".begin:" << endl;
    p_node->then_stmt->accept(this);
    p_node->condition_expr->accept(this);
    cout << "\tcmp $0, %rax" << endl
         << "\tjne .L" << n << ".begin" << endl;
}

void CodeGenerator::visitForStatementNode(ForStatementNode *p_node) {
    using std::cout, std::endl;

    const int n = n_mnemonic++;

    if (p_node->expr1)
        p_node->expr1->accept(this);
    cout << ".L" << n << ".begin:" << endl;
    if (p_node->expr2) {
        p_node->expr2->accept(this);
        cout << "\tcmp $0, %rax" << endl
             << "\tje .L" << n << ".end" << endl;
    }
    p_node->then_stmt->accept(this);
    if (p_node->expr3)
        p_node->expr3->accept(this);
    cout << "\tjmp .L" << n << ".begin" << endl
         << ".L" << n << ".end:" << endl;
}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode *p_node) {
    p_node->left->accept(this);
    std::cout << "\tjmp .L." << func_name << ".Return" << std::endl;
}

void CodeGenerator::visitAssignmentNode(AssignmentNode *p_node) {
    using std::cout, std::endl;

    /// x86
    assert(p_node->left != nullptr);
    cout << "\tlea " << p_node->left->local->offset << "(%rbp), %rax" << endl;    // 取变量地址至rax
    pushRAX();    // rax（变量的地址）入栈
    p_node->right->accept(this);    // 计算赋值运算符的右操作数并取至rax
    popTo("%rdi");    // 变量的地址弹出至rdi
    cout << "\tmov %rax, (%rdi)" << endl;    // 右操作数存入变量的地址处
}

void CodeGenerator::visitBinaryNode(BinaryNode *p_node) {
    using std::cout, std::endl;

    // 右操作数在rax
    p_node->right->accept(this);
    pushRAX();

    // 左操作数在rdi
    p_node->left->accept(this);
    popTo("%rdi");

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
        case BinaryOperator::EQ:
            cout << "\tcmp %rdi, %rax" << endl
                 << "\tsete %al" << endl
                 << "\tmovzb %al, %rax" << endl;    // z: 扩展; b: 单字节
            break;
        case BinaryOperator::NE:
            cout << "\tcmp %rdi, %rax" << endl
                 << "\tsetne %al" << endl
                 << "\tmovzb %al, %rax" << endl;
            break;
        case BinaryOperator::GT:
            cout << "\tcmp %rdi, %rax" << endl
                 << "\tsetg %al" << endl
                 << "\tmovzb %al, %rax" << endl;
            break;
        case BinaryOperator::GE:
            cout << "\tcmp %rdi, %rax" << endl
                 << "\tsetge %al" << endl
                 << "\tmovzb %al, %rax" << endl;
            break;
        case BinaryOperator::LT:
            cout << "\tcmp %rdi, %rax" << endl
                 << "\tsetl %al" << endl
                 << "\tmovzb %al, %rax" << endl;
            break;
        case BinaryOperator::LE:
            cout << "\tcmp %rdi, %rax" << endl
                 << "\tsetle %al" << endl
                 << "\tmovzb %al, %rax" << endl;
            break;
    }
}

void CodeGenerator::visitConstantNode(ConstantNode *p_node) {
    std::cout << "\tmov $" << p_node->value << ", %rax" << std::endl;
}

void CodeGenerator::pushRAX() {
    std::cout << "\tpush %rax" << std::endl;
    stack_level++;
}

void CodeGenerator::popTo(const char *reg) {
    std::cout << "\tpop " << reg << std::endl;
    stack_level--;
}

void CodeGenerator::visitIdentifierNode(IdentifierNode *p_node) {
    using std::cout, std::endl;

    /// x86
    cout << "\tlea " << p_node->local->offset << "(%rbp), %rax" << endl    // 取变量地址至rax
         << "\tmov (%rax), %rax" << endl;    // 间接寻址
}

constexpr int CodeGenerator::align(int size, int align) {
    return (size + align - 1) / align * align;
}
