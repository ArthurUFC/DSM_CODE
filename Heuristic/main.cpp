#include <iostream>
#include "User.h"
#include "Scheduler.h"
using namespace std;

int main()
{
  //  vector<vector<int> > aabb;
  // Scheduler b;
   User data;

   for(int i = 0; i < 50; i++) {
        data.adjustSchedule();
   }
   //data.PrintSchedule();
   data.PrintTotalDemand();
   // double ddg[24] ={0};
    //for(int i =0; i < 24; i++)
      //  cout<<ddg[i];
   //dd.testFun();
   //cout<<b.get();
   //User *a = new User[2];
    return 0;
};
