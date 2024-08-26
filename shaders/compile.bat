@echo off
echo Compilazione degli shader GLSL...

REM Compila il primo file GLSL
glslc armChairVert.vert -o armchairVert.spv

REM Compila il secondo file GLSL
glslc C:\Users\sim.diienno1\Documents\Visual Studio 2022\ComputerGraphicsProject\ComputerGraphicsProject\shaders\facesRoomShader.vert -o facesRoomVert.spv

glslc C:\Users\sim.diienno1\Documents\Visual Studio 2022\ComputerGraphicsProject\ComputerGraphicsProject\shaders\bedVert.vert -o bedVert.spv

glslc C:\Users\sim.diienno1\Documents\Visual Studio 2022\ComputerGraphicsProject\ComputerGraphicsProject\shaders\armChairShader.frag -o armchairFrag.spv

glslc C:\Users\sim.diienno1\Documents\Visual Studio 2022\ComputerGraphicsProject\ComputerGraphicsProject\shaders\bedFrag.frag -o bedFrag.spv

glslc C:\Users\sim.diienno1\Documents\Visual Studio 2022\ComputerGraphicsProject\ComputerGraphicsProject\shaders\bottomFaceShader.frag -o bottomFrag.spv

glslc C:\Users\sim.diienno1\Documents\Visual Studio 2022\ComputerGraphicsProject\ComputerGraphicsProject\shaders\frontFaceShader.frag -o frontFaceFrag.spv

glslc C:\Users\sim.diienno1\Documents\Visual Studio 2022\ComputerGraphicsProject\ComputerGraphicsProject\shaders\leftRightFaceShader.frag -o leftRightFacesFrag.spv

glslc C:\Users\sim.diienno1\Documents\Visual Studio 2022\ComputerGraphicsProject\ComputerGraphicsProject\shaders\frontFaceShader.frag -o frontFragShader.spv

glslc C:\Users\sim.diienno1\Documents\Visual Studio 2022\ComputerGraphicsProject\ComputerGraphicsProject\shaders\kitchenShader.frag -o kitchenFrag.spv

glslc C:\Users\sim.diienno1\Documents\Visual Studio 2022\ComputerGraphicsProject\ComputerGraphicsProject\shaders\kitchenShader.vert -o kitchenVert.spv

glslc C:\Users\sim.diienno1\Documents\Visual Studio 2022\ComputerGraphicsProject\ComputerGraphicsProject\shaders\vaseShader.frag -o vaseFrag.spv

glslc C:\Users\sim.diienno1\Documents\Visual Studio 2022\ComputerGraphicsProject\ComputerGraphicsProject\shaders\vaseShader.vert -o vaseVert.spv


echo Compilazione completata.

