
#pragma once
#include <iostream>
#include <vector>
#include <semaphore.h>
// 先模拟单生产单消费
const static int defaultcap = 5; // 默认大小
template <class T>
class RingQueue
{

public:
    void P(sem_t &sem)
    {
        sem_wait(&sem);
    }
    void V(sem_t &sem)
    {
        sem_post(&sem);
    }
    void Lock(pthread_mutex_t &mutex)
    {
      pthread_mutex_lock(&mutex);
    }

    void Unlock(pthread_mutex_t &mutex)
    {
      pthread_mutex_unlock(&mutex);
    }
    RingQueue(int cap = defaultcap) : _ringqueue(cap), _cap(cap), c_step(0), p_step(0)
    {
        sem_init(&cdata_sem, 0, 0);
        sem_init(&pspace_sem, 0, cap);

        pthread_mutex_init(&c_mutex, nullptr);
        pthread_mutex_init(&p_mutex, nullptr);
    }
    ~RingQueue()
    {
        sem_destroy(&cdata_sem);
        sem_destroy(&pspace_sem);

        pthread_mutex_destroy(&c_mutex);
        pthread_mutex_destroy(&p_mutex);
    }
//注意的是，对下标加锁，应该在申请信号量后面，从两个方面解释：信号量不需要保护，本身就是原子的。
//先申请信号量，后加锁可以提高效率，一个并发度，别人正在执行时，其他线程还可以申请信号量
    void Push(const T &in)
    {
        // 生产者在生产数据之前，需要先申请对应的信号量，申请成功说明可以生产数据
        P(pspace_sem); // 生产者申请空间资源
        
        Lock(p_mutex);
        _ringqueue[p_step] = in;
        p_step++;
        p_step %= _cap; // 维持环形
        Unlock(p_mutex);
       
        V(cdata_sem); // 格子已经被占了，数据多了一个，所以释放数据资源
    }

    void Pop(T *out)
    {
        // 消费者在消费数据之前，也需要申请对应的信号量，申请成功，说明可以消费

        P(cdata_sem); // 消费者申请数据资源
        Lock(c_mutex);
        *out = _ringqueue[c_step];
        c_step++;
        c_step %= _cap; // 维持环形
        Unlock(c_mutex);

        V(pspace_sem); // 拿走一个数据，空间资源多出来一个
    }

private:                       // 环形队列的基本属性
    std::vector<T> _ringqueue; // 用vector来模拟环形队列
    int _cap;                  // 该环形队列的最大值

    int c_step; // 消费者下标
    int p_step; // 生产者下标

    // 消费者和生产者各自关心的资源

    sem_t cdata_sem;  // 消费者关心的数据资源
    sem_t pspace_sem; // 生产者关心的空间资源

    // 如果是多个生产者，多个消费者，那么就需要用到锁
    // 因为生产者与生产者直接会竞争下标，而下标只有一个，所以需要对下标进行加锁保护，实现生产者与生产者之间的互斥。
    // 而消费者之间也会竞争下标，生产消费因为关心的是不同资源，所以各自保护各自的下标。
    pthread_mutex_t c_mutex;
    pthread_mutex_t p_mutex;
};