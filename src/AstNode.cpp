//
// Created by mind on 2022/2/25.
//

#include "AstNode.h"

using namespace CCC;

void ProgramNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitProgramNode(this);
}

void BinaryNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitBinaryNode(this);
}

void ConstantNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitConstantNode(this);
}


