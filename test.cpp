#include <iostream>

using namespace std;

int main() {
    unsigned int a = 0b00000010;
    bool b = false;
    cout << (a << 1 | b) << endl;
}