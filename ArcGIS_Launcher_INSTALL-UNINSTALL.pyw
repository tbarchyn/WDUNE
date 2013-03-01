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

# This script installs and uninstalls the ArcGIS Launcher
# Import modules
import os, sys, shutil, time

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

# set working directory to script location
os.chdir (sys.path[0])
global oput, version, inSurf_dir, inBsmt_dir, outSurf_dir, outBsmt_dir
oput = "Ready ..."

# Set defaults for writing configuration file (note, this program will only ever work for Windows)
outSurf_dir = "C://"
outBsmt_dir = "C://"
inSurf_dir = "C://"
inBsmt_dir = "C://"

# FUNCTIONS
# -------------------------------------------------------------------------------------------------
# Configuration file writer
def configWrite(version):
    # write a configuration file locally, to store the chosen mode for next time the model runs
    global outSurf_dir, outBsmt_dir, inSurf_dir, inBsmt_dir
    confile = open ("C:\\wdune\\core\\config.txt", 'w')
    confile.write (version + "\n")
    confile.write (outSurf_dir + "\n")
    confile.write (outBsmt_dir + "\n")
    confile.write (inSurf_dir + "\n")
    confile.write (inBsmt_dir + "\n")
    confile.close()
    return

# -------------------------------------------------------------------------------------------------
# Install for ArcGIS version 9.2
def set92():
    global oput
    oput = "Installing ..."
    status.config (text = oput)

    # copy the folder over
    try:
        os.mkdir ("C:\\wdune")
        shutil.copytree ("core", "C:\\wdune\\core")
        oput = oput + "\nSUCCESS: copied model core to C:/wdune"
        status.config (text = oput)
        step_1 = 1
    except:
        oput = oput + "\nERROR: Copying files failed, please refer to manual ..."
        status.config (text = oput)
        step_1 = 0

    try:
        # write the configuration file to the version
        configWrite("9.2")
        oput = oput + "\nSUCCESS: configuration file written for ArcGIS 9.2"
        status.config (text = oput)
        step_2 = 1
    except:
        step_2 = 0
        oput = oput + "\nERROR: unable to write configuration file"
        status.config (text = oput)

    # if successful, print messages and exit
    if step_1 + step_2 == 2:
        tkMessageBox.showinfo (title = "Successful installation!",
            message = "Next steps:\n   1) Open ArcGIS" +
                               "\n   2) Open ArcToolbox" +
                               "\n   3) Right click on the toolbox window and click 'Add Toolbox ...'" +
                               "\n   4) Add: 'C:/wdune/core/WDUNE_toolbox_92_93.tbx'" +
                               "\n   5) Right click again and click 'Save settings' -> 'To Default'"
                               )
        exitProgram()
        
    else:
        tkMessageBox.showwarning (title = "Installation failure",
                               message = "Installation failure ... refer to manual to install manually" )
            
    return

# -------------------------------------------------------------------------------------------------
# Install for ArcGIS version 9.3
def set93():
    global oput
    oput = "Installing ..."
    status.config (text = oput)

    # copy the folder over
    try:
        os.mkdir ("C:\\wdune")
        shutil.copytree ("core", "C:\\wdune\\core")
        oput = oput + "\nSUCCESS: copied model core to C:/wdune"
        status.config (text = oput)
        step_1 = 1
    except:
        oput = oput + "\nERROR: Copying files failed, please refer to manual ..."
        status.config (text = oput)
        step_1 = 0

    try:
        # write the configuration file to the version
        configWrite("9.3")
        oput = oput + "\nSUCCESS: configuration file written for ArcGIS 9.3"
        status.config (text = oput)
        step_2 = 1
    except:
        step_2 = 0
        oput = oput + "\nERROR: unable to write configuration file"
        status.config (text = oput)
        
    # if successful, print messages and exit    
    if step_1 + step_2 == 2:
        tkMessageBox.showinfo (title = "Successful installation!",
            message = "Next steps:\n   1) Open ArcGIS" +
                               "\n   2) Open ArcToolbox" +
                               "\n   3) Right click on the toolbox window and click 'Add Toolbox ...'" +
                               "\n   4) Add: 'C:/wdune/core/WDUNE_toolbox_92_93.tbx'" +
                               "\n   5) Right click again and click 'Save settings' -> 'To Default'"
                               )
        exitProgram()
        
    else:
        tkMessageBox.showwarning (title = "Installation failure",
                               message = "Installation failure ... refer to manual to install manually" )
            
    return

# -------------------------------------------------------------------------------------------------
# Install for ArcGIS version 10.0
def set10():
    global oput
    oput = "Installing ..."
    status.config (text = oput)

    # copy the folder over
    try:
        os.mkdir ("C:\\wdune")
        shutil.copytree ("core", "C:\\wdune\\core")
        oput = oput + "\nSUCCESS: copied model core to C:/wdune"
        status.config (text = oput)
        step_1 = 1
    except:
        oput = oput + "\nERROR: Copying files failed, please refer to manual ..."
        status.config (text = oput)
        step_1 = 0
    try:
        # In ArcGIS 10 it is possible to simply paste the toolbox into the folder
        shutil.copy2 (os.path.join (os.getcwd(), "core", "WDUNE_toolbox_10.tbx"),
                    "C:\\Documents and Settings\\" + os.getenv ('USERNAME') +
                    "\\Application Data\\ESRI\\Desktop10.0\\ArcToolbox\\My Toolboxes\\WDUNE_toolbox.tbx")

        oput = oput + "\nSUCCESS: toolbox installation for ArcGIS 10.0"
        status.config (text = oput)
        step_2 = 1
        
    except:
        oput = oput + "\nERROR: unable to install toolbox ... view help file"
        status.config (text = oput)
        step_2 = 0
    try:
        # write the configuration file to the version
        configWrite("10.0")
        oput = oput + "\nSUCCESS: configuration file written for ArcGIS 10.0"
        status.config (text = oput)
        step_3 = 1
    except:
        step_3 = 0
        oput = oput + "\nERROR: unable to write configuration file"
        status.config (text = oput)

    # if successful, print messages and exit
    if step_1 + step_2 + step_3 == 3:
        tkMessageBox.showinfo (title = "Successful installation!",
                               message = "Successful installation, close this program" )
        exitProgram()
        
    else:
        tkMessageBox.showwarning (title = "Installation failure",
                               message = "Installation failure ... refer to manual" )
    return


# -------------------------------------------------------------------------------------------------
# Uninstall for all versions of ArcGIS
def uninstall():
    global oput
    oput = "Uninstalling ..."
    status.config (text  = oput)
    # delete toolboxes and folders
    try:
        shutil.rmtree ("C:\\wdune")
        oput = oput + "\nSUCCESS: Deleted model core"
        status.config (text = oput)
        step_1 = 1
    except:
        oput = oput + "\nERROR: Unable to delete model core ... try deleting 'C:/wdune' manually"
        status.config (text = oput)
        step_1 = 0

    # set toolbox uninstallation flags
    step_2 = 0
    
    try:
        # try to delete the .dat file that is saved for user specified toolboxes
        os.unlink ("C:\\Documents and Settings\\" + os.getenv ('USERNAME') + "\\Application Data\\ESRI\\ArcToolbox\\ArcToolbox.dat")
        step_2 = 1
    except:
        pass
    try:
        # try to delete the .dat file that is saved for user specified toolboxes
        os.unlink ("C:\\Users\\" + os.getenv ('USERNAME') + "\\AppData\\Roaming\\ESRI\\ArcToolbox\\ArcToolbox.dat")
        step_2 = 1
    except:
        pass

    try:
        # in ArcGIS 10.0, similar uninstall, just delete the toolbox    
        os.unlink ("C:\\Documents and Settings\\" + os.getenv ('USERNAME') + "\\Application Data\\ESRI\\Desktop10.0\\ArcToolbox\\My Toolboxes\\WDUNE_toolbox.tbx")
        step_2 = 1
    except:
        pass
    
    # print success message
    if step_2 == 1:
        oput = oput + "\nSUCCESS: Deleted ArcGIS toolbox"
        status.config (text = oput)
    else:
        oput = oput + "\nERROR: unable to uninstall ArcGIS toolbox ... please refer to manual"
        status.config (text = oput)
    
    if step_1 + step_2 == 2:
        tkMessageBox.showinfo (title = "Successful uninstallation of launcher!",
                               message = "Successful uninstallation of launcher, press OK to close this program" )
        exitProgram()
        
    else:
        tkMessageBox.showwarning (title = "Uninstallation failure",
                               message = "Uninstallation failure ... refer to manual" )

    return

# -------------------------------------------------------------------------------------------------
# Exit program
def exitProgram():
    root.destroy()
    root.quit()
    return

# -------------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------------
# GRAPHICAL USER INTERFACE DESIGN
root = Tk()
root.title ("INSTALL - UNINSTALL WDUNE ArcGIS control panel launcher")
root.focus_force()      # force focus to GUI

# print instructions
instructions = Label (root, text = "This program installs the ArcGIS control panel launcher for the WDUNE model\n" +
                      "Please review the license prior to using the program ('license.txt')\n\n" +
                      "Installation consists of:\n" +
                      "  1) Copying the 'wdune' folder to 'C:/wdune'\n" +
                      "  2) Copying the ArcGIS toolbox to the 'My Toolboxes' folder\n",
                      justify = LEFT)

# set frames for each row of buttons
buttons1 = Frame (root)
buttons2 = Frame (root)

b92_button = Button (buttons1, text = "Install for ArcGIS 9.2", width = 20, command = set92)
b93_button = Button (buttons1, text = "Install for ArcGIS 9.3", width = 20, command = set93)
b10_button = Button (buttons1, text = "Install for ArcGIS 10.0", width = 20, command = set10)

uninstall_button = Button (buttons2, text = "Uninstall launcher", command = uninstall, width = 31)
exit_button = Button (buttons2, text = "Exit", command = exitProgram, width = 31)

b92_button.pack (side = LEFT, padx = 2, pady = 2)
b93_button.pack (side = LEFT, padx = 2, pady = 2)
b10_button.pack (side = LEFT, padx = 2, pady = 2)

uninstall_button.pack(side = LEFT, padx = 2, pady = 2)
exit_button.pack(side = LEFT, padx = 2, pady = 2)

# put a status bar on the bottom of the window
status = Label (root, text = oput, bd = 1, relief = SUNKEN, anchor = N+W, justify = LEFT, height = 4)

# pack the GUI with the widget frames
instructions.pack (padx = 2, pady = 2)
buttons1.pack (padx = 2, pady = 2)
buttons2.pack (padx = 2, pady = 2)
status.pack (fill = X)

# main GUI loop
root.mainloop()


