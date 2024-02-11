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

## GreedyChunk
this class still represents a single chunk, but instead of every single fact of a block having its own verteces and triangles, we use the Greedy Mesh Algorithm to combine faces of the same type that are next to each other

  ### Generate Mesh
  this function will be the workload of our greedy mesh algorithm, taking slices equal to each plane in a perpindicular axis, [X,Y,Z] Planes. <br> To start with we set our variables, the current Axis, the two other axis so we can loop through our plane and their respective limits <br> we also set deltaAxis which will store the width and height of our current greedy mesh, and two itterator over specific blocks and specific axis. <br> next we loop through all the actual blocks, going from -1 to limit because we want the block before the 1st mesh and after all other meshs giving us an additional block. <br> now that we have a specific 2d mesh plane we loop through its width and height to set our mask values based on whether the face should be drawn and if it should, which block and normal to draw it on. <br> Next we generate the mesh based on our mask we just created. <br> we itterate over the slice again starting at every position and ensure it has a normal. <br> at a given position we grow the width until we hit a different block or the limit: ![Animation of Greedy Mesh Grow](https://github.com/gilchristb78/Voxel/blob/main/ReadmeImages/greedy_anim.svg)
  > credit to https://gedge.ca/blog/2014-08-17-greedy-voxel-meshing/

<br> then we simply set our values, 0 out the normals for the blocks in our greedy mesh, and create the quads <br>
> note that our greedy meshing algorithm is not optimal as it always scales in the width first instead of considering both 
<br>

  ### Remaining Functions
  - Generate Blocks: works the same as Chunk
  - Apply Mesh: works the same as Chunk with certain variable types changed
  - Get Block: Index: works the same as chunk
  - Get Block: works the same as chunk
  - CompareMask: checks that the blocks and normals are the same
  - Create Quad: Adds the vertices, Triangles, UV values and normals to the data, note that the UV values in the 0 chanel currently do not scal themselves as width and height should be included.


  ### Result
  By implementing Greedy Meshing we effectively reduce our vertex count by approximately 90% going from roughly 20,000 vertices in the normal algorithm where each block has its own vertices and such down to roughly 2,000 vertices with the greedy mesh implementation. This wireframe shows the meshes combined to form less vertices.
  ![UE5 Wireframe View of Chunks Loaded](https://github.com/gilchristb78/Voxel/blob/main/ReadmeImages/CaptureVertexCount.PNG)


<br><br><br>

  > A big thank you to CodeBlaze on youtube for providing the tutorial to which I followed closely [Minecraft Like Voxel Terrain Generation by CodeBlaze](https://www.youtube.com/playlist?list=PLgji-9GMuqkK7EwUCVCuc2w3En1bPo5rA)
