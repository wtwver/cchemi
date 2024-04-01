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

#ifndef __CCHEMI_TREEMOLECULE_H__
#define __CCHEMI_TREEMOLECULE_H__

#include "../Molecule/Molecule.h"

typedef struct _TreeMolecule
{
	int nAtoms; 
	int **connected;
	boolean *inStack;
	boolean done;
	int bonds;
	int ringSize;
}TreeMolecule;

void initTreeMolecule(TreeMolecule* treeMolecule, Molecule*  mol, int ringSize);
boolean inRingTreeMolecule(TreeMolecule* treeMolecule, int currentAtom, int rootAtom);
int* getRingTreeMolecule(TreeMolecule* treeMolecule);
void getCentreRingTreeMolecule(TreeMolecule* treeMolecule, Molecule* mol, int i, int j, double C[]);
int* getListGroupe(int* nGroupAtoms, Molecule*  mol, int i1, int i2, int i3, int i4);
#endif /* __CCHEMI_TREEMOLECULE_H__*/

