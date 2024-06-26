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

const char* mdCLSource = 
"__kernel void computeKineticEnergy(__global float* energyBuffer, __global float8* atoms, int nAtoms)\n"
"{\n"
"	float ekin=0;\n"
"	int i;\n"
"	if( get_global_id(0)>0) return;\n"
"\n"
"	for(i=0;i<nAtoms;i++)\n"
"	{\n"
"		ekin+= atoms[i].s5*atoms[i].s5*atoms[i].s4;\n"
"		ekin+= atoms[i].s6*atoms[i].s6*atoms[i].s4;\n"
"		ekin+= atoms[i].s7*atoms[i].s7*atoms[i].s4;\n"
"	}\n"
"	energyBuffer[0] = ekin/2;\n"
"}\n"
"__kernel void scaleVelocities(__global float8* atoms, int nAtoms, float fact)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	if(i>=nAtoms) return;\n"
"	atoms[i].s5 *= fact;\n"
"	atoms[i].s6 *= fact;\n"
"	atoms[i].s7 *= fact;\n"
"}\n"
"__kernel void copyAccelarations(__global float4* a, int nAtoms)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	int oaold = nAtoms+i;\n"
"	int oa = i; \n"
"	if(i>=nAtoms) return;\n"
"	a[oaold].xyz =  a[oa].xyz;\n"
"}\n"
"__kernel void initAccelarations(__global float4* a, int nAtoms)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	int oa = i; \n"
"	int oaold = nAtoms+i; \n"
"	if(i>=nAtoms) return;\n"
"	a[oa].s0 =  0;\n"
"	a[oa].s1 =  0;\n"
"	a[oa].s2 =  0;\n"
"	a[oaold].s0 =  0;\n"
"	a[oaold].s1 =  0;\n"
"	a[oaold].s2 =  0;\n"
"}\n"
"__kernel void computeAccelarationsFromGradients(__global float4* buffer, __global float4* a,  __global float8* atoms, int nAtoms)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	int oa = i; \n"
"	if(i>=nAtoms) return;\n"
"	a[oa].s0 =  -buffer[oa].s0/atoms[i].s4;\n"
"	a[oa].s1 =  -buffer[oa].s1/atoms[i].s4;\n"
"	a[oa].s2 =  -buffer[oa].s2/atoms[i].s4;\n"
"}\n"
"__kernel void copyPositions(__global float4* oldPositionBuffer, __global float8* atoms, int nAtoms)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	if(i>=nAtoms) return;\n"
"	oldPositionBuffer[i].s0 = atoms[i].s0;\n"
"	oldPositionBuffer[i].s1 = atoms[i].s1;\n"
"	oldPositionBuffer[i].s2 = atoms[i].s2;\n"
"}\n"
"__kernel void applyVerlet1(__global float4* a, __global float8* atoms, float dt, int nAtoms)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	float dt2_2 = dt*dt/2;\n"
"	float dt_2 = dt/2;\n"
"	int oaold = nAtoms+i;\n"
"	int oa = i; \n"
"\n"
"	if(i>=nAtoms) return;\n"
"\n"
"	atoms[i].s0 += atoms[i].s5 * dt + a[oa].s0*dt2_2;	\n"
"	atoms[i].s1 += atoms[i].s6 * dt + a[oa].s1*dt2_2;	\n"
"	atoms[i].s2 += atoms[i].s7 * dt + a[oa].s2*dt2_2;	\n"
"\n"
"	atoms[i].s5 += a[oa].s0 * dt_2;\n"
"	atoms[i].s6 += a[oa].s1 * dt_2;\n"
"	atoms[i].s7 += a[oa].s2 * dt_2;\n"
"}\n"
"__kernel void applyVerlet2( __global float4* a, __global float8* atoms, float dt, int nAtoms)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	float dt_2 = dt/2;\n"
"	int oa = i; \n"
"	if(i>=nAtoms) return;\n"
"\n"
"	atoms[i].s5 += a[oa].s0 * dt_2;\n"
"	atoms[i].s6 += a[oa].s1 * dt_2;\n"
"	atoms[i].s7 += a[oa].s2 * dt_2;\n"
"}\n"
"__kernel void applyBeeman1(__global float4* a, __global float8* atoms, float dt, int nAtoms)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	float dt2_8 = dt*dt/8;\n"
"	float dt_8 = dt/8;\n"
"	float terms[3];\n"
"	int oaold = nAtoms+i;\n"
"	int oa = i; \n"
"	if(i>=nAtoms) return;\n"
"	terms[0] = 5.0* a[oa].s0- a[oaold].s0;\n"
"	terms[1] = 5.0* a[oa].s1- a[oaold].s1;\n"
"	terms[2] = 5.0* a[oa].s2- a[oaold].s2;\n"
"\n"
"	atoms[i].s0 += atoms[i].s5 * dt + terms[0]*dt2_8;	\n"
"	atoms[i].s1 += atoms[i].s6 * dt + terms[1]*dt2_8;	\n"
"	atoms[i].s2 += atoms[i].s7 * dt + terms[2]*dt2_8;	\n"
"\n"
"	atoms[i].s5 += terms[0] * dt_8;\n"
"	atoms[i].s6 += terms[1] * dt_8;\n"
"	atoms[i].s7 += terms[2] * dt_8;\n"
"}\n"
"__kernel void applyBeeman2(__global float4* a, __global float8* atoms, float dt, int nAtoms)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	float dt_8 = dt/8;\n"
"	int oaold = nAtoms+i;\n"
"	int oa = i; \n"
"	if(i>=nAtoms) return;\n"
"	atoms[i].s5 += (3.0* a[oa].s0+ a[oaold].s0) * dt_8;\n"
"	atoms[i].s6 += (3.0* a[oa].s1+ a[oaold].s1) * dt_8;\n"
"	atoms[i].s7 += (3.0* a[oa].s2+ a[oaold].s2) * dt_8;\n"
"}\n"
"__kernel void applyStochastic1(__global float4* frictionBuffer, __global float4* a, __global float8* atoms, float dt, int nAtoms)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	float dt2_2 = dt*dt/2;\n"
"	float dt_2 = dt/2;\n"
"	int oaold = nAtoms+i;\n"
"	int oa = i; \n"
"	int ofric = i; // 2 = afric, 1 = vfric, 0 = posfric(one real) \n"
"	int oposrand = nAtoms+i;\n"
"	int ovrand   = 2*nAtoms+i;\n"
"\n"
"	if(i>=nAtoms) return;\n"
"\n"
"	atoms[i].s0 += atoms[i].s5 * frictionBuffer[ofric].s1 + a[oa].s0 * frictionBuffer[ofric].s2 +  frictionBuffer[oposrand].s0;\n"
"	atoms[i].s1 += atoms[i].s6 * frictionBuffer[ofric].s1 + a[oa].s1 * frictionBuffer[ofric].s2 +  frictionBuffer[oposrand].s1;\n"
"	atoms[i].s2 += atoms[i].s7 * frictionBuffer[ofric].s1 + a[oa].s2 * frictionBuffer[ofric].s2 +  frictionBuffer[oposrand].s2;\n"
"\n"
"	atoms[i].s5 =  atoms[i].s5 * frictionBuffer[ofric].s0 + 0.5 * a[oa].s0 * frictionBuffer[ofric].s1;\n"
"	atoms[i].s6 =  atoms[i].s6 * frictionBuffer[ofric].s0 + 0.5 * a[oa].s1 * frictionBuffer[ofric].s1;\n"
"	atoms[i].s7 =  atoms[i].s7 * frictionBuffer[ofric].s0 + 0.5 * a[oa].s2 * frictionBuffer[ofric].s1;\n"

"}\n"
"__kernel void applyStochastic2(__global float4* frictionBuffer,  __global float4* a, __global float8* atoms, float dt, int nAtoms)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	float dt_2 = dt/2;\n"
"	int oa = i; \n"
"	int ofric = i; // 2 = afric, 1 = vfric, 0 = posfric(one real) \n"
"	int ovrand = 2*nAtoms+i;\n"
"	if(i>=nAtoms) return;\n"
"	atoms[i].s5 +=  0.5 * a[oa].s0 * frictionBuffer[ofric].s1 +   frictionBuffer[ovrand].s0;\n"
"	atoms[i].s6 +=  0.5 * a[oa].s1 * frictionBuffer[ofric].s1 +   frictionBuffer[ovrand].s1;\n"
"	atoms[i].s7 +=  0.5 * a[oa].s2 * frictionBuffer[ofric].s1 +   frictionBuffer[ovrand].s2;\n"
"}\n"
" // Generate random numbers\n"
"\n"
"__kernel void generateRandomNumbers( __global float4* randoms, __global uint4* seed, int numValues)\n"
"{\n"
"    int index = get_global_id(0);\n"
"    uint4 state = seed[index];\n"
"    unsigned int carry = 0;\n"
"    while (index < numValues) {\n"
"        float4 value;\n"
"\n"
"        // Generate first value.\n"
"\n"
"        state.x = state.x * 69069 + 1;\n"
"        state.y ^= state.y << 13;\n"
"        state.y ^= state.y >> 17;\n"
"        state.y ^= state.y << 5;\n"
"        unsigned int k = (state.z >> 2) + (state.w >> 3) + (carry >> 2);\n"
"        unsigned int m = state.w + state.w + state.z + carry;\n"
"        state.z = state.w;\n"
"        state.w = m;\n"
"        carry = k >> 30;\n"
"        state.x = state.x * 69069 + 1;\n"
"        state.y ^= state.y << 13;\n"
"        state.y ^= state.y >> 17;\n"
"        state.y ^= state.y << 5;\n"
"        k = (state.z >> 2) + (state.w >> 3) + (carry >> 2);\n"
"        m = state.w + state.w + state.z + carry;\n"
"        state.z = state.w;\n"
"        state.w = m;\n"
"        carry = k >> 30;\n"
"        float x2 = (float)(state.x + state.y + state.w) / (float)0xffffffff;\n"
"        value.x = x2;\n"
"\n"
"        // Generate second value.\n"
"\n"
"        state.x = state.x * 69069 + 1;\n"
"        state.y ^= state.y << 13;\n"
"        state.y ^= state.y >> 17;\n"
"        state.y ^= state.y << 5;\n"
"        k = (state.z >> 2) + (state.w >> 3) + (carry >> 2);\n"
"        m = state.w + state.w + state.z + carry;\n"
"        state.z = state.w;\n"
"        state.w = m;\n"
"        carry = k >> 30;\n"
"        state.x = state.x * 69069 + 1;\n"
"        state.y ^= state.y << 13;\n"
"        state.y ^= state.y >> 17;\n"
"        state.y ^= state.y << 5;\n"
"        k = (state.z >> 2) + (state.w >> 3) + (carry >> 2);\n"
"        m = state.w + state.w + state.z + carry;\n"
"        state.z = state.w;\n"
"        state.w = m;\n"
"        carry = k >> 30;\n"
"        float x4 = (float)(state.x + state.y + state.w) / (float)0xffffffff;\n"
"        value.y = x4;\n"
"\n"
"        // Generate third value.\n"
"\n"
"        state.x = state.x * 69069 + 1;\n"
"        state.y ^= state.y << 13;\n"
"        state.y ^= state.y >> 17;\n"
"        state.y ^= state.y << 5;\n"
"        k = (state.z >> 2) + (state.w >> 3) + (carry >> 2);\n"
"        m = state.w + state.w + state.z + carry;\n"
"        state.z = state.w;\n"
"        state.w = m;\n"
"        carry = k >> 30;\n"
"        state.x = state.x * 69069 + 1;\n"
"        state.y ^= state.y << 13;\n"
"        state.y ^= state.y >> 17;\n"
"        state.y ^= state.y << 5;\n"
"        k = (state.z >> 2) + (state.w >> 3) + (carry >> 2);\n"
"        m = state.w + state.w + state.z + carry;\n"
"        state.z = state.w;\n"
"        state.w = m;\n"
"        carry = k >> 30;\n"
"        float x6 = (float)(state.x + state.y + state.w) / (float)0xffffffff;\n"
"        value.z = x6;\n"
"\n"
"        // Generate fourth value.\n"
"\n"
"        state.x = state.x * 69069 + 1;\n"
"        state.y ^= state.y << 13;\n"
"        state.y ^= state.y >> 17;\n"
"        state.y ^= state.y << 5;\n"
"        k = (state.z >> 2) + (state.w >> 3) + (carry >> 2);\n"
"        m = state.w + state.w + state.z + carry;\n"
"        state.z = state.w;\n"
"        state.w = m;\n"
"        carry = k >> 30;\n"
"        state.x = state.x * 69069 + 1;\n"
"        state.y ^= state.y << 13;\n"
"        state.y ^= state.y >> 17;\n"
"        state.y ^= state.y << 5;\n"
"        k = (state.z >> 2) + (state.w >> 3) + (carry >> 2);\n"
"        m = state.w + state.w + state.z + carry;\n"
"        state.z = state.w;\n"
"        state.w = m;\n"
"        carry = k >> 30;\n"
"        float x8 = (float)(state.x + state.y + state.w) / (float)0xffffffff;\n"
"        value.w = x8;\n"
"\n"
"        // Record the values.\n"
"\n"
"        randoms[index] = value;\n"
"        index += get_global_size(0);\n"
"    }\n"
"    seed[get_global_id(0)] = state;\n"
"}\n"
"float erfinv(float P)\n"
"{\n"
"	float Y, A, B, X, Z, W, WI, SN, SD, F, Z2, SIGMA;\n"
"	float A1 = -.5751703, A2 = -1.896513, A3 = -.5496261E-1;\n"
"	float B0 = -.1137730, B1 = -3.293474, B2 = -2.374996, B3 = -1.187515;\n"
"	float C0 = -.1146666, C1 = -.1314774, C2 = -.2368201, C3 = .5073975e-1;\n"
"	float D0 = -44.27977, D1 = 21.98546, D2 = -7.586103;\n"
"	float E0 = -.5668422E-1, E1 = .3937021, E2 = -.3166501, E3 = .6208963E-1;\n"
"	float F0 = -6.266786, F1 = 4.666263, F2 = -2.962883;\n"
"	float G0 = .1851159E-3, G1 = -.2028152E-2, G2 = -.1498384, G3 = .1078639E-1;\n"
"	float H0 = .9952975E-1, H1 = .5211733, H2 = -.6888301E-1;\n"
"\n"
"	X = P;\n"
"	SIGMA = sign(X);\n"
"	if(X < -1 || X > 1) return 0;\n"
"	Z = fabs(X);\n"
"	if (Z > .85) {\n"
"		A = 1 - Z;\n"
"		B = Z;\n"
"		W = sqrt(-log(A + A * B));\n"
"	if (W >= 2.5) {\n"
"	if (W >= 4.) {\n"
"		WI = 1. / W;\n"
"         	SN = ((G3 * WI + G2) * WI + G1) * WI;\n"
"	        SD = ((WI + H2) * WI + H1) * WI + H0;\n"
"     	  	F = W + W * (G0 + SN / SD);\n"
"       }\n"
"       else {\n"
"		SN = ((E3 * W + E2) * W + E1) * W;\n"
"		SD = ((W + F2) * W + F1) * W + F0;\n"
"		F = W + W * (E0 + SN / SD);\n"
"	}\n"
"       }\n"
"       else {\n"
"       		SN = ((C3 * W + C2) * W + C1) * W;\n"
"       		SD = ((W + D2) * W + D1) * W + D0;\n"
"       		F = W + W * (C0 + SN / SD);\n"
"     	}\n"
"     }\n"
"     else {\n"
"     Z2 = Z * Z;\n"
"     F = Z + Z * (B0 + A1 * Z2 / (B1 + Z2 + A2 / (B2 + Z2 + A3 / (B3 + Z2))));\n"
"   }\n"
"   Y = SIGMA * F;\n"
"   return 0;\n"
"   return Y;\n"
"}\n"
"__kernel void andersen( __global float4* randoms, __global float8* atoms, int nRandoms, int nAtoms, float temperature, float rate)\n"
"{\n"
"    int i = get_global_id(0);\n"
"    int ia = i;\n"
"    float trial = randoms[i++].x;\n"
"    float Kb = 1.9871914e-3;\n"
"    float xs, ys, zs;\n"
"    float l;\n"
"    float speed;\n"
"    if(i>nAtoms) return;\n"
"\n"
"    if(i>nRandoms) i=0;\n"
"    if(trial<rate){\n"
"		float v1,v2,rsq,pnorm;\n"
"		float speed = sqrt(Kb*temperature/atoms[ia].s4);\n"
"		do{\n"
"        		v1 = 2.0 * randoms[i].x - 1.0;\n"
"         		v2 = 2.0 * randoms[i].y - 1.0;\n"
"         		rsq = v1*v1 + v2*v2;\n"
"			i++;\n"
"			if(i>=nRandoms) i = 0;\n"
"		}while(rsq >= 1.0);\n"
"		pnorm = v2 *  sqrt(-2.0*log(rsq)/rsq);\n"
"    		atoms[ia].s5 =  pnorm*speed;\n"
"		pnorm = pnorm/v2*v1;\n"
"    		atoms[ia].s6 =  pnorm*speed;\n"
"		do{\n"
"        		v1 = 2.0 * randoms[i].x - 1.0;\n"
"         		v2 = 2.0 * randoms[i].y - 1.0;\n"
"         		rsq = v1*v1 + v2*v2;\n"
"			i++;\n"
"			if(i>=nRandoms) i = 0;\n"
"		}while(rsq >= 1.0);\n"
"		pnorm = v2 *  sqrt(-2.0*log(rsq)/rsq);\n"
"    		atoms[ia].s7 =  pnorm*speed;\n"
"    	}\n"
"}\n"
"__kernel void setFrictionalAndRandomForce(__global float4* frictionBuffer, __global float8* atoms,  __global float4* randoms, int nRandoms, int nAtoms, float temperature, float dt, float friction)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	float gamma = friction;\n"
"	int ofric = i; // 2 = afric, 1 = vfric, 0 = posfric(one real) \n"
"	int oposrand = nAtoms+i;\n"
"	int ovrand = 2*nAtoms+i;\n"
"	float gdt;\n"
"	float egdt;\n"
"	float ktm = 0;\n"
"	float pterm;\n"
"	float vterm;\n"
"       float psig;\n"
"       float vsig;\n"
"       float rho;\n"
"       float rhoc;\n"
"	float pnorm;\n"
"	float vnorm;\n"
"	float v1,v2,rsq;\n"
"	float Kb = 1.9871914e-3;\n"
"	int ir = i;\n"
"\n"
"	if(i>=nAtoms) return;\n"
"	{\n"
"		gdt = gamma * dt;\n"
"		if (gdt <= 0.0)\n"
"		{\n"
"               	frictionBuffer[ofric].x = 1.0;\n"
"               	frictionBuffer[ofric].y = dt;\n"
"			frictionBuffer[ofric].z = 0.5 * dt * dt;\n"
"                	frictionBuffer[oposrand] = (float4)(0.0, 0.0, 0.0, 0.0);\n"
"                  	frictionBuffer[ovrand] = (float4)(0.0, 0.0, 0.0, 0.0);\n"
"		}\n"
"            	else\n"
"		{\n"
"			// analytical expressions when friction coefficient is large \n"
"               	if (gdt>=0.05)\n"
"			{\n"
"                  		egdt = exp(-gdt);\n"
"               		frictionBuffer[ofric].x = egdt;\n"
"               		frictionBuffer[ofric].y = (1.0-egdt) / gamma;\n"
"               		frictionBuffer[ofric].z =  (dt- frictionBuffer[ofric].y) / gamma;\n"
"                  		pterm = 2.0*gdt - 3.0 + (4.0-egdt)*egdt;\n"
"                  		vterm = 1.0 - egdt*egdt;\n"
"                  		rho = (1.0-egdt)*(1.0-egdt) / sqrt(pterm*vterm);\n"
"			}\n"
"			// use series expansions when friction coefficient is small \n"
"			else\n"
"			{\n"
"                  		float gdt2 = gdt * gdt;\n"
"                  		float gdt3 = gdt * gdt2;\n"
"                  		float gdt4 = gdt2 * gdt2;\n"
"                  		float gdt5 = gdt2 * gdt3;\n"
"                  		float gdt6 = gdt3 * gdt3;\n"
"                  		float gdt7 = gdt3 * gdt4;\n"
"                  		float gdt8 = gdt4 * gdt4;\n"
"                  		float gdt9 = gdt4 * gdt5;\n"
"                  		frictionBuffer[ofric].z = (gdt2/2.0 - gdt3/6.0 + gdt4/24.0\n"
"                               	- gdt5/120.0 + gdt6/720.0\n"
"                               	- gdt7/5040.0 + gdt8/40320.0\n"
"                               	- gdt9/362880.0) / gamma/gamma;\n"
"                  		frictionBuffer[ofric].y = dt - gamma*frictionBuffer[ofric].z;\n"
"                  		frictionBuffer[ofric].x = 1.0 - gamma* frictionBuffer[ofric].y;\n"
"                  		pterm = 2.0*gdt3/3.0 - gdt4/2.0\n"
"                            	+ 7.0*gdt5/30.0 - gdt6/12.0\n"
"                            	+ 31.0*gdt7/1260.0 - gdt8/160.0\n"
"                            	+ 127.0*gdt9/90720.0;\n"
"                  		vterm = 2.0*gdt - 2.0*gdt2 + 4.0*gdt3/3.0\n"
"                            	- 2.0*gdt4/3.0 + 4.0*gdt5/15.0\n"
"                            	- 4.0*gdt6/45.0 + 8.0*gdt7/315.0\n"
"                            	- 2.0*gdt8/315.0 + 4.0*gdt9/2835.0;\n"
"                  		rho = sqrt(3.0) * (0.5 - 3.0*gdt/16.0\n"
"                            	- 17.0*gdt2/1280.0\n"
"                            	+ 17.0*gdt3/6144.0\n"
"                            	+ 40967.0*gdt4/34406400.0\n"
"                            	- 57203.0*gdt5/275251200.0\n"
"                            	- 1429487.0*gdt6/13212057600.0);\n"
"			}\n"
"              		ktm = Kb * temperature / atoms[i].s4;\n"
"              		psig = sqrt(ktm*pterm) / gamma;\n"
"              		vsig = sqrt(ktm*vterm);\n"
"              		rhoc = sqrt(1.0 - rho*rho);\n"
"			{\n"
"				float factor;\n"
"				do{\n"
"        				v1 = 2.0 * randoms[ir].x - 1.0;\n"
"         				v2 = 2.0 * randoms[ir].y - 1.0;\n"
"         				rsq = v1*v1 + v2*v2;\n"
"					ir++;\n"
"					if(ir>=nRandoms) ir = 0;\n"
"				}while(rsq >= 1.0);\n"
"				factor = sqrt(-2.0*log(rsq)/rsq);\n"
"				pnorm = v2 * factor;\n"
"				vnorm = v1 * factor;\n"
"\n"
"                		frictionBuffer[oposrand].x =  psig * pnorm;\n"
"                  		frictionBuffer[ovrand].x =  vsig * (rho*pnorm+rhoc*vnorm);\n"
"\n"
"				do{\n"
"        				v1 = 2.0 * randoms[ir].x  - 1.0;\n"
"         				v2 = 2.0 * randoms[ir].y - 1.0;\n"
"         				rsq = v1*v1 + v2*v2;\n"
"					ir++;\n"
"					if(ir>=nRandoms) ir = 0;\n"
"				}while(rsq >= 1.0);\n"
"				factor = sqrt(-2.0*log(rsq)/rsq);\n"
"				pnorm = v2 * factor;\n"
"				vnorm = v1 * factor;\n"
"\n"
"                		frictionBuffer[oposrand].y =  psig * pnorm;\n"
"                  		frictionBuffer[ovrand].y =  vsig * (rho*pnorm+rhoc*vnorm);\n"
"				do{\n"
"        				v1 = 2.0 * randoms[ir].x  - 1.0;\n"
"         				v2 = 2.0 * randoms[ir].y - 1.0;\n"
"         				rsq = v1*v1 + v2*v2;\n"
"					ir++;\n"
"					if(ir>=nRandoms) ir = 0;\n"
"				}while(rsq >= 1.0);\n"
"				factor = sqrt(-2.0*log(rsq)/rsq);\n"
"				pnorm = v2 * factor;\n"
"				vnorm = v1 * factor;\n"
"                		frictionBuffer[oposrand].z =  psig * pnorm;\n"
"                  		frictionBuffer[ovrand].z =  vsig * (rho*pnorm+rhoc*vnorm);\n"
"			}\n"
"		}\n"
"	}\n"
"}\n"
"__kernel void removeTranslation( __global float8* atoms, int nAtoms)\n"
"{\n"
"	float4 vtot = (float4)(0,0,0,0);\n"
"	int i;\n"
"	float mass = 1.0;\n"
"	float totMass = 0.0;\n"
"	if(get_global_id(0)>0) return;\n"
"	for ( i = 0; i < nAtoms; i++)\n"
"	{\n"
"		mass = atoms[i].s4;\n"
"		totMass += mass;\n"
"		vtot.s0 += mass*atoms[i].s5;\n"
"		vtot.s1 += mass*atoms[i].s6;\n"
"		vtot.s2 += mass*atoms[i].s7;\n"
"	}\n"
"\n"
"	vtot.s0 /= totMass;\n"
"	vtot.s1 /= totMass;\n"
"	vtot.s2 /= totMass;\n"
"\n"
"	for ( i = 0; i < nAtoms; i++)\n"
"	{\n"
"		atoms[i].s5 -= vtot.s0;\n"
"		atoms[i].s6 -= vtot.s1;\n"
"		atoms[i].s7 -= vtot.s2;\n"
"	}\n"
"}\n"
"int InverseTensor(float mat[3][3],float invmat[3][3])\n"
"{\n"
"	float t4,t6,t8,t10,t12,t14,t17;\n"
"	float d = 0;\n"
"	float precision = 1e-6;\n"
"\n"
"	t4 = mat[0][0]*mat[1][1];     \n"
" 	t6 = mat[0][0]*mat[1][2];\n"
"      	t8 = mat[0][1]*mat[1][0];\n"
"      	t10 = mat[0][2]*mat[1][0];\n"
"      	t12 = mat[0][1]*mat[2][0];\n"
"      	t14 = mat[0][2]*mat[2][0];\n"
"      	d =(t4*mat[2][2]-t6*mat[2][1]-t8*mat[2][2]+t10*mat[2][1]+t12*mat[1][2]-t14*mat[1][1]);\n"
"	if(fabs(d)<precision) \n"
"	{\n"
"      		invmat[0][0] = 0;\n"
"      		invmat[0][1] = 0;\n"
"      		invmat[0][2] = 0;\n"
"      		invmat[1][0] = 0;\n"
"      		invmat[1][1] = 0;\n"
"      		invmat[1][2] = 0;\n"
"      		invmat[2][0] = 0;\n"
"      		invmat[2][1] = 0;\n"
"      		invmat[2][2] = 0;\n"
"		return 0;\n"
"	}\n"
"      	t17 = 1/d;\n"
"      	invmat[0][0] = (mat[1][1]*mat[2][2]-mat[1][2]*mat[2][1])*t17;\n"
"      	invmat[0][1] = -(mat[0][1]*mat[2][2]-mat[0][2]*mat[2][1])*t17;\n"
"      	invmat[0][2] = -(-mat[0][1]*mat[1][2]+mat[0][2]*mat[1][1])*t17;\n"
"      	invmat[1][0] = -(mat[1][0]*mat[2][2]-mat[1][2]*mat[2][0])*t17;\n"
"      	invmat[1][1] = (mat[0][0]*mat[2][2]-t14)*t17;\n"
"      	invmat[1][2] = -(t6-t10)*t17;\n"
"      	invmat[2][0] = -(-mat[1][0]*mat[2][1]+mat[1][1]*mat[2][0])*t17;\n"
"      	invmat[2][1] = -(mat[0][0]*mat[2][1]-t12)*t17;\n"
"      	invmat[2][2] = (t4-t8)*t17;\n"
"\n"
"	return 1;\n"
"}\n"
"__kernel void removeRotation( __global float8* atoms, int nAtoms)\n"
"{\n"
"	float vtot[3] = {0,0,0};\n"
"	float cm[3] = {0,0,0};\n"
"	float L[3] = {0,0,0};\n"
"	int i;\n"
"	int j;\n"
"	int k;\n"
"	float mass = 1.0;\n"
"	float totMass = 0.0;\n"
"	float cdel[3];\n"
"	float vAng[3]={0,0,0};\n"
"	float tensor[3][3];\n"
"	float invTensor[3][3];\n"
"        float xx, xy,xz,yy,yz,zz;\n"
"	float coordinates[3];\n"
"	float velocity[3];\n"
"	// find the center of mass coordinates  and total velocity\n"
"\n"
"	for ( i = 0; i < nAtoms; i++)\n"
"	{\n"
"		mass = atoms[i].s4;\n"
"		totMass += mass;\n"
"\n"
"		cm[0] += mass*atoms[i].s0;\n"
"		cm[1] += mass*atoms[i].s1;\n"
"		cm[2] += mass*atoms[i].s2;\n"
"\n"
"		vtot[0] += mass*atoms[i].s5;\n"
"		vtot[1] += mass*atoms[i].s6;\n"
"		vtot[2] += mass*atoms[i].s7;\n"
"	}\n"
"\n"
"\n"
"	for ( j = 0; j < 3; j++) cm[j] /= totMass;\n"
"	for ( j = 0; j < 3; j++) vtot[j] /= totMass;\n"
"\n"
"	//   compute the angular momentum \n"
"	for ( i = 0; i < nAtoms; i++)\n"
"	{\n"
"		mass = atoms[i].s4;\n"
"		coordinates[0] = atoms[i].s0;\n"
"		coordinates[1] = atoms[i].s1;\n"
"		coordinates[2] = atoms[i].s2;\n"
"		velocity[0] = atoms[i].s5;\n"
"		velocity[1] = atoms[i].s6;\n"
"		velocity[2] = atoms[i].s7;\n"
"		for ( j = 0; j < 3; j++)\n"
"			L[j] += (\n"
"				coordinates[(j+1)%3]*velocity[(j+2)%3]\n"
"			      - coordinates[(j+2)%3]*velocity[(j+1)%3]\n"
"			      )*mass;\n"
"	}\n"
"	for ( j = 0; j < 3; j++)\n"
"		L[j] -= (\n"
"			cm[(j+1)%3]*vtot[(j+2)%3]\n"
"		      - cm[(j+2)%3]*vtot[(j+1)%3]\n"
"			      )*totMass;\n"
"\n"
"	// calculate and invert the inertia tensor \n"
"	for ( k = 0; k < 3; k++)\n"
"	for ( j = 0; j < 3; j++)\n"
"		tensor[k][j] = 0;\n"
"	xx = 0;\n"
"	yy = 0;\n"
"	zz = 0;\n"
"	xy = 0;\n"
"	xz = 0;\n"
"	yz = 0;\n"
"	for ( i = 0; i < nAtoms; i++)\n"
"	{\n"
"		mass = atoms[i].s4;\n"
"		coordinates[0] = atoms[i].s0;\n"
"		coordinates[1] = atoms[i].s1;\n"
"		coordinates[2] = atoms[i].s2;\n"
"		for ( j = 0; j < 3; j++)\n"
"			cdel[j] = coordinates[j]-cm[j];\n"
"		xx +=  cdel[0]*cdel[0]*mass;\n"
"		xy +=  cdel[0]*cdel[1]*mass;\n"
"		xz +=  cdel[0]*cdel[2]*mass;\n"
"		yy +=  cdel[1]*cdel[1]*mass;\n"
"		yz +=  cdel[1]*cdel[2]*mass;\n"
"		zz +=  cdel[2]*cdel[2]*mass;\n"
"	}\n"
"	tensor[0][0] = yy+zz;\n"
"	tensor[1][0] = -xy;\n"
"	tensor[2][0] = -xz;\n"
"	tensor[0][1] = -xy;\n"
"	tensor[1][1] = xx+zz;\n"
"	tensor[2][1] = -yz;\n"
"	tensor[0][2] = -xz;\n"
"	tensor[1][2] = -yz;\n"
"	tensor[2][2] = xx+yy;\n"
"	if(InverseTensor(tensor,invTensor)!=0)\n"
"	{\n"
"		for ( j = 0; j < 3; j++)\n"
"		{\n"
"			vAng[j] = 0;\n"
"			for ( k = 0; k < 3; k++)\n"
"				vAng[j] += invTensor[j][k]*L[k];\n"
"		}\n"
"		//  eliminate any rotation about the system center of mass\n"
"		for ( i = 0; i < nAtoms; i++)\n"
"		{\n"
"			coordinates[0] = atoms[i].s0;\n"
"			coordinates[1] = atoms[i].s1;\n"
"			coordinates[2] = atoms[i].s2;\n"
"			velocity[0] = atoms[i].s5;\n"
"			velocity[1] = atoms[i].s6;\n"
"			velocity[2] = atoms[i].s7;\n"
"			for ( j = 0; j < 3; j++) cdel[j] = coordinates[j]-cm[j];\n"
"			for ( j = 0; j < 3; j++)\n"
"				velocity[j] += \n"
"					cdel[(j+1)%3]*vAng[(j+2)%3]-\n"
"					cdel[(j+2)%3]*vAng[(j+1)%3];\n"
"			atoms[i].s5 =  velocity[0];\n"
"			atoms[i].s6 =  velocity[1];\n"
"			atoms[i].s7 =  velocity[2];\n"
"		}\n"
"	}\n"
"}\n"
"__kernel void initRattles(__global float4* rattledeltaPositionBuffer,   __global float4* rattledeltaVelocityBuffer, __global int* rattledUpdateBuffer, __global int* rattleMovedBuffer, int nAtoms, int nBlocks)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	size_t i=j;\n"
"	float4 tmp = (float4) (0,0,0,0);\n"
"	if(j>=nAtoms*nBlocks) return;\n"
"	for(i=j;i<nAtoms*nBlocks;i+=get_global_size(0))\n"
"	{\n"
"	    rattledeltaPositionBuffer[i]=tmp;\n"
"	    rattledeltaVelocityBuffer[i]=tmp;\n"
"	    rattledUpdateBuffer[i]=0;\n"
"	    if(i<nAtoms) rattleMovedBuffer[i]=1;\n"
"	}\n"
"}\n"
"__kernel void updateRattle1(__global float4* rattledeltaPositionBuffer, __global float4* rattledeltaVelocityBuffer, __global int* rattledUpdateBuffer,  __global int* rattleMovedBuffer,  __global float8* atoms, int nAtoms, int nBlocks)\n"
"{\n"
"	size_t k = get_global_id(0);\n"
"	size_t j;\n"
"	float4 sump;\n"
"	float4 sumv;\n"
"	int sumint;\n"
"	int i;\n"
"	float4 tmp = (float4) (0,0,0,0);\n"
"	for(j=k;j<nAtoms;j+=get_global_size(0))\n"
"	{\n"
"		sump = rattledeltaPositionBuffer[j];\n"
"		sumv = rattledeltaVelocityBuffer[j];\n"
"		sumint = rattledUpdateBuffer[j];\n"
"		for(i=j+nAtoms;i<nAtoms*nBlocks;i+=nAtoms)\n"
"		{\n"
"	    		sump += rattledeltaPositionBuffer[i];\n"
"	    		sumv += rattledeltaVelocityBuffer[i];\n"
"	    		sumint += rattledUpdateBuffer[i];\n"
"		}\n"
"		atoms[j].s0 += sump.s0;\n"
"		atoms[j].s1 += sump.s1;\n"
"		atoms[j].s2 += sump.s2;\n"
"		atoms[j].s5 += sumv.s0;\n"
"		atoms[j].s6 += sumv.s1;\n"
"		atoms[j].s7 += sumv.s2;\n"
"		rattleMovedBuffer[j] = (sumint>0)?1:0;\n"
"		for(i=j;i<nAtoms*nBlocks;i+=nAtoms)\n"
"		{\n"
"			rattledeltaPositionBuffer[i] = tmp;\n"
"	    		rattledeltaVelocityBuffer[i]=tmp;\n"
"	    		rattledUpdateBuffer[i]=0;\n"
"		}\n"
"	}\n"
"}\n"
"__kernel void testDoneRattle(__global int* rattleMovedBuffer, __global int* doneBuffer, int nAtoms)\n"
"{\n"
"	size_t i = get_global_id(0);\n"
"	int done = 0;\n"
"	if(i>=1) return;\n"
"	for(i=0;i<nAtoms;++i)\n"
"	      done += rattleMovedBuffer[i];\n"
"	if(done>0) doneBuffer[0] = 0;\n"
"	else doneBuffer[0] = 1;\n"
"}\n"
"__kernel void applyRattle1(__global float4* oldPositionBuffer, __global int4* bondIndex, __global float* r2Terms, __global float4* rattledeltaPositionBuffer, __global float4* rattledeltaVelocityBuffer, __global int* rattledUpdateBuffer,  __global int* rattleMovedBuffer,  __global float8* atoms, int nAtoms, int nTerms, float dt)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	int i,k;\n"
"	int ai,aj,oi,oj;\n"
"	float r2ij;\n"
"	float dot;\n"
"	float invMass1;\n"
"	float invMass2;\n"
"	float delta;\n"
"	float term = 0;\n"
"	float4 terms;\n"
"	float d;\n"
"	float deltaMax = 0;\n"
"	int maxIter = 1000;\n"
"	float omega = 1.0; \n"
"	float tolerance = 1e-6;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0))\n"
"	{\n"
"		ai = bondIndex[i].s0;\n"
"		aj = bondIndex[i].s1;\n"
"		oi = ai+nAtoms*bondIndex[i].s2;\n"
"		oj = aj+nAtoms*bondIndex[i].s3;\n"
"		if( !rattleMovedBuffer[ai] && !rattleMovedBuffer[aj] ) continue;\n"
"		r2ij = 0;\n"
"		{\n"
"			d = atoms[aj].s0-atoms[ai].s0;\n"
"			r2ij +=d*d;\n"
"			d = atoms[aj].s1-atoms[ai].s1;\n"
"			r2ij +=d*d;\n"
"			d = atoms[aj].s2-atoms[ai].s2;\n"
"			r2ij +=d*d;\n"
"		}\n"
"		delta = r2Terms[i]-r2ij;\n"
"		/* if(fabs(delta)<=tolerance) continue;*/\n"
"		if(r2ij>0 && fabs(delta/r2ij)<=tolerance) continue;\n"
"		if(deltaMax<fabs(delta)) deltaMax = fabs(delta);\n"
"		rattledUpdateBuffer[oi] = 1;\n"
"		rattledUpdateBuffer[oj] = 1;\n"
"		/* here : rattle image for PBC, not yet implemented */\n"
"		dot = 0;\n"
"		{\n"
"			d = atoms[aj].s0-atoms[ai].s0;\n"
"			dot +=d*(oldPositionBuffer[aj].s0-oldPositionBuffer[ai].s0);\n"
"			d = atoms[aj].s1-atoms[ai].s1;\n"
"			dot +=d*(oldPositionBuffer[aj].s1-oldPositionBuffer[ai].s1);\n"
"			d = atoms[aj].s2-atoms[ai].s2;\n"
"			dot +=d*(oldPositionBuffer[aj].s2-oldPositionBuffer[ai].s2);\n"
"		}\n"
"		invMass1 = 1/atoms[ai].s4;\n"
"		invMass2 = 1/atoms[aj].s4;\n"
"		term = omega*delta / (2.0*(invMass1+invMass2)*dot);\n"
"		terms.xyz = (oldPositionBuffer[aj].xyz-oldPositionBuffer[ai].xyz)*term;\n"
"		rattledeltaPositionBuffer[oi].xyz = -terms.xyz*invMass1;\n"
"		rattledeltaPositionBuffer[oj].xyz =  terms.xyz*invMass2;\n"
"\n"
"		invMass1 /= dt;\n"
"		invMass2 /= dt;\n"
"		rattledeltaVelocityBuffer[oi].xyz = -terms.xyz*invMass1;\n"
"		rattledeltaVelocityBuffer[oj].xyz =  terms.xyz*invMass2;\n"
"	}\n"
"}\n"
"__kernel void updateRattle2(__global float4* rattledeltaVelocityBuffer, __global int* rattledUpdateBuffer,  __global int* rattleMovedBuffer,  __global float8* atoms, int nAtoms, int nBlocks)\n"
"{\n"
"	size_t k = get_global_id(0);\n"
"	size_t j;\n"
"	float4 sumv;\n"
"	int sumint;\n"
"	int i;\n"
"	float4 tmp = (float4) (0,0,0,0);\n"
"	for(j=k;j<nAtoms;j+=get_global_size(0))\n"
"	{\n"
"		sumv = rattledeltaVelocityBuffer[j];\n"
"		sumint = rattledUpdateBuffer[j];\n"
"		for(i=j+nAtoms;i<nAtoms*nBlocks;i+=nAtoms)\n"
"		{\n"
"	    		sumv += rattledeltaVelocityBuffer[i];\n"
"	    		sumint += rattledUpdateBuffer[i];\n"
"		}\n"
"		atoms[j].s5 += sumv.s0;\n"
"		atoms[j].s6 += sumv.s1;\n"
"		atoms[j].s7 += sumv.s2;\n"
"		rattleMovedBuffer[j] = (sumint>0)?1:0;\n"
"		for(i=j;i<nAtoms*nBlocks;i+=nAtoms)\n"
"		{\n"
"	    		rattledeltaVelocityBuffer[i]=tmp;\n"
"	    		rattledUpdateBuffer[i]=0;\n"
"		}\n"
"	}\n"
"}\n"
"__kernel void applyRattle2(__global int4* bondIndex, __global float* r2Terms,  __global float4* rattledeltaVelocityBuffer, __global int* rattledUpdateBuffer,  __global int* rattleMovedBuffer,  __global float8* atoms, int nAtoms, int nTerms, float dt)\n"
"{\n"
"	size_t j = get_global_id(0);\n"
"	int i,k;\n"
"	int ai,aj,oi,oj;\n"
"	float r2ij;\n"
"	float dot;\n"
"	float invMass1;\n"
"	float invMass2;\n"
"	float delta;\n"
"	float term = 0;\n"
"	float4 terms;\n"
"	float d;\n"
"	float deltaMax = 0;\n"
"	int maxIter = 1000;\n"
"	float omega = 1.0; \n"
"	float tolerance = 1e-6;\n"
"	tolerance /= dt;\n"
"	for(i=j;i<nTerms;i+=get_global_size(0))\n"
"	{\n"
"		ai = bondIndex[i].s0;\n"
"		aj = bondIndex[i].s1;\n"
"		oi = ai+nAtoms*bondIndex[i].s2;\n"
"		oj = aj+nAtoms*bondIndex[i].s3;\n"
"		if( !rattleMovedBuffer[ai] && !rattleMovedBuffer[aj] ) continue;\n"
"		/* here : rattle image for PBC, not yet implemented */\n"
"		dot = 0;\n"
"		{\n"
"			d = atoms[aj].s0-atoms[ai].s0;\n"
"			dot +=d*(atoms[aj].s5-atoms[ai].s5);\n"
"			d = atoms[aj].s1-atoms[ai].s1;\n"
"			dot +=d*(atoms[aj].s6-atoms[ai].s6);\n"
"			d = atoms[aj].s2-atoms[ai].s2;\n"
"			dot +=d*(atoms[aj].s7-atoms[ai].s7);\n"
"		}\n"
"		invMass1 = 1/atoms[ai].s4;\n"
"		invMass2 = 1/atoms[aj].s4;\n"
"		term =  -dot / ((invMass1+invMass2)*r2Terms[i]);\n"
"		if(fabs(term)<=tolerance) continue;\n"
"		if(deltaMax<fabs(term)) deltaMax = fabs(term);\n"
"		rattledUpdateBuffer[oi] = 1;\n"
"		rattledUpdateBuffer[oj] = 1;\n"
"		term *= omega;\n"
"		terms.xyz = term*(atoms[aj].xyz - atoms[ai].xyz);\n"
"		rattledeltaVelocityBuffer[oi].xyz -= terms.xyz*invMass1;\n"
"		rattledeltaVelocityBuffer[oj].xyz += terms.xyz*invMass2;\n"
"	}\n"
"}\n"
;
