#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <queue>
#include <pthread.h>


/** \brief General interface for an object implementing a thread pooling .
 * \details Threads are spawned in the constructor of I_Pool and mantained internally alived, ready to process tasks.
 * The tasks are stored in an internal queue that is accessed by the threads. To insert a new task in the queue use the mehtod push()
 * of the deriving object.
 * Deriving objects differ for the type of container used to store the tasks to dispatch to the pool of threads.
*/
class I_Pool{
public:

    /** \brief The termination of all the tasks in the queue is internally waited before destruction.
    */
    virtual ~I_Pool();

    I_Pool() = delete;
    I_Pool(const I_Pool& ) = delete;
    void operator=(const I_Pool& ) = delete;

    /** \brief Blocking await: the calling thread is stopped till the internal queue of tasks is empty, i.e. all the 
     * passed tasks have been processed.
     * \details Threads are not switched off after exiting this function. Therefore, new tasks can be inserted after calling await.
    */
    void await();    

protected:

    class I_Job{
    public:
        virtual ~I_Job() {};
        virtual void operator()() = 0;
    };

    template<typename Job_functor>
    class Job_concrete : public I_Job{
    public:
        Job_concrete(Job_functor* job, const bool& destroy_job) : Job(job), Destroy_Job(destroy_job) {};
        ~Job_concrete() { if(this->Destroy_Job) delete this->Job; };
        virtual void operator()(){ (*this->Job)(); };
    private:
        bool          Destroy_Job;
        Job_functor*  Job;
    };

    class I_Job_decorator : public I_Job{
    public:
        ~I_Job_decorator() { delete this->wrapped; };
        virtual void operator()(){ (*this->wrapped)(); };
    protected:
        I_Job_decorator(I_Job* to_wrap) : wrapped(to_wrap) {};

        I_Job*        wrapped;
    };


    class I_Queue{
    public:
        virtual ~I_Queue() {};
        virtual bool        empty() = 0;
        virtual I_Job*      pop() = 0;
        virtual void        push(I_Job* job) = 0;
    };

    template<typename Container> //Container must have: empty(), pop(), push(<T>)
    class Queue : public I_Queue{
    public:
        Queue(){ if (pthread_mutex_init(&this->queue_mtx, NULL) != 0) throw 0; };
        ~Queue() { pthread_mutex_destroy(&this->queue_mtx); };

        bool         empty(){ 
            bool res;
            pthread_mutex_lock(&this->queue_mtx);
            res =  this->queue.empty();
            pthread_mutex_unlock(&this->queue_mtx);
            return res;
        };
        I_Job*       pop(){
            I_Job* temp = nullptr;
            pthread_mutex_lock(&this->queue_mtx);
            if(!this->queue.empty()) temp = this->queue.pop();
            pthread_mutex_unlock(&this->queue_mtx);
            return temp;
        };
        void         push(I_Job* job){
            pthread_mutex_lock(&this->queue_mtx);
            this->queue.push(job);
            pthread_mutex_unlock(&this->queue_mtx);
        };
    private:
        pthread_mutex_t     queue_mtx;
        Container           queue;
    };


    I_Pool(I_Queue* q , const size_t& pool_size, const unsigned int polling_microseconds);

    void __push(I_Job* to_push) { this->queue->push(to_push); };
private:

    static void*    __thread_loop(void* context);

    struct __thread_info{
        I_Pool*         creator;
        bool            life;
        pthread_t       handler;
        pthread_mutex_t life_mtx;
    };

// data
    I_Queue*                                                                   queue;
    const unsigned int                                                         polling;
    std::vector<__thread_info>                                                 pool;
};



/** \brief The queue considered in this object is a simple FIFO buffer.
*/
class Pool_equipriority : public I_Pool{
private:

    class equi_Q : public std::queue<I_Job*>{
    public:
        equi_Q() {};

        I_Job* pop() { 
            I_Job* temp = this->front();
            this->std::queue<I_Job*>::pop();
            return temp;
        };
    };

public:

    /** 
     * @param[in] pool_size the size of the pool
     * @param[in] polling_microseconds the sleep time of the threads assumed when remaining inactive. 
    */
    Pool_equipriority(const size_t& pool_size, const unsigned int polling_microseconds = 5) : I_Pool(new Queue<equi_Q>(), pool_size, polling_microseconds) {};

    /** \brief Use this method to add a new task in the queue. 
     * @param[in] job the new task to do (must be a functor having the method void operator()(void) )
     * @param[in] destroy_after_process a flag specifying whether this functor must be destroyed or not after have being processed by a thread in the pool 
    */
    template<typename Job_functor>
    void     push(Job_functor* job, const bool& destroy_after_process = true){ this->__push(new Job_concrete<Job_functor>(job , destroy_after_process)); };

};



/** \brief The queue considered in this object is a priority queue.
 * \details The priority of a job is specified when inserting it into the queue.
*/
class Pool_priority : public I_Pool{
private:

    class prioritized_Job : public I_Job_decorator{
    public:
        template<typename Job_functor>
        prioritized_Job(Job_functor* job, const bool& destroy_job, const unsigned int& priority) : 
        I_Job_decorator(new Job_concrete<Job_functor>(job, destroy_job)), __Priority(priority) {};

        const unsigned int& get_Priority() const { return this->__Priority; };
    private:
        unsigned int __Priority;
    };

    class Priority_comparator{
    public:
        bool operator()(const I_Job* a, const I_Job* b) const{ return (static_cast<const prioritized_Job*>(a)->get_Priority() < static_cast<const prioritized_Job*>(b)->get_Priority()); };
    };

    class priority_Q : public std::priority_queue<I_Job*, std::vector<I_Job*>, Priority_comparator>{
    public:
        priority_Q() {};

        I_Job* pop() { 
            I_Job* temp = this->top();
            this->std::priority_queue<I_Job*, std::vector<I_Job*>, Priority_comparator>::pop();
            return temp;
        };
    };

public:

    /** \brief Similar to Pool_equipriority::Pool_equipriority.
    */
    Pool_priority(const size_t& pool_size, const unsigned int polling_microseconds = 5) : I_Pool( new Queue<priority_Q>() ,pool_size, polling_microseconds) {};

    /** \brief Use this method to add a new task in the queue. 
     * @param[in] job Similar to Pool_equipriority::push
     * @param[in] destroy_after_process Similar to Pool_equipriority::push
     * @param[in] priority the priority of this job with respect to the other inserted in the queue. Job with an higher priority must be processed sooner.
    */
    template<typename Job_functor>
    void     push(Job_functor* job, const bool& destroy_after_process = true, const unsigned int& priority = 0){ this->__push(new prioritized_Job( job, destroy_after_process, priority)); };

};

#endif