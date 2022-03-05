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
    content.push_back(';');
}

void PrintVisitor::visitBlockStatementNode(BlockStatementNode *p_node) {
    content.push_back('{');
    for (const auto &stmt:p_node->statements) {
        stmt->accept(this);
    }
    content.push_back('}');
}

void PrintVisitor::visitIfStatementNode(IfStatementNode *p_node) {
    content.append("if");
    content.push_back('(');
    p_node->condition_expr->accept(this);
    content.push_back(')');
    p_node->then_stmt->accept(this);
    if (p_node->else_stmt) {
        content.append("else ");
        p_node->else_stmt->accept(this);
    }
}

void PrintVisitor::visitAssignmentNode(AssignmentNode *p_node) {
    p_node->left->accept(this);
    content.push_back('=');
    p_node->right->accept(this);
}

void PrintVisitor::visitBinaryNode(BinaryNode *p_node) {
    p_node->left->accept(this);
    switch (p_node->op) {
        case BinaryOperator::Add:
            content.push_back('+');
            break;
        case BinaryOperator::Sub:
            content.push_back('-');
            break;
        case BinaryOperator::Mul:
            content.push_back('*');
            break;
        case BinaryOperator::Div:
            content.push_back('/');
            break;
        case BinaryOperator::EQ:
            content.append("==");
            break;
        case BinaryOperator::NE:
            content.append("!=");
            break;
        case BinaryOperator::GT:
            content.push_back('>');
            break;
        case BinaryOperator::GE:
            content.append(">=");
            break;
        case BinaryOperator::LT:
            content.push_back('<');
            break;
        case BinaryOperator::LE:
            content.append("<=");
            break;
    }
    p_node->right->accept(this);
}

void PrintVisitor::visitConstantNode(ConstantNode *p_node) {
    content.append(std::to_string(p_node->value));
}

void PrintVisitor::visitIdentifierNode(IdentifierNode *p_node) {
    content.append(p_node->local->name);
}
