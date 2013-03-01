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


// Initialize - run - finalize functions for Werner Dune

void init_wdune()  // initialization
{
    FILE *pSurf, *pBsmt;

    // seed the random number generator
    timeval tm;
    gettimeofday(&tm, NULL);
    init_genrand (tm.tv_usec);  // seed mersenne twister with milliseconds

    // print arguments to console
    cout << "Core release: 28 October 2011" << endl;

    cout << "Arguments passed to core:"
        << "\n    Iterations = " << numIterations
        << "\n    Wind direction = " << wdir
        << "\n    Deposition jump = " << depjump
        << "\n    P Sand = " << psand
        << "\n    P Basement = " << pnosand
        << "\n    Shadow drop = " << dropdist
        << "\n    Number of rows = " << nrows
        << "\n    Number of columns = " << ncols
        << "\n    Boundaries code = " << bound_type
        << "\n    New sand code = " << newSandCode
        << "\n    New sand slabs = " << newSandSlabs << endl;

    // read in the input files
    // topography
    int scan;       // dummy variable to store return values
    pSurf = fopen ("surf.txt", "r");
    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < ncols; j++)
        {
            scan = fscanf (pSurf, "%d", &surf[i][j]);
        }
    }
    fclose (pSurf);

    // basement
    pBsmt = fopen ("bsmt.txt", "r");
    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < ncols; j++)
        {
            scan = fscanf (pBsmt, "%d", &bsmt[i][j]);
        }
    }
    fclose (pBsmt);

    // set the boundary lookups
    if (bound_type == 1) { nonperiodic_bounds(); }
    if (bound_type == 2) { periodic_bounds(); }
    if (bound_type == 3) { nonperiodic_bounds_EW(); }
    if (bound_type == 4) { nonperiodic_bounds_NS(); }

    init_shadupdate();      // update the shadow for the first time

    cout << "Initialization complete . . entering time loop" << endl;
}

void run_wdune()   // run
{
    int t_poll = 0;                         // poll counter variable
    while (t_poll < (ncols * nrows))
    {
        picksite_ero();                     // pick a site to erode from
        if (ero_flag)                       // flag is true if the site is good for erosion
        {
            surf[i_ero][j_ero]--;               // remove a slab off the erosion site
            avalanche_up(i_ero, j_ero);         // avalanche up after removing the sand
            picksite_depo(i_ero, j_ero);        // pick a site to deposit the sand
            deposit(i_depo, j_depo);            // put a slab of sand onto the deposition site
        }
        t_poll++;                           // advance the poll counter
    }
    newSandEngine();                        // add some new sand if required
}

void final_wdune()     // finalization
{
    cout << "Exiting time loop . . finalization beginning" << endl;
    cout << "Number of slabs that were transported out of modelspace: " << slabs_out << endl;
    FILE *pSurf;

    // write out the surface array, overwrite what was there originally
    pSurf = fopen ("surf.txt", "w");
    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < (ncols - 2); j++)
        {
            fprintf (pSurf, "%i ", surf[i][j]);
        }
        fprintf (pSurf, "%i", surf[i][ncols - 2]);
        fprintf (pSurf, " %i", surf[i][ncols - 1]);
        fprintf (pSurf, "%s", "\n");    // endline character
    }
    fclose (pSurf);

    cout << "Finalization complete" << endl;
}


