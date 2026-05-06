#include <stdio.h>
#include <omp.h>
#include <fstream> 
#include <sstream> 
#include <ctime>
#include <iostream> 
#include <vector>
#include "windows.h"
#include <iomanip>
#include <random>
#include "..\GL\freeglut.h"



using namespace std;
#define pause system("pause");
#define timer timer2 = clock()/CLOCKS_PER_SEC; 	cout << "time (seconds)= " << (timer2 - timer1) << endl;
#define show_matrix(matrix) 	for (int j = 0; j < 4; j++) { for (int i = 0; i < 4; i++) cout << matrix[i + j * 4] << " "; 		cout << endl; } cout << "_______________" << endl;
enum MouseClick { LeftButton, MiddleButton, RightButton };


//parameters of graphics
double test_var = 100.0;
float tx, ty, tz, px, py, pz;
float nx, ny, nz;
float transX, transY, transZ;
float angleX, angleY, angleZ;
float fMin, fMax, alpha;
unsigned int Size, iter_draw = 0;
float *R_, *G_, *B_;
GLdouble wx, wy, wz;
GLdouble mvmatrix[16], projmatrix[16];
float *X, *Y;
double *XX, *YY, *ZZ;
int width, height;
float lt;

void define_default_camera() {
	tx = 8; 	ty = 3.5; 	tz = 6.5;
	px = -4.5;	py = -0.5;	pz = -2;
	fMin = 0.5;	fMax = 100.0; 	alpha = 75.0;
	transX = 0, transY = 0, transZ = -1;
	angleX = 0, angleY = 0, angleZ = 0;
}




int N_grid, off, off2;
int Nx, Ny, Nz;
double tau, tau2;
double t = 0;
double Lx, Ly, Lz;
double hx, hy, hz;
double *vx, *vy, *vz;
double *fx, *fy, *fz, *fx0, *fy0, *fz0;
double timeq = 0;
unsigned int timer1, timer2, iter = 0, png_iter, tt;
unsigned int last_time = 0, current_time = 0;
double delta_time = 1, event_timer = 0;
double Vmax, Vmin, VyColorMax = 0, VyColorMin = 0;
bool _test = false;

#define AMPLIFY 1
vector <double> traceX;
vector <double> traceY;
vector <double> traceZ;
#define NN 2000
double x[NN], y[NN], z[NN], R[NN], G[NN], B[NN];

#include "Reader.h"
#include "Saver.h"

struct part
{
	vector <double> traceX;
	vector <double> traceY;
	vector <double> traceZ;
	double x, y, z, R, G, B;
	double vx, vy, vz;

	part() {
		REgenerate(1, 1, 1);
	}
	void REgenerate(double Lx, double Ly, double Lz) {
		std::random_device rd;
		std::default_random_engine generator(rd());
		std::uniform_real_distribution <double> gLx(0.0, Lx), gLy(0.0, Ly), gLz(0.0, Lz), g1(0.0, 1.0), g05(0.3, 1.0);
		std::uniform_real_distribution <double> within(5, 15);
		for (int i = 0; i < NN; i++) {
			x = gLx(generator);
			//x = within(generator); 
			//x = 0.5;
			y = gLy(generator); 
			//y = 0.4;
			z = gLz(generator); 
			//z = Lz * 0.8;

			R = g1(generator);
			G = g05(generator);
			B = x / Lx; //g1(generator);
		}
		reset_tracers();
	}
	void reset_tracers()
	{
		traceX.clear();
		traceY.clear();
		traceZ.clear();
	}

};


part P[NN];



#include "Camera_and_ImputDevices.h"
Camera Cam;
MouseState MS;




void random_generator(double *x, double *y, double *z, double *vx, double *vy, double *vz) {
	random_device rd;
	default_random_engine generator(rd());
	uniform_real_distribution <double> unix(0, Lx);
	uniform_real_distribution <double> uniy(Ly*0.1, Ly*0.9);
	uniform_real_distribution <double> uniz(0, Lz);
	uniform_real_distribution <double> uni_v(-Lx / 100, Lx / 100);
	normal_distribution <double> gauss(0.0, Lx);
	int q = 0;
	double RR = 2.5; // effective RNG radius
	for (int i = 0; i < N_grid; i++) {
		x[i] = unix(generator);
		y[i] = uniy(generator);
		z[i] = uniz(generator);
		vx[i] = 0; //uni_v(generator);
		vy[i] = 0; //uni_v(generator);
		vz[i] = 0; //uni_v(generator);
	}
	cout << "rebuildings = " << q << endl;
}
void RGB_generate(float *R, float *G, float *B) {
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_real_distribution <float> g1(0.0, 6.0), g3(0.0, 1.0), g5(0.0, 1.0), g05(0.5, 1.0);

	for (int i = 0; i < N_grid; i++) {
		R[i] = g1(generator);
		G[i] = g05(generator);
		B[i] = 0.0;
	}
}

//OpenGL functions
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();



	gluPerspective(alpha, 1.0, fMin, fMax);

	glMatrixMode(GL_MODELVIEW);

}


void trace(double& x, double& y, double& z, double* vx, double* vy, double* vz, unsigned int offset, unsigned int offset2, unsigned int nx, unsigned int ny, unsigned int nz, double hx, double hy, double hz, double tau, double &VX_, double &VY_, double &VZ_)
{
	unsigned int i1, j1, k1, i2, j2, k2;
	double VX, VY, VZ, x1, x2, y1, y2, z1, z2, h3;
	double inp;
	h3 = hx*hy*hz;
#define zero 1e-10

	//i1, i2
	if (modf((x / hx), &inp) < zero) {
		i1 = unsigned int(x / hx);
		i2 = i1 + 1;
		if (i1 == nx) {
			i2 = nx;
			i1 = i2 - 1;
		}
	}
	else {
		i1 = unsigned int(floor(x / hx));
		i2 = unsigned int(ceil(x / hx));
	}

	//j1,j2
	if (modf((y / hy), &inp) < zero) {
		j1 = unsigned int(y / hy);
		j2 = j1 + 1;
		if (j1 == ny) {
			j2 = ny;
			j1 = j2 - 1;
		}
	}
	else {
		j1 = unsigned int(floor(y / hy));
		j2 = unsigned int(ceil(y / hy));
	}

	//k1,k2
	if (modf((z / hz), &inp) < zero) {
		k1 = unsigned int(z / hz);
		k2 = k1 + 1;
		if (k1 == nz) {
			k2 = nz;
			k1 = k2 - 1;
		}
	}
	else {
		k1 = unsigned int(floor(z / hz));
		k2 = unsigned int(ceil(z / hz));
	}


	x1 = hx*i1; x2 = hx*i2;
	y1 = hy*j1; y2 = hy*j2;
	z1 = hz*k1; z2 = hz*k2;


	VX = vx[i1 + offset*j1 + offset2*k1] * (x2 - x)* (y2 - y) * (z2 - z) +
		 vx[i1 + offset*j1 + offset2*k2] * (x2 - x)* (y2 - y) * (-z1 + z) +
		 vx[i1 + offset*j2 + offset2*k1] * (x2 - x)* (-y1 + y) * (z2 - z) +
		 vx[i1 + offset*j2 + offset2*k2] * (x2 - x)* (-y1 + y) * (-z1 + z) +
		 vx[i2 + offset*j1 + offset2*k1] * (-x1 + x)* (y2 - y) * (z2 - z) +
		 vx[i2 + offset*j1 + offset2*k2] * (-x1 + x)* (y2 - y) * (-z1 + z) +
		 vx[i2 + offset*j2 + offset2*k1] * (-x1 + x)* (-y1 + y) * (z2 - z) +
		 vx[i2 + offset*j2 + offset2*k2] * (-x1 + x)* (-y1 + y) * (-z1 + z);
	VX = VX / h3;

	VY= vy[i1 + offset*j1 + offset2*k1]* (x2 - x)* (y2 - y) * (z2 - z) +
		vy[i1 + offset*j1 + offset2*k2] * (x2 - x)* (y2 - y) * (-z1 + z) +
		vy[i1 + offset*j2 + offset2*k1] * (x2 - x)* (-y1 + y) * (z2 - z) +
		vy[i1 + offset*j2 + offset2*k2] * (x2 - x)* (-y1 + y) * (-z1 + z) +
		vy[i2 + offset*j1 + offset2*k1] * (-x1 + x)* (y2 - y) * (z2 - z) +
		vy[i2 + offset*j1 + offset2*k2] * (-x1 + x)* (y2 - y) * (-z1 + z) +
		vy[i2 + offset*j2 + offset2*k1] * (-x1 + x)* (-y1 + y) * (z2 - z) +
		vy[i2 + offset*j2 + offset2*k2] * (-x1 + x)* (-y1 + y) * (-z1 + z);
	VY = VY / h3;


	VZ= vz[i1 + offset*j1 + offset2*k1] * (x2 - x)* (y2 - y) * (z2 - z) +
		vz[i1 + offset*j1 + offset2*k2] * (x2 - x)* (y2 - y) * (-z1 + z) +
		vz[i1 + offset*j2 + offset2*k1] * (x2 - x)* (-y1 + y) * (z2 - z) +
		vz[i1 + offset*j2 + offset2*k2] * (x2 - x)* (-y1 + y) * (-z1 + z) +
		vz[i2 + offset*j1 + offset2*k1] * (-x1 + x)* (y2 - y) * (z2 - z) +
		vz[i2 + offset*j1 + offset2*k2] * (-x1 + x)* (y2 - y) * (-z1 + z) +
		vz[i2 + offset*j2 + offset2*k1] * (-x1 + x)* (-y1 + y) * (z2 - z) +
		vz[i2 + offset*j2 + offset2*k2] * (-x1 + x)* (-y1 + y) * (-z1 + z);
	VZ = VZ / h3;


	if (VY > Vmax) Vmax = VY;
	if (VY < Vmin) Vmin = VY;

	VX_ = VX; VY_ = VY; VZ_ = VZ;

	//VX *= 10; VY *= 10; VZ *= 10;

	x = x + AMPLIFY * VX * tau;
	y = y + AMPLIFY * VY * tau;
	z = z + AMPLIFY * VZ * tau;

	if (x <= 0 + hx) x = x + Lx;
	if (x >= hx*nx - hx) x = x - Lx;
	if (y <= 0) y = y + Ly;
	if (y >= hy*ny) y = y - Ly;
	if (z <= 0 + hz) z = z + Lz;
	if (z >= hz*nz - hz) z = z - Lz;
}


void display()
{

	//glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	//glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();


	//____________________________________//
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	gluPerspective(alpha, 1, fMin, fMax);

	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
	projmatrix[12] = transX;
	projmatrix[13] = transY;
	//projmatrix[14] = transZ;
	glLoadMatrixd(projmatrix);
	glRotatef(angleX, 1.0f, 0.0f, 0.0f);
	glRotatef(angleY, 0.0f, 1.0f, 0.0f);
	glRotatef(angleZ, 0.0f, 0.0f, 1.0f);
	gluLookAt(tx, ty, tz, px, py, pz, 0, 1, 0);


	nx = (tx - px);
	ny = (ty - py);
	nz = (tz - pz);

	//____________________________________//
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//axes
	glBegin(GL_LINES);
	glColor3d(0.0, 0.0, 1.0);
	glVertex3d(0, 0, 0); 	 glVertex3d(Lx, 0, 0);
	glColor3d(1.0, 0.0, 0.0);
	glVertex3d(0, 0, 0);  	glVertex3d(0, Ly, 0);
	glColor3d(0.0, 1.0, 0.0);
	glVertex3d(0, 0, 0);		glVertex3d(0, 0, Lz);

	glEnd();


	//draw paths
	Vmax = 0; Vmin = 0;
	for (int C = 0; C < NN; C++) {
		for (int i = 0; i < 100; i++) {
			trace(P[C].x, P[C].y, P[C].z, vx, vy, vz, off, off2, Nx, Ny, Nz, hx, hy, hz, tau, P[C].vx, P[C].vy, P[C].vz);
		}
		P[C].traceX.push_back(P[C].x);
		P[C].traceY.push_back(P[C].y);
		P[C].traceZ.push_back(P[C].z);
	}
	Size = P[0].traceX.size();


	// draw circles
	for (int C = 0; C < NN; C++) {
		glBegin(GL_POINTS);
		glColor3d(P[C].R, P[C].G, P[C].B);

		for (unsigned int k = Size - 1; k > 0; k = k - 1) 
		{
			glVertex3f((GLfloat)P[C].traceX[k], (GLfloat)P[C].traceY[k], (GLfloat)P[C].traceZ[k]);
		}
		glEnd();


		//glColor3f(1.0, 0.0, 0.0);
		glTranslated(P[C].x, P[C].y, P[C].z);
		glutSolidSphere(0.025, 10, 10);
		glLoadIdentity();
	}
	glutSwapBuffers();


	//erase path endings
#define q 5
	if (P[0].traceX.size() > 3000) {
		for (int C = 0; C < NN; C++) {
			P[C].traceX.erase(P[C].traceX.begin(), P[C].traceX.begin() + q);
			P[C].traceY.erase(P[C].traceY.begin(), P[C].traceY.begin() + q);
			P[C].traceZ.erase(P[C].traceZ.begin(), P[C].traceZ.begin() + q);
		}
	}
	iter++;

}

void display2()
{
	current_time = glutGet(GLUT_ELAPSED_TIME);
	delta_time = double(current_time - last_time) / 1000.0f;
	last_time = current_time;
	event_timer += delta_time;

	//glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	#define gla	gluLookAt(Cam.Pos.x, Cam.Pos.y, Cam.Pos.z, Cam.Focus.x, Cam.Focus.y, Cam.Focus.z, Cam.Up.x, Cam.Up.y, Cam.Up.z);
	//glLoadIdentity();

	Cam.Focus = Cam.Pos + Cam.Front;


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(/*vec::radians*/(Cam.Zoom), (float)width / (float)height, 0.1f, 100.0f);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gla;
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	//____________________________________//

	//axes
	glBegin(GL_LINES);
	glColor3d(0.0, 0.0, 1.0);
	glVertex3d(0, 0, 0); 	 glVertex3d(Lx, 0, 0);
	glColor3d(1.0, 0.0, 0.0);
	glVertex3d(0, 0, 0);  	glVertex3d(0, Ly, 0);
	glColor3d(0.0, 1.0, 0.0);
	glVertex3d(0, 0, 0);		glVertex3d(0, 0, Lz);
	glEnd();

	//compute routes
	Vmax = 0; Vmin = 0;
	for (int C = 0; C < NN; C++) {
		for (int i = 0; i < 100; i++) {
			trace(P[C].x, P[C].y, P[C].z, vx, vy, vz, off, off2, Nx, Ny, Nz, hx, hy, hz, tau, P[C].vx, P[C].vy, P[C].vz);
		}
		P[C].traceX.push_back(P[C].x);
		P[C].traceY.push_back(P[C].y);
		P[C].traceZ.push_back(P[C].z);
	}
	Size = P[0].traceX.size();

	//glLoadIdentity();
	//gla

	// draw circles
	for (int C = 0; C < NN; C++) {
		glPushMatrix();
			glBegin(GL_POINTS);
			glColor3d(P[C].R, P[C].G, P[C].B);

			for (unsigned int k = Size - 1; k > 0; k = k - 1)
			{
				glVertex3f((GLfloat)P[C].traceX[k], (GLfloat)P[C].traceY[k], (GLfloat)P[C].traceZ[k]);
			}
			glEnd();
			//glColor3f(1.0, 0.0, 0.0);
			glTranslated(P[C].x, P[C].y, P[C].z);
			glutSolidSphere(0.025, 10, 10);
		glPopMatrix();
	}

	//erase path endings
#define q 5
	if (P[0].traceX.size() > 6000) {
		for (int C = 0; C < NN; C++) {
			P[C].traceX.erase(P[C].traceX.begin(), P[C].traceX.begin() + q);
			P[C].traceY.erase(P[C].traceY.begin(), P[C].traceY.begin() + q);
			P[C].traceZ.erase(P[C].traceZ.begin(), P[C].traceZ.begin() + q);
		}
	}
	iter++;


	glutSwapBuffers();
	glutPostRedisplay();


	//if (event_timer > 30 && FILES.currentFileIndex < FILES.getFileCount())
	//{
	//	png_iter++;
	//	saveFrame(_pathGL("pictures") + _strGLmask(png_iter, 5), 0, 0);
	//	cout << png_iter << " png saved " << endl;

	//	std::ifstream read = FILES.getNextFile();
	//	read_3d_fields(read);
	//	for (int i = 0; i < NN; i++) P[i].REgenerate(Lx, Ly, Lz);

	//	event_timer = 0;
	//}

}



void change_normal(float coef) {
	float shift = 0.2f;
	tx = nx + px * (1.0f + coef*shift);
	ty = ny + py * (1.0f + coef*shift);
	tz = nz + pz * (1.0f + coef*shift);
}

void mouseMovePassive(int x, int y) {
	//cout << x << " " << y << endl;
}
void mouseMove2(int key, int state, int x, int y) {

	int stat = glutGetModifiers();
	switch (stat)
	{
	case 0:
		if (key == 3)
		{
			if (state == GLUT_DOWN)
			{
				change_normal(1);
			}
		}

		if (key == 4) {
			if (state == GLUT_DOWN) {
				change_normal(-1);
			}
		}

		if (key == 0)
		{
			if (state == GLUT_DOWN) {
			}
		}

		if (key == 1) {
			if (state == GLUT_DOWN) {
				//exit(0);
			}
		}


		break;
	case GLUT_ACTIVE_SHIFT:
		if (key == 3)
		{
			if (state == GLUT_DOWN)
			{
				//transZ += 0.05f;
				//cout << transZ << endl;
			}
		}

		if (key == 4) {
			if (state == GLUT_DOWN) {
				//transZ += -0.05f;
				//cout << transZ << endl;
			}
		}

		break;


	default:
		break;
	}
}
void D()
{
	if (iter_draw % 10000 == 0) 
	{
		show_matrix(projmatrix);
	}


	if (_test)
	{
		std::ifstream read = FILES.getNextFile(10);
		read_3d_fields(read);

		for (int i = 0; i < NN; i++)	P[i].REgenerate(Lx, Ly, Lz);


		_test = false;
		cout << "_test" << endl;
	}

	glutReshapeFunc(reshape);
	glutPostRedisplay();
	iter_draw++;
}



#include "Keyboard.h"
#include "Mouse.h"
int main(int argc, char **argv)
{


	Cam.readCameraState();



	tau = 5e-4;
	tt = (unsigned int)round(1.0 / tau);
	iter = 0;
	double grid =  20;
	double hh = 1.0 / grid;
	hx = hy = hz = hh;
	Ly = 1.0;
	Lx = 10;
	Lz = 10;

	Ny = (int)(Ly / hy);
	Nx = (int)(Lx / hx);
	Nz = (int)(Lz / hz);
	off = Nx + 1;
	off2 = (Nx + 1) * (Ny + 1);
	N_grid = (Nx + 1) * (Ny + 1) * (Nz + 1);


	//CPU memory allocation and initialization
	size_t size_b = N_grid * sizeof(double);
	vx = new double[N_grid];
	vy = new double[N_grid];
	vz = new double[N_grid];

	double vmax = 0;
	ifstream read;
	string fname = "all.txt";
	fname = "Z:\\Projects\\ternary_3d\\CudaRuntimeTry\\all.txt";
	//fname = "Z:\\Projects\\ternary_3d\\CudaRuntimeTry\\fields\\50 4800 all.txt";
	//ifstream read("Z:\\Projects\\CUDA\\ternary_3d\\CudaRuntimeT\\all.txt");
	//ifstream read("Z:\\Projects\\CUDA\\ternary_3d\\CudaRuntimeT\\fields\\28 4700 all.txt");
	//ifstream read("Z:\\Projects\\CUDA\\ternary_3d\\CudaRuntime4\\5x5 osc\\fields\\360 5000 all.txt");
	//read.open("Z:\\Projects\\CUDA\\ternary_3d\\bench\\all.txt");



	read.open(fname);
	read_3d_fields(read);

	for (int i = 0; i < NN; i++) 
		P[i].REgenerate(Lx, Ly, Lz);
	



	//window resolution
	height = 800;
	width = 1200;

	std::cout << "number of grid nodes: " << N_grid << std::endl;

	timer1 = clock() / CLOCKS_PER_SEC;
	define_default_camera();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(250, 0);
	glutCreateWindow("particle visual");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display2);
	glutIdleFunc(D);
	//glutPassiveMotionFunc(mouseMovePassive);
	//glutMouseFunc(mouseMove2);

	//mouse
	glutPassiveMotionFunc(mouseMovePassive);
	glutMotionFunc(mouseMoveClick);
	glutMouseFunc(mouseClick);
	glutMouseWheelFunc(mouseWheel);


	glutKeyboardFunc(keyboard2);
	glutSpecialFunc(keyboard_spec);
	glutMainLoop();

	return 0;
}