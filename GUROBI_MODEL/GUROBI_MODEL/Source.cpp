#include "gurobi_c++.h"
#include <vector>
#include <ctime>
#include "stdlib.h"
using namespace std;


int main(int   argc, char *argv[])
{
	srand(time(0));
	const int bignumber = 10000000000;
	const int nTimeSlot = 24;
	const double batteryCapacity = 10.0;
	int nUser = rand() % 10 + 1; // number of users
	int *nJobs = new int[nUser]; // numser of jobs of user i;
	int **duration = NULL;
	int **jobDeadline = NULL;
	double **jobEnergyConsumption = NULL;
	double **renewableEnergy = NULL;
	//double batteryCapacity = 10.0;

	duration = new int *[nUser];
	jobDeadline = new int *[nUser];
	renewableEnergy = new double *[nUser];
	jobEnergyConsumption = new double *[nUser];
	for (int i = 0; i < nUser; i++){
		nJobs[i] = rand() % 5 + 1;
		duration[i] = new int[nJobs[i]];
		jobDeadline[i] = new int[nJobs[i]];
		renewableEnergy[i] = new double[nTimeSlot];
		jobEnergyConsumption[i] = new double[nJobs[i]];
		for (int j = 0; j < nJobs[i]; j++) {
			duration[i][j] = rand() % 4 + 4;
			jobDeadline[i][j] = 23;
			renewableEnergy[i][j] = rand() % 3 + 1;
			jobEnergyConsumption[i][j] = rand() % 4 + 4;
		}
	}
	try {
		/*
		*	Build the model
		*
		*/
		
		GRBEnv env = GRBEnv();//crate the environment
		GRBModel model = GRBModel(env);//create the model

		/**
		*
		*	Decision Variable Declarations
		*
		*/
		//GRBVar *totalDemand = new GRBVar[nTimeSlot];
		//GRBVar *gridDemand = new GRBVar[nTimeSlot];
		GRBVar *energyPrice = new GRBVar[nTimeSlot];
		GRBVar **gridDemand = NULL;
		GRBVar **totalApplianceDemand = NULL;
		GRBVar **batteryLevel = NULL;
		GRBVar **c = NULL;
		GRBVar **d = NULL;
		GRBVar **p = NULL;
		GRBVar **f = NULL;
		GRBVar **g = NULL;
		GRBVar **h = NULL;
		GRBVar **u = NULL;
		GRBVar **k = NULL;
		GRBVar **s = NULL;
		GRBVar **x = NULL;
		GRBVar **y = NULL;
		GRBVar **c_binary = NULL;
		GRBVar **d_binary = NULL;

		vector<vector<vector <GRBVar> > > Sigma;
		vector<vector<vector <GRBVar> > > I;
		vector<vector<double> > applianceDemand;
		batteryLevel = new GRBVar*[nUser];
		totalApplianceDemand = new GRBVar*[nUser];
		gridDemand = new GRBVar*[nUser];
		c = new GRBVar *[nUser];
		d = new GRBVar *[nUser];
		p = new GRBVar *[nUser];
		f = new GRBVar *[nUser];
		g = new GRBVar *[nUser];
		h = new GRBVar *[nUser];
		u = new GRBVar *[nUser];
		k = new GRBVar *[nUser];
		s = new GRBVar *[nUser];
		x = new GRBVar *[nUser];
		y = new GRBVar *[nUser];
		c_binary = new GRBVar *[nUser];
		d_binary = new GRBVar *[nUser];

		for (int i = 0; i < nUser; i++) {
			batteryLevel[i] = new GRBVar[nTimeSlot];
			totalApplianceDemand[i] = new GRBVar[nTimeSlot];
			gridDemand[i] = new GRBVar[nTimeSlot];
			c[i] = new GRBVar[nTimeSlot];
			d[i] = new GRBVar[nTimeSlot];
			p[i] = new GRBVar[nTimeSlot];
			f[i] = new GRBVar[nTimeSlot];
			g[i] = new GRBVar[nTimeSlot];
			h[i] = new GRBVar[nTimeSlot];
			u[i] = new GRBVar[nTimeSlot];
			k[i] = new GRBVar[nTimeSlot];
			s[i] = new GRBVar [nTimeSlot];
			x[i] = new GRBVar[nTimeSlot];
			y[i] = new GRBVar[nTimeSlot];
			c_binary[i] = new GRBVar[nTimeSlot];
			d_binary[i] = new GRBVar[nTimeSlot];
		}

		Sigma.resize(nUser);
		I.resize(nUser);
		for (int i = 0; i < nUser; i++) {
			Sigma[i].resize(nJobs[i]);
			I[i].resize(nJobs[i]);
			for (int j = 0; j < nJobs[i]; j++) {
				Sigma[i][j].resize(nTimeSlot);
				I[i][j].resize(nTimeSlot);
			}
		}
		string price = "price";
		for (int i = 0; i < nTimeSlot; i++){
			//totalDemand[i] = model.addVar(0.0, GRB_INFINITY, GRB_CONTINUOUS, 0.0, "Total Demand : ");
			energyPrice[i] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "price constraint");
		}
		model.update();
		for (int i = 0; i < nUser; i++){
			for (int j = 0; j < nTimeSlot; j++){
				
				batteryLevel[i][j] = model.addVar(0.0, batteryCapacity, 0.0, GRB_CONTINUOUS, "Battery Level");
				totalApplianceDemand[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "c");
				c[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "c");
				d[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "d");
				p[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "p");
				f[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "f");
				g[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "g");
				h[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "h");
				u[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "u");
				k[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "k");
				s[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "s");
				x[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "x");
				y[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "y");
				gridDemand[i][j] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "Grid Demand : ");
				c_binary[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "Grid Demand : ");
				d_binary[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "Grid Demand : ");
			}
		}
		model.update();

		for (int i = 0; i < nUser; i++) {
			for (int j = 0; j < nJobs[i]; j++) {
				for (int k = 0; k < nTimeSlot; k++){
					Sigma[i][j][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "Sigma");
					I[i][j][k] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "I");
				}
			}
		}
		model.update();

		// Add constraints
		for (int i = 0; i < nTimeSlot; i++){
			GRBLinExpr price_RHS = 0;
			for (int j = 0; j < nUser; j++){
				price_RHS += 2 * gridDemand[j][i];
			}
			model.addConstr(energyPrice[i], GRB_EQUAL, price_RHS); //constraint 3.2
		}
		model.update();

		for (int i = 0; i < nUser; i++){
			GRBLinExpr Grid_Demand_RHS = 0;
			for (int j = 0; j < nTimeSlot; j++){
				Grid_Demand_RHS = totalApplianceDemand[i][j] + g[i][j] - d[i][j] - u[i][j] + y[i][j]; //constraint 3.3
				model.addConstr(gridDemand[i][j], GRB_EQUAL, Grid_Demand_RHS);
			}
		}
		model.update();
		for (int i = 0; i < nUser; i++) {
			for (int j = 0; j < nJobs[i]; j++){
				GRBLinExpr LHS_35 = 0;
				GRBLinExpr LHS_36 = 0;
				for (int k = 0; k < (jobDeadline[i][j] - duration[i][j] + 1); k++){
					LHS_35 += Sigma[i][j][k];
				}
				for (int k = (jobDeadline[i][j] - duration[i][j] + 1); k < nTimeSlot; k++){
					LHS_36 += Sigma[i][j][k];
				}
				model.addConstr(LHS_35, GRB_EQUAL, 1);
				model.addConstr(LHS_36, GRB_EQUAL, 0);
			}
		}
		for (int i = 0; i < nUser; i++) {
			for (int j = 0; j < nJobs[i]; j++){
				for (int k = 0; k < nTimeSlot; k++){
					int start = 0;
					start = ((k - duration[i][j] + 1) > 0 ? (k - duration[i][j] + 1) : 0);
					GRBLinExpr RHS_37 = 0;
					for (start; start <= k; start++){
						RHS_37 += Sigma[i][j][start];
					}
					model.addConstr(I[i][j][k], GRB_EQUAL, RHS_37);
				}
			}
		}
		model.update();
		for (int i = 0; i < nUser; i++) {
			for (int j = 0; j < nTimeSlot; j++){
				GRBLinExpr RHS_38 = 0;
				for (int k = 0; k < nJobs[i]; k++){
					RHS_38 += jobEnergyConsumption[i][k] * I[i][k][j];
				}
				model.addConstr(totalApplianceDemand[i][j], GRB_EQUAL, RHS_38);
			}
		}
		for (int i = 0; i < nUser; i++) {
			GRBLinExpr RHS_312 = 0;
			for (int j = 1; j < nTimeSlot; j++) {
				RHS_312 = batteryLevel[i][j - 1] + c[i][j - 1] - d[i][j - 1];//constraint 3.12
				model.addConstr(batteryLevel[i][j], GRB_EQUAL, RHS_312);
			}
		}

		for (int i = 0; i < nUser; i++) {
			//GRBLinExpr RHS_34 = 0;
			//GRBLinExpr RHS_310 = 0;
			//GRBLinExpr RHS_311 = 0;
			//GRBQuadExpr LHS_313 = 0;
			//GRBLinExpr LHS_313 = 0;
			GRBLinExpr RHS_314 = 0;
			GRBLinExpr RHS_315 = 0;

			for (int j = 0; j < nTimeSlot; j++) {
				//RHS_34 = x[i][j] + k[i][j];
				//RHS_310 = f[i][j] + g[i][j];
				//RHS_311 = x[i][j] + y[i][j];
				//LHS_313 = c[i][j] * d[i][j];
				//RHS_313 = c[i][j] + d[i][j];
				RHS_314 = h[i][j] + g[i][j];
				RHS_315 = h[i][j] + u[i][j];

				model.addConstr(x[i][j] + k[i][j] == d[i][j], "34");//add constraint 3.4
				model.addConstr(f[i][j] + g[i][j] == p[i][j], "310");//add constraint 3.10
				model.addConstr(x[i][j] + y[i][j] == s[i][j], "310");//add constraint 3.11
				model.addConstr(c[i][j] <= c_binary[i][j] * bignumber, "313_c");//add constraint 3.11
				model.addConstr(d[i][j] <= d_binary[i][j] * bignumber, "313_d");//add constraint 3.11
				model.addConstr(c_binary[i][j] + d_binary[i][j] == 1,"c_d_binary");
				//model.addConstr(LHS_313, GRB_LESS_EQUAL, 1);
				//model.addQConstr(LHS_313, GRB_EQUAL, 0, "310");//add quadratic constraint 3.13
				//model.addQConstr(c[i][j] * c_binary[i][j] + d[i][j] * d_binary[i][j] >= 0);
				//model.addConstr(c_binary[i][j] + c_binary[i][j] <= 1);
				model.addConstr(RHS_314, GRB_EQUAL, c[i][j],"314");//add constraint 3.14
				model.addConstr(renewableEnergy[i][j], GRB_EQUAL, RHS_315);//add constraint 3.15
				model.addConstr(d[i][j], GRB_LESS_EQUAL, batteryLevel[i][j]);//add constraint 3.17
			}
			model.addConstr(batteryLevel[i][23], GRB_GREATER_EQUAL, batteryLevel[i][0]);//add constraint 3.20;
		}
		model.update();
		for (int j = 0; j < nTimeSlot; j++){
			GRBLinExpr RHS_39 = 0;
			GRBLinExpr LHS_39 = 0;
			for (int i = 0; i < nUser; i++) {
				RHS_39 += (totalApplianceDemand[i][j] + c[i][j] + s[i][j]);
				LHS_39 += (p[i][j] + d[i][j] + renewableEnergy[i][j]);
			}
			model.addConstr(LHS_39, GRB_LESS_EQUAL, RHS_39,"39");//add constraint 3.9
		}
		/*
		for (int i = 0; i < nUser; i++){
		for (int j = 0; j < nTimeSlot; j++){
		model.addConstr(batteryLevel[i][j], GRB_LESS_EQUAL, batteryCapacity);//add constraint 3.18
		model.addConstr(batteryLevel[i][j], GRB_GREATER_EQUAL, 0);//add constraint 3.19
		}
		model.addConstr(batteryLevel[i][23], GRB_GREATER_EQUAL, batteryLevel[i][0]);//add constraint 3.20;
		}
		*/

		//Set objective function
		model.set(GRB_IntAttr_ModelSense, 1);
		model.update();
		GRBQuadExpr objective_func = 0;
		for (int i = 0; i < nUser; i++) {
			for (int j = 0; j < nTimeSlot; j++) {
				objective_func += (gridDemand[i][j] * energyPrice[j]);
			}
		}
		model.setObjective(objective_func);
		// 6.Optimization 
		model.optimize();
		// 7. Check Optimality

		int status = model.get(GRB_IntAttr_Status);
		if (status == GRB_OPTIMAL) {
			double ObjValue = model.get(GRB_DoubleAttr_ObjVal);
			cout << "total cost=" << ObjValue << endl;
		}
		else if (status == GRB_INF_OR_UNBD) {
			cout << "Infeasible or unbounded" << endl;
		}
		else if (status == GRB_INFEASIBLE) {
			cout << "Infeasible" << endl;
		}
		else if (status == GRB_UNBOUNDED) {
			cout << "Uunbounded" << endl;
		}
		else {
			cout << "Optimization was stopped with status" << status << endl;
		}
	}
	catch (GRBException e) {
		cout << "Error code = " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...) {
		cout << "Exception during optimization" << endl;
	}

	system("pause");

	return 0;

}
