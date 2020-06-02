#ifndef MR_UTIL_H_
#define MR_UTIL_H_

#include <android/log.h>
#define LOG_TAG "jda4android"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__))
#include "sstream"
using namespace std;
static string double2string(double d)
{
	stringstream ss;
	ss << d;
	return ss.str();
}


class MRTimer
{
public:
    MRTimer(){cost_time = 0;}
    void log4debug(const char * prefix=NULL)
    {
        long start = ((long)t_start.tv_sec)*1000+(long)t_start.tv_usec/1000;
        long end = ((long)t_end.tv_sec)*1000+(long)t_end.tv_usec/1000;
        cost_time = end - start;
        LOGD("%s %ld ms\n",prefix,cost_time);
    }
    void start()
    {
        gettimeofday(&t_start, NULL);
    }
    void stop()
    {
        gettimeofday(&t_end, NULL);
    }
private:
        struct timeval t_start,t_end;
        long cost_time;
};

#endif
