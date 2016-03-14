#include <iostream>
#include "User.h"
#include "Scheduler.h"
using namespace std;

int main()
{

   User users;
   for(int i = 0; i < 50; i++) {
        users.adjustSchedule();
   }
   users.PrintSchedule();
   users.PrintTotalDemand();
    return 0;
};
