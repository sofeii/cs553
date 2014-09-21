//
//  memory.cpp
//  meomory
//
//  Created by 王一 on 9/13/14.
//  Copyright (c) 2014 王一. All rights reserved.
//

#include <iostream>
#include <time.h>
#include <math.h>
#include <string>
#include <sys/time.h>
#include <pthread.h>
long c_start, c_end;
int THREAD=1;
int RANDOM=0;
int numOps=0;
size_t size;
long average=0.0;
double result_1=0.0;
const size_t B = 1;
const size_t KB = 1024;
const size_t MB = 1048576;
const int CHUNK_SIZE = 1048;
struct timeval tv;
int r,n=0;
char l=NULL;
int sum = 0; //定义全局变量，让所有线程同时写，这样就需要锁机制
pthread_mutex_t sum_mutex; //互斥锁
using namespace std;

void sequential(void* lowptr, void* highptr, size_t size){
    char *ptr1, *ptr2;
    ptr1 = (char*)lowptr;
    ptr2 = ptr1+size;
    
    for(int i=0; i<numOps; i++){
        ptr1 = (char*)memcpy(ptr1, ptr2, size);
        ptr2 += size;
    }
    
    return;
}

void random_access(void* lowptr, void* highptr, size_t size){
    char *ptr1, *ptr2;
    int r;
    ptr1 = (char*)lowptr;
    
    for(int i=0; i<numOps; i++){
        r = rand() % (numOps);
        ptr2 = (char*)lowptr+((r==0) ? r+1 : r)*size;
        memcpy(ptr1, ptr2, size);
    }
    return;
}
/*void read(int **matric,int RowSize,int LineSize)
{
    for(int i=0;i<RowSize;i++)
        
        for(int j=0;j<LineSize;j++)
        {
            cout<<*((int*)matric+i*LineSize+j)<<endl;
        }
    
}

void randomRead(int **matric,int RowSize,int LineSize)
{
    for(int i=0;i<RowSize;i++)
        
        for(int j=0;j<LineSize;j++)
        {
            cout<<*((int*)matric+(rand()%RowSize)*LineSize+(rand()%LineSize))<<endl;
        }
}

void randomWrite(int **matric,int RowSize,int LineSize)
{
    for(int i=0;i<RowSize;i++)
        
        for(int j=0;j<LineSize;j++)
        {
            *((int*)matric+(rand()%RowSize)*LineSize+(rand()%LineSize)) =rand()%100;
        }
    
}*/

void setThread(int thread)
{
    THREAD=thread;
}

void setRandom(int random)
{
    RANDOM=random;
}

void setSize(size_t Size,int Ops)
{
    size=Size;
    numOps=Ops;
    //cnt1=size1;
    //cnt2=size2;
}

static void *run(void* args)
{
    void* endptr;
    void* mem;
    double result;
    pthread_mutex_lock( &sum_mutex );
    for(int i=0;i<3;i++)
    {
        
        //int** A;
       // A=new int*[cnt2];
        mem = malloc(MB*CHUNK_SIZE);
        endptr = (char*)mem+(CHUNK_SIZE*MB);
        gettimeofday(&tv, NULL);
        c_start = tv.tv_sec*1000000LL + tv.tv_usec;
        if(RANDOM)
        {
           // c_start = clock();
            
            random_access(mem, endptr, size);
           // gettimeofday(&tv, NULL);
         //  c_end = tv.tv_sec*1000000LL + tv.tv_usec;
           
            //randomWrite((int **)A,cnt1,cnt2);
          //  c_end = clock();
           // average[i]=c_end-c_start;
           // c_start1 = clock();
          //  randomRead((int **)A,cnt1,cnt2);
           // c_end1=clock();
        }
        else
        {
            sequential(mem, endptr, size);
            //gettimeofday(&tv, NULL);
            //c_end = tv.tv_sec*1000000LL + tv.tv_usec;
            
        }
        gettimeofday(&tv, NULL);
        c_end = tv.tv_sec*1000000LL + tv.tv_usec;
        
        result +=c_end-c_start;
        result_1=result;
        free(mem);
    }
    average=result/3.0;
   // long result=(average[0]+average[1]+average[2])/3;
   // long result1=(average1[0]+average1[1]+average1[2])/3;
    pthread_mutex_unlock( &sum_mutex ); //释放锁，供其他线程使用
    pthread_exit( ( void* )average);
}

int main()
{
    cout<<"Random access or not?(input 0 ro 1)"<<endl;
    cin>>r;
    while(r!=0&&r!=1)
    {
        cout<<"Exceed limited number!please choose again(input 0 or 1)"<<endl;
        cin>>r;
    }
    setRandom(r);
    cout<<"Set block size(input b,k or m)"<<endl;
    cin>>l;
    while (l!='b'&&l!='k'&&l!='m')
    {
        cout<<"Exceed limited number!please choose again(input b,k or m)"<<endl;
        cin>>l;
    }
    if(l=='b')setSize(B,1073741824);
    else if(l=='k')setSize(KB, 1048576);
    else if (l=='m')setSize(MB, 1024);
    cout<<"Choose the number of thread(input 1 or 2)"<<endl;
    cin>>n;
    while(n!=1&&n!=2)
    {
        cout<<"Exceed limited number!please choose again(input 1 or 2)"<<endl;
        cin>>n;
    }
    setThread(n);
    pthread_t tids[THREAD]; //线程id
    int indexes[THREAD];
    pthread_attr_t attr; //线程属性结构体，创建线程时加入的参数
    pthread_attr_init( &attr ); //初始化
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
    pthread_mutex_init( &sum_mutex, NULL ); //对锁进行初始化

    //c_start = clock();
    
   
        
    gettimeofday(&tv, NULL);
    c_start = tv.tv_sec*1000000LL + tv.tv_usec;
    for( int i = 0; i < THREAD; ++i )
    {
        indexes[i] = i;
        int ret = pthread_create( &tids[i], &attr, run, (void* )&(indexes[i])); //参数：创建的线程id，线程参数，线程运行函数的起始地址，运行函数的参数
        if( ret != 0 ) //创建线程成功返回0
        {
            cout << "pthread_create error:error_code=" << ret << endl;
        }
    }
    pthread_attr_destroy( &attr );
    void *average_1;
    for( int j = 0; j <THREAD; ++j )
    {
        int ret = pthread_join( tids[j],&average_1); //主程序join每个线程后取得每个线程的退出信息status
        
        if( ret != 0)
        {
            cout << "pthread_join error:error_code=" << ret << endl;
        }
        else
        {
            //gettimeofday(&tv, NULL);
            //c_end = tv.tv_sec*1000000LL + tv.tv_usec;
           // result=(c_end-c_start)/1000000.0;
            cout<<"Thread"""<<j+1<<":"<<"The average time is:"<<(long)average_1/3000000.0<<"s"<<endl;
            //cout<<"Average result is:"<<result<<"s"<<endl;
           cout<<"Thread"""<<j+1<<":"<<"The throughput is:"<<CHUNK_SIZE*1000000.0/(long)average_1<<"MB/S"<<endl;
            cout<<"Thread"""<<j+1<<":"<<"The latency is:"<<(long)average_1/(CHUNK_SIZE*MB*THREAD*8*1000.0)<<"MS"<<endl;

          //  cout<<"The block size is:1"<<l<<endl;
         // a=(long)average;
                                          }
    }
    //cout<<"Average time is:"<<result/3000000.0<<"S"<<endl;
    //cout<<"Average result is:"<<result<<"s"<<endl;
   // cout<<"The average throughput is:"<<CHUNK_SIZE*THREAD*1000000.0/a<<"MB/S"<<endl;
    //cout<<"Latency is:"<<(double)(a/(CHUNK_SIZE*MB*THREAD*8*1000.0))<<"MS"<<endl;
    //cout<<"The block size is:1"<<l<<endl;

    pthread_mutex_destroy( &sum_mutex ); //注销锁
    pthread_exit( NULL );
    return 0;
}

