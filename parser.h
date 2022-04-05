//
// Created by Savin Dmitry on 20.02.2022.
//

#ifndef PARSER_H
#define PARSER_H

#include <stack>
#include <vector>
#include <string>
#include <cstdint>

using namespace std;

struct Node {
  virtual double Evaluate() const = 0;

  virtual void Increase(char value) {}
};

struct Value : public Node {
  explicit Value(char digit) : _value(digit - '0') {}

  explicit Value(double val) : _value(val) {}

  double Evaluate() const override { return _value; }

  void Increase(char digit) {
    _value *= 10;
    _value += (digit - '0');
  }

 private:
  double _value;
};

struct Variable : public Node {
  explicit Variable(const double& x) : _x(x) {}

  double Evaluate() const override { return _x; }

 private:
  const double& _x;
};

struct Op : public Node {
  explicit Op(const string& value, const int& skobka)
      : precedence([value, skobka] {
    if (value == "'") {
      return 4 + skobka;
    } else if (value == "cos" || value == "sin" || value == "log"
        || value == "ctg" || value == "tg") {
      return 3 + skobka;
    } else if (value == "*" || value == "/") {
      return 2 + skobka;
    } else {
      return 1 + skobka;
    }
  }()),
        _op(value) {}

  const uint8_t precedence;

  double Evaluate() const override {
    if (_op == "*") {
      return _left->Evaluate() * _right->Evaluate();
    } else if (_op == "+") {
      return _left->Evaluate() + _right->Evaluate();
    } else if (_op == "-") {
      return _left->Evaluate() - _right->Evaluate();
    } else if (_op == "/") {
      return _left->Evaluate() / _right->Evaluate();
    } else if (_op == "cos") {
      return _left->Evaluate() + cos(_right->Evaluate());
    } else if (_op == "sin") {
      return _left->Evaluate() + sin(_right->Evaluate());
    } else if (_op == "'") {
      return pow(_left->Evaluate(), _right->Evaluate());
    } else if (_op == "log") {
      return _left->Evaluate() + log(_right->Evaluate());
    } else if (_op == "ctg") {
      return _left->Evaluate() + 1.0 / tan(_right->Evaluate());
    } else if (_op == "tg") {
      return _left->Evaluate() + tan(_right->Evaluate());
    }

    return 0;
  }

  void SetLeft(const shared_ptr<Node>& node) { _left = node; }
  void SetRight(const shared_ptr<Node>& node) { _right = node; }

 private:
  const string _op;
  shared_ptr<const Node> _left, _right;
};

template<class Iterator>
shared_ptr<Node> Parse(Iterator token, Iterator end, const double& x) {
  // Empty expression
  if (token == end) {
    return make_shared<Value>('0');
  }

  stack<shared_ptr<Node>> values;
  stack<shared_ptr<Op>> ops;

  auto PopOps = [&](int precedence) {
    while (!ops.empty() && ops.top()->precedence >= precedence) {
      auto value1 = values.top();
      values.pop();
      auto value2 = values.top();
      values.pop();
      auto op = ops.top();
      ops.pop();

      op->SetRight(value1);
      op->SetLeft(value2);

      values.push(op);
    }
  };

  bool num = false;

  int skobka = 0;

  while (token != end) {
    const auto& value = *token;
    if (value >= '0' && value <= '9') {
      if (!num) {
        values.push(make_shared<Value>(value));
        num = true;
      } else {
        values.top()->Increase(value);
      }
    } else if (value == 'e') {
      values.push(make_shared<Value>(M_E));
      num = false;
    } else if (value == 'p') {
      ++token;
      values.push(make_shared<Value>(M_PI));
      num = false;
    } else if (value == 'x') {
      values.push(make_shared<Variable>(x));
      num = false;
    } else if (value == '*' || value == '/') {
      PopOps(2 + skobka);
      string s(1, value);
      ops.push(make_shared<Op>(s, skobka));
      num = false;
    } else if (value == '+' || value == '-') {
      PopOps(1 + skobka);
      string s(1, value);
      ops.push(make_shared<Op>(s, skobka));
      num = false;
    } else if (value == 'c') {
      PopOps(3 + skobka);
      ++token;
      values.push(make_shared<Value>('0'));
      if (*token == 'o') {
        ops.push(make_shared<Op>("cos", skobka));
      } else if (*token == 't') {
        ops.push(make_shared<Op>("ctg", skobka));
      }
      ++token;
      num = false;
    } else if (value == 's') {
      PopOps(3 + skobka);
      ++token;
      ++token;
      values.push(make_shared<Value>('0'));
      ops.push(make_shared<Op>("sin", skobka));
      num = false;
    } else if (value == 't') {
      PopOps(3 + skobka);
      ++token;
      values.push(make_shared<Value>('0'));
      ops.push(make_shared<Op>("tg", skobka));
      num = false;
    } else if (value == 'l') {
      PopOps(3 + skobka);
      ++token;
      ++token;
      values.push(make_shared<Value>('0'));
      ops.push(make_shared<Op>("log", skobka));
      num = false;
    } else if (value == '\'') {
      PopOps(4 + skobka);
      string s(1, value);
      ops.push(make_shared<Op>(s, skobka));
      num = false;
    } else if (value == '(') {
      skobka = 5;
      num = false;
    } else if (value == ')') {
      skobka = 0;
      num = false;
    } else if (value == ' ') {}

    ++token;
  }

  while (!ops.empty()) {
    PopOps(0);
  }

  return values.top();
}


#endif //PARSER_H
