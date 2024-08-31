# FluidSim
The goal of this project is to achieve a non-compressible Eulerian fluid simulation using the finite element method. 

Current state of the simulation using the VELOCITY display mode : 
![](resources/images/fluid2.png)


## What is done 
- implementing simulation of fluid for games on CPU
- making controls to interact with the simulation
- displaying simulation with 2 modes : VELOCITY MODE and DENSITY MODE

## To do
- Modifying bounds managment to simplify it and to easily add objects and obstacles (with a mask array)
- Simplify projection function on CPU and see if it works well
- passing everything on GPU using openGL to have better performance
- passing everything on GPU using Vulkan and compare the performance with the openGL simulation to si if Vulkan realy worth a try
