#include <iostream>

class TestClass{
public:
    std::string ival;
    TestClass();
    void PrintFunc();
    void PrintFunc(std::string );
private:
    std::string str;
};

TestClass::TestClass(){
    ival = "999999999";
    str = "abcdefg";
    std::cout <<str<<"this is a constructor\n";
}
void TestClass::PrintFunc()
{
    std::cout <<str<<"\tthis is a printfunction\n";
}
void TestClass::PrintFunc(std::string str1)
{    
    std::cout <<str<<"\tthis is overridden,and print private variable str\n";
    std::cout <<str1<<"\tthis is overridden\n";
    std::cout <<this<<"\tPrintFunc(std::string str1) address\n";
}

int main(void){
    TestClass test;
    //test.PrintFunc("xxxxxxx");
    test.PrintFunc();
    test.PrintFunc(test.ival);
    //std::cout <<<<"\n";
    return 0;
}
