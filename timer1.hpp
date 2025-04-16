#include<iostream>
using namespace std;
#include <functional>
#include <chrono>
#include<set>
#include <sys/timerfd.h>
#include <time.h>


struct TimerNodeBase{
    time_t expire;
    uint64_t id;
};

//暴漏给用户的是TimerNodeBase，不会暴漏子类



bool operator<(const TimerNodeBase & lhd,const TimerNodeBase & rhd){
   if(lhd.expire<rhd.expire){
     return true;
   }else if(lhd.expire>rhd.expire){
      return false;
   }else{
     return lhd.id<rhd.id;
   }
}

class Timer{

protected:

      struct TimerNode:public TimerNodeBase{
        using Callback=std::function<void(const TimerNode &node)>;
        Callback func;
        TimerNode(uint64_t id,time_t expire,Callback func){
        this->expire=expire;
        this->id=id;
        this->func=std::move(func);
       }
       };

    public:
      static inline time_t GetTick(){
         return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
      }
      TimerNodeBase AddTimer(int msec,TimerNode::Callback func){
         time_t expire=GetTick()+msec;
         if(timeouts.empty()||expire<=timeouts.crbegin()->expire){//begin()返回最小值、crbegin()返回最大值
            auto pairs = timeouts.emplace(TimerNode(GenID(), expire, std::move(func)));
             return static_cast<TimerNodeBase>(*pairs.first);
         }
         auto ele=timeouts.emplace_hint(timeouts.crbegin().base(),TimerNode(GenID(), expire, std::move(func)));
         return static_cast<TimerNodeBase>(*ele);
      }
      void HandleTimer(time_t now){
        auto iter=timeouts.begin();
        while(iter!=timeouts.end()&&iter->expire<=now){
           iter->func(*iter);
           iter=timeouts.erase(iter);
        }
      }
      void DelTimer(TimerNodeBase &node){
        auto iter=timeouts.find(node);
        if(iter!=timeouts.end()){
          timeouts.erase(iter);
        }
      }

      virtual void UpdateTimerfd(const int fd) {
        struct timespec abstime;
        auto iter = timeouts.begin();
    
        if (iter != timeouts.end()) {
          abstime.tv_sec = iter->expire / 1000;  // seconds part
          abstime.tv_nsec = (iter->expire % 1000) * 1000000;  // nanoseconds part
        } else {//如果timeouts 为空，will not trigger any action
          abstime.tv_sec = 0;
          abstime.tv_nsec = 0;
        }

        struct itimerspec its = {
          .it_interval = {0, 0},  // One-shot timer (no periodic behavior)
          .it_value = abstime      // Set the absolute expiration time
        };
         // Set the timer with the new expiration time
       int ret = timerfd_settime(fd, TFD_TIMER_ABSTIME, &its, nullptr);
       if (ret == -1) {
        perror("timerfd_settime failed");
       } 
}
   int WaitTime(){
         auto iter=timeouts.begin();
         if(iter==timeouts.end()){
           return -1;
         }
         int dis=iter->expire-GetTick();
         return dis>0? dis:0;
       }
 
    private:
    uint64_t GenID(){
      return gid++;
    }
    static uint64_t gid;
    set<TimerNode,std::less<>>timeouts;//底层是红黑树
};
uint64_t Timer::gid=0;