# funcpp

Functional C++, it's fun.

This experiment shows how to use C++ as a functional language. Loops (**for**, **while**) and jumps (**goto**) are not allowed.

# Types

The fundamental type is **Var** (*v*). It is internally either a numberic value (**Number**, *n*) or a list (**List**, *l*).

## Creation

```c++
Var n = Number::number(2);  // = 2.
Var l1 = List::list(std::vector<int>({1, 2, 3})); // = [1, 2, 3]
Var l2 = List::list(1, 3);  // = [1, 2, 3]
Var l3 = List::empty();     // = []
```

## Helpers

```c++
int i = as_int(v);     // interprets v as an int
// For internal use only:
auto l = as_list(v);    // interprets v as a list
auto n = as_number(v);  // interprets v as a number
```

# Functions

The functions operate on the **Var** types. The correctness of the types is checked during run-time.

Generic functions:

+ ```print(v)``` - prints the string representation of ```v``` to the sandard output

List operations:

+ ```length(l)``` - a number with the value of the number of elements of ```l```
+ ```head(l)``` -  the first element of ```l```
+ ```tail(l)``` - a list containing the elements of ```l``` excluding the first element
+ ```append(n, l)``` - a list created by appending ```n``` to the end of ```l```
+ ```concat(l1, l2)``` - a list containing the elements of ```l1``` and ```l2```

Arithmetic operation:

+ ```add(n1, n2)``` - a number with the value of ```n1``` + ```n2```
+ ```sub(n1, n2)``` - a number with the value of ```n1``` - ```n2```
+ ```mult(n1, n2)``` - a number with the value of ```n1``` * ```n2```
+ ```idiv(n1, n2)``` - a number with the value of ```n1``` / ```n2```, where '/' is the integer division

Higher-order functions:

+ ```map(f(n), l)``` - a list created by applying ```f``` to every element of ```l```
+ ```filter(f(n), l)``` - a list from ```l``` where ```f``` is true for every element of this list 

# Examples

Demo:

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

Filtering out prime numbers:

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