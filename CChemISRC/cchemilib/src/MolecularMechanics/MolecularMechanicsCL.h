
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

const char* mmCLSource = 
"__kernel void initEnergy(__global float* energyBuffer, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i=j;\n"
//"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"	energyBuffer[i] = 0;	\n"
"	}\n"
"}\n"
"__kernel void addEnergyBondAmber(__global float* energyBuffer, __global float8* atoms, __global int4* bondIndex, __global float2* bondTerms, int nAtoms, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"	int ai,aj;\n"
"	float term;\n"
"	float4 rij;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"\n"
"	rij.s0 = atoms[ai].s0  - atoms[aj].s0;\n"
"	rij.s1 = atoms[ai].s1  - atoms[aj].s1;\n"
"	rij.s2 = atoms[ai].s2  - atoms[aj].s2;\n"
"\n"
"	term = sqrt( rij.s0*rij.s0 + rij.s1*rij.s1 + rij.s2*rij.s2) - bondTerms[i].s1;\n"
"	energyBuffer[i] += bondTerms[i].s0 * term * term;\n"
"	}\n"
"}\n"
"__kernel void addEnergyBendAmber(__global float* energyBuffer, __global float8* atoms, __global int8* bondIndex, __global float2* bondTerms, int nAtoms, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"	int ai,aj,ak;\n"
"	float x12, x32, y12, y32, z12, z32, l12, l32, dp;\n"
"	float D2RxD2R = 1/(57.29577951308232090712 * 57.29577951308232090712);\n"
"	float term;\n"
"	float thetaDeg;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"	ak = bondIndex[i].s2;\n"
"\n"
"        x12 = atoms[ai].s0 - atoms[aj].s0;\n"
"        y12 = atoms[ai].s1 - atoms[aj].s1;\n"
"        z12 = atoms[ai].s2 - atoms[aj].s2;\n"
"\n"
"        x32 = atoms[ak].s0 - atoms[aj].s0;\n"
"        y32 = atoms[ak].s1 - atoms[aj].s1;\n"
"        z32 = atoms[ak].s2 - atoms[aj].s2;\n"
"\n"
"       	l12 = sqrt( x12 * x12 + y12 * y12 + z12 * z12 );\n"
"       	l32 = sqrt( x32 * x32 + y32 * y32 + z32 * z32 );\n"
"        if( l12 != 0.0  &&  l32 != 0.0)\n"
"	{\n"
"		dp = ( x12 * x32 + y12 * y32 + z12 * z32 ) / (l12 * l32 );\n"
"		if ( dp < -1.0 ) dp = -1.0;\n"
"		else if ( dp > 1.0 ) dp = 1.0;\n"
"    		thetaDeg = 57.29577951308232090712 * acos(dp);\n"
"	}\n"
"\n"
"	term = thetaDeg - bondTerms[i].s1;\n"
"	term *= term *  bondTerms[i].s0;\n"
"	term *= D2RxD2R;\n"
"\n"
"	energyBuffer[i] += term;\n"
"	}\n"
"}\n"
"__kernel void addEnergyDihedralAmber(__global float* energyBuffer, __global float8* atoms, __global int8* bondIndex, __global float4* bondTerms, int nAtoms, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"	int ai, aj, ak, al;\n"
"	float phiDeg= 0;\n"
"	float	D2R = 1.0/57.29577951308232090712;\n"
"	float	PI = 3.14159265358979323846;\n"
"	float   xij, yij, zij;\n"
"       	float   xkj, ykj, zkj;\n"
"	float   xkl, ykl, zkl;\n"
"      	float   dx, dy, dz;\n"
"        float   gx, gy, gz;\n"
"        float   bi, bk;\n"
"        float   ct, d, ap, app, bibk;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"	ak = bondIndex[i].s2;\n"
"	al = bondIndex[i].s3;\n"
"\n"
"        xij = atoms[ai].s0 -  atoms[aj].s0;\n"
"        yij = atoms[ai].s1 -  atoms[aj].s1;\n"
"        zij = atoms[ai].s2 -  atoms[aj].s2;\n"
"\n"
"        xkj = atoms[ak].s0 -  atoms[aj].s0;\n"
"        ykj = atoms[ak].s1 -  atoms[aj].s1;\n"
"        zkj = atoms[ak].s2 -  atoms[aj].s2;\n"
"\n"
"        xkl = atoms[ak].s0 -  atoms[al].s0;\n"
"        ykl = atoms[ak].s1 -  atoms[al].s1;\n"
"        zkl = atoms[ak].s2 -  atoms[al].s2;\n"
"\n"
"        dx = yij * zkj - zij * ykj;\n"
"        dy = zij * xkj - xij * zkj;\n"
"        dz = xij * ykj - yij * xkj;\n"
"        gx = zkj * ykl - ykj * zkl;\n"
"        gy = xkj * zkl - zkj * xkl;\n"
"        gz = ykj * xkl - xkj * ykl;\n"
"\n"
"        bi = dx * dx + dy * dy + dz * dz;\n"
"        bk = gx * gx + gy * gy + gz * gz;\n"
"        ct = dx * gx + dy * gy + dz * gz;\n"
"	bibk = bi * bk;\n"
"		\n"
"	phiDeg = 0.0;\n"
"	if ( bibk >= 1.0e-6 )\n"
"	{\n"
"		ct = ct / sqrt( bibk );\n"
"        \n"
"		if( ct < -1.0 ) ct = -1.0;\n"
"        	else if( ct > 1.0 ) ct = 1.0;\n"
"        	ap = acos( ct );\n"
"        \n"
"		d  = xkj*(dz*gy-dy*gz) + ykj*(dx*gz-dz*gx) + zkj*(dy*gx-dx*gy);\n"
"        \n"
"		if( d < 0.0 ) ap = -ap;\n"
"        \n"
"		ap = PI - ap;\n"
"    		app = 180.0 * ap / PI;\n"
"       	if( app > 180.0 ) app = app - 360.0;\n"
"		phiDeg = app;\n"
"	}\n"
"\n"
"	energyBuffer[i] += bondTerms[i].s1/bondTerms[i].s0 * ( 1.0 + cos( D2R*(bondTerms[i].s3 * phiDeg - bondTerms[i].s2 )) );\n"
"	}\n"
"}\n"
"__kernel void addEnergyImproperTorsionAmber(__global float* energyBuffer, __global float8* atoms, __global int8* bondIndex, __global float4* bondTerms, int nAtoms, int numberOfStretchTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"}\n"
"__kernel void addEnergyNonBondedAmber(__global float* energyBuffer, __global float8* atoms, __global int4* bondIndex, __global float4* bondTerms, int nAtoms, int nTerms, int useCoulomb)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	int ai, aj;\n"
"	int i;\n"
"	float rij2, rij6, coulombTerm;\n"
"	float rijx, rijy, rijz;\n"
"	float rij;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"\n"
"	rijx =  atoms[ai].s0-atoms[aj].s0;\n"
"	rijy =  atoms[ai].s1-atoms[aj].s1;\n"
"	rijz =  atoms[ai].s2-atoms[aj].s2;\n"
"\n"
"	rij2 = rijx * rijx + rijy * rijy + rijz * rijz;\n"
"	rij = sqrt( rij2 );\n"
"	rij6 = 1.0/(rij2 * rij2 * rij2);\n"
"\n"
"	if(useCoulomb!=0) coulombTerm = ( bondTerms[i].s2 ) / rij;\n"
"	else coulombTerm = 0.0;\n"
"\n"
"	energyBuffer[i] += (bondTerms [i].s0 * rij6 - bondTerms [i].s1 ) * rij6 + coulombTerm;\n"
" }\n"
"}\n"
"__kernel void initRho(__global float* rho, int nAtoms, int nBlocks)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i=j;\n"
"	if(j>=nAtoms*nBlocks) return;\n"
"	for(i=j;i<nAtoms*nBlocks;i+=get_global_size(0))\n"
"	{\n"
"	    rho[i]=0;\n"
"	}\n"
"}\n"
"__kernel void reduceRho(__global float* rho, int nAtoms, int nBlocks)\n"
"{\n"
"	size_t k = get_global_id(0);\n"
"	size_t j;\n"
"	float sum;\n"
"	int i;\n"
"	for(j=k;j<nAtoms;j+=get_global_size(0))\n"
"	{\n"
"		sum = rho[j];\n"
"		for(i=j+nAtoms;i<nAtoms*nBlocks;i+=nAtoms)\n"
"		{\n"
"	    		sum += rho[i];\n"
"		}\n"
"		if(sum>0) sum = srqr(1/sum); else sum = 1e-8;\n"
"		rho[j] = sum;\n"
"	}\n"
"}\n"
"__kernel void computRho(__global float* rho, __global float8* atoms, __global int4* bondIndex, __global float8* bondTerms, int nAtoms, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"	int ai, aj;\n"
"	int oi, oj;\n"
"	float rijx, rijy, rijz, term;\n"
"	float rijm;\n"
"	float bondLength;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"	oi = ai+nAtoms*bondIndex[i].s2;\n"
"	oj = aj+nAtoms*bondIndex[i].s3;\n"
"\n"
"	rijx =  atoms[ai].s0-atoms[aj].s0;\n"
"	rijy =  atoms[ai].s1-atoms[aj].s1;\n"
"	rijz =  atoms[ai].s2-atoms[aj].s2;\n"
"\n"
"	rijm = pow(rij2,  bondTerms [i].s4/2);\n"
"\n"
"	rho[oi].s0 += rijm;\n"
"	rho[oj].s0 += rijm;\n"
"	}\n"
"		\n"
"}\n"
"__kernel void addEnergySuttonChenAmber(__global float* energyBuffer, __global float8* atoms, __global int4* bondIndex, __global float8* bondTerms, __global float* rho, int nAtoms, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	int ai, aj;\n"
"	int i;\n"
"	float rij2, rijm;\n"
"	float rijx, rijy, rijz;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"\n"
"	rijx =  atoms[ai].s0-atoms[aj].s0;\n"
"	rijy =  atoms[ai].s1-atoms[aj].s1;\n"
"	rijz =  atoms[ai].s2-atoms[aj].s2;\n"
"\n"
"	rij2 = rijx * rijx + rijy * rijy + rijz * rijz;\n"
"	rijm = pow(rij2,  bondTerms [i].s4/2);\n"
"\n"
"\n"
"	energyBuffer[i] += bondTerms [i].s0*(rijm - bondTerms [i].s2/ rho[i]);\n"
" }\n"
"}\n"
"__kernel void addEnergyHydrogenBondedAmber(__global float* energyBuffer, __global float8* atoms, __global int4* bondIndex, __global float2* bondTerms, int nAtoms, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"	int ai, aj;\n"
"	float rij2, rij6, rij12;\n"
"	float rijx, rijy, rijz;\n"
"	float rij4, rij10;\n"
"	float Cij, Dij;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"\n"
"	Cij    = bondTerms [i].s0;\n"
"	Dij    = bondTerms [i].s1;\n"
"\n"
"	rijx =  atoms[ai].s0-atoms[aj].s0;\n"
"	rijy =  atoms[ai].s1-atoms[aj].s1;\n"
"	rijz =  atoms[ai].s2-atoms[aj].s2;\n"
"\n"
"	rij2 = rijx * rijx + rijy * rijy + rijz * rijz;\n"
"	if ( rij2 < 1.0e-2 ) rij2 = 1.0e-2;	\n"
"	rij4 = rij2 * rij2;\n"
"	rij6 = rij4 * rij2;\n"
"	rij10 = rij6 * rij4;\n"
"	rij12 = rij10 * rij2;\n"
"\n"
"	energyBuffer[i] += Cij / rij12 - Dij / rij10;\n"
"	}\n"
"}\n"
"__kernel void addEnergyPairWise(__global float* energyBuffer, __global float8* atoms, __global int4* bondIndex, __global float8* bondTerms, int nAtoms, int nTerms, int useCoulomb, int useVanderWals)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"	int ai, aj;\n"
"	float rij2, rij6, rij8, rij10;\n"
"	float coulombTerm;\n"
"	float rijx, rijy, rijz;\n"
"	float rij;\n"
"	float permittivityScale = 1, permittivity = 1;\n"
"	float coulombFactor;\n"
"	float A, Beta;\n"
"	float  B6, B8, B10;\n"
"	float c6, c8, c10, b;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	coulombFactor = 332.05382/ ( permittivity * permittivityScale );\n"
"\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"\n"
"	A    = bondTerms [i].s0;\n"
"	Beta = bondTerms [i].s1;\n"
"	c6   = bondTerms [i].s2;\n"
"	c8   = bondTerms [i].s3;\n"
"	c10  = bondTerms [i].s4;\n"
"	b    = bondTerms [i].s5;\n"
"\n"
"	rijx =  atoms[ai].s0-atoms[aj].s0;\n"
"	rijy =  atoms[ai].s1-atoms[aj].s1;\n"
"	rijz =  atoms[ai].s2-atoms[aj].s2;\n"
"\n"
"	rij2 = rijx * rijx + rijy * rijy + rijz * rijz;\n"
"	if(rij2<1e-2) rij = 1e-2;\n"
"\n"
"	rij = sqrt( rij2 );\n"
"	rij6 = rij2 * rij2 * rij2;\n"
"	rij8 = rij6* rij2;\n"
"	rij10 = rij8 * rij2;\n"
"\n"
"	if(useCoulomb!=0) coulombTerm = ( atoms[ai].s3*atoms[aj].s3 * coulombFactor ) / rij;\n"
"	else coulombTerm = 0.0;\n"
"\n"
"	B6  = 0;\n"
"	B8  = 0;\n"
"	B10 = 0;\n"
"	if(useVanderWals!=0)\n"
"	{\n"
"		float fact = 1.0;\n"
"		float s = 1.0;\n"
"		float br = b*rij;\n"
"		float brk = 1.0;\n"
"		int k;\n"
"\n"
"		if(fabs(c6)>1e-12)\n"
"		{\n"
"			for(k=1;k<=2*3;k++)\n"
"			{\n"
"				fact *= k;\n"
"				brk *= br;\n"
"				s += brk/fact;\n"
"			}\n"
"			B6 = c6*(1-exp(-br)*s);\n"
"		}\n"
"\n"
"		if(fabs(c8)>1e-12)\n"
"		{\n"
"			fact = 1.0;\n"
"			s = 1.0;\n"
"			br = b*rij;\n"
"			for(k=1;k<=2*4;k++)\n"
"			{\n"
"				fact *= k;\n"
"				brk *= br;\n"
"				s += brk/fact;\n"
"			}\n"
"			B8 = c8*(1-exp(-br)*s);\n"
"		}\n"
"\n"
"		if(fabs(c10)>1e-12)\n"
"		{\n"
"			fact = 1.0;\n"
"			s = 1.0;\n"
"			br = b*rij;\n"
"			for(k=1;k<=2*5;k++)\n"
"			{\n"
"				fact *= k;\n"
"				brk *= br;\n"
"				s += brk/fact;\n"
"			}\n"
"			B10 = c10*(1-exp(-br)*s);\n"
"		}\n"
"	}\n"
"				\n"
"	energyBuffer[i] += A*exp(-Beta*rij)\n"
"		- B6 / rij6 \n"
"		- B8 / rij8 \n"
"		- B10 / rij10 \n"
"		+ coulombTerm;\n"
"	}\n"
"}\n"
"__kernel void initGradients(__global float4* gradientBuffer, int nAtoms, int nBlocks)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i=j;\n"
"	float4 tmp = (float4) (0,0,0,0);\n"
"	if(j>=nAtoms*nBlocks) return;\n"
"	for(i=j;i<nAtoms*nBlocks;i+=get_global_size(0))\n"
"	{\n"
"	    gradientBuffer[i]=tmp;\n"
"	}\n"
"}\n"
"__kernel void reduceGradients(__global float4* gradientBuffer, int nAtoms, int nBlocks)\n"
"{\n"
"	size_t k = get_global_id(0);\n"
"	size_t j;\n"
"	float4 sum;\n"
"	int i;\n"
"	for(j=k;j<nAtoms;j+=get_global_size(0))\n"
"	{\n"
"		sum = gradientBuffer[j];\n"
"		for(i=j+nAtoms;i<nAtoms*nBlocks;i+=nAtoms)\n"
"		{\n"
"	    		sum += gradientBuffer[i];\n"
"		}\n"
"		gradientBuffer[j] = sum;\n"
"	}\n"
"}\n"
"__kernel void initVelocities( __global float8* atoms, int nAtoms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	if(j>=nAtoms) return;\n"
"	atoms[j].s5 = 0;\n"
"	atoms[j].s6 = 0;\n"
"	atoms[j].s7 = 0;\n"
"}\n"
"__kernel void addGradientBondAmber(__global float* energyBuffer, __global float4* gradientBuffer, __global float8* atoms, __global int4* bondIndex, __global float2* bondTerms, int nAtoms, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"	int ai, aj;\n"
"	int oi, oj;\n"
"	float rijx, rijy, rijz, term;\n"
"	float forceix, forceiy, forceiz;\n"
"	float bondLength;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"	oi = ai+nAtoms*bondIndex[i].s2;\n"
"	oj = aj+nAtoms*bondIndex[i].s3;\n"
"\n"
"	rijx =  atoms[ai].s0-atoms[aj].s0;\n"
"	rijy =  atoms[ai].s1-atoms[aj].s1;\n"
"	rijz =  atoms[ai].s2-atoms[aj].s2;\n"
"\n"
"	bondLength = sqrt( rijx * rijx + rijy * rijy + rijz * rijz );\n"
"\n"
"	if ( bondLength < 1.0e-10 ) bondLength = 1.0e-10;\n"
"\n"
"	term = - 2*bondTerms [i].s0 * ( bondLength - bondTerms [i].s1 ) / bondLength;\n"
"	forceix = term * rijx;\n"
"	forceiy = term * rijy;\n"
"	forceiz = term * rijz;\n"
"\n"
"	gradientBuffer[oi].s0 -= forceix;\n"
"	gradientBuffer[oi].s1 -= forceiy;\n"
"	gradientBuffer[oi].s2 -= forceiz;\n"
"		\n"
"	gradientBuffer[oj].s0 += forceix;\n"
"	gradientBuffer[oj].s1 += forceiy;\n"
"	gradientBuffer[oj].s2 += forceiz;\n"
"\n"
"	term = ( bondLength - bondTerms [i].s1 );\n"
"	energyBuffer[i] += bondTerms[i].s0 * term * term;\n"
"	}\n"
"		\n"
"}\n"
"__kernel void addGradientBendAmber(__global float* energyBuffer, __global float4* gradientBuffer, __global float8* atoms, __global int8* bondIndex, __global float2* bondTerms, int nAtoms, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"	float	PI = 3.14159265358979323846;\n"
"	float	D2R = 1.0/57.29577951308232090712;\n"
"	float D2RxD2R = 1/(57.29577951308232090712 * 57.29577951308232090712);\n"
"	int ai, aj, ak;\n"
"	int oi, oj, ok;\n"
"	float x12, y12, z12, x32, y32, z32, l12, l32, dp;\n"
"\n"
"	float term;\n"
"	float thetaRad, cosTheta;\n"
"	float denominator, absTheta;\n"
"	float delta = 1e-10;\n"
"\n"
"	float rijx, rijy, rijz;\n"
"	float rkjx, rkjy, rkjz;\n"
"	float rij2, rij, rkj2, rkj,rij3, rkj3;\n"
"	float denominatori, denominatork;\n"
"\n"
"	float forceix, forceiy, forceiz;\n"
"	float forcejx, forcejy, forcejz;\n"
"	float forcekx, forceky, forcekz;\n"
"\n"
"	float rijDotrkj;\n"
"	float term2ix, term2iy, term2iz;\n"
"	float term2jx, term2jy, term2jz;\n"
"	float term2kx, term2ky, term2kz;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"	ak = bondIndex[i].s2;\n"
"	oi = ai+nAtoms*bondIndex[i].s3;\n"
"	oj = aj+nAtoms*bondIndex[i].s4;\n"
"	ok = ak+nAtoms*bondIndex[i].s5;\n"
"\n"
"        x12 = atoms[ai].s0 - atoms[aj].s0;\n"
"        y12 = atoms[ai].s1 - atoms[aj].s1;\n"
"        z12 = atoms[ai].s2 - atoms[aj].s2;\n"
"\n"
"        x32 = atoms[ak].s0 - atoms[aj].s0;\n"
"        y32 = atoms[ak].s1 - atoms[aj].s1;\n"
"        z32 = atoms[ak].s2 - atoms[aj].s2;\n"
"\n"
"       l12 = sqrt( x12 * x12 + y12 * y12 + z12 * z12 );\n"
"       l32 = sqrt( x32 * x32 + y32 * y32 + z32 * z32 );\n"
"        if( l12 != 0.0  &&  l32 != 0.0)\n"
"	{\n"
"		dp = ( x12 * x32 + y12 * y32 + z12 * z32 ) / (l12 * l32 );\n"
"		if ( dp < -1.0 ) dp = -1.0;\n"
"		else if ( dp > 1.0 ) dp = 1.0;\n"
"    		thetaRad = acos(dp);\n"
"	}\n"
"\n"
"	absTheta = fabs( thetaRad );\n"
"	cosTheta = cos( thetaRad );\n"
"\n"
"	if ( ( absTheta > delta ) && ( absTheta < PI - delta ) )\n"
"	{\n"
"		denominator = sin(thetaRad);\n"
"			\n"
"		if ( denominator < 1.0e-10 ) denominator = 1.0e-10;\n"
"\n"
"		term = 2*bondTerms[i].s0 * (thetaRad/PI*180.0 - bondTerms[i].s1) / denominator;\n"
"		term *= D2R;\n"
"\n"
"        	rijx = atoms[ai].s0 - atoms[aj].s0;\n"
"        	rijy = atoms[ai].s1 - atoms[aj].s1;\n"
"        	rijz = atoms[ai].s2 - atoms[aj].s2;\n"
"\n"
"        	rkjx = atoms[ak].s0 - atoms[aj].s0;\n"
"        	rkjy = atoms[ak].s1 - atoms[aj].s1;\n"
"        	rkjz = atoms[ak].s2 - atoms[aj].s2;\n"
"\n"
"		rij2 = rijx * rijx + rijy * rijy + rijz * rijz;\n"
"		rij = sqrt( rij2 );\n"
"\n"
"		rkj2 = rkjx * rkjx + rkjy * rkjy + rkjz * rkjz;\n"
"		rkj = sqrt( rkj2 );\n"
"\n"
"		rijDotrkj = rijx * rkjx + rijy * rkjy + rijz * rkjz;\n"
"\n"
"		rij3 = rij2 * rij;\n"
"		rkj3 = rkj2 * rkj;\n"
"\n"
"		denominatori = rij3 * rkj;\n"
"		if ( denominatori < 1.0e-10 ) denominatori = 1.0e-10;\n"
"\n"
"		denominatork = rij * rkj3;\n"
"		if ( denominatork < 1.0e-10 ) denominatork = 1.0e-10;\n"
"			\n"
"		term2ix = ( rij2 * rkjx - rijDotrkj * rijx ) / denominatori;\n"
"		term2iy = ( rij2 * rkjy - rijDotrkj * rijy ) / denominatori;\n"
"		term2iz = ( rij2 * rkjz - rijDotrkj * rijz ) / denominatori;\n"
"			\n"
"		term2kx = ( rkj2 * rijx - rijDotrkj * rkjx ) / denominatork;\n"
"		term2ky = ( rkj2 * rijy - rijDotrkj * rkjy ) / denominatork;\n"
"		term2kz = ( rkj2 * rijz - rijDotrkj * rkjz ) / denominatork;\n"
"			\n"
"		term2jx = - term2ix - term2kx;\n"
"		term2jy = - term2iy - term2ky;\n"
"		term2jz = - term2iz - term2kz;\n"
"			\n"
"		forceix = term * term2ix;\n"
"		forceiy = term * term2iy;\n"
"		forceiz = term * term2iz;\n"
"			\n"
"		forcejx = term * term2jx;\n"
"		forcejy = term * term2jy;\n"
"		forcejz = term * term2jz;\n"
"			\n"
"		forcekx = term * term2kx;\n"
"		forceky = term * term2ky;\n"
"		forcekz = term * term2kz;\n"
"\n"
"		gradientBuffer[oi].s0 -= forceix;\n"
"		gradientBuffer[oi].s1 -= forceiy;\n"
"		gradientBuffer[oi].s2 -= forceiz;\n"
"\n"
"		gradientBuffer[oj].s0 -= forcejx;\n"
"		gradientBuffer[oj].s1 -= forcejy;\n"
"		gradientBuffer[oj].s2 -= forcejz;\n"
"\n"
"		gradientBuffer[ok].s0 -= forcekx;\n"
"		gradientBuffer[ok].s1 -= forceky;\n"
"		gradientBuffer[ok].s2 -= forcekz;\n"
"		term = (thetaRad/PI*180.0 - bondTerms[i].s1);\n"
"		term *= term *  bondTerms[i].s0;\n"
"		term *= D2RxD2R;\n"
"\n"
"		energyBuffer[i] += term;\n"
"\n"
"	}\n"
"	}\n"
"}\n"
"__kernel void addGradientDihedralAmber(__global float* energyBuffer, __global float4* gradientBuffer, __global float8* atoms, __global int8* bondIndex, __global float4* bondTerms, int nAtoms, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"	int ai,aj,ak,al;\n"
"	int oi,oj,ok,ol;\n"
"	float	D2R = 1.0/57.29577951308232090712;\n"
"	float	PI = 3.14159265358979323846;\n"
"\n"
"	float rjix, rjiy, rjiz;\n"
"	float rkjx, rkjy, rkjz;\n"
"	float rkix, rkiy, rkiz;\n"
"	float rljx, rljy, rljz;\n"
"	float rlkx, rlky, rlkz;\n"
"\n"
"	float forceix, forceiy, forceiz;\n"
"	float forcejx, forcejy, forcejz;\n"
"	float forcekx, forceky, forcekz;\n"
"	float forcelx, forcely, forcelz;\n"
"\n"
"	float rkj;\n"
"	float xt, yt, zt;\n"
"	float xu, yu, zu;\n"
"	float xtu, ytu, ztu;\n"
"	float rt2, ru2, rtru;\n"
"	float cosine1, sine1, cosineN, sineN, cosold, sinold;\n"
"	float cosPhase, sinPhase;\n"
"	float dedxt, dedyt, dedzt;\n"
"	float dedxu, dedyu, dedzu;\n"
"	float dedphi;\n"
"	int n;\n"
"	float vn;\n"
"	float phi;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"	ak = bondIndex[i].s2;\n"
"	al = bondIndex[i].s3;\n"
"	oi = ai+nAtoms*bondIndex[i].s4;\n"
"	oj = aj+nAtoms*bondIndex[i].s5;\n"
"	ok = ak+nAtoms*bondIndex[i].s6;\n"
"	ol = al+nAtoms*bondIndex[i].s7;\n"
"\n"
"        rjix = atoms[aj].s0 -  atoms[ai].s0;\n"
"        rjiy = atoms[aj].s1 -  atoms[ai].s1;\n"
"        rjiz = atoms[aj].s2 -  atoms[ai].s2;\n"
"\n"
"        rkjx = atoms[ak].s0 -  atoms[aj].s0;\n"
"        rkjy = atoms[ak].s1 -  atoms[aj].s1;\n"
"        rkjz = atoms[ak].s2 -  atoms[aj].s2;\n"
"\n"
"        rlkx = atoms[al].s0 -  atoms[ak].s0;\n"
"        rlky = atoms[al].s1 -  atoms[ak].s1;\n"
"        rlkz = atoms[al].s2 -  atoms[ak].s2;\n"
"\n"
"	xt = rjiy*rkjz - rkjy*rjiz;\n"
"	yt = rjiz*rkjx - rkjz*rjix;\n"
"	zt = rjix*rkjy - rkjx*rjiy;\n"
"\n"
"	xu = rkjy*rlkz - rlky*rkjz;\n"
"	yu = rkjz*rlkx - rlkz*rkjx;\n"
"	zu = rkjx*rlky - rlkx*rkjy;\n"
"\n"
"	xtu = yt*zu - yu*zt;\n"
"	ytu = zt*xu - zu*xt;\n"
"	ztu = xt*yu - xu*yt;\n"
"\n"
"	rt2 = xt*xt + yt*yt + zt*zt;\n"
"	ru2 = xu*xu + yu*yu + zu*zu;\n"
"\n"
"	rtru = sqrt(rt2 * ru2);\n"
"\n"
"	rkj = sqrt(rkjx*rkjx + rkjy*rkjy + rkjz*rkjz);\n"
"	cosine1 = 1.0;\n"
"	sine1   = 0.0;\n"
"\n"
"	if (rtru <1e-10) rtru = 1e-10;\n"
"	if (rt2 <1e-10) rt2 = 1e-10;\n"
"	if (ru2 <1e-10) ru2 = 1e-10;\n"
"\n"
"	cosine1 = (xt*xu + yt*yu + zt*zu) / rtru;\n"
"	sine1 = (rkjx*xtu + rkjy*ytu + rkjz*ztu) / (rkj*rtru);\n"
"\n"
"	n = (int)bondTerms[i].s3;\n"
"	cosPhase = cos(D2R*bondTerms[i].s2);\n"
"	sinPhase = sin(D2R*bondTerms[i].s2);\n"
"	vn = bondTerms[i].s1/bondTerms[i].s0;\n"
"\n"
"	cosineN = cosine1;\n"
"	sineN   = sine1;\n"
"\n"
"	for(j=2;j<=n;j++)\n"
"	{\n"
"	   cosold = cosineN;\n"
"	   sinold = sineN;\n"
"	   cosineN = cosine1*cosold - sine1*sinold;\n"
"	   sineN   = cosine1*sinold + sine1*cosold;\n"
"	}\n"
"\n"
"	dedphi = vn*n*(cosineN*sinPhase-sineN*cosPhase);\n"
"\n"
"        rkix = atoms[ak].s0 -  atoms[ai].s0;\n"
"        rkiy = atoms[ak].s1 -  atoms[ai].s1;\n"
"        rkiz = atoms[ak].s2 -  atoms[ai].s2;\n"
"\n"
"        rljx = atoms[al].s0 -  atoms[aj].s0;\n"
"        rljy = atoms[al].s1 -  atoms[aj].s1;\n"
"        rljz = atoms[al].s2 -  atoms[aj].s2;\n"
"\n"
"	dedxt = dedphi * (yt*rkjz - rkjy*zt) / (rt2*rkj);\n"
"	dedyt = dedphi * (zt*rkjx - rkjz*xt) / (rt2*rkj);\n"
"	dedzt = dedphi * (xt*rkjy - rkjx*yt) / (rt2*rkj);\n"
"\n"
"	dedxu = -dedphi * (yu*rkjz - rkjy*zu) / (ru2*rkj);\n"
"	dedyu = -dedphi * (zu*rkjx - rkjz*xu) / (ru2*rkj);\n"
"	dedzu = -dedphi * (xu*rkjy - rkjx*yu) / (ru2*rkj);\n"
"\n"
"	forceix = rkjz*dedyt - rkjy*dedzt;\n"
"	forceiy = rkjx*dedzt - rkjz*dedxt;\n"
"	forceiz = rkjy*dedxt - rkjx*dedyt;\n"
"\n"
"	forcejx = rkiy*dedzt - rkiz*dedyt + rlkz*dedyu - rlky*dedzu;\n"
"	forcejy = rkiz*dedxt - rkix*dedzt + rlkx*dedzu - rlkz*dedxu;\n"
"	forcejz = rkix*dedyt - rkiy*dedxt + rlky*dedxu - rlkx*dedyu;\n"
"\n"
"	forcekx = rjiz*dedyt - rjiy*dedzt + rljy*dedzu - rljz*dedyu;\n"
"	forceky = rjix*dedzt - rjiz*dedxt + rljz*dedxu - rljx*dedzu;\n"
"	forcekz = rjiy*dedxt - rjix*dedyt + rljx*dedyu - rljy*dedxu;\n"
"\n"
"	forcelx = rkjz*dedyu - rkjy*dedzu;\n"
"	forcely = rkjx*dedzu - rkjz*dedxu;\n"
"	forcelz = rkjy*dedxu - rkjx*dedyu;\n"
"\n"
"	gradientBuffer[oi].s0 += forceix;\n"
"	gradientBuffer[oi].s1 += forceiy;\n"
"	gradientBuffer[oi].s2 += forceiz;\n"
"\n"
"	gradientBuffer[oj].s0 += forcejx;\n"
"	gradientBuffer[oj].s1 += forcejy;\n"
"	gradientBuffer[oj].s2 += forcejz;\n"
"\n"
"	gradientBuffer[ok].s0 += forcekx;\n"
"	gradientBuffer[ok].s1 += forceky;\n"
"	gradientBuffer[ok].s2 += forcekz;\n"
"\n"
"	gradientBuffer[ol].s0 += forcelx;\n"
"	gradientBuffer[ol].s1 += forcely;\n"
"	gradientBuffer[ol].s2 += forcelz;\n"
"\n"
"	if(cosine1<-1) phi = 180.0;\n"
"	else if(cosine1>1) phi=0;\n"
"	else phi=acos(cosine1)/PI*180;\n"
"	energyBuffer[i] += bondTerms[i].s1/bondTerms[i].s0 * ( 1.0 + cos( D2R*(bondTerms[i].s3 * phi - bondTerms[i].s2 )) );\n"
"	}\n"
"\n"
"}\n"
"__kernel void addGradientImproperTorsion(__global float* energyBuffer, __global float4* gradientBuffer, __global float8* atoms, __global int8* bondIndex, __global float4* bondTerms, int nAtoms, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"}\n"
"__kernel void addGradientNonBondedAmber(__global float* energyBuffer, __global float4* gradientBuffer, __global float8* atoms, __global int4* bondIndex, __global float4* bondTerms, int nAtoms, int nTerms, int useCoulomb)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"\n"
"	float4 rij4=(float4)(0,0,0,0);\n"
"\n"
"	float4 forcei=(float4)(0,0,0,0);\n"
"\n"
"	float rij2, rij;\n"
"	float rij3;\n"
"	float coulombTerm;\n"
"	float rij6, rij8;\n"
"	float  term;\n"
"	int ai,aj,oi,oj;\n"
"\n"
"	if(j>=nTerms) return;\n"
"\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai =  bondIndex[i].s0;\n"
"	aj =  bondIndex[i].s1;\n"
"	oi =bondIndex[i].s0+nAtoms*bondIndex[i].s2;\n"
"	oj =bondIndex[i].s1+nAtoms*bondIndex[i].s3;\n"
"	rij4.x =  atoms[ai].s0-atoms[aj].s0;\n"
"	rij4.y =  atoms[ai].s1-atoms[aj].s1;\n"
"	rij4.z =  atoms[ai].s2-atoms[aj].s2;\n"
"\n"
"	rij2 = rij4.x*rij4.x;\n"
"	rij2 += rij4.y*rij4.y;\n"
"	rij2 += rij4.z*rij4.z;\n"
"	if ( rij2 < 1.0e-2 ) rij2 = 1.0e-2;	\n"
"\n"
"	rij2 = 1/rij2;\n"
"	rij = sqrt( rij2 );\n"
"	rij3 = rij*rij2;\n"
"	rij6 = rij3 * rij3;\n"
"	rij8 = rij6 * rij2;\n"
"\n"
"	if(useCoulomb!=0) coulombTerm = ( bondTerms[i].s2 ) * rij3;\n"
"	else coulombTerm = 0.0;\n"
"\n"
"	term = 6*(2 * bondTerms [i].s0 * rij6-bondTerms [i].s1) * rij8+ coulombTerm;\n"
"	forcei.xyz = term * rij4.xyz;\n"
"\n"
"	rij4 = gradientBuffer[oi]-forcei;\n"
"	gradientBuffer[oi] = rij4;\n"
"	rij4 = gradientBuffer[oj]+forcei;\n"
"	gradientBuffer[oj] = rij4;\n"
"\n"
"	term = energyBuffer[i];\n"
"	term += (bondTerms [i].s0 * rij6 - bondTerms [i].s1 ) * rij6 + coulombTerm/rij2;\n"
"	energyBuffer[i] = term;\n"
"	}\n"
"\n"
"}\n"
"__kernel void addGradientSuttonChenAmber(__global float* energyBuffer, __global float4* gradientBuffer, __global float8* atoms, __global int4* bondIndex, __global float8* bondTerms, __global float* rho, int nAtoms, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"\n"
"	float4 rij4=(float4)(0,0,0,0);\n"
"\n"
"	float4 forcei=(float4)(0,0,0,0);\n"
"\n"
"	float rij2, rijn, rijm;\n"
"	float  term;\n"
"	int ai,aj,oi,oj;\n"
"\n"
"	if(j>=nTerms) return;\n"
"\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai =  bondIndex[i].s0;\n"
"	aj =  bondIndex[i].s1;\n"
"	oi =bondIndex[i].s0+nAtoms*bondIndex[i].s2;\n"
"	oj =bondIndex[i].s1+nAtoms*bondIndex[i].s3;\n"
"	rij4.x =  atoms[ai].s0-atoms[aj].s0;\n"
"	rij4.y =  atoms[ai].s1-atoms[aj].s1;\n"
"	rij4.z =  atoms[ai].s2-atoms[aj].s2;\n"
"\n"
"	rij2 = rij4.x*rij4.x;\n"
"	rij2 += rij4.y*rij4.y;\n"
"	rij2 += rij4.z*rij4.z;\n"
"	if ( rij2 < 1.0e-2 ) rij2 = 1.0e-2;	\n"
"\n"
"	rijn =  pow( rij2, suttonChenTerms[5][i]/2);\n"
"	rijm =  pow( rij2, suttonChenTerms[6][i]/2);\n"
"\n"
"	term = bondTerms[i].s0*(bondTerms [i].s3 * rijn-bondTerms [i].s2* bondTerms [i].s4/2*rijm*(rho[ai]+rho[aj]))/rij2;\n"
"	forcei.xyz = term * rij4.xyz;\n"
"\n"
"	rij4 = gradientBuffer[oi]-forcei;\n"
"	gradientBuffer[oi] = rij4;\n"
"	rij4 = gradientBuffer[oj]+forcei;\n"
"	gradientBuffer[oj] = rij4;\n"
"\n"
"	term = energyBuffer[i];\n"
"	term += bondTerms [i].s0 * (rijn - bondTerms [i].s2/rho[ai]);\n"
"	energyBuffer[i] = term;\n"
"	}\n"
"\n"
"}\n"
"__kernel void addGradientHydrogenBondedAmber(__global float* energyBuffer, __global float4* gradientBuffer, __global float8* atoms, __global int4* bondIndex, __global float2* bondTerms, int nAtoms, int nTerms)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"\n"
"	int ai, aj;\n"
"	int oi, oj;\n"
"\n"
"	float rijx, rijy, rijz;\n"
"\n"
"	float forceix, forceiy, forceiz;\n"
"	float forcejx, forcejy, forcejz;\n"
"\n"
"	float Cij, Dij, rij2,  rij4, rij8, rij12, rij14;\n"
"	float  term1, term2, term3;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"	oi = ai+nAtoms*bondIndex[i].s2;\n"
"	oj = aj+nAtoms*bondIndex[i].s3;\n"
"\n"
"	Cij    = bondTerms [i].s0;\n"
"	Dij    = bondTerms [i].s1;\n"
"\n"
"	rijx =  atoms[ai].s0-atoms[aj].s0;\n"
"	rijy =  atoms[ai].s1-atoms[aj].s1;\n"
"	rijz =  atoms[ai].s2-atoms[aj].s2;\n"
"\n"
"	rij2 = rijx * rijx + rijy * rijy + rijz * rijz;\n"
"\n"
"	if ( rij2 < 1.0e-2 ) rij2 = 1.0e-2;	\n"
"\n"
"	rij4 = rij2 * rij2;\n"
"	rij8 = rij4 * rij4;\n"
"	rij12 = rij8 * rij4;\n"
"	rij14 = rij12 * rij2;\n"
"	term1 = Cij / rij14;\n"
"	term2 = Dij / rij12;\n"
"	term3 = term1 - term2;\n"
"	forceix = term3 * rijx;\n"
"	forceiy = term3 * rijy;\n"
"	forceiz = term3 * rijz;\n"
"	forcejx = - forceix;\n"
"	forcejy = - forceiy;\n"
"	forcejz = - forceiz;\n"
"\n"
"	gradientBuffer[oi].s0 -= forceix;\n"
"	gradientBuffer[oi].s1 -= forceiy;\n"
"	gradientBuffer[oi].s2 -= forceiz;\n"
"\n"
"	gradientBuffer[oj].s0 -= forcejx;\n"
"	gradientBuffer[oj].s1 -= forcejy;\n"
"	gradientBuffer[oj].s2 -= forcejz;\n"
"\n"
"	energyBuffer[i] += Cij / rij12 - Dij / rij12*rij2;\n"
"	}\n"
"\n"
"}\n"
"__kernel void addGradientPairWise(__global float* energyBuffer, __global float4* gradientBuffer, __global float8* atoms, __global int4* bondIndex, __global float8* bondTerms, int nAtoms, int nTerms, int useCoulomb, int useVanderWals)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i;\n"
"	int ai, aj;\n"
"	int oi, oj;\n"
"\n"
"	float  B6, B8, B10;\n"
"	float rijx, rijy, rijz;\n"
"\n"
"	float forceix, forceiy, forceiz;\n"
"	float forcejx, forcejy, forcejz;\n"
"\n"
"	float permittivityScale = 1, permittivity = 1;\n"
"	float coulombFactor;\n"
"	float rij2, rij;\n"
"	float rij3;\n"
"	float coulombTerm;\n"
"	float rij6, rij7, rij8, rij9, rij10, rij11, rij12;\n"
"	float  term1, term6, term8, term10, termAll;\n"
"	float A, Beta, C6, C8, C10,b;\n"
"	float s, sp, fact, br, brk, ebr;\n"
"	int n, k;\n"
"\n"
"	if(j>=nTerms) return;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0)) {\n"
"\n"
"	coulombFactor = 332.05382 / ( permittivity * permittivityScale );\n"
"	ai = bondIndex[i].s0;\n"
"	aj = bondIndex[i].s1;\n"
"	oi = ai+nAtoms*bondIndex[i].s2;\n"
"	oj = aj+nAtoms*bondIndex[i].s3;\n"
"\n"
"	A    = bondTerms [i].s0;\n"
"	Beta = bondTerms [i].s1;\n"
"	C6   = bondTerms [i].s2;\n"
"	C8   = bondTerms [i].s3;\n"
"	C10  = bondTerms [i].s4;\n"
"	b    = bondTerms [i].s5;\n"
"\n"
"	rijx =  atoms[ai].s0-atoms[aj].s0;\n"
"	rijy =  atoms[ai].s1-atoms[aj].s1;\n"
"	rijz =  atoms[ai].s2-atoms[aj].s2;\n"
"\n"
"	rij2 = rijx * rijx + rijy * rijy + rijz * rijz;\n"
"	if ( rij2 < 1.0e-2 ) rij2 = 1.0e-2;	\n"
"\n"
"	rij = sqrt( rij2 );\n"
"	rij3 = rij2 * rij;\n"
"	rij6 = rij3 * rij3;\n"
"	rij7 = rij6 * rij;\n"
"	rij8 = rij7 * rij;\n"
"	rij9 = rij8 * rij;\n"
"	rij10 = rij9 * rij;\n"
"	rij11 = rij10 * rij;\n"
"	rij12 = rij11 * rij;\n"
"	if(useCoulomb!=0) coulombTerm = ( atoms[ai].s3*atoms[ai].s3 * coulombFactor ) / rij3;\n"
"	else coulombTerm = 0.0;\n"
"		\n"
"		\n"
"	term1 = -A*Beta/rij*exp(-Beta*rij);\n"
"\n"
"	br = b*rij;\n"
"	ebr = exp(-b*rij);\n"
"\n"
"	term6 =   0.0;\n"
"	if(useVanderWals!=0 && fabs(C6)>1e-12)\n"
"	{\n"
"		fact = 1.0;\n"
"		s = 1.0;\n"
"		n = 3;\n"
"		brk = 1.0;\n"
"		for(k=1;k<2*n;k++)\n"
"		{\n"
"			fact *= k;\n"
"			brk *= br;\n"
"			s += brk/fact;\n"
"		}\n"
"		sp = s*b;\n"
"		fact *=2*n;\n"
"		brk *= br;\n"
"		s += brk/fact;\n"
"		term6 =   b*C6*ebr*s/rij7\n"
"			-(2*n)*C6*(1-ebr*s)/rij8\n"
"			-C6*ebr/rij7*sp;\n"
"	}\n"
"	term8 =   0.0;\n"
"	if(useVanderWals!=1 && fabs(C8)>1e-12)\n"
"	{\n"
"		fact = 1.0;\n"
"		s = 1.0;\n"
"		n = 4;\n"
"		brk = 1.0;\n"
"		for(k=1;k<2*n;k++)\n"
"		{\n"
"			fact *= k;\n"
"			brk *= br;\n"
"			s += brk/fact;\n"
"		}\n"
"		sp = s*b;\n"
"		fact *=2*n;\n"
"		brk *= br;\n"
"		s += brk/fact;\n"
"		term8 =   b*C8*ebr*s/rij9\n"
"			-(2*n)*C8*(1-ebr*s)/rij10\n"
"			-C8*ebr/rij9*sp;\n"
"	}\n"
"\n"
"	term10 =   0.0;\n"
"	if(useVanderWals!=0 && fabs(C10)>1e-12)\n"
"	{\n"
"		fact = 1.0;\n"
"		s = 1.0;\n"
"		n = 5;\n"
"		brk = 1.0;\n"
"		for(k=1;k<2*n;k++)\n"
"		{\n"
"			fact *= k;\n"
"			brk *= br;\n"
"			s += brk/fact;\n"
"		}\n"
"		sp = s*b;\n"
"\n"
"		fact *=2*n;\n"
"		brk *= br;\n"
"		s += brk/fact;\n"
"		term10 =   b*C10*ebr*s/rij11\n"
"			-(2*n)*C10*(1-ebr*s)/rij12\n"
"			-C10*ebr/rij11*sp;\n"
"	}\n"
"\n"
"	termAll = term1 - term6 - term8 - term10 + coulombTerm;\n"
"\n"
"\n"
"	forceix = termAll * rijx;\n"
"	forceiy = termAll * rijy;\n"
"	forceiz = termAll * rijz;\n"
"	forcejx = - forceix;\n"
"	forcejy = - forceiy;\n"
"	forcejz = - forceiz;\n"
"\n"
"	gradientBuffer[oi].s0 -= forceix;\n"
"	gradientBuffer[oi].s1 -= forceiy;\n"
"	gradientBuffer[oi].s2 -= forceiz;\n"
"\n"
"	gradientBuffer[oj].s0 -= forcejx;\n"
"	gradientBuffer[oj].s1 -= forcejy;\n"
"	gradientBuffer[oj].s2 -= forcejz;\n"
"	B6  = 0;\n"
"	B8  = 0;\n"
"	B10 = 0;\n"
"	if(useVanderWals!=0)\n"
"	{\n"
"		float fact = 1.0;\n"
"		float s = 1.0;\n"
"		float br = b*rij;\n"
"		float brk = 1.0;\n"
"		int k;\n"
"\n"
"		if(fabs(C6)>1e-12)\n"
"		{\n"
"			for(k=1;k<=2*3;k++)\n"
"			{\n"
"				fact *= k;\n"
"				brk *= br;\n"
"				s += brk/fact;\n"
"			}\n"
"			B6 = C6*(1-exp(-br)*s);\n"
"		}\n"
"\n"
"		if(fabs(C8)>1e-12)\n"
"		{\n"
"			fact = 1.0;\n"
"			s = 1.0;\n"
"			br = b*rij;\n"
"			for(k=1;k<=2*4;k++)\n"
"			{\n"
"				fact *= k;\n"
"				brk *= br;\n"
"				s += brk/fact;\n"
"			}\n"
"			B8 = C8*(1-exp(-br)*s);\n"
"		}\n"
"\n"
"		if(fabs(C10)>1e-12)\n"
"		{\n"
"			fact = 1.0;\n"
"			s = 1.0;\n"
"			br = b*rij;\n"
"			for(k=1;k<=2*5;k++)\n"
"			{\n"
"				fact *= k;\n"
"				brk *= br;\n"
"				s += brk/fact;\n"
"			}\n"
"			B10 = C10*(1-exp(-br)*s);\n"
"		}\n"
"	}\n"
"				\n"
"	energyBuffer[i] += A*exp(-Beta*rij)\n"
"		- B6 / rij6 \n"
"		- B8 / rij8 \n"
"		- B10 / rij10 \n"
"		+ coulombTerm;\n"
"	}\n"
"\n"
"}\n"
;
