# Voxel
Learning how to create Voxel Terrain Generation using Perlin Noise

In this project I will begin to learn how terrain is generated in various procedurally generated games. I begun by following a tutorial that recreates a basic Minecraft terrain using Perlin Noise to Generate a Voxel Map representing the "Blocks" within a "Chunk" of the world.


## Chunk: 
This class represents a single "chunk" of data which is a collection of blocks of an arbitrary size that can be generated all at once.

  ### Generate Blocks:
  This function generates the actual blocks and populates the array based on the Perlin Noise Map. <br><br> To get the type of block you simply take the perlin noise which is simply a float value between -1 and 1. <br> then you add one to adjust the value between 0 and 2. <br> multiply by the size to set the maximum height leaving you with a value between 0 and 2 * Size. <br> then remove the pesky 2 by dividing by 2 resulting in a value we want between 0 and Size
  ### Generate Mesh:
  This function generates the meshes that you can see in the world, instead of generating all meshes and wasting computing power we only generate the "visible" sides, that is the sides with air next to them. <br><br> To do this we simply loop through all the blocks in the mesh and further loop through all the sides on those blocks. <br> Once we have each individual side we check the block touching that side with our Check Function. <br> If our Check passes we then create a face in that direction. <br> note we must multiply the position by 100 because Unreal Engine uses CM instead of M and we wish to use M in order to emulate minecraft.
  ### Remaining Functions
  - Apply Mesh: Apply the data we have created to the Mesh of the Chunk
  - Check: check if the position we are checking is outside the chunk or the block is an air block
  - Create Face: add all the data about vertexes, UV's, and Triangles to the respective arrays and increments the amount of verteces we have created.
  - Get Face Vertices: return the 4 vertices that correspond to the face we are passing in
  - Get Position In Direction: Get the Position of a block in a given direction, for instance if we have a block at (0,0,0) and we ask for the block above it, it will return (0,1,0)
  - Get Block Index: Convert the position of our block in 3D space to its index within a 1D array

## Chunk World:
this class represents our "world" which will generate all of the needed chunks.

### Begin Play
At the start of Game we will generate chunks starting at the origin and branching out DrawDistance chunks away forming a rectangle around the origin with corresponding chunks.
