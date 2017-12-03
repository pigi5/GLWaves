#include <GL/glui.h>
#include <GL/glut.h>
#include "FFTWaves.h"
#include "Camera.h"
#include <ctime>
#include <iostream>

/** These are the live variables passed into GLUI ***/
int  wireframe = 1;
int  fill = 1;
int  normal = 0;
int  lods = 4;
int  layers = 2;
int	 rotX = 0;
int	 rotY = 0;
int	 rotZ = 0;
int  scale = 50;

int	 camRotU = 0;
int	 camRotV = 0;
int	 camRotW = 0;
int  viewAngle = 45;
float eyeX = 1000;
float eyeY = 500;
float eyeZ = 1000;
float lookX = -2;
float lookY = -.5;
float lookZ = -2;
float clipNear = 0.001;
float clipFar = 10000;


int  main_window;


/** these are the global variables used for rendering **/
FFTWaves* shape = new FFTWaves(64, 0.0005f, Vector2(0.0f,32.0f), 64, false);
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
    float timeSince = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

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
	camera->RotateV(camRotV);
	camera->RotateU(camRotU);
	camera->RotateW(camRotW);
	Matrix modelView = camera->GetModelViewMatrix();
	glMultMatrixd(modelView.unpack());

	//rotate object
	glRotatef(rotX, 1.0, 0.0, 0.0);
	glRotatef(rotY, 0.0, 1.0, 0.0);
	glRotatef(rotZ, 0.0, 0.0, 1.0);

	//drawing the axes
    glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0, 0, 0); glVertex3f(1.0, 0, 0);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0, 0, 0); glVertex3f(0.0, 1.0, 0);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0, 0, 0); glVertex3f(0, 0, 1.0);
	glEnd();

	//scale object
	glScalef(scale, scale, scale);

	if (normal) {
		glColor3f(1.0, 0.0, 0.0);
		shape->drawNormal(timeSince);
	}

    glEnable(GL_LIGHTING);
	if (fill) {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glColor3f(0.5, 0.5, 0.5);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		shape->draw(timeSince, true);
	}
	
	if (wireframe) {
		glDisable(GL_POLYGON_OFFSET_FILL);
		glColor3f(0.0, 0.0, 0.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shape->draw(timeSince, true);
	}

	camera->RotateV(-camRotV);
	camera->RotateU(-camRotU);
	camera->RotateW(-camRotW);

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


	    glClearColor (0.38, 0.38, 0.38, 0.0);
        glShadeModel (GL_SMOOTH);

        GLfloat light_pos0[] = {1000.0f, 1000.0f, 1000.0f, 0.0f};
        GLfloat diffuse[] = {0.5f, 0.5f, 0.5f, 0.0f};
        GLfloat ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};

        glLightfv (GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv (GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv (GL_LIGHT0, GL_POSITION, light_pos0);

        glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable (GL_COLOR_MATERIAL);

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
	(new GLUI_Spinner(render_panel, "Num LODs:", &lods))
		->set_int_limits(1, 10);
	(new GLUI_Spinner(render_panel, "Num Layers:", &layers))
		->set_int_limits(1, 10);

	GLUI_Panel *camera_panel = glui->add_panel("Camera");
	(new GLUI_Spinner(camera_panel, "RotateV:", &camRotV))
		->set_int_limits(-179, 179);
	(new GLUI_Spinner(camera_panel, "RotateU:", &camRotU))
		->set_int_limits(-179, 179);
	(new GLUI_Spinner(camera_panel, "RotateW:", &camRotW))
		->set_int_limits(-179, 179);
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


	GLUI_Panel *object_panel = glui->add_panel("Object");
	(new GLUI_Spinner(object_panel, "Rotate X:", &rotX))
		->set_int_limits(0, 359);
	(new GLUI_Spinner(object_panel, "Rotate Y:", &rotY))
		->set_int_limits(0, 359);
	(new GLUI_Spinner(object_panel, "Rotate Z:", &rotZ))
		->set_int_limits(0, 359);
	(new GLUI_Spinner(object_panel, "Scale:", &scale))
		->set_int_limits(1, 100);

	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window);

	/* We register the idle callback with GLUI, *not* with GLUT */
	GLUI_Master.set_glutIdleFunc(myGlutIdle);

	glutMainLoop();

	return EXIT_SUCCESS;
}



