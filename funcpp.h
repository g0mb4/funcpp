#pragma once

#include <stdio.h>

#include <memory>
#include <vector>
#include <string>

#define ILLEGAL_INSTRUCTION     "Illegal instruction."

//#define for     ILLEGAL_INSTRUCTION
//#define while   ILLEGAL_INSTRUCTION
//#define do      ILLEGAL_INSTRUCTION

class Variable {
public:
        friend void print(const std::shared_ptr<Variable>& v);
        friend int length(const std::shared_ptr<Variable>& v);
        friend bool empty(const std::shared_ptr<Variable>& v);

        friend std::shared_ptr<Variable> head(const std::shared_ptr<Variable>& v);
        friend std::shared_ptr<Variable> tail(const std::shared_ptr<Variable>& v);
        //friend std::shared_ptr<Variable> concat(const std::shared_ptr<Variable>& v1, const std::shared_ptr<Variable>& v2);
        //friend std::shared_ptr<Variable> sum(const std::shared_ptr<Variable>& v);

        enum class Type {
                Number = 0,
                List,
        };

        virtual const std::string string() const = 0;
        virtual const std::shared_ptr<Variable> value() const = 0;

        bool is_number() const { return m_type == Type::Number; }
        bool is_list() const { return m_type == Type::List; }

        Type type() const { return m_type; }

protected:
        Variable(Type type) : m_type(type) {};

        Type m_type;
};

class Number final : public Variable {
public:
        static const std::shared_ptr<Variable> number(int value) {
                return std::shared_ptr<Number>(new Number(value));
        }

        const std::string string() const override {
                return std::to_string(m_value);
        }

        int as_int() const { return m_value; }

        const std::shared_ptr<Variable> value() const override {
                return Number::number(m_value);
        }

        std::shared_ptr<Variable> add(int value) {
                return std::make_shared<Number>(m_value + value);
        }

        std::shared_ptr<Variable> add(const std::shared_ptr<Variable> & n) {
                if (n == nullptr || n->type() != Variable::Type::Number) {
                        return {};
                }

                Number* num = static_cast<Number*>(n.get());
                return std::make_shared<Number>(m_value + num->as_int());
        }

        // TODO: make this private....
        Number(int value) : Variable(Type::Number), m_value(value) {};
private:
        int m_value;
};

class List final : public Variable {
public:
        static const std::shared_ptr<Variable> list(const std::vector<int>& elements) {
                return std::shared_ptr<List>(new List(elements));
        }

        static const std::shared_ptr<Variable> list(const std::vector<std::shared_ptr<Variable>>& elements) {
                return std::shared_ptr<List>(new List(elements));
        }

        static const std::shared_ptr<Variable> list(const std::shared_ptr<Variable>& v) {
                return std::shared_ptr<List>(new List(v));
        }

        static const std::shared_ptr<Variable> empty() {
                return std::shared_ptr<List>(new List(std::vector<int>({})));
        }

        const std::string string() const override {
                std::string s = "[";
                const std::shared_ptr<Variable> l = value();
                string_impl(l, s);
                s += "]";
                return s;
        }

        const std::shared_ptr<Variable> value() const override {
                return List::list(m_elements);
        }

        const std::vector<std::shared_ptr<Variable>>& elements() const {
                return m_elements;
        }

        int length_impl() const { return (int)m_elements.size(); };

        std::shared_ptr<Variable> at(int i) const { 
                if (i < 0 || i >= m_elements.size()) {
                        return {};
                }

                return m_elements[i];
        };

private:
        List(const std::vector<int>& elements) : Variable(Type::List) {
                add_elements(elements);
        };

        List(const std::vector<std::shared_ptr<Variable>>& elements) : Variable(Type::List) {
                add_elements(elements);
        };

        List(const std::shared_ptr<Variable>& v) : Variable(Type::List) {
                add_elements(v);
        };

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

        void add_elements(const std::shared_ptr<Variable>& v) {
                switch (v->type()) {
                case Variable::Type::Number:
                        m_elements.emplace_back(v);
                        break;
                case Variable::Type::List:
                        if (length(v) > 0) {
                                List* l = static_cast<List*>(v.get());
                                auto e = l->at(0);
                                m_elements.emplace_back(e);
                                auto new_elements = tail(v);
                                add_elements(new_elements);
                        }
                        break;
                }
        }

        void string_impl(const std::shared_ptr<Variable>& list, std::string & str) const {
                if (length(list) > 0) {
                        auto v = head(list);

                        if (length(list) > 1) {
                                str += v->string() + ", ";
                        } else {
                                str += v->string();
                        }

                        string_impl(tail(list), str);
                } else {
                        return;
                }
        }

        std::vector<std::shared_ptr<Variable>> m_elements;
};

void print(const std::shared_ptr<Variable>& v) {
        printf("%s\n", v->string().c_str());
}

int length(const std::shared_ptr<Variable>& v) {
        if (v->is_number()) {
                fprintf(stderr, "Illegal operation : %s\n", __FUNCTION__);
                return 0;
        }

        List* l = static_cast<List*>(v.get());
        return l->length_impl();
}

bool empty(const std::shared_ptr<Variable>& v) {
        return length(v) == 0;
}

std::shared_ptr<Variable> head(const std::shared_ptr<Variable>& v) {
        if (v->is_number()) {
                fprintf(stderr, "Illegal operation : %s\n", __FUNCTION__);
                return nullptr;
        }

        List* l = static_cast<List*>(v.get());
        return l->at(0);
}

std::shared_ptr<Variable> tail(const std::shared_ptr<Variable>& v) {
        if (v->is_number()) {
                fprintf(stderr, "Illegal operation : %s\n", __FUNCTION__);
                return nullptr;
        }

        if (length(v) < 2 ) {
                return List::empty();
        } else {
                List* l = static_cast<List*>(v.get());

                std::vector<std::shared_ptr<Variable>> new_elements;
                for (int i = 1; i < l->length_impl(); i++) {
                        new_elements.emplace_back(l->at(i));
                }

                return List::list(new_elements);
        }
}

/*
std::shared_ptr<Variable> concat(const std::shared_ptr<Variable>& v1, const std::shared_ptr<Variable>& v2) {
        if (v1 == nullptr && v2 == nullptr) {
                return {};
        }

        if (v1 != nullptr && v2 == nullptr) {
                return List::list(v1);
        } else if (v1 == nullptr && v2 != nullptr) {
                return List::list(v2);
        }
        else {
                auto list1 = List::list(v1);
                auto list2 = List::list(v2);

                List* l1 = static_cast<List*>(list1.get());
                List* l2 = static_cast<List*>(list2.get());

                std::vector<std::shared_ptr<Variable>> new_elements;

                for (int i = 0; i < l1->length(); i++) {
                        new_elements.emplace_back(l1->at(i));
                }

                for (int i = 0; i < l2->length(); i++) {
                        new_elements.emplace_back(l2->at(i));
                }

                return List::list(new_elements);
        }
}

std::shared_ptr<Variable> sum(const std::shared_ptr<Variable>& v) {
        if (v == nullptr) {
                return 0;
        }

        switch (v->type()) {
        case Variable::Type::Number:
                return v->value();
        case Variable::Type::List:
                List* l = static_cast<List*>(v.get());
                auto sum_value = Number::number(0);

                for (int i = 0; i < l->length(); i++) {
                        sum_value = static_cast<Number*>(sum_value.get())->add(sum(l->at(i)));
                }

                return sum_value;
        }
}*/