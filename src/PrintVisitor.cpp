//
// Created by mind on 2022/2/25.
//

#include "PrintVisitor.h"

using namespace CCC;

void PrintVisitor::visitProgramNode(ProgramNode *p_node) {
    for (const auto &f:p_node->functions) {
        f->accept(this);
    }
}

void PrintVisitor::visitFunctionDefinitionNode(FunctionDefinitionNode *p_node) {
    content += p_node->name;
    content += '(';
    for (auto cit = p_node->parameters.cbegin(); cit != p_node->parameters.cend(); cit++) {
        if (cit != p_node->parameters.cbegin())
            content += ',';
        content += (*cit)->name;
    }
    content += ')';
    content += '{';
    for (const auto &s:p_node->statements) {
        s->accept(this);
    }
    content += '}';
}

void PrintVisitor::visitFunctionCallNode(FunctionCallNode *p_node) {
    content += p_node->name;
    content += '(';
    for (auto cit = p_node->arguments.cbegin(); cit != p_node->arguments.cend(); cit++) {
        if (cit != p_node->arguments.cbegin())
            content += ',';
        (*cit)->accept(this);
    }
    content += ')';
}

void PrintVisitor::visitStatementNode(StatementNode *p_node) {
    if (p_node->left)
        p_node->left->accept(this);
    content += ';';
}

void PrintVisitor::visitBlockStatementNode(BlockStatementNode *p_node) {
    content += '{';
    for (const auto &stmt:p_node->statements) {
        stmt->accept(this);
    }
    content += '}';
}

void PrintVisitor::visitIfStatementNode(IfStatementNode *p_node) {
    content += "if";
    content += '(';
    p_node->condition_expr->accept(this);
    content += ')';
    p_node->then_stmt->accept(this);
    if (p_node->else_stmt) {
        content += "else ";
        p_node->else_stmt->accept(this);
    }
}

void PrintVisitor::visitWhileStatementNode(WhileStatementNode *p_node) {
    content += "while";
    content += '(';
    p_node->condition_expr->accept(this);
    content += ')';
    p_node->then_stmt->accept(this);
}

void PrintVisitor::visitDoWhileStatementNode(DoWhileStatementNode *p_node) {
    content += "do ";
    p_node->then_stmt->accept(this);
    content += "while(";
    p_node->condition_expr->accept(this);
    content += ");";
}

void PrintVisitor::visitForStatementNode(ForStatementNode *p_node) {
    content += "for(";
    if (p_node->expr1) p_node->expr1->accept(this);
    content += ';';
    if (p_node->expr2) p_node->expr2->accept(this);
    content += ';';
    if (p_node->expr3) p_node->expr3->accept(this);
    content += ')';
    p_node->then_stmt->accept(this);
}

void PrintVisitor::visitReturnStatementNode(ReturnStatementNode *p_node) {
    content += "return ";
    p_node->left->accept(this);
    content += ';';
}

void PrintVisitor::visitAssignmentNode(AssignmentNode *p_node) {
    if (p_node->wrapped) content += '(';
    p_node->left->accept(this);
    content += '=';
    p_node->right->accept(this);
    if (p_node->wrapped) content += ')';
}

void PrintVisitor::visitBinaryNode(BinaryNode *p_node) {
    if (p_node->wrapped) content += '(';
    p_node->left->accept(this);
    switch (p_node->op) {
        case BinaryOperator::Add:
            content += '+';
            break;
        case BinaryOperator::Sub:
            content += '-';
            break;
        case BinaryOperator::Mul:
            content += '*';
            break;
        case BinaryOperator::Div:
            content += '/';
            break;
        case BinaryOperator::EQ:
            content += "==";
            break;
        case BinaryOperator::NE:
            content += "!=";
            break;
        case BinaryOperator::GT:
            content += '>';
            break;
        case BinaryOperator::GE:
            content += ">=";
            break;
        case BinaryOperator::LT:
            content += '<';
            break;
        case BinaryOperator::LE:
            content += "<=";
            break;
    }
    p_node->right->accept(this);
    if (p_node->wrapped) content += ')';
}

void PrintVisitor::visitConstantNode(ConstantNode *p_node) {
    if (p_node->wrapped) content += '(';
    content += std::to_string(p_node->value);
    if (p_node->wrapped) content += ')';
}

void PrintVisitor::visitIdentifierNode(IdentifierNode *p_node) {
    if (p_node->wrapped) content += '(';
    content += p_node->local->name;
    if (p_node->wrapped) content += ')';
}
