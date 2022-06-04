#include <iostream>
#include <memory>

int main (){
    std::unique_ptr<int> pj(new int(42));

    std::cout << *pj << std::endl;

    
}