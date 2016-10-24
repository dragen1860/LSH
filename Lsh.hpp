#ifndef _Lsh_hpp_
#define _Lsh_hpp_

#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <assert.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <time.h>
using namespace std; 
#include "Matrix.hpp"


typedef vector<float>::iterator Point;
typedef pair<Point,size_t> HashItem;
typedef unordered_multimap<string,HashItem> Table;
 
class QueryResult_t{
public:
    size_t index;
    float dist;
    QueryResult_t(size_t index,float dist):index(index),dist(dist){};
    
    bool operator==(const QueryResult_t& b) const
    {
        return index==b.index;
        
    };
    
};

//hashing function for unordered_set.
namespace std{
    template<>
    struct hash<QueryResult_t>
    {
    size_t  operator()(const QueryResult_t& obj) const
    {
        return hash<int>()(obj.index);
    }
    }; 
} 

class Lsh
{
    /**
     *The class implement a standard LSH alogithm.
     * It generate tablenum random plane to separate each Points.
     */
public:
    bool init(string path,size_t dbnum,size_t qdbnum,size_t gtk,size_t dim, size_t tablenum,size_t hashsize)
    {
        /**
         *path: the path including your dataset. for instance, you have 3 files:[sift_base.fvecs, sift_groundtruth.ivecs, sift_query.fvecs]
         *  in path /home/i/db/sift/, so the path is: /home/i/db/sift/, we will use the nearest directory name to generate each file name.
         * dbnum: the number of db features.
         * qdbnum: the number of query db features.
         * gtk: nearest number of goundtruth of each feature.
         * dim: dimensionality of feature.
         * tablenum: Lsh hash table number.
         * hashsize: the keysize, namely the number of separation planes.
         */
        this->path=path;
        this->dbnum=dbnum;
        this->qdbnum=qdbnum;
        this->gtk=gtk;
        this->tablenum=tablenum;
        this->hashsize=hashsize;
        this->dim=dim;
        
        planes.clear();
        tables.clear();
        
        for(int i=0;i<tablenum;i++)
        {
            Matrix* mat=new Matrix();
            mat->init(hashsize,dim);
            planes.push_back(*mat);
            
            Table tab;
            tables.push_back(tab);
        }
        initRandomPlanes();
        
        Vecs vecs;
        vecs.init(path+"_base.fvecs",dbnum,dim,db);
        int actual_num=vecs.read();
        cout<<"Readed "<<actual_num<<" from db, vec size:"<<db.size()<<endl;
        
        vecs.init(path+"_query.fvecs",qdbnum,dim,qdb);
        actual_num=vecs.read();
        cout<<"Readed "<<actual_num<<" from query db, vec size:"<<qdb.size()<<endl; 
        
        vecs.init(path+"_groundtruth.ivecs",qdbnum,gtk,gtdb);
        actual_num=vecs.read();
        cout<<"Readed "<<actual_num<<" from gt db, vec size:"<<gtdb.size()<<endl; 
        return true;
    };
    
    void build_index()
    {
        /**
         *build index of read dataset.
         */
        int counter=0;
        auto point=db.begin();
        while(point!=db.end())
        {
            index(point,counter++); 
            //NOT point++, you should add dimensionality len.
            point+=dim;            
        }
        counter=0;
        for(auto table:tables)
        {
            cout<<"Table:"<<counter++<<" bucket_count:"<<table.bucket_count()<<" total size:"<<table.size()<<" build_index done."<<endl;
        }
    };
    
    void index(Point point,size_t indexnum)
    { 
        /**
         *build index for single point.
         */ 
        
        //you must use auto& to modify the content of container.
        int plane=0;
        for(auto& table:tables)
        {
            string key=hash(planes[plane++],point);
            pair<string,HashItem> item(key,make_pair(point,indexnum));
            table.insert(item);    
        }
    };
    
    string hash(Matrix& plane,Point point)
    {
        /**
         *generate hash key for corresponding point and its plane. 
         */
        size_t xnum=plane.getXnum();
        size_t ynum=plane.getYnum(); 
        string key;
        
        for(int i=0;i<xnum;i++)
        {
            float*  p=plane[i];
            float   value=0;
            for(int j=0;j<ynum;j++)
            {
                value+=(*p++)*point[j];
            }
            if(value>0)key+="1";
            else key+="0";
        }
        return key;
    };
    
    
    vector<QueryResult_t> query(Point point,size_t k)
    {
        /**
         *query operation of Point point and get nearest k point.
         * point: query point
         * k: nearest k
         * vector<>: contain the index and distance of each nearest k Neighbors.
         */
        int plane=0;
        unordered_set<QueryResult_t> result;
        current_point=point;
        
        for(auto table:tables)
        {
            string key=hash(planes[plane],point);
            int  bucketnum=table.count(key);
            //cout<<"Table:"<<plane<<" Key:"<<key<<" Accurate buck size:"<<bucketnum<<endl;
            auto range=table.equal_range(key);
            auto beg=range.first;
            auto end=range.second;
            while(beg!=end)
            { 
                QueryResult_t qr(beg->second.second,l2_dist(current_point,beg->second.first));
                result.insert(qr);                 
                beg++;                    
            }   
            
            for(int i=0;i<hashsize;i++)
            {
                string neighbor_key=key;
                neighbor_key.replace(i,1,(key[i]=='1'?"0":"1"));
             
                auto range=table.equal_range(neighbor_key);
                int  neighborbucketnum=table.count(neighbor_key);
                //cout<<"Table:"<<plane<<" Key:"<<neighbor_key<<" Neighbor buck size:"<<neighborbucketnum<<endl;
                auto beg=range.first;
                auto end=range.second;
                while(beg!=end)
                { 
                    result.insert(QueryResult_t(beg->second.second,l2_dist(current_point,beg->second.first))); 
                    beg++;                    
                }    
            }
            
            plane++;
        }
            
        vector<QueryResult_t> result2;
        try{
            for(auto it:result)result2.push_back(it);
        }catch(const std::bad_alloc& err)
        {
            cerr<<"Copy result Out of memory!"<<endl;
            exit(1);
        }
        
        cout<<"total result size:"<<result2.size()<<endl;
        
        
        
        clock_t time1=clock();
        sort(result2.begin(),result2.end(),
             [](const QueryResult_t& a,const QueryResult_t& b){return a.dist<b.dist;});
        clock_t time2=clock();
        cout<<"sort time:"<<(float)(time2-time1)/CLOCKS_PER_SEC<<endl;
        
        if(result2.size()>k)
        {
            vector<QueryResult_t> result3(result2.begin(),result2.begin()+k); 
            return result3;
        }
        else
        {
            return result2;
        } 
    };
    
    float queryAll(size_t num=10,size_t topk=10)
    { 
        /**
         *query all of num points from query dataset.
         * num: number of points to query 
         * topk: precision calculated based on topk number of Neighbors.
         * return: average precision.
         */
        if(num>(qdb.size()/dim))
        {
            num=(qdb.size()/dim);
        }
        auto point=qdb.begin();
        float avg_precision=0;
        for(int counter=0;counter<num;counter++)
        { 
 
            auto result=query(point,topk);
            float precision=calPrecision(result,counter);
            precision/=topk;
            cout<<"Point: "<<counter<<" Precision: "<<precision<<endl<<endl;
            avg_precision+=precision;
            point+=dim;
        }
        avg_precision/=num;
        cout<<">>>>>>>>>>> AVG_precision: "<<avg_precision<<" <<<<<<<<<<<<<"<<endl;
        return avg_precision;
    };
    
    float calPrecision(vector<QueryResult_t>& result,size_t index)
    {
        /**
         *calculate each point's precision. we will compare the index of point instead of the feature itself.
         * result: the query result containing index of each point.
         * index: the index of current query point, we will get its goundtruth neighbors by index.
         * return: accumulated precision, we will divide it by topk.
         */
        float precision=0;
        int i=0;
        for(auto it:result)
        {
            /*
            cout<<"dist:"<<it.dist<<" vs "<<l2_dist(current_point,db.begin()+gtdb[index*gtk+i]*dim)<<" find:"<<it.index<<" gt:"<<gtdb[index*gtk+i]<<" pos:"<<i<<endl;
            */
            if(it.index==gtdb[index*gtk+i])
            {
                precision++;
            }
            i++;
        } 
        return precision;
    };
    
    
    
    
private:
    size_t tablenum;
    size_t hashsize;
    size_t dim;
    string path;
    size_t dbnum,qdbnum,gtk;
    Point current_point;
    vector<float> db,qdb,gtdb; 
    vector<Matrix> planes;
    vector<unordered_multimap<string,HashItem > > tables;
    
    
    void initRandomPlanes()
    {
        //generate random tablenum*keysize plane;
       for(auto iter:planes)
       {
           iter.random(0,1);  
        }
    };
    
 
    
    float l2_dist(Point a,Point b)
    {
        //Euclidean distance
        int i=0;
        float value=0; 
        while(i<dim)
        {
        
           value+=(a[i]-b[i])*(a[i]-b[i]);
           i++;
        }
        return sqrt(value);
    };
};





#endif
