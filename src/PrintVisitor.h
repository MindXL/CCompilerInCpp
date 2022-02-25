//
// Created by mind on 2022/2/25.
//

#ifndef CCOMPILERINCPP_PRINTVISITOR_H
#define CCOMPILERINCPP_PRINTVISITOR_H

#include "AstNode.h"

namespace CCC {
    class PrintVisitor : public AstVisitor {
    public:
        void visitProgramNode(ProgramNode *p_node) override;

    private:
        void visitBinaryNode(BinaryNode *p_node) override;

        void visitConstantNode(ConstantNode *p_node) override;
    };
}


#endif //CCOMPILERINCPP_PRINTVISITOR_H
