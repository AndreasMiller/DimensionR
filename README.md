DimensionR

This app is a visualization tool for an algorithm that generates fundamental geometry from an arbitrary number of vertices, 
with an arbitrary number of dimensions.

In the most basic configuration, there are two parameters that need to be specified:
    1) number of vertices
    2) number of dimensions

The algorithm will then create the vertices with random values, and iterate through them continuosly while updating their positions graphically 
on the screen.

The display will show their (x, y, z) 3D components, and will show higher dimensions by scaling vertex postions. The pseudo-visualization effect of 
higher dimensions can be adjusted with a slider control, and rotations in the 3D scene can be made by left-mouse-drag.

There is a "Cube Special Case" check box that allows for the visualization of the connected topology for cubes (8 vertices, 3 dimensions) and 
hyper-cubes (16 vertices, 4 dimensions)... (32 vertices, 5 dimensions)... etc.

The algorithm works like this:
  1) choose a random unit vector
  2) perform a dot-product calculation with the random unit vector against each entry in the list of vertices
  3) determine the largest dot-product value, and its corresponding vertex index
  4) a small fraction of the random unit vector is then added to the vertex at that index
  5) repeat steps 1) through 4), over and over again


*Written for Windows 11 using Visual Studio
