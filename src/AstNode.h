//
// Created by mind on 2022/2/25.
//

#ifndef CCOMPILERINCPP_ASTNODE_H
#define CCOMPILERINCPP_ASTNODE_H

#include <memory>

namespace CCC {
    class AstVisitor;

    class AstNode {
    public:
//        virtual ~AstNode() {};

        virtual void accept(AstVisitor *p_visitor) = 0;
    };

    class ProgramNode : public AstNode {
    public:
        std::shared_ptr<AstNode> left;

        explicit ProgramNode(std::shared_ptr<AstNode> &&left) : left{left} {}

        void accept(AstVisitor *p_visitor) override;
    };

    enum class BinaryOperator {
        Add,
        Sub,
        Mul,
        Div
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

    class AstVisitor {
    public:
//        virtual ~AstVisitor(){}
        virtual void visitProgramNode(ProgramNode *p_node) = 0;

        virtual void visitBinaryNode(BinaryNode *p_node) = 0;

        virtual void visitConstantNode(ConstantNode *p_node) = 0;
    };
}


#endif //CCOMPILERINCPP_ASTNODE_H
