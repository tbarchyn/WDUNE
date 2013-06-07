/*
wdune: This is an accessible and freely available interpretation of a cellular automata
simulation program for sand dunes. Please note that the random number generator
has a different license than this program, see file in this directory: 'mersenne_twister.h'.

Copyright (C) 2011 Thomas E. Barchyn, Chris H. Hugenholtz
Contact: tom.barchyn@uleth.ca, +1 (403) 332-4043

License:
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

Credits:
This program is further detailed in a accompanying publication. The code is an
interpretation of a simulation algorithm first described in the following publication:

Werner, B.T., 1995. Eolian dunes: Computer simulations and attractor interpretation.
Geology 23, 1107-1110. DOI: 10.1130/0091-7613(1995)023<1107:EDCSAA>2.3.CO;2

If you are using this program for research, we would appreciate citation of
both papers.

Notes:
This program is written in C/C++ and has been compiled successfully with GCC 4.4.1 in
both Windows (XP, Vista, 7) and Linux (Ubuntu 11.04). We have used the following compiler
flags: -Wall -pedantic -O1. The program will function on some systems with higher optimization
but we have encountered problems in some cases with -O2 and -O3.

This program is designed to be called exclusively from a Python script as a long string
of arguments need to be passed to the executable. The idea being that the Python script
can easily be modified for batch operation, etc. Please contact Tom Barchyn for further
assistance if you wish to extend the program (tom.barchyn@uleth.ca).
*/

// include standard libraries
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sys/time.h>
#include <math.h>

using namespace std;

// include the program as header file
#include "mersenne_twister.h"     		// include the random number generator: Mersenne Twister
#include "wdune_globals.hpp"      		// global variables
#include "wdune_analysis.hpp"	  		// analysis functions
#include "wdune_functions.hpp"    		// IRF function definitions
#include "wdune_irfs.hpp"         		// core functions, called by the IRF functions
#include "wdune_acc.hpp"          		// accessory functions
//#include "wdune_default_params.hpp"		// a basic default parameter file for debugging purposes

int main(int nArgs, char *pszArgs[])
{
    /*
	Note: this program requires 11 arguments (space separated)
		Arguments:
        Number of iterations (integer)
        Wind direction (integer one of 1 = north, 2 = South, 3 = east, 4 = west)
        Deposition jump (integer)
        Probability of depositing on sand (double)
        Probability of depositing on no sand (double)
        Drop distance of shadow downwind (double)
        Number of rows (integer)
        Number of columns (integer)
        Type of boundaries (integer, 1 = non-periodic, 2 = periodic, 3 = non-periodic EW,
                            4 = non-periodic NW)
        New sand code (integer code to allow new sand: 2 digits: 1st: 1 = point, 2 = edge;
            2nd = side (1 = north, 2 = South, 3 = east, 4 = west))
        New sand slabs (number of slabs to add)

    Input files:
    1) 'surf.txt': integer space separated grid of surface slab heights
    2) 'bsmt.txt': integer space separated grid of non-erodible basement height

    Output files:
    1) 'surf.txt': integer space separated grid of output surface slab heights (overwrites input)
    */
	/*
	numIterations = atoi (pszArgs[1]);
	wdir = atoi (pszArgs[2]);
	depjump = atoi (pszArgs[3]);
	psand = atof (pszArgs[4]);
	pnosand = atof (pszArgs[5]);
	dropdist = atof (pszArgs[6]);
	nrows = atoi (pszArgs[7]);
	ncols = atoi (pszArgs[8]);
	bound_type = atoi (pszArgs[9]);
	newSandCode = atoi (pszArgs[10]);
	newSandSlabs = atoi (pszArgs[11]);
	*/	
	// if input variables are not supplied, operate the program with basic inputs
	set_default_params();
	
    // A) initialize
    init_wdune();

    // B) loop
    while (t < numIterations)
    {
        // run, called every timestep
        run_wdune();
		timePrinter();
        t++;
    }

    // C) finalize
    final_wdune();
    return 0;
}
