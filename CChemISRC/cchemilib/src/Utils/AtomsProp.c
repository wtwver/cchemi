/********************************************************************************
 cchemi is an interface to ab initio computational chemistry programs 
 designed for add them many functionalities non available in these packages.
 Copyright (C) 2010 Abdulrahman Allouche (University Lyon 1)

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

/* AtomsProp.c */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "../Utils/Types.h"
#include "../Utils/Constants.h"
#include "../Utils/AtomsProp.h"
#include "../Utils/Utils.h"

#define NATOMS 112
/********************************************************************************/
enum 
{
	DISPLAY_RADII = 0,
	COVALENT_RADII,
	VANDERWALS_RADII,
	MAXIMUM_BOND_VALENCE,
	MASSE,
	ELECTRONEGATIVITY,
	NUMBER_OF_ENTRYS,
};
/********************************************************************************/
static SAtomsProp AtomsProp[NATOMS];
/********************************************************************************/
void saveAtomsProp()
{
 int i;
 int j;
 char *atomspropfile;
 FILE *fd;

 atomspropfile = strdup_printf("%s%satomsprop.txt",cchemiDirectory(),DIR_SEPARATOR_S);
 fd = fopen(atomspropfile, "w");

 if(fd==NULL)
 {
	printf("I cannot open %s file to save the atoms properties\n",atomspropfile);
	return;
 }

 for(i=0;i<NATOMS;i++)
 {
  	fprintf(fd,"%s %s %d %f %f %f %f %d %f %f %d %d %d %d\n",
	AtomsProp[i].name,
	AtomsProp[i].symbol,
	AtomsProp[i].atomicNumber,
	AtomsProp[i].covalentRadii,
	AtomsProp[i].bondOrderRadii,
	AtomsProp[i].vanDerWaalsRadii,
	AtomsProp[i].radii,
	AtomsProp[i].maximumBondValence,
	AtomsProp[i].mass,
	AtomsProp[i].electronegativity,
	AtomsProp[i].color[0],
	AtomsProp[i].color[1],
	AtomsProp[i].color[2],
	AtomsProp[i].nIsotopes
	);
 	for(j=0;j<AtomsProp[i].nIsotopes;j++)
  		fprintf(fd,"%d %f %f\n", AtomsProp[i].iMass[j], AtomsProp[i].rMass[j], AtomsProp[i].abundances[j]);
  }
  fclose(fd);
}
/********************************************************************************/
boolean readAtomsProp()
{
 
 int i;
 int j;
 char *t;
 boolean OK=TRUE;
 char *atomspropfile;
 FILE *fd;
 
 atomspropfile = strdup_printf("%s%satomsprop.txt",cchemiDirectory(),DIR_SEPARATOR_S);
 fd = fopen(atomspropfile, "r");  

 t=malloc(BSIZE*sizeof(char));

 i=-1;
 if(fd!=NULL)
 {
  while(!feof(fd) && (i<NATOMS-1))
  {
    i++;
    fgets(t,BSIZE,fd);
    AtomsProp[i].name= malloc(30);
    AtomsProp[i].symbol= malloc(3);
    sscanf(t,"%s %s %d %lf %lf %lf %lf %d %lf %lf %d %d %d %d",
	AtomsProp[i].name,
	AtomsProp[i].symbol,
	&AtomsProp[i].atomicNumber,
	&AtomsProp[i].covalentRadii,
	&AtomsProp[i].bondOrderRadii,
	&AtomsProp[i].vanDerWaalsRadii,
	&AtomsProp[i].radii,
	&AtomsProp[i].maximumBondValence,
	&AtomsProp[i].mass,
	&AtomsProp[i].electronegativity,
	&AtomsProp[i].color[0],
	&AtomsProp[i].color[1],
	&AtomsProp[i].color[2],
	&AtomsProp[i].nIsotopes
        );
 	for(j=0;j<AtomsProp[i].nIsotopes;j++)
	{
    		fgets(t,BSIZE,fd);
    		sscanf(t,"%d %lf %lf", &AtomsProp[i].iMass[j], &AtomsProp[i].rMass[j], &AtomsProp[i].abundances[j]);
	}
  }
 }
 else
    OK  = FALSE;
 free(t);
 return OK;
}
/********************************************************************************/
double getMassFromSymbol(char* symbol)
{
	int i;
	for(i=0;i<NATOMS;i++)
	{
		if (!strcmp(symbol,AtomsProp[i].symbol))
			return AtomsProp[i].mass;
	}
	return 1.0;
}
/******************************************************************/
double getAtomicNumberFromSymbol(char* symbol)
{
	int i;
        static char symb[5];
	int l;

	if(!symbol) return 0.0;
	l = strlen(symbol);
	symb[0] = toupper(symbol[0]);
	if(l>1) symb[1]=tolower(symbol[1]);
	if(l>2) symb[2]=tolower(symbol[1]);
	symb[l] = '\0';
	
	for(i=0;i<NATOMS;i++)
	{
		if (!strcmp(symb,AtomsProp[i].symbol))
		{
			return AtomsProp[i].atomicNumber;
		}
	}
	return -1.0;
}
/********************************************************************************/
char* getSymbolUsingZ(int z)
{
	int i;
	
	for(i=0;i<NATOMS;i++)
	{
		if(z == (int)AtomsProp[i].atomicNumber)
		{
			return strdup(AtomsProp[i].symbol);
		}
	}
	return strdup("X");
}
/********************************************************************************/
boolean testAtomDefine(char *Symb)
{
	int i;
	
	for(i=0;i<NATOMS;i++)
	{
		if (!strcmp(Symb,AtomsProp[i].symbol))
 		{
			return TRUE;
		}

	}
	return FALSE;
}
/********************************************************************************/
void propAtomFree(SAtomsProp* prop)
{
	if(prop->name) free(prop->name);
	if(prop->symbol) free(prop->symbol);
}
/********************************************************************************/
SAtomsProp propAtomGet(const char *Symb)
{
	int i;
	int j;
        SAtomsProp temp;
	int c;
	
	for(i=0;i<NATOMS;i++)
	{
		if (!strcmp(Symb,AtomsProp[i].symbol))
 		{
			temp.name   = strdup(AtomsProp[i].name);
			temp.symbol   = strdup(AtomsProp[i].symbol);
			temp.atomicNumber      = AtomsProp[i].atomicNumber;
			temp.covalentRadii    = AtomsProp[i].covalentRadii;
			temp.bondOrderRadii    = AtomsProp[i].bondOrderRadii;
			temp.vanDerWaalsRadii   = AtomsProp[i].vanDerWaalsRadii;
			temp.radii = AtomsProp[i].radii;
			temp.maximumBondValence = AtomsProp[i].maximumBondValence;
			temp.mass    = AtomsProp[i].mass;
			temp.electronegativity    = AtomsProp[i].electronegativity;
			for(c=0;c<3;c++) temp.color[c]  = AtomsProp[i].color[c];
			temp.nIsotopes  = AtomsProp[i].nIsotopes;
			for(j=0;j<temp.nIsotopes;j++) temp.iMass[j] = AtomsProp[i].iMass[j];
			for(j=0;j<temp.nIsotopes;j++) temp.rMass[j] = AtomsProp[i].rMass[j];
			for(j=0;j<temp.nIsotopes;j++) temp.abundances[j] = AtomsProp[i].abundances[j];

			return temp;
		}

	}
	printf(("Warning : Sorry , Your atom %s is not defined\n"),Symb);
        i=0;
	temp.name   = strdup(AtomsProp[i].name);
	temp.symbol   = strdup(AtomsProp[i].symbol);
	temp.atomicNumber      = AtomsProp[i].atomicNumber;
	temp.covalentRadii    = AtomsProp[i].covalentRadii;
	temp.bondOrderRadii    = AtomsProp[i].bondOrderRadii;
	temp.vanDerWaalsRadii   = AtomsProp[i].vanDerWaalsRadii;
	temp.maximumBondValence = AtomsProp[i].maximumBondValence;
	temp.radii = AtomsProp[i].radii;
	temp.mass = AtomsProp[i].mass;
	temp.electronegativity = AtomsProp[i].electronegativity;
	for(c=0;c<3;c++) temp.color[c]  = AtomsProp[i].color[c];
	temp.nIsotopes = AtomsProp[i].nIsotopes;
	for(j=0;j<temp.nIsotopes;j++) temp.iMass[j] = AtomsProp[i].iMass[j];
	for(j=0;j<temp.nIsotopes;j++) temp.rMass[j] = AtomsProp[i].rMass[j];
	for(j=0;j<temp.nIsotopes;j++) temp.abundances[j] = AtomsProp[i].abundances[j];
	return temp;
}
/******************************************************************/
char *symbAtomGet(int Z)
{
	int i;
        char *symb;
       
        symb = strdup("X");
	
	for(i=0;i<NATOMS;i++)
	{
		if (Z==AtomsProp[i].atomicNumber)
 		{
		symb =strdup(AtomsProp[i].symbol);
 		break;
		}

	}
	return symb;
}
/******************************************************************/
void addIsotopeToAtom(char* symbol, int atomicNumber, int iMass, double rMass, double abundance)
{
	int i = (int)atomicNumber-1;
	int j = 0;
	if(i>=NATOMS) return;
	if(i<0) return;
	if(strcmp(symbol,"Xx")==0) return;
	if(strcmp(symbol,"X")==0) return;
	if(i>=109) return;
	if(AtomsProp[i].nIsotopes>= MAXISOTOP) return;

	j = AtomsProp[i].nIsotopes;
	AtomsProp[i].nIsotopes++;
	AtomsProp[i].iMass[j] = iMass;
	AtomsProp[i].rMass[j] = rMass;
	AtomsProp[i].abundances[j] = abundance;
	if(j==0) AtomsProp[i].mass = rMass;
}
/******************************************************************/
void setPropForOneAtom(char* name, char* symbol, int atomicNumber, 
		double covalentRadii, double bondOrderRadii, double vanDerWaalsRadii, 
		int maximumBondValence, double mass, double electronegativity, double red, double green, double blue)
{
	int i = (int)atomicNumber-1;
	if(strcmp(symbol,"Xx")==0) i = 109;
	if(strcmp(symbol,"X")==0) i = 110;

	AtomsProp[i].name = strdup(name);
	AtomsProp[i].symbol = strdup(symbol);
	AtomsProp[i].atomicNumber = atomicNumber;
	AtomsProp[i].covalentRadii = (covalentRadii+0.2)*ANGTOBOHR;
	AtomsProp[i].bondOrderRadii = bondOrderRadii*ANGTOBOHR;
	AtomsProp[i].vanDerWaalsRadii = vanDerWaalsRadii*ANGTOBOHR;
	/*AtomsProp[i].radii = covalentRadii/1.5*ANG_TO_BOHR;*/
	AtomsProp[i].radii = vanDerWaalsRadii*0.2*ANGTOBOHR;
	AtomsProp[i].maximumBondValence = maximumBondValence;
	AtomsProp[i].mass = mass;
	AtomsProp[i].electronegativity = electronegativity/27.21;
	AtomsProp[i].color[0] = (red*65535);
	AtomsProp[i].color[1] = (green*65535);
	AtomsProp[i].color[2] = (blue*65535);
	AtomsProp[i].nIsotopes =  0;
}
/******************************************************************/
void defineDefaultAtomsProp()
{
	setPropForOneAtom("Hydrogen", "H", 1, 0.230000, 0.330000, 1.200000, 1, 1.007940, 2.100000, 1.000000, 1.000000, 1.000000);
	setPropForOneAtom("Helium", "He", 2, 0.930000, 0.700000, 1.400000, 0, 4.002602, 0.000000, 0.850000, 1.000000, 1.000000);
	setPropForOneAtom("Lithium", "Li", 3, 0.680000, 1.230000, 1.820000, 1, 6.941000, 0.980000, 0.800000, 0.500000, 1.000000);
	setPropForOneAtom("Beryllium", "Be", 4, 0.350000, 0.900000, 1.700000, 2, 9.012182, 1.570000, 0.760000, 1.000000, 0.000000);
	setPropForOneAtom("Boron", "B", 5, 0.830000, 0.820000, 2.080000, 3, 10.811000, 2.040000, 1.000000, 0.710000, 0.710000);
	setPropForOneAtom("Carbon", "C", 6, 0.680000, 0.770000, 1.950000, 4, 12.010700, 2.550000, 0.500000, 0.500000, 0.500000);
	setPropForOneAtom("Nitrogen", "N", 7, 0.680000, 0.700000, 1.850000, 3, 14.006700, 3.040000, 0.050000, 0.050000, 1.000000);
	setPropForOneAtom("Oxygen", "O", 8, 0.680000, 0.660000, 1.700000, 2, 15.999400, 3.440000, 1.000000, 0.050000, 0.050000);
	setPropForOneAtom("Fluorine", "F", 9, 0.640000, 0.611000, 1.730000, 1, 18.998404, 3.980000, 0.700000, 1.000000, 1.000000);
	setPropForOneAtom("Neon", "Ne", 10, 1.120000, 0.700000, 1.540000, 0, 20.179701, 0.000000, 0.700000, 0.890000, 0.960000);
	/*
	setPropForOneAtom("Sodium", "Na", 11, 0.970000, 1.540000, 2.270000, 1, 22.989771, 0.930000, 0.670000, 0.360000, 0.950000);
	*/
	setPropForOneAtom("Sodium", "Na", 11, 1.9, 3.06, 2.270000, 1, 22.989771, 0.930000, 0.670000, 0.360000, 0.950000);
	AtomsProp[10].radii /= 2;

	setPropForOneAtom("Magnesium", "Mg", 12, 1.100000, 1.360000, 1.730000, 2, 24.305000, 1.310000, 0.540000, 1.000000, 0.000000);
	setPropForOneAtom("Aluminium", "Al", 13, 1.350000, 1.180000, 2.050000, 6, 26.981539, 1.610000, 0.750000, 0.650000, 0.650000);
	setPropForOneAtom("Silicon", "Si", 14, 1.200000, 0.937000, 2.100000, 6, 28.085501, 1.900000, 0.500000, 0.600000, 0.600000);
	setPropForOneAtom("Phosphorus", "P", 15, 1.100000, 0.890000, 2.080000, 5, 30.973761, 2.190000, 1.000000, 0.500000, 0.000000);
	setPropForOneAtom("Sulphur", "S", 16, 1.020000, 1.040000, 2.000000, 6, 32.064999, 2.580000, 1.000000, 1.000000, 0.190000);
	setPropForOneAtom("Chlorine", "Cl", 17, 0.990000, 0.997000, 1.970000, 1, 35.452999, 3.160000, 0.120000, 0.940000, 0.120000);
	setPropForOneAtom("Argon", "Ar", 18, 1.570000, 1.740000, 1.880000, 0, 39.948002, 0.000000, 0.500000, 0.820000, 0.890000);
	setPropForOneAtom("Potassium", "K", 19, 1.330000, 2.030000, 2.750000, 1, 39.098301, 0.820000, 0.560000, 0.250000, 0.830000);
	setPropForOneAtom("Calcium", "Ca", 20, 0.990000, 1.740000, 1.973000, 2, 40.077999, 1.000000, 0.240000, 1.000000, 0.000000);
	setPropForOneAtom("Scandium", "Sc", 21, 1.440000, 1.440000, 1.700000, 6, 44.955910, 1.360000, 0.900000, 0.900000, 0.900000);
	setPropForOneAtom("Titanium", "Ti", 22, 1.470000, 1.320000, 1.700000, 6, 47.867001, 1.540000, 0.750000, 0.760000, 0.780000);
	setPropForOneAtom("Vanadium", "V", 23, 1.330000, 1.220000, 1.700000, 6, 50.941502, 1.630000, 0.650000, 0.650000, 0.670000);
	setPropForOneAtom("Chromium", "Cr", 24, 1.350000, 1.180000, 1.700000, 6, 51.996101, 1.660000, 0.540000, 0.600000, 0.780000);
	setPropForOneAtom("Manganese", "Mn", 25, 1.350000, 1.170000, 1.700000, 8, 54.938049, 1.550000, 0.610000, 0.480000, 0.780000);
	setPropForOneAtom("Iron", "Fe", 26, 1.340000, 1.170000, 1.700000, 6, 55.845001, 1.830000, 0.500000, 0.480000, 0.780000);
	setPropForOneAtom("Cobalt", "Co", 27, 1.330000, 1.160000, 1.700000, 6, 58.933201, 1.880000, 0.440000, 0.480000, 0.780000);
	setPropForOneAtom("Nickel", "Ni", 28, 1.500000, 1.150000, 1.630000, 6, 58.693401, 1.910000, 0.360000, 0.480000, 0.760000);
	setPropForOneAtom("Copper", "Cu", 29, 1.520000, 1.170000, 1.400000, 6, 63.546001, 1.900000, 1.000000, 0.480000, 0.380000);
	setPropForOneAtom("Zinc", "Zn", 30, 1.450000, 1.250000, 1.390000, 6, 65.408997, 1.650000, 0.490000, 0.500000, 0.690000);
	setPropForOneAtom("Gallium", "Ga", 31, 1.220000, 1.260000, 1.870000, 3, 69.723000, 1.810000, 0.760000, 0.560000, 0.560000);
	setPropForOneAtom("Germanium", "Ge", 32, 1.170000, 1.188000, 1.700000, 4, 72.639999, 2.010000, 0.400000, 0.560000, 0.560000);
	setPropForOneAtom("Arsenic", "As", 33, 1.210000, 1.200000, 1.850000, 3, 74.921600, 2.180000, 0.740000, 0.500000, 0.890000);
	setPropForOneAtom("Selenium", "Se", 34, 1.220000, 1.170000, 1.900000, 2, 78.959999, 2.550000, 1.000000, 0.630000, 0.000000);
	setPropForOneAtom("Bromine", "Br", 35, 1.210000, 1.167000, 2.100000, 1, 79.903999, 2.960000, 0.650000, 0.160000, 0.160000);
	setPropForOneAtom("Krypton", "Kr", 36, 1.910000, 1.910000, 2.020000, 0, 83.797997, 0.000000, 0.360000, 0.720000, 0.820000);
	setPropForOneAtom("Rubidium", "Rb", 37, 1.470000, 2.160000, 1.700000, 1, 85.467796, 0.820000, 0.440000, 0.180000, 0.690000);
	setPropForOneAtom("Strontium", "Sr", 38, 1.120000, 1.910000, 1.700000, 2, 87.620003, 0.950000, 0.000000, 1.000000, 0.000000);
	setPropForOneAtom("Yttrium", "Y", 39, 1.780000, 1.620000, 1.700000, 6, 88.905853, 1.220000, 0.580000, 1.000000, 1.000000);
	setPropForOneAtom("Zirconium", "Zr", 40, 1.560000, 1.450000, 1.700000, 6, 91.223999, 1.330000, 0.580000, 0.880000, 0.880000);
	setPropForOneAtom("Niobium", "Nb", 41, 1.480000, 1.340000, 1.700000, 6, 92.906380, 1.600000, 0.450000, 0.760000, 0.790000);
	setPropForOneAtom("Molybdenum", "Mo", 42, 1.470000, 1.300000, 1.700000, 6, 95.940002, 2.160000, 0.330000, 0.710000, 0.710000);
	setPropForOneAtom("Technetium", "Tc", 43, 1.350000, 1.270000, 1.700000, 6, 98.000000, 1.900000, 0.230000, 0.620000, 0.620000);
	setPropForOneAtom("Ruthenium", "Ru", 44, 1.400000, 1.250000, 1.700000, 6, 101.070000, 2.200000, 0.140000, 0.560000, 0.560000);
	setPropForOneAtom("Rhodium", "Rh", 45, 1.450000, 1.250000, 1.700000, 6, 102.905502, 2.280000, 0.040000, 0.490000, 0.550000);
	setPropForOneAtom("Palladium", "Pd", 46, 1.500000, 1.280000, 1.630000, 6, 106.419998, 2.200000, 0.000000, 0.410000, 0.520000);
	setPropForOneAtom("Silver", "Ag", 47, 1.590000, 1.340000, 1.720000, 6, 107.868202, 1.930000, 0.880000, 0.880000, 1.000000);
	setPropForOneAtom("Cadmium", "Cd", 48, 1.690000, 1.480000, 1.580000, 6, 112.411003, 1.690000, 1.000000, 0.850000, 0.560000);
	setPropForOneAtom("Indium", "In", 49, 1.630000, 1.440000, 1.930000, 3, 114.818001, 1.780000, 0.650000, 0.460000, 0.450000);
	setPropForOneAtom("Tin", "Sn", 50, 1.460000, 1.385000, 2.170000, 4, 118.709999, 1.960000, 0.400000, 0.500000, 0.500000);
	setPropForOneAtom("Antimony", "Sb", 51, 1.460000, 1.400000, 2.200000, 3, 121.760002, 2.050000, 0.620000, 0.390000, 0.710000);
	setPropForOneAtom("Tellurium", "Te", 52, 1.470000, 1.378000, 2.060000, 2, 127.599998, 2.100000, 0.830000, 0.480000, 0.000000);
	setPropForOneAtom("Iodine", "I", 53, 1.400000, 1.387000, 2.150000, 1, 126.904472, 2.660000, 0.580000, 0.000000, 0.580000);
	setPropForOneAtom("Xenon", "Xe", 54, 1.980000, 1.980000, 2.160000, 0, 131.292999, 2.600000, 0.260000, 0.620000, 0.690000);
	setPropForOneAtom("Cesium", "Cs", 55, 1.670000, 2.350000, 1.700000, 1, 132.905457, 0.790000, 0.340000, 0.090000, 0.560000);
	setPropForOneAtom("Barium", "Ba", 56, 1.340000, 1.980000, 1.700000, 2, 137.326996, 0.890000, 0.000000, 0.790000, 0.000000);
	setPropForOneAtom("Lanthanum", "La", 57, 1.870000, 1.690000, 1.700000, 12, 138.905502, 1.100000, 0.440000, 0.830000, 1.000000);
	setPropForOneAtom("Cerium", "Ce", 58, 1.830000, 1.830000, 1.700000, 6, 140.115997, 1.120000, 1.000000, 1.000000, 0.780000);
	setPropForOneAtom("Praseodymium", "Pr", 59, 1.820000, 1.820000, 1.700000, 6, 140.907654, 1.130000, 0.850000, 1.000000, 0.780000);
	setPropForOneAtom("Neodymium", "Nd", 60, 1.810000, 1.810000, 1.700000, 6, 144.240005, 1.140000, 0.780000, 1.000000, 0.780000);
	setPropForOneAtom("Promethium", "Pm", 61, 1.800000, 1.800000, 1.700000, 6, 145.000000, 1.130000, 0.640000, 1.000000, 0.780000);
	setPropForOneAtom("Samarium", "Sm", 62, 1.800000, 1.800000, 1.700000, 6, 150.360001, 1.170000, 0.560000, 1.000000, 0.780000);
	setPropForOneAtom("Europium", "Eu", 63, 1.990000, 1.990000, 1.700000, 6, 151.964005, 1.200000, 0.380000, 1.000000, 0.780000);
	setPropForOneAtom("Gadolinium", "Gd", 64, 1.790000, 1.790000, 1.700000, 6, 157.250000, 1.200000, 0.270000, 1.000000, 0.780000);
	setPropForOneAtom("Terbium", "Tb", 65, 1.760000, 1.760000, 1.700000, 6, 158.925339, 1.100000, 0.190000, 1.000000, 0.780000);
	setPropForOneAtom("Dysprosium", "Dy", 66, 1.750000, 1.750000, 1.700000, 6, 162.500000, 1.220000, 0.120000, 1.000000, 0.780000);
	setPropForOneAtom("Holmium", "Ho", 67, 1.740000, 1.740000, 1.700000, 6, 164.930313, 1.230000, 0.000000, 1.000000, 0.610000);
	setPropForOneAtom("Erbium", "Er", 68, 1.730000, 1.730000, 1.700000, 6, 167.259003, 1.240000, 0.000000, 0.900000, 0.460000);
	setPropForOneAtom("Thulium", "Tm", 69, 1.720000, 1.720000, 1.700000, 6, 168.934204, 1.250000, 0.000000, 0.830000, 0.320000);
	setPropForOneAtom("Ytterbium", "Yb", 70, 1.940000, 1.940000, 1.700000, 6, 173.039993, 1.100000, 0.000000, 0.750000, 0.220000);
	setPropForOneAtom("Lutetium", "Lu", 71, 1.720000, 1.720000, 1.700000, 6, 174.966995, 1.270000, 0.000000, 0.670000, 0.140000);
	setPropForOneAtom("Hafnium", "Hf", 72, 1.570000, 1.440000, 1.700000, 6, 178.490005, 1.300000, 0.300000, 0.760000, 1.000000);
	setPropForOneAtom("Tantalum", "Ta", 73, 1.430000, 1.340000, 1.700000, 6, 180.947906, 1.500000, 0.300000, 0.650000, 1.000000);
	setPropForOneAtom("Tungsten", "W", 74, 1.370000, 1.300000, 1.700000, 6, 183.839996, 2.360000, 0.130000, 0.580000, 0.840000);
	setPropForOneAtom("Rhenium", "Re", 75, 1.350000, 1.280000, 1.700000, 6, 186.207001, 1.900000, 0.150000, 0.490000, 0.670000);
	setPropForOneAtom("Osmium", "Os", 76, 1.370000, 1.260000, 1.700000, 6, 190.229996, 2.200000, 0.150000, 0.400000, 0.590000);
	setPropForOneAtom("Iridium", "Ir", 77, 1.320000, 1.270000, 1.700000, 6, 192.216995, 2.200000, 0.090000, 0.330000, 0.530000);
	setPropForOneAtom("Platinum", "Pt", 78, 1.500000, 1.300000, 1.720000, 6, 195.078003, 2.280000, 0.960000, 0.930000, 0.820000);
	setPropForOneAtom("Gold", "Au", 79, 1.500000, 1.340000, 1.660000, 6, 196.966553, 2.540000, 0.800000, 0.820000, 0.120000);
	setPropForOneAtom("Mercury", "Hg", 80, 1.700000, 1.490000, 1.550000, 6, 200.589996, 2.000000, 0.710000, 0.710000, 0.760000);
	setPropForOneAtom("Thallium", "Tl", 81, 1.550000, 1.480000, 1.960000, 3, 204.383301, 2.040000, 0.650000, 0.330000, 0.300000);
	setPropForOneAtom("Lead", "Pb", 82, 1.540000, 1.480000, 2.020000, 4, 207.199997, 2.330000, 0.340000, 0.350000, 0.380000);
	setPropForOneAtom("Bismuth", "Bi", 83, 1.540000, 1.450000, 1.700000, 3, 208.980377, 2.020000, 0.620000, 0.310000, 0.710000);
	setPropForOneAtom("Polonium", "Po", 84, 1.680000, 1.460000, 1.700000, 2, 209.000000, 2.000000, 0.670000, 0.360000, 0.000000);
	setPropForOneAtom("Astatine", "At", 85, 1.700000, 1.450000, 1.700000, 1, 210.000000, 2.200000, 0.460000, 0.310000, 0.270000);
	setPropForOneAtom("Radon", "Rn", 86, 2.400000, 2.400000, 1.700000, 0, 222.000000, 0.000000, 0.260000, 0.510000, 0.590000);
	setPropForOneAtom("Francium", "Fr", 87, 2.000000, 2.000000, 1.700000, 1, 223.000000, 0.700000, 0.260000, 0.000000, 0.400000);
	setPropForOneAtom(" radium", "Ra", 88, 1.900000, 1.900000, 1.700000, 2, 226.000000, 0.890000, 0.000000, 0.490000, 0.000000);
	setPropForOneAtom("Actinium", "Ac", 89, 1.880000, 1.880000, 1.700000, 6, 227.000000, 1.100000, 0.440000, 0.670000, 0.980000);
	setPropForOneAtom("Thorium", "Th", 90, 1.790000, 1.790000, 1.700000, 6, 232.038101, 1.300000, 0.000000, 0.730000, 1.000000);
	setPropForOneAtom("Protactinium", "Pa", 91, 1.610000, 1.610000, 1.700000, 6, 231.035873, 1.500000, 0.000000, 0.630000, 1.000000);
	setPropForOneAtom("Uranium", "U", 92, 1.580000, 1.580000, 1.860000, 6, 238.028915, 1.380000, 0.000000, 0.560000, 1.000000);
	setPropForOneAtom("Neptunium", "Np", 93, 1.550000, 1.550000, 1.700000, 6, 237.000000, 1.360000, 0.000000, 0.500000, 1.000000);
	setPropForOneAtom("Plutionium", "Pu", 94, 1.530000, 1.530000, 1.700000, 6, 244.000000, 1.280000, 0.000000, 0.420000, 1.000000);
	setPropForOneAtom("Americium", "Am", 95, 1.510000, 1.070000, 1.700000, 6, 243.000000, 1.300000, 0.330000, 0.360000, 0.950000);
	setPropForOneAtom("Curium", "Cm", 96, 1.500000, 0.000000, 1.700000, 6, 247.000000, 1.300000, 0.470000, 0.360000, 0.890000);
	setPropForOneAtom("Berkelium", "Bk", 97, 1.500000, 0.000000, 1.700000, 6, 247.000000, 1.300000, 0.540000, 0.310000, 0.890000);
	setPropForOneAtom("Californium", "Cf", 98, 1.500000, 0.000000, 1.700000, 6, 251.000000, 1.300000, 0.630000, 0.210000, 0.830000);
	setPropForOneAtom("Einsteinium", "Es", 99, 1.500000, 0.000000, 1.700000, 6, 252.000000, 1.300000, 0.700000, 0.120000, 0.830000);
	setPropForOneAtom("Fermium", "Fm", 100, 1.500000, 0.000000, 1.700000, 6, 257.000000, 1.300000, 0.700000, 0.120000, 0.730000);
	setPropForOneAtom("Mendelevium", "Md", 101, 1.500000, 0.000000, 1.700000, 6, 258.000000, 1.300000, 0.700000, 0.050000, 0.650000);
	setPropForOneAtom("Nobelium", "No", 102, 1.500000, 0.000000, 1.700000, 6, 259.000000, 1.300000, 0.740000, 0.050000, 0.530000);
	setPropForOneAtom("Lawrencium", "Lr", 103, 1.500000, 0.000000, 1.700000, 6, 262.000000, 1.300000, 0.780000, 0.000000, 0.400000);
	setPropForOneAtom("Rutherfordium", "Rf", 104, 1.600000, 0.000000, 1.700000, 6, 261.000000, 0.000000, 0.800000, 0.000000, 0.350000);
	setPropForOneAtom("Dubnium", "Db", 105, 1.600000, 0.000000, 1.700000, 6, 262.000000, 0.000000, 0.820000, 0.000000, 0.310000);
	setPropForOneAtom("Seaborgium", "Sg", 106, 1.600000, 0.000000, 1.700000, 6, 263.000000, 0.000000, 0.850000, 0.000000, 0.270000);
	setPropForOneAtom("Bohrium", "Bh", 107, 1.600000, 0.000000, 1.700000, 6, 264.000000, 0.000000, 0.880000, 0.000000, 0.220000);
	setPropForOneAtom("Hassium", "Hs", 108, 1.600000, 0.000000, 1.700000, 6, 265.000000, 0.000000, 0.900000, 0.000000, 0.180000);
	setPropForOneAtom("Meitnerium", "Mt", 109, 1.600000, 0.000000, 1.700000, 6, 268.000000, 0.000000, 0.920000, 0.000000, 0.150000);
	setPropForOneAtom("Dummy", "Xx", 0, 0.000000, 0.000000, 0.000000, 0, 1.000000, 0.000000, 0.070000, 0.500000, 0.700000);
	setPropForOneAtom("Dummy", "X", 0, 0.000000, 0.000000, 0.000000, 0, 1.000000, 0.000000, 0.070000, 0.500000, 0.700000);
	setPropForOneAtom("Tv", "Tv", 110, 0.000000, 0.000000, 0.000000, 0, 1.000000, 0.000000, 0.070000, 0.500000, 0.700000);
	addIsotopeToAtom("H", 1, 1,     1.007825032100,    99.98850000);
	addIsotopeToAtom("H", 1, 2,     2.014101778000,     0.01150000);
	addIsotopeToAtom("H", 1, 3,     3.016049267500,     0.00000000);
	addIsotopeToAtom("He", 2, 4,     4.002603249700,    99.99986300);
	addIsotopeToAtom("He", 2, 3,     3.016029309700,     0.00013700);
	addIsotopeToAtom("Li", 3, 7,     7.016004000000,    92.41000000);
	addIsotopeToAtom("Li", 3, 6,     6.015122300000,     7.59000000);
	addIsotopeToAtom("Be", 4, 9,     9.012182100000,   100.00000000);
	addIsotopeToAtom("B", 5, 11,    11.009305500000,    80.10000000);
	addIsotopeToAtom("B", 5, 10,    10.012937000000,    19.90000000);
	addIsotopeToAtom("C", 6, 12,    12.000000000000,    98.93000000);
	addIsotopeToAtom("C", 6, 13,    13.003354837800,     1.07000000);
	addIsotopeToAtom("C", 6, 14,    14.003241988000,     0.00000000);
	addIsotopeToAtom("N", 7, 14,    14.003074005200,    99.63200000);
	addIsotopeToAtom("N", 7, 15,    15.000108898400,     0.36800000);
	addIsotopeToAtom("O", 8, 16,    15.994914622100,    99.75700000);
	addIsotopeToAtom("O", 8, 18,    17.999160400000,     0.20500000);
	addIsotopeToAtom("O", 8, 17,    16.999131500000,     0.03800000);
	addIsotopeToAtom("F", 9, 19,    18.998403200000,   100.00000000);
	addIsotopeToAtom("Ne", 10, 20,    19.992440175900,    90.48000000);
	addIsotopeToAtom("Ne", 10, 22,    21.991385510000,     9.25000000);
	addIsotopeToAtom("Ne", 10, 21,    20.993846740000,     0.27000000);
	addIsotopeToAtom("Na", 11, 23,    22.989769670000,   100.00000000);
	addIsotopeToAtom("Mg", 12, 24,    23.985041900000,    78.99000000);
	addIsotopeToAtom("Mg", 12, 26,    25.982593040000,    11.01000000);
	addIsotopeToAtom("Mg", 12, 25,    24.985837020000,    10.00000000);
	addIsotopeToAtom("Al", 13, 27,    26.981538440000,   100.00000000);
	addIsotopeToAtom("Si", 14, 28,    27.976926532700,    92.22970000);
	addIsotopeToAtom("Si", 14, 29,    28.976494720000,     4.68320000);
	addIsotopeToAtom("Si", 14, 30,    29.973770220000,     3.08720000);
	addIsotopeToAtom("P", 15, 31,    30.973761510000,   100.00000000);
	addIsotopeToAtom("S", 16, 32,    31.972070690000,    94.93000000);
	addIsotopeToAtom("S", 16, 34,    33.967866830000,     4.29000000);
	addIsotopeToAtom("S", 16, 33,    32.971458500000,     0.76000000);
	addIsotopeToAtom("S", 16, 36,    35.967080880000,     0.02000000);
	addIsotopeToAtom("Cl", 17, 35,    34.968852710000,    75.78000000);
	addIsotopeToAtom("Cl", 17, 37,    36.965902600000,    24.22000000);
	addIsotopeToAtom("Ar", 18, 40,    39.962383123000,    99.60030000);
	addIsotopeToAtom("Ar", 18, 36,    35.967546280000,     0.33650000);
	addIsotopeToAtom("Ar", 18, 38,    37.962732200000,     0.06320000);
	addIsotopeToAtom("K", 19, 39,    38.963706900000,    93.25810000);
	addIsotopeToAtom("K", 19, 41,    40.961825970000,     6.73020000);
	addIsotopeToAtom("K", 19, 40,    39.963998670000,     0.01170000);
	addIsotopeToAtom("Ca", 20, 40,    39.962591200000,    96.94100000);
	addIsotopeToAtom("Ca", 20, 44,    43.955481100000,     2.08600000);
	addIsotopeToAtom("Ca", 20, 42,    41.958618300000,     0.64700000);
	addIsotopeToAtom("Ca", 20, 48,    47.952534000000,     0.18700000);
	addIsotopeToAtom("Ca", 20, 43,    42.958766800000,     0.13500000);
	addIsotopeToAtom("Ca", 20, 46,    45.953692800000,     0.00400000);
	addIsotopeToAtom("Sc", 21, 45,    44.955910200000,   100.00000000);
	addIsotopeToAtom("Ti", 22, 48,    47.947947100000,    73.72000000);
	addIsotopeToAtom("Ti", 22, 46,    45.952629500000,     8.25000000);
	addIsotopeToAtom("Ti", 22, 47,    46.951763800000,     7.44000000);
	addIsotopeToAtom("Ti", 22, 49,    48.947870800000,     5.41000000);
	addIsotopeToAtom("Ti", 22, 50,    49.944792100000,     5.18000000);
	addIsotopeToAtom("V", 23, 51,    50.943963700000,    99.75000000);
	addIsotopeToAtom("V", 23, 50,    49.947162800000,     0.25000000);
	addIsotopeToAtom("Cr", 24, 52,    51.940511900000,    83.78900000);
	addIsotopeToAtom("Cr", 24, 53,    52.940653800000,     9.50100000);
	addIsotopeToAtom("Cr", 24, 50,    49.946049600000,     4.34500000);
	addIsotopeToAtom("Cr", 24, 54,    53.938884900000,     2.36500000);
	addIsotopeToAtom("Mn", 25, 55,    54.938049600000,   100.00000000);
	addIsotopeToAtom("Fe", 26, 56,    55.934942100000,    91.75400000);
	addIsotopeToAtom("Fe", 26, 54,    53.939614800000,     5.84500000);
	addIsotopeToAtom("Fe", 26, 57,    56.935398700000,     2.11900000);
	addIsotopeToAtom("Fe", 26, 58,    57.933280500000,     0.28200000);
	addIsotopeToAtom("Co", 27, 59,    58.933200200000,   100.00000000);
	addIsotopeToAtom("Ni", 28, 58,    57.935347900000,    68.07690000);
	addIsotopeToAtom("Ni", 28, 60,    59.930790600000,    26.22310000);
	addIsotopeToAtom("Ni", 28, 62,    61.928348800000,     3.63450000);
	addIsotopeToAtom("Ni", 28, 61,    60.931060400000,     1.13990000);
	addIsotopeToAtom("Ni", 28, 64,    63.927969600000,     0.92560000);
	addIsotopeToAtom("Cu", 29, 63,    62.929601100000,    69.17000000);
	addIsotopeToAtom("Cu", 29, 65,    64.927793700000,    30.83000000);
	addIsotopeToAtom("Zn", 30, 64,    63.929146600000,    48.63000000);
	addIsotopeToAtom("Zn", 30, 66,    65.926036800000,    27.90000000);
	addIsotopeToAtom("Zn", 30, 68,    67.924847600000,    18.75000000);
	addIsotopeToAtom("Zn", 30, 67,    66.927130900000,     4.10000000);
	addIsotopeToAtom("Zn", 30, 70,    69.925325000000,     0.62000000);
	addIsotopeToAtom("Ga", 31, 69,    68.925581000000,    60.10800000);
	addIsotopeToAtom("Ga", 31, 71,    70.924705000000,    39.89200000);
	addIsotopeToAtom("Ge", 32, 74,    73.921178200000,    36.28000000);
	addIsotopeToAtom("Ge", 32, 72,    71.922076200000,    27.54000000);
	addIsotopeToAtom("Ge", 32, 70,    69.924250400000,    20.84000000);
	addIsotopeToAtom("Ge", 32, 73,    72.923459400000,     7.73000000);
	addIsotopeToAtom("Ge", 32, 76,    75.921402700000,     7.61000000);
	addIsotopeToAtom("As", 33, 75,    74.921596400000,   100.00000000);
	addIsotopeToAtom("Se", 34, 80,    79.916521800000,    49.61000000);
	addIsotopeToAtom("Se", 34, 78,    77.917309500000,    23.77000000);
	addIsotopeToAtom("Se", 34, 76,    75.919214100000,     9.37000000);
	addIsotopeToAtom("Se", 34, 82,    81.916700000000,     8.73000000);
	addIsotopeToAtom("Se", 34, 77,    76.919914600000,     7.63000000);
	addIsotopeToAtom("Se", 34, 74,    73.922476600000,     0.89000000);
	addIsotopeToAtom("Br", 35, 79,    78.918337600000,    50.69000000);
	addIsotopeToAtom("Br", 35, 81,    80.916291000000,    49.31000000);
	addIsotopeToAtom("Kr", 36, 84,    83.911507000000,    57.00000000);
	addIsotopeToAtom("Kr", 36, 86,    85.910610300000,    17.30000000);
	addIsotopeToAtom("Kr", 36, 82,    81.913484600000,    11.58000000);
	addIsotopeToAtom("Kr", 36, 83,    82.914136000000,    11.49000000);
	addIsotopeToAtom("Kr", 36, 80,    79.916378000000,     2.28000000);
	addIsotopeToAtom("Kr", 36, 78,    77.920386000000,     0.35000000);
	addIsotopeToAtom("Rb", 37, 85,    84.911789300000,    72.17000000);
	addIsotopeToAtom("Rb", 37, 87,    86.909183500000,    27.83000000);
	addIsotopeToAtom("Sr", 38, 88,    87.905614300000,    82.58000000);
	addIsotopeToAtom("Sr", 38, 86,    85.909262400000,     9.86000000);
	addIsotopeToAtom("Sr", 38, 87,    86.908879300000,     7.00000000);
	addIsotopeToAtom("Sr", 38, 84,    83.913425000000,     0.56000000);
	addIsotopeToAtom("Y", 39, 89,    88.905847900000,   100.00000000);
	addIsotopeToAtom("Zr", 40, 90,    89.904703700000,    51.45000000);
	addIsotopeToAtom("Zr", 40, 94,    93.906315800000,    17.38000000);
	addIsotopeToAtom("Zr", 40, 92,    91.905040100000,    17.15000000);
	addIsotopeToAtom("Zr", 40, 91,    90.905645000000,    11.22000000);
	addIsotopeToAtom("Zr", 40, 96,    95.908276000000,     2.80000000);
	addIsotopeToAtom("Nb", 41, 93,    92.906377500000,   100.00000000);
	addIsotopeToAtom("Mo", 42, 98,    97.905407800000,    24.13000000);
	addIsotopeToAtom("Mo", 42, 96,    95.904678900000,    16.68000000);
	addIsotopeToAtom("Mo", 42, 95,    94.905841500000,    15.92000000);
	addIsotopeToAtom("Mo", 42, 92,    91.906810000000,    14.84000000);
	addIsotopeToAtom("Mo", 42, 100,    99.907477000000,     9.63000000);
	addIsotopeToAtom("Mo", 42, 97,    96.906021000000,     9.55000000);
	addIsotopeToAtom("Mo", 42, 94,    93.905087600000,     9.25000000);
	addIsotopeToAtom("Tc", 43, 97,    96.906365000000,    33.33333333);
	addIsotopeToAtom("Tc", 43, 98,    97.907216000000,    33.33333333);
	addIsotopeToAtom("Tc", 43, 99,    98.906254600000,    33.33333333);
	addIsotopeToAtom("Ru", 44, 102,   101.904349500000,    31.55000000);
	addIsotopeToAtom("Ru", 44, 104,   103.905430000000,    18.62000000);
	addIsotopeToAtom("Ru", 44, 101,   100.905582200000,    17.06000000);
	addIsotopeToAtom("Ru", 44, 99,    98.905939300000,    12.76000000);
	addIsotopeToAtom("Ru", 44, 100,    99.904219700000,    12.60000000);
	addIsotopeToAtom("Ru", 44, 96,    95.907598000000,     5.54000000);
	addIsotopeToAtom("Ru", 44, 98,    97.905287000000,     1.87000000);
	addIsotopeToAtom("Rh", 45, 103,   102.905504000000,   100.00000000);
	addIsotopeToAtom("Pd", 46, 106,   105.903483000000,    27.33000000);
	addIsotopeToAtom("Pd", 46, 108,   107.903894000000,    26.46000000);
	addIsotopeToAtom("Pd", 46, 105,   104.905084000000,    22.33000000);
	addIsotopeToAtom("Pd", 46, 110,   109.905152000000,    11.72000000);
	addIsotopeToAtom("Pd", 46, 104,   103.904035000000,    11.14000000);
	addIsotopeToAtom("Pd", 46, 102,   101.905608000000,     1.02000000);
	addIsotopeToAtom("Ag", 47, 107,   106.905093000000,    51.83900000);
	addIsotopeToAtom("Ag", 47, 109,   108.904756000000,    48.16100000);
	addIsotopeToAtom("Cd", 48, 114,   113.903358100000,    28.73000000);
	addIsotopeToAtom("Cd", 48, 112,   111.902757200000,    24.13000000);
	addIsotopeToAtom("Cd", 48, 111,   110.904182000000,    12.80000000);
	addIsotopeToAtom("Cd", 48, 110,   109.903006000000,    12.49000000);
	addIsotopeToAtom("Cd", 48, 113,   112.904400900000,    12.22000000);
	addIsotopeToAtom("Cd", 48, 116,   115.904755000000,     7.49000000);
	addIsotopeToAtom("Cd", 48, 106,   105.906458000000,     1.25000000);
	addIsotopeToAtom("Cd", 48, 108,   107.904183000000,     0.89000000);
	addIsotopeToAtom("In", 49, 115,   114.903878000000,    95.71000000);
	addIsotopeToAtom("In", 49, 113,   112.904061000000,     4.29000000);
	addIsotopeToAtom("Sn", 50, 120,   119.902196600000,    32.58000000);
	addIsotopeToAtom("Sn", 50, 118,   117.901606000000,    24.22000000);
	addIsotopeToAtom("Sn", 50, 116,   115.901744000000,    14.54000000);
	addIsotopeToAtom("Sn", 50, 119,   118.903309000000,     8.59000000);
	addIsotopeToAtom("Sn", 50, 117,   116.902954000000,     7.68000000);
	addIsotopeToAtom("Sn", 50, 124,   123.905274600000,     5.79000000);
	addIsotopeToAtom("Sn", 50, 122,   121.903440100000,     4.63000000);
	addIsotopeToAtom("Sn", 50, 112,   111.904821000000,     0.97000000);
	addIsotopeToAtom("Sn", 50, 114,   113.902782000000,     0.66000000);
	addIsotopeToAtom("Sn", 50, 115,   114.903346000000,     0.34000000);
	addIsotopeToAtom("Sb", 51, 121,   120.903818000000,    57.21000000);
	addIsotopeToAtom("Sb", 51, 123,   122.904215700000,    42.79000000);
	addIsotopeToAtom("Te", 52, 130,   129.906222800000,    34.08000000);
	addIsotopeToAtom("Te", 52, 128,   127.904461400000,    31.74000000);
	addIsotopeToAtom("Te", 52, 126,   125.903305500000,    18.84000000);
	addIsotopeToAtom("Te", 52, 125,   124.904424700000,     7.07000000);
	addIsotopeToAtom("Te", 52, 124,   123.902819500000,     4.74000000);
	addIsotopeToAtom("Te", 52, 122,   121.903047100000,     2.55000000);
	addIsotopeToAtom("Te", 52, 123,   122.904273000000,     0.89000000);
	addIsotopeToAtom("Te", 52, 120,   119.904020000000,     0.09000000);
	addIsotopeToAtom("I", 53, 127,   126.904468000000,   100.00000000);
	addIsotopeToAtom("Xe", 54, 132,   131.904154500000,    26.89000000);
	addIsotopeToAtom("Xe", 54, 129,   128.904779500000,    26.44000000);
	addIsotopeToAtom("Xe", 54, 131,   130.905081900000,    21.18000000);
	addIsotopeToAtom("Xe", 54, 134,   133.905394500000,    10.44000000);
	addIsotopeToAtom("Xe", 54, 136,   135.907220000000,     8.87000000);
	addIsotopeToAtom("Xe", 54, 130,   129.903507900000,     4.08000000);
	addIsotopeToAtom("Xe", 54, 128,   127.903530400000,     1.92000000);
	addIsotopeToAtom("Xe", 54, 126,   125.904269000000,     0.09000000);
	addIsotopeToAtom("Xe", 54, 124,   123.905895800000,     0.09000000);
	addIsotopeToAtom("Cs", 55, 133,   132.905447000000,   100.00000000);
	addIsotopeToAtom("Ba", 56, 138,   137.905241000000,    71.69800000);
	addIsotopeToAtom("Ba", 56, 137,   136.905821000000,    11.23200000);
	addIsotopeToAtom("Ba", 56, 136,   135.904570000000,     7.85400000);
	addIsotopeToAtom("Ba", 56, 135,   134.905683000000,     6.59200000);
	addIsotopeToAtom("Ba", 56, 134,   133.904503000000,     2.41700000);
	addIsotopeToAtom("Ba", 56, 130,   129.906310000000,     0.10600000);
	addIsotopeToAtom("Ba", 56, 132,   131.905056000000,     0.10100000);
	addIsotopeToAtom("La", 57, 139,   138.906348000000,    99.91000000);
	addIsotopeToAtom("La", 57, 138,   137.907107000000,     0.09000000);
	addIsotopeToAtom("Ce", 58, 140,   139.905434000000,    88.45000000);
	addIsotopeToAtom("Ce", 58, 142,   141.909240000000,    11.11400000);
	addIsotopeToAtom("Ce", 58, 138,   137.905986000000,     0.25100000);
	addIsotopeToAtom("Ce", 58, 136,   135.907140000000,     0.18500000);
	addIsotopeToAtom("Pr", 59, 141,   140.907648000000,   100.00000000);
	addIsotopeToAtom("Nd", 60, 142,   141.907719000000,    27.20000000);
	addIsotopeToAtom("Nd", 60, 144,   143.910083000000,    23.80000000);
	addIsotopeToAtom("Nd", 60, 146,   145.913112000000,    17.20000000);
	addIsotopeToAtom("Nd", 60, 143,   142.909810000000,    12.20000000);
	addIsotopeToAtom("Nd", 60, 145,   144.912569000000,     8.30000000);
	addIsotopeToAtom("Nd", 60, 148,   147.916889000000,     5.70000000);
	addIsotopeToAtom("Nd", 60, 150,   149.920887000000,     5.60000000);
	addIsotopeToAtom("Pm", 61, 145,   144.912744000000,    50.00000000);
	addIsotopeToAtom("Pm", 61, 147,   146.915134000000,    50.00000000);
	addIsotopeToAtom("Sm", 62, 152,   151.919728000000,    26.75000000);
	addIsotopeToAtom("Sm", 62, 154,   153.922205000000,    22.75000000);
	addIsotopeToAtom("Sm", 62, 147,   146.914893000000,    14.99000000);
	addIsotopeToAtom("Sm", 62, 149,   148.917180000000,    13.82000000);
	addIsotopeToAtom("Sm", 62, 148,   147.914818000000,    11.24000000);
	addIsotopeToAtom("Sm", 62, 150,   149.917271000000,     7.38000000);
	addIsotopeToAtom("Sm", 62, 144,   143.911995000000,     3.07000000);
	addIsotopeToAtom("Eu", 63, 153,   152.921226000000,    52.19000000);
	addIsotopeToAtom("Eu", 63, 151,   150.919846000000,    47.81000000);
	addIsotopeToAtom("Gd", 64, 158,   157.924101000000,    24.84000000);
	addIsotopeToAtom("Gd", 64, 160,   159.927051000000,    21.86000000);
	addIsotopeToAtom("Gd", 64, 156,   155.922120000000,    20.47000000);
	addIsotopeToAtom("Gd", 64, 157,   156.923957000000,    15.65000000);
	addIsotopeToAtom("Gd", 64, 155,   154.922619000000,    14.80000000);
	addIsotopeToAtom("Gd", 64, 154,   153.920862000000,     2.18000000);
	addIsotopeToAtom("Gd", 64, 152,   151.919788000000,     0.20000000);
	addIsotopeToAtom("Tb", 65, 159,   158.925343000000,   100.00000000);
	addIsotopeToAtom("Dy", 66, 164,   163.929171000000,    28.18000000);
	addIsotopeToAtom("Dy", 66, 162,   161.926795000000,    25.51000000);
	addIsotopeToAtom("Dy", 66, 163,   162.928728000000,    24.90000000);
	addIsotopeToAtom("Dy", 66, 161,   160.926930000000,    18.91000000);
	addIsotopeToAtom("Dy", 66, 160,   159.925194000000,     2.34000000);
	addIsotopeToAtom("Dy", 66, 158,   157.924405000000,     0.10000000);
	addIsotopeToAtom("Dy", 66, 156,   155.924278000000,     0.06000000);
	addIsotopeToAtom("Ho", 67, 165,   164.930319000000,   100.00000000);
	addIsotopeToAtom("Er", 68, 166,   165.930290000000,    33.61000000);
	addIsotopeToAtom("Er", 68, 168,   167.932368000000,    26.78000000);
	addIsotopeToAtom("Er", 68, 167,   166.932045000000,    22.93000000);
	addIsotopeToAtom("Er", 68, 170,   169.935460000000,    14.93000000);
	addIsotopeToAtom("Er", 68, 164,   163.929197000000,     1.61000000);
	addIsotopeToAtom("Er", 68, 162,   161.928775000000,     0.14000000);
	addIsotopeToAtom("Tm", 69, 169,   168.934211000000,   100.00000000);
	addIsotopeToAtom("Yb", 70, 174,   173.938858100000,    31.83000000);
	addIsotopeToAtom("Yb", 70, 172,   171.936377700000,    21.83000000);
	addIsotopeToAtom("Yb", 70, 173,   172.938206800000,    16.13000000);
	addIsotopeToAtom("Yb", 70, 171,   170.936322000000,    14.28000000);
	addIsotopeToAtom("Yb", 70, 176,   175.942568000000,    12.76000000);
	addIsotopeToAtom("Yb", 70, 170,   169.934759000000,     3.04000000);
	addIsotopeToAtom("Yb", 70, 168,   167.933894000000,     0.13000000);
	addIsotopeToAtom("Lu", 71, 175,   174.940767900000,    97.41000000);
	addIsotopeToAtom("Lu", 71, 176,   175.942682400000,     2.59000000);
	addIsotopeToAtom("Hf", 72, 180,   179.946548800000,    35.08000000);
	addIsotopeToAtom("Hf", 72, 178,   177.943697700000,    27.28000000);
	addIsotopeToAtom("Hf", 72, 177,   176.943220000000,    18.60000000);
	addIsotopeToAtom("Hf", 72, 179,   178.945815100000,    13.62000000);
	addIsotopeToAtom("Hf", 72, 176,   175.941401800000,     5.26000000);
	addIsotopeToAtom("Hf", 72, 174,   173.940040000000,     0.16000000);
	addIsotopeToAtom("Ta", 73, 181,   180.947996000000,    99.98800000);
	addIsotopeToAtom("Ta", 73, 180,   179.947466000000,     0.01200000);
	addIsotopeToAtom("W", 74, 184,   183.950932600000,    30.64000000);
	addIsotopeToAtom("W", 74, 186,   185.954362000000,    28.43000000);
	addIsotopeToAtom("W", 74, 182,   181.948206000000,    26.50000000);
	addIsotopeToAtom("W", 74, 183,   182.950224500000,    14.31000000);
	addIsotopeToAtom("W", 74, 180,   179.946706000000,     0.12000000);
	addIsotopeToAtom("Re", 75, 187,   186.955750800000,    62.60000000);
	addIsotopeToAtom("Re", 75, 185,   184.952955700000,    37.40000000);
	addIsotopeToAtom("Os", 76, 192,   191.961479000000,    40.78000000);
	addIsotopeToAtom("Os", 76, 190,   189.958445000000,    26.26000000);
	addIsotopeToAtom("Os", 76, 189,   188.958144900000,    16.15000000);
	addIsotopeToAtom("Os", 76, 188,   187.955836000000,    13.24000000);
	addIsotopeToAtom("Os", 76, 187,   186.955747900000,     1.96000000);
	addIsotopeToAtom("Os", 76, 186,   185.953838000000,     1.59000000);
	addIsotopeToAtom("Os", 76, 184,   183.952491000000,     0.02000000);
	addIsotopeToAtom("Ir", 77, 193,   192.962924000000,    62.70000000);
	addIsotopeToAtom("Ir", 77, 191,   190.960591000000,    37.30000000);
	addIsotopeToAtom("Pt", 78, 195,   194.964774000000,    33.83200000);
	addIsotopeToAtom("Pt", 78, 194,   193.962664000000,    32.96700000);
	addIsotopeToAtom("Pt", 78, 196,   195.964935000000,    25.24200000);
	addIsotopeToAtom("Pt", 78, 198,   197.967876000000,     7.16300000);
	addIsotopeToAtom("Pt", 78, 192,   191.961035000000,     0.78200000);
	addIsotopeToAtom("Pt", 78, 190,   189.959930000000,     0.01400000);
	addIsotopeToAtom("Au", 79, 197,   196.966552000000,   100.00000000);
	addIsotopeToAtom("Hg", 80, 202,   201.970626000000,    29.86000000);
	addIsotopeToAtom("Hg", 80, 200,   199.968309000000,    23.10000000);
	addIsotopeToAtom("Hg", 80, 199,   198.968262000000,    16.87000000);
	addIsotopeToAtom("Hg", 80, 201,   200.970285000000,    13.18000000);
	addIsotopeToAtom("Hg", 80, 198,   197.966752000000,     9.97000000);
	addIsotopeToAtom("Hg", 80, 204,   203.973476000000,     6.87000000);
	addIsotopeToAtom("Hg", 80, 196,   195.965815000000,     0.15000000);
	addIsotopeToAtom("Tl", 81, 205,   204.974412000000,    70.47600000);
	addIsotopeToAtom("Tl", 81, 203,   202.972329000000,    29.52400000);
	addIsotopeToAtom("Pb", 82, 208,   207.976636000000,    52.40000000);
	addIsotopeToAtom("Pb", 82, 206,   205.974449000000,    24.10000000);
	addIsotopeToAtom("Pb", 82, 207,   206.975881000000,    22.10000000);
	addIsotopeToAtom("Pb", 82, 204,   203.973029000000,     1.40000000);
	addIsotopeToAtom("Bi", 83, 209,   208.980383000000,   100.00000000);
	addIsotopeToAtom("Po", 84, 209,   208.982416000000,    50.00000000);
	addIsotopeToAtom("Po", 84, 210,   209.982857000000,    50.00000000);
	addIsotopeToAtom("At", 85, 210,   209.987131000000,    50.00000000);
	addIsotopeToAtom("At", 85, 211,   210.987481000000,    50.00000000);
	addIsotopeToAtom("Rn", 86, 211,   210.990585000000,    33.33333333);
	addIsotopeToAtom("Rn", 86, 220,   220.011384100000,    33.33333333);
	addIsotopeToAtom("Rn", 86, 222,   222.017570500000,    33.33333333);
	addIsotopeToAtom("Fr", 87, 223,   223.019730700000,   100.00000000);
	addIsotopeToAtom("Ra", 88, 223,   223.018497000000,    25.00000000);
	addIsotopeToAtom("Ra", 88, 224,   224.020202000000,    25.00000000);
	addIsotopeToAtom("Ra", 88, 226,   226.025402600000,    25.00000000);
	addIsotopeToAtom("Ra", 88, 228,   228.031064100000,    25.00000000);
	addIsotopeToAtom("Ac", 89, 227,   227.027747000000,   100.00000000);
	addIsotopeToAtom("Th", 90, 232,   232.038050400000,   100.00000000);
	addIsotopeToAtom("Th", 90, 230,   230.033126600000,     0.00000000);
	addIsotopeToAtom("Pa", 91, 231,   231.035878900000,   100.00000000);
	addIsotopeToAtom("U", 92, 238,   238.050782600000,    99.27450000);
	addIsotopeToAtom("U", 92, 235,   235.043923100000,     0.72000000);
	addIsotopeToAtom("U", 92, 234,   234.040945600000,     0.00550000);
	addIsotopeToAtom("U", 92, 236,   236.045561900000,     0.00000000);
	addIsotopeToAtom("U", 92, 233,   233.039628000000,     0.00000000);
	addIsotopeToAtom("Np", 93, 237,   237.048167300000,    50.00000000);
	addIsotopeToAtom("Np", 93, 239,   239.052931400000,    50.00000000);
	addIsotopeToAtom("Pu", 94, 238,   238.049553400000,    16.66666667);
	addIsotopeToAtom("Pu", 94, 239,   239.052156500000,    16.66666667);
	addIsotopeToAtom("Pu", 94, 240,   240.053807500000,    16.66666667);
	addIsotopeToAtom("Pu", 94, 241,   241.056845300000,    16.66666667);
	addIsotopeToAtom("Pu", 94, 242,   242.058736800000,    16.66666667);
	addIsotopeToAtom("Pu", 94, 244,   244.064198000000,    16.66666667);
	addIsotopeToAtom("Am", 95, 241,   241.056822900000,    50.00000000);
	addIsotopeToAtom("Am", 95, 243,   243.061372700000,    50.00000000);
	addIsotopeToAtom("Cm", 96, 243,   243.061382200000,    16.66666667);
	addIsotopeToAtom("Cm", 96, 244,   244.062746300000,    16.66666667);
	addIsotopeToAtom("Cm", 96, 245,   245.065485600000,    16.66666667);
	addIsotopeToAtom("Cm", 96, 246,   246.067217600000,    16.66666667);
	addIsotopeToAtom("Cm", 96, 247,   247.070347000000,    16.66666667);
	addIsotopeToAtom("Cm", 96, 248,   248.072342000000,    16.66666667);
	addIsotopeToAtom("Bk", 97, 247,   247.070299000000,    50.00000000);
	addIsotopeToAtom("Bk", 97, 249,   249.074980000000,    50.00000000);
	addIsotopeToAtom("Cf", 98, 249,   249.074847000000,    25.00000000);
	addIsotopeToAtom("Cf", 98, 250,   250.076400000000,    25.00000000);
	addIsotopeToAtom("Cf", 98, 251,   251.079580000000,    25.00000000);
	addIsotopeToAtom("Cf", 98, 252,   252.081620000000,    25.00000000);
	addIsotopeToAtom("Es", 99, 252,   252.082970000000,   100.00000000);
	addIsotopeToAtom("Fm", 100, 257,   257.095099000000,   100.00000000);
	addIsotopeToAtom("Md", 101, 256,   256.094050000000,    50.00000000);
	addIsotopeToAtom("Md", 101, 258,   258.098425000000,    50.00000000);
	addIsotopeToAtom("No", 102, 259,   259.101020000000,   100.00000000);
	addIsotopeToAtom("Lr", 103, 262,   262.109690000000,   100.00000000);
	addIsotopeToAtom("Rf", 104, 261,   261.108750000000,   100.00000000);
	addIsotopeToAtom("Db", 105, 262,   262.114150000000,   100.00000000);
	addIsotopeToAtom("Sg", 106, 266,   266.121930000000,   100.00000000);
	addIsotopeToAtom("Bh", 107, 264,   264.124730000000,   100.00000000);
	addIsotopeToAtom("Hs", 108, 277,   277.000000000000,   100.00000000);
	addIsotopeToAtom("Mt", 109, 268,   268.138820000000,   100.00000000);
	addIsotopeToAtom("Uun", 110, 281,   281.000000000000,   100.00000000);
	addIsotopeToAtom("Uuu", 111, 272,   272.153480000000,   100.00000000);
	addIsotopeToAtom("Uub", 112, 285,   285.000000000000,   100.00000000);
	addIsotopeToAtom("Uuq", 114, 289,   289.000000000000,   100.00000000);
	addIsotopeToAtom("Uuh", 116, 292,   292.000000000000,   100.00000000);
}
/********************************************************************************/
char*** get_periodic_table()
{
	static const char *table0[PERIODIC_TABLE_N_COLUMNS][PERIODIC_TABLE_N_ROWS]={
		{"H" ,"Li","Na","K" ,"Rb","Cs","Fr","00","00","00"},
		{"00","Be","Mg","Ca","Sr","Ba","Ra","00","X","Tv"},
		{"00","00","00","Sc","Y" ,"La","Ac","00","00","00"},
		{"00","00","00","Ti","Zr","Hf","Rf","00","00","00"},
		{"00","00","00","V" ,"Nb","Ta","Db","Ce","Th","00"},
		{"00","00","00","Cr","Mo","W" ,"Sg","Pr","Pa","00"},
		{"00","00","00","Mn","Tc","Re","Bh","Nd","U" ,"00"},
		{"00","00","00","Fe","Ru","Os","Hs","Pm","Np","00"},
		{"00","00","00","Co","Rh","Ir","Mt","Sm","Pu","00"},
		{"00","00","00","Ni","Pd","Pt","00","Eu","Am","00"},
		{"00","00","00","Cu","Ag","Au","00","Gd","Cm","00"},
		{"00","00","00","Zn","Cd","Hg","00","Tb","Bk","00"},
		{"00","B" ,"Al","Ga","In","Tl","00","Dy","Cf","00"},
		{"00","C" ,"Si","Ge","Sn","Pb","00","Ho","Es","00"},
		{"00","N" ,"P" ,"As","Sb","Bi","00","Er","Fm","00"},
		{"00","O" ,"S" ,"Se","Te","Po","00","Tm","Md","00"},
		{"00","F" ,"Cl","Br","I" ,"At","00","Yb","No","00"},
		{"He","Ne","Ar","Kr","Xe","Rn","00","Lu","Lr","00"},
		};
	static char*** table = NULL;
	if(table == NULL)
	{
		int i;
		int j;
		table = malloc(PERIODIC_TABLE_N_COLUMNS*sizeof(char**));
		for(i=0;i<PERIODIC_TABLE_N_COLUMNS;i++)
		{
			table[i] = malloc(PERIODIC_TABLE_N_ROWS*sizeof(char*));
			for(j=0;j<PERIODIC_TABLE_N_ROWS;j++)
				table[i][j] = strdup(table0[i][j]);
		}
	}
	return table;
}
