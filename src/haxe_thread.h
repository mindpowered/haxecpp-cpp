#pragma once

/**
 * Creating haxe objects must be done in a single thread
 * in case the garbage collecor decides to run on allocation
 */

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <haxecpp/main_hook.h>

/**
 * run all haxe stuff in one thread
 * - needed so gc will work
 * - call run() and pass a fully-bound lambda which returns byref
 */
class HaxeThread
{
private:
    std::thread _thread;
    std::thread::id _threadid;
    std::mutex _queue_mutex;
    std::queue<std::function<void()>> _queue;
    std::condition_variable _queue_changed;
    std::atomic<unsigned int> _hold;
    std::atomic<bool> _shutdown;
public:
    // singleton
    static HaxeThread* getInstance()
    {
        static std::unique_ptr<HaxeThread> __instance = nullptr;
        if (!__instance)
        {
            __instance = std::unique_ptr<HaxeThread>(new HaxeThread());
        }
        return __instance.get();
    }
private:
    // private constructor
    HaxeThread():
        _thread(&HaxeThread::startDoingStuff, this),
        _threadid(),
        _queue_mutex(),
        _queue(),
        _queue_changed(),
        _hold(0),
        _shutdown(false)
    {
    }
public:
    // delete copy constructor
    HaxeThread(const HaxeThread&) = delete;

    ~HaxeThread()
    {
        _shutdown = true;
        _queue_changed.notify_all();
        _thread.join();
    }

    void run(std::function<void()> fun)
    {
        {
            // place the function in the queue
            _queue_mutex.lock();
            _queue.push(fun);
            _queue_mutex.unlock();
            _queue_changed.notify_all();
        }

        if (_threadid == std::this_thread::get_id())
        {
            doStuff();
        }
        else
        {
            waitForStuff();
        }
    }
private:
    void startDoingStuff()
    {
        HAXECPP_MAIN_HOOK
        _threadid = std::this_thread::get_id();
        while(!_shutdown)
        {
            doStuff();
            std::unique_lock<std::mutex> queue_lock(_queue_mutex);
            _queue_changed.wait(queue_lock, [this](){
                return !this->_queue.empty() || _shutdown;
            });
        }
    }
    void doStuff()
    {
        bool finished = false;
        while(!finished)
        {
            if (_queue_mutex.try_lock())
            {
                if (!_queue.empty())
                {
                    std::function<void()> fun = _queue.front();
                    _queue.pop();
                    _hold++;
                    _queue_mutex.unlock();
                    fun();
                    _hold--;
                    _queue_changed.notify_all();
                }
                else
                {
                    _queue_mutex.unlock();
                    finished = true;
                }
            } // try_lock
        } // !finished
    }

    void waitForStuff()
    {
        bool finished = false;
        do
        {
            _queue_mutex.lock();
            finished = _queue.empty() && !_hold;
            _queue_mutex.unlock();

            std::unique_lock<std::mutex> queue_lock(_queue_mutex);
            _queue_changed.wait(queue_lock, [this](){
                return this->_queue.empty() && !_hold;
            });

        }
        while(!finished);
    }
};

// C style wrapper

void run_haxe(std::function<void()> fun)
{
    HaxeThread* ht = HaxeThread::getInstance();
    ht->run(fun);
}
