#pragma once
void keyboard(unsigned char key, int x, int y)
{
	float a = 0.5;

	switch (key)
	{
	case 'r':
	case 'R':
		define_default_camera();
		break;
	case 'z':
		alpha += 1;
		//cout << "alpha = " << alpha << endl;
		break;
	case 'x':
		alpha += -1;
		//cout << "alpha = " << alpha << endl;
		break;
	case '1':

		break;
	case '2':


		break;
	case 'n':
	{
		std::ifstream read = FILES.getNextFile();
		read_3d_fields(read);

		for (int i = 0; i < NN; i++) P[i].REgenerate(Lx, Ly, Lz);
		//for (int i = 0; i < NN; i++) P[i].reset_tracers();

		break;
	}
	case 't':
		_test = true;
		break;
	case 'c':
		P->reset_tracers();
		std::cout << "cleared" << std::endl;
		break;
	case 'i':
		py = py + 0.5f;
		glutPostRedisplay();
		break;
	case 'k':
		py = py - 0.5f;
		glutPostRedisplay();
		break;
	case 'j':
		px = px - 0.5f;
		glutPostRedisplay();
		break;
	case 'l':
		px = px + 0.5f;
		glutPostRedisplay();
		break;
	case 'u':
		pz = pz - 0.5f;
		glutPostRedisplay();
		break;
	case 'o':
		pz = pz + 0.5f;
		glutPostRedisplay();
		break;
	case 'w':
		ty = ty + 0.5f;

		glutPostRedisplay();
		break;
	case 's':
		ty = ty - 0.5f;

		glutPostRedisplay();
		break;
	case 'a':
		tx = tx - 0.5f;
		glutPostRedisplay();
		break;
	case 'd':
		tx = tx + 0.5f;
		glutPostRedisplay();
		break;
	case 'q':
		tz = tz - 0.5f;
		glutPostRedisplay();
		break;
	case 'e':
		tz = tz + 0.5f;
		glutPostRedisplay();
		break;
	case 'm':
		glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
		glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
		show_matrix(projmatrix);
		show_matrix(mvmatrix);
		break;
	default:
		break;
	}


}
void keyboard_spec(int key, int x, int y) {
	int stat = glutGetModifiers();
	if (stat == 0) {
		switch (key)
		{
		case GLUT_KEY_UP:
			transY += -0.1f;
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			transY += 0.1f;
			break;
		case GLUT_KEY_LEFT:
			transX += 0.1f;
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			transX += -0.1f;
			glutPostRedisplay();
			break;
		}
	}
	if (stat == GLUT_ACTIVE_SHIFT) {
		switch (key)
		{
		case GLUT_KEY_UP:
			angleX += -5;
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			angleX += 5;
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:
			angleY += -5;
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			angleY += 5;
			glutPostRedisplay();
			break;
		}
	}
	if (stat == GLUT_ACTIVE_ALT) {
		switch (key)
		{
		case GLUT_KEY_UP:
			change_normal(1);
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			change_normal(-1);
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:
			angleZ += -5;
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			angleZ += 5;
			glutPostRedisplay();
			break;
		}
	}
}


void keyboard2(unsigned char key, int x, int y)
{
	float a = 0.5;

	switch (key)
	{
	case 'r':

		break;
	case 'z':
		alpha += 1;
		//cout << "alpha = " << alpha << endl;
		break;
	case 'x':
		alpha += -1;
		//cout << "alpha = " << alpha << endl;
		break;
	case '1':
		png_iter++;
		saveFrame(_pathGL("pictures") + _strGLmask(png_iter, 5), 0, 0);
		cout << png_iter << " png saved " << endl;
		break;
	case '2':


		break;
	case 'n':
	{
		std::ifstream read = FILES.getNextFile();
		read_3d_fields(read);
		for (int i = 0; i < NN; i++) P[i].REgenerate(Lx, Ly, Lz);
		//for (int i = 0; i < NN; i++) P[i].reset_tracers();

		break;
	}
	case 't':
		_test = true;
		break;
	case 'c':
		P->reset_tracers();
		std::cout << "cleared" << std::endl;
		break;


	case 'w':
		Cam.MoveCamera(0); // Up
		break;
	case 's':
		Cam.MoveCamera(1); // Down
		break;
	case 'a':
		Cam.MoveCamera(2); // Left
		break;
	case 'd':
		Cam.MoveCamera(3); // Right
		break;

	case 'p':
		cout << endl << "Camera look-at function parameters: " << endl;
		cout << "Position: " << Cam.Pos.x << " " << Cam.Pos.y << " " << Cam.Pos.z << endl;
		cout << "Front: " << Cam.Front.x << " " << Cam.Front.y << " " << Cam.Front.z << endl;
		cout << "Up: " << Cam.Up.x << " " << Cam.Up.y << " " << Cam.Up.z << endl;
		break;
	case 'm':
		glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
		glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
		show_matrix(projmatrix);
		show_matrix(mvmatrix);
		break;
	case 'S':
		Cam.saveCameraState();
		cout << "Camtera state saved " << endl;
		break;
	case 'R':
		cout << "Camera state read " << endl;
		Cam.readCameraState();
		break;
	case '+':
		Cam.Zoom += 1;
		cout << "Zoom: " << Cam.Zoom << endl;
		break;
	case '-':
		Cam.Zoom -= 1;
		cout << "Zoom: " << Cam.Zoom << endl;
		break;
	default:
		break;
	}


}