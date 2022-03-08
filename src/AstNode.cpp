//
// Created by mind on 2022/2/25.
//

#include "AstNode.h"

using namespace CCC;

void ProgramNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitProgramNode(this);
}

void StatementNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitStatementNode(this);
}

void BlockStatementNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitBlockStatementNode(this);
}

void IfStatementNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitIfStatementNode(this);
}

void WhileStatementNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitWhileStatementNode(this);
}

void DoWhileStatementNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitDoWhileStatementNode(this);
}

void AssignmentNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitAssignmentNode(this);
}

void BinaryNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitBinaryNode(this);
}

void ConstantNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitConstantNode(this);
}

void IdentifierNode::accept(AstVisitor *p_visitor) {
    p_visitor->visitIdentifierNode(this);
}
