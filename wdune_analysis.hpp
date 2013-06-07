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

// WDUNE analysis functions
// Object definitions for analysis functions are first, followed by generic wrapper functions
// NOTE: some analysis functions require deep integration with the model, so there may be 

class slablogger {
	/*
	The slablogger records the dune field wide sediment flux through a gate at the downwind edge
	of the model space. Note there is a direction to this flux. For example, if a slab avalanches
	upwind across the gate (entirely possible), the number of slabs passing over the downwind
	boundary will decrement. I've set it up to record slabs passing through the gate in transport
	and slabs passing through the gate in avalanche separately.
	
	The flux is in units of number of slabs per iteration (assuming the record method is called
	every iteration).	
	*/
	
	public:
		int trans_log;			// the number of slabs that pass the downwind edge in transport
		int avi_log;			// the number of slabs that pass the downwind edge in avalanche
		
		int * iter;				// Pointer leads for initialized arrays
		int * trans;
		int * avi;
		
		// flux arrays are used to know when a slab is deposited off the edge of the model space
		// 0 = sediment in movement isn't contributing to the slablogger flux
		// 1 = sediment in movement is contributing to the slablogger flux (passing downwind over edge)
		// -1 = sediment in movement is contributing negatively to the slablogger flux (passing upwind over edge)
		int * i_n_flux;			// Pointer leads for flux arrays
		int * i_s_flux;
		int * j_e_flux;
		int * j_w_flux;
		int * i_dp_flux;
		int * j_dp_flux;
		
		//  CONSTRUCTOR
		slablogger() {

		}
		
		void init() {
			// INITIALIZE the slablogger object: called at runtime
			trans_log = 0;
			avi_log = 0;
			
			// allocate new memory for the slablogger variables
			try {
				iter = new int [numIterations];
				trans = new int [numIterations];
				avi = new int [numIterations];
				i_n_flux = new int [nrows];
				i_s_flux = new int [nrows];
				j_e_flux = new int [ncols];
				j_w_flux = new int [ncols];
				i_dp_flux = new int [nrows];
				j_dp_flux = new int [ncols];				
				
			}
			catch(...) {
				cout << "CANNOT ALLOCATE MEMORY!!" << endl;
				exit (10);
			}
			
			// populating the flux variables
			// Set everything to 0 to start things out
			for (int ff = 0; ff < nrows; ff++) {
				i_n_flux[ff] = 0;
				i_s_flux[ff] = 0;
				i_dp_flux[ff] = 0;			
			}
			
			for (int ff = 0; ff < ncols; ff++) {
				j_e_flux[ff] = 0;
				j_w_flux[ff] = 0;
				j_dp_flux[ff] = 0;			
			}
			
			// Flux passing over the downwind edge will contribute positively or negatively
			// to the slablogger measured flux
			// We make use of the wind direction global variable 'wdir' where:
			// 1 = north, 2 = South, 3 = east, 4 = west (direction wind is coming from)
			// to define whether the flux adds negatively or positively to the total flux
			if (wdir == 1) {
				i_n_flux[0] = -1;
				i_s_flux[nrows - 1] = 1;
			}
			else if (wdir == 2) {
				i_n_flux[0] = 1;
				i_s_flux[nrows - 1] = -1;			
			}
			else if (wdir == 3) {
				j_e_flux[0] = 1;
				j_w_flux[ncols - 1] = -1;			
			}
			else if (wdir == 4) {
				j_e_flux[0] = - 1;
				j_w_flux[ncols - 1] = 1;
			}
			else {
				cout << "ERROR WITH WIND DIRECTION" << endl;
				exit (5);
			}
			
			// The deposition lookups are a bit more complicated
			// we can look for situations where the difference between the present coordinate
			// and the deposit coordinate designated by the deposit lookups is greater than
			// or less than 1 - we know it is referring to a coordinate across an edge
			for (int ff = 0; ff < nrows; ff++) {
				if (i_dp[ff] == i_toxic) {
					i_dp_flux[ff] = 1;
				}
				if ((i_dp[ff] - ff) > 1 || (i_dp[ff] - ff) < 1) {
					i_dp_flux[ff] = 1;
				}
			}
			for (int ff = 0; ff < ncols; ff++) {
				if (j_dp[ff] == j_toxic) {
					j_dp_flux[ff] = 1;
				}
				if ((j_dp[ff] - ff) > 1 || (j_dp[ff] - ff) < 1) {
					j_dp_flux[ff] = 1;
				}
			}
		}
		
		// Method to increment the counter based on transport
		void increment_trans(int i_trans, int j_trans) {
			// Arguments are the coordinates of the site just before the focal
			// coordinates are moved and the slab is assessed for deposition
			if (i_dp_flux[i_trans] || j_dp_flux[j_trans]) {
				trans_log++;	// the slab is about to move across a model boundary!
			}
		}
		
		// Method to increment with avalanches
		void increment_avi(int i_avi, int j_avi, int avi_dir) {
			// Arguments are the coordinates of the site, after it is approved that
			// the avalanche will occur, and the direction the slab is going to be
			// moved to. In the case of avalanche up, this function should be called
			// at the coordinates of the cell that will fall down, in the case of avalanche
			// down, this should be called at the coordinates just before the sediment is
			// avalanched down
			
			// Avalanche direction:  1 = north, 2 = South, 3 = east, 4 = west
			if (avi_dir == 1) {
				avi_log = avi_log + i_n_flux[i_avi];
			}
			if (avi_dir == 2) {
				avi_log = avi_log + i_s_flux[i_avi];
			}
			if (avi_dir == 3) {
				avi_log = avi_log + j_e_flux[j_avi];
			}
			if (avi_dir == 4) {
				avi_log = avi_log + j_w_flux[j_avi];
			}
		}
		
		// Method to record the variables to the internal array at each iteration end
		void record() {
			iter[t] = t;
			trans[t] = trans_log;
			avi[t] = avi_log;
			// reset the variables
			trans_log = 0;
			avi_log = 0;
		}
		
		// Method to finalize
		void finalize() {
			// Open up a file and record out the slab log to a csv file
			FILE *pSlabLog;
			pSlabLog = fopen ("slab_log.csv", "w");
			// write out a header
			fprintf (pSlabLog, "%s", "iteration,trans_pass,avi_pass\n");
			
			// write out the data
			for (int w = 0; w < numIterations; w++) {
				fprintf (pSlabLog, "%i,", iter[w]);
				fprintf (pSlabLog, "%i,", trans[w]);
				fprintf (pSlabLog, "%i", avi[w]);
				fprintf (pSlabLog, "%s", "\n");			
			}
					
			fclose (pSlabLog);
		}
};

// Initialization of slablogger object in GLOBAL SCOPE!
slablogger wdune_slablogger;

// Initialize the analysis functions
void init_analysis()
{
	wdune_slablogger.init();	// initialize at runtime (allocate memory)
}

// Run analysis functions
void analyze_wdune()
{
	wdune_slablogger.record();
}

// Finalize the analysis functions
void final_analysis()
{
	wdune_slablogger.finalize();
}




