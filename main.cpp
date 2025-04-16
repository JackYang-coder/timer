#include"timer1.hpp"
#include <sys/epoll.h>
#include<sys/timerfd.h>


void myfun1(const TimerNodeBase &node){
   std::cout<<"11"<<endl;
}



int main(){

  Timer timer;
  timer.AddTimer(1000,myfun1);
  timer.AddTimer(8000,myfun1);


 
     
     int epfd=epoll_create(1);
     int timerfd=timerfd_create(CLOCK_MONOTONIC,0);

     struct epoll_event ev;
     ev.events=EPOLLIN|EPOLLET;
     epoll_ctl(epfd,EPOLL_CTL_ADD,timerfd,&ev);
     
     struct epoll_event evs[64];
    //  while (1)
    //  {
    //    timer.UpdateTimerfd(timerfd);
    //    int n=epoll_wait(epfd,evs,64,-1);
    //    time_t now=Timer::GetTick();
    //    for(int i=0;i<n;i++){

    //    }
    //    timer.HandleTimer(now);
    //  }

     while (1)
     {
       int n=epoll_wait(epfd,evs,64,timer.WaitTime());
       time_t now=Timer::GetTick();
       for(int i=0;i<n;i++){

       }
       timer.HandleTimer(now);
     }
     



  return 0;
}