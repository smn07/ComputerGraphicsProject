@echo off
echo Compilazione degli shader GLSL...

REM Compila il primo file GLSL
glslc armChairVert.vert -o armchairVert.spv

REM Compila il secondo file GLSL
glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\facesRoomShader.vert -o facesRoomVert.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\bedVert.vert -o bedVert.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\armChairShader.frag -o armchairFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\bedFrag.frag -o bedFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\bottomFaceShader.frag -o bottomFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\frontFaceShader.frag -o frontFaceFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\leftRightFaceShader.frag -o leftRightFacesFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\frontFaceShader.frag -o frontFragShader.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\kitchenShader.frag -o kitchenFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\kitchenShader.vert -o kitchenVert.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\vaseShader.frag -o vaseFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\vaseShader.vert -o vaseVert.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\toiletShader.vert -o toiletVert.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\toiletShader.frag -o toiletFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\headphonesShader.frag -o headphonesFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\headphonesShader.vert -o headphonesVert.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\tvShader.frag -o tvFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\tvShader.vert -o tvVert.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\coffeeShader.frag -o coffeeFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\coffeeShader.vert -o coffeeVert.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\chairShader.frag -o chairFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\chairShader.vert -o chairVert.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\cameraShader.frag -o cameraFrag.spv

glslc C:\Users\Francesco\Desktop\Project\ComputerGraphicsProject\shaders\cameraShader.vert -o cameraVert.spv





echo Compilazione completata.

