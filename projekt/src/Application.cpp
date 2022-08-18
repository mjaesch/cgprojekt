//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Application.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include "lineplanemodel.h"
#include "triangleplanemodel.h"
#include "trianglespheremodel.h"
#include "lineboxmodel.h"
#include "triangleboxmodel.h"
#include "model.h"
#include "ShaderLightmapper.h"


#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif


Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin), pModel(NULL), ShadowGenerator(2048, 2048)
{
	//createScene();
	//createNormalTestScene();
	//createShadowTestScene();
	createGeometryTestScene();


}
void Application::start()
{
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::update(float dtime)
{
	//erst hinzufügen von dem Block
	//translation ist die position
	
	//player->update(dtime);


	//wir brauchen das level als Model damit collision detection gemacht werden kann?

	Matrix movement;
	movement.translation(0, -dtime, 0);

	Matrix blockTransformCurrent = player->getBlockModel()->transform();
	Vector blockPositionBefore = blockTransformCurrent.translation();

	player->getBlockModel()->transform(blockTransformCurrent * movement);
	cubeTest->transform(blockTransformCurrent * movement);
	Vector blockPositionAfter = player->getBlockModel()->transform().translation();


	//durchiterieren durch die bounding boxes
	/*
	for (int i{ 0 }; i < aabbList.size(); i++) {
		std::cout << AABB::checkCollision(player->getBlockModel()->boundingBox(), aabbList[i]);
	}*/

	//die bounding box kann man transformen!!!!
	Matrix translationBB;
	translationBB.translation(blockPositionAfter);
	//translationBB.printMatrix();

	/*
	 
	 for (int i{ 0 }; i < aabbList.size(); i++) {

		 Vector lvlObjectPosition = aabbList[i].transform().translation();

		 std::cout << AABB::checkCollision(bbOfPlayer, aabbList[i]);
	 }*/

	//vlt hier wieder alles probieren, nicht nur translation
	AABB bbOfPlayer = player->getBlockModel()->boundingBox().transform(player->getBlockModel()->transform());
	
	 for (ModelList::iterator it = lvlList.begin(); it != lvlList.end(); ++it)
	 {
		 //Vector translationOfModel = (*it)->transform().translation();
		 AABB bbOfModel = (*it)->boundingBox().transform((*it)->transform());
		 std::cout << AABB::checkCollision(bbOfPlayer, bbOfModel);
		 std::cout << "model box pos: " << bbOfModel.Max.X << ", " << bbOfModel.Max.Y << " , " << bbOfModel.Max.Z << " min"
			 << bbOfModel.Min.X << ", " << bbOfModel.Min.Y << ", " << bbOfModel.Min.Z << "\n";
		 //jz noch die beiden printen und manuell vergleichen
	 }

	 
	std::cout << "bounding box pos: " << bbOfPlayer.Max.X << ", " << bbOfPlayer.Max.Y << " , " << bbOfPlayer.Max.Z << " min"
		<< bbOfPlayer.Min.X << ", " << bbOfPlayer.Min.Y << ", " << bbOfPlayer.Min.Z << "\n";
	/*
	AABB bbOfPLayerTest = player->getBlockModel()->boundingBox();
	std::cout << "bounding box pos: " << bbOfPLayerTest.Max.X << ", " << bbOfPLayerTest.Max.Y << " , " << bbOfPLayerTest.Max.Z << " min"
		<< bbOfPLayerTest.Min.X << ", " << bbOfPLayerTest.Min.Y << ", " << bbOfPLayerTest.Min.Z << "\n";*/

	/*//make camera follow the block
	Cam.setPosition(Vector(blockPositionAfter.X + 5, blockPositionAfter.Y + 3, blockPositionAfter.Z - 15));
	Cam.setTarget(blockPositionAfter);*/
    Cam.update();
}

void Application::draw()
{
	ShadowGenerator.generate(Models);
	
    // 1. clear screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShaderLightMapper::instance().activate();
    // 2. setup shaders and draw models
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
    {
        (*it)->draw(Cam);
    }
	ShaderLightMapper::instance().deactivate();
	
    // 3. check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error==0);
}
void Application::end()
{
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
        delete *it;
    
    Models.clear();
	delete player;
}

void Application::createGeometryTestScene()
{
	Matrix m, n;

	/// <summary>
	/// wenn man die skybox als erstes macht, dann ist einfach teil der skybox holz!!!
	/// </summary>

	

	/*
	pModel = new Model(ASSET_DIRECTORY "f.obj", false);
	pModel->shader(new PhongShader(), true);

	m.scale(1);
	m.translation(40, 2, 0);
	pModel->transform(m);
	Models.push_back(pModel);*/

	/*
	pModel = new Model(ASSET_DIRECTORY "woodLVL2.dae", false);
	pModel->shader(new PhongShader(), true);
	m.translation(0, 0, 0);
	m.scale(3);
	pModel->transform(m);
	Models.push_back(pModel);*/

	//wooden box length in units : 6.0f genau
	//wooden box height in units : 0.4f 
	ConstantShader* pConstShader;

	cubeTest = new LineBoxModel(5, 5, 5);

	pConstShader = new ConstantShader();
	pConstShader->color(Color(0, 1, 0));
	cubeTest->shader(pConstShader, true);
	Models.push_back(cubeTest);
	

	Matrix translation, rotation, scale;
	
	pModel = new Model(ASSET_DIRECTORY "dragon.dae", false);
	pModel->shader(new PhongShader(), true);
	translation.translation(-8.0f, 0, 0);
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);

	AABB bbOfModel = pModel->boundingBox();// .transform(pModel->transform());
	//std::cout << AABB::checkCollision(bbOfPlayer, bbOfModel);
	std::cout << "model box pos: " << bbOfModel.Max.X << ", " << bbOfModel.Max.Y << " , " << bbOfModel.Max.Z << " min"
		<< bbOfModel.Min.X << ", " << bbOfModel.Min.Y << ", " << bbOfModel.Min.Z << "\n";
	
	/*
	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	translation.translation(0.0f, 0, 0);
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	translation.translation(6.0f, 0, 0);
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	translation.translation(12.0f, 0, 0);
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	translation.translation(12.0f, 0.4f, 0);
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	//jedes mal wird ne neue matrix erstellt!!!!"
	translation.translation(12.0f, 0, 0);
	rotation.rotationZ(AI_DEG_TO_RAD(90));
	scale.scale(1);
	pModel->transform(translation * rotation * scale );
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	//jedes mal wird ne neue matrix erstellt!!!!"
	translation.translation(18.0f, -6.0f, 0);
	//rotation.rotationZ(AI_DEG_TO_RAD(90));
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "woodenBox.dae", false);
	pModel->shader(new PhongShader(), true);
	//jedes mal wird ne neue matrix erstellt!!!!"
	translation.translation(24.0f, -6.0f, 0);
	//rotation.rotationZ(AI_DEG_TO_RAD(90));
	scale.scale(1);
	pModel->transform(translation * scale);
	Models.push_back(pModel);*/

	//aabbList.reserve(sizeof(AABB) * 20);
	for (ModelList::iterator it = Models.begin(); it != Models.end(); ++it)
	{
		lvlList.push_back((*it));
	}

	player = new PlayingCube(ASSET_DIRECTORY "block.dae");
	Models.push_back(player->getBlockModel());

	pModel = new Model(ASSET_DIRECTORY "skybox_bright.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	Models.push_back(pModel);



	// directional lights
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0.2f, -1, 1));
	dl->color(Color(1, 1, 1));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);

	
}

void Application::createScene()
{
	Matrix m,n;

	pModel = new Model(ASSET_DIRECTORY "skybox.obj", false);
	pModel->shader(new PhongShader(), true);
	pModel->shadowCaster(false);
	Models.push_back(pModel);


	pModel = new Model(ASSET_DIRECTORY "scene.dae", false);
	pModel->shader(new PhongShader(), true);
	m.translation(10, 0, -10);
	pModel->transform(m);
	Models.push_back(pModel);
	

	// directional lights
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0.2f, -1, 1));
	dl->color(Color(0.25, 0.25, 0.5));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);
	
	Color c = Color(1.0f, 0.7f, 1.0f);
	Vector a = Vector(1, 0, 0.1f);
	float innerradius = 45;
	float outerradius = 60;
	
	// point lights
	PointLight* pl = new PointLight();
	pl->position(Vector(-1.5, 3, 10));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5.0f, 3, 10));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-1.5, 3, 28));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5.0f, 3, 28));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(-1.5, 3, -8));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);

	pl = new PointLight();
	pl->position(Vector(5.0f, 3, -8));
	pl->color(c);
	pl->attenuation(a);
	ShaderLightMapper::instance().addLight(pl);
	
	
	// spot lights
	SpotLight* sl = new SpotLight();
	sl->position(Vector(-1.5, 3, 10));
	sl->color(c);
	sl->direction(Vector(1,-4,0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);

	sl = new SpotLight();
	sl->position(Vector(5.0f, 3, 10));
	sl->color(c);
	sl->direction(Vector(-1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);

	sl = new SpotLight();
	sl->position(Vector(-1.5, 3, 28));
	sl->color(c);
	sl->direction(Vector(1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);

	sl = new SpotLight();
	sl->position(Vector(5.0f, 3, 28));
	sl->color(c);
	sl->direction(Vector(-1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	
	sl = new SpotLight();
	sl->position(Vector(-1.5, 3, -8));
	sl->color(c);
	sl->direction(Vector(1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	
	sl = new SpotLight();
	sl->position(Vector(5.0f, 3, -8));
	sl->color(c);
	sl->direction(Vector(-1, -4, 0));
	sl->innerRadius(innerradius);
	sl->outerRadius(outerradius);
	ShaderLightMapper::instance().addLight(sl);
	
}

void Application::createNormalTestScene()
{
	pModel = new LinePlaneModel(10, 10, 10, 10);
	ConstantShader* pConstShader = new ConstantShader();
	pConstShader->color(Color(0, 0, 0));
	pModel->shader(pConstShader, true);
	// add to render list
	Models.push_back(pModel);


	pModel = new Model(ASSET_DIRECTORY "cube.obj", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);


}

void Application::createShadowTestScene()
{
	pModel = new Model(ASSET_DIRECTORY "shadowcube.obj", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);

	pModel = new Model(ASSET_DIRECTORY "bunny.dae", false);
	pModel->shader(new PhongShader(), true);
	Models.push_back(pModel);
	
	// directional lights
	DirectionalLight* dl = new DirectionalLight();
	dl->direction(Vector(0, -1, -1));
	dl->color(Color(0.5, 0.5, 0.5));
	dl->castShadows(true);
	ShaderLightMapper::instance().addLight(dl);
	
	SpotLight* sl = new SpotLight();
	sl->position(Vector(2, 2, 0));
	sl->color(Color(0.5, 0.5, 0.5));
	sl->direction(Vector(-1, -1, 0));
	sl->innerRadius(10);
	sl->outerRadius(13);
	sl->castShadows(true);
	ShaderLightMapper::instance().addLight(sl);
}