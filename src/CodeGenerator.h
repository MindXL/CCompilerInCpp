//
// Created by mind on 2022/2/25.
//

#ifndef CCOMPILERINCPP_CODEGENERATOR_H
#define CCOMPILERINCPP_CODEGENERATOR_H

#include"AstNode.h"

#include <iostream>
#include <string>
#include <cassert>

namespace CCC {
    class CodeGenerator : public AstVisitor {
    private:
        int stack_level{0};
        int n_mnemonic{0};
        /* x86寄存器 */
        static constexpr const char *registers[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        std::string func_name;    // The function which is to be defined.

    public:
        void visitProgramNode(ProgramNode *p_node) override;

    private:
        static std::string wrapFunctionName(std::string &name);

        void visitFunctionDefinitionNode(FunctionDefinitionNode *p_node) override;

        void visitFunctionCallNode(FunctionCallNode *p_node) override;

        void visitStatementNode(StatementNode *p_node) override;

        void visitBlockStatementNode(BlockStatementNode *p_node) override;

        void visitIfStatementNode(IfStatementNode *p_node) override;

        void visitWhileStatementNode(WhileStatementNode *p_node) override;

        void visitDoWhileStatementNode(DoWhileStatementNode *p_node) override;

        void visitForStatementNode(ForStatementNode *p_node) override;

        void visitReturnStatementNode(ReturnStatementNode *p_node) override;


        void visitAssignmentNode(AssignmentNode *p_node) override;

        void visitBinaryNode(BinaryNode *p_node) override;

        void visitConstantNode(ConstantNode *p_node) override;

        void visitIdentifierNode(IdentifierNode *p_node) override;

        void pushRAX();

        void popTo(const char *reg);

        // 将size向上调整至align的整数倍
        static constexpr int align(int size, int align);
    };
}


#endif //CCOMPILERINCPP_CODEGENERATOR_H
