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
    return is_prime(as_number(value)->as_int());
}

int main(){
    Var l = List::list(1, 30);

    print(l);
    print(filter(prime, l));

    return 0;
}