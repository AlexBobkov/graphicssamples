#include "Application.h"

int main()
{
	Application app;

	app.initContext();
	app.initGL();
	app.initOthers();
	app.initOVR();
	app.makeScene();
	app.run();		

	return 0;
}