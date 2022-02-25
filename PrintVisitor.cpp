//
// Created by mind on 2022/2/25.
//

#include "PrintVisitor.h"

#include<iostream>
#include <cassert>

using namespace CCC;

void PrintVisitor::visitProgramNode(ProgramNode *p_node) {
    p_node->left->accept(this);
    std::cout << std::endl;
}

void PrintVisitor::visitBinaryNode(BinaryNode *p_node) {
    p_node->right->accept(this);
    p_node->left->accept(this);
    switch (p_node->op) {
        case BinaryOperator::Add:
            std::cout << " + ";
            break;
        case BinaryOperator::Sub:
            std::cout << " - ";
            break;
        case BinaryOperator::Mul:
            std::cout << " * ";
            break;
        case BinaryOperator::Div:
            std::cout << " / ";
            break;
        default:
            assert(0);
    }
}

void PrintVisitor::visitConstantNode(ConstantNode *p_node) {
    std::cout << ' ' << p_node->value << ' ';
}
