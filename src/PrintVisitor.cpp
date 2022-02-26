//
// Created by mind on 2022/2/25.
//

#include "PrintVisitor.h"

#include <cassert>

using namespace CCC;

void PrintVisitor::visitProgramNode(ProgramNode *p_node) {
    for (const auto &statement:p_node->statements) {
        statement->accept(this);
    }
}

void PrintVisitor::visitStatementNode(StatementNode *p_node) {
    p_node->left->accept(this);
    this->content.push_back(';');
}

void PrintVisitor::visitAssignmentNode(AssignmentNode *p_node) {
    p_node->left->accept(this);
    this->content.push_back('=');
    p_node->right->accept(this);
}

void PrintVisitor::visitBinaryNode(BinaryNode *p_node) {
    p_node->left->accept(this);
    switch (p_node->op) {
        case BinaryOperator::Add:
            this->content.push_back('+');
            break;
        case BinaryOperator::Sub:
            this->content.push_back('-');
            break;
        case BinaryOperator::Mul:
            this->content.push_back('*');
            break;
        case BinaryOperator::Div:
            this->content.push_back('/');
            break;
        default:
            assert(0);
    }
    p_node->right->accept(this);
}

void PrintVisitor::visitConstantNode(ConstantNode *p_node) {
    this->content.append(std::to_string(p_node->value));
}

void PrintVisitor::visitIdentifierNode(IdentifierNode *p_node) {
    this->content.append(p_node->local->name);
}




