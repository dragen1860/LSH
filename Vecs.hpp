#ifndef _vecs_hpp_
#define _vecs_hpp_

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;
 
 
class Vecs
{
        /**
         * The class read data from fvecs, ivecs, bvecs and store it in STL vector.
         * for more details, please refer to yael(http://yael.gforge.inria.fr/) library.
         */
public:  
    
    bool init(string filename,size_t num,size_t dim,vector<float>& db)
    { 
        /**
         *filename: should end with .fvecs or ivecs. we will parse it to get file type.
         * num: num of feature to get.
         * dim: dimensionality of each feature.
         * db: container of the data. You should init a corresponding
         */
        this->filename=filename;
        this->num=num;
        this->dim=dim;
        this->dataset=&db;
        
        try{
            dataset->reserve(this->num*this->dim*sizeof(float)); 
        }catch(const std::bad_alloc& err)
        {
            cerr<<"Allocate sizeof: "<<this->num*this->dim*sizeof(float)/1024/1024<<"MB Failed!!!"<<endl;
            exit(1);
        }
        filetype=filename.substr(filename.find_last_of(".")+1,filename.length()); 
        if(filetype!="fvecs" && filetype!="ivecs")
        {
            cerr<<"Unsupported type: "<<filetype<<" ! Currently supported file types: fvecs, ivecs."<<endl;
            return false;
        }
        return true;
    }; 
    
    size_t read()
    {
        /**
         *implement the read process.
         */
        size_t actual_num=0; 
        
        FILE *fp=fopen(filename.c_str(),"rb");
        if(!fp)
        {
            cerr<<filename<<" NOT found!"<<endl;            
        }  
        if(filetype=="ivecs")
        {
            char* buff=nullptr;
            try{
                buff=new char[this->num*this->dim*sizeof(int)];
            }catch(const std::bad_alloc& err)
            {
                cerr<<"Allocate sizeof: "<<this->num*this->dim*sizeof(float)/1024/1024<<"MB Failed!!!"<<endl;
                exit(1);
            }
            actual_num=xvecs_fread(sizeof(int),fp,(void*)buff,num,dim);
            dataset->clear();
            for(int i=0;i<actual_num*dim;i++)
            {
                int value=*((int*)buff+i);
                dataset->push_back(value);
            }
            delete[] buff;
            
        }
        else if(filetype=="fvecs")
        {
            char* buff=nullptr;
            try{
                buff=new char[this->num*this->dim*sizeof(float)];
            }catch(const std::bad_alloc& err)
            {
                cerr<<"Allocate sizeof: "<<this->num*this->dim*sizeof(float)/1024/1024<<"MB Failed!!!"<<endl;
                exit(1);
            }
            actual_num=xvecs_fread(sizeof(float),fp,(void*)buff,num,dim);
            dataset->clear();
            for(int i=0;i<actual_num*dim;i++)
            {
                float value=*((float*)buff+i);
                dataset->push_back(value);
            }
            delete[] buff;
        }
        fclose(fp);
        
        return actual_num;         
    };
     
    void print()
    {
        /**
         *print the first 5 lines of the achieved data.
         */
        cout<<"Num:"<<dataset->size()<<endl;
        for(int i=0;i<5;i++)
        {
            cout<<"Index:"<<i<<":";
            for(int j=0;j<dim;j++)
            {
                cout<<(*dataset)[i*dim+j]<<" ";                
            }
            cout<<endl;
        }
    };
     

    size_t flush()
    {
        dataset->clear();
        return dataset->size();
    }; 
     
    
private:
    vector<float>* dataset;
    string filename;
    string filetype;
    size_t num;
    size_t dim;
    
    
    
    long xvecs_fread (long unit_size, FILE * f, void * v, long n, int d_alloc)
    {
        /**
         *vecs read function, please refering to yael library.
         */
        long i = 0, d = -1, ret;
        for (i = 0 ; i < n ; i++) {
            if (feof (f))
            break;

            ret = xvec_fread (unit_size, f, (char *) v + unit_size * i * d, d_alloc);

            if (ret == 0)  /* eof */
            break;

            if (ret == -1) /* err */
            return 0;

            if (i == 0) {
            d = ret;
            if(d != d_alloc) 
            fprintf(stderr, "xvecs_fread: warn allocated d = %d, found d = %ld\n",
                d_alloc, d);
            }
            

            if (d != ret) {
            perror ("# xvecs_fread: dimension of the vectors is not consistent\n");
            return 0;
            }
        }
        return i;
    };
    
    int xvec_fread (long unit_size, FILE * f, void * v, int d_alloc)
    {
        
        /**
         *vecs read function, please refering to yael library.
         */
        int d;
        int ret = fread (&d, sizeof (int), 1, f);

        if (feof (f))
            return 0;

        if (ret != 1) {
            perror ("# xvec_fread error 1");
            return -1;
        }

        if (d < 0 || d > d_alloc) {
            fprintf(stderr, "xvec_fread: weird vector size (expect %d found %d)\n", d_alloc, d);
            return -1;
        }

        ret = fread (v, unit_size, d, f);
        if (ret != d) {
            perror ("# xvec_fread error 2");
            return -1;
        }

        return d;
    };
}; 
 


#endif
 
