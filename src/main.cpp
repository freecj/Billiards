#define NUM_OPENGL_LIGHTS 8

#include <iostream>
#include <fstream>
#include <string>
#include <GL/glui.h>
#include <GL/glut.h>
#include "Shape.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Sphere.h"
#include "SceneParser.h"
#include "Camera.h"
#include "pool.h"
#include <chrono>
#include <mutex>
#include <vector>
#include "ParticleSystem.h"
#include "Headers.h" //this file contains all the headers needed for the program

#define GL_LIGHT_MODEL_COLOR_CONTROL      0x81F8
#define GL_SINGLE_COLOR                   0x81F9
#define GL_SEPARATE_SPECULAR_COLOR        0x81FA

bool strikeflag = false;
bool bumper_pool = 0;
GLUI_RadioGroup *radio;
GLUI_RadioGroup *radiobumper;
GLUI_RadioGroup *radiobumperwhite;
GLUI_RadioGroup *radiobumperred;
bool bumper_turn = 0;
double timestop = 0;
PoolGame *pool = new PoolGame(0);
PoolGame *poolbumper = new PoolGame(1);
bool particleFlag = true;
bool blendFlag = true;
//ParticleSystem particles;
using namespace std; 


chrono::high_resolution_clock::time_point t_prior;
#define MAX_PARTICLES 1000
ShapeCreator shapeCreator; //this will handle drawing the platform
float gParticleSysPos[3] = { pool->balls[14].position[0] , pool->balls[14].position[1],pool->balls[14].position[2] }; //particle emmiter location
float gGravity = (float)-0.0007; //this gets applied to the particles every update
float gWind[3] = { 0, 0, 0 }; //applied to the particles
float gCannonRotation[3] = { 0, 0, 0 }; //rotation of the cannon

bool gPause = false; //whether the simulation is running
ParticleSystem particleSystem(gParticleSysPos, gGravity, gWind); //this is the particle emmitter class
																 //variables for the platform
float gPlatformColors[6][3] = { { 1, 0, 0 },{ 0, 1, 1 },{ 1, 1, 0 },{ 0, 1, 0 },{ 0, 0, 1 },{ 1, 0, 1 } }; //colors of the faces of the platform box
float gPlatformWidth = 1;
float gPlatformHeight = 1;
float gPlatformDepth = 1;
bool speical_s = false;

static GLfloat colors[12][3] =     // Rainbow Of Colors 
{
	{ 1.0f,0.5f,0.5f },{ 1.0f,0.75f,0.5f },{ 1.0f,1.0f,0.5f },{ 0.75f,1.0f,0.5f },
	{ 0.5f,1.0f,0.5f },{ 0.5f,1.0f,0.75f },{ 0.5f,1.0f,1.0f },{ 0.5f,0.75f,1.0f },
	{ 0.5f,0.5f,1.0f },{ 0.75f,0.5f,1.0f },{ 1.0f,0.5f,1.0f },{ 1.0f,0.5f,0.75f }
};

int controlball = 5;

int	 camRotU = 0;
int	 camRotV = 0;
int	 camRotW = 0;
int  viewAngle = 45;
float eyeX = -3;
float eyeY = 12;
float eyeZ = 0;
float lookX = 0;
float lookY = 0;
float lookZ = 0;

float Force_X = 0.1;
float Force_Z = 2.5;
int movetime = 0;
/** These are GLUI control panel objects ***/
int  main_window;
string filenamePath = "xml/pool.xml";
GLUI_EditText* filenameTextField = NULL;

/** these are the global variables used for rendering **/
Cube* cube = new Cube();
Cylinder* cylinder = new Cylinder();
Cone* cone = new Cone();
Sphere* sphere = new Sphere();

Shape* shape = NULL;
SceneParser* parser = NULL;
Camera* camera = new Camera();
//Puck* puck = new Puck();

SceneNode *root = NULL;


struct BitMapFile
{
	int sizeX;
	int sizeY;
	unsigned char *data;
};

unsigned int texture[30];
string filenameball[27] = { "image/ball1.bmp", "image/ball2.bmp", "image/ball3.bmp",
"image/ball4.bmp","image/ball5.bmp","image/ball6.bmp","image/ball7.bmp",
"image/ball8.bmp","image/ball9.bmp","image/ball10.bmp",
"image/ball11.bmp", "image/ball12.bmp", "image/ball13.bmp",
"image/ball14.bmp","image/ball0.bmp",//14
"image/ball15.bmp", "image/Wood_Light.bmp","image/wallpaper.bmp", "image/green.bmp",  "image/floor.bmp","image/ceiling.bmp","image/ceiling1.bmp",
"image/black.bmp","image/stick.bmp",
"image/bumperred.bmp","image/bumperwhite.bmp","image/red.bmp"//26
};
//void drawParticle(Particle currParticle);
void setupCamera();
const int NUMBALLS = 2;

// quadric object for GLU functions
GLUquadricObj* quadricObject = NULL;
// change current shininess
void setShininess(double s)
{
	// set specular shininess
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	// set specular colour
	float colour[4];
	colour[0] = (float)s;
	colour[1] = (float)s;
	colour[2] = (float)s;
	colour[3] = 1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colour);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, colour);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colour);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, colour);
//	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colour);
}
// draw sphere using specified size
void drawSphere(double radius)
{
	int slices = 30;
	int stacks = 30;
	gluSphere(quadricObject, radius, slices, stacks);
}

// draw cuboid using specified sizes
void drawCuboid(double width, double height, double depth)
{
	glPushMatrix();
	glScaled(width, height, depth);
	glutSolidCube(1);
	glPopMatrix();
}

// draw closed cylinder using specified sizes
void drawCylinder(double baseRadius, double topRadius, double height)
{
	int slices = 30;
	int stacks = 1;

	// first cylinder cap
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	gluDisk(quadricObject, 0, baseRadius, slices, stacks);
	glPopMatrix();

	// cylinder side
	gluCylinder(quadricObject, baseRadius, topRadius, height, slices, stacks);

	// second cylinder cap
	glPushMatrix();
	glTranslated(0, 0, height);
	gluDisk(quadricObject, 0, topRadius, slices, stacks);
	glPopMatrix();
}
//void drawAllParticles() {
//	particles.updateAll();
//	for (int i = 0; i < particles.getNumberOfParticles(); i++)
//		drawParticle(particles.getNextParticle());
//
//}
////Draws one individual particle, given a particle struct from the particle
////system object
//void drawParticle(Particle currParticle)
//{
//	glEnable(GL_TEXTURE_2D);
//	glPushMatrix();
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//	glRotatef(currParticle.azimuthRoation, 0, 1, 0);
//	glRotatef(currParticle.zenithRotation, 0, 0, 1);
//	glTranslatef(pool->balls[14].position[0] + currParticle.surfaceTranslationFactor, 0, 0);
//	glRotatef(90, 0, 1, 0);
//	glScalef(.1, .1, .1);
//	glBindTexture(GL_TEXTURE_2D, texture[17]);
//
//	// Logo Facing Earth
//	glBegin(GL_TRIANGLE_STRIP);
//	glTexCoord2d(1, 1);
//	glVertex3f(0.5f, 0.5f, 0.0f); // Top Right
//	glTexCoord2d(0, 1);
//	glVertex3f(-0.5f, 0.5f, 0.0f); // Top Left
//	glTexCoord2d(1, 0);
//	glVertex3f(0.5f, -0.5f, 0.0f); // Bottom Right
//	glTexCoord2d(0, 0);
//	glVertex3f(-0.5f, -0.5f, 0.0f); // Bottom Left
//	glEnd();
//
//	// Logo Facing Away From Earth
//	glBegin(GL_TRIANGLE_STRIP);
//	glTexCoord2d(1, 1);
//	glVertex3f(-0.5f, 0.5f, 0.0f); // Top Right
//	glTexCoord2d(0, 1);
//	glVertex3f(0.5f, 0.5f, 0.0f); // Top Left
//	glTexCoord2d(1, 0);
//	glVertex3f(-0.5f, -0.5f, 0.0f); // Bottom Right
//	glTexCoord2d(0, 0);
//	glVertex3f(0.5f, -0.5f, 0.0f); // Bottom Left
//	glEnd();
//	glPopMatrix();
//	glDisable(GL_TEXTURE_2D);
//}

void callback_load(int id) {
	char curDirName [2048];
	if (filenameTextField == NULL) {
		return;
	}

    t_prior = chrono::high_resolution_clock::now();

	printf ("%s\n", filenameTextField->get_text());

	if (parser != NULL) {
		delete parser;
	}
	parser = new SceneParser (filenamePath);
	cout << "success? " << parser->parse() << endl;

    root = parser->getRootNode();

	setupCamera();
}
void callback_reset(int id) {
	Force_X = 0.1;
	Force_Z = 2.5;
	setupCamera();
	if (bumper_pool) {
		poolbumper->reset(bumper_pool);
	}
	else {
		pool->reset(bumper_pool);
	}
	
	timestop = 0;
	particleSystem.reset();
}
void renderShape (int shapeType) {
	switch (shapeType) {
	case SHAPE_CUBE:
		shape = cube;
		break;
	case SHAPE_CYLINDER:
		shape = cylinder;
		break;
	case SHAPE_CONE:
		shape = cone;
		break;
	case SHAPE_SPHERE:
		shape = sphere;
		break;
	case SHAPE_SPECIAL1:
		shape = sphere;
		break;
	default:
		shape = sphere;
	}
	shape->draw();
}

/***************************************** myGlutIdle() ***********/

void myGlutIdle(void)
{
	/* According to the GLUT specification, the current window is
	undefined during an idle callback.  So we need to explicitly change
	it if necessary */
	if (glutGetWindow() != main_window)
		glutSetWindow(main_window);

	glutPostRedisplay();
}


/**************************************** myGlutReshape() *************/

void myGlutReshape(int x, int y)
{
	float xy_aspect;

	xy_aspect = (float)x / (float)y;
	glViewport(0, 0, x, y);

	camera->SetScreenSize(x, y);

	glutPostRedisplay();
}


/***************************************** setupCamera() *****************/
void setupCamera()
{
	SceneCameraData cameraData;
	parser->getCameraData(cameraData);

	camera->Reset();  //note that this is a new function. Be sure to set values for near and far plane!
	camera->SetViewAngle(cameraData.heightAngle);
	if (cameraData.isDir == true) {
		camera->Orient(cameraData.pos, cameraData.look, cameraData.up);
	}
	else {
		camera->Orient(cameraData.pos, cameraData.lookAt, cameraData.up);
	}

	viewAngle = camera->GetViewAngle();
	Point eyeP = camera->GetEyePoint();
	Vector lookV = camera->GetLookVector();
	eyeX = eyeP[0];
	eyeY = eyeP[1];
	eyeZ = eyeP[2];
	lookX = lookV[0];
	lookY = lookV[1];
	lookZ = lookV[2];
	camRotU = 0;
	camRotV = 0;
	camRotW = 0;
	GLUI_Master.sync_live_all();
}



/***************************************** setLight() *****************/
void setLight(const SceneLightData &light)
{
    // Zero ambient light because the ambient light for the scene is
    // set with GL_LIGHT_MODEL_AMBIENT
    float zero[] = { 0, 0, 0, 0 };

    // There are 8 lights in OpenGL, GL_LIGHT0 to GL_LIGHT7, and
    // each GL_LIGHT* is one greater than the previous one.  This
    // means the nth light is GL_LIGHT0 + n.
    int id = GL_LIGHT0 + light.id;

    glLightfv(id, GL_AMBIENT, zero);
    glLightfv(id, GL_DIFFUSE, &light.color.r);
    glLightfv(id, GL_SPECULAR, &light.color.r);
    glLightf(id, GL_CONSTANT_ATTENUATION, light.function[0]);
    glLightf(id, GL_LINEAR_ATTENUATION, light.function[1]);
    glLightf(id, GL_QUADRATIC_ATTENUATION, light.function[2]);

    if (glIsEnabled(id))
        cout << "warning: GL_LIGHT" << light.id << " enabled more than once, not supposed to happen" << endl;

    switch (light.type)
    {
        case LIGHT_POINT:
        {
            // Convert from double[] to float[] and make sure the w coordinate is correct 
            float position[] = { light.pos[0], light.pos[1], light.pos[2], light.pos[3] };
            glLightfv(id, GL_POSITION, position);
			glEnable(id);
            break;
        }

        case LIGHT_DIRECTIONAL:
        {
            // Convert from double[] to float[] and make sure the direction vector is normalized (it isn't for a lot of scene files)
            Vector direction = -light.dir;
			direction.normalize();
            float position[] = { direction[0], direction[1], direction[2], 0 };
            glLightfv(id, GL_POSITION, position);
			glEnable(id);
            break;
        }

        case LIGHT_SPOT:
        {
            // Convert from double[] to float[] and make sure the direction vector is normalized (it isn't for a lot of scene files)
            Vector direction = -light.dir;
			direction.normalize();
            float position[] = { direction[0], direction[1], direction[2], direction[3] };
            float direction3[] = { direction[0], direction[1], direction[2]};
            glLightfv(id, GL_POSITION, position);
            glLightfv(id, GL_SPOT_DIRECTION, direction3);
            glLightf(id, GL_SPOT_CUTOFF, light.angle);
            glLightf(id, GL_SPOT_EXPONENT, (light.radius / light.penumbra) * 128);
			glEnable(id);
            break;
        }

        case LIGHT_AREA:
        {
            // Convert from double[] to float[] and make sure the direction vector is normalized (it isn't for a lot of scene files)
            Vector direction = -light.dir;
			direction.normalize();
            float position[] = { direction[0], direction[1], direction[2], direction[3] };
            glLightfv(id, GL_POSITION, position);
			glEnable(id);
            break;
        }
        default:
        {
        	printf("Light type cannot be applied!\n");
        	break;       	
        }
    }
}


/***************************************** applyMaterial() *****************/
void applyMaterial(const SceneMaterial &material)
{
	SceneGlobalData globalData;
	parser->getGlobalData(globalData);

	SceneMaterial material_local = material;
	material_local.cAmbient.r *= 1 * globalData.ka;
	material_local.cAmbient.g *= 1 * globalData.ka;
	material_local.cAmbient.b *= 1 * globalData.ka;
	material_local.cDiffuse.r *= globalData.kd;
	material_local.cDiffuse.g *= globalData.kd;
	material_local.cDiffuse.b *= globalData.kd;
	material_local.cSpecular.r *= globalData.ks;
	material_local.cSpecular.g *= globalData.ks;
	material_local.cSpecular.b *= globalData.ks;

    glMaterialfv(GL_FRONT, GL_AMBIENT, material_local.cAmbient.channels);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_local.cDiffuse.channels);
    glMaterialfv(GL_FRONT, GL_EMISSION, material_local.cEmissive.channels);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_local.cSpecular.channels);
    glMaterialf(GL_FRONT, GL_SHININESS, material_local.shininess);
}

void drawSceneRecursive(SceneNode *node) {

    if (node != NULL) {
	glPushMatrix();
        double *t, *s, *r;
		for (int i = 0; i < node->transformations.size(); i++) {
			switch(node->transformations[i]->type) {
				case TRANSFORMATION_TRANSLATE:
                    t = node->transformations[i]->translate.unpack();
					glTranslated(t[0],t[1],t[2]);
                    break;
				case TRANSFORMATION_SCALE:
                    s = node->transformations[i]->scale.unpack();
					glScaled(s[0],s[1],s[2]);
                    break;
				case TRANSFORMATION_ROTATE:
                    r = node->transformations[i]->rotate.unpack();
					glRotated(node->transformations[i]->angle, r[0], r[1], r[2]);
                    break;
				case TRANSFORMATION_MATRIX:
					glMultMatrixd(node->transformations[i]->matrix.unpack());
                    break;
				default:
					break;
			}
		}

		for (int i = 0; i < node->primitives.size(); ++i) {
			applyMaterial(node->primitives[i]->material);
			if (node->primitives[i]->type == SHAPE_CUBE) {
			
				renderShape(node->primitives[i]->type);
			}
			else {
				renderShape(node->primitives[i]->type);
			}
		}

	    for (int i = 0; i < node->children.size(); ++i) {
            drawSceneRecursive(node->children[i]);
	    }
	 glPopMatrix();
    }
}


// Routine to read a bitmap file.
// Works only for uncompressed bmp files of 24-bit color.
BitMapFile *getBMPData(string filename)
{
	struct ImageFileDate *image = NULL;
	BitMapFile *bmp = new BitMapFile;
	unsigned int size, offset, headerSize;
	FILE* pFile = fopen(filename.c_str(), "rb");
	fseek(pFile,18, SEEK_SET);
	fread(&bmp->sizeX, 4, 1, pFile);
	fread(&bmp->sizeY, 4, 1, pFile);
	//size = bmp->sizeX * 3;
	unsigned char *imageff;
	//size = bmp->sizeX*bmp->sizeY * 3;
	//size = ftell(pFile) - 54;  //算出文件数据部分的长度
//	cout << size << endl;

	int width, height, m, j;
	height = bmp->sizeX;
	width = bmp->sizeY;
	fseek(pFile, 0, SEEK_END);
	size = height*width * 3;

	bmp->data =new unsigned char[size];
	
	fseek(pFile, 54, SEEK_SET);    // image data  
	fread(bmp->data, sizeof(unsigned char), size, pFile);
	//printf("%s", size);
	//cout << height << endl;
	fclose(pFile);
	//cout << height << endl;
	for (int i = 0; i < size; i += 3)
	{
		unsigned char temp = bmp->data[i];
		bmp->data[i] = bmp->data[i + 2];
		bmp->data[i + 2] = temp;
	}
	return bmp;
}
// Load external textures.
void loadExternalTextures()
{
	//glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	
	// Local storage for bmp image data.
	
	BitMapFile *image1[30];
	for (int i = 0; i < 27; ++i) {
		glGenTextures(1, &texture[i]);     // 为第i个位图创建纹理
		image1[i] = getBMPData(filenameball[i]);
		//glGenTextures(1, &texture[i]);

		glBindTexture(GL_TEXTURE_2D, texture[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	/*	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/
		//glTexImage2D(GL_TEXTURE_2D, 0, 4, image1[i]->sizeX, image1[i]->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1[i]->data);
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 4, image1[i]->sizeX, image1[i]->sizeY, GL_RGBA, GL_UNSIGNED_BYTE, image1[i]->data);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image1[i]->sizeX, image1[i]->sizeY, 0,
			GL_RGB, GL_UNSIGNED_BYTE, image1[i]->data);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glAlphaFunc(GL_GREATER, 0.5);//  
		
	}
	
}
void drawTable(void) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
	glEnable(GL_TEXTURE_GEN_T);

	// draw 3 lamps
	for (int i = -1; i <= 1; i++)
	{
		glPushMatrix();
	//	setShininess(0.0); // not shiny
		glBindTexture(GL_TEXTURE_2D, texture[19]);       //map the third texture to the cue stick
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		gluQuadricTexture(quadricObject, GL_TRUE);

		// lamp shade
		glColor3d(1.0, 0.3, 0.2); // red colour
		
		glTranslated(i * 2.3, 5.8, 0);
		glRotated(-90, 1, 0, 0);
		drawCylinder(0.8, 0, 1.1);
		glBindTexture(GL_TEXTURE_2D, texture[14]);       
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		gluQuadricTexture(quadricObject, GL_TRUE);
		// light bulb
		glColor3d(2.0, 2.0, 2.0); // white colour
		drawSphere(0.25);

		glBindTexture(GL_TEXTURE_2D, texture[21]);       
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		gluQuadricTexture(quadricObject, GL_TRUE);

		// lamp mount
	//	setShininess(1.0); // shiny
		glTranslated(0, 0, 0.8);
		glColor3d(1.0, 0.8, 0.4); // yellow colour
		drawCylinder(0.2, 0.2, 0.2);
		glTranslated(0, 0, 0.2);
		glColor3d(0.9, 0.7, 0.4); // yellow colour
		drawCylinder(0.2, 0.04, 0.1);
		glColor3d(0.8, 0.6, 0.4); // yellow colour
		drawCylinder(0.04, 0.04, 8);

		glPopMatrix();
	}
	double hsize = 0.55; // hole size
		// draw table holes
		glColor3d(0.0, 0.0, 0.0); // black colour
	setShininess(0.0); // not shiny
		glBindTexture(GL_TEXTURE_2D, texture[22]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		gluQuadricTexture(quadricObject, GL_TRUE);
		float setx = 9.3;
		float setz = 5.4;
		float set_y = 0.13;
	glPushMatrix();
	glTranslated(setx, set_y, setz);
	glRotated(90, 1, 0, 0);
	glutSolidCone(hsize, 0.001, 20, 1);
	glPopMatrix();
	glPushMatrix();
	glTranslated(setx, set_y, -setz);
	glRotated(90, 1, 0, 0);
	glutSolidCone(hsize, 0.001, 20, 1);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, set_y, setz);
	glRotated(90, 1, 0, 0);
	glutSolidCone(hsize, 0.001, 20, 1);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, set_y, -setz);
	glRotated(90, 1, 0, 0);
	glutSolidCone(hsize, 0.001, 20, 1);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-setx, set_y, setz);
	glRotated(90, 1, 0, 0);
	glutSolidCone(hsize, 0.001, 20, 1);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-setx, set_y, -setz);
	glRotated(90, 1, 0, 0);
	glutSolidCone(hsize, 0.001, 20, 1);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texture[16]);//leg
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluQuadricTexture(quadricObject, GL_TRUE);
	glPushMatrix();
	glTranslated(8, 0, 4.5);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 5);
	glPopMatrix();
	glPushMatrix();
	glTranslated(8, 0, -4.5);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 5);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 4.5);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 5);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, -4.5);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 5);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-8, 0, 4.5);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 5);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-8, 0, -4.5);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 5);
	glPopMatrix();

//	glEnable(GL_LIGHTING);
	
//	glBindTexture(GL_TEXTURE_2D, texture[23]);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//	gluQuadricTexture(quadricObject, GL_TRUE);
//// draw table cushions
//	glColor3d(0.1, 0.3, 0.5); // dark blue colour
//	glPushMatrix();
//	glTranslated(-3.7, 0.3, 3.95); drawCuboid(6.5 - hsize, 0.2, 0.1);
//	glPopMatrix();
//
//
//	glPushMatrix();
//	glTranslated(-3.7, 0.3, -3.95); drawCuboid(6.5 - hsize, 0.2, 0.1);
//	glPopMatrix();


	setShininess(1.0); 
	glBindTexture(GL_TEXTURE_2D, texture[19]);       //map the third texture to the cue stick
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluQuadricTexture(quadricObject, GL_TRUE);

	// draw floor
	glPushMatrix();
	glTranslated(0, -6, 0);
	drawCuboid(30, 0.1, 20);
	glPopMatrix();

	//glBindTexture(GL_TEXTURE_2D, texture[17]);       //map the tglhird texture to the cue stick
	//glTexEnvf(GL_TEXTURE_ENV_COLOR, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//glEnable(GL_TEXTURE_GEN_S);
	//glEnable(GL_TEXTURE_GEN_T);
	//gluQuadricTexture(quadricObject, GL_TRUE);

	//glPushMatrix();
	//glTranslated(-10,3, 0);
	//glRotated(90, 0, 0, 1);
	//drawCuboid(30, 0.1, 20);
	//glPopMatrix();
	glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);
	

	particleSystem.setParticleType(0);
	glPushMatrix(); // push scene rotationglPopMatrix();
	glTranslatef(gParticleSysPos[0], gParticleSysPos[1] + 1.1, gParticleSysPos[2]);
	glRotatef(270, 0, 0, 1);
	//glutSolidTeapot(1); //draw the cannon
	glPopMatrix(); //pop cannon translation
	glPopMatrix(); //pop cannon rotation
	glPopMatrix(); //pop scene rotation
				   //float m_TableX = 0;
				   //float m_TableZ = 0;
				   //float m_Tablewidth = 4.5;
				   //float m_TableLength = 8;
				   //glBegin(GL_QUADS); //lEFT
				   //glNormal3f(0, 1, 0);
				   //glColor3d(0.5, 0.35, 0.05);
				   //glVertex3f(m_TableX, 0.0f, m_TableZ);
				   //glVertex3f(m_TableX+m_TableLength, 0.0f, m_TableZ);
				   //
				   //glVertex3f(m_TableX + m_TableLength, 0.0f, m_TableZ+ m_Tablewidth);
				   //glVertex3f(m_TableX, 0.0f, m_TableZ + m_Tablewidth);
				   //glEnd();
				   //glBegin(GL_TRIANGLE_FAN); //Draws bottom, left and front
				   //glVertex3f(-1, -1, -1); //0
				   //glVertex3f(1, -1, -1); //1
				   //glVertex3f(1, 1, -1); //5
				   //glVertex3f(-1, 1, -1); //4
				   //glVertex3f(-1, 1, 1); //7
				   //glVertex3f(-1, -1, 1); //3
				   //glVertex3f(1, -1, 1); //2
				   //glVertex3f(1, -1, -1); //1
				   //glEnd();
				   //glBegin(GL_TRIANGLE_FAN); //Draws top, right and back
				   //glVertex3f(1, 1, 1); //6
				   //glVertex3f(1, 1, -1); //5
				   //glVertex3f(-1, 1, -1); //4
				   //glVertex3f(-1, 1, 1); //7
				   //glVertex3f(-1, -1, 1); //3
				   //glVertex3f(1, -1, 1); //2
				   //glVertex3f(1, -1, -1); //1
				   //glVertex3f(1, 1, -1); //5
				   //glEnd();

	drawSceneRecursive(root);
	float matAmbAndDif[4] = { 1, 1, 1, 1.0 };
	float matSpec[4]{ 0, 0, 0, 0 };
	float matShine[1] = { 3 };
	
	
	glEnable(GL_TEXTURE_2D);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texture[18]);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(-9.5,0.1, 5.5);
	glTexCoord2i(0, 1); glVertex3f(9.5, 0.1,5.5);
	glTexCoord2i(1, 1); glVertex3f(9.5, 0.1, -5.5);
	glTexCoord2i(1, 0); glVertex3f(-9.5,0.1, -5.5);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);


	int width = 15;
	glBindTexture(GL_TEXTURE_2D, texture[17]);//floor
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-15, -width, -10);
	glTexCoord2f(0.0, 1); glVertex3f(-15, width, -10);
	glTexCoord2f(1, 1); glVertex3f(20, width, -10);
	glTexCoord2f(1, 0.0); glVertex3f(20, -width, -10);
	glEnd();

	

	//glBindTexture(GL_TEXTURE_2D, texture[17]);//ceiling
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-15, -width, 10);
	glTexCoord2f(0.0, 1.0); glVertex3f(-15, width, 10);
	glTexCoord2f(1.0, 1.0); glVertex3f(20, width, 10);
	glTexCoord2f(1.0, 0.0); glVertex3f(20, -width, 10);
	glEnd();

	//glBindTexture(GL_TEXTURE_2D, texture[17]);//ceiling
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(20, -width, -10);
	glTexCoord2f(0.0, 1.0); glVertex3f(20, width, -10);
	glTexCoord2f(1.0, 1.0); glVertex3f(20, width, 10);
	glTexCoord2f(1.0, 0.0); glVertex3f(20,- width, 10);
	glEnd();
	//glBindTexture(GL_TEXTURE_2D, texture[17]);//ceiling
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-15, -width, -10);
	glTexCoord2f(0.0, 1.0); glVertex3f(-15, width, -10);
	glTexCoord2f(1.0, 1.0); glVertex3f(-15, width, 10);
	glTexCoord2f(1.0, 0.0); glVertex3f(-15, -width, 10);
	glEnd();

	//glBindTexture(GL_TEXTURE_2D, texture[17]);//ceiling
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-15, -width, -10);
	glTexCoord2f(0.0, 1.0); glVertex3f(-15, -width, 10);
	glTexCoord2f(1.0, 1.0); glVertex3f(20, -width, 10);
	glTexCoord2f(1.0, 0.0); glVertex3f(20, -width, -10);
	glEnd();

	//glBindTexture(GL_TEXTURE_2D, texture[17]);//ceiling
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-15, width, -10);
	glTexCoord2f(0.0, 1.0); glVertex3f(-15, width, 10);
	glTexCoord2f(1.0, 1.0); glVertex3f(20, width, 10);
	glTexCoord2f(1.0, 0.0); glVertex3f(20, width, -10);
	glEnd();
	
	
	//glDisable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	//
	////glEnable(GL_POLYGON_OFFSET_FILL);
	//glPushMatrix(); // push scene rotation
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glColor3f(0.0, 0.5, 0.5);

	//Point test[6] = { Point(0,0,-3.75),Point(0,0,3.75),
	//	Point(-7.5,0,-3.75),Point(-7.5,0,3.75), Point(7.5,0,-3.75),Point(7.5,0,3.75)
	//}; 
	//glTranslatef(test[5][0], test[5][1], test[5][2]);
	//int steps = 12;

	//double sc =  1;

	//double I1x = 0;
	//double I1y = 2;

	//double delta = ((PI / 2.0) / (double)steps);
	//double lastX = I1x - sc;
	//double lastY = I1y;

	//for (double w = delta, i = 1; i <= steps; w += delta, ++i)
	//{
	//	double x = -cos(w);
	//	double y = -sin(w);
	//	double x1 = I1x + (x * sc);
	//	double y1 = I1y + (y * sc);


	//	glBegin(GL_TRIANGLES);
	//	glVertex3f(lastX,1, lastY);
	//	glVertex3f(x1, 1, y1);
	//	glVertex3f(I1x,1, I1y);
	//	glEnd();

	//	lastX = x1;
	//	lastY = y1;
	//}
	//glPopMatrix();
}
void drawBumperTable(void) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
	glEnable(GL_TEXTURE_GEN_T);

	// draw 3 lamps
	for (int i = -1; i <= 1; i++)
	{
		glPushMatrix();
		//	setShininess(0.0); // not shiny
		glBindTexture(GL_TEXTURE_2D, texture[19]);       //map the third texture to the cue stick
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		gluQuadricTexture(quadricObject, GL_TRUE);

		// lamp shade
		glColor3d(1.0, 0.3, 0.2); // red colour

		glTranslated(i * 2.3, 5.8, 0);
		glRotated(-90, 1, 0, 0);
		drawCylinder(0.8, 0, 1.1);
		glBindTexture(GL_TEXTURE_2D, texture[14]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		gluQuadricTexture(quadricObject, GL_TRUE);
		// light bulb
		glColor3d(2.0, 2.0, 2.0); // white colour
		drawSphere(0.25);

		glBindTexture(GL_TEXTURE_2D, texture[21]);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		gluQuadricTexture(quadricObject, GL_TRUE);

		// lamp mount
		//	setShininess(1.0); // shiny
		glTranslated(0, 0, 0.8);
		glColor3d(1.0, 0.8, 0.4); // yellow colour
		drawCylinder(0.2, 0.2, 0.2);
		glTranslated(0, 0, 0.2);
		glColor3d(0.9, 0.7, 0.4); // yellow colour
		drawCylinder(0.2, 0.04, 0.1);
		glColor3d(0.8, 0.6, 0.4); // yellow colour
		drawCylinder(0.04, 0.04, 8);

		glPopMatrix();
	}
	double hsize = 0.55; // hole size
						 // draw table holes
	glColor3d(0.0, 0.0, 0.0); // black colour
	setShininess(0.0); // not shiny
	glBindTexture(GL_TEXTURE_2D, texture[22]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluQuadricTexture(quadricObject, GL_TRUE);
	float setx = 8.5;
	float sety = 1.1;
	
	glPushMatrix();
	glTranslated(setx, 0.13,0);
	glRotated(90, 1, 0, 0);
	glutSolidCone(hsize, 0.01, 20, 1);
	glPopMatrix();
	glPushMatrix();
	
	glPushMatrix();
	glTranslated(-setx, 0.13, 0);
	glRotated(90, 1, 0, 0);
	glutSolidCone(hsize, 0.01, 20, 1);
	glPopMatrix();

	// set specular shininess
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	// set specular colour
	float colour[4];
	colour[0] = 1;
	colour[1] = 1;
	colour[2] = 1;
	colour[3] = 1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colour);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, colour);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colour);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, colour);
	glBindTexture(GL_TEXTURE_2D, texture[23]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluQuadricTexture(quadricObject, GL_TRUE);
//1
	glPushMatrix();
	glTranslated(setx, 0.2, sety);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 0.2);
	glPopMatrix();

	glTranslated(setx, 0.8, sety);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.2, 0.2, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(setx, 0.8, sety);
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.2, 0.3, 50, 50);
	glPopMatrix();
	
//2	
	glPushMatrix();
	glTranslated(setx, 0.2, -sety);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 0.2);
	glPopMatrix();


	glPushMatrix();
	glTranslated(setx, 0.8, -sety);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.2, 0.2, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(setx, 0.8, -sety);
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.2, 0.3, 50, 50);
	glPopMatrix();

	//5
	glPushMatrix();
	glTranslated(0, 0.2, -sety);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.8, -sety);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.2, 0.2, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.8, -sety);
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.2, 0.3, 50, 50);
	glPopMatrix();

	//6
	glPushMatrix();
	glTranslated(0, 0.2, sety);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.8, sety);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.2, 0.2, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.8, sety);
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.2, 0.3, 50, 50);
	glPopMatrix();

	//7
	glPushMatrix();
	glTranslated(sety, 0.2, 0);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(sety, 0.8, 0);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.2, 0.2, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(sety, 0.8, 0);
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.2, 0.3, 50, 50);
	glPopMatrix();
	
	//8
	glPushMatrix();
	glTranslated(-sety, 0.2, 0);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-sety, 0.8, 0);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.2, 0.2, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-sety, 0.8, 0);
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.2, 0.3, 50, 50);
	glPopMatrix();

	colour[0] = 1;
	colour[1] = 0;
	colour[2] = 0;
	colour[3] = 1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colour);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, colour);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colour);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, colour);
	glBindTexture(GL_TEXTURE_2D, texture[23]);
//3
	glPushMatrix();
	glTranslated(-setx, 0.2, sety);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-setx, 0.8, sety);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.2, 0.2, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-setx, 0.8, sety);
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.2, 0.3, 50, 50);
	glPopMatrix();


	//4
	glPushMatrix();
	glTranslated(-setx, 0.2, -sety);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-setx, 0.8, -sety);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.2, 0.2, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-setx, 0.8, -sety);
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.2, 0.3, 50, 50);
	glPopMatrix();
//5

	setx = 2.5;
	glPushMatrix();
	glTranslated(0, 0.2, setx);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.8, setx);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.2, 0.2, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.8, setx);
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.2, 0.3, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.2, -setx);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.8, -setx);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.2, 0.2, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.8, -setx);
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.2, 0.3, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-setx, 0.2, 0);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-setx, 0.8, 0);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.2, 0.2, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-setx, 0.8, 0);
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.2, 0.3, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslated(setx, 0.2, 0);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(setx, 0.8, 0);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.2, 0.2, 1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(setx, 0.8, 0);
	glRotated(90, 1, 0, 0);
	glutSolidTorus(0.2, 0.3, 50, 50);
	glPopMatrix();

	setShininess(1.0); // not shiny
	glBindTexture(GL_TEXTURE_2D, texture[16]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluQuadricTexture(quadricObject, GL_TRUE);
	glPushMatrix();
	glTranslated(8, 0, 4.5);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 5);
	glPopMatrix();
	glPushMatrix();
	glTranslated(8, 0, -4.5);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 5);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 4.5);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 5);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, -4.5);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 5);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-8, 0, 4.5);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 5);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-8, 0, -4.5);
	glRotated(90, 1, 0, 0);
	drawCylinder(0.4, 0.4, 5);
	glPopMatrix();
	


	setShininess(1.0);
	glBindTexture(GL_TEXTURE_2D, texture[19]);       //map the third texture to the cue stick
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	gluQuadricTexture(quadricObject, GL_TRUE);

	// draw floor
	glPushMatrix();
	glTranslated(0, -6, 0);
	drawCuboid(30, 0.1, 20);
	glPopMatrix();

	
	glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);


	particleSystem.setParticleType(0);
	glPushMatrix(); // push scene rotationglPopMatrix();
	glTranslatef(gParticleSysPos[0], gParticleSysPos[1] + 1.1, gParticleSysPos[2]);
	glRotatef(270, 0, 0, 1);
	//glutSolidTeapot(1); //draw the cannon
	glPopMatrix(); //pop cannon translation
	glPopMatrix(); //pop cannon rotation
	glPopMatrix(); //pop scene rotation
				

	drawSceneRecursive(root);
	float matAmbAndDif[4] = { 1, 1, 1, 1.0 };
	float matSpec[4]{ 0, 0, 0, 0 };
	float matShine[1] = { 3 };


	glEnable(GL_TEXTURE_2D);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texture[18]);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(-9.5, 0.1, 5.5);
	glTexCoord2i(0, 1); glVertex3f(9.5, 0.1, 5.5);
	glTexCoord2i(1, 1); glVertex3f(9.5, 0.1, -5.5);
	glTexCoord2i(1, 0); glVertex3f(-9.5, 0.1, -5.5);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);


	int width = 15;
	glBindTexture(GL_TEXTURE_2D, texture[17]);//floor
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-15, -width, -10);
	glTexCoord2f(0.0, 1); glVertex3f(-15, width, -10);
	glTexCoord2f(1, 1); glVertex3f(20, width, -10);
	glTexCoord2f(1, 0.0); glVertex3f(20, -width, -10);
	glEnd();



	//glBindTexture(GL_TEXTURE_2D, texture[17]);//ceiling
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-15, -width, 10);
	glTexCoord2f(0.0, 1.0); glVertex3f(-15, width, 10);
	glTexCoord2f(1.0, 1.0); glVertex3f(20, width, 10);
	glTexCoord2f(1.0, 0.0); glVertex3f(20, -width, 10);
	glEnd();

	//glBindTexture(GL_TEXTURE_2D, texture[17]);//ceiling
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(20, -width, -10);
	glTexCoord2f(0.0, 1.0); glVertex3f(20, width, -10);
	glTexCoord2f(1.0, 1.0); glVertex3f(20, width, 10);
	glTexCoord2f(1.0, 0.0); glVertex3f(20, -width, 10);
	glEnd();
	//glBindTexture(GL_TEXTURE_2D, texture[17]);//ceiling
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-15, -width, -10);
	glTexCoord2f(0.0, 1.0); glVertex3f(-15, width, -10);
	glTexCoord2f(1.0, 1.0); glVertex3f(-15, width, 10);
	glTexCoord2f(1.0, 0.0); glVertex3f(-15, -width, 10);
	glEnd();

	//glBindTexture(GL_TEXTURE_2D, texture[17]);//ceiling
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-15, -width, -10);
	glTexCoord2f(0.0, 1.0); glVertex3f(-15, -width, 10);
	glTexCoord2f(1.0, 1.0); glVertex3f(20, -width, 10);
	glTexCoord2f(1.0, 0.0); glVertex3f(20, -width, -10);
	glEnd();

	//glBindTexture(GL_TEXTURE_2D, texture[17]);//ceiling
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-15, width, -10);
	glTexCoord2f(0.0, 1.0); glVertex3f(-15, width, 10);
	glTexCoord2f(1.0, 1.0); glVertex3f(20, width, 10);
	glTexCoord2f(1.0, 0.0); glVertex3f(20, width, -10);
	glEnd();


	
}
//This function will draw the cue stick by taking the dot product of the angle that the user's mouse is making with the ball
//and rotating a tapered cylinder by the proper amount
void drawStick(void) {
	 if (!IN_RANGE((pool->balls[14].velocity.length()), 0)) {
		 return;
	}
	glEnable(GL_TEXTURE_2D);
	//material lighting properties for the cue stick
	float matAmbAndDif[4] = { 1, 1, 1, 1.0 };
	float matSpec[4]{ 1.0, 1.0, 1.0, 1.0 };
	float matShine[1] = { 10 };
	float cueStickLength = 5;
	//initialization of variables for calculating the cue stick's position
	double result = 0;
	double x1 = 0;
	double y1 = 1;
	double z1 = 0;
	//double x2 = cueStickEndX - cueStickBeginX; //x2 is the x component of the vector between the cue stick's end and beginning points
	//double y2 = cueStickEndY - cueStickBeginY; //y2 is the y component of the vector between the cue stick's end and beginning points
	//double dot = x1*x2 + y1 *y2;               //take the dot product of a vertical vector and the cue stick's vector to get the angle between them
//	dot /= cueStickLength;                     //divide by the magnitude of the cue stick, the magnitude of the vertical is 1, so not needed here
//	double theta = acos(dot);                  //to get the angle between the two vectors, take the arccos of the dot product
	//if (x2 > 0) theta *= -1 * 180.0 / PI;         //convert the angle into degrees, and if x2 > 0, which means the cue stick is pointing to the right of
												  //the screen, multiply by a negative to get the proper rotation, else
//	else theta *= 180.0 / PI;                  //just convert to degrees

	GLUquadricObj *stick = gluNewQuadric();
											   //set lighting properties for the cue stick
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	Vector temp (Force_X, 0, Force_Z);
	Vector test1(1,0,0);
	//isolate its transformation matrix, and push the cue stick into the frustum and set its beginning point to point = (cueStickBeginX, cueStickBeginY)
	glPushMatrix();
	//Point 
	//glTranslatef(0, 1, 0);
	glTranslatef(pool->balls[14].position[0], pool->balls[14].position[1], pool->balls[14].position[2]);
	glRotatef(90, 0,1,0); //rotate the cue stick by the amount calculated above, take into account the user changing the angle
	///cout << acos(dot(temp, test1)) << endl;
	glRotatef(-Force_X, 0, 1, 0);//glRotatef(acos(dot(Vector(Force_X, 0, Force_Z), Vector(1, 0, 0))), pool->balls[14].position[0], 1.0, pool->balls[14].position[2]);                //make the cue stick point vertically up first, before rotating it according to the calculation
	//glRotatef(-30, 1,0 , 0);
	glTranslatef(0, 0, 1);
	
	glBindTexture(GL_TEXTURE_2D, texture[16]);       //map the third texture to the cue stick
	gluQuadricTexture(stick, GL_TRUE); 
		setShininess(0);
	glEnable(GL_TEXTURE_GEN_S);                     //enable mapping on a gluCylinder object
	glEnable(GL_TEXTURE_GEN_T);
	glRotated(-15, 1, 0, 0);
	gluCylinder(stick, 0.1, 0.1, cueStickLength, 20, 20);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();                                  //disable mapping and pop the transformation matrix
	
	glColor3f(0.0f, 1.0f, 0.0f);

	GLfloat x, y, z, angle;

	z = -50.0f;

	Point test[6] = { Point(0,0.5,-5.2),Point(0,0.5,5.2),
		Point(-9.3,0.5,-5.2),Point(-9.3,1,5.2), Point(9.3,0.5,-5.5),Point(9.3,0.5,5.2)
	};
	GLUquadric *m_Quadratic = gluNewQuadric();
	//glEnable(GL_POLYGON_SMOOTH);
	//gluQuadricNormals(m_Quadratic, GL_SMOOTH);
	//gluQuadricDrawStyle(m_Quadratic, GLU_LINE);


	glDisable(GL_POLYGON_SMOOTH);
	for (int i = 0; i < 6; i++) {
		glPushMatrix();
		glTranslatef(test[i][0], test[i][1], test[i][2]);
		GLUquadric * qobj1 = gluNewQuadric();
	
		//gluSphere(qobj1, 0.5, 100, 100);
		gluDeleteQuadric(qobj1);
		glPopMatrix();
	}

	/*glBegin(GL_LINES);

	Point pos = pool->balls[14].position;
	Point tt(pos[0] +1, 0, pos[2]);
	Vector vt(pos[0], 0, pos[2]);
	Point temp1 = trans_mat(vt)*rotY_mat(Force_X) * inv_trans_mat(vt)*tt;*/
	//cout << temp1[0] << " " << temp1[2] << endl;
	//cout << temp[0] << " " << temp[2] << endl;
	//glBegin(GL_LINE); //BACK
					  //	  glNormal3f(0, 1, 0);
//	glColor3d(0.5, 0.35, 0.05);
	/*glVertex3f(tt[0], 0, tt[2]);
	glVertex3f(pos[0], 0, pos[2]);

	glEnd();
	*/
	//cout<< pool->balls[14].prev_inHole;
	//cout << pool->balls[14].inHole;
	if (strikeflag) {
		strikeflag = false;
		if (pool->balls[14].inHole) {
			for (int i = 0; i < 16; ++i) {
				pool->balls[i].inHole = pool->balls[i].prev_inHole;
				pool->balls[i].position = pool->balls[i].prev_position;
				pool->balls[i].velocity = Vector(0, 0, 0);

			}

			return;
		}
		for (int i = 0; i < 16; ++i) {
			pool->balls[i].prev_position = pool->balls[i].position;
			pool->balls[i].prev_inHole = pool->balls[i].isInHole();


		}
	}
	
}
//This function will draw the cue stick by taking the dot product of the angle that the user's mouse is making with the ball
//and rotating a tapered cylinder by the proper amount
void drawBumperStick(void) {
	for (int i = 0; i < 10; ++i) {
		if (!IN_RANGE((poolbumper->balls[i].velocity.length()), 0)) {
			return;
		}
	}
	
	glEnable(GL_TEXTURE_2D);
	//material lighting properties for the cue stick
	float matAmbAndDif[4] = { 1, 1, 1, 1.0 };
	float matSpec[4]{ 1.0, 1.0, 1.0, 1.0 };
	float matShine[1] = { 10 };
	float cueStickLength = 5;
	//initialization of variables for calculating the cue stick's position
	double result = 0;
	double x1 = 0;
	double y1 = 1;
	double z1 = 0;
	
	GLUquadricObj *stick = gluNewQuadric();
	//set lighting properties for the cue stick
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
	Vector temp(Force_X, 0, Force_Z);
	Vector test1(1, 0, 0);
	//isolate its transformation matrix, and push the cue stick into the frustum and set its beginning point to point = (cueStickBeginX, cueStickBeginY)
	glPushMatrix();
	//Point 
	//glTranslatef(0, 1, 0);
	glTranslatef(poolbumper->balls[controlball].position[0], poolbumper->balls[controlball].position[1], poolbumper->balls[controlball].position[2]);
	glRotatef(90, 0, 1, 0); //rotate the cue stick by the amount calculated above, take into account the user changing the angle
							///cout << acos(dot(temp, test1)) << endl;
	glRotatef(-Force_X, 0, 1, 0);//glRotatef(acos(dot(Vector(Force_X, 0, Force_Z), Vector(1, 0, 0))), pool->balls[14].position[0], 1.0, pool->balls[14].position[2]);                //make the cue stick point vertically up first, before rotating it according to the calculation
								 //glRotatef(-30, 1,0 , 0);
	glTranslatef(0, 0, 1);

	glBindTexture(GL_TEXTURE_2D, texture[16]);       //map the third texture to the cue stick
	gluQuadricTexture(stick, GL_TRUE);
	setShininess(0);
	glEnable(GL_TEXTURE_GEN_S);                     //enable mapping on a gluCylinder object
	glEnable(GL_TEXTURE_GEN_T);
	glRotated(-15, 1, 0, 0);
	gluCylinder(stick, 0.1, 0.1, cueStickLength, 20, 20);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();                                  //disable mapping and pop the transformation matrix

	glColor3f(0.0f, 1.0f, 0.0f);

	GLfloat x, y, z, angle;

	z = -50.0f;
	float setx = 8.5;
	float sety = 1.1;
	float setz = 2.5;
	Point test[12] = { Point(setx,0.5,-sety),Point(setx,0.5,sety),Point(-setx,0.5,-sety),Point(-setx,0.5,sety),
		Point(0,0.5,-sety),Point(0,0.5,sety),Point(-sety,0.5,0),Point(sety,0.5,0),
		Point(0,0.5,-setz),Point(0,0.5,setz),Point(-setz,0.5,0),Point(setz,0.5,0)
	};


	GLUquadric *m_Quadratic = gluNewQuadric();
	

	glDisable(GL_POLYGON_SMOOTH);
	for (int i = 0; i < 12; i++) {
		glPushMatrix();
		glTranslatef(test[i][0], test[i][1], test[i][2]);
		GLUquadric * qobj1 = gluNewQuadric();
		//gluSphere(qobj1, BALL_RADIUS*1.5, 100, 100);
		gluDeleteQuadric(qobj1);
		glPopMatrix();
	}

	//glBegin(GL_LINES);

	//Point pos = poolbumper->balls[controlball].position;
	//Point tt(pos[0] + 1, 0, pos[2]);
	//Vector vt(pos[0], 0, pos[2]);
	//Point temp1 = trans_mat(vt)*rotY_mat(Force_X) * inv_trans_mat(vt)*tt;
	//
	//glVertex3f(tt[0], 0, tt[2]);
	//glVertex3f(pos[0], 0, pos[2]);

	//glEnd();
	if (strikeflag) {
		strikeflag = false;
		bumper_turn = !bumper_turn;
		if (bumper_turn) {
					for (int i = 0; i < 5; ++i) {
						if (!poolbumper->balls[i].isInHole()) {
							radiobumperred->set_int_val(i);
							radiobumperred->redraw();
							controlball = i;
							break;
						}
					}
			//controlball = radiobumperred->get_int_val();
		}
		else {
					for (int i = 0 + 5; i < 5 + 5; ++i) {
						if (!poolbumper->balls[i].isInHole()) {
							radiobumperwhite->set_int_val(i-5);
							controlball = i;
							radiobumperwhite->redraw();
							break;
						}
					}
			//controlball = radiobumperwhite->get_int_val() + 5;
		}
		cout << poolbumper->bumpercollision;
		
		if (poolbumper->bumpercollision) {
			for (int i = 0; i < 10; ++i) {
				poolbumper->balls[i].inHole = poolbumper->balls[i].prev_inHole;
				poolbumper->balls[i].position = poolbumper->balls[i].prev_position;
				poolbumper->balls[i].velocity = Vector(0, 0, 0);

			}
			poolbumper->bumpercollision = false;
			return;
		}
		
		for (int i = 0; i < 10; ++i) {
			poolbumper->balls[i].prev_position = poolbumper->balls[i].position;
			poolbumper->balls[i].prev_inHole = poolbumper->balls[i].isInHole();
		}
		
	}
	//if (poolbumper->balls[controlball].isInHole()) {
	//	if (controlball < 5) {
	//		for (int i = 0; i < 5; ++i) {
	//			if (!poolbumper->balls[i].isInHole()) {
	//				radiobumperred->set_int_val(i);
	//				controlball = i;
	//			}
	//		}
	//		for (int i = 0 + 5; i < 5 + 5; ++i) {
	//			if (!poolbumper->balls[i].isInHole()) {
	//				radiobumperwhite->set_int_val(i);
	//			}
	//		}
	//	}
	//	else {
	//		for (int i = 0; i < 5; ++i) {
	//			if (!poolbumper->balls[i].isInHole()) {
	//				radiobumperred->set_int_val(i);
	//			}
	//		}
	//		for (int i = 0 + 5; i < 5 + 5; ++i) {
	//			if (!poolbumper->balls[i].isInHole()) {
	//				radiobumperwhite->set_int_val(i);
	//				controlball = i;
	//			}
	//		}
	//	}
	//}
}
void drawBumperBalls() {
	glEnable(GL_LIGHTING);
	chrono::high_resolution_clock::time_point t_now = chrono::high_resolution_clock::now();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	double ms = chrono::duration_cast<chrono::milliseconds>(t_now - t_prior).count();
	movetime++;
	
	if (speical_s) {
		if (poolbumper->balls[14].isCollision) {
			timestop += ms;
		}
	}
	else {
		for (int i = 0; i < poolbumper->balls.size(); ++i) {
			poolbumper->balls[14].isCollision = false;
		}
	}

	for (int i = 0; i < poolbumper->balls.size(); ++i) {
		if (!poolbumper->balls[i].isInHole()) {

			glDisable(GL_TEXTURE_2D);
			poolbumper->updateMotion(ms / 1000);
			GLdouble *p = poolbumper->balls[i].position.unpack();
			


			glPushMatrix();
			GLUquadric * qobj = gluNewQuadric();
			glEnable(GL_TEXTURE_2D);

			//  glPushMatrix();
			applyMaterial(poolbumper->balls[i].material);
			switch (i)
			{
			case 0:
				glBindTexture(GL_TEXTURE_2D, texture[24]);       //map the third texture to the cue stick
				break;
			case 1:
			case 2:
			case 3:
			case 4:
				glBindTexture(GL_TEXTURE_2D, texture[26]);       //map the third texture to the cue stick
				break;
			case 5:
				glBindTexture(GL_TEXTURE_2D, texture[25]);       //map the third texture to the cue stick
				break;
			case 6:
			case 7:
			case 8:
			case 9:
				glBindTexture(GL_TEXTURE_2D, texture[14]);       //map the third texture to the cue stick
				break;
			}
			//glBindTexture(GL_TEXTURE_2D, texture[i]);       //map the third texture to the cue stick
			glTranslated(p[0], p[1], p[2]);

			
			float stepLength = length(poolbumper->balls[i].velocity*ms / 1000);
			Vector vec = poolbumper->balls[i].velocity*ms / 100;
			vec[1] = 0;
		
			float rotateAngle = 57.3f * vec.length() / BALL_RADIUS;
			poolbumper->balls[i].angle += rotateAngle;
			if (poolbumper->balls[i].angle > 360) {
				poolbumper->balls[i].angle -= 360;
			}
			
			glRotatef(poolbumper->balls[i].angle, poolbumper->balls[i].vAngular[0],
				poolbumper->balls[i].vAngular[1], poolbumper->balls[i].vAngular[2]);

			gluQuadricDrawStyle(qobj, GLU_FILL);
			gluQuadricOrientation(qobj, GLU_OUTSIDE);
			gluQuadricTexture(qobj, GL_TRUE);
			gluQuadricNormals(qobj, GLU_SMOOTH);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
		
			if (speical_s) {
				if (poolbumper->balls[14].isCollision) {
					if (timestop < 50000) {
						// cout << timestop << endl;
						particleSystem.position[0] = poolbumper->balls[14].position[0];
						particleSystem.position[1] = poolbumper->balls[14].position[1] + 1;
						particleSystem.position[2] = poolbumper->balls[14].position[2];
						particleSystem.drawParticles(); //tell the particle system to draw the particles
						particleSystem.spawnParticle();
						particleSystem.updateParticles();
						glPopMatrix();
						continue;
					}
					else {
						glPopMatrix();
						continue;
					}
				}
			}

			gluSphere(qobj, BALL_RADIUS, 100, 100);
			gluDeleteQuadric(qobj);
			glPopMatrix();

			// draw shadow
			setShininess(0); // not shiny
			glColor3f(0.1, 0.2, 0.3); // gray colour

			glPushMatrix();
			glTranslated(poolbumper->balls[i].position[0], 0.15, poolbumper->balls[i].position[2]);
			glRotated(-90, 1, 0, 0);
			glutSolidCone(BALL_RADIUS, 0.001, 20, 1);
			glPopMatrix();
			setShininess(1); // not shiny
		}
	}
	glDisable(GL_TEXTURE_2D);// 
	glDisable(GL_CULL_FACE);            //disable face culling
	t_prior = chrono::high_resolution_clock::now();
}
void drawBalls() {
	glEnable(GL_LIGHTING);
    chrono::high_resolution_clock::time_point t_now = chrono::high_resolution_clock::now();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
    double ms = chrono::duration_cast<chrono::milliseconds>(t_now-t_prior).count();
	movetime++;
	//timestop += ms;
	if (speical_s) {
		if (pool->balls[14].isCollision) {
			timestop += ms;
		}
	}
	else {
		for (int i = 0; i < pool->balls.size(); ++i) {
			pool->balls[14].isCollision = false;
		}
	}
	
   for (int i = 0; i < pool->balls.size(); ++i) {
	   if (!pool->balls[i].isInHole()) {
		  
		   glDisable(GL_TEXTURE_2D);
		   pool->updateMotion(ms / 1000);
		   GLdouble *p = pool->balls[i].position.unpack();
		   
		  

		   glPushMatrix();
		   GLUquadric * qobj = gluNewQuadric();
		   glEnable(GL_TEXTURE_2D);
		 
		 //  glPushMatrix();
		   applyMaterial(pool->balls[i].material);
		
		   glBindTexture(GL_TEXTURE_2D, texture[i]);       //map the third texture to the cue stick
		   glTranslated(p[0], p[1], p[2]);
		  
		  // if (!IN_RANGE((pool->balls[i].velocity.length()), 0)) {
			   float stepLength = length(pool->balls[i].velocity*ms / 1000);
			   Vector vec = pool->balls[i].velocity*ms / 100;
			   vec[1] = 0;
			  // vec.normalize();
			  // float stepLength = pool->balls[i].distance;
			  // float rotateAngle = stepLength * 180 / (PI*0.5);
			   float rotateAngle = 57.3f * vec.length() / BALL_RADIUS;
			   pool->balls[i].angle += rotateAngle;
			   if (pool->balls[i].angle > 360) {
				   pool->balls[i].angle -= 360;
			   }
			  // Vector ntemp = (-vec[1], vec[0], 0);
			   glRotatef(pool->balls[i].angle, pool->balls[i].vAngular[0],
				 pool->balls[i].vAngular[1], pool->balls[i].vAngular[2] );

			  // pool->balls[i].angle = rotateAngle;
			   //pool->balls[i].angle += 1;
			  
			   gluQuadricDrawStyle(qobj, GLU_FILL);
			   gluQuadricOrientation(qobj, GLU_OUTSIDE);
			   gluQuadricTexture(qobj, GL_TRUE);
			   gluQuadricNormals(qobj, GLU_SMOOTH);
			   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			   glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
			 //  glLightModeliv(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SEPARATE_SPECULAR_COLOR_EXT);
			 //  if (step > lengthof) initialize();
			 //  cout << pool->balls[14].isCollision << endl;
			//glPopMatrix();
			if (speical_s) {
				if (pool->balls[14].isCollision) {
					if (timestop < 50000) {
						// cout << timestop << endl;
						particleSystem.position[0] = pool->balls[14].position[0];
						particleSystem.position[1] = pool->balls[14].position[1] + 1;
						particleSystem.position[2] = pool->balls[14].position[2];
						particleSystem.drawParticles(); //tell the particle system to draw the particles
						particleSystem.spawnParticle();
						particleSystem.updateParticles();
						glPopMatrix();
						continue;
					}
					else {
						/* if (i == 14) {
						}
						else {
						gluSphere(qobj, BALL_RADIUS, 100, 100);
						gluDeleteQuadric(qobj);
						}*/
						glPopMatrix();
						continue;
					}
				}
				
			}
		
			gluSphere(qobj, BALL_RADIUS, 100, 100);
		
		    gluDeleteQuadric(qobj);
          //  renderShape(SHAPE_SPHERE);
			
            glPopMatrix();

			// draw shadow
			setShininess(0); // not shiny
			glColor3f(0.1, 0.2, 0.3); // gray colour

			glPushMatrix();
			glTranslated(pool->balls[i].position[0], 0.15, pool->balls[i].position[2]);
			glRotated(-90, 1, 0, 0);
			glutSolidCone(BALL_RADIUS, 0.01, 20, 1);
			glPopMatrix();
			setShininess(1); // not shiny
        }
    }
   glDisable(GL_TEXTURE_2D);// 
   glDisable(GL_CULL_FACE);            //disable face culling
   t_prior = chrono::high_resolution_clock::now();
}

/***************************************** myGlutDisplay() *****************/

void myGlutDisplay(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (parser == NULL) {
		return;
	}
	Matrix projection = camera->GetProjectionMatrix();
	int choice = radio->get_int_val();
	Point pos;
	if (bumper_pool) {
	
		
		pos = poolbumper->balls[controlball].position;
		if (poolbumper->winner == 1) {
			callback_reset(1);
			MessageBox(NULL, L"Winner is Red", L"Info", MB_OK | MB_ICONEXCLAMATION);
		}
		else if (poolbumper->winner == 2) {
			callback_reset(1);
			MessageBox(NULL, L"Winner is White", L"Info", MB_OK | MB_ICONEXCLAMATION);
		}
	}
	else {
		pos = pool->balls[14].position;
	}
	
	Point temp(pos[0] + 5, pos[1]+2.5, pos[2]);
	Vector vt(pos[0], pos[1], pos[2]);
	Point temp1;
	if (Force_X == 0) {
		temp1 = trans_mat(vt) * inv_trans_mat(vt)*temp;
	}
	else {
		temp1 = trans_mat(vt)*inv_rotY_mat(DEG_TO_RAD(Force_X)) * inv_trans_mat(vt)*temp;
	}

	//temp1[1] = 0.5;
	Vector tt = temp1 - pos;
	temp.normalize();
	///tt[1] = 1;
	/*if (bumper_pool) {
		if (!IN_RANGE((poolbumper->balls[controlball].velocity.length()), 0)) {
			choice = 1;
		}
	}
	else {
		if (!IN_RANGE((pool->balls[14].velocity.length()), 0)) {
			choice = 1;
		}
	}*/
	
	switch(choice) {
	case 0:
		camera->SetViewAngle(viewAngle);
		glMatrixMode(GL_PROJECTION);
		
		glLoadMatrixd(projection.unpack());

		camera->Orient(Point(eyeX, eyeY, eyeZ), Vector(lookX, lookY, lookZ), Vector(0, 1, 0));
		camera->RotateV(camRotV);
		camera->RotateU(camRotU);
		camera->RotateW(camRotW);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(camera->GetModelViewMatrix().unpack());
		break;
	case 1:
		camera->SetViewAngle(viewAngle);
		glMatrixMode(GL_PROJECTION);
	
		glLoadMatrixd(projection.unpack());

		camera->Orient(pos + tt, -tt, Vector(0, 1, 0));
		camera->RotateV(camRotV);
		camera->RotateU(camRotU);
		camera->RotateW(camRotW);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(camera->GetModelViewMatrix().unpack());
		break;
	case 2: 
		camera->SetViewAngle(50);
		glMatrixMode(GL_PROJECTION);

		glLoadMatrixd(projection.unpack());

		camera->Orient(Point(0,5.6, 0), Vector(0, -6, 0), Vector(0, 0, 1));
		camera->RotateV(camRotV);
		camera->RotateU(camRotU);
		camera->RotateW(camRotW);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(camera->GetModelViewMatrix().unpack());
		break;
	}
	

	//disable all the lights, and then enable each one...
	for (int i = 0; i < NUM_OPENGL_LIGHTS; i++) {
		glDisable(GL_LIGHT0 + i);
	}

	int numLights = parser->getNumLights();
	for (int i = 0; i < numLights; i++) {
		SceneLightData lightData;
		parser->getLightData(i, lightData);
		setLight(lightData);
	}

	glEnable(GL_LIGHTING);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// set light colour and position
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat lightAmbient[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPosition[4] = { 0.0f, 4.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);


	if (bumper_pool) {
		drawBumperTable();
		drawBumperStick();
		drawBumperBalls();
	}
	else {
		drawTable();
		drawStick();
		drawBalls();
	}
   
	
	
	glDisable(GL_LIGHTING);
	
	camera->RotateV(-camRotV);
	camera->RotateU(-camRotU);
	camera->RotateW(-camRotW);

	glutSwapBuffers();
}

void onExit()
{
	delete cube;
	delete cylinder;
	delete cone;
	delete sphere;
	delete camera;
	if (parser != NULL) {
		delete parser;
	}

    delete pool;
}


void callback_strike(int id) {
	
	if (!bumper_pool) {
		if (strikeflag) {
			return;
		}
		Point pos = pool->balls[14].position;
		Point temp(pos[0] + 2, pos[1], pos[2]);
		Vector vt(pos[0], pos[1], pos[2]);
		Point temp1;
		if (Force_X == 0) {
			temp1 = trans_mat(vt) * inv_trans_mat(vt)*temp;
		}
		else {
			temp1 = trans_mat(vt)*inv_rotY_mat(DEG_TO_RAD(Force_X)) * inv_trans_mat(vt)*temp;
		}

		//cout << temp1[0] << " " << temp1[2] << endl;
		//cout << temp[0] << " " << temp[2] << endl;
		//glBegin(GL_LINE); //BACK
		//			//	  glNormal3f(0, 1, 0);
		//				  glColor3d(0.5, 0.35, 0.05);
		//glVertex3f(0, 0, 1);
		//glVertex3f(0, 10,1);
		//
		//glEnd();
		//temp1[1] = 0.5;
		Vector tt = temp1 - pos;
		//tt[1] = 0;
		//cout << temp1[0] << " " << temp1[2] << endl;
		tt.normalize();
		tt = tt* Force_Z;
		strikeflag = true;
		pool->strikeBall(SOLID_WHITE, Vector(-tt[0], 0, -tt[2]), 0.1);
	}
	else {
		if (strikeflag) {
			return;
		}
		Point pos = poolbumper->balls[controlball].position;
		Point temp(pos[0] + 2, pos[1], pos[2]);
		Vector vt(pos[0], pos[1], pos[2]);
		Point temp1;
		if (Force_X == 0) {
			temp1 = trans_mat(vt) * inv_trans_mat(vt)*temp;
		}
		else {
			temp1 = trans_mat(vt)*inv_rotY_mat(DEG_TO_RAD(Force_X)) * inv_trans_mat(vt)*temp;
		}

		
		Vector tt = temp1 - pos;
		
		tt.normalize();
		tt = tt* Force_Z;
		strikeflag = true;
		poolbumper->strikeBall(controlball, Vector(-tt[0], 0, -tt[2]), 0.1);
		
	}
  
}
/*
void callback_reset(int id) {
    pool->reset();
}*/
void callback_sp(int id) {

	speical_s = !speical_s;
}

void callback_cc(int id) {
	bumper_pool = radiobumper->get_int_val();
	if (bumper_pool) {
		if (bumper_turn) {
			controlball = radiobumperred->get_int_val();
		}
		else {
			controlball = radiobumperwhite->get_int_val()+5;
		}
	
	}
	
	//int radiochoice = 
	//cout << bumper_pool << endl;
	/*switch (radio->get_int_val())
	{
	case 0:
	
		gluLookAt(eyeX, eyeY, eyeZ,
		 pool->balls[14].position[0], 10, pool->balls[14].position[2],
		 0.0, 1.0, 0.0);
		break;
	case 1:
		gluLookAt(0, 35, 0, 0, 0, 0, 1.0, 0.0, 0.0);
		break;
	case 2:
		break;
	}*/
}
/**************************************** main() ********************/
// handle mouse clicks
void mouse(int button, int state, int x, int y)
{
	// shoot by clicking
	/*if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		cout << x << " " << y << endl;
		if (bumper_pool) {
			for (int i = 0; i < poolbumper->balls.size(); ++i) {
				if ((poolbumper->balls[i].position - Point(x,0.5,y)).length() <= BALL_RADIUS) {
					controlball = i;
					return ;
				}
			}
		}
	}*/
}
int main(int argc, char* argv[])
{
	atexit(onExit);

	/****************************************/
	/*   Initialize GLUT and create window  */
	/****************************************/

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(1000, 800);

	main_window = glutCreateWindow("Final Assignment");
	glutDisplayFunc(myGlutDisplay);
	glutReshapeFunc(myGlutReshape);

	glShadeModel (GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);

	// Specular reflections will be off without this, since OpenGL calculates
	// specular highlights using an infinitely far away camera by default, not
	// the actual location of the camera
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	// Show all ambient light for the entire scene (not one by default)
	GLfloat one[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, one);

	glPolygonOffset(1, 1);
	glutInitWindowPosition(100, 100);


	/****************************************/
	/*         Here's the GLUI code         */
	/****************************************/

	GLUI* glui = GLUI_Master.create_glui("GLUI");
	//GLUI_Panel *motion_pane2 = glui->add_panel("");
//	glui->add_button("Strike Cue Ball", 0, callback_strike);
	//GLUI_Panel *motion_pane3 = glui->add_panel("");
	//glui->add_button("Strike Cue Ball", 0, callback_strike);
	filenameTextField = new GLUI_EditText( glui, "Filename:", filenamePath);
	filenameTextField->set_w(300);
	glui->add_button("Load", 0, callback_load);
	glui->add_button("Reset", 0, callback_reset);
    GLUI_Panel *motion_panel = glui->add_panel("Motion");
	GLUI_Panel *bumper_panel = glui->add_panel("Bumper");
    glui->add_button("Strike Cue Ball", 0, callback_strike);
	GLUI_Panel *camera_panel = glui->add_panel("Camera");
	(new GLUI_Spinner(camera_panel, "RotateV:", &camRotV))
		->set_int_limits(-179, 179);
	(new GLUI_Spinner(camera_panel, "RotateU:", &camRotU))
		->set_int_limits(-179, 179);
	(new GLUI_Spinner(camera_panel, "RotateW:", &camRotW))
		->set_int_limits(-179, 179);
	(new GLUI_Spinner(camera_panel, "Angle:", &viewAngle))
		->set_int_limits(1, 179);

	glui->add_column_to_panel(camera_panel, true);

	GLUI_Spinner* eyex_widget = glui->add_spinner_to_panel(camera_panel, "EyeX:", GLUI_SPINNER_FLOAT, &eyeX);
	eyex_widget->set_float_limits(-100, 100);
	GLUI_Spinner* eyey_widget = glui->add_spinner_to_panel(camera_panel, "EyeY:", GLUI_SPINNER_FLOAT, &eyeY);
	eyey_widget->set_float_limits(-100, 100);
	GLUI_Spinner* eyez_widget = glui->add_spinner_to_panel(camera_panel, "EyeZ:", GLUI_SPINNER_FLOAT, &eyeZ);
	eyez_widget->set_float_limits(-100, 100);

	GLUI_Spinner* lookx_widget = glui->add_spinner_to_panel(camera_panel, "LookX:", GLUI_SPINNER_FLOAT, &lookX);
	lookx_widget->set_float_limits(-100, 100);
	GLUI_Spinner* looky_widget = glui->add_spinner_to_panel(camera_panel, "LookY:", GLUI_SPINNER_FLOAT, &lookY);
	looky_widget->set_float_limits(-100, 100);
	GLUI_Spinner* lookz_widget = glui->add_spinner_to_panel(camera_panel, "LookZ:", GLUI_SPINNER_FLOAT, &lookZ);
	lookz_widget->set_float_limits(-100, 100);
    GLUI_Spinner* force_X = glui->add_spinner_to_panel(motion_panel, "Degree:",GLUI_SPINNER_FLOAT, &Force_X);
    force_X->set_float_limits(-360,360);
    GLUI_Spinner* force_Z = glui->add_spinner_to_panel(motion_panel, "Force:",GLUI_SPINNER_FLOAT, &Force_Z);
    force_Z->set_float_limits(0,10);
	glui-> add_checkbox_to_panel(motion_panel, "Special strike:", NULL,-1, callback_sp);
	glui->add_column_to_panel(motion_panel, true);
	radio = new GLUI_RadioGroup(motion_panel, NULL, 4, callback_cc);
	new GLUI_RadioButton(radio, "SHOOTING");
	new GLUI_RadioButton(radio, "CAM_OVERVIEW");
	//new GLUI_RadioButton(radio, "CAM_CENTER");

	glui->add_column_to_panel(motion_panel, true);
	radiobumper = new GLUI_RadioGroup(motion_panel, NULL, 4, callback_cc);
	new GLUI_RadioButton(radiobumper, "Regular");
	new GLUI_RadioButton(radiobumper, "Bumper");

    glui->add_column_to_panel(motion_panel, true);
	glui->add_column(true);
	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);
	glui->add_column_to_panel(bumper_panel, true);
	radiobumperwhite = new GLUI_RadioGroup(bumper_panel, NULL, 4, callback_cc);
	new GLUI_RadioButton(radiobumperwhite, "spotted white balls");
	new GLUI_RadioButton(radiobumperwhite, "white_1");
	new GLUI_RadioButton(radiobumperwhite, "white_2");
	new GLUI_RadioButton(radiobumperwhite, "white_3");
	new GLUI_RadioButton(radiobumperwhite, "white_4");
	glui->add_column_to_panel(bumper_panel, true);
	radiobumperred = new GLUI_RadioGroup(bumper_panel, NULL, 4, callback_cc);
	new GLUI_RadioButton(radiobumperred, "spotted red balls");
	new GLUI_RadioButton(radiobumperred, "red_1");
	new GLUI_RadioButton(radiobumperred, "red_2");
	new GLUI_RadioButton(radiobumperred, "red_3");
	new GLUI_RadioButton(radiobumperred, "red_4");
	glutMouseFunc(mouse);

	glui->set_main_gfx_window(main_window);
	quadricObject = gluNewQuadric();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	cout << bumper_pool << endl;
	// Renormalize scaled normals so that lighting still works properly.
	glEnable(GL_NORMALIZE);
	callback_load(0);
	loadExternalTextures();
//	init();
	/* We register the idle callback with GLUI, *not* with GLUT */
	GLUI_Master.set_glutIdleFunc(myGlutIdle);

	glutMainLoop();

	return EXIT_SUCCESS;
}



