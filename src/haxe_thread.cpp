#include "haxe_thread.h"

void run_haxe(std::function<void()> fun)
{
    HaxeThread* ht = HaxeThread::getInstance();
    ht->run(fun);
}

