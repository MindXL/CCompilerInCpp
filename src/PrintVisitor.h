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
        void visitStatementNode(StatementNode *p_node) override;

        void visitAssignmentNode(AssignmentNode *p_node) override;

        void visitBinaryNode(BinaryNode *p_node) override;

        void visitConstantNode(ConstantNode *p_node) override;

        void visitIdentifierNode(IdentifierNode *p_node) override;
    };
}


#endif //CCOMPILERINCPP_PRINTVISITOR_H
