//
// Created by mind on 2022/2/25.
//

#include "PrintVisitor.h"

#include <cassert>

using namespace CCC;

void PrintVisitor::visitProgramNode(ProgramNode *p_node) {
    p_node->left->accept(this);
}

void PrintVisitor::visitBinaryNode(BinaryNode *p_node) {
    p_node->right->accept(this);
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
}

void PrintVisitor::visitConstantNode(ConstantNode *p_node) {
    this->content.append(std::to_string(p_node->value));
}
