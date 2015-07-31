# Angle Correction



NOT UPDATED

----------------------------------------------------------------------------
Building
-----------------------------------------------------------------------------
The source code is organized in following folders:

core - which contains the Makefile and the main source file
core/lib - which contains the rest of the source code
core/test - which contains tests for the code

To build the program, you will first need the following packages (with development headers for the libraries:

libvtk
libeigen3
libsuperlu
g++-4.7
cmake
ninja (or make)

These can be installed on a Debian GNU/Linux machine by issuing (as root)
apt-get install libvtk5-dev libeigen3-dev libsuperlu3-dev g++-4.7 ninja-build cmake cmake-qt-gui

To build the program as a standalone algorithm:

git clone  git@github.com:SINTEFMedtek/AngleCorr.git angelCorr

mkdir -p build; cd build

cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DANGLECORRECTION_TEST_DATA_DIR=../testData/ ../angelCorr/core/

ninja


-----------------------------------------------------------------------------
Using
-----------------------------------------------------------------------------

Running the program angle_correction without any arguments gives a short help text:

$ ./angle_correction 

Usage: ./angle_correction centerline.vtk image_prefix Vnyq cutoff nConvolutions


As indicated, the program must be run with 6 parameters:

centerline.vtk 	 - the center line

image_prefix 	 - the prefix of the images. The suffix $NUMBER.mhd will be appended to it, 
		   where $NUMBER starts at 0 and progresses until no more files are found.

Vnyq 		 - Nyquist velocity used when acquiring the images

cutoff 		 - The lower cutoff value for abs(cos(theta)), used for the velocity estimation algorithms

nConvolutions 	 - the number of times to run the smoothAll algorithm

-----------------------------------------------------------------------------
Output
-----------------------------------------------------------------------------

An output file are generated.


output.vtk
---------------------------------
output.vtk is a file that may be visualized by ParaView, and contains the center line as a point set, with one point at every point it intersects an image. Each of those points are annotated with the following data:

The flow direction (normalized gradient of the center line curve)
The estimated flow velocity (simple estimate)
The estimated acquisition direction vector
