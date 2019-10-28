#include <stdio.h>

int main(int, char**)
{
    enum class Test {
        x,
        y,
        z,
    };

    printf("%d\n", Test::x);
    Test x = Test::x;
    printf("%d\n", x);
}
