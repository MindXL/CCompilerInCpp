//
// Created by mind on 2022/2/25.
//

#ifndef CCOMPILERINCPP_PRINTVISITOR_H
#define CCOMPILERINCPP_PRINTVISITOR_H

#include "AstNode.h"
#include <string>

namespace CCC {
    class PrintVisitor : public AstVisitor {
    public:
        std::string content;

        void visitProgramNode(ProgramNode *p_node) override;

    private:
        void visitFunctionDefinitionNode(FunctionDefinitionNode *p_node) override;

        void visitFunctionCallNode(FunctionCallNode *p_node) override;

        void visitStatementNode(StatementNode *p_node) override;

        void visitBlockStatementNode(BlockStatementNode *p_node) override;

        void visitIfStatementNode(IfStatementNode *p_node) override;

        void visitWhileStatementNode(WhileStatementNode *p_node) override;

        void visitDoWhileStatementNode(DoWhileStatementNode *p_node) override;

        void visitForStatementNode(ForStatementNode *p_node) override;

        void visitAssignmentNode(AssignmentNode *p_node) override;

        void visitBinaryNode(BinaryNode *p_node) override;

        void visitConstantNode(ConstantNode *p_node) override;

        void visitIdentifierNode(IdentifierNode *p_node) override;
    };
}


#endif //CCOMPILERINCPP_PRINTVISITOR_H
