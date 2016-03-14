#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED

#include <iostream>
#include <vector>
using namespace std;

class User {
public:
    User();
    ~User();
    User(int x, int y , int z);
    void scheduleJob();
    void computeCost();
    double *computeTotalDemand(vector<vector<vector<bool> > > &isWorking);
    double *getPrice();
    double *computePrice(double *totalDemand);
    void adjustSchedule();
    void copySchedule(vector<vector<vector<bool> > > &isWorkingTemp);
    double computejobCost(double *price, vector<vector<vector<bool> > > &isWorkingArg,
                            vector<vector<double> >jobPower, int userIndex, int jobIndex);
    void UpdateSchedule();
    void PrintSchedule();
    void PrintTotalDemand();

private:
	int jobNum;
	int nUsers;
	int *nJob = NULL;
	double *price = NULL;
	const static int ntimeSlot = 24;
	double batteryCapacity;
	vector<vector<int> >startSlot;
	vector<vector<int> >earliestSlot;
	vector<vector<int> >deadLine;
	vector<vector<int> >jobLength;
	vector<vector<double> >jobPower;
	vector<vector<bool> >isScheduled;
	vector<vector <double> > f;
	vector<vector <double> > g;
	vector<vector <double> > h;
	vector<vector <double> > u;
	vector<vector <double> > k;
	vector<vector <double> > x;
	vector<vector <double> > y;
	vector<vector<vector<bool> > >isWorking;
	double totalDemand[ntimeSlot];
};

#endif // USER_H_INCLUDED
