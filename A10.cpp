// This has been adapted from the Vulkan tutorial

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
#include "Mesh.hpp"
#include <vector>

int numberObject = -1;
bool all = true;

glm::vec3 oldCamPos = glm::vec3(0.0, 0.0, 0.0);
float oldCamAlpha = 0.0f;//cam rotation
float oldCamBeta = 0.0f;//cam rotation

std::vector<SingleText> outText = {
	{2, {"Monolocale di Milano", "Press SPACE when cursor is over an object to focus", "",""}, 0, 0}
	
};

enum hitBoxOjects {
	table, bed, frontWall, leftWall, rightWall, fridge, kitchenLeftWall, kitchenFrontWall, armchair, vase, microwave, tea, 
	tv, ball, headphones, camera, coffee, toilet, camHitBoxEnum, cursor
};


struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir[4];
	alignas(16) glm::vec3 lightPos[4];
	alignas(16) glm::vec4 lightColor[4];
	alignas(16) glm::vec3 eyePos;
	alignas(16) glm::vec4 lightOn;
};


struct GlobalUniformBufferObjectFocus {
	alignas(16) glm::vec3 lightDir[5];
	alignas(16) glm::vec3 lightPos[5];
	alignas(16) glm::vec4 lightColor[5];
	alignas(16) glm::vec3 eyePos;
	alignas(16) glm::vec4 lightOn;
	alignas(4) float cosIn;
	alignas(4) float cosOut;
};


struct RoomUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
	alignas(4) int selected;
};
//for the armchair
struct armchairUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
	alignas(4) int selected;
};
//for the bed
struct bedUniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
	alignas(4) int selected;
};

//for the Blinn parameters (headphones)
struct HeadphonesParUniformBufferObject {
	alignas(4) float Pow;
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

struct AABB {

	glm::vec3 min;  // Minimum corner of the AABB
	glm::vec3 max;  // Maximum corner of the AABB 
	hitBoxOjects object;
	bool selected = false;
	AABB(const glm::vec3& min_, const glm::vec3& max_, hitBoxOjects obj)  {
	min = min_;
	max = max_;  
	object = obj;

} 

AABB() {}
/*AABB& operator=(const AABB& other) {  if (this == &other) {
  return *this; // Gestisce l'auto-assegnazione  }
 min = other.min;
 max = other.max;  object = other.object;
 selected = other.selected;  return *this;
} */

void setSelection(bool s) {
	selected = s;
} 

bool isSelected() {
	return selected;
}
};

const int numLight = 4;
const int numLightFocus = 5;

//MAIN
class A10 : public BaseProject {
	protected:
	
	// Descriptor Layouts ["classes" of what will be passed to the shaders]

	DescriptorSetLayout DSLGlobal;	// For Global

	DescriptorSetLayout DSLRoomFace;	// For Room

	DescriptorSetLayout DSLHeadphones;

	DescriptorSetLayout DSLGlobalFocus;// For Global with focus


	// vertex descriptor for the room
	VertexDescriptor VDRoom;

	// Vertex descriptor for the armchair

	// Pipelines [Shader couples]

	Pipeline PRoomFrontFace, PRoomBackFace,PRoomRightFace,PRoomLeftFace,PRoomBottomFace,PArmChair,Pbed,PTable,Pkitchen,Pvase,Pfridge,
		Pmicrowave,Pball,Ptea, Ptoilet, Pheadphones, Ptv, Pcoffee, Pcamera, Pchair,Pcursor;

	// Scenes and texts
    TextMaker txt;

	// Models, textures and Descriptor Sets (values assigned to the uniforms)
	DescriptorSet DSGlobal, DSGlobalFocus;

	Model MroomFace, bottomFace, Marmchair, Mbed,Mtable,Mkitchen,Mvase,Mfridge,Mmicrowave,Mball,Mtea, 
		Mtoilet, Mheadphones, Mtv, Mcoffee, Mcamera, Mchair,Mcursor;
	Texture Troom,Tbed,TarmChair,Ttable,Tvase,Tcursor;
	DescriptorSet DSRoomFrontFace, DSRoomRightFace, DSRoomLeftFace, DSRoomBottomFace, DSArmchair, DSBed, DSTable, DSKitchen,
		DSvase, DSfridge, DSmicrowave, DSball, DStea, DStoilet, DSheadphones, DStv, DScoffee, DScamera, DSchair,DScursor, DSRoomBackFace;

	

	
// Other application parameters
	int currScene = 0;
	int subpass = 0;

	glm::vec3 CamPos = glm::vec3(0.0, -3, -13);
	float CamAlpha = 0.0f;//cam rotation
	float CamBeta = 0.0f;//cam rotation
	float ObjAlpha = 0.0f;//cam rotation
	float ObjBeta = 0.0f;//cam rotation
	float Ar;//screen aspect ratio
	glm::mat4 ViewMatrix;
	glm::mat4 LWm[6];// used to contain the light world matrix
	glm::vec3 LCol[6];//used to contain the light color
	float LInt[6];//used to contain the light intensity
	float ScosIn, ScosOut;//used to contain the inner and outer cone of the spot light
	glm::vec4 lightOn;//used to contain the light status, DA VEDERE SE RIDURLO AD UN VETTORE DI 2 PER AVERE SOLO POINT LIGHT E DIRECT
	
	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Graphicsproject";
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

		DSLGlobalFocus.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObjectFocus), 1}
		});

		DSLHeadphones.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(RoomUniformBufferObject), 1},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1 },
			{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(HeadphonesParUniformBufferObject), 1},
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
		PRoomFrontFace.init(this, &VDRoom, "shaders/facesRoomVert.spv", "shaders/frontFaceFrag.spv", {&DSLGlobalFocus,&DSLRoomFace });
		PRoomBackFace.init(this, &VDRoom, "shaders/backRoomVert.spv", "shaders/backFrag.spv", { &DSLGlobalFocus,&DSLRoomFace });
		PRoomRightFace.init(this, &VDRoom, "shaders/facesRoomVert.spv", "shaders/leftRightFacesFrag.spv", { &DSLGlobalFocus,&DSLRoomFace });
		PRoomLeftFace.init(this, &VDRoom, "shaders/facesRoomVert.spv", "shaders/leftRightFacesFrag.spv", { &DSLGlobalFocus,&DSLRoomFace });
		PRoomBottomFace.init(this, &VDRoom, "shaders/facesRoomVert.spv", "shaders/bottomFrag.spv", { &DSLGlobalFocus,&DSLRoomFace });
		PArmChair.init(this, &VDRoom, "shaders/armchairVert.spv", "shaders/armchairFrag.spv", { &DSLGlobalFocus,&DSLRoomFace });
		Pbed.init(this, &VDRoom, "shaders/bedVert.spv", "shaders/bedFrag.spv", { &DSLGlobal,&DSLRoomFace });
		PTable.init(this, &VDRoom, "shaders/facesRoomVert.spv", "shaders/tableFrag.spv", { &DSLGlobal,&DSLRoomFace });
		Pkitchen.init(this, &VDRoom, "shaders/kitchenVert.spv", "shaders/kitchenFrag.spv", { &DSLGlobal,&DSLRoomFace });
		Pvase.init(this, &VDRoom, "shaders/vaseVert.spv", "shaders/vaseFrag.spv", { &DSLGlobalFocus,&DSLRoomFace });
		Pfridge.init(this, &VDRoom, "shaders/fridgeVert.spv", "shaders/fridgeFrag.spv", { &DSLGlobal,&DSLRoomFace });
		Pmicrowave.init(this, &VDRoom, "shaders/microwaveVert.spv", "shaders/microwaveFrag.spv", { &DSLGlobalFocus,&DSLRoomFace });
		Pball.init(this, &VDRoom, "shaders/ballVert.spv", "shaders/ballFrag.spv", { &DSLGlobalFocus,&DSLRoomFace });
		Ptea.init(this, &VDRoom, "shaders/teaVert.spv", "shaders/teaFrag.spv", { &DSLGlobalFocus,&DSLRoomFace });
		Ptoilet.init(this, &VDRoom, "shaders/toiletVert.spv", "shaders/toiletFrag.spv", { &DSLGlobal,&DSLRoomFace });
		Pheadphones.init(this, &VDRoom, "shaders/headphonesVert.spv", "shaders/headphonesFrag.spv", { &DSLGlobalFocus,&DSLHeadphones });
		Ptv.init(this, &VDRoom, "shaders/tvVert.spv", "shaders/tvFrag.spv", { &DSLGlobal,&DSLRoomFace });
		Pcoffee.init(this, &VDRoom, "shaders/coffeeVert.spv", "shaders/coffeeFrag.spv", { &DSLGlobalFocus,&DSLRoomFace });
		Pchair.init(this, &VDRoom, "shaders/chairVert.spv", "shaders/chairFrag.spv", { &DSLGlobal,&DSLRoomFace });
		Pcamera.init(this, &VDRoom, "shaders/cameraVert.spv", "shaders/cameraFrag.spv", { &DSLGlobalFocus,&DSLRoomFace });
		Pcursor.init(this, &VDRoom, "shaders/cursorVert.spv", "shaders/cursorFrag.spv", { &DSLRoomFace });

		// Create models
		MroomFace.init(this, &VDRoom, "models/Walls_009_Plane.003.mgcg", MGCG);
		bottomFace.init(this, &VDRoom, "models/floor_016_Mesh.338.mgcg", MGCG);
		Marmchair.init(this, &VDRoom, "models/Armchair.obj", OBJ);
		Mbed.init(this, &VDRoom, "models/Bed.obj", OBJ);
		Mtable.init(this, &VDRoom, "models/Table.obj", OBJ);
		Mkitchen.init(this, &VDRoom, "models/kitchen_item_024_Mesh.583.mgcg", MGCG);
		Mvase.init(this, &VDRoom, "models/Vase.obj", OBJ);
		Mfridge.init(this, &VDRoom, "models/fridge.mgcg", MGCG);
		Mmicrowave.init(this, &VDRoom, "models/microwave.mgcg", MGCG);
		Mball.init(this, &VDRoom, "models/ball.mgcg", MGCG);
		Mtea.init(this, &VDRoom, "models/tea.mgcg", MGCG);
		Mtoilet.init(this, &VDRoom, "models/toilet.mgcg", MGCG);
		Mheadphones.init(this, &VDRoom, "models/headphones.mgcg", MGCG);
		Mtv.init(this, &VDRoom, "models/tv.mgcg", MGCG);
		Mcoffee.init(this, &VDRoom, "models/coffee.mgcg", MGCG);
		Mcamera.init(this, &VDRoom, "models/camera.mgcg", MGCG);
		Mchair.init(this, &VDRoom, "models/chair.mgcg", MGCG);
		Mcursor.init(this, &VDRoom, "models/Sphere.obj", OBJ);



		
		// Create the textures used also for the internal objects

		Troom.init(this, "textures/Textures_Forniture.png");
		Tbed.init(this, "textures/Bed_texture.png");
		TarmChair.init(this, "textures/Armchair_Texture.png");
		Ttable.init(this, "textures/Table_Texture.jpg");
		Tvase.init(this, "textures/Vase_Texture.jpg");
		Tcursor.init(this, "textures/Cursor_Texture.png");
		

		// Descriptor pool sizes
		// WARNING!!!!!!!!
		// Must be set before initializing the text and the scene
		DPSZs.uniformBlocksInPool = 60;
		DPSZs.texturesInPool = 50;
		DPSZs.setsInPool = 50;

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
			for (int i = 0; i < numLightFocus; i++) {
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
				std::cout << LInt[i] << "\n";
			}
			ScosIn = 0.4;
			ScosOut = 0.5;
			
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
		PRoomBackFace.create();
		PRoomRightFace.create();
		PRoomLeftFace.create();
		PRoomBottomFace.create();
		PArmChair.create();
		Pbed.create();
		PTable.create();
		Pkitchen.create();
		Pvase.create();
		Pfridge.create();
		Pmicrowave.create();
		Pball.create();
		Ptea.create();
		Ptoilet.create();
		Pheadphones.create();
		Ptv.create();
		Pcoffee.create();
		Pchair.create();
		Pcamera.create();
		Pcursor.create();


		// init the descriptor sets
		DSRoomRightFace.init(this, &DSLRoomFace, { &Troom });
		DSRoomBackFace.init(this, &DSLRoomFace, { &Troom });
		DSRoomFrontFace.init(this, &DSLRoomFace, {&Troom });
		DSRoomLeftFace.init(this, &DSLRoomFace, {&Troom });
		DSRoomBottomFace.init(this, &DSLRoomFace, {&Troom });
		DSArmchair.init(this, &DSLRoomFace, {&TarmChair});
		DSBed.init(this, &DSLRoomFace, {&Tbed });
		DSTable.init(this, &DSLRoomFace, {&Ttable });
		DSKitchen.init(this, &DSLRoomFace, {&Troom });
		DSvase.init(this, &DSLRoomFace, {&Tvase });
		DSfridge.init(this, &DSLRoomFace, {&Troom });
		DSmicrowave.init(this, &DSLRoomFace, {&Troom });
		DSball.init(this, &DSLRoomFace, {&Troom });
		DStea.init(this, &DSLRoomFace, {&Troom });
		DStoilet.init(this, &DSLRoomFace, { &Troom });
		DSheadphones.init(this, &DSLHeadphones, {&Troom});
		DStv.init(this, &DSLRoomFace, { &Troom });
		DSchair.init(this, &DSLRoomFace, { &Troom });
		DScamera.init(this, &DSLRoomFace, { &Troom });
		DScoffee.init(this, &DSLRoomFace, { &Troom });
		DScursor.init(this, &DSLRoomFace, { &Tcursor });

	
			
		DSGlobal.init(this, &DSLGlobal, {});
		DSGlobalFocus.init(this, &DSLGlobalFocus, {});
		

		txt.pipelinesAndDescriptorSetsInit();		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines

		PRoomFrontFace.cleanup();
		PRoomBackFace.cleanup();
		PRoomRightFace.cleanup();
		PRoomLeftFace.cleanup();
		PRoomBottomFace.cleanup();
		PArmChair.cleanup();
		Pbed.cleanup();
		PTable.cleanup();
		Pkitchen.cleanup();
		Pvase.cleanup();
		Pfridge.cleanup();
		Pmicrowave.cleanup();
		Pball.cleanup();
		Ptea.cleanup();
		Ptoilet.cleanup();
		Pheadphones.cleanup();
		Ptv.cleanup();
		Pcoffee.cleanup();
		Pchair.cleanup();
		Pcamera.cleanup();
		Pcursor.cleanup();





		DSRoomRightFace.cleanup();
		DSRoomBackFace.cleanup();
		DSRoomLeftFace.cleanup();
		DSRoomFrontFace.cleanup();
		DSRoomBottomFace.cleanup();
		DSArmchair.cleanup();
		DSBed.cleanup();
		DSTable.cleanup();
		DSKitchen.cleanup();
		DSvase.cleanup();
		DSfridge.cleanup();
		DSmicrowave.cleanup();
		DSball.cleanup();
		DStea.cleanup();
		DStoilet.cleanup();
		DSheadphones.cleanup();
		DStv.cleanup();
		DScoffee.cleanup();
		DSchair.cleanup();
		DScamera.cleanup();
		DScursor.cleanup();



		DSGlobal.cleanup();
		DSGlobalFocus.cleanup();
		


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
		Ttable.cleanup();
		Tvase.cleanup();
		Tcursor.cleanup();

		// Cleanup models
		MroomFace.cleanup();
		bottomFace.cleanup();
		Marmchair.cleanup();
		Mbed.cleanup();
		Mtable.cleanup();
		Mkitchen.cleanup();
		Mvase.cleanup();
		Mfridge.cleanup();
		Mmicrowave.cleanup();
		Mball.cleanup();
		Mtea.cleanup();
		Mtoilet.cleanup();
		Mheadphones.cleanup();
		Mtv.cleanup();
		Mchair.cleanup();
		Mcoffee.cleanup();
		Mcamera.cleanup();
		Mcursor.cleanup();


		
		// Cleanup descriptor set layouts

		DSGlobal.cleanup();
		DSLRoomFace.cleanup();
		DSLHeadphones.cleanup();
		DSLGlobalFocus.cleanup();
	

		
		// Destroies the pipelines
		PRoomFrontFace.destroy();
		PRoomBackFace.destroy();
		PRoomRightFace.destroy();
		PRoomLeftFace.destroy();
		PRoomBottomFace.destroy();
		PArmChair.destroy();
		Pbed.destroy();
		PTable.destroy();
		Pkitchen.destroy();
		Pvase.destroy();
		Pfridge.destroy();
		Pmicrowave.destroy();
		Pball.destroy();
		Ptea.destroy();
		Ptoilet.destroy();
		Pheadphones.destroy();
		Ptv.destroy();
		Pchair.destroy();
		Pcoffee.destroy();
		Pcamera.destroy();
		Pcursor.destroy();

		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

		if (all || numberObject == 8) {
			Marmchair.bind(commandBuffer);
			PArmChair.bind(commandBuffer);
			DSArmchair.bind(commandBuffer, PArmChair, 1, currentImage);
			DSGlobalFocus.bind(commandBuffer, PArmChair, 0, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Marmchair.indices.size()), 1, 0, 0, 0);
		}

		if (all) {
			Mbed.bind(commandBuffer);
			Pbed.bind(commandBuffer);
			DSGlobal.bind(commandBuffer, Pbed, 0, currentImage);
			DSBed.bind(commandBuffer, Pbed, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mbed.indices.size()), 1, 0, 0, 0);
		}

		PRoomBackFace.bind(commandBuffer);
		MroomFace.bind(commandBuffer);
		DSRoomBackFace.bind(commandBuffer, PRoomBackFace, 1, currentImage);	// The Room Descriptor Set (Set 1)
		DSGlobalFocus.bind(commandBuffer, PRoomBackFace, 0, currentImage);	// The Global Descriptor Set (Set 0)
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MroomFace.indices.size()), 1, 0, 0, 0);

		PRoomFrontFace.bind(commandBuffer);
		MroomFace.bind(commandBuffer);
		DSRoomFrontFace.bind(commandBuffer, PRoomFrontFace, 1, currentImage);	// The Room Descriptor Set (Set 1)
		DSGlobalFocus.bind(commandBuffer, PRoomFrontFace, 0, currentImage);	// The Global Descriptor Set (Set 0)
		vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MroomFace.indices.size()), 1, 0, 0, 0);


	
		PRoomRightFace.bind(commandBuffer);
		DSRoomRightFace.bind(commandBuffer, PRoomRightFace, 1, currentImage);
		DSGlobalFocus.bind(commandBuffer, PRoomRightFace, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MroomFace.indices.size()), 1, 0, 0, 0);



		PRoomLeftFace.bind(commandBuffer);
		DSRoomLeftFace.bind(commandBuffer, PRoomLeftFace, 1, currentImage);
		DSGlobalFocus.bind(commandBuffer, PRoomLeftFace, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MroomFace.indices.size()), 1, 0, 0, 0);





		bottomFace.bind(commandBuffer);
		PRoomBottomFace.bind(commandBuffer);
		DSRoomBottomFace.bind(commandBuffer, PRoomBottomFace, 1, currentImage);
		DSGlobalFocus.bind(commandBuffer, PRoomBottomFace, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(bottomFace.indices.size()), 1, 0, 0, 0);


		if (all) {
			PTable.bind(commandBuffer);
			Mtable.bind(commandBuffer);
			DSGlobal.bind(commandBuffer, PTable, 0, currentImage);
			DSTable.bind(commandBuffer, PTable, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mtable.indices.size()), 1, 0, 0, 0);
		}

		if (all) {
			Pkitchen.bind(commandBuffer);
			Mkitchen.bind(commandBuffer);
			DSGlobal.bind(commandBuffer, Pkitchen, 0, currentImage);
			DSKitchen.bind(commandBuffer, Pkitchen, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mkitchen.indices.size()), 1, 0, 0, 0);
		}

		if (all || numberObject == 9) {
			Pvase.bind(commandBuffer);
			Mvase.bind(commandBuffer);
			DSGlobalFocus.bind(commandBuffer, Pvase, 0, currentImage);
			DSvase.bind(commandBuffer, Pvase, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mvase.indices.size()), 1, 0, 0, 0);
		}

		if (all) {

			Pfridge.bind(commandBuffer);
			Mfridge.bind(commandBuffer);
			DSGlobal.bind(commandBuffer, Pfridge, 0, currentImage);
			DSfridge.bind(commandBuffer, Pfridge, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mfridge.indices.size()), 1, 0, 0, 0);
		}

		if (all || numberObject == 10) {
			Pmicrowave.bind(commandBuffer);
			Mmicrowave.bind(commandBuffer);
			DSGlobalFocus.bind(commandBuffer, Pmicrowave, 0, currentImage);
			DSmicrowave.bind(commandBuffer, Pmicrowave, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mmicrowave.indices.size()), 1, 0, 0, 0);
		}


		if (all || numberObject == 13) {

			Pball.bind(commandBuffer);
			Mball.bind(commandBuffer);
			DSGlobalFocus.bind(commandBuffer, Pball, 0, currentImage);
			DSball.bind(commandBuffer, Pball, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mball.indices.size()), 1, 0, 0, 0);
		}

		if (all || numberObject == 11) {
			Ptea.bind(commandBuffer);
			Mtea.bind(commandBuffer);
			DSGlobalFocus.bind(commandBuffer, Ptea, 0, currentImage);
			DStea.bind(commandBuffer, Ptea, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mtea.indices.size()), 1, 0, 0, 0);
		}

		if (all || numberObject == 17) {
			Ptoilet.bind(commandBuffer);
			Mtoilet.bind(commandBuffer);
			DSGlobal.bind(commandBuffer, Ptoilet, 0, currentImage);
			DStoilet.bind(commandBuffer, Ptoilet, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mtoilet.indices.size()), 1, 0, 0, 0);
		}

		if (all || numberObject == 14) {
			Pheadphones.bind(commandBuffer);
			Mheadphones.bind(commandBuffer);
			DSGlobalFocus.bind(commandBuffer, Pheadphones, 0, currentImage);
			DSheadphones.bind(commandBuffer, Pheadphones, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mheadphones.indices.size()), 1, 0, 0, 0);
		}

		if (all || numberObject == 12) {
			Ptv.bind(commandBuffer);
			Mtv.bind(commandBuffer);
			DSGlobal.bind(commandBuffer, Ptv, 0, currentImage);
			DStv.bind(commandBuffer, Ptv, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mtv.indices.size()), 1, 0, 0, 0);
		}

		if (all) {
			Pchair.bind(commandBuffer);
			Mchair.bind(commandBuffer);
			DSGlobal.bind(commandBuffer, Pchair, 0, currentImage);
			DSchair.bind(commandBuffer, Pchair, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mchair.indices.size()), 1, 0, 0, 0);
		}

		if (all || numberObject == 15) {
			Pcamera.bind(commandBuffer);
			Mcamera.bind(commandBuffer);
			DSGlobalFocus.bind(commandBuffer, Pcamera, 0, currentImage);
			DScamera.bind(commandBuffer, Pcamera, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mcamera.indices.size()), 1, 0, 0, 0);
		}

		if (all || numberObject == 16) {
			Pcoffee.bind(commandBuffer);
			Mcoffee.bind(commandBuffer);
			DSGlobalFocus.bind(commandBuffer, Pcoffee, 0, currentImage);
			DScoffee.bind(commandBuffer, Pcoffee, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mcoffee.indices.size()), 1, 0, 0, 0);
		}

		if(numberObject == -1) {
			Pcursor.bind(commandBuffer);
			Mcursor.bind(commandBuffer);
			DScursor.bind(commandBuffer, Pcursor, 0, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(Mcursor.indices.size()), 1, 0, 0, 0);

			txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
		
		}

		
	}
	// put all the object in a certain initial position in the screen 
	glm::mat4 initialTranslation() {
		return glm::translate(glm::mat4(1), glm::vec3(0, -5, -15));
	}

	// each objects has an invisible HitBox representing its solid state, if two boxes collides (hence you encountered an object) movement should not be allowed
	bool checkCollision(AABB& box1, AABB& box2) {
		return (box1.max.x > box2.min.x &&
			box1.min.x < box2.max.x &&
			box1.max.y > box2.min.y &&
			box1.min.y < box2.max.y &&
			box1.max.z > box2.min.z &&
			box1.min.z < box2.max.z);
	}



	std::vector<float> rotateFocusedObj() {

		std::vector<float> rotation;
		float deltaTime = 0.0f;
		glm::vec3 m2 = glm::vec3(0.0f), r2 = glm::vec3(0.0f);
		bool fire2 = false;

		moveObj(deltaTime, m2, r2, fire2);


		static float autoTime2 = true;
		static float cTime2 = 0.0;
		const float turnTime2 = 36.0f;
		const float angTurnTimeFact2 = 2.0f * M_PI / turnTime2;

		if (autoTime2) {
			cTime2 = cTime2 + deltaTime;
			cTime2 = (cTime2 > turnTime2) ? (cTime2 - turnTime2) : cTime2;
		}
		cTime2 += r2.z * angTurnTimeFact2 * 4.0;

		const float ROT_SPEED2 = glm::radians(120.0f);
		const float MOVE_SPEED2 = 2.0f;

		ObjAlpha = ObjAlpha - ROT_SPEED2 * deltaTime * r2.y;
		ObjBeta = ObjBeta - ROT_SPEED2 * deltaTime * r2.x;

		rotation.push_back(ObjAlpha);
		rotation.push_back(ObjBeta);

		return rotation;
	}


	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;

		float deltaT = 0.0f;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;

		//if (numberObject == -1) {
			getSixAxis(deltaT, m, r, fire);
		//}


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

		// Movement in the camera's local space
		glm::vec3 moveRight = m.x * ux * MOVE_SPEED * deltaT;
		glm::vec3 moveUp = m.y * glm::vec3(0, 1, 0) * MOVE_SPEED * deltaT;
		glm::vec3 moveForward = m.z * uz * MOVE_SPEED * deltaT;
		// Combine movement vectors
		glm::vec3 potentialMove = moveRight + moveUp + moveForward;
		glm::vec3 newCamPos = CamPos + potentialMove;	// Temp variable to check whether the potential movement we are trying to input results in a collision
		
		/* DA REVISIONARE : dal momento che non dovremmo poterci muovere su asse y(dato che siamo in modalità walk)
		rimuovere la possibilità di alzarsi e abbassarsi sulla cordinata y e rimuovere il moveUp da potentialMove*/

		// Hit boxes of objects in the scene
		std::vector<AABB> objectsHitBox = {
		  AABB(glm::vec3(-1.37f, -5.0f, -11.7f), glm::vec3(1.65f, -3.65f, -10.0f),table), //table object0
		  AABB(glm::vec3(-0.5f, -5.0f, -19.7f), glm::vec3(2.5f, -3.7f, -15.5f),bed),  //bed object1
		  AABB(glm::vec3(-4.5f, -5.0f, -8.5f), glm::vec3(-3.5f, -4.0f, -6.5f), ball),//ball object13
		  AABB(glm::vec3(-10.0f, -10.0f, -20.0f), glm::vec3(10.0f, 10.0f, -18.8f),frontWall),   //frontWall object2
		  AABB(glm::vec3(-7.2f, -10.0f, -19.0f), glm::vec3(-5.8f, 10.0f, -7.0f),leftWall),  //leftWall object3
		  AABB(glm::vec3(5.8f, -10.0f, -19.0f), glm::vec3(7.2f, 10.0f, -7.0f),rightWall),   //rightWall object4
		  AABB(glm::vec3(4.0f, -5.0f, -13.8f), glm::vec3(6.0f, -3.0f, -12.0f),armchair), //armchair object8
		  AABB(glm::vec3(-2.1f, -5.0f, -8.0f), glm::vec3(2.0f, -2.3f, -6.5f), tv),//tv object12
		  AABB(glm::vec3(-1.2f, -3.7f, -10.9f), glm::vec3(-0.5f, -3.4f, -10.4f),camera),//camera object15t
		  AABB(glm::vec3(1.0f, -3.7f, -10.7f), glm::vec3(1.4f, -3.4f, -10.2f),headphones), //headphones 
		  AABB(glm::vec3(0.0f, -3.7f, -11.1f), glm::vec3(0.5f, -3.0f, -10.7f),vase), //vase object9
		  AABB(glm::vec3(-7.0f, -5.0f, -14.0f), glm::vec3(-5.0f, -2.0f, -12.5f),fridge),//fridge object5
		  AABB(glm::vec3(-7.0f, -5.0f, -19.8f), glm::vec3(-5.0f, -2.0f, -14.0f),kitchenLeftWall), //kitchen left side wall object6
		  AABB(glm::vec3(-7.0f, -5.0f, -19.8f), glm::vec3(0.0f, -2.0f, -17.9f),kitchenFrontWall),  //kitchen front side wall object7
		  AABB(glm::vec3(-7.0f, -3.7f, -16.6f), glm::vec3(-5.0f, -2.7f, -15.5f),microwave), //microwave object10
		  AABB(glm::vec3(-7.0f, -3.7f, -18.2f), glm::vec3(-5.0f, -2.7f, -17.7f),tea), //tea object11
		  AABB(glm::vec3(-2.0f, -3.7f, -19.8f), glm::vec3(-1.0f, -2.7f, -17.9f),coffee), //coffe machine
		  AABB(glm::vec3(1.6f, -5.0f, -19.8f), glm::vec3(4.4f, -3.0f, -17.3f),toilet), //toilet object17
				};

		std::vector<AABB> objectsHitBoxFocus = {
			 AABB(glm::vec3(4.0f, -5.0f, -13.8f), glm::vec3(6.0f, -3.0f, -12.0f),armchair), //armchair object8
			 AABB(glm::vec3(-0.3f, -3.6f, -11.0f), glm::vec3(0.3f, -3.0f, -10.5f),vase), //vase object9
			 AABB(glm::vec3(-5.8f, -4.0f, -16.3f), glm::vec3(-5.2f, -3.4f, -15.7f),microwave), //microwave object 10
			 AABB(glm::vec3(-5.7f, -3.9f, -18.2f), glm::vec3(-5.3f, -3.5f, -17.7f),tea), //tea object11
			 AABB(glm::vec3(-4.5f, -5.0f, -8.5f), glm::vec3(-3.5f, -4.0f, -6.5f), ball), //ball object13
			  AABB(glm::vec3(1.0f, -3.7f, -10.7f), glm::vec3(1.4f, -3.4f, -10.2f),headphones), //headphones  
			 AABB(glm::vec3(-1.2f, -3.7f, -10.8f), glm::vec3(-0.5f, -3.4f, -10.4f),camera),//camera object15
			AABB(glm::vec3(-1.7f, -10.0f, -18.5f), glm::vec3(-1.2f, 10.0f, -17.8f),coffee), //coffe machine

		};


		//[CAMBIA TUTTE LE y CON -10 E 10 UNA VOLTA MODELLATE TUTTE LE HITBOX]
	
		// Hit box of the camera and of the new camera (after the potential move)
		AABB camHitBox(glm::vec3(CamPos.x -0.1, -10, CamPos.z-0.1), glm::vec3(CamPos.x+0.1, 10, CamPos.z+0.1), camHitBoxEnum);
		AABB newcamHitBox(glm::vec3(newCamPos.x-0.1, -10, newCamPos.z-0.1), glm::vec3(newCamPos.x+0.1, 10, newCamPos.z+0.1), camHitBoxEnum);

		bool collisionDetected = false;
		for (AABB& object : objectsHitBox) {
			if (checkCollision(newcamHitBox, object)) {
				collisionDetected = true;
				break;
			}
		}

		if (!collisionDetected) {
			CamPos = newCamPos;
		}
		else {
			// Handle axis-specific collision
			bool allowX = true, allowY = true, allowZ = true;

			// Check X-axis (sideways) movement
			glm::vec3 tempPosX = CamPos + moveRight;
			AABB tempHitBoxX(glm::vec3(tempPosX.x - 0.1, -10, tempPosX.z - 0.1), glm::vec3(tempPosX.x + 0.1, 10, tempPosX.z + 0.1), camHitBoxEnum);
			for (AABB& object : objectsHitBox) {
				if (checkCollision(tempHitBoxX, object)) {
					allowX = false;
					break;
				}
			}

			// Check Y-axis (up/down) movement
			glm::vec3 tempPosY = CamPos + moveUp;
			AABB tempHitBoxY(glm::vec3(tempPosY.x - 0.1,-10, tempPosY.z - 0.1), glm::vec3(tempPosY.x + 0.1, 10, tempPosY.z + 0.1), camHitBoxEnum);
			for (AABB& object : objectsHitBox) {
				if (checkCollision(tempHitBoxY, object)) {
					allowY = false;
					break;
				}
			}

			// Check Z-axis (forward/backward) movement
			glm::vec3 tempPosZ = CamPos + moveForward;
			AABB tempHitBoxZ(glm::vec3(tempPosZ.x - 0.1, -10, tempPosZ.z - 0.1), glm::vec3(tempPosZ.x + 0.1, 10, tempPosZ.z + 0.1), camHitBoxEnum);
			for (AABB& object : objectsHitBox) {
				if (checkCollision(tempHitBoxZ, object)) {
					allowZ = false;
					break;
				}
			}

			// Apply allowed movement only after checking all objects
			if (allowX) CamPos += moveRight;
			if (allowY) CamPos += moveUp;
			if (allowZ) CamPos += moveForward;
		}

		static float subpassTimer = 0.0;

		/*if (glfwGetKey(window, GLFW_KEY_SPACE)) {
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
		*/
		// Standard procedure to quit when the ESC key is pressed
	


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
		//gubo.cosIn = ScosIn;
		//gubo.cosOut = ScosOut;
		for (int i = 0; i < numLight; i++) {
			gubo.lightColor[i] = glm::vec4(LCol[i], LInt[i]);
			gubo.lightDir[i] = LWm[i] * glm::vec4(0, 0, 1, 0);// direction taken by multiplying the vector (0,0,1) by the light world matrix, we just need the Z component direction
			gubo.lightPos[i] = LWm[i] * glm::vec4(0, 0, 0, 1); //position taken by multiplying the vector (0,0,0) by the light world matrix, we just need the the last component of the matrix that indicates the position in the space where the light it has been put
		}
		DSGlobal.map(currentImage, &gubo, 0);


		GlobalUniformBufferObjectFocus focus{};
		focus.eyePos = CamPos;
		focus.lightOn = lightOn;
		focus.cosIn = ScosIn;
		focus.cosOut = ScosOut;
		for (int i = 0; i < numLightFocus; i++) {
			focus.lightColor[i] = glm::vec4(LCol[i], LInt[i]);
			focus.lightDir[i] = LWm[i] * glm::vec4(0, 0, 1, 0);// direction taken by multiplying the vector (0,0,1) by the light world matrix, we just need the Z component direction
			focus.lightPos[i] = LWm[i] * glm::vec4(0, 0, 0, 1); //position taken by multiplying the vector (0,0,0) by the light world matrix, we just need the the last component of the matrix that indicates the position in the space where the light it has been put
		}
		DSGlobalFocus.map(currentImage, &focus, 0);

		// objects
		//ROOM FRONT FACE
		RoomUniformBufferObject roomFrontFaceUbo{};
		if (numberObject != -1) {
			roomFrontFaceUbo.selected = 1;
		}
		roomFrontFaceUbo.mMat= glm::translate(glm::mat4(1), glm::vec3(0, 0, -4)) * initialTranslation() * glm::scale(glm::mat4(1), glm::vec3(3, 1, 1)) * baseTr;
		roomFrontFaceUbo.mvpMat = ViewPrj * roomFrontFaceUbo.mMat;
		roomFrontFaceUbo.nMat = glm::inverse(glm::transpose(roomFrontFaceUbo.mMat));
		//roomRightFaceUbo.mvpMat = ViewPrj * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) * baseTr;
		//DSRoomRightFace.map(currentImage, &roomRightFaceUbo, 0);
		DSRoomFrontFace.map(currentImage, &roomFrontFaceUbo, 0);

		//ROOM BACK FACE
		RoomUniformBufferObject roomBackFaceUbo{};
		if (numberObject != -1) {
			roomBackFaceUbo.selected = 1;
		}
		roomBackFaceUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0, 0, +8))*initialTranslation()*glm::scale(glm::mat4(1), glm::vec3(3, 1, 1)) * baseTr;
		roomBackFaceUbo.mvpMat = ViewPrj * roomBackFaceUbo.mMat;
		roomBackFaceUbo.nMat = glm::inverse(glm::transpose(roomBackFaceUbo.mMat));
		DSRoomBackFace.map(currentImage, &roomBackFaceUbo, 0);

		//ROOM RIGHT FACE
		RoomUniformBufferObject roomRightFaceUbo{};
		if (numberObject != -1) {
			roomRightFaceUbo.selected = 1;
		}
		roomRightFaceUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(6, 0, 2)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(3, 1, 1)) * baseTr;
		roomRightFaceUbo.mvpMat = ViewPrj* roomRightFaceUbo.mMat;
		roomRightFaceUbo.nMat = glm::inverse(glm::transpose(roomRightFaceUbo.mMat));
		DSRoomRightFace.map(currentImage, &roomRightFaceUbo, 0);

		//ROOM LEFT FACE
		RoomUniformBufferObject roomLeftFaceUbo{};
		if (numberObject != -1) {
			roomLeftFaceUbo.selected = 1;
		}
		roomLeftFaceUbo.mMat=glm::translate(glm::mat4(1), glm::vec3(-6, 0, 2))* initialTranslation()* glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0))* glm::scale(glm::mat4(1), glm::vec3(3, 1, 1))* baseTr;
		roomLeftFaceUbo.mvpMat = ViewPrj * roomLeftFaceUbo.mMat;
		roomLeftFaceUbo.nMat = glm::inverse(glm::transpose(roomLeftFaceUbo.mMat));

		DSRoomLeftFace.map(currentImage, &roomLeftFaceUbo, 0);
		//ROOM BOTTON FACE
		RoomUniformBufferObject roomBottomFaceUbo{};
		roomBottomFaceUbo.selected = 0;

		if (numberObject != -1) {
			roomBottomFaceUbo.selected = 1;
		}
		roomBottomFaceUbo.mMat= glm::translate(glm::mat4(1), glm::vec3(0, 0, +2)) * initialTranslation() * glm::scale(glm::mat4(1), glm::vec3(3, 1, 3)) * baseTr;
		//roomBottomFaceUbo.mvpMat = ViewPrj * glm::translate(glm::mat4(1), glm::vec3(0, 0, -4)) *initialTranslation()* glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::mat4(1), glm::vec3(3, 3, 1)) * baseTr;
		roomBottomFaceUbo.mvpMat = ViewPrj * roomBottomFaceUbo.mMat;
		roomBottomFaceUbo.nMat = glm::inverse(glm::transpose(roomBottomFaceUbo.mMat));

		DSRoomBottomFace.map(currentImage, &roomBottomFaceUbo, 0);

		//armchair
		armchairUniformBufferObject armchairUbo{};
		armchairUbo.selected = 0;
		if (numberObject == 8) {
			std::vector<float> rotation = rotateFocusedObj();
			armchairUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0, 2, 1)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), rotation[1], glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), rotation[0], glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(1, 1, 1)) * baseTr;
			armchairUbo.selected = 1;
		}
		else {
			armchairUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(5, 0.35, 2)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(0.7, 0.7, 0.7)) * baseTr;
		}
		armchairUbo.mvpMat = ViewPrj * armchairUbo.mMat;
		armchairUbo.nMat = glm::inverse(glm::transpose(armchairUbo.mMat));
		DSArmchair.map(currentImage, &armchairUbo, 0);

		//bed
		bedUniformBufferObject bedUbo{};
		bedUbo.selected = 0;
		bedUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(1, -0.07, -2.7)) * initialTranslation() * baseTr;
		bedUbo.mvpMat = ViewPrj * bedUbo.mMat;
		bedUbo.nMat = glm::inverse(glm::transpose(bedUbo.mMat));
		DSBed.map(currentImage, &bedUbo, 0);

		//table
		RoomUniformBufferObject tableUbo{};
		tableUbo.selected = 0;
		tableUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0, 0.4, +4)) * initialTranslation() * glm::scale(glm::mat4(1), glm::vec3(0.8, 0.8, 0.8)) * baseTr;
		tableUbo.mvpMat = ViewPrj * tableUbo.mMat;
		tableUbo.nMat = glm::inverse(glm::transpose(tableUbo.mMat));
		DSTable.map(currentImage, &tableUbo, 0);

		//kitchen
		RoomUniformBufferObject kitchenUbo{};
		kitchenUbo.selected = 0;
		kitchenUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(-5.8, 0, -3.7))*initialTranslation() * glm::scale(glm::mat4(1), glm::vec3(1.5, 1.5, 1.5))*baseTr;
		kitchenUbo.mvpMat = ViewPrj * kitchenUbo.mMat;
		kitchenUbo.nMat = glm::inverse(glm::transpose(kitchenUbo.mMat));
		DSKitchen.map(currentImage, &kitchenUbo, 0);

		//vase
		RoomUniformBufferObject vaseUbo{};
		vaseUbo.selected = 0;
		if (numberObject == 9) {
			std::vector<float> rotation = rotateFocusedObj();
			vaseUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(-0.5, 2, 1)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), rotation[1], glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), rotation[0], glm::vec3(0, 1, 0)) *glm::scale(glm::mat4(1), glm::vec3(2, 2, 2)) * baseTr;
			vaseUbo.selected = 1;
		}
		else {
			vaseUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0, 1.7, 4)) * initialTranslation() * baseTr;
		}
		vaseUbo.mvpMat = ViewPrj * vaseUbo.mMat;
		vaseUbo.nMat = glm::inverse(glm::transpose(vaseUbo.mMat));
		DSvase.map(currentImage, &vaseUbo, 0);

		//fridge
		RoomUniformBufferObject fridgeUbo{};
		fridgeUbo.selected = 0;
		fridgeUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(-5.5, 0, 1.8)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(1.5, 1.5, 1.5)) * baseTr;
		fridgeUbo.mvpMat = ViewPrj * fridgeUbo.mMat;
		fridgeUbo.nMat = glm::inverse(glm::transpose(fridgeUbo.mMat));
		DSfridge.map(currentImage, &fridgeUbo, 0);

		//microwave
		RoomUniformBufferObject microwaveUbo{};
		microwaveUbo.selected = 0;
		if (numberObject == 10) {
			std::vector<float> rotation = rotateFocusedObj();
			microwaveUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0, 2, 1)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), rotation[1], glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), rotation[0], glm::vec3(0, 1, 0)) *glm::scale(glm::mat4(1), glm::vec3(5, 5, 5)) * baseTr;
			microwaveUbo.selected = 1;
		}
		else {
			microwaveUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(-5.5, 1.3, -1)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(1.8, 1.8, 1.8)) * baseTr;
		}
		microwaveUbo.mvpMat = ViewPrj * microwaveUbo.mMat;
		microwaveUbo.nMat = glm::inverse(glm::transpose(microwaveUbo.mMat));
		DSmicrowave.map(currentImage, &microwaveUbo, 0);

		//ball
		RoomUniformBufferObject ballUbo{};
		ballUbo.selected = 0;
		if (numberObject == 13) {
			std::vector<float> rotation = rotateFocusedObj();
			ballUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0, 2, 1)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), rotation[1], glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), rotation[0], glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(3, 3, 3)) * baseTr;
			ballUbo.selected = 1;
		}
		else {
			ballUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(-4, 0.4, 7)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(1, 1, 1)) * baseTr;
		}
		ballUbo.mvpMat = ViewPrj * ballUbo.mMat;
		ballUbo.nMat = glm::inverse(glm::transpose(ballUbo.mMat));
		DSball.map(currentImage, &ballUbo, 0);

		//tea
		RoomUniformBufferObject teaUbo{};
		teaUbo.selected = 0;
		if (numberObject == 11) {
			std::vector<float> rotation = rotateFocusedObj();
			teaUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0, 2, 1)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), rotation[1], glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), rotation[0], glm::vec3(0, 1, 0)) *glm::scale(glm::mat4(1), glm::vec3(6, 6, 6)) * baseTr;
			teaUbo.selected = 1;
		}
		else {
			teaUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(-5.5, 1.3, -3)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(2, 2, 2)) * baseTr;
		}
		teaUbo.mvpMat = ViewPrj * teaUbo.mMat;
		teaUbo.nMat = glm::inverse(glm::transpose(teaUbo.mMat));
		DStea.map(currentImage, &teaUbo, 0);

		//toilet
		RoomUniformBufferObject toiletUbo{};
		toiletUbo.selected = 0;
		toiletUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(4.0f, 0.0f, -3.6f)) * initialTranslation() * glm::scale(glm::mat4(1), glm::vec3(2, 2, 2)) * baseTr;
		toiletUbo.mvpMat = ViewPrj * toiletUbo.mMat;
		toiletUbo.nMat = glm::inverse(glm::transpose(toiletUbo.mMat));
		DStoilet.map(currentImage, &toiletUbo, 0);

		//headphones
		RoomUniformBufferObject headphoneUbo{};
		headphoneUbo.selected = 0;
		if (numberObject == 14) {
			std::vector<float> rotation = rotateFocusedObj();
			headphoneUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0, 2, 1)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), rotation[1], glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), rotation[0], glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(6, 6, 6)) * baseTr;
			headphoneUbo.selected = 1;
		}
		else {
			headphoneUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(1.2f, 1.365f, 4.5f)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-13.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::mat4(1), glm::vec3(2, 2, 2)) * baseTr;
		}
		headphoneUbo.mvpMat = ViewPrj * headphoneUbo.mMat;
		headphoneUbo.nMat = glm::inverse(glm::transpose(headphoneUbo.mMat));
		DSheadphones.map(currentImage, &headphoneUbo, 0);
		HeadphonesParUniformBufferObject headphoneParUbo{};
		headphoneParUbo.Pow = 200.0f;
		DSheadphones.map(currentImage, &headphoneParUbo, 2);

		//tv
		RoomUniformBufferObject tvUbo{};
		tvUbo.selected = 0;
		tvUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, 7.5f)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(1.5, 1.5, 1.5)) * baseTr;
		tvUbo.mvpMat = ViewPrj * tvUbo.mMat;
		tvUbo.nMat = glm::inverse(glm::transpose(tvUbo.mMat));
		DStv.map(currentImage, &tvUbo, 0);

		//chair
		RoomUniformBufferObject chairUbo{};
		chairUbo.selected = 0;
		chairUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0, 0.0, 3.5)) * initialTranslation() * glm::scale(glm::mat4(1), glm::vec3(1.7, 1.7, 1.7)) * baseTr;
		chairUbo.mvpMat = ViewPrj * chairUbo.mMat;
		chairUbo.nMat = glm::inverse(glm::transpose(chairUbo.mMat));
		DSchair.map(currentImage, &chairUbo, 0);

		//camera
		RoomUniformBufferObject cameraUbo{};
		cameraUbo.selected = 0;
		if (numberObject == 15) {
			std::vector<float> rotation = rotateFocusedObj();
			cameraUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0, 2, 1)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), rotation[1], glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), rotation[0], glm::vec3(0, 1, 0)) *glm::scale(glm::mat4(1), glm::vec3(6, 6, 6)) * baseTr;
			cameraUbo.selected = 1;
		}
		else {
			cameraUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(-0.8, 1.3, 4.3)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(2.5, 2.5, 2.5)) * baseTr;
		}
		cameraUbo.mvpMat = ViewPrj * cameraUbo.mMat;
		cameraUbo.nMat = glm::inverse(glm::transpose(cameraUbo.mMat));
		DScamera.map(currentImage, &cameraUbo, 0);

		//coffee
		RoomUniformBufferObject coffeeUbo{};
		coffeeUbo.selected = 0;
		if (numberObject == 16) {
			std::vector<float> rotation = rotateFocusedObj();
			coffeeUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(0, 2, 1)) * initialTranslation() * glm::rotate(glm::mat4(1.0f), rotation[1], glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), rotation[0], glm::vec3(0, 1, 0)) *glm::scale(glm::mat4(1), glm::vec3(6, 6, 6)) * baseTr;
			coffeeUbo.selected = 1;
		}
		else {
			coffeeUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(-1.5f, 1.3f, -3.3f)) * initialTranslation() * glm::scale(glm::mat4(1), glm::vec3(2.5, 2.5, 2.5)) * baseTr;
		}
		coffeeUbo.mvpMat = ViewPrj * coffeeUbo.mMat;
		coffeeUbo.nMat = glm::inverse(glm::transpose(coffeeUbo.mMat));
		DScoffee.map(currentImage, &coffeeUbo, 0);

		//cursor
		RoomUniformBufferObject cursorUbo{};
		cursorUbo.selected = 0;
		//cameraForward: Calcoli la direzione "frontale" della telecamera estraendo la terza colonna della matrice di vista (Mv),
		// e invertendo il segno. Questo ti dà la direzione in cui la telecamera sta guardando nello spazio mondo.
		glm::vec3 cameraForward = -glm::vec3(Mv[0][2], Mv[1][2], Mv[2][2]);
		glm::vec3 objectPosition = CamPos + cameraForward * 2.0f;
		cursorUbo.mMat = glm::translate(glm::mat4(1.0f), objectPosition) * glm::scale(glm::mat4(1), glm::vec3(0.01, 0.01, 0.01)) * baseTr;

		//cursorUbo.mMat = glm::translate(glm::mat4(1), glm::vec3(CamPos.x, CamPos.y, CamPos.z - 2)) * glm::scale(glm::mat4(1), glm::vec3(0.01, 0.01, 0.01)) * baseTr;
		cursorUbo.mvpMat = ViewPrj  *cursorUbo.mMat;
		cursorUbo.nMat = glm::inverse(glm::transpose(cursorUbo.mMat));
		DScursor.map(currentImage, &cursorUbo, 0);



		//code for object focus (intersection)
		//hit box cursor
		AABB cursorHitBox(glm::vec3(objectPosition.x-0.2f, objectPosition.y-0.2f, objectPosition.z + 0.2f), glm::vec3(objectPosition.x + 0.2f, objectPosition.y + 0.2f, objectPosition.z + 0.2f), cursor);
		//check collission with all the object hitbox and camera
		for (AABB& object : objectsHitBoxFocus) {
			if (checkCollision(cursorHitBox, object)) {
				//if there is a collision, the object is focused
				//print the object name with which the camera is colliding
				//std::cout << "Object focused: " << object.object << "\n";

				//if the user has clicked SPACE, the object is selected and the camera is moved to the object by seeing only the object with a zoom

				if (glfwGetKey(window, GLFW_KEY_SPACE)) {
					if (!debounce) {
						//debounce = true;
						curDebounce = GLFW_KEY_SPACE;
						numberObject = object.object;
						all = false;
						oldCamPos = CamPos;
						oldCamAlpha = CamAlpha;
						oldCamBeta = CamBeta;

						CamPos = glm::vec3(0.0, -2.5, -7);//cam position
						CamAlpha = 0.0f;//cam rotation
						CamBeta = 0.0f;//cam rotation
						lightOn = glm::vec4(0, 0,0, 0);
						std::cout << "Object selected: " << object.object << "\n";

						RebuildPipeline();

					}
				}

				else {
					if ((curDebounce == GLFW_KEY_SPACE) && debounce) {
						debounce = false;
						curDebounce = 0;
					}
				}


				break;
			}
		}

		//Press F8 to reset the camera to the initial position

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			if (!debounce) {
				//debounce = true;
				curDebounce = GLFW_KEY_ESCAPE;
				all = true;
				numberObject = -1;
				std::cout << "PREMUTO ESCAPE: \n";

				CamPos = oldCamPos;
				CamAlpha = oldCamAlpha;
				CamBeta = oldCamBeta;

				ObjAlpha = 0.0f;
				ObjBeta = 0.0f;
				lightOn = glm::vec4(1.0, 1.0, 1.0, 1.0);

				RebuildPipeline();

			}
		}
		else {
			if ((curDebounce == GLFW_KEY_ESCAPE) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
		
		
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
