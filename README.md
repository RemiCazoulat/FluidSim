# FluidSim
The goal of this project is to achieve a non-compressible Eulerian fluid simulation using the finite element method. 

Current state of the simulation using the VELOCITY display mode : 
![](resources/images/fluid2.png)


## What is done 
- implementing simulation of fluid for games on CPU
- making controls to interact with the simulation
- displaying simulation with 2 modes : VELOCITY MODE and DENSITY MODE
- Modifying bounds management to simplify it and to easily add objects and obstacles (with a mask array)
- Simplify project function on CPU and see if it works well (it doesn't work well)

## To do
- passing everything on GPU using openGL to have better performance
- add a third dimension
- passing everything on GPU using Vulkan and compare the performance with the openGL simulation to see if Vulkan really worths a try
- make a vorticity confinement step to improve performance and accuracy
- make a Qt interface to have better interactions with the simulation (optional)
- make a sound input (optional)

## future of the project
- learn how to transform the fluid into plasma, and add magnets
- make a simulation of a rocket engine
- make a simulation of a nuclear fusion reactor
