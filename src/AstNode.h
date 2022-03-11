//
// Created by mind on 2022/2/25.
//

#ifndef CCOMPILERINCPP_ASTNODE_H
#define CCOMPILERINCPP_ASTNODE_H

#include <memory>
#include <string>
#include <list>
#include <vector>

namespace CCC {
    class IdentifierNode;

    class AstVisitor;

    class AstNode {
    public:
        virtual void accept(AstVisitor *p_visitor) = 0;
    };

    class Identifier {
    public:
        std::string name;
        int offset{-1};    // should always be negative

        explicit Identifier(std::string &name) : name{name} {}
    };

    // TODO: management between function and its namesake
    class ProgramNode : public AstNode {
    public:
        std::list<std::shared_ptr<AstNode>> functions;

        void accept(AstVisitor *p_visitor) override;
    };

    // TODO: management between parameters and namesake locals
    class FunctionNode : public AstNode {
    public:
        std::string name;
        std::vector<std::shared_ptr<Identifier>> parameters;
        std::list<std::shared_ptr<Identifier>> locals;
        std::list<std::shared_ptr<AstNode>> statements;

        void accept(AstVisitor *p_visitor) override;
    };

    class StatementNode : public AstNode {
    public:
        std::shared_ptr<AstNode> left;

        explicit StatementNode(std::shared_ptr<AstNode> &&left) : left{left} {}

        void accept(AstVisitor *p_visitor) override;
    };

    class BlockStatementNode : public AstNode {
    public:
        std::list<std::shared_ptr<AstNode>> statements;

        void accept(AstVisitor *p_visitor) override;
    };

    class IfStatementNode : public AstNode {
    public:
        std::shared_ptr<AstNode> condition_expr{nullptr};
        std::shared_ptr<AstNode> then_stmt{nullptr};
        std::shared_ptr<AstNode> else_stmt{nullptr};

        void accept(AstVisitor *p_visitor) override;
    };

    class WhileStatementNode : public AstNode {
    public:
        std::shared_ptr<AstNode> condition_expr{nullptr};
        std::shared_ptr<AstNode> then_stmt{nullptr};

        void accept(AstVisitor *p_visitor) override;
    };

    class DoWhileStatementNode : public AstNode {
    public:
        std::shared_ptr<AstNode> then_stmt{nullptr};
        std::shared_ptr<AstNode> condition_expr{nullptr};

        void accept(AstVisitor *p_visitor) override;
    };

    class ForStatementNode : public AstNode {
    public:
        std::shared_ptr<AstNode> expr1, expr2, expr3;
        std::shared_ptr<AstNode> then_stmt{nullptr};

        void accept(AstVisitor *p_visitor) override;
    };

    class AssignmentNode : public AstNode {
    public:
        std::shared_ptr<IdentifierNode> left;
        std::shared_ptr<AstNode> right;

        AssignmentNode(
                std::shared_ptr<IdentifierNode> &&left,
                std::shared_ptr<AstNode> &&right
        ) : left{left}, right{right} {}

        void accept(AstVisitor *p_visitor) override;
    };

    enum class BinaryOperator {
        Add, Sub, Mul, Div,
        EQ, NE, GT, GE, LT, LE
    };

    class BinaryNode : public AstNode {
    public:
        BinaryOperator op;
        std::shared_ptr<AstNode> left;
        std::shared_ptr<AstNode> right;

        BinaryNode(
                BinaryOperator op,
                std::shared_ptr<AstNode> &&left,
                std::shared_ptr<AstNode> &&right
        ) : op{op}, left{left}, right{right} {}

        void accept(AstVisitor *p_visitor) override;
    };

    class ConstantNode : public AstNode {
    public:
        int value;

        explicit ConstantNode(int value) : value{value} {}

        void accept(AstVisitor *p_visitor) override;
    };

    class IdentifierNode : public AstNode {
    public:
        std::shared_ptr<Identifier> local;

        explicit IdentifierNode(std::shared_ptr<Identifier> &&local) : local{local} {}

        void accept(AstVisitor *p_visitor) override;
    };

    class AstVisitor {
    public:
        virtual void visitProgramNode(ProgramNode *p_node) = 0;

        virtual void visitFunctionNode(FunctionNode *p_node) = 0;

        virtual void visitStatementNode(StatementNode *p_node) = 0;

        virtual void visitBlockStatementNode(BlockStatementNode *p_node) = 0;

        virtual void visitIfStatementNode(IfStatementNode *p_node) = 0;

        virtual void visitWhileStatementNode(WhileStatementNode *p_node) = 0;

        virtual void visitDoWhileStatementNode(DoWhileStatementNode *p_node) = 0;

        virtual void visitForStatementNode(ForStatementNode *p_node) = 0;

        virtual void visitAssignmentNode(AssignmentNode *p_node) = 0;

        virtual void visitBinaryNode(BinaryNode *p_node) = 0;

        virtual void visitConstantNode(ConstantNode *p_node) = 0;

        virtual void visitIdentifierNode(IdentifierNode *p_node) = 0;
    };
}


#endif //CCOMPILERINCPP_ASTNODE_H
