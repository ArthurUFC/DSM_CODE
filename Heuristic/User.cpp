#include <iostream>
#include <ctime>
#include <cstdlib>
#include "User.h"


//Initializes the simulation data
User::User()
{
    srand (time(NULL));
    this->nUsers = rand() % 2 + 500;    //# of users
    this->batteryCapacity = 10;
    this->nJob = new int[nUsers];
    this->price = new double[ntimeSlot];
	earliestSlot.resize(nUsers);  //The earliest time slot the job can start
	deadLine.resize(nUsers);  //the latest time slot the job have to be done.
	startSlot.resize(nUsers);
	jobLength.resize(nUsers);
	jobPower.resize(nUsers);  //The power consumption of job.
	isScheduled.resize(nUsers);  // Check whether the job scheduled or not
    isWorking.resize(nUsers);
    for(int a = 0; a < ntimeSlot; a++) {
        totalDemand[a] = 0;
    }
	for(int i = 0; i < nUsers; i++) {
        nJob[i] = rand()% 10 + 1;
        isWorking[i].resize(nJob[i]);
        for(int j = 0; j < nJob[i]; j++) {
            this->earliestSlot[i].push_back(0);
            this->deadLine[i].push_back(23);
            this->jobLength[i].push_back(rand() % 5 + 6);
            this->jobPower[i].push_back((double)(rand() % 100 + 1) / 10);
            this->isScheduled[i].push_back(false);
            this->startSlot[i].push_back(0);
            for(int k = 0; k < ntimeSlot; k++) {
                if(k >= startSlot[i][j] && k < (startSlot[i][j] + jobLength[i][j]))
                    this->isWorking[i][j].push_back(true);
                else
                    this->isWorking[i][j].push_back(false);
            }
        }
    }
}

User::~User(){
    delete [] nJob;
    delete [] price;
}

double *User::computeTotalDemand(vector<vector<vector<bool> > > &isWorkingArg) {
    double *totalDemandPtr = new double [ntimeSlot];
    for(int k = 0; k < ntimeSlot; k++){
    *(totalDemandPtr+k) = 0;
        for(int i = 0; i < nUsers; i++){
            for(int j = 0; j < nJob[i]; j++){
                *(totalDemandPtr+k) += jobPower[i][j] * (double)isWorkingArg[i][j][k];
            }
        }
    }
    return totalDemandPtr;
}

double *User::computePrice(double *totalDemand) {
    double *price = new double [ntimeSlot];
    for(int i=0; i<ntimeSlot; i++) {
        price[i] = 0;
        price[i] = 5 * totalDemand[i] ;
    }
    return price;
}

double *User::getPrice() {
    return price;
}

void User::copySchedule(vector<vector<vector<bool> > > &isWorkingTemp) {
    for(int i = 0; i < nUsers; i++){
        for(int j =0; j < nJob[i]; j++){
            for(int k = 0; k < ntimeSlot; k++) {
                isWorkingTemp[i][j].assign(isWorking[i][j].begin(), isWorking[i][j].end());
            }
        }
    }
}
void User::UpdateSchedule(){
    for(int i = 0; i < nUsers; i++){
        for(int j =0; j < nJob[i]; j++){
            for(int k = 0; k < ntimeSlot; k++) {
                if(k >= startSlot[i][j] && k < (startSlot[i][j] + jobLength[i][j]))
                    isWorking[i][j][k] = true;
                else
                    isWorking[i][j][k] = false;
            }
        }
    }
}
void User::PrintSchedule(){
    int tes;
    for(int i = 0; i < nUsers; i++){
        for(int j =0; j < nJob[i]; j++){
            tes = 0;
            for(int k = 0; k < ntimeSlot; k++) {
                tes += isWorking[i][j][k];
                cout<<isWorking[i][j][k]<<" ";
            }
            if (tes = jobLength[i][j])
                cout<<"True";
            else
                cout<<"False";
                cout<<endl;
        }
    }
}
void User::PrintTotalDemand(){
    double TDemand[ntimeSlot] = {0};
    for(int i = 0; i < ntimeSlot; i++){
        for(int j =0; j < nUsers; j++){
            for(int k = 0; k < nJob[j]; k++) {
                TDemand[i] += jobPower[j][k] * (double)isWorking[j][k][i];
            }
        }
        cout<<TDemand[i]<<" ";
    }
}
void User::adjustSchedule() {
    double minJobCost = 0;
    double tempJobCost = 0;//To store the temporary job cost
    double *totalTempDemand = NULL;//To store the temporary total energy demand
    double totalReverseTempDemand[ntimeSlot] = {0};//To store the temporary total energy demand
    double totalMinDemand[ntimeSlot] = {0};
    totalTempDemand = computeTotalDemand(isWorking);
    cout<<"Start:"<<endl;
    for (int i = 0; i < ntimeSlot; i++){
        totalReverseTempDemand[i] = totalTempDemand[i];
        cout<<totalTempDemand[i]<<" ";
    }
    for(int i = 0; i < nUsers; i++) {
        for(int j = 0; j < nJob[i]; j++) {
            for(int k = startSlot[i][j]; k < (startSlot[i][j] + jobLength[i][j]); k++){
                minJobCost += (5 * totalTempDemand[k] * jobPower[i][j]);
            }
            //Search the minimum job cost in chronological order
            for(int newStartSlot = startSlot[i][j] + 1; newStartSlot <= (deadLine[i][j] - jobLength[i][j] + 1); newStartSlot++) {
                totalTempDemand[newStartSlot - 1] -= jobPower[i][j];
                totalTempDemand[newStartSlot + jobLength[i][j] - 1] += jobPower[i][j];
                for(int l = newStartSlot; l < newStartSlot + jobLength[i][j]; l++){
                    tempJobCost += (5 * totalTempDemand[l] * jobPower[i][j]);
                }
                if(tempJobCost < minJobCost) {
                    minJobCost = tempJobCost;
                    startSlot[i][j] = newStartSlot;
                    for(int l = 0; l < ntimeSlot; l++){
                        totalMinDemand[l] = totalTempDemand[l];
                    }
                }
                tempJobCost = 0;
            }
            //Search the minimum job cost in reverse chronological order
            for(int newStartSlot = startSlot[i][j] - 1; newStartSlot >= (earliestSlot[i][j]); newStartSlot--) {
                totalReverseTempDemand[newStartSlot + jobLength[i][j]] -= jobPower[i][j];
                totalReverseTempDemand[newStartSlot] += jobPower[i][j];
                for(int l = newStartSlot; l < newStartSlot + jobLength[i][j]; l++){
                    tempJobCost += (5 * totalReverseTempDemand[l] * jobPower[i][j]); // Price =  5 * Total Demand  
                }
                if(tempJobCost < minJobCost) {
                    minJobCost = tempJobCost;
                    startSlot[i][j] = newStartSlot;
                    for(int l = 0; l < ntimeSlot; l++){
                        totalMinDemand[l] = totalReverseTempDemand[l];
                    }
                }
                tempJobCost = 0;
            }
            for(int k = 0; k < ntimeSlot; k++){
                totalTempDemand[k] = totalReverseTempDemand[k] = totalMinDemand[k];
            }
            minJobCost = 0;
        }
    }
    UpdateSchedule();
    cout<<endl<<"ended"<<endl;
}

double User::computejobCost(double *priceArg, vector<vector<vector<bool> > > &isWorkingArg,
                            vector<vector<double> >jobPower, int userIndex, int jobIndex) {
    double jobCost = 0;
    for(int i = 0; i < ntimeSlot; i++) {
        jobCost +=  (priceArg[i] * jobPower[userIndex][jobIndex] * isWorkingArg[userIndex][jobIndex][i]);
    }
    return jobCost;
}
