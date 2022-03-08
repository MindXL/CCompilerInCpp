//
// Created by mind on 2022/2/25.
//

#ifndef CCOMPILERINCPP_CODEGENERATOR_H
#define CCOMPILERINCPP_CODEGENERATOR_H

#include"AstNode.h"

namespace CCC {
    class CodeGenerator : public AstVisitor {
    private:
        int stack_level{0};
        int n_mnemonic{0};

    public:
        void visitProgramNode(ProgramNode *p_node) override;

    private:
        void visitStatementNode(StatementNode *p_node) override;

        void visitBlockStatementNode(BlockStatementNode *p_node) override;

        void visitIfStatementNode(IfStatementNode *p_node) override;

        void visitWhileStatementNode(WhileStatementNode *p_node) override;

        void visitDoWhileStatementNode(DoWhileStatementNode *p_node) override;

        void visitAssignmentNode(AssignmentNode *p_node) override;

        void visitBinaryNode(BinaryNode *p_node) override;

        void visitConstantNode(ConstantNode *p_node) override;

        void visitIdentifierNode(IdentifierNode *p_node) override;

        void pushRAX();

        void popTo(const char *reg);
    };
}


#endif //CCOMPILERINCPP_CODEGENERATOR_H
