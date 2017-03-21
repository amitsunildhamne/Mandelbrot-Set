# Mandelbrot-Set
Displaying Mandelbrot Set using OpenGL and POSIX Threads
The Mandelbrot set is the set of complex numbers c for which the function {f_{c}(z)=z^{2}+c} does not diverge when iterated from z=0, i.e., for which the sequence  { f_{c}(0)}, {f_{c}(f_{c}(0))}, etc., remains bounded in absolute value.
In my implementation of Mandelbrot Set if Z>|2|, we consider that pixel as black else, else we give it a different pixel. It should be noted that every unique magnitude of Z in the plane has been given a unique color.
In case the magnitude of 2 points Z1 and Z2 in the plane is same, the color assigned is also same.
To run this project:
1. make
2. ./MBSet
3. To zoom a particular area: select the region by pressing the mouse button
4. To zoom out: Press "b" from keyboard
