/*************************************************************************
    > File Name: condition.h
    > Author: hsz
    > Desc: 
    > Created Time: 2021年05月30日 星期日 11时44分27秒
 ************************************************************************/

#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "utils.h"
#include "mutex.h"

namespace eular {
class Condition {
public:
    enum {
        PRIVATE = 0,
        SHARED = 1
    };
    Condition();
    Condition(int type);
    ~Condition();

    int wait(Mutex& mutex);
    int timedWait(Mutex& mutex, nsec_t ns);

    void signal();
    void broadcast();

private:
    pthread_cond_t mCond;
};

} // namespace eular

#endif // __CONDITION_H__
