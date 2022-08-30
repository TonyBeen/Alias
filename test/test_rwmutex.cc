#include <utils/mutex.h>
#include <utils/utils.h>
#include <utils/string8.h>
#include <utils/thread.h>
#include <assert.h>
#include <stdio.h>
#include <atomic>

using namespace std;
using namespace eular;

static char buf[128] = {0};
RWMutex gRwMutex;
std::atomic<bool> gExit;

int read_func(void *)
{
    while (gExit == false) {
        {
            RDAutoLock<RWMutex> lock(gRwMutex);
            printf("[%ld]buf: %s\n", gettid(), buf);
        }
        usleep(100 * 1000);
    }
}

int write_func(void *)
{
    int num = 0;
    while (gExit == false) {
        {
            WRAutoLock<RWMutex> lock(gRwMutex);
            snprintf(buf, 127, "num = %d", ++num);
        }
        usleep(50 * 1000);
    }
}

int main()
{
    gExit = false;
    Thread t1(std::bind(read_func, nullptr), "read");
    Thread t2(std::bind(write_func, nullptr), "write");

    t1.detach();
    t2.detach();

    sleep(2);
    gExit = true;
    return 0;
}