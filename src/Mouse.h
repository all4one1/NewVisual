#pragma once
//
//void mouseMovePassive(int x, int y) 
//{
//	//cout << x << " " << y << endl;
//
//
//}
void mouseMoveClick(int x, int y)
{
	int stat = glutGetModifiers();
	//if (!(x > 0 && x < width && y > 0 && y < height)) return;

	float xpos = (float)x;
	float ypos = (float)y;

	MS.mouseShift(xpos, ypos);

	if (stat == 0)
	{
		if (MS.onPressed[RightButton])
		{
			Cam.ProcessMouseMovementTranslation(MS.shiftX, MS.shiftY);
		}

		if (MS.onPressed[LeftButton])
		{
			Cam.ProcessMouseRotation(MS.shiftX, MS.shiftY);
		}

	}
}
void mouseClick(int key, int state, int x, int y) {

	int stat = glutGetModifiers();
	if (state == GLUT_DOWN)
	{

		if (stat == GLUT_ACTIVE_SHIFT)
		{
			//MS.normalized(x, y);

			//cout << x << " " << y << " " << MS.PosXn << " " << MS.PosYn << endl;
			if (key == LeftButton)
			{
				MS.getCursorPosition((float)x, (float)y);
				//WI.accept_a_click(MS.realX, MS.realY, MS.realZ);
			}


			if (key == RightButton)
			{
				MS.getCursorPosition((float)x, (float)y);
				cout << "Normalized coords: " << "x= " << MS.PosXn << ", y= " << MS.PosYn << " " << endl;
				cout << "Physical   coords: " << "x= " << MS.realX << ", y= " << MS.realY << ", z= " << MS.realZ << endl;
			}

			if (key == MiddleButton)
			{

			}
		}
	}


	MS.detectPressed(key, state, (float)x, (float)y);
}
void mouseWheel(int button, int dir, int x, int y)
{
	int stat = glutGetModifiers();

	if (stat == 0)
	{
		Cam.ProcessMouseScroll(dir);
	}

	if (stat == GLUT_ACTIVE_SHIFT)
	{
		Cam.ProcessMouseScroll(dir, 5);
	}

}

