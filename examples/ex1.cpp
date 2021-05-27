#include <funcpp.h>

Var add2(Var & value){
        return add(value, Number::number(2));
}

bool odd(Var & value){
        return as_int(value) % 2 != 0;
}

int main() {
        auto l = List::list(std::vector<int>({1, 2, 3}));

        print(l);
        print(head(l));
        print(tail(l));
        print(concat(List::list(head(l)), tail(l)));
        print(map(add2, l));
        print(filter(odd, l));

        return 0;
}