#ifdef __APPLE__
  #include <GL/glui.h>
#elif defined _WIN32 || defined _WIN64
  #include "Library/Dependencies/glui/glui.h"
#endif
#include "constant.hpp"

extern int windowWidth, windowHieght;
extern GLuint mainWindow;

enum
{
	COLOR_LISTBOX = 0,
	VIEWPOINT_RADIOGROUP,
	FOG_TYPE_RADIOGROUP,
	TRANSLATION_XY,
	TRANSLATION_Z,
	ROTATION,
	SPEED_SPINNER,
	INFO_BUTTON,
	QUIT_BUTTON
};

int moveFlag = 0;
int normalMapFlag = 0;
int trajectoryFlag = 0;


int listbox_item_id = 12;	//  Id of the selected item in the list box
int radiogroup_item_id = 0; //  Id of the selcted radio button
float speed = 1;			//  Spinner Speed Live Variable


int fogFlag = 0;


void myGlutReshape(int w, int h) {
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);
	
	windowWidth = tw;
	windowHieght = tw;


	//if(windowWidth != tw){
	//	windowWidth = tw;
	//	windowHieght = tw * ((float) WINDOW_HEIGHT/ WINDOW_WIDTH);
	//}
	//else {
	//	windowWidth = th * ((float) WINDOW_WIDTH/ WINDOW_HEIGHT);
	//	windowHieght = th;
	//}
}

void glui_callback(int control_id)
{
	//  Notify that this is a GLUI Callback
	printf("GLUI: ");

	//  Behave based on control ID
	switch (control_id)
	{
		//  Color Listbox item changed
	case COLOR_LISTBOX:

		printf("Color List box item changed: ");

		switch (listbox_item_id)
		{
			//  Select black color
		case 1:
			printf("1\n");
			break;
			//  Select blue color
		case 2:
			printf("1\n");
			break;
			//  Select cyan color
		}

		printf("Item %d selected.\n", listbox_item_id);

		break;

		//  A Radio Button in the radio group is selected
	case VIEWPOINT_RADIOGROUP:

		printf("Radio Button %d selected.\n", radiogroup_item_id);

		break;
		

	case SPEED_SPINNER:
		printf("Speed of vehicle : %f.\n", speed);

		break;

	case FOG_TYPE_RADIOGROUP:

		printf("Radio Button %d selected.\n", radiogroup_item_id);

		break;

	case INFO_BUTTON:
		printf("Move Flag : %i\n", moveFlag);
		printf("Normal Flag : %i\n", normalMapFlag);
		printf("Trajectory Flag : %i\n", trajectoryFlag);
		printf("Fog Flag : %i\n", fogFlag);

		break;

	case QUIT_BUTTON:

		printf("Quit Button clicked... Exit!\n");

		exit(1);

		break;

	}
}

void setupGLUI()
{
	//GLUI_Master.set_glutKeyboardFunc();

	GLUI_Master.set_glutReshapeFunc(myGlutReshape);  
	GLUI_Master.set_glutKeyboardFunc(NULL);
	GLUI_Master.set_glutSpecialFunc(NULL);
	GLUI_Master.set_glutMouseFunc(NULL);

	GLUI *glui = GLUI_Master.create_glui("Panel");
	

	// GLUI *glui = GLUI_Master.create_glui_subwindow(mainWindow, GLUI_SUBWINDOW_RIGHT);

	glui->set_main_gfx_window(mainWindow);
	GLUI_StaticText *infoText = glui->add_statictext("SpaceVehicle-CUHK");
	infoText->set_alignment(GLUI_ALIGN_CENTER);

	GLUI_Panel *vehicle_panel = glui->add_panel("Vehicle Panel");

	GLUI_Spinner *spinner = glui->add_spinner_to_panel(vehicle_panel, "Speed", GLUI_SPINNER_FLOAT, &speed, SPEED_SPINNER, glui_callback);

	glui->add_separator();


	GLUI_Panel *op_panel = glui->add_rollout("Object Properties");

	GLUI_Panel *move_panel = glui->add_panel_to_panel(op_panel,"Move/NormalMap");

	glui->add_checkbox_to_panel(move_panel, "Move", &moveFlag);

	glui->add_checkbox_to_panel(move_panel, "Normal Map", &normalMapFlag);

	glui->add_checkbox_to_panel(move_panel, "Trajectory", &trajectoryFlag);

	glui->add_column_to_panel(op_panel, true);


	GLUI_Panel *view_panel = glui->add_panel_to_panel(op_panel, "Viewpoint");

	//  Create radio button group
	GLUI_RadioGroup *ot_group = glui->add_radiogroup_to_panel
	(view_panel, &radiogroup_item_id, VIEWPOINT_RADIOGROUP, glui_callback);

	//  Add the radio buttons to the radio group
	glui->add_radiobutton_to_group(ot_group, "Cube");
	glui->add_radiobutton_to_group(ot_group, "Sphere");



	GLUI_Listbox *color_listbox = glui->add_listbox_to_panel(op_panel,
		"Color", &listbox_item_id, COLOR_LISTBOX, glui_callback);

	color_listbox->add_item(1, "Black");
	color_listbox->add_item(2, "Blue");

	color_listbox->set_int_val(1);


	glui->add_separator();

	GLUI_Panel *fog_panel = glui->add_panel_to_panel(op_panel, "Fog");

	glui->add_checkbox_to_panel(fog_panel, "Fog", &fogFlag);

	glui->add_column_to_panel(fog_panel, true);

	GLUI_RadioGroup *fog_group = glui->add_radiogroup_to_panel
	(fog_panel, &radiogroup_item_id, FOG_TYPE_RADIOGROUP, glui_callback);

	//  Add the radio buttons to the radio group
	glui->add_radiobutton_to_group(fog_group, "Ivory");
	glui->add_radiobutton_to_group(fog_group, "Ocean Blue");


	//GLUI_RadioGroup add_radiogroup(int *live_var = NULL,
	//	int user_id = -1,
	//	GLUI_Update_CB callback = NULL);
	//
	//GLUI_RadioButton button(
	//	add_radiogroup, "test");


	glui->add_button("Info", INFO_BUTTON, glui_callback);

	//  Add the Quit Button
	glui->add_button("Quit", QUIT_BUTTON, glui_callback);
}