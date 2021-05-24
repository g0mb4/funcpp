#include <funcpp.h>

void main() {
        auto l = List::list(std::vector<int>({1, 2, 3}));

        print(l);
        print(head(l));
        print(tail(l));
        //print(concat(head(l), tail(l)));
        //print(sum(l));
}