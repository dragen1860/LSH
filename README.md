#Locality Sensitive Hashing
implemented via c++.


```
cd yourpath
vi main.cpp #modify the dataset path
mkdir build
cd build
cmake .
make -j12
./lsh

```

Program has been tested on [SIFT1M](http://corpus-texmex.irisa.fr/) AND [GIST1M](http://corpus-texmex.irisa.fr/) dataset.

The program output will like:
```
Readed 1000000 from db, vec size:128000000
Readed 10000 from query db, vec size:1280000
Readed 10000 from gt db, vec size:1000000
Table:0 bucket_count:1056323 total size:1000000 build_index done.
Table:1 bucket_count:1056323 total size:1000000 build_index done.
Table:2 bucket_count:1056323 total size:1000000 build_index done.
Table:3 bucket_count:1056323 total size:1000000 build_index done.
Table:4 bucket_count:1056323 total size:1000000 build_index done.
Table:5 bucket_count:1056323 total size:1000000 build_index done.
total result size:573166
sort time:0.207287
Point: 0 Precision: 0.45

total result size:695341
sort time:0.255467
Point: 1 Precision: 0.96

total result size:244936
sort time:0.084784
Point: 2 Precision: 1

total result size:679177
sort time:0.246404
Point: 3 Precision: 1

total result size:357057
sort time:0.126274
Point: 4 Precision: 0.06

total result size:582624
sort time:0.212659
Point: 5 Precision: 0.1

total result size:727292
sort time:0.269344
Point: 6 Precision: 1

total result size:166486
sort time:0.056165
Point: 7 Precision: 0.24

total result size:467258
sort time:0.166909
Point: 8 Precision: 1

total result size:470236
sort time:0.169878
Point: 9 Precision: 0.6

>>>>>>>>>>> AVG_precision: 0.641 <<<<<<<<<<<<<
Build Index time:45.3007s
Query time:39.6068s
```
