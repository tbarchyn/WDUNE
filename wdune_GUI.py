# wdune: This is an accessible and freely available interpretation of a cellular automata
# simulation program for sand dunes. Please note that the random number generator
# has a different license than this program, see file in this directory: 'mersenne_twister.h'.
#
# Copyright (C) 2011 Thomas E. Barchyn, Chris H. Hugenholtz
# Contact: tom.barchyn@uleth.ca, +1 (403) 332-4043
#
# License:
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
# Credits:
# This program is further detailed in an accompanying publication: 
#
# Barchyn, T.E., Hugenholtz, C.H., 2012. A new tool for modeling dune field evolution
# based on an accessible, GUI version of the Werner dune model. Geomorphology.
#
# The code is an interpretation of a simulation algorithm first described in the 
# following publication:
#
# Werner, B.T., 1995. Eolian dunes: Computer simulations and attractor interpretation.
# Geology 23, 1107-1110. DOI: 10.1130/0091-7613(1995)023<1107:EDCSAA>2.3.CO;2
#

# Werner Dune GUI

# This script runs the GUI program
# Note that this program has been tested on Python versions from 2.1 to 3.2
# It may or may not work with newer or older version of Python.

# ----------------------------------------------------------------------------------
# Import modules
import os, random, math, sys, shutil, time

# load tkinter packages (for python versions 2.x)
if sys.version_info[0] == 2:
    from Tkinter import *
    import tkSimpleDialog, tkFileDialog, tkMessageBox

# load tkinter packages (for python versions 3.x)
if sys.version_info[0] == 3:
    from tkinter import *
    import tkinter.simpledialog as tkSimpleDialog
    import tkinter.filedialog as tkFileDialog
    import tkinter.messagebox as tkMessageBox

# set the working directory to the core directory
if os.path.basename (sys.argv[0]) == "wdune_GUI.py":
    os.chdir (sys.path[0])
    def_dir = "C://"
else:
    # else, the working directory is ./core/
    # set path names differently based on operating system
    if os.name == 'nt':
        os.chdir (sys.path[0] + "\\core")
        def_dir = "C://"
    if os.name == 'posix':
        os.chdir (sys.path[0] + "/core")
        def_dir = "/home"
    

# set variables and defaults
global bounds_NS, bounds_EW, outSurf_dir, outSurf, outBsmt_dir, outBsmt
global inSurf_dir, inBsmt_dir, inSurf, inBsmt, version

bounds_NS = 1
bounds_EW = 1
outSurf_dir = def_dir
outBsmt_dir = def_dir
inSurf_dir = def_dir
inBsmt_dir = def_dir
status_oput = "Ready ..."

# try to find an existing configuration file to populate defaults
# try in Windows:
if os.name == 'nt':
    # first try in local directory
    if os.path.isfile ("config.txt"):
        confile = open ("config.txt", 'r')
        version = confile.readline()        [:-1]
        outSurf_dir = confile.readline()    [:-1]
        outBsmt_dir = confile.readline()    [:-1]
        inSurf_dir = confile.readline()     [:-1]
        inBsmt_dir = confile.readline()     [:-1]
        confile.close()
    # else check in the installation folder
    elif os.path.isfile ("C://wdune//config.txt"):
        confile = open ("C://wdune//config.txt", 'r')
        version = confile.readline()         [:-1]
        outSurf_dir = confile.readline()     [:-1]
        outBsmt_dir = confile.readline()     [:-1]
        inSurf_dir = confile.readline()      [:-1]
        inBsmt_dir = confile.readline()      [:-1]
        confile.close()
    # else, there is no configuration file and the model will run standalone mode
    else:
        version = "S"
        
# in Linux, ArcGIS cannot be installed, so the only mode is standalone
if os.name == 'posix':
    version = "S"


# Functions
# ----------------------------------------------------------------------------------
# ArcGIS finishing program, to finish off ArcGIS files
def ArcGISfinish(outSurf, outBsmt, outSurf_dir, outBsmt_dir, version):
    # ArcGIS 9.2, 9.3, and 10.0 will all read Arc ASCII files natively, however,
    # the loading of files can be improved by pre-calculating raster statistics.
    # These statistics are calculated and stored in accompanying XML files. This
    # is primarily meant for convenience, and is not a vital part of the program
    # or model.

    # We recommend users with ArcGIS 9.1 or older should investigate using another
    # GIS viewer to examine model outputs. Older versions of ArcGIS require users
    # to convert Arc ASCII files to another format prior to viewing with the
    # "ASCII to Raster" tool in the "Conversion toolbox". Our experiences with this
    # conversion have been mixed. It is also inconvenient.

    # ArcGIS versions 9.2 and 9.3 have identical instructions 
    if version == "9.2" or version == "9.3":
        try:
            # import ArcGIS scripting library
            import arcgisscripting
            gp = arcgisscripting.create()

            # calculate statistics
            gp.CalculateStatistics_management (outSurf)
            gp.CalculateStatistics_management (outBsmt)
        except:
            pass

    # ArcGIS version 10.0 instructions
    if version == "10.0":
        try:
            # import ArcGIS scripting library
            import arcpy

            # calculate statistics
            arcpy.CalculateStatistics_management (outSurf)
            arcpy.CalculateStatistics_management (outBsmt)
        except:
            pass

    return

# ----------------------------------------------------------------------------------
# Configuration file writer
def configWrite():
    # write a configuration file locally, to store parameters for next time program opens
    global outSurf_dir, outBsmt_dir, inSurf_dir, inBsmt_dir, version
    # double check that an ArcGIS mode is not selected when running in Linux
    if os.name == 'posix' and version != "S":
        version = "S"
    # open the file and write it out locally
    confile = open ("config.txt", 'w')
    confile.write (version + "\n")
    confile.write (outSurf_dir + "\n")
    confile.write (outBsmt_dir + "\n")
    confile.write (inSurf_dir + "\n")
    confile.write (inBsmt_dir)
    confile.close()
    return

# ----------------------------------------------------------------------------------
# Asciifier
def asciifier (infile, outfile):
    # convert a space separated grid to a Arc ASCII file
    # infile: the filename of the grid to be asciified
    # outfile: the filename of the ascii grid

    # Note: this function and the de-asciifier function could be very much streamlined
    # with the use of Numerical Python (NumPy), we've avoided using NumPy because
    # not all Python distributions come with NumPy (see: numpy.scipy.org)
    # copy over the file to the new location
    shutil.copy2 (infile, outfile)

    # figure out the number of rows and columns (necessary to write header)
    newf = open (outfile, "r+")
    ncols = len ( (newf.readline()).split (" ") ) # split to list, then count elements
    nrows = 1
    while (len ( (newf.readline()).split (" ") ) == ncols):
        nrows = nrows + 1

    newf.seek (0)                               # back to the beginning of the file
    newfdata = newf.read()                      # read everything in
    newf.seek (0)                               # back to the beginning, to write header

    # write the header (projection can be modified with the header string if desired)
    newf.write ("ncols " + str (ncols) + "\n" +
                "nrows " + str (nrows) + "\n" +
                "xllcorner 0.0\nyllcorner 0.0\ncellsize 1.0\nNODATA_value -9999\n" +
                newfdata)
    newf.close()                                # close the file object
    return

# ----------------------------------------------------------------------------------
# De-asciifier
def deasciifier (infile, outfile):
    # convert a Arc ASCII file to a space separated grid
    # infile: the filename of the grid to be deasciified
    # outfile: the filename of the raw space separated grid
    # open input file
    oldf = open (infile, "r")
    # skip forward 6 lines to get past the header
    for i in range (0,6):
        oldf.readline()
    oldfdata = oldf.read()      # read in the data
    oldf.close()

    # write the new file without the header
    newf = open (outfile, "w")
    newf.write (oldfdata)
    newf.close()
    return

# ----------------------------------------------------------------------------------
# Set input: surface file
def setFileSurf():
    # set a surface file for input into the model
    global inSurf_dir, inSurf
    inSurf_name = ""
    # open a dialog to ask for the file name
    inSurf = tkFileDialog.askopenfilename(initialdir = inSurf_dir, parent = root, title = "Input surface file",
                filetypes = [('Arc ASCII Grids', '.asc'), ('All files', '.*')])

    # if a file is chosen, separate the directory and name
    if inSurf != "":
        # separate instructions for Linux and Windows
        if os.name == 'nt':
            # split up the string to extract directory and name
            surfSplit = inSurf.split ("/")
            inSurf_dir = "/".join (surfSplit [0:(len (surfSplit) - 1)])
            inSurf_name = surfSplit [len (surfSplit) - 1]

        if os.name == 'posix':
            # split up the string to extract directory and name
            surfSplit = inSurf.split ("/")
            inSurf_dir = "/" + "/".join (surfSplit [0:(len (surfSplit) - 1)])
            inSurf_name = surfSplit [len (surfSplit) - 1]
      
        status.config(text =  "Input surface file set: " + inSurf)
        p8_2.config (text = inSurf_name)
        configWrite()

    # else, the user selected 'Cancel', print error message on status bar            
    else:
        status.config(text =  "ERROR: Failed to set input surface file")
        p8_2.config (text = "Choose file ...")
        
    return

# ----------------------------------------------------------------------------------
# Set input: basement file
def setFileBsmt():
    # set a basement file for input into the model
    global inBsmt_dir, inBsmt
    inBsmt_name = ""
    # open a dialog to ask for the file name
    inBsmt = tkFileDialog.askopenfilename(initialdir = inBsmt_dir, parent = root, title = "Input basement file",
                filetypes = [('Arc ASCII Grids', '.asc'), ('All files', '.*')])

    # if a file is chosen, separate the directory and name
    if inBsmt != "":
        # separate instructions for Linux and Windows
        if os.name == 'nt':
            # split up the string to extract directory and name
            bsmtSplit = inBsmt.split ("/")
            inBsmt_dir = "/".join (bsmtSplit [0:(len (bsmtSplit) - 1)])
            inBsmt_name = bsmtSplit [len (bsmtSplit) - 1]
            
        if os.name == 'posix':
            # split up the string to extract directory and name
            bsmtSplit = inBsmt.split ("/")
            inBsmt_dir = "/" + "/".join (bsmtSplit [0:(len (bsmtSplit) - 1)])
            inBsmt_name = bsmtSplit [len (bsmtSplit) - 1]
            
        status.config(text = "Input basement file set: " + inBsmt)
        p9_2.config (text = inBsmt_name)
        configWrite()
        
    # else, the user selected 'Cancel', print error message on status bar
    else:
        status.config(text =  "ERROR: Failed to set input basement file")
        p9_2.config (text = "Choose file ...")
        
    return

# ----------------------------------------------------------------------------------
# Set output: surface file
def setOutSurf():
    # set an output location for a surface file
    global outSurf_dir, outSurf
    # open a dialog box to choose the location
    outSurf = tkFileDialog.asksaveasfilename (initialdir = outSurf_dir, initialfile = "surf_GIS.asc",
                parent = root, title = "Save output surface file", defaultextension = ".asc")  

    # if a file is chosen, separate the directory and name
    if outSurf != "":
        # separate instructions for Linux and Windows
        if os.name == 'nt':
            # split up the outSurf string to open dialog in correct directory
            surfSplit = outSurf.split ("/")
            outSurf_dir = "/".join (surfSplit [0:(len (surfSplit) - 1)])
            status.config (text = "Output surface file set: " + outSurf)
                               
        if os.name == 'posix':
            # split up the outSurf string to open dialog in correct directory
            surfSplit = outSurf.split ("/")
            outSurf_dir = "/" + "/".join (surfSplit [0:(len (surfSplit) - 1)])
            status.config (text = "Output surface file set: " + outSurf)
        configWrite()
        
    # else, the user selected 'Cancel', print an error message on the status bar
    else:
        status.config (text = "ERROR: Failed to set output surface file")
        
    return

# ----------------------------------------------------------------------------------
# Set output: basement file
def setOutBsmt():
    # set an output location for a basement file
    global outBsmt_dir, outBsmt
    # open a dialog box to choose the location
    outBsmt = tkFileDialog.asksaveasfilename (initialdir = outBsmt_dir, initialfile = "bsmt_GIS.asc",
                parent = root, title = "Save output basement file", defaultextension = ".asc")  

    # if a file is chosen, separate the directory and name
    if outBsmt != "":
        # separate instructions for Linux and Windows
        if os.name == 'nt':
            # split up the outSurf string to open dialog in correct directory
            bsmtSplit = outBsmt.split ("/")
            outBsmt_dir = "/".join (bsmtSplit [0:(len (bsmtSplit) - 1)])     # re-join, without the filename
            status.config (text = "Output basement file set: " + outSurf)
                    
        if os.name == 'posix':
            # split up the outSurf string to open dialog in correct directory
            bsmtSplit = outBsmt.split ("/")
            outBsmt_dir = "/" + "/".join (bsmtSplit [0:(len (bsmtSplit) - 1)])     # re-join, without the filename
            status.config (text = "Output basement file set: " + outSurf)
        configWrite()
        
    # else, the user selected 'Cancel', print an error message on the status bar
    else:
        status.config (text = "ERROR: Failed to set output basement file")
        
    return

# ----------------------------------------------------------------------------------
# View the license
def viewLicense():
    # make a system call to view the license, separate instructions for Windows and Linux
    if os.name == 'nt':
        try:
            os.system("license.txt")
        except:
            status.config (text = "ERROR: Unable to automatically open file, open 'license.txt'")
            
    if os.name == 'posix':
        try:
            os.system("gedit ./license.txt")
        except:
            status.config (text = "ERROR: Unable to automatically open file, open 'license.txt'")
            
    return

# ----------------------------------------------------------------------------------
# View the help file
def viewHelp():
    # make a system call to view the help file, separate instructions for Windows and Linux
    if os.name == 'nt':
        try:
            os.system("wdune_help.pdf")
        except:
            status.config (text = "ERROR: Unable to automatically open file, open 'wdune_help.pdf'")
            
    if os.name == 'posix':
        try:
            os.system("evince ./wdune_help.pdf")
        except:
            status.config (text = "ERROR: Unable to automatically open file, open 'wdune_help.pdf'")
            
    return

# ----------------------------------------------------------------------------------
# Run the model
def runModel():
    # run a series of input checks before calling the model core
    global inSurf, inBsmt, outSurf, outBsmt, outSurf_dir, outBsmt_dir, bounds_NS, bounds_EW, version
    status.config (text = "Checking inputs ...")

    # if 'new inputs' are chosen, make some new input files
    if in_type.get() == "N":
        # get the inputs and try to parse to integers
        try:
            nrows = int ( p4_2.get() )
            ncols = int ( p5_2.get() )
            slabs = int ( p6_2.get() )
        except:
            status.config (text = "ERROR: Check inputs for new files ...")
            return

        # check the inputs for acceptable ranges
        if nrows < 1 or ncols < 1:
            status.config (text = "ERROR: model space cannot be smaller than 1 m")
            return
        if nrows >= 5000 or ncols >= 5000:
            status.config (text = "ERROR: model space cannot be larger than 5000 m")
            return
        if slabs < 0:
            status.config (text = "ERROR: number of slabs cannot be smaller than 0")
            return
        if slabs > 5000:
            status.config (text = "ERROR: number of slabs cannot be larger than 5000")
            return
            
        # create the surface and basement files locally in preparation for model ingestion
        surf_file = open ("surf.txt", "w")
        for i in range (0, nrows):
            for j in range (0, (ncols - 1)):
                surf_file.write (str(slabs) + " ")          # write the contents of the file
            surf_file.write (str(slabs) + "\n")             # write the end line character
        surf_file.close()

        bsmt_file = open ("bsmt.txt", "w")
        for i in range (0, nrows):
            for j in range (0, (ncols - 1)):
                bsmt_file.write ("0 ")                      # write the contents of the file
            bsmt_file.write ("0\n")                         # write the end line character
        bsmt_file.close()

    # if 'existing inputs' are chosen, prepare the existing files for ingestion
    if in_type.get() == "E":
        # try to deasciify the input files
        try:
            deasciifier (inSurf, "surf.txt")
        except:
            status.config (text = "ERROR: Unable to read input surface file ...")
            return
        try:
            deasciifier (inBsmt, "bsmt.txt")
        except:
            status.config (text = "ERROR: Unable to read input basement file ...")
            return

        # next, read the surface file to get the number of rows and columns
        newf = open ("surf.txt", "r")
        ncols = len ( (newf.readline()).split (" ") )
        nrows = 1
        while (len ( (newf.readline()).split (" ") ) == ncols):
            nrows = nrows + 1
        newf.close()

        # read the basement file to check that both are identical
        newf = open ("bsmt.txt", "r")
        ncols_bsmt = len ( (newf.readline()).split (" ") )
        nrows_bsmt = 1
        while (len ( (newf.readline()).split (" ") ) == ncols):
            nrows_bsmt = nrows_bsmt + 1
        newf.close()

        # check that the surface and basement files are identical sized
        if ncols != ncols_bsmt or nrows != nrows_bsmt:
            status.config (text = "ERROR: input files are invalid")
            return

        # further validation of input files could be added in future

    # get model inputs
    # get the radiobutton inputs first, as they do not require 'try-except' wrapping
    version = mode.get()        # get the chosen mode
    wind_dir = wdir.get()
    sed_Source = sedSource.get()
    sed_side = sedSide.get()
    newslabs = 0                # default set of parameter
    # try to get entry inputs
    try:
        numIterations = int ( p11_2.get() )
        depjump = int ( p13_2.get() )
        psand = float ( p14_2.get() )
        pnosand = float ( p15_2.get() )
        shadangle = float ( p16_2.get() )
    except:
        status.config (text = "ERROR: Model parameters are invalid or missing ...")
        return

    # check the inputs to ensure the numbers are within realistic ranges
    if depjump < 0:
        status.config (text = "ERROR: deposition jump must be positive")
        return
    if (wind_dir == "1" or wind_dir == "2") and depjump > nrows:
        status.config (text = "ERROR: deposition jump must be less than the size of the model space")
        return
    if (wind_dir == "3" or wind_dir == "4") and depjump > ncols:
        status.config (text = "ERROR: deposition jump must be less than the size of the model space")
        return
    if psand < 0.0 or psand > 1.0:
        status.config (text = "ERROR: probability of depositing on sand must be between 0.0 and 1.0")
        return
    if pnosand < 0.0 or pnosand > 1.0:
        status.config (text = "ERROR: probability of depositing on no sand must be between 0.0 and 1.0")
        return
    if shadangle < 0.0 or shadangle > 89.9:
        status.config (text = "ERROR: shadow zone angle must be between 0.0 and 89.9 degrees")
        return
    if numIterations < 10:
        status.config (text = "ERROR: number of iterations must be greater than 10")
        return

    # prepare the inputs for calling the model core
    # boundary codes are an integer code, as follows:
    if bounds_NS == 0 and bounds_EW == 0: boundCode = 1
    if bounds_NS == 1 and bounds_EW == 1: boundCode = 2
    if bounds_NS == 1 and bounds_EW == 0: boundCode = 3
    if bounds_NS == 0 and bounds_EW == 1: boundCode = 4

    # construct sediment source codes and get the source rates, only if new sediment is supplied
    if sed_Source != "0":
        sed_Source = int (sed_Source) + int (sed_side)      # construct sediment source code
        # try to get the sediment source rate
        try:
            newslabs = int ( p21_2.get() )
        except:
            status.config (text = "ERROR: number of new slabs is invalid")
            return
        # check the input
        if newslabs < 0:
            status.config (text = "ERROR: number of new slabs must be positive")
            return

        
    # calculate shadow drop drop distance (in slab heights)
    dropdist = math.tan (math.pi * (shadangle / 180.0)) * 10.0
    
    # prepare callstring for calling model core
    if os.name == 'nt': callString = "wdune_core.exe"
    if os.name == 'posix': callString = "./wdune_core_linux.exe"

    # add the rest of the arguments to the callstring
    callString = (callString + " " + str(numIterations) + " " + str(wind_dir) + " " +
                  str(depjump) + " " + str(psand) +  " " + str(pnosand) + " " +
                  str(dropdist) + " " + str(nrows) + " " + str(ncols) + " " +
                  str(boundCode) + " " + str(sed_Source) + " " + str(newslabs) )

    # print calling message
    status.config (text = "Calling model core ...")

    # try to call the model core
    try:
        os.system (callString)
    except:
        status.config (text = "ERROR: Unable to call model core ...")
        return

    # prompt the user to save the output files
    setOutSurf()
    setOutBsmt()
    
    # print error when user clicks 'Cancel' instead of selecting a location for output
    if outSurf == "" or outBsmt == "":
        status.config (text = "ERROR: Unable to save model outputs ...")

    # else, asciify the output files
    else:
        asciifier ("surf.txt", outSurf)
        asciifier ("bsmt.txt", outBsmt)
        status.config (text = "Model run successfull ... attempting ArcGIS finishing")
     
        # call the ArcGIS finishing function
        try:
            ArcGISfinish (outSurf, outBsmt, outSurf_dir, outBsmt_dir, version)
            status.config (text = "Model run successfull!")
        except:
            status.config (text = "Model run successfull!   ArcGIS finishing unsuccessful!")

    # if this script is called from ArcGIS launcher, close the window
    if os.path.basename (sys.argv[0]) == "wdune_GUI.py":
        exitProgram()
    return

# ----------------------------------------------------------------------------------
# Exit program
def exitProgram():
    # exit the program by destroying the root mainloop and main function
    root.destroy()
    root.quit()
    return

# ----------------------------------------------------------------------------------
# Toggle switching of variables when user clicks on checkboxes
def setBounds_NS():
    # trigger checkbox switching when user clicks on checkbox
    global bounds_NS
    if bounds_NS == 1: bounds_NS = 0
    else: bounds_NS = 1
    return

# ----------------------------------------------------------------------------------
def setBounds_EW():
    # trigger checkbox switching when user clicks on checkbox
    global bounds_EW
    if bounds_EW == 1: bounds_EW = 0
    else: bounds_EW = 1
    return

# ----------------------------------------------------------------------------------
def greyNew():
    # trigger greying out of the 'New file' inputs
    p4_2.config (state = DISABLED)
    p5_2.config (state = DISABLED)
    p6_2.config (state = DISABLED)
    p8_2.config (state = NORMAL)
    p9_2.config (state = NORMAL)
    return

# ----------------------------------------------------------------------------------
def greyExisting():
    # trigger greying out of the 'Existing file' inputs
    p4_2.config (state = NORMAL)
    p5_2.config (state = NORMAL)
    p6_2.config (state = NORMAL)
    p8_2.config (state = DISABLED)
    p9_2.config (state = DISABLED)
    return

# ----------------------------------------------------------------------------------
def greyNewSlabOptions():
    # trigger greying out of the 'New slab' inputs
    p20_2_1.config (state = DISABLED)
    p20_2_2.config (state = DISABLED)
    p20_2_3.config (state = DISABLED)
    p20_2_4.config (state = DISABLED)
    p21_2.config (state = DISABLED)
    return

# ----------------------------------------------------------------------------------
def ungreyNewSlabOptions():
    # trigger greying out of the 'Existing file' inputs
    p20_2_1.config (state = NORMAL)
    p20_2_2.config (state = NORMAL)
    p20_2_3.config (state = NORMAL)
    p20_2_4.config (state = NORMAL)
    p21_2.config (state = NORMAL)
    return

# ----------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------
# Main graphical user interface design
root = Tk()
root.title("WDUNE Control panel")
root.focus_force()      # force focus to GUI window (necessary when called from launcher)

# main frame: all widgets are gridded in this frame
parFrame = Frame (root)
parFrame.pack()

# user interface widgets
p2 = Label (parFrame, text = "Choose type of inputs:")
p2_2 = Frame (parFrame)

# new/existing radiobutton setup
in_type = StringVar()
p2_2_1 = Radiobutton (p2_2, text = "New", fg = "red", variable = in_type, value = "N", command = greyExisting)
p2_2_2 = Radiobutton (p2_2, text = "Existing", fg = "blue", variable = in_type, value = "E", command = greyNew)
p2_2_1.pack (side = LEFT)
p2_2_2.pack (side = LEFT)

p3 = Label (parFrame, text = "NEW INPUTS:", fg = "red")
p4 = Label (parFrame, text = "North - South size of model space:")
p4_2 = Entry (parFrame, width = 20)
p5 = Label (parFrame, text = "East - West size of model space:")
p5_2 = Entry (parFrame, width = 20)
p6 = Label (parFrame, text = "Depth of slabs in model space:")
p6_2 = Entry (parFrame, width = 20)
p7 = Label (parFrame, text = "EXISTING INPUTS:", fg = "blue")
p8 = Label (parFrame, text = "Choose existing surface file:")
p8_2 = Button (parFrame, width = 20, text = "Choose file ...", state = DISABLED, command = setFileSurf)
p9 = Label (parFrame, text = "Choose existing basement file:")
p9_2 = Button (parFrame, width = 20, text = "Choose file ...", state = DISABLED, command = setFileBsmt)

p10 = Label (parFrame, text = "MODEL PARAMETERS:", fg = "#228B22")
p11 = Label (parFrame, text = "Number of iterations:")
p11_2 = Entry (parFrame, width = 20)
p12 = Label (parFrame, text = "Wind direction:")
p12_2 = Frame (parFrame)

# wind direction radiobutton setup
wdir = StringVar()
p12_2_1 = Radiobutton (p12_2, text = "N", variable = wdir, value = "1")
p12_2_2 = Radiobutton (p12_2, text = "S", variable = wdir, value = "2")
p12_2_3 = Radiobutton (p12_2, text = "E", variable = wdir, value = "3")
p12_2_4 = Radiobutton (p12_2, text = "W", variable = wdir, value = "4")
p12_2_1.pack (side = LEFT)
p12_2_2.pack (side = LEFT)
p12_2_3.pack (side = LEFT)
p12_2_4.pack (side = LEFT)

p13 = Label (parFrame, text = "Deposition jump:")
p13_2 = Entry (parFrame, width = 20)
p14 = Label (parFrame, text = "Probability of depositing on sand:")
p14_2 = Entry (parFrame, width = 20)
p15 = Label (parFrame, text = "Probability of depositing on basement:")
p15_2 = Entry (parFrame, width = 20)
p16 = Label (parFrame, text = "Shadow zone angle (from horizontal):")
p16_2 = Entry (parFrame, width = 20)
p17 = Label (parFrame, text = "Periodic boundaries:")
p17_2 = Checkbutton (parFrame, text = "North - South", command = setBounds_NS)
p18_2 = Checkbutton (parFrame, text = "East - West", command = setBounds_EW)
p19 = Label (parFrame, text = "New slab source:")
p19_2 = Frame (parFrame)

# sediment source radiobutton setup
sedSource = StringVar()
p19_2_1 = Radiobutton (p19_2, text = "None", variable = sedSource, value = "0", command = greyNewSlabOptions)
p19_2_2 = Radiobutton (p19_2, text = "Edge", variable = sedSource, value = "20", command = ungreyNewSlabOptions)
p19_2_3 = Radiobutton (p19_2, text = "Point", variable = sedSource, value = "10", command = ungreyNewSlabOptions)
p19_2_1.pack (side = LEFT)
p19_2_2.pack (side = LEFT)
p19_2_3.pack (side = LEFT)

p20 = Label (parFrame, text = "Side to add slabs:")
p20_2 = Frame (parFrame)

# sediment side radiobutton setup
sedSide = StringVar()
p20_2_1 = Radiobutton (p20_2, text = "N", variable = sedSide, value = "1", state = DISABLED)
p20_2_2 = Radiobutton (p20_2, text = "S", variable = sedSide, value = "2", state = DISABLED)
p20_2_3 = Radiobutton (p20_2, text = "E", variable = sedSide, value = "3", state = DISABLED)
p20_2_4 = Radiobutton (p20_2, text = "W", variable = sedSide, value = "4", state = DISABLED)
p20_2_1.pack (side = LEFT)
p20_2_2.pack (side = LEFT)
p20_2_3.pack (side = LEFT)
p20_2_4.pack (side = LEFT)

p21 = Label (parFrame, text = "Number of slabs to add per iteration:")
p21_2 = Entry (parFrame, width = 20, state = DISABLED)

# select ArcGIS finishing
p22 = Label (parFrame, text = "Prepare files for viewing in ArcGIS\nchoose a version:", justify = LEFT)
p22_2 = Frame (parFrame)

# mode choice radiobutton setup
mode = StringVar()
p22_2_1 = Radiobutton (p22_2, text = "No ArcGIS preparation", variable = mode, value = "S")
p22_2_2 = Radiobutton (p22_2, text = "9.2 or 9.3", variable = mode, value = "9.2")
p22_2_3 = Radiobutton (p22_2, text = "10.0", variable = mode, value = "10.0")
p22_2_1.grid (row = 0, column = 0, sticky = W, columnspan = 2)
p22_2_2.grid (row = 1, column = 0, sticky = W)
p22_2_3.grid (row = 1, column = 1, sticky = W)

# bottom buttons
p24 = Button (parFrame, width = 20, text = "View help file ...", command = viewHelp)
p24_2 = Button (parFrame, width = 20, text = "RUN MODEL!", command = runModel)
p25 = Button (parFrame, width = 20, text = "View license ...", command = viewLicense)
p25_2 = Button (parFrame, width = 20, text = "EXIT", command = exitProgram)

# pack the parFrame grid with the widgets
p2.grid         (row = 1, column = 0, sticky = W, padx = 2, pady = 2)
p2_2.grid       (row = 1, column = 1, sticky = W, padx = 2, pady = 2)
p3.grid         (row = 2, column = 0, sticky = W, padx = 2, pady = 2)
p4.grid         (row = 3, column = 0, sticky = W, padx = 2, pady = 2)
p4_2.grid       (row = 3, column = 1, sticky = W, padx = 2, pady = 2)
p5.grid         (row = 4, column = 0, sticky = W, padx = 2, pady = 2)
p5_2.grid       (row = 4, column = 1, sticky = W, padx = 2, pady = 2)
p6.grid         (row = 5, column = 0, sticky = W, padx = 2, pady = 2)
p6_2.grid       (row = 5, column = 1, sticky = W, padx = 2, pady = 2)
p7.grid         (row = 6, column = 0, sticky = W, padx = 2, pady = 2)
p8.grid         (row = 7, column = 0, sticky = W, padx = 2, pady = 2)
p8_2.grid       (row = 7, column = 1, sticky = W, padx = 2, pady = 2)
p9.grid         (row = 8, column = 0, sticky = W, padx = 2, pady = 2)
p9_2.grid       (row = 8, column = 1, sticky = W, padx = 2, pady = 2)

p10.grid        (row = 9, column = 0, sticky = W, padx = 2, pady = 2)
p11.grid        (row = 10, column = 0, sticky = W, padx = 2, pady = 2)
p11_2.grid      (row = 10, column = 1, sticky = W, padx = 2, pady = 2)
p12.grid        (row = 11, column = 0, sticky = W, padx = 2, pady = 2)
p12_2.grid      (row = 11, column = 1, sticky = W, padx = 2, pady = 2)
p13.grid        (row = 12, column = 0, sticky = W, padx = 2, pady = 2)
p13_2.grid      (row = 12, column = 1, sticky = W, padx = 2, pady = 2)
p14.grid        (row = 13, column = 0, sticky = W, padx = 2, pady = 2)
p14_2.grid      (row = 13, column = 1, sticky = W, padx = 2, pady = 2)
p15.grid        (row = 14, column = 0, sticky = W, padx = 2, pady = 2)
p15_2.grid      (row = 14, column = 1, sticky = W, padx = 2, pady = 2)
p16.grid        (row = 15, column = 0, sticky = W, padx = 2, pady = 2)
p16_2.grid      (row = 15, column = 1, sticky = W, padx = 2, pady = 2)
p17.grid        (row = 16, column = 0, sticky = W, padx = 2, pady = 2)
p17_2.grid      (row = 16, column = 1, sticky = W, padx = 2, pady = 2)
p18_2.grid      (row = 17, column = 1, sticky = W, padx = 2, pady = 2)
p19.grid        (row = 18, column = 0, sticky = W, padx = 2, pady = 2)
p19_2.grid      (row = 18, column = 1, sticky = W, padx = 2, pady = 2)
p20.grid        (row = 19, column = 0, sticky = W, padx = 2, pady = 2)
p20_2.grid      (row = 19, column = 1, sticky = W, padx = 2, pady = 2)
p21.grid        (row = 20, column = 0, sticky = W, padx = 2, pady = 2)
p21_2.grid      (row = 20, column = 1, sticky = W, padx = 2, pady = 2)
p22.grid        (row = 21, column = 0, sticky = W, padx = 2, pady = 2)
p22_2.grid      (row = 21, column = 1, sticky = W, padx = 2, pady = 2)

p24.grid        (row = 23, column = 0, sticky = W, padx = 2, pady = 2)
p24_2.grid      (row = 23, column = 1, sticky = W, padx = 2, pady = 2)
p25.grid        (row = 24, column = 0, sticky = W, padx = 2, pady = 2)
p25_2.grid      (row = 24, column = 1, sticky = W, padx = 2, pady = 2)

# set the default values
# set the mode based on existing configuration file, pre-select the radiobutton
if version == "S":
    p22_2_1.select()
if version == "9.2":
    p22_2_2.select()
if version == "9.3":
    p22_2_2.select()
if version == "10.0":
    p22_2_3.select()

# insert / select default values
p2_2_1.select()
p4_2.insert (0, "100")
p5_2.insert (0, "100")
p6_2.insert (0, "30")
p11_2.insert (0, "500")
p12_2_4.select()
p13_2.insert (0, "1")
p14_2.insert (0, "0.6")
p15_2.insert (0, "0.4")
p16_2.insert (0, "15")
p17_2.select()
p18_2.select()
p19_2_1.select()
p20_2_1.select()
p21_2.insert (0, "0")

# set status bar on the bottom of the window
status = Label (root, text = status_oput , bd = 1, relief = SUNKEN, anchor = W, width = 60)
status.pack (side = BOTTOM, fill = X)

# invoke GUI by calling mainloop
root.mainloop()


