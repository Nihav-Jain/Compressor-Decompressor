# Compressor-Decompressor
C based compressor / decompressor for a project at FIEA (Fall 2015)

## Problem statement

You will take in a file of Vertices formatted as ID: x y z u v Texture Name R G B

x,y,z can be floats, any precision

u,v,r,g,b will be between 0.0 and 1.0

Texture name will be a string that is less than 32 characters

0: 3.14 5.43 7.28 0.0 0.2 brick.jpg 1.0 1.0 1.0

1: 4.32 5.43 7.69 1.0 1.0 brick1.jpg 1.0 1.0 1.0

…

A link to the actual data input file is attached to this assignment.


Output a file of verts x,y,z as binary data

Your binary output will be a bitstream of vert data only, as well as any other info you need to read the file and reconstruct the input.

You will write a program that reads in the vert data and recreates the verts as well as you can. You will then compute your Root Mean Square error of the vert data. You will then create a graph in excel of the RMS error for bit values between 5 and 16.


In summary -

• Create a C program that writes out the binary vert data as a bit stream (as well as the stats into a separate file),bit size of each vert specified by a user argument.

• Write a second program that reads in the same file and creates a vert list.

• In the second program, compute the RMS error of the vert data.

• Run both programs for bit counts of 5-16 (12 total times) and plot the error on an excel chart.

• The whole process should be automated with a batch file that runs from the DOS command line. 

• At the end of the process, you should have 12 data files. Append the size of each data file onto the chart manually in the excel file, and include the size on the chart as well.


Turn in both programs, and the chart as an xls file. Also, in a readme, tell me if your lead engineer asked you how many bits we should be compressing the data to, what would your answer be?

FAQ

Do we need to specify the bits in X, Y, and Z separately? No. One bit size for all data.

Do we have to use every bit in the file, or can we pad? No padding allowed. Make use of every bit in the stream.

How do we compute root mean square error? Look it up.

What type of data do we put in the output data to help us reconstruct the data? Be creative, but remember the whole point of this tool is to have the file be as small as possible so try to have as little info as possible to do a great job reconstructing when given adequate bits. As a general rule, your “extra” data should be FAR, FAR, FAR less than the actual data stream – think minimally.

What pieces of the vert data do we need to store? x, y, z

So we don’t do anything with the u,v,r,g,b,texture data? Correct

Do we have to create a mechanism to get at the original values to compute the RMS error when unpacking the bitstream? Yes
