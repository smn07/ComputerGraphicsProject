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

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\kitchenShader.frag -o kitchenFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\kitchenShader.vert -o kitchenVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\vaseShader.frag -o vaseFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\vaseShader.vert -o vaseVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\toiletShader.vert -o toiletVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\toiletShader.frag -o toiletFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\headphonesShader.frag -o headphonesFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\headphonesShader.vert -o headphonesVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\tvShader.frag -o tvFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\tvShader.vert -o tvVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\coffeeShader.frag -o coffeeFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\coffeeShader.vert -o coffeeVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\chairShader.frag -o chairFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\chairShader.vert -o chairVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\cameraShader.frag -o cameraFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\cameraShader.vert -o cameraVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\cursorFragShader.frag -o cursorFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\cursorVertShader.vert -o cursorVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\ballFrag.frag -o ballFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\ballVert.vert -o ballVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\microwaveFrag.frag -o microwaveFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\microwaveVert.vert -o microwaveVert.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\teaFrag.frag -o teaFrag.spv

glslc C:\Users\spagn\source\repos\ComputerGraphicsProject\shaders\teaVert.vert -o teaVert.spv






echo Compilazione completata.

