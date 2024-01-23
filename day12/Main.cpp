#include <pthread.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <ctime>
#include "RingQueue.hpp"
#include "TASK.hpp"
struct ThreadData
{
    RingQueue<TASK>* RQ;
    std::string name;
};
void *Consumer(void *args)
{
   
    ThreadData *td=static_cast<ThreadData*>(args);
    RingQueue<TASK> *rq=td->RQ;

    while (true)
    {
        //sleep(2);
        // 1.消费数据
        TASK t;
        rq->Pop(&t);

        // 2.处理数据
        t.run();
        std::cout << "Comsumer get task,task is: " << t.GetTASK() <<"who: "<<td->name<< " reslut :" << t.Getresult() << std::endl;
    }
    return nullptr;
}

void *Producer(void *args)
{
    ThreadData *td=static_cast<ThreadData*>(args);
    RingQueue<TASK> *rq=td->RQ;

    int len = opera.size();
    while (true)
    {
       
        // 1.获取数据
        int data1 = rand() % 10 + 1;
        usleep(10);
        int data2 = rand() % 10;
        char op = opera[rand() % len];
        // 2.生产数据
        TASK t(data1, data2, op);
        rq->Push(t);
        std::cout << "Productor task done,task is: " << t.GetTASK() <<" who: "<<td->name<< std::endl;
         sleep(2);
    }
    return nullptr;
}


int main()
{

    srand(time(nullptr));
    RingQueue<TASK> *rq = new RingQueue<TASK>();
    pthread_t c[5], p[3];
    for (int i = 0; i < 3; i++)
    {
        ThreadData* td=new ThreadData();
        td->name="Procductor-"+std::to_string(i);
        td->RQ=rq;
        pthread_create(p + i, nullptr, Producer, td);
    }
    for (int i = 0; i < 5; i++)//在创建线程的时候可以给线程定义基本的属性比如名字,以参数的形式传给线程
    {
         
        ThreadData* td=new ThreadData();
        td->name="Consumer-"+std::to_string(i);
        td->RQ=rq;
        pthread_create(c + i, nullptr, Consumer, td);
    }
    

    for (int i = 0; i < 1; i++)
    {
        pthread_join(c[i], nullptr);
    }
    for (int i = 0; i < 1; i++)
    {
        pthread_join(p[i], nullptr);
    }
}