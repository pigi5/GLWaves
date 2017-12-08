/**
 * Author: Ford Hash
 * Date Modified: 12/7/17
 */

#include <GL/glui.h>
#include <GL/glut.h>
#include "FFTWaves.h"
#include "Skybox.h"
#include "Camera.h"
#include <ctime>
#include <iostream>

#define CB_BIRDSEYE 1
#define CB_RESET 2

int WORLD_SIZE = 2048;
/** These are the live variables passed into GLUI ***/
GLUI *glui;

int wireframe = 1;
int fill = 1;

int viewAngle = 45;
float eyeX = WORLD_SIZE / 2;
float eyeY = 6;
float eyeZ = WORLD_SIZE / 2;
float lookX = 0;
float lookY = -.17;
float lookZ = -1;
float clipNear = 0.001;
float clipFar = 4000;

int wavesR = 40;
int wavesG = 50;
int wavesB = 70;
int wavesA = 255;
int lightR = 255;
int lightG = 255;
int lightB = 255;
float dayTime = 11;
int compassDir = 140;

float timeSince = 0;
int pause = 0;

int detail = 8;
float amp = 0.0001f;
float windX = 10.0f;
float windZ = 32.0f;
int tileSize = 6;
int lods = 5;
int lodLength = 6;
int foam = 0;

int main_window;


/** these are the global variables used for rendering **/
FFTWaves* waves;
Skybox* skybox;
Camera* camera = new Camera();

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

/***************************************** myGlutDisplay() *****************/

void myGlutDisplay(void)
{
    if (!pause) {
        timeSince = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    }
    
    GLfloat diffuse[] = {lightR / 255.0f, lightG / 255.0f, lightB / 255.0f, 0.0f};
    float distance = 1000.0f;
    GLfloat light_pos0[] = {distance * sin((dayTime - 12) * 2 * PI / 12.0) * cos(DEG_TO_RAD(compassDir)), distance * cos((dayTime - 12) * 2 * PI / 12.0), distance * sin((dayTime - 12) * 2 * PI / 12.0) * sin(DEG_TO_RAD(compassDir)), 0.0f};
    glLightfv (GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv (GL_LIGHT0, GL_POSITION, light_pos0);

	glClearColor(.9f, .9f, .9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera->SetViewAngle(viewAngle);
	camera->SetNearPlane(clipNear);
	camera->SetFarPlane(clipFar);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	Matrix projection = camera->GetProjectionMatrix();
	glMultMatrixd(projection.unpack());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Point eyeP(eyeX, eyeY, eyeZ);
	Vector lookV(lookX, lookY, lookZ);
	Vector upV(0, 1, 0);
	camera->Orient(eyeP, lookV, upV);
	Matrix modelView = camera->GetModelViewMatrix();
	glMultMatrixd(modelView.unpack());
    
	glDisable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
    glColor4f(1, 1, 1, 1);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    skybox->draw();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    if (!waves) {
        waves = new FFTWaves(pow(2, detail), amp, Vector2(windX, windZ), pow(2, tileSize), WORLD_SIZE);
    } else if (!waves->testAttrs(detail, amp, windX, windZ, tileSize)) {
        delete waves;
        waves = new FFTWaves(pow(2, detail), amp, Vector2(windX, windZ), pow(2, tileSize), WORLD_SIZE);
    }
    
    Point centerPoint;
    lookV.normalize();
    float t, centerX, centerZ;
	if (fill || wireframe) {
        waves->update(timeSince);

        if (intersectYPlane(eyeP, lookV, t)) {
            centerPoint = eyeP + lookV * t;
            centerX = clamp(centerPoint[0], 0, WORLD_SIZE);
            centerZ = clamp(centerPoint[2], 0, WORLD_SIZE);

	        if (fill) {
		        glEnable(GL_POLYGON_OFFSET_FILL);
		        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                waves->setColor(wavesR / 255.0f, wavesG / 255.0f, wavesB / 255.0f, wavesA / 255.0f, foam);
		        waves->draw(centerX, centerZ, lods, lodLength);
	        }
	
	        glDisable(GL_LIGHTING);
	        if (wireframe) {
		        glDisable(GL_POLYGON_OFFSET_FILL);
		        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                waves->setColor(0.0, 0.0, 0.0, 1.0, false);
		        waves->draw(centerX, centerZ, lods, lodLength);
	        }

	        glEnable(GL_LIGHTING);
        }
    }

	glutSwapBuffers();
}

void onExit()
{
	if(waves) delete waves;
	if(camera) delete camera;
    if(skybox) delete skybox;
}

void glui_cb(int control) {
	switch(control) {
	case CB_BIRDSEYE:
        viewAngle = 45;
        eyeX = WORLD_SIZE / 2;
        eyeY = 100;
        eyeZ = WORLD_SIZE / 2;
        lookX = 0;
        lookY = -1;
        lookZ = -0.000001;
        clipNear = 0.001;
        clipFar = 4000;
		break;
	case CB_RESET:
        viewAngle = 45;
        eyeX = WORLD_SIZE / 2;
        eyeY = 6;
        eyeZ = WORLD_SIZE / 2;
        lookX = 0;
        lookY = -.17;
        lookZ = -1;
        clipNear = 0.001;
        clipFar = 4000;
		break;
	}

    glui->sync_live();
	glutPostRedisplay();
}

/**************************************** main() ********************/

int main(int argc, char* argv[])
{
	atexit(onExit);

	/****************************************/
	/*   Initialize GLUT and create window  */
	/****************************************/

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(500, 500);

	main_window = glutCreateWindow("CSI 4341: Assignment 2");
	glutDisplayFunc(myGlutDisplay);
	glutReshapeFunc(myGlutReshape);

	/****************************************/
	/*       Set up OpenGL lighting         */
	/****************************************/


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glShadeModel(GL_SMOOTH);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    GLfloat light_pos0[] = {100.0f, 100.0f, 100.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
    
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 100.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    if (!skybox) {
        skybox = new Skybox(WORLD_SIZE, WORLD_SIZE, WORLD_SIZE);
    }

	///****************************************/
	///*          Enable z-buferring          */
	///****************************************/


	glPolygonOffset(1, 1);



	/****************************************/
	/*         Here's the GLUI code         */
	/****************************************/

	glui = GLUI_Master.create_glui("GLUI");

	GLUI_Panel *render_panel = glui->add_panel("Render");
	new GLUI_Checkbox(render_panel, "Wireframe", &wireframe);
	new GLUI_Checkbox(render_panel, "Fill", &fill);
	new GLUI_Checkbox(render_panel, "Pause", &pause);

	GLUI_Panel *camera_panel = glui->add_panel("Camera");
	(new GLUI_Spinner(camera_panel, "Angle:", &viewAngle))
		->set_int_limits(1, 179);

	GLUI_Spinner* eyex_widget = glui->add_spinner_to_panel(camera_panel, "EyeX:", GLUI_SPINNER_FLOAT, &eyeX);
	eyex_widget->set_float_limits(0, WORLD_SIZE);
	GLUI_Spinner* eyey_widget = glui->add_spinner_to_panel(camera_panel, "EyeY:", GLUI_SPINNER_FLOAT, &eyeY);
	eyey_widget->set_float_limits(-50, WORLD_SIZE / 2);
	GLUI_Spinner* eyez_widget = glui->add_spinner_to_panel(camera_panel, "EyeZ:", GLUI_SPINNER_FLOAT, &eyeZ);
	eyez_widget->set_float_limits(0, WORLD_SIZE);

	GLUI_Spinner* lookx_widget = glui->add_spinner_to_panel(camera_panel, "LookX:", GLUI_SPINNER_FLOAT, &lookX);
	lookx_widget->set_float_limits(-5, 5);
	GLUI_Spinner* looky_widget = glui->add_spinner_to_panel(camera_panel, "LookY:", GLUI_SPINNER_FLOAT, &lookY);
	looky_widget->set_float_limits(-5, 5);
	GLUI_Spinner* lookz_widget = glui->add_spinner_to_panel(camera_panel, "LookZ:", GLUI_SPINNER_FLOAT, &lookZ);
	lookz_widget->set_float_limits(-5, 5);

	GLUI_Spinner* clipN_widget = glui->add_spinner_to_panel(camera_panel, "Near:", GLUI_SPINNER_FLOAT, &clipNear);
	clipN_widget->set_float_limits(0, 10);
	GLUI_Spinner* clipF_widget = glui->add_spinner_to_panel(camera_panel, "Far:", GLUI_SPINNER_FLOAT, &clipFar);
	clipF_widget->set_float_limits(0, 10000);

	glui->add_column(true);
    
	GLUI_Panel *object_panel = glui->add_panel("Light & Color");
	(new GLUI_Spinner(object_panel, "Waves R:", &wavesR))
		->set_int_limits(0, 255);
	(new GLUI_Spinner(object_panel, "Waves G:", &wavesG))
		->set_int_limits(0, 255);
	(new GLUI_Spinner(object_panel, "Waves B:", &wavesB))
		->set_int_limits(0, 255);
	(new GLUI_Spinner(object_panel, "Waves A:", &wavesA))
		->set_int_limits(0, 255);
	(new GLUI_Spinner(object_panel, "Light R:", &lightR))
		->set_int_limits(0, 255);
	(new GLUI_Spinner(object_panel, "Light G:", &lightG))
		->set_int_limits(0, 255);
	(new GLUI_Spinner(object_panel, "Light B:", &lightB))
		->set_int_limits(0, 255);
	(new GLUI_Spinner(object_panel, "Time:", &dayTime))
		->set_float_limits(0, 24);
	(new GLUI_Spinner(object_panel, "Compass:", &compassDir))
		->set_int_limits(0, 359);
    GLUI_Button *action_button = glui->add_button_to_panel(object_panel, "Bird's Eye", CB_BIRDSEYE, glui_cb);
    GLUI_Button *reset_button = glui->add_button_to_panel(object_panel, "Reset View", CB_RESET, glui_cb);

	GLUI_Panel *wave_panel = glui->add_panel("Wave Attributes");
	(new GLUI_Spinner(wave_panel, "Detail:", &detail))
		->set_int_limits(2, 10);
	GLUI_Spinner* amp_widget = glui->add_spinner_to_panel(wave_panel, "Amplitude:", GLUI_SPINNER_FLOAT, &amp);
	amp_widget->set_float_limits(0, 0.001f);
	GLUI_Spinner* windX_widget = glui->add_spinner_to_panel(wave_panel, "WindX:", GLUI_SPINNER_FLOAT, &windX);
	windX_widget->set_float_limits(-32.0f, 32.0f);
	GLUI_Spinner* windZ_widget = glui->add_spinner_to_panel(wave_panel, "WindZ:", GLUI_SPINNER_FLOAT, &windZ);
	windZ_widget->set_float_limits(-32.0f, 32.0f);
	(new GLUI_Spinner(wave_panel, "Tile Size:", &tileSize))
		->set_int_limits(0, 8);
	(new GLUI_Spinner(wave_panel, "Num LODs:", &lods))
		->set_int_limits(1, 10);
	(new GLUI_Spinner(wave_panel, "Num Layers:", &lodLength))
		->set_int_limits(1, 10);
	new GLUI_Checkbox(wave_panel, "\"Foam\"", &foam);

	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window);

	/* We register the idle callback with GLUI, *not* with GLUT */
	GLUI_Master.set_glutIdleFunc(myGlutIdle);

	glutMainLoop();

	return EXIT_SUCCESS;
}



