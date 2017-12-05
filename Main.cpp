#include <GL/glui.h>
#include <GL/glut.h>
#include "FFTWaves.h"
#include "Camera.h"
#include <ctime>
#include <iostream>

/** These are the live variables passed into GLUI ***/
int wireframe = 1;
int fill = 1;
int normal = 0;
int lods = 4;
int layers = 2;

int viewAngle = 45;
float eyeX = 0;
float eyeY = 32;
float eyeZ = 68;
float lookX = 0;
float lookY = -.5;
float lookZ = -1;
float clipNear = 0.001;
float clipFar = 1000;

int wavesR = 78;
int wavesG = 112;
int wavesB = 159;
int wavesA = 225;
int lightR = 255;
int lightG = 255;
int lightB = 255;
float dayTime = 14.5;
int compassDir = 260;

float timeSince = 0;
int pause = 0; 
int useFFT = 1; 

int main_window;


/** these are the global variables used for rendering **/
FFTWaves* shape = new FFTWaves(64, 0.00005f, Vector2(0.0f,32.0f), 64);
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
    
    shape->update(timeSince, useFFT);
	if (fill) {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glColor4f(wavesR / 255.0f, wavesG / 255.0f, wavesB / 255.0f, wavesA / 255.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		shape->draw();
	}
	
	glDisable(GL_LIGHTING);
	if (wireframe) {
		glDisable(GL_POLYGON_OFFSET_FILL);
		glColor3f(0.0, 0.0, 0.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shape->draw();
	}

	if (normal) {
		glColor3f(1.0, 0.0, 0.0);
		shape->drawNormal();
	}
	glEnable(GL_LIGHTING);

	glutSwapBuffers();
}

void onExit()
{
	delete shape;
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


	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    glShadeModel (GL_SMOOTH);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    GLfloat light_pos0[] = {100.0f, 100.0f, 100.0f, 0.0f};
    glLightfv (GL_LIGHT0, GL_POSITION, light_pos0);
    
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 100.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glEnable (GL_DEPTH_TEST);

	///****************************************/
	///*          Enable z-buferring          */
	///****************************************/


	glPolygonOffset(1, 1);



	/****************************************/
	/*         Here's the GLUI code         */
	/****************************************/

	GLUI *glui = GLUI_Master.create_glui("GLUI");

	GLUI_Panel *render_panel = glui->add_panel("Render");
	new GLUI_Checkbox(render_panel, "Wireframe", &wireframe);
	new GLUI_Checkbox(render_panel, "Fill", &fill);
	new GLUI_Checkbox(render_panel, "Normal", &normal);
	new GLUI_Checkbox(render_panel, "Use FFT", &useFFT);
	new GLUI_Checkbox(render_panel, "Pause", &pause);
	(new GLUI_Spinner(render_panel, "Num LODs:", &lods))
		->set_int_limits(1, 10);
	(new GLUI_Spinner(render_panel, "Num Layers:", &layers))
		->set_int_limits(1, 10);

	GLUI_Panel *camera_panel = glui->add_panel("Camera");
	(new GLUI_Spinner(camera_panel, "Angle:", &viewAngle))
		->set_int_limits(1, 179);

	GLUI_Spinner* eyex_widget = glui->add_spinner_to_panel(camera_panel, "EyeX:", GLUI_SPINNER_FLOAT, &eyeX);
	eyex_widget->set_float_limits(-50, 1000);
	GLUI_Spinner* eyey_widget = glui->add_spinner_to_panel(camera_panel, "EyeY:", GLUI_SPINNER_FLOAT, &eyeY);
	eyey_widget->set_float_limits(-50, 1000);
	GLUI_Spinner* eyez_widget = glui->add_spinner_to_panel(camera_panel, "EyeZ:", GLUI_SPINNER_FLOAT, &eyeZ);
	eyez_widget->set_float_limits(-50, 1000);

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

	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window);

	/* We register the idle callback with GLUI, *not* with GLUT */
	GLUI_Master.set_glutIdleFunc(myGlutIdle);

	glutMainLoop();

	return EXIT_SUCCESS;
}



