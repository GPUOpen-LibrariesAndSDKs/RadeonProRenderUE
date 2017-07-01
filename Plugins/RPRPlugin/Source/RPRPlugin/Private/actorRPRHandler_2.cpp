// Fill out your copyright notice in the Description page of Project Settings.

#include "actorRPRHandler_2.h"



//---RPR example data------------------------------------

// Number of iterations for rendering
int const NUM_ITERATIONS = 16;

// Structure to describe vertex layout
struct vertex
{
    rpr_float pos[3];
    rpr_float norm[3];
    rpr_float tex[2];
};

// Cube geometry
vertex cube_data[] =
{
    { -1.0f, 1.0f, -1.0f, 0.f, 1.f, 0.f, 0.f, 0.f },
    {  1.0f, 1.0f, -1.0f, 0.f, 1.f, 0.f, 0.f, 0.f },
    {  1.0f, 1.0f, 1.0f , 0.f, 1.f, 0.f, 0.f, 0.f },
    {  -1.0f, 1.0f, 1.0f , 0.f, 1.f, 0.f, 0.f, 0.f},

    {  -1.0f, -1.0f, -1.0f , 0.f, -1.f, 0.f, 0.f, 0.f },
    {  1.0f, -1.0f, -1.0f , 0.f, -1.f, 0.f, 0.f, 0.f },
    {  1.0f, -1.0f, 1.0f , 0.f, -1.f, 0.f, 0.f, 0.f },
    {  -1.0f, -1.0f, 1.0f , 0.f, -1.f, 0.f, 0.f, 0.f },

    {  -1.0f, -1.0f, 1.0f , -1.f, 0.f, 0.f, 0.f, 0.f },
    {  -1.0f, -1.0f, -1.0f , -1.f, 0.f, 0.f, 0.f, 0.f },
    {  -1.0f, 1.0f, -1.0f , -1.f, 0.f, 0.f, 0.f, 0.f },
    {  -1.0f, 1.0f, 1.0f , -1.f, 0.f, 0.f, 0.f, 0.f },

    {  1.0f, -1.0f, 1.0f ,  1.f, 0.f, 0.f, 0.f, 0.f },
    {  1.0f, -1.0f, -1.0f ,  1.f, 0.f, 0.f, 0.f, 0.f },
    {  1.0f, 1.0f, -1.0f ,  1.f, 0.f, 0.f, 0.f, 0.f },
    {  1.0f, 1.0f, 1.0f ,  1.f, 0.f, 0.f, 0.f, 0.f },

    {  -1.0f, -1.0f, -1.0f ,  0.f, 0.f, -1.f , 0.f, 0.f },
    {  1.0f, -1.0f, -1.0f ,  0.f, 0.f, -1.f , 0.f, 0.f },
    {  1.0f, 1.0f, -1.0f ,  0.f, 0.f, -1.f, 0.f, 0.f },
    {  -1.0f, 1.0f, -1.0f ,  0.f, 0.f, -1.f, 0.f, 0.f },

    {  -1.0f, -1.0f, 1.0f , 0.f, 0.f, 1.f, 0.f, 0.f },
    {  1.0f, -1.0f, 1.0f , 0.f, 0.f,  1.f, 0.f, 0.f },
    {  1.0f, 1.0f, 1.0f , 0.f, 0.f, 1.f, 0.f, 0.f },
    {  -1.0f, 1.0f, 1.0f , 0.f, 0.f, 1.f, 0.f, 0.f },
};

// Plane geometry
vertex plane_data[] =
{
    {-15.f, 0.f, -15.f, 0.f, 1.f, 0.f, 0.f, 0.f},
    {-15.f, 0.f,  15.f, 0.f, 1.f, 0.f, 0.f, 1.f},
    { 15.f, 0.f,  15.f, 0.f, 1.f, 0.f, 1.f, 1.f},
    { 15.f, 0.f, -15.f, 0.f, 1.f, 0.f, 1.f, 0.f},
};

// Cube indices
rpr_int indices[] =
{
    3,1,0,
    2,1,3,

    6,4,5,
    7,4,6,

    11,9,8,
    10,9,11,

    14,12,13,
    15,12,14,

    19,17,16,
    18,17,19,

    22,20,21,
    23,20,22
};

// Number of vertices per face
rpr_int num_face_vertices[] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};


//#define CHECK(x) status = x; assert(status == RPR_SUCCESS);
#define CHECK(x) status = x; UE_LOG ( LogTemp, Warning, TEXT ( "RPR Check () : %d ." ), status );

//-------------------------------------------------------


// Sets default values
AactorRPRHandler_2::AactorRPRHandler_2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AactorRPRHandler_2::BeginPlay()
{
	Super::BeginPlay();
	
	// Test
	
}

// Called every frame
void AactorRPRHandler_2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AactorRPRHandler_2 :: Init () {
  UE_LOG ( LogTemp, Warning, TEXT ( "RPR : Initializing ." ) );


}

void AactorRPRHandler_2 :: RenderTestScene () {
  UE_LOG ( LogTemp, Warning, TEXT ( "RPR : Starting to render test scene." ) );
  


  rpr_int          status = RPR_SUCCESS;
	// Create OpenCL context using a single GPU
	rpr_context context = NULL;

	// Register Tahoe ray tracing plugin.
	rpr_int tahoePluginID = rprRegisterPlugin("Tahoe64.dll");
	//assert(tahoePluginID != -1);
	if ( tahoePluginID == -1 ) {
		UE_LOG ( LogTemp, Warning, TEXT ( "RPR () : tahoePluginID == -1 ." ) );
		return;

	} //-if
	
	
	
	rpr_int plugins[] = { tahoePluginID };
	size_t pluginCount = sizeof(plugins) / sizeof(plugins[0]);

	// Create context using a single GPU
	//CHECK( rprCreateContext(RPR_API_VERSION, plugins, pluginCount, RPR_CREATION_FLAGS_ENABLE_GPU0, NULL, NULL, &context) );
	rprCreateContext(RPR_API_VERSION, plugins, pluginCount, RPR_CREATION_FLAGS_ENABLE_GPU0, NULL, NULL, &context );
	if (status != RPR_SUCCESS) {
		UE_LOG ( LogTemp, Warning, TEXT ( "RPR () : Context was not created." ) );
		return;

	} //-if

	// Set active plugin.
	//CHECK(  rprContextSetActivePlugin(context, plugins[0]) );
	rprContextSetActivePlugin(context, plugins[0] );


	rpr_material_system matsys;
	//CHECK( rprContextCreateMaterialSystem(context, 0, &matsys) );
	status                                 = rprContextCreateMaterialSystem(context, 0, &matsys );
	// Check if it is created successfully
	if (status != RPR_SUCCESS)
	{
		//std::cout << "Context creation failed: check your OpenCL runtime and driver versions.\n";
		//return -1;
		
		UE_LOG ( LogTemp, Warning, TEXT ( "RPR () : Context creation failed: check your OpenCL runtime and driver versions." ) );
		
		return;
		
	} //-if

	//std::cout << "Context successfully created.\n";
	UE_LOG ( LogTemp, Warning, TEXT ( "RPR () : Context successfully created." ) );

	// Create a scene
	rpr_scene scene;
	CHECK( rprContextCreateScene(context, &scene) );
	//rprContextCreateScene(context, &scene );

    // Create cube mesh
	rpr_shape cube;
	CHECK( rprContextCreateMesh(context,
        (rpr_float const*)&cube_data[0], 24, sizeof(vertex),
        (rpr_float const*)((char*)&cube_data[0] + sizeof(rpr_float)*3), 24, sizeof(vertex),
        (rpr_float const*)((char*)&cube_data[0] + sizeof(rpr_float)*6), 24, sizeof(vertex),
        (rpr_int const*)indices, sizeof(rpr_int),
        (rpr_int const*)indices, sizeof(rpr_int),
        (rpr_int const*)indices, sizeof(rpr_int),
        num_face_vertices, 12, &cube) );

    // Create plane mesh
	rpr_shape plane;
	CHECK( rprContextCreateMesh(context,
        (rpr_float const*)&plane_data[0], 4, sizeof(vertex),
        (rpr_float const*)((char*)&plane_data[0] + sizeof(rpr_float)*3), 4, sizeof(vertex),
        (rpr_float const*)((char*)&plane_data[0] + sizeof(rpr_float)*6), 4, sizeof(vertex),
        (rpr_int const*)indices, sizeof(rpr_int),
        (rpr_int const*)indices, sizeof(rpr_int),
        (rpr_int const*)indices, sizeof(rpr_int),
        num_face_vertices, 2, &plane) );

    // Add cube into the scene
    CHECK( rprSceneAttachShape(scene, cube) );
    //rprSceneAttachShape(scene, cube );

    // Create a transform: -2 unit along X axis and 1 unit up Y axis
    RadeonProRender::matrix m = RadeonProRender::translation(RadeonProRender::float3(-2, 1, 0));

    // Set the transform
    CHECK( rprShapeSetTransform(cube, RPR_TRUE, &m.m00) );
    //rprShapeSetTransform(cube, RPR_TRUE, &m.m00 );

    // Add plane into the scene
    CHECK( rprSceneAttachShape(scene, plane) );
    //rprSceneAttachShape(scene, plane );

    // Create camera
    rpr_camera camera;
    CHECK( rprContextCreateCamera(context, &camera) );
    //rprContextCreateCamera ( context, &camera );

    // Position camera in world space:
    // Camera position is (5,5,20)
    // Camera aimed at (0,0,0)
    // Camera up vector is (0,1,0)
    CHECK( rprCameraLookAt(camera, 5, 5, 20, 0, 0, 0, 0, 1, 0) );

	CHECK( rprCameraSetFocalLength(camera, 75.f) );

    // Set camera for the scene
    CHECK( rprSceneSetCamera(scene, camera) );

    // Set scene to render for the context
    CHECK( rprContextSetScene(context, scene) );

    // Create simple diffuse shader
	rpr_material_node diffuse;
	CHECK( rprMaterialSystemCreateNode(matsys, RPR_MATERIAL_NODE_DIFFUSE, &diffuse) );

    // Set diffuse color parameter to gray
    CHECK( rprMaterialNodeSetInputF(diffuse, "color", 0.5f, 0.5f, 0.5f, 1.f) );

    // Set shader for cube & plane meshes
    CHECK( rprShapeSetMaterial(cube, diffuse) );

    CHECK( rprShapeSetMaterial(plane, diffuse) );

    // Create point light
	rpr_light light;
	CHECK( rprContextCreatePointLight(context, &light) );

    // Create a transform: move 5 units in X axis, 8 units up Y axis, -2 units in Z axis
    RadeonProRender::matrix lightm = RadeonProRender::translation(RadeonProRender::float3(5,8,-2));

    // Set transform for the light
    CHECK( rprLightSetTransform(light, RPR_TRUE, &lightm.m00) );

    // Set light radiant power in Watts
    CHECK( rprPointLightSetRadiantPower3f(light, 255, 241, 224) );

    // Attach the light to the scene
    CHECK( rprSceneAttachLight(scene, light) );

    // Create framebuffer to store rendering result
    rpr_framebuffer_desc desc;
    desc.fb_width = 800;
    desc.fb_height = 600;

    // 4 component 32-bit float value each
    rpr_framebuffer_format fmt = {4, RPR_COMPONENT_TYPE_FLOAT32};
	rpr_framebuffer frame_buffer;
	CHECK( rprContextCreateFrameBuffer(context, fmt, &desc, &frame_buffer) );

    // Clear framebuffer to black color
    CHECK( rprFrameBufferClear(frame_buffer) );

    // Set framebuffer for the context
    CHECK( rprContextSetAOV(context, RPR_AOV_COLOR, frame_buffer) );

	// Set antialising options
	CHECK( rprContextSetParameter1u(context, "aasamples", 2) );

    // Progressively render an image
    for (int i = 0; i < NUM_ITERATIONS; ++i)
    {
        CHECK( rprContextRender(context) );
    }

    //std::cout << "Rendering finished.\n";
    UE_LOG ( LogTemp, Warning, TEXT ( "RPR () : Rendering finished ." ) );

    // Save the result to file
    CHECK( rprFrameBufferSaveToFile(frame_buffer, "D:/simple_render.png") );

    // Release the stuff we created
	CHECK(rprObjectDelete(matsys));
    CHECK(rprObjectDelete(plane));
	CHECK(rprObjectDelete(light));
	CHECK(rprObjectDelete(cube));
	CHECK(rprObjectDelete(diffuse));
	CHECK(rprObjectDelete(scene));
	CHECK(rprObjectDelete(camera));
	CHECK(rprObjectDelete(frame_buffer));
	CHECK(rprObjectDelete(context));

}


