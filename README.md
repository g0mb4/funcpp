# funcpp

Functional C++, it's fun.

This experiment shows how to use C++ as a functional language. Loops (for, while) and jumps (goto) are not allowed.

## Examples
```c++
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
```

```c++
#include <funcpp.h>

bool is_prime(int n, int i = 2){
    if(n <= 1){
        return false;
    } else if(n == 2){
        return true;
    } else if(i * i > n){
        return true;
    } else if(n % i == 0){
        return false;
    } else {
        return is_prime(n, i + 1);
    }
}

bool prime(Var & value){
    return is_prime(as_int(value));
}

int main(){
    Var l = List::list(1, 100);

    print(l);
    print(filter(prime, l));

    return 0;
}
```