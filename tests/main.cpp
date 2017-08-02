
#include <cstdio>

#include <math/math.hpp>

void test_renderer();
void test_widgets();

int main() {
    math::init();
    test_renderer();
    test_widgets();
    puts("OK");
}
