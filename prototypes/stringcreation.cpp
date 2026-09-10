#include <iostream>
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{
    for (int i = 68; i <= 90; i++)
    {
        
        string s = {(char)i};
        s = s + ":\\example.txt";
        cout<<s<<endl;
    }
    
    return 0;
}
