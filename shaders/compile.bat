@echo off
echo Compilazione degli shader GLSL...

REM Compila il primo file GLSL
glslc armChairVert.vert -o armchairVert.spv

REM Compila il secondo file GLSL
glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\facesRoomShader.vert -o facesRoomVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\bedVert.vert -o bedVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\armChairShader.frag -o armchairFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\bedFrag.frag -o bedFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\bottomFaceShader.frag -o bottomFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\frontFaceShader.frag -o frontFaceFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\leftRightFaceShader.frag -o leftRightFacesFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\frontFaceShader.frag -o frontFragShader.spv

echo Compilazione completata.

