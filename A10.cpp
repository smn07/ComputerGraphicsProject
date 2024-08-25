// This has been adapted from the Vulkan tutorial

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
#include "Mesh.hpp"


std::vector<SingleText> outText = {
	{2, {"Adding an object", "Press SPACE to save the screenshots","",""}, 0, 0},
	{1, {"Saving Screenshots. Please wait.", "", "",""}, 0, 0}
};




// The uniform buffer object used in this example
//#define NSHIP 16


struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir[4];
	alignas(16) glm::vec3 lightPos[4];
	alignas(16) glm::vec4 lightColor[4];
	alignas(16) glm::vec3 eyePos;
	alignas(16) glm::vec4 lightOn;
};



struct RoomUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

//for the armchair
struct armchairUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

//for the bed
struct bedUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};


// The vertices data structures
struct RoomVertex {
	glm::vec3 pos;
	glm::vec2 UV;
	glm::vec3 norm;
};
/* DA VEDERE SE USARE O MENO
//armchair vertex
struct armchairVertex {
	glm::vec3 pos;
	glm::vec2 UV;
};

//bed vertex
struct bedVertex {
	glm::vec3 pos;
	glm::vec2 UV;
};

*/
const int numLight = 4;
// MAIN ! 
class A10 : public BaseProject {
	protected:
	
	// Descriptor Layouts ["classes" of what will be passed to the shaders]

	DescriptorSetLayout DSLGlobal;	// For Global

	DescriptorSetLayout DSLRoomFace;	// For Room



	// vertex descriptor for the room
	VertexDescriptor VDRoom;

	// Vertex descriptor for the armchair

	// Pipelines [Shader couples]

	Pipeline PRoomFrontFace,PRoomRightFace,PRoomLeftFace,PRoomBottomFace,PArmChair,Pbed,PTable,Pkitchen;

	// Scenes and texts
    TextMaker txt;

	// Models, textures and Descriptor Sets (values assigned to the uniforms)
	DescriptorSet DSGlobal;

	Model MroomFace, bottomFace, Marmchair, Mbed,Mtable,Mkitchen;
	Texture Troom,Tbed,TarmChair,Ttable;
	DescriptorSet DSRoomFrontFace, DSRoomRightFace, DSRoomLeftFace, DSRoomBottomFace, DSArmchair, DSBed,DSTable,DSKitchen;

	

	
// Other application parameters
	int currScene = 0;
	int subpass = 0;

	glm::vec3 CamPos = glm::vec3(0.0, 1.5, 7.0);
	float CamAlpha = 0.0f;//cam rotation
	float CamBeta = 0.0f;//cam rotation
	float Ar;//screen aspect ratio
	glm::mat4 ViewMatrix;
	glm::mat4 LWm[5];// used to contain the light world matrix
	glm::vec3 LCol[5];//used to contain the light color
	float LInt[5];//used to contain the light intensity
	float ScosIn, ScosOut;//used to contain the inner and outer cone of the spot light
	glm::vec4 lightOn;//used to contain the light status, DA VEDERE SE RIDURLO AD UN VETTORE DI 2 PER AVERE SOLO POINT LIGHT E DIRECT
	
	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "project";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		
		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSLGlobal.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject), 1}
				});

		DSLRoomFace.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(RoomUniformBufferObject), 1},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
			});

	


		// Vertex descriptors

		VDRoom.init(this, {
		  {0, sizeof(RoomVertex), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
			  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(RoomVertex, pos),
					 sizeof(glm::vec3), POSITION},
			  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(RoomVertex, UV),
					 sizeof(glm::vec2), UV},
			  {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(RoomVertex, norm),
					 sizeof(glm::vec3), NORMAL}
			});



		// Pipelines [Shader couples]
		PRoomFrontFace.init(this, &VDRoom, "shaders/facesRoomVert.spv", "shaders/frontFaceFrag.spv", {&DSLGlobal,&DSLRoomFace });
		PRoomRightFace.init(this, &VDRoom, "shaders/facesRoomVert.spv", "shaders/leftRightFacesFrag.spv", { &DSLGlobal,&DSLRoomFace });
		PRoomLeftFace.init(this, &VDRoom, "shaders/facesRoomVert.spv", "shaders/leftRightFacesFrag.spv", { &DSLGlobal,&DSLRoomFace });
		PRoomBottomFace.init(this, &VDRoom, "shaders/facesRoomVert.spv", "shaders/bottomFrag.spv", { &DSLGlobal,&DSLRoomFace });
		PArmChair.init(this, &VDRoom, "shaders/armchairVert.spv", "shaders/armchairFrag.spv", { &DSLGlobal,&DSLRoomFace });
		Pbed.init(this, &VDRoom, "shaders/bedVert.spv", "shaders/bedFrag.spv", { &DSLGlobal,&DSLRoomFace });
		PTable.init(this, &VDRoom, "shaders/facesRoomVert.spv", "shaders/bottomFrag.spv", { &DSLGlobal,&DSLRoomFace });
		Pkitchen.init(this, &VDRoom, "shaders/kitchenVert.spv", "shaders/kitchenFrag.spv", { &DSLGlobal,&DSLRoomFace });

		// Create models

		MroomFace.init(this, &VDRoom, "models/Walls_009_Plane.003.mgcg", MGCG);
		bottomFace.init(this, &VDRoom, "models/floor_016_Mesh.338.mgcg", MGCG);
		Marmchair.init(this, &VDRoom, "models/Armchair.obj", OBJ);
		Mbed.init(this, &VDRoom, "models/Bed.obj", OBJ);
		Mtable.init(this, &VDRoom, "models/Table.obj", OBJ);
		Mkitchen.init(this, &VDRoom, "models/kitchen_item_024_Mesh.583.mgcg", MGCG);
		
		// Create the textures used also for the internal objects

		Troom.init(this, "textures/Textures_Forniture.png");
		Tbed.init(this, "textures/Bed_texture.png");
		TarmChair.init(this, "textures/Armchair_Texture.png");
		Ttable.init(this, "textures/Table_Texture.jpg");
		

		// Descriptor pool sizes
		// WARNING!!!!!!!!
		// Must be set before initializing the text and the scene
		DPSZs.uniformBlocksInPool = 50;
		DPSZs.texturesInPool = 40;
		DPSZs.setsInPool = 40;

		std::cout << "Initializing text\n";
		txt.init(this, &outText);

		std::cout << "Initialization completed!\n";
		std::cout << "Uniform Blocks in the Pool  : " << DPSZs.uniformBlocksInPool << "\n";
		std::cout << "Textures in the Pool        : " << DPSZs.texturesInPool << "\n";
		std::cout << "Descriptor Sets in the Pool : " << DPSZs.setsInPool << "\n";
		
		//ViewMatrix = glm::translate(glm::mat4(1), -CamPos);

		// Init local variables
		nlohmann::json js;
		std::ifstream ifs("models/Lights.json");
		if (!ifs.is_open()) {
			std::cout << "Error! Lights file not found!";
			exit(-1);
		}
		try {
			std::cout << "Parsing JSON\n";
			ifs >> js;
			ifs.close();
			//			std::cout << "\n\n\nJson contains " << js.size() << " parts\n\n\n";
			nlohmann::json ns = js["nodes"];
			nlohmann::json ld = js["extensions"]["KHR_lights_punctual"]["lights"];
			for (int i = 0; i < numLight; i++) {
				glm::vec3 T;
				glm::vec3 S;
				glm::quat Q;
				if (ns[i].contains("translation")) {
					//std::cout << "node " << i << " has T\n";
					T = glm::vec3(ns[i]["translation"][0],
						ns[i]["translation"][1],
						ns[i]["translation"][2]);
				}
				else {
					T = glm::vec3(0);
				}
				if (ns[i].contains("rotation")) {
					//std::cout << "node " << i << " has Q\n";
					Q = glm::quat(ns[i]["rotation"][3],
						ns[i]["rotation"][0],
						ns[i]["rotation"][1],
						ns[i]["rotation"][2]);
				}
				else {
					Q = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
				}
				if (ns[i].contains("scale")) {
					//std::cout << "node " << i << " has S\n";
					S = glm::vec3(ns[i]["scale"][0],
						ns[i]["scale"][1],
						ns[i]["scale"][2]);
				}
				else {
					S = glm::vec3(1);
				}
				printVec3("T",T);
				printQuat("Q",Q);
				printVec3("S",S);
				LWm[i] = glm::translate(glm::mat4(1), T) *
					glm::mat4(Q) *
					glm::scale(glm::mat4(1), S);
				printMat4("LWm", LWm[i]);
				nlohmann::json cl = ld[i]["color"];
				std::cout << cl[0] << "\n";
				LCol[i] = glm::vec3(cl[0], cl[1], cl[2]);
				printVec3("LCol",LCol[i]);
				LInt[i] = ld[i]["intensity"];
				std::cout << LInt[i] << "\n\n";
			}
			
		}
		catch (const nlohmann::json::exception& e) {
			std::cout << e.what() << '\n';
		}
		

		lightOn = glm::vec4(1);
		std::cout << "Initialization completed!\n";
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders

		PRoomFrontFace.create();
		PRoomRightFace.create();
		PRoomLeftFace.create();
		PRoomBottomFace.create();
		PArmChair.create();
		Pbed.create();
		PTable.create();
		Pkitchen.create();

		// init the descriptor sets
		DSRoomRightFace.init(this, &DSLRoomFace, { &Troom });
		DSRoomFrontFace.init(this, &DSLRoomFace, {&Troom });
		DSRoomLeftFace.init(this, &DSLRoomFace, {&Troom });
		DSRoomBottomFace.init(this, &DSLRoomFace, {&Troom });
		DSArmchair.init(this, &DSLRoomFace, {&TarmChair});
		DSBed.init(this, &DSLRoomFace, {&Tbed });
		DSTable.init(this, &DSLRoomFace, {&Ttable });
		DSKitchen.init(this, &DSLRoomFace, {&Troom });
	
			
		DSGlobal.init(this, &DSLGlobal, {});
		//DSGlobalRigthFace.init(this, &DSLGlobal, {});
		//DSGlobalArmchair.init(this, &DSLGlobal, {});
		

		txt.pipelinesAndDescriptorSetsInit();		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines

		PRoomFrontFace.cleanup();
		PRoomRightFace.cleanup();
		PRoomLeftFace.cleanup();
		PRoomBottomFace.cleanup();
		PArmChair.cleanup();
		Pbed.cleanup();
		PTable.cleanup();
		Pkitchen.cleanup();


		DSRoomRightFace.cleanup();
		DSRoomLeftFace.cleanup();
		DSRoomFrontFace.cleanup();
		DSRoomBottomFace.cleanup();
		DSArmchair.cleanup();
		DSBed.cleanup();
		DSTable.cleanup();
		DSKitchen.cleanup();


		DSGlobal.cleanup();
		


		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {	

		Troom.cleanup();
		Tbed.cleanup();
		TarmChair.cleanup();
		MroomFace.cleanup();
		bottomFace.cleanup();
		Marmchair.cleanup();
		Mbed.cleanup();
		Mtable.cleanup();
		Mkitchen.cleanup();

		
		// Cleanup descriptor set layouts

		DSGlobal.cleanup();
		DSLRoomFace.cleanup();
		//DSLRoomFace.cleanup();
	

		
		// Destroies the pipelines
		PRoomFrontFace.destroy();
		PRoomRightFace.destroy();
		PRoomLeftFace.destroy();
		PRoomBottomFace.destroy();
		PArmChair.destroy();
		Pbed.destroy();
		PTable.destroy();
		Pkitchen.destroy();

		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		//armchair
		Marmchair.bind(commandBuffer);
		PArmChair.bind(commandBuffer);
		DSArmchair.bind(commandBuffer, PArmChair, 1, currentImage);
		DSGlobal.bind(commandBuffer, PArmChair, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Marmchair.indices.size()), 1, 0, 0, 0);

		//bed
		Mbed.bind(commandBuffer);
		Pbed.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, Pbed, 0, currentImage);
		DSBed.bind(commandBuffer, Pbed, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(Mbed.indices.size()), 1, 0, 0, 0);
		// binds the pipeline

		PRoomFrontFace.bind(commandBuffer);
		
		// The models (both index and vertex buffers)

		MroomFace.bind(commandBuffer);
		//bottomFace.bind(commandBuffer);
		
		// The descriptor sets, for each descriptor set specified in the pipeline
		//DSGlobalFrontFace.bind(commandBuffer, PRoomFrontFace, 0, currentImage);	// The Global Descriptor Set (Set 0)
		
		
		DSRoomFrontFace.bind(commandBuffer, PRoomFrontFace, 1, currentImage);	// The Room Descriptor Set (Set 1)
		DSGlobal.bind(commandBuffer, PRoomFrontFace, 0, currentImage);
					
		// The actual draw call.
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MroomFace.indices.size()), 1, 0, 0, 0);	

		// binds the pipeline
		PRoomRightFace.bind(commandBuffer);

		// The models (both index and vertex buffers)
		//Msun.bind(commandBuffer);

		// The descriptor sets, for each descriptor set specified in the pipeline
		DSRoomRightFace.bind(commandBuffer, PRoomRightFace, 1, currentImage);
		DSGlobal.bind(commandBuffer, PRoomRightFace, 0, currentImage);
		// The actual draw call.
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MroomFace.indices.size()), 1, 0, 0, 0);

		PRoomLeftFace.bind(commandBuffer);
		DSRoomLeftFace.bind(commandBuffer, PRoomLeftFace, 1, currentImage);
		DSGlobal.bind(commandBuffer, PRoomLeftFace, 0, currentImage);
		// The actual draw call.
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MroomFace.indices.size()), 1, 0, 0, 0);


		//l'idea è di fare il binding del modello sempre a ridosso del binding della pipeline.
		bottomFace.bind(commandBuffer);

		PRoomBottomFace.bind(commandBuffer);
		DSRoomBottomFace.bind(commandBuffer, PRoomBottomFace, 1, currentImage);
		DSGlobal.bind(commandBuffer, PRoomBottomFace, 0, currentImage);
		// The actual draw call.
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(bottomFace.indices.size()), 1, 0, 0, 0);
		PTable.bind(commandBuffer);
		Mtable.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, PTable, 0, currentImage);
		DSTable.bind(commandBuffer, PTable, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
						static_cast<uint32_t>(Mtable.indices.size()), 1, 0, 0, 0);
		
		Pkitchen.bind(commandBuffer);
		Mkitchen.bind(commandBuffer);
		DSGlobal.bind(commandBuffer, Pkitchen, 0, currentImage);
		DSKitchen.bind(commandBuffer, Pkitchen, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
						static_cast<uint32_t>(Mkitchen.indices.size()), 1, 0, 0, 0);

		txt.populateCommandBuffer(commandBuffer, currentImage, currScene);

		
	}
	// put all the object in a certain initial position in the screen 
	glm::mat4 initialTranslation() {
		return glm::translate(glm::mat4(1), glm::vec3(0, -5, -15));
	}
	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);

		static float autoTime = true;
		static float cTime = 0.0;
		const float turnTime = 36.0f;
		const float angTurnTimeFact = 2.0f * M_PI / turnTime;

		if (autoTime) {
			cTime = cTime + deltaT;
			cTime = (cTime > turnTime) ? (cTime - turnTime) : cTime;
		}
		cTime += r.z * angTurnTimeFact * 4.0;

		const float ROT_SPEED = glm::radians(120.0f);
		const float MOVE_SPEED = 2.0f;

		CamAlpha = CamAlpha - ROT_SPEED * deltaT * r.y;
		CamBeta = CamBeta - ROT_SPEED * deltaT * r.x;
		CamBeta = CamBeta < glm::radians(-90.0f) ? glm::radians(-90.0f) :
			(CamBeta > glm::radians(90.0f) ? glm::radians(90.0f) : CamBeta);

		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0, 1, 0)) * glm::vec4(1, 0, 0, 1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, 1, 1);
		CamPos = CamPos + MOVE_SPEED * m.x * ux * deltaT;
		CamPos = CamPos + MOVE_SPEED * m.y * glm::vec3(0, 1, 0) * deltaT;
		CamPos = CamPos + MOVE_SPEED * m.z * uz * deltaT;

		static float subpassTimer = 0.0;

		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				if (currScene != 1) {
					currScene = (currScene + 1) % outText.size();

				}
				if (currScene == 1) {
					if (subpass >= 4) {
						currScene = 0;
					}
				}
				std::cout << "Scene : " << currScene << "\n";

				RebuildPipeline();
			}
		}
		else {
			if ((curDebounce == GLFW_KEY_SPACE) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		// Standard procedure to quit when the ESC key is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}


		if (glfwGetKey(window, GLFW_KEY_V)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_V;

				printVec3("CamPos  ", CamPos);
				std::cout << "CamAlpha = " << CamAlpha << ";\n";
				std::cout << "CamBeta  = " << CamBeta << ";\n";
				std::cout << "cTime    = " << cTime << ";\n";
			}
		}
		else {
			if ((curDebounce == GLFW_KEY_V) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_1)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_1;
				std::cout << " value1= " << lightOn.x << ";\n";
				lightOn.x = 1 - lightOn.x;
			}
		}
		else {
			if ((curDebounce == GLFW_KEY_1) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_2)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_2;
				lightOn.y = 1 - lightOn.y;
			}
		}
		else {
			if ((curDebounce == GLFW_KEY_2) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_3)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_3;
				lightOn.z = 1 - lightOn.z;
			}
		}
		else {
			if ((curDebounce == GLFW_KEY_3) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_4)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_4;
				lightOn.w = 1 - lightOn.w;
			}
		}
		else {
			if ((curDebounce == GLFW_KEY_4) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		/*		if(currScene == 1) {
					switch(subpass) {
					  case 0:
		CamPos   = glm::vec3(0.0644703, 6.442, 8.83251);
		CamAlpha = 0;
		CamBeta  = -0.4165;
		cTime    = 2.40939;
		autoTime = false;
						break;
					  case 1:
		CamPos   = glm::vec3(-1.21796, 6.82323, 5.58497);
		CamAlpha = 0.284362;
		CamBeta  = -0.58455;
		cTime    = 23.3533;
						break;
					  case 2:
		CamPos   = glm::vec3(0.921455, 3.97743, 0.855181);
		CamAlpha = -1.16426;
		CamBeta  = -0.388393;
		cTime    = 36.6178;
						break;
					  case 3:
		 CamPos   = glm::vec3(5.59839, 4.04786, 2.59767);
		CamAlpha = 1.01073;
		CamBeta  = -0.213902;
		cTime    = 15.6739;
						break;
					}
				}

				if(currScene == 1) {
					subpassTimer += deltaT;
					if(subpassTimer > 4.0f) {
						subpassTimer = 0.0f;
						subpass++;
						std::cout << "Scene : " << currScene << " subpass: " << subpass << "\n";
						char buf[20];
						sprintf(buf, "A08_%d.png", subpass);
						saveScreenshot(buf, currentImage);
						if(subpass == 4) {
		CamPos   = glm::vec3(0, 1.5, 7);
		CamAlpha = 0;
		CamBeta  = 0;
		autoTime = true;
							currScene = 0;
							std::cout << "Scene : " << currScene << "\n";
							RebuildPipeline();
						}
					}
				}*/


				// Here is where you actually update your uniforms
		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 50.0f);
		M[1][1] *= -1;

		glm::mat4 Mv = glm::rotate(glm::mat4(1.0), -CamBeta, glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0), -CamAlpha, glm::vec3(0, 1, 0)) *
			glm::translate(glm::mat4(1.0), -CamPos);

		glm::mat4 ViewPrj = M * Mv;
		glm::mat4 baseTr = glm::mat4(1.0f);

		// updates global uniforms
		// Global

		
		GlobalUniformBufferObject gubo{};
		gubo.eyePos = CamPos;
		gubo.lightOn = lightOn;
		for (int i = 0; i < numLight; i++) {
			gubo.lightColor[i] = glm::vec4(LCol[i], LInt[i]);
			gubo.lightDir[i] = LWm[i] * glm::vec4(0, 0, 1, 0);// direction taken by multiplying the vector (0,0,1) by the light world matrix, we just need the Z component direction
			gubo.lightPos[i] = LWm[i] * glm::vec4(0, 0, 0, 1); //position taken by multiplying the vector (0,0,0) by the light world matrix, we just need the the last component of the matrix that indicates the position in the space where the light it has been put
		}
		DSGlobal.map(currentImage, &gubo, 0);

		// objects
		RoomUniformBufferObject roomFrontFaceUbo{};
		roomFrontFaceUbo.mMat= glm::translate(glm::mat4(1), glm::vec3(0, 0, -4)) * initialTranslation() * glm::scale(glm::mat4(1), glm::vec3(3, 1, 1)) * baseTr;
		roomFrontFaceUbo.mvpMat = ViewPrj* roomFrontFaceUbo.mMat;
		roomFrontFaceUbo.nMat = glm::inverse(glm::transpose(roomFrontFaceUbo.mMat));
		//roomRightFaceUbo.mvpMat = ViewPrj * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) * baseTr;
		//DSRoomRightFace.map(currentImage, &roomRightFaceUbo, 0);
		DSRoomFrontFace.map(currentImage, &roomFrontFaceUbo, 0);

		RoomUniformBufferObject roomRightFaceUbo{};
		roomRightFaceUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(6, 0, 2)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(3, 1, 1)) * baseTr;
		roomRightFaceUbo.mvpMat = ViewPrj* roomRightFaceUbo.mMat;
		roomRightFaceUbo.nMat = glm::inverse(glm::transpose(roomRightFaceUbo.mMat));
		DSRoomRightFace.map(currentImage, &roomRightFaceUbo, 0);

		RoomUniformBufferObject roomLeftFaceUbo{};
		roomLeftFaceUbo.mMat=glm::translate(glm::mat4(1), glm::vec3(-6, 0, 2))* initialTranslation()* glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0))* glm::scale(glm::mat4(1), glm::vec3(3, 1, 1))* baseTr;
		roomLeftFaceUbo.mvpMat = ViewPrj * roomLeftFaceUbo.mMat;
		roomLeftFaceUbo.nMat = glm::inverse(glm::transpose(roomLeftFaceUbo.mMat));

		DSRoomLeftFace.map(currentImage, &roomLeftFaceUbo, 0);

		RoomUniformBufferObject roomBottomFaceUbo{};
		roomBottomFaceUbo.mMat= glm::translate(glm::mat4(1), glm::vec3(0, 0, +2)) * initialTranslation() * glm::scale(glm::mat4(1), glm::vec3(3, 1, 3)) * baseTr;
		//roomBottomFaceUbo.mvpMat = ViewPrj * glm::translate(glm::mat4(1), glm::vec3(0, 0, -4)) *initialTranslation()* glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::mat4(1), glm::vec3(3, 3, 1)) * baseTr;
		roomBottomFaceUbo.mvpMat = ViewPrj * roomBottomFaceUbo.mMat;
		roomBottomFaceUbo.nMat = glm::inverse(glm::transpose(roomBottomFaceUbo.mMat));

		DSRoomBottomFace.map(currentImage, &roomBottomFaceUbo, 0);

		//armchair
		armchairUniformBufferObject armchairUbo{};
		armchairUbo.mMat= glm::translate(glm::mat4(1), glm::vec3(5, 0.35, 2)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0))* glm::scale(glm::mat4(1), glm::vec3(0.7, 0.7, 0.7)) * baseTr;
		armchairUbo.mvpMat = ViewPrj * armchairUbo.mMat;
		armchairUbo.nMat = glm::inverse(glm::transpose(armchairUbo.mMat));
		DSArmchair.map(currentImage, &armchairUbo, 0);

		//bed
		bedUniformBufferObject bedUbo{};
		bedUbo.mMat= glm::translate(glm::mat4(1), glm::vec3(1, -0.1, -2.7)) * initialTranslation() * baseTr;
		bedUbo.mvpMat = ViewPrj * bedUbo.mMat;
		bedUbo.nMat = glm::inverse(glm::transpose(bedUbo.mMat));
		DSBed.map(currentImage, &bedUbo, 0);

		//table
		RoomUniformBufferObject tableUbo{};
		tableUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0, 0.3, +4))*initialTranslation() * glm::scale(glm::mat4(1), glm::vec3(0.8, 0.8, 0.8)) * baseTr;
		tableUbo.mvpMat = ViewPrj * tableUbo.mMat;
		tableUbo.nMat = glm::inverse(glm::transpose(tableUbo.mMat));
		DSTable.map(currentImage, &tableUbo, 0);

		//kitchen
		RoomUniformBufferObject kitchenUbo{};
		kitchenUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(-5.8, 0, -3.7))*initialTranslation() * glm::scale(glm::mat4(1), glm::vec3(1.5, 1.5, 1.5))*baseTr;
		kitchenUbo.mvpMat = ViewPrj * kitchenUbo.mMat;
		kitchenUbo.nMat = glm::inverse(glm::transpose(kitchenUbo.mMat));
		DSKitchen.map(currentImage, &kitchenUbo, 0);

	}
};

// This is the main: probably you do not need to touch this!
int main() {
    A10 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
