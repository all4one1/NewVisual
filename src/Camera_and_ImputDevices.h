#pragma once
bool gluInvertMatrix(const double m[16], double invOut[16]);
void gluMultiplyMatrix(double a[16], double b[16], double Result[16]);



struct vec
{
	float x, y, z;
	vec() {};
	vec(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {};
	vec(float f) { x = y = z = f; };
	static vec normalize(const vec& v)
	{
		float length = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
		return vec(v.x / length, v.y / length, v.z / length);
	};
	static vec cross(const vec& a, const vec& b)
	{
		return vec(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	};

	static float distance(const vec& a, const vec& b)
	{
		float dx = b.x - a.x;
		float dy = b.y - a.y;
		float dz = b.z - a.z;
		return sqrt(dx * dx + dy * dy + dz * dz);
	}

	vec operator * (float coef)
	{
		return vec(this->x * coef, this->y * coef, this->z * coef);
	}

	vec operator+(const vec& v)
	{
		return vec(this->x + v.x, this->y + v.y, this->z + v.z);
	}
	vec operator-(const vec& v)
	{
		return vec(this->x - v.x, this->y - v.y, this->z - v.z);
	}

	vec operator+=(const vec& v)
	{
		this->x = this->x + v.x;
		this->y = this->y + v.y;
		this->z = this->z + v.z;
		return *this;
	}
	vec operator-=(const vec& v)
	{
		this->x = this->x - v.x;
		this->y = this->y - v.y;
		this->z = this->z - v.z;
		return *this;
	}

	static float radians(double degree)
	{
		return (float)degree * 3.14159265358979323846f / 180;
	};

	void print()
	{
		std::cout << "vec: " << x << " " << y << " " << z << std::endl;
	}
};

struct vec4
{
	float x, y, z, w;
	vec4() {};
	vec4(float x_, float y_, float z_, float w_ = 1) : x(x_), y(y_), z(z_), w(w_) {};
	static vec4 gluMatrixVec4(const double m[16], double v[4])
	{
		double pos[4];

		//pos[0] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3] * v[3];
		//pos[1] = m[4] * v[0] + m[5] * v[1] + m[6] * v[2] + m[7] * v[3];
		//pos[2] = m[8] * v[0] + m[9] * v[1] + m[10] * v[2] + m[11] * v[3];
		//pos[3] = m[12] * v[0] + m[13] * v[1] + m[14] * v[2] + m[15] * v[3];

		pos[0] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12] * v[3];
		pos[1] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13] * v[3];
		pos[2] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14] * v[3];
		pos[3] = m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15] * v[3];


		return vec4((float)pos[0], (float)pos[1], (float)pos[2], (float)pos[3]);
	}
};

struct Camera
{
	vec Pos, Front, Up, Focus;
	vec PosDef, FrontDef, UpDef;
	vec WorldUp, Right;
	vec WorldCursor;
	float Yaw, Pitch, Zoom;
	float WheelSensitivity, RotationSensitivity, MouseSensitivityTranslation, KeyBoardSpeed;


	Camera(float px = 0.0f, float py = 0.0f, float pz = 5.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f)
	{
		RotationSensitivity = 0.05f;
		MouseSensitivityTranslation = 0.005f;
		WheelSensitivity = 1.0f;
		KeyBoardSpeed = 11.5f;
		PosDef = vec(px, py, pz);
		UpDef = vec(upX, upY, upZ);
		WorldUp = vec(0.0f, 1.0f, 0.0f);
		CameraReset();
	}
	void setCamera(float px = 0.0f, float py = 0.0f, float pz = 5.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f)
	{
		PosDef = vec(px, py, pz);
		UpDef = vec(upX, upY, upZ);
		CameraReset();
	}

	void CameraReset() {
		Zoom = 65;
		Yaw = -90.0f;
		Pitch = 0.0f;

		Pos = PosDef;
		Up = UpDef;
		UpdateCameraVectors();
	}
	void MoveCamera(int type) {
		if (type == 0) Pos += Up * KeyBoardSpeed * delta_time;
		if (type == 1) Pos -= Up * KeyBoardSpeed * delta_time;
		if (type == 2) Pos -= Right * KeyBoardSpeed * delta_time;
		if (type == 3) Pos += Right * KeyBoardSpeed * delta_time;
	}
	void ProcessMouseRotation(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= RotationSensitivity;
		yoffset *= RotationSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
		UpdateCameraVectors();
	}
	void ProcessMouseMovementTranslation(float xoffset, float yoffset)
	{
		xoffset *= MouseSensitivityTranslation;
		yoffset *= MouseSensitivityTranslation;
		Pos += Up * yoffset;
		Pos += Right * xoffset;

		UpdateCameraVectors();
	}
	void ProcessMouseScroll(int yoffset, float ampl = 1)
	{
		if (yoffset > 0) Pos += Front * WheelSensitivity * ampl;
		if (yoffset < 0) Pos -= Front * WheelSensitivity * ampl;
	}
	void saveCameraState()
	{
		std::ofstream w("camerastate.dat");
		w << Pos.x << " " << Pos.y << " " << Pos.z << " ";
		w << Up.x << " " << Up.y << " " << Up.z << " ";
		w << Zoom << " " << Yaw << " " << Pitch << endl;
	}
	void readCameraState()
	{
		std::ifstream r("camerastate.dat");
		r >> Pos.x >> Pos.y >> Pos.z;
		r >> Up.x >> Up.y >> Up.z;
		r >> Zoom >> Yaw >> Pitch;
		UpdateCameraVectors();
	}


	~Camera() {};
private:
	void UpdateCameraVectors()
	{
		auto radians = [](double degree) {return (float)degree * 3.14159265358979323846f / 180; };

		vec front;
		front.x = cosf(radians(Yaw)) * cosf(radians(Pitch));
		front.y = sinf(radians(Pitch));
		front.z = sinf(radians(Yaw)) * cosf(radians(Pitch));
		Front = vec::normalize(front);
		Right = vec::normalize(vec::cross(Front, WorldUp));
		Up = vec::normalize(vec::cross(Right, Front));

	}
};

struct MouseState
{
	float lastX, lastY; //previous position pixels
	float PosX, PosY; //position pixels
	float shiftX, shiftY; //difference between former two 
	float PosXn, PosYn; //normalized coords
	float realX, realY, realZ; //physical coords
	float z_cursor = 0.5f;

	bool firstMouse = true;
	bool onPressed[3] = { false, false, false };
	bool leftPressed = false;
	bool rightPressed = false;
	bool middlePressed = false;

	void init(int width, int height)
	{
		lastX = (float)width / 2;
		lastY = (float)height / 2;
		firstMouse = true;
	}
	void normalized(float xpos, float ypos)
	{
		PosXn = (-1.0f + 2.0f * xpos / width);
		PosYn = (1.0f - 2.0f * ypos / height);
	}

	void mouseShift(float xpos, float ypos)
	{
		//if (xpos > 0 && xpos < width && ypos > 0 && ypos < height)
		{
			if (firstMouse)
			{
				lastX = xpos;
				lastY = ypos;
				firstMouse = false;
			}

			shiftX = xpos - lastX;
			shiftY = ypos - lastY;

			lastX = xpos;
			lastY = ypos;


		}
	}

	void getCursorPosition(float xpos, float ypos)
	{
		normalized(xpos, ypos);

		//glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);



		GLdouble inv[16];
		GLdouble product[16];
		gluInvertMatrix(projmatrix, inv);


		gluMultiplyMatrix(projmatrix, mvmatrix, product);
		gluInvertMatrix(product, inv);


		float zz = z_cursor;
		double screenPos[4] = { PosXn, PosYn, zz, 1.0f };

		vec4 Pos = vec4::gluMatrixVec4(inv, screenPos);
		realX = Pos.x / Pos.w;
		realY = Pos.y / Pos.w;
		realZ = Pos.z / Pos.w;


		zz = 1;
		float dz = 0.1f;
		int n = 0;
		while (1)
		{
			zz = zz - dz;
			double screenPos[4] = { PosXn, PosYn, zz, 1.0f };

			vec4 Pos = vec4::gluMatrixVec4(inv, screenPos);
			realX = Pos.x / Pos.w;
			realY = Pos.y / Pos.w;
			realZ = Pos.z / Pos.w;
			//cout << zz << " " << realZ << endl;
			if (realZ > z_cursor)
			{
				zz = zz + dz;
				dz = dz / 2;
			}
			n++;
			//system("pause");
			if (abs(realZ - z_cursor) < 0.01) break;
		}
	}



	void detectPressed(int key, int state, float xpos, float ypos)
	{

		if (state == GLUT_DOWN)
		{
			lastX = xpos;
			lastY = ypos;
			onPressed[key] = 1;
		}
		if (state == GLUT_UP)
		{
			onPressed[key] = 0;
		}
	}

};





bool gluInvertMatrix(const double m[16], double invOut[16])
{
	double inv[16], det;
	int i;

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return true;
}


void gluMultiplyMatrix(double a[16], double b[16], double Result[16])
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float SumElements = 0.0f;
			for (int k = 0; k < 4; k++)
			{
				SumElements += (float)(a[i + 4 * k] * b[k + 4 * j]);
			}
			Result[i + 4 * j] = SumElements;
		}
	}
}
