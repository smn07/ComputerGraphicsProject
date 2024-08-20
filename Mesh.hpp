

/**************
 Creae the meshes, as described below
 
 WARNING!
 Since it is a C program, you can use for loops and functions if you think they can be helpful in your solution.
 However, please include all your code in this file, since it will be put in an automatic correction process
 for the final evaluation. Please also be cautious when using standard libraries and symbols, since they
 might not be available in all the development environments (especially, they might not be available
 in the final evaluation environment, preventing your code from compiling).
 This WARNING will be valid far ALL THE ASSIGNMENTs, but it will not be repeated in the following texts,
 so please remember these advices carefully!
 
***************/


/*SIMONE DI IENNO, MATRICOLA: 225606, CODICE ID: 10938038*/

void MakeSquare(float size, std::vector<glm::vec3> &vertices, std::vector<uint32_t> &indices) {
// Creates a square, on the xz-plane, aligned with the axis, and centered in the origin.
// The length of the four sides is in parameter >size<.
// The procedure should fill the array contained in the >vertices< parameter, with the positions of
// the vertices of the primitive, expressed with their local coordinates in a glm::vec3 element. 
// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
// be a multiple of 3: each group of three indices, defines a different triangle.
//
// HINT: this procedure has already been implemented. You can keep it as is
	vertices = {
				   {-size/2.0f,0.0f,-size/2.0f},
				   {-size/2.0f,0.0f, size/2.0f},
				   { size/2.0f,0.0f,-size/2.0f},
				   { size/2.0f,0.0f, size/2.0f}};
	indices = {0, 1, 2,    1, 3, 2};
}

void MakeCube(float size, std::vector<glm::vec3> &vertices, std::vector<uint32_t> &indices) {
// Creates a cube, with the faces perpendicular to the axis, and centered in the origin.
// The length of one edge of the cube is >size<.
// The procedure should fill the array contained in the >vertices< parameter, with the positions of
// the vertices of the primitive, expressed with their local coordinates in a glm::vec3 element. 
// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
// be a multiple of 3: each group of three indices, defines a different triangle.
//
// HINT: the procedure below creates a square. You can use it as a side of the cube (please remember
// to change the value of the y component, otherwise the result will be wrong
	vertices = {
				   {-size/2.0f,-size/2.0f,-size/2.0f}, //0
				   {-size/2.0f,-size/2.0f, size/2.0f}, //1
				   { size/2.0f,-size/2.0f,-size/2.0f}, //2
				   { size/2.0f,-size/2.0f, size/2.0f}, //3

				   {-size/2.0f, size/2.0f,-size/2.0f}, //4
				   {-size/2.0f, size/2.0f, size/2.0f}, //5
				   { size/2.0f, size / 2.0f,-size/2.0f}, //6
				   { size/2.0f, size/2.0f, size/2.0f}}; //7

	indices = { 0,2,1,    1,2,3,   4,5,6,   6,5,7,   1,3,7,   1,7,5,   0,1,5,  0,5,4,    3,2,7,   2,6,7,   0,4,6,  2,0,6 };

}

void MakeCylinder(float radius, float height, int slices, std::vector<glm::vec3> &vertices, std::vector<uint32_t> &indices) {
// Creates a cylinder, approximated by a prism with a base composed by a regular polygon with >slices< sides.
// The radius of the cylinder is >radius<, and it height is >height<. The cylinder has its centere
// in the origin, and spans half above and half below the plane that passes thorugh the origin.
// The top and bottom caps are are aligned with xz-plane and perpendicular to the y-axis.
// The procedure should fill the array contained in the >vertices< parameter, with the positions of
// the vertices of the primitive, expressed with their local coordinates in a glm::vec3 element. 
// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
// be a multiple of 3: each group of three indices, defines a different triangle.
//
// HINT: the procedure below creates a rectangle. You have to change it, or you will obtain a wrong result
// You should use a for loop, and you should start from the procedure to create a circle seen during the lesson
	/*vertices = {
				   {-radius,-height/2.0f,0.0f},
				   {-radius, height/2.0f,0.0f},
				   { radius,-height/2.0f,0.0f},
				   { radius, height/2.0f,0.0f}};
	indices = {0, 2, 1,    1, 2, 3};*/


	//i have 2 bases which are 2 circles with their own center.
	vertices.resize(slices * 2 + 2);
	indices.resize((3 * slices) * 4);
	// i'm inserting the center of the first base at the end of the vertices array
	vertices[vertices.size() - 1] = {0.0f, -height / 2.0f, 0.0f};

	// now i'm creating the first base
	for (int i = 0; i < slices; i++) {
		float ang = 2 * M_PI * (float)i / (float)slices;
		vertices[i] = glm::vec3(radius * cos(ang), -height / 2.0f, radius * sin(ang));

		indices[3 * i] = vertices.size() - 1;
		indices[3 * i + 1] = i;
		indices[3 * i + 2] = (i + 1) % slices;
	}

	// now i'm creating the second base
	// i'm inserting the center of the first base at the end of the vertices array
	vertices[vertices.size() - 2] = {0.0f, height / 2.0f, 0.0f};
	for (int i = slices; i < 2*slices; i++) {
		float ang = 2 * M_PI * (float)i / (float)slices;
		vertices[i] = glm::vec3(radius * cos(ang), height / 2.0f, radius * sin(ang));

		indices[3 * i] = vertices.size() - 2;
		// i'm checking if i'm at the last vertex of the circle, in that case i have to connect it with the first vertex of the circle
		if(i == 2*slices - 1) //if i is equal to (2*slices - 1) then i have to come back to "slices" value.
			indices[3 * i + 1] = slices;
		else
			indices[3 * i + 1] = i + 1;

		indices[3 * i + 2] = i;
	}

	// now i'm creating the lateral surfaces
	for(int i = 0; i < slices; i++) {

		/*1. set the index of the first vertex of the triangle on the bottom base
		  2. set the index of the second vertex of the triangle on the top base
		  3. set the index of the third vertex of the triangle on the top base*/
		indices[3 * (2 * slices + i)] = i; //1
		indices[3 * (2 * slices + i) + 1] = i + slices; //2
		indices[3 * (2 * slices + i) + 2] = (i + 1) % slices + slices; //3

		/*this is for creating the second triagle of the surface*/
		indices[3 * (3 * slices + i)] = i;
		indices[3 * (3 * slices + i) + 1] = (i + 1) % slices + slices;
		indices[3 * (3 * slices + i) + 2] = (i + 1) % slices;

	}

}

void MakeCone(float radius, float height, int slices, std::vector<glm::vec3> &vertices, std::vector<uint32_t> &indices) {
// Creates a cone, approximated by a pyramid with a base composed by a regular polygon with >slices< sides.
// The radius of the cone is >radius<, and it height is >height<. The cone has its center
// in the origin, and spans half above and half below the plane that passes thorugh the origin.
// The bottom cap is aligned with xz-plane and perpendicular to the y-axis.
// The procedure should fill the array contained in the >vertices< parameter, with the positions of
// the vertices of the primitive, expressed with their local coordinates in a glm::vec3 element. 
// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
// be a multiple of 3: each group of three indices, defines a different triangle.
//
// HINT: the procedure below creates a triangle. You have to change it, or you will obtain a wrong result
// You should use a for loop, and you should start from the procedure to create a circle seen during the lesson
	/*vertices = {
				   {-radius,-height/2.0f,0.0f},
				   { radius,-height/2.0f,0.0f},
				   { 0.0f, height/2.0f,0.0f}};
	indices = {0, 1, 2};*/

	vertices.resize(slices + 2); // +2 because i have to add the upper vertex of the cone and the center of the base
	indices.resize((3 * slices) * 2); //i need 1 triangle for each slice and 1 triangle for each lateral surface
	// i'm inserting the center of the first base at the end of the vertices array
	vertices[vertices.size() - 1] = {0.0f, -height / 2.0f, 0.0f};

	// now i'm creating the base
	for (int i = 0; i < slices; i++) {
		float ang = 2 * M_PI * (float)i / (float)slices;
		vertices[i] = glm::vec3(radius * cos(ang), -height / 2.0f, radius * sin(ang));

		//i'm creating the triangles of the base
		indices[3 * i] = vertices.size() - 1;
		indices[3 * i + 1] = i;
		indices[3 * i + 2] = (i + 1) % slices;
	}

	// now i'm creating the upper vertex of the cone
	vertices[vertices.size() - 2] = {0.0f, height / 2.0f, 0.0f};

	// now i'm creating the lateral surfaces
	for(int i = 0; i < slices; i++) {

		/*1. set the index of the first vertex of the triangle on the bottom base
		  2. set the index of the upper vertex of the cone
		  3. set the index of the third vertex of the triangle on the bottom base*/
		indices[3 * (slices + i)] = i; //1
		indices[3 * (slices + i) + 1] = slices; //2
		indices[3 * (slices + i) + 2] = (i + 1) % slices; //3
	}

}

void MakeSphere(float radius, int rings, int slices, std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices)
{
	// Creates a sphere, approximated by a poliedron composed by >slices<, and >rings< rings.
	// The radius of the sphere is >radius<, and it is centered in the origin.
	// The procedure should fill the array contained in the >vertices< parameter, with the positions of
	// the vertices of the primitive, expressed with their local coordinates in a glm::vec3 element. 
	// Indices should be returned in the >indices< array, starting from 0, and up to vertices.size()-1.
	// The primitive is encoded as an indexed triangle list, so the size of the >indices< array, should
	// be a multiple of 3: each group of three indices, defines a different triangle.
	//
	// HINT: the procedure below creates a circle. You have to change it, or you will obtain a wrong result
	// You should use two nested for loops, one used to span across the rings, and the other that spans along
	// the rings.
	  /*vertices.resize(slices + 1);
	  indices.resize(3*slices);
	  vertices[slices]= {0.0f,0.0f,0.0f};
	  for(int i = 0; i < slices; i++) {
		float ang = 2*M_PI * (float)i / (float)slices;
		vertices[i] = glm::vec3(radius * cos(ang), radius * sin(ang), 0.0f);
		indices[3*i  ] = slices;
		indices[3*i+1] = i;
		indices[3*i+2] = (i+1) % slices;
	  }*/

	vertices.resize(slices * (rings - 1) + 2 + (rings - 1)); // +2 because i have to add the upper and lower vertex of the sphere + (rings-1) because i have to add the center of the rings
	indices.resize(6 * slices * (rings - 2) + 3 * 2 * slices);
	// i have to insert the lower vertex of the sphere at the end of the vertices array
	vertices[vertices.size() - 1] = { 0.0f, -radius, 0.0f };
	// i have to insert the upper vertex of the sphere at the end-1 of the vertices array
	vertices[vertices.size() - 2] = { 0.0f, radius, 0.0f };

	//i have to generate all the rings that compose the sphere
	for (int i = 0; i < rings - 1; i++) {
		// i'm inserting the center of the ring at the end of the vertices array
		float angle = ((float)180 / (float)rings) * (i + 1);
		float angCircle = (float)glm::radians(angle);


		vertices[vertices.size() - 3 - i] = { 0.0f, -radius + (radius - radius * cos(angCircle)), 0.0f };
	}

	// i need a crescent radius for each ring of the sphere (i'm starting from the top ring)

	for (int i = 0; i < rings - 1; i++) {
		float cresecent_radius = 0.0f;
		float angle = ((float)180 / (float)rings) * (i + 1);
		float angCircle = (float)glm::radians(angle);
		cresecent_radius = (float)radius * sin(angCircle);

		// i'm creating the first ring
		for (int j = 0; j < slices; j++) {
			float ang = 2 * M_PI * (float)j / (float)slices;
			vertices[i * slices + j] = glm::vec3(cresecent_radius * cos(ang), -radius + (radius - radius * cos(angCircle)), cresecent_radius * sin(ang));

			indices[3 * slices * i + 3 * j] = vertices.size() - 3 - i;
			indices[3 * slices * i + 3 * j + 1] = ((j + 1) % slices) + slices * i;
			indices[3 * slices * i + 3 * j + 2] = j + slices * i;

		}
	}

	// connect all the rings
	for (int i = 0; i < rings - 2; i++) {
		for (int j = 0; j < slices; j++) {
			indices[6 * slices * i + 6 * j] = i * slices + j;
			indices[6 * slices * i + 6 * j + 1] = (i + 1) * slices + j;
			indices[6 * slices * i + 6 * j + 2] = (i + 1) * slices + (j + 1) % slices;
			indices[6 * slices * i + 6 * j + 3] = i * slices + j;
			indices[6 * slices * i + 6 * j + 4] = (i + 1) * slices + (j + 1) % slices;
			indices[6 * slices * i + 6 * j + 5] = i * slices + (j + 1) % slices;
		}
	}


	// Connect the lower ring with the lower center of the sphere
	for (int j = 0; j < slices; j++) {
		// Indices for the triangle connecting the upper center, current vertex, and next vertex
		indices[6 * slices * (rings - 2) + 3 * j] = vertices.size() - 1;  // Index of the upper center
		indices[6 * slices * (rings - 2) + 3 * j + 1] = j;  // Current vertex index
		indices[6 * slices * (rings - 2) + 3 * j + 2] = (j + 1) % slices;  // Next vertex index
	}
	// Connect the lower ring with the lower center of the sphere
	for (int j = 0; j < slices; j++) {
		// Indices for the triangle connecting the lower center, next vertex, and current vertex
		indices[6 * slices * (rings - 2) + 3 * slices + 3 * j] = vertices.size() - 2;  // Index of the lower center
		indices[6 * slices * (rings - 2) + 3 * slices + 3 * j + 1] = (rings - 2) * slices + (j + 1) % slices;
		indices[6 * slices * (rings - 2) + 3 * slices + 3 * j + 2] = (rings - 2) * slices + j;
	}


}