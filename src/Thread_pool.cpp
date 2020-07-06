#include "Thread_pool.h"
using namespace std;

#include <thread>
#include <chrono>


I_Pool::I_Pool(I_Queue* q, const size_t& pool_size, const unsigned int polling_microseconds) :
queue(q), polling(polling_microseconds) {   

    if(pool_size <= 1) throw 0;
    this->pool.reserve(pool_size);
    for(size_t k=0; k<pool_size; ++k){
        this->pool.emplace_back();
        this->pool.back().creator = this;
        this->pool.back().life = true;
        if (pthread_mutex_init(&this->pool.back().life_mtx, NULL) != 0) throw 1;
        if (pthread_create(&this->pool.back().handler, NULL, &__thread_loop, &this->pool.back()) != 0) throw 2;
    }

}

I_Pool::~I_Pool(){
    for(size_t k=0; k<this->pool.size(); ++k){
        pthread_mutex_lock(&this->pool[k].life_mtx);
        this->pool[k].life = false;
        pthread_mutex_unlock(&this->pool[k].life_mtx);
        pthread_join(this->pool[k].handler, NULL);
        pthread_mutex_destroy(&this->pool[k].life_mtx);
    }

    delete this->queue;

}

void* I_Pool::__thread_loop(void* context){

    bool life;
    __thread_info* info = (__thread_info*)context;
    while (true) {
        I_Job* job = info->creator->queue->pop();

        if(job == nullptr) {
            pthread_mutex_lock(&info->life_mtx);
            life = info->life;
            pthread_mutex_unlock(&info->life_mtx);
            if(!life) break;

            std::this_thread::sleep_for(std::chrono::microseconds(info->creator->polling));
        }
        else{
            (*job)();
            delete job;
        }
    }

}

void I_Pool::await(){

    while(true){
        if(this->queue->empty()) break;
        std::this_thread::sleep_for(std::chrono::microseconds(this->polling));
    }

}
