#include <iostream>
#include <string>
#include <cstring>

using namespace std;

int main(int argc, char const *argv[])
{
    char buf[] = "hello world";
    char foo[256] = {0};
    string s(buf);
    strcpy(foo, s.c_str());
    foo[0] = 'j';
    cout<<foo<<endl;
    return 0;
}
