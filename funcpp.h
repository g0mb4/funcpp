#pragma once

#include <stdio.h>

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

class Variable {
public:
        friend void print(const std::shared_ptr<Variable>& v);
        friend int length(const std::shared_ptr<Variable>& v);
        friend bool empty(const std::shared_ptr<Variable>& v);

        friend const std::shared_ptr<Variable> head(const std::shared_ptr<Variable>& v);
        friend const std::shared_ptr<Variable> tail(const std::shared_ptr<Variable>& v);
        friend const std::shared_ptr<Variable> append(const std::shared_ptr<Variable>& v, const std::shared_ptr<Variable>& l);
        friend const std::shared_ptr<Variable> concat(const std::shared_ptr<Variable>& l1, const std::shared_ptr<Variable>& l2);
        
        friend const std::shared_ptr<Variable> map(const std::shared_ptr<Variable> (*function)(const std::shared_ptr<Variable>& v), const std::shared_ptr<Variable>& l);
        friend const std::shared_ptr<Variable> filter(bool (*function)(const std::shared_ptr<Variable>& v), const std::shared_ptr<Variable>& l);

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

        int as_int() const { return m_value; };

        Var add(int value) {
                return std::make_shared<Number>(m_value + value);
        }

        Var add(Var & n) {
                if (!n->is_number()) {
                        return nullptr;
                }

                auto num = as_number(n);
                return std::make_shared<Number>(m_value + num->as_int());
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
                string_impl(l, s);
                s += "]";
                return s;
        }

        std::vector<std::shared_ptr<Variable>> elements() const {
                return m_elements;
        }

        int length_impl() const { return (int)m_elements.size(); };

        Var at(int i) const { 
                if (i < 0 || i >= m_elements.size()) {
                        return nullptr;
                }

                return m_elements[i];
        };

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

        void append_elements(Var & v, int index){
                if(index < length(v)){
                        auto list = as_list(v);
                        m_elements.emplace_back(list->at(index));
                        append_elements(v, index + 1);
                }
        }
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

                if (length(v) > 0) {
                        auto l = as_list(v);
                        auto e = l->at(0);
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

        void string_impl(Var & list, std::string & str) const {
                if (length(list) > 0) {
                        auto v = head(list);

                        if (length(list) > 1) {
                                str += v->string() + ", ";
                        } else {
                                str += v->string();
                        }

                        string_impl(tail(list), str);
                }
        }

        std::vector<std::shared_ptr<Variable>> m_elements;
};

void print(Var & v) {
        printf("%s\n", v->string().c_str());
}

int length(Var & v) {
        if (v->is_number()) {
                fprintf(stderr, "Illegal operation on a number (%s): %s\n", v->string().c_str(), __FUNCTION__);
                return 0;
        }

        auto l = as_list(v);
        return l->length_impl();
}

bool empty(Var & v) {
        if (v->is_number()) {
                fprintf(stderr, "Illegal operation on a number (%s): %s\n", v->string().c_str(), __FUNCTION__);
                return 0;
        }

        return length(v) == 0;
}

Var head(Var & v) {
        if (v->is_number()) {
                fprintf(stderr, "Illegal operation on a number (%s): %s\n", v->string().c_str(), __FUNCTION__);
                return nullptr;
        }

        auto l = as_list(v);
        return l->at(0);
}
Var tail(Var & v) {
        if (v->is_number()) {
                fprintf(stderr, "Illegal operation on a number (%s): %s\n", v->string().c_str(), __FUNCTION__);
                return nullptr;
        }

        if (length(v) < 2) {
                return List::empty();
        } else {
                auto new_list = List::empty();
                auto l = as_list(new_list);

                l->append_elements(v, 1);

                return new_list;
        }
}

Var append(Var & v, Var & l){
        if(!l->is_list()){
                fprintf(stderr, "Second parameter (%s) is not a list in: %s\n", l->string().c_str(), __FUNCTION__);
                return nullptr;
        }

        if(empty(l)){
                return List::list(v);
        } else {
                auto list = as_list(l);

                if(list->at(0)->type() != v->type()){
                        fprintf(stderr, "Incompatible types to append: %s + %s\n", l->string().c_str(), v->string().c_str());
                        return nullptr;
                }

                auto e = list->elements();
                e.emplace_back(v);

                return List::list(e);
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

static Var map_impl(Var (*function)(Var & v), Var & l, Var & res){
        if(!empty(l)){
                auto v = function(head(l));
                return map_impl(function, tail(l), append(v, res));
        } else {
                return res;
        }
}

Var map(Var (*function)(Var & v), Var & l){
        if(!l->is_list()){
                fprintf(stderr, "Second parameter (%s) is not a list in: %s\n", l->string().c_str(), __FUNCTION__);
                return nullptr;
        }

        return map_impl(function, l, List::empty());
}

static Var filter_impl(bool (*function)(Var & v), Var & l, Var & res){
        if(!empty(l)){
                if(function(head(l))){
                        return filter_impl(function, tail(l), append(head(l), res));
                } else {
                        return filter_impl(function, tail(l), res);
                }
        } else {
                return res;
        }
}

Var filter(bool (*function)(Var & v), Var & l){
        if(!l->is_list()){
                fprintf(stderr, "Second parameter (%s) is not a list in: %s\n", l->string().c_str(), __FUNCTION__);
                return nullptr;
        }

        return filter_impl(function, l, List::empty());       
}
