/********************************************************************************
 cchemi is an interface to ab initio computational chemistry programs 
 designed for add them many functionalities non available in these packages.
 Copyright (C) 2020 Abdulrahman Allouche (University Lyon 1)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
********************************************************************************/

#ifndef __CCHEMILIB_QUANTUMMECHANICSMD_H__
#define __CCHEMILIB_QUANTUMMECHANICSMD_H__

typedef struct _QuantumMechanicsMD  QuantumMechanicsMD;

struct _QuantumMechanicsMD
{
	QuantumMechanicsModel* qmModel;
	int numberOfAtoms;
	int updateFrequency;
	double** coordinatesOld; 
	double** a; 
	double** aold; 
	double dt;
	double dt_2;
	double dt2_2;
	double dt_4;
	double dt_8;
	double dt2_8;
	MDIntegratorType integratorType;
	double potentialEnergy;
	double kineticEnergy;
	double totalEnergy;
	double kelvin;
	boolean* moved;
	boolean* update;

	double* positionFriction;
	double* velocityFriction;
	double* accelarationFriction;
	double** positionRandom;
	double** velocityRandom;
	double* gamma;
	double friction;
	double temperature;
	double collide;
	MDThermostatType thermostat;

/* QTB && LANGEVIN */
	double* theta;
/* QTB */
	double* Ht;
	double** rnoise;
	int Nf;
	double h;
	int M;

	FILE* fileTraj;
	FILE* fileProp;
	int nvariables;
	int index;
#define MAXNH 2
	double xNH[MAXNH];
	double vNH[MAXNH];
	double qNH[MAXNH];
	double gNH[MAXNH];
};
void	freeQuantumMechanicsMD(QuantumMechanicsMD* molecularDynamics);
void	runQuantumMechanicsMD(
		QuantumMechanicsMD* molecularDynamics, QuantumMechanicsModel* qmModel, 
		int updateFrequency, 
		double heatTime, double equiTime, double runTime, double coolTime, 
		double heatTemperature, double equiTemperature, double runTemperature, double coolTemperature, 
		double stepSize,
		MDIntegratorType integratorType, 
		MDThermostatType thermostat,
		double friction,
		double omegaMax,
		int Nf,
		double collide,
		double qNH,
		char* fileNameTraj,
		char* fileNameProp
		);
QuantumMechanicsModel**	runQuantumMechanicsMDConfo(
		QuantumMechanicsMD* molecularDynamics, QuantumMechanicsModel* qmModel, 
		int updateFrequency, 
		double heatTime, double equiTime, double runTime, 
		double heatTemperature, double equiTemperature, double runTemperature,
		double stepSize,
		MDIntegratorType integratorType, 
		MDThermostatType thermostat,
		double friction,
		double omegaMax,
		int Nf,
		double collide,
		double qNH,
		int numberOfGeometries,
		char* fileNameTraj,
		char* fileNameProp
		);
QuantumMechanicsModel**    runQuantumMechanicsREMDConfo(
		QuantumMechanicsMD* molecularDynamics, QuantumMechanicsModel* qmModel, 
		int updateFrequency, 
		double heatTime, double equiTime, double runTime, 
		double heatTemperature, double runTemperature, double runTemperatureMax,
		double stepSize,
		MDIntegratorType integratorType,
		MDThermostatType thermostat,
		double friction,
		double omegaMax,
		int Nf,
		double collide,
		double qNH,
		int numberOfGeometries,
		int nTemperatures,
		int numberOfExchanges,
		char* fileNameTraj,
		char* fileNameProp
		);
void	freeQuantumMechanicsMD(QuantumMechanicsMD* molecularDynamics);
#endif /* __CCHEMILIB_QUANTUMMECHANICSMD_H__ */

