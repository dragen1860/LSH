#include <iostream>
#include <vector>
#include <time.h>
#include "Vecs.hpp"
#include "Matrix.hpp"
#include "Lsh.hpp"
using namespace std;

 

int main(int argc, char **argv) {   
    
    
    Lsh lsh;
    auto time1=clock();
    //Modify the datset path to your.
    lsh.init("/home/llq/lsh/sift/sift",1000000,10000,100,128,6,12);
    lsh.build_index();
    auto time2=clock();
    lsh.queryAll(10,100);
    auto time3=clock();
    
    
    cout<<"Build Index time:"<<(float)(time2-time1)/CLOCKS_PER_SEC<<"s"<<endl;
    
    cout<<"Query time:"<<(float)(time3-time2)/CLOCKS_PER_SEC<<"s"<<endl;
    
    
    return 0;
}
