// /*
//  Copyright (C) 2017-2018  Kim HOANG
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
// */
// #pragma once
// #include "Token.hpp"
// #include "Value.hpp"

// #include <cassert>
// #include <deque>
// #include <iostream>
// #include <memory>

// namespace bodhi {
// namespace Expr {
// class Assignment;
// class Binary;
// class Call;
// class Get;
// class Set;
// class Unary;
// class Logical;
// class Literal;
// class Grouping;
// class Variable;

// class IVisitor {
//   public:
//     virtual Value::Ptr visitAssignment(Assignment &expr) = 0;
//     virtual Value::Ptr visitBinary(Binary &expr) = 0;
//     virtual Value::Ptr visitCallExpr(Call &expr) = 0;
//     virtual Value::Ptr visitGet(Get &expr) = 0;
//     virtual Value::Ptr visitSet(Set &expr) = 0;
//     virtual Value::Ptr visitUnary(Unary &expr) = 0;
//     virtual Value::Ptr visitLogical(Logical &expr) = 0;
//     virtual Value::Ptr visitLiteral(Literal &expr) = 0;
//     virtual Value::Ptr visitGrouping(Grouping &expr) = 0;
//     virtual Value::Ptr visitVariable(Variable &expr) = 0;

//   protected:
//     virtual ~IVisitor() {}
// };

// class Base {
//   public:
//     typedef std::shared_ptr<Base> Ptr;

//     virtual Value::Ptr accept(IVisitor &visitor) = 0;

//     // Encode type information here. I want to get rid of RTTI
//     virtual bool isVariable() const { return false; }
//     virtual bool isGetExpr() const { return false; }

//     virtual Token::Ptr getName() const { return nullptr; }
//     virtual Base::Ptr getObject() const { return nullptr; }

//     virtual std::string typeName() const { return "Base"; }

//     unsigned id() const { return m_id; }

//   protected:
//     virtual ~Base() {}
//     Base() : m_id(0) {
//         static unsigned idSeed = 0;
//         idSeed++;
//         m_id = idSeed;
//     }

//   private:
//     Base(Base &&) = delete;
//     Base(const Base &) = delete;

//     unsigned m_id;
// };

// class Assignment : public Base {
//   public:
//     Assignment(Token::Ptr name, Base::Ptr value)
//         : m_name(name), m_value(value) {}

//     ~Assignment() override {}

//     Value::Ptr accept(IVisitor &visitor) override {
//         return visitor.visitAssignment(*this);
//     }
//     Token::Ptr getName() const override { return m_name; }

//     std::string typeName() const override { return "Assignment"; }

//     Token::Ptr m_name;
//     Base::Ptr m_value;
// };

// class Binary : public Base {
//   public:
//     Binary(Base::Ptr left, Token::Ptr token, Base::Ptr right)
//         : m_left(left), m_operator(token), m_right(right) {
//         assert(m_left != nullptr);
//         assert(m_right != nullptr);
//     }

//     ~Binary() override {}

//     std::string typeName() const override { return "Binary"; }

//     Value::Ptr accept(IVisitor &visitor) override {
//         return visitor.visitBinary(*this);
//     }

//     Base::Ptr left() { return m_left; }
//     void left(Base::Ptr val) {
//         m_left = val;
//         assert(val != nullptr);
//     }

//     Base::Ptr right() { return m_right; }
//     void right(Base::Ptr val) {
//         m_right = val;
//         assert(val != nullptr);
//     }

//     Token::Ptr op() { return m_operator; }
//     void op(Token::Ptr val) {
//         m_operator = val;
//         assert(val != nullptr);
//     }

//   private:
//     Base::Ptr m_left;
//     Token::Ptr m_operator;
//     Base::Ptr m_right;
// };

// class Call : public Base {
//   public:
//     Call(Base::Ptr callee, Token::Ptr paren,
//          const std::deque<Base::Ptr> &arguments)
//         : m_callee(callee), m_paren(paren),
//           m_arguments(arguments) // TODO: maybe move the arguments?
//     {}

//     ~Call() override {};

//     std::string typeName() const override { return "Call"; }

//     Value::Ptr accept(IVisitor &visitor) override {
//         // std::cout << "Expr::Call::accept  ENTRY\n";
//         return visitor.visitCallExpr(*this);
//         // std::cout << "Expr::Call::accept  EXIT\n";
//     }

//     Base::Ptr m_callee;
//     Token::Ptr m_paren;
//     std::deque<Base::Ptr> m_arguments;
// };

// class Get : public Base {
//   public:
//     Get(Base::Ptr object, Token::Ptr name) : m_object(object), m_name(name)
//     {}

//     ~Get() override {}

//     std::string typeName() const override { return "Get"; }
//     bool isGetExpr() const override { return true; }
//     Token::Ptr getName() const override { return m_name; }
//     Base::Ptr getObject() const override { return m_object; }

//     Value::Ptr accept(IVisitor &visitor) override {
//         return visitor.visitGet(*this);
//     }

//     Base::Ptr m_object;
//     Token::Ptr m_name;
// };

// class Set : public Base {
//   public:
//     Set(Base::Ptr object, Token::Ptr name, Base::Ptr value)
//         : m_object(object), m_name(name), m_value(value) {}

//     ~Set() override {}

//     std::string typeName() const override { return "Set"; }
//     Token::Ptr getName() const override { return m_name; }
//     Base::Ptr getObject() const override { return m_object; }

//     Value::Ptr accept(IVisitor &visitor) override {
//         return visitor.visitSet(*this);
//     }

//     Base::Ptr m_object;
//     Token::Ptr m_name;
//     Base::Ptr m_value;
// };

// class Grouping : public Base {
//   public:
//     explicit Grouping(Base::Ptr expr) : m_expression(expr) {}

//     ~Grouping() override {}

//     std::string typeName() const override { return "Grouping"; }

//     Value::Ptr accept(IVisitor &visitor) override {
//         return visitor.visitGrouping(*this);
//     }

//     Base::Ptr m_expression;
// };

// class Literal : public Base {
//   public:
//     explicit Literal(Value::Ptr val) : m_value(val) {
//         assert(m_value != nullptr);
//     }

//     explicit Literal(const char *str) : Literal(std::string(str)) {
//         assert(m_value != nullptr);
//     }

//     explicit Literal(bool value) : m_value(std::make_shared<Value>(value)) {
//         assert(m_value != nullptr);
//     }

//     explicit Literal(double value) : m_value(std::make_shared<Value>(value))
//     {
//         assert(m_value != nullptr);
//     }

//     explicit Literal(const std::string &value)
//         : m_value(std::make_shared<Value>(value)) {
//         assert(m_value != nullptr);
//     }

//     std::string typeName() const override { return "Literal"; }

//     Value::Ptr accept(IVisitor &visitor) override {
//         auto val = visitor.visitLiteral(*this);
//         m_value = val;
//         assert(m_value != nullptr);
//         return val;
//     }

//     ~Literal() override {}

//     Value::Ptr value() {
//         assert(m_value != nullptr);
//         return m_value;
//     }
//     void value(Value::Ptr val) {
//         m_value = val;
//         assert(m_value != nullptr);
//     }

//   private:
//     Literal() = delete;
//     Literal(const Literal &) = delete;
//     Literal(Literal &&) = delete;

//     Value::Ptr m_value;
// };

// class Logical : public Base {
//   public:
//     Logical(Base::Ptr left, Token::Ptr op, Base::Ptr right)
//         : m_left(left), m_operator(op), m_right(right) {}

//     std::string typeName() const override { return "Logical"; }

//     Value::Ptr accept(IVisitor &visitor) override {
//         return visitor.visitLogical(*this);
//     }

//     Base::Ptr m_left;
//     Token::Ptr m_operator;
//     Base::Ptr m_right;
// };

// class Unary : public Base {
//   public:
//     Unary(Token::Ptr token, Base::Ptr right)
//         : m_operator(token), m_right(right) {}

//     ~Unary() override {}

//     Value::Ptr accept(IVisitor &visitor) override {
//         return visitor.visitUnary(*this);
//     }

//     Token::Ptr m_operator;
//     Base::Ptr m_right;
// };

// class Variable : public Base {
//   public:
//     explicit Variable(Token::Ptr name) : m_name(name) {}

//     virtual bool isVariable() const override { return true; }
//     virtual Token::Ptr getName() const override { return m_name; }

//     std::string typeName() const override { return "Variable"; }

//     Value::Ptr accept(IVisitor &visitor) override {
//         return visitor.visitVariable(*this);
//     }

//     Token::Ptr m_name;
// };
// } // namespace Expr
// } // namespace bodhi
