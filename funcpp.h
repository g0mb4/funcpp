#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <vector>
#include <string>

#define ILLEGAL_INSTRUCTION     "Illegal instruction."

#define for     ILLEGAL_INSTRUCTION
#define while   ILLEGAL_INSTRUCTION
#define do      ILLEGAL_INSTRUCTION
#define goto    ILLEGAL_INSTRUCTION

#define Var     const std::shared_ptr<Variable>

#define as_number(x)    static_cast<Number*>((x).get())
#define as_list(x)      static_cast<List*>((x).get())
#define as_int(x)       as_number(x)->value()

class Variable {
private:
        friend void print(const std::shared_ptr<Variable>& v);
        friend const std::shared_ptr<Variable> length(const std::shared_ptr<Variable>& l);

        friend const std::shared_ptr<Variable> head(const std::shared_ptr<Variable>& l);
        friend const std::shared_ptr<Variable> tail(const std::shared_ptr<Variable>& l);
        friend const std::shared_ptr<Variable> append(const std::shared_ptr<Variable>& v, const std::shared_ptr<Variable>& l);
        friend const std::shared_ptr<Variable> concat(const std::shared_ptr<Variable>& l1, const std::shared_ptr<Variable>& l2);
        
        friend const std::shared_ptr<Variable> map(const std::shared_ptr<Variable> (*function)(const std::shared_ptr<Variable>& n), const std::shared_ptr<Variable>& l);
        friend const std::shared_ptr<Variable> filter(bool (*function)(const std::shared_ptr<Variable>& n), const std::shared_ptr<Variable>& l);

public:
        enum class Type {
                Number = 0,
                List,
        };

        virtual const std::string string() const = 0;

        bool is_number() const { return m_type == Type::Number; }
        bool is_list() const { return m_type == Type::List; }

        Type type() const { return m_type; }

protected:
        Variable(Type type) : m_type(type) {};

        Type m_type;
};

class Number final : public Variable {
public:
        static Var number(int value) {
                return std::make_shared<Number>(value);
        }

        const std::string string() const override {
                return std::to_string(m_value);
        }

        int value() const { return m_value; };

        Var add(int value) {
                return std::make_shared<Number>(m_value + value);
        }

        Var add(Var & n) {
                if (!n->is_number()) {
                        fprintf(stderr, "Illegal operation on a list (%s): %s\n", n->string().c_str(), __FUNCTION__);
                        exit(1);
                }

                return std::make_shared<Number>(m_value + as_int(n));
        }

        Number(int value) : Variable(Type::Number), m_value(value) {};
        
private:
        int m_value;
};

class List final : public Variable {
public:
        static Var list(const std::vector<int>& elements) {
                return std::make_shared<List>(elements);
        }

        static Var list(const std::vector<std::shared_ptr<Variable>>& elements) {
                return std::make_shared<List>(elements);
        }

        static Var list(Var& v) {
                return std::make_shared<List>(v);
        }

        static Var list(int start, int end) {
                return std::make_shared<List>(start, end);
        }

        static Var empty() {
                return std::make_shared<List>(std::vector<int>({}));
        }

        const std::string string() const override {
                std::string s = "[";
                Var l = List::list(m_elements);
                string(l, s);
                s += "]";
                return s;
        }

        std::vector<std::shared_ptr<Variable>> elements() const {
                return m_elements;
        }

        int size() const { return (int)m_elements.size(); };

        Var first() const { 
                if(m_elements.size() == 0){
                        fprintf(stderr, "Illegal operation on a list (%s): %s, it's empty\n", string().c_str(), __FUNCTION__);
                        exit(1);
                }

                return m_elements[0];
        };

        Var others() const {
                std::vector<std::shared_ptr<Variable>> elements = m_elements;
                elements.erase(elements.begin());
                return List::list(elements);
        }

        Var append(Var & v) const {
                std::vector<std::shared_ptr<Variable>> elements = m_elements;
                elements.emplace_back(v);
                return List::list(elements);
        }

        List(const std::vector<int>& elements) : Variable(Type::List) {
                add_elements(elements);
        };

        List(const std::vector<std::shared_ptr<Variable>>& elements) : Variable(Type::List) {
                add_elements(elements);
        };

        List(Var & v) : Variable(Type::List) {
                add_elements(v);
        };

        List(int start, int end) : Variable(Type::List) {
                add_elements(start, end);
        };

private:
        void add_elements(const std::vector<int>& elements) {
                if (elements.size() > 0) {
                        int e = elements.at(0);
                        m_elements.emplace_back(Number::number(e));
                        auto new_elements = elements;
                        new_elements.erase(new_elements.begin());
                        add_elements(new_elements);
                }
        }

        void add_elements(const std::vector<std::shared_ptr<Variable>>& elements) {
                if (elements.size() > 0) {
                        auto e = elements.at(0);
                        m_elements.emplace_back(e);
                        auto new_elements = elements;
                        new_elements.erase(new_elements.begin());
                        add_elements(new_elements);
                }
        }

        void add_elements(Var & v) {
                if(v->is_number()){
                        m_elements.emplace_back(v);
                        return;
                } 

                if (as_int(length(v)) > 0) {
                        auto l = as_list(v);
                        auto e = l->first();
                        m_elements.emplace_back(e);
                        auto new_elements = tail(v);
                        add_elements(new_elements);
                }
        }

        void add_elements(int start, int end){
                if(start != end + 1){
                       m_elements.emplace_back(Number::number(start));
                       add_elements(start + 1, end);
                }
        }

        void string(Var & l, std::string & str) const {
                if (as_int(length(l)) > 0) {
                        auto v = head(l);

                        if (as_int(length(l)) > 1) {
                                str += v->string() + ", ";
                        } else {
                                str += v->string();
                        }

                        string(tail(l), str);
                }
        }

        std::vector<std::shared_ptr<Variable>> m_elements;
};

void print(Var & v) {
        printf("%s\n", v->string().c_str());
}

const std::shared_ptr<Variable> length(Var & l) {
        if (l->is_number()) {
                fprintf(stderr, "Illegal operation on a number (%s): %s\n", l->string().c_str(), __FUNCTION__);
                exit(1);
        }

        return Number::number(as_list(l)->size());
}

static bool empty(Var & l) {
        if (l->is_number()) {
                fprintf(stderr, "Illegal operation on a number (%s): %s\n", l->string().c_str(), __FUNCTION__);
                exit(1);
        }

        return as_int(length(l)) == 0;
}

Var head(Var & l) {
        if (l->is_number()) {
                fprintf(stderr, "Illegal operation on a number (%s): %s\n", l->string().c_str(), __FUNCTION__);
                exit(1);
        }

        return as_list(l)->first();
}

Var tail(Var & l) {
        if (l->is_number()) {
                fprintf(stderr, "Illegal operation on a number (%s): %s\n", l->string().c_str(), __FUNCTION__);
                exit(1);
        }

        if (as_int(length(l)) < 2) {
                return List::empty();
        } else {
                return as_list(l)->others();
        }
}

Var append(Var & v, Var & l){
        if(!l->is_list()){
                fprintf(stderr, "Second parameter (%s) is not a list in: %s\n", l->string().c_str(), __FUNCTION__);
                exit(1);
        }

        if(empty(l)){
                return List::list(v);
        } else {
                auto list = as_list(l);

                if(list->first()->type() != v->type()){
                        fprintf(stderr, "Incompatible types to append: %s + %s\n", l->string().c_str(), v->string().c_str());
                        exit(1);
                }

                return list->append(v);
        }
}

Var concat(Var & l1, Var & l2) {
        if(empty(l1) && empty(l2)){
                return List::empty();
        } else if(empty(l1) && !empty(l2)){
                return List::list(l2);
        } else if(!empty(l1) && empty(l2)){
                return List::list(l1);
        } else {
                auto l = List::list(l1);
                auto h = head(l2);
                return concat(append(h, l), tail(l2));
        }
}

static Var map_impl(Var (*function)(Var & v), Var & l, Var & r){
        if(as_int(length(l)) > 0){
                auto v = function(head(l));
                return map_impl(function, tail(l), append(v, r));
        } else {
                return r;
        }
}

Var map(Var (*function)(Var & n), Var & l){
        if(!l->is_list()){
                fprintf(stderr, "Second parameter (%s) is not a list in: %s\n", l->string().c_str(), __FUNCTION__);
                exit(1);
        }

        if(empty(l)){
                return List::empty();
        }

        if(!as_list(l)->first()->is_number()){
                fprintf(stderr, "List (%s) is not a list of numbers: %s\n", l->string().c_str(), __FUNCTION__);
                exit(1);
        }

        return map_impl(function, l, List::empty());
}

static Var filter_impl(bool (*function)(Var & n), Var & l, Var & r){
        if(as_int(length(l)) > 0){
                if(function(head(l))){
                        return filter_impl(function, tail(l), append(head(l), r));
                } else {
                        return filter_impl(function, tail(l), r);
                }
        } else {
                return r;
        }
}

Var filter(bool (*function)(Var & n), Var & l){
        if(!l->is_list()){
                fprintf(stderr, "Second parameter (%s) is not a list in: %s\n", l->string().c_str(), __FUNCTION__);
                exit(1);
        }

        if(empty(l)){
                return List::empty();
        }

        if(!as_list(l)->first()->is_number()){
                fprintf(stderr, "List (%s) is not a list of numbers: %s\n", l->string().c_str(), __FUNCTION__);
                exit(1);
        }

        return filter_impl(function, l, List::empty());       
}
