#ifndef _Matrix_hpp_
#define _Matrix_hpp_

#include <iostream>
#include <random>
#include <memory>
#include <string.h>
#include <time.h>
using namespace std;
 
class Matrix
{
    /**
     *Here implement a rather simple Matrix class to store xnum*ynum data.
     */
public: 
    bool init(size_t xnum,size_t ynum) 
    {
        this->xnum=xnum;
        this->ynum=ynum;
        try{
            pdata=new float[xnum*ynum];
        }catch(const std::bad_alloc& err)
        {            
            cerr<<"Allocate sizeof: "<<xnum*ynum*sizeof(float)/1024/1024<<"MB Failed!!!"<<endl;
            exit(1);
        }
        memset(pdata,0,xnum*ynum*sizeof(float));
        return true;
    }; 
    
    float* operator[](size_t index)
    {
        /**
         *return the first element of index row, NOT a line object.
         * you should take care the columns of line by yourself. It's just a pointer.
         */
        return pdata+index*xnum;
    };
    
    float* ptr()
    {
        return pdata;
    };
    
    void random(size_t mean,float variance)
    {        
        /**
         *Gerenate a Guassian Distribution according to mean and variance given.
         */
        default_random_engine engine;
        engine.seed(rand());
        normal_distribution<float> dist(mean,variance);
        for(int i=0;i<xnum;i++)
        {            
            for(int j=0;j<ynum;j++)
            { 
                *(pdata+i*xnum+j)=dist(engine);
            }          
        }
        
    };
    void toString()
    {
        cout<<"xnum:"<<xnum<<" ynum:"<<ynum<<endl;
        for(int i=0;i<xnum;i++)
        {            
            for(int j=0;j<ynum;j++)
            {
                cout<<*(pdata+xnum*i+j)<<" ";
            }
            cout<<endl;            
        }
                
    };
    size_t getXnum()
    {
        return xnum;
    }
    size_t getYnum()
    {
        return ynum;
    }
    
    ~Matrix()
    { 
        /**
         *We need to take care of the class span on STL container.
         */
        //delete[] pdata;  ???????????????
    };
    
private:
    size_t xnum,ynum;
    float* pdata;
    
};































#endif
