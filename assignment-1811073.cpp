/*
	ASSIGNMENT 2
	HK202
	NG THI XUAN MAI - 1811073
*/
#include <iostream>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>

using namespace std;
/////////////////////////////////////////////////////////////////////
#define PI			3.1415926
#define	COLORNUM	14
#define nSegment	40
#define	MAT_BEN		1
#define MAT_DAY		2
#define MAT_TREN	0
#define bgSize		2.0
#define bgOpacity	0.5
enum {
	X, Y, Z, T
};
enum {
	A, B, C, D
};
void printMenu();
float	ColorArr[COLORNUM][3] = {
	{ 1.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0 },
	{ 0.0,  0.0, 1.0 },
	{ 1.0, 1.0,  0.0 },
	{ 1.0, 0.0, 1.0 },
	{ 0.0, 1.0, 1.0 },
	{ 0.7, 0.7, 0.7 },
	{ 0.5, 0.5, 0.5 },
	{ 0.8,  0.8, 0.8 },
	{ 1.0, 0.5,  0.5 },
	{ 0.5, 1.0, 0.5 },
	{ 0.5, 0.5, 1.0 },
	{ 0.0, 0.0, 0.0 },
	{ 0.3, 0.3, 0.3 } };
class Point3
{
public:
	float x, y, z;
	void set(float dx, float dy, float dz)
	{
		x = dx; y = dy; z = dz;
	}
	void set(Point3& p)
	{
		x = p.x; y = p.y; z = p.z;
	}
	Point3() { x = y = z = 0; }
	Point3(float dx, float dy, float dz)
	{
		x = dx; y = dy; z = dz;
	}

};
class Vector3
{
public:
	float	x, y, z;
	void set(float dx, float dy, float dz)
	{
		x = dx; y = dy; z = dz;
	}
	void set(Vector3& v)
	{
		x = v.x; y = v.y; z = v.z;
	}
	void flip()
	{
		x = -x; y = -y; z = -z;
	}
	void normalize()
	{
		float temp = sqrt(x * x + y * y + z * z);
		x = x / temp;
		y = y / temp;
		z = z / temp;
	};
	Vector3() { x = y = z = 0; }
	Vector3(float dx, float dy, float dz)
	{
		x = dx; y = dy; z = dz;
	}
	Vector3(Vector3& v)
	{
		x = v.x; y = v.y; z = v.z;
	}
	Vector3 cross(Vector3 b)
	{
		Vector3 c(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
		return c;
	};
	float dot(Vector3 b)
	{
		return x * b.x + y * b.y + z * b.z;
	};
};
class VertexID
{
public:
	int		vertIndex;
	int		colorIndex;
};

class Face
{
public:
	int		nVerts;
	VertexID* vert;
	Vector3 norm;
	Face()
	{
		nVerts = 0;
		vert = NULL;
	}
	~Face()
	{
		if (vert != NULL)
		{
			delete[] vert;
			vert = NULL;
		}
		nVerts = 0;
	}
};

class Mesh
{
public:
	float		slideX, slideY, slideZ;
	float		rotateX, rotateY, rotateZ;
	float		scaleX, scaleY, scaleZ;

	int		numVerts;
	Point3* pt;

	int		numFaces;
	Face* face;
public:
	Mesh()
	{
		slideX = slideY = slideZ = 0;
		rotateX = rotateY = rotateZ = 0;
		scaleX = scaleY = scaleZ = 1;

		numVerts = 0;
		pt = NULL;
		numFaces = 0;
		face = NULL;
	}
	~Mesh()
	{
		if (pt != NULL)
		{
			delete[] pt;
		}
		if (face != NULL)
		{
			delete[] face;
		}
		numVerts = 0;
		numFaces = 0;
	}
	void SetColor(int colorIdx) {
		for (int f = 0; f < numFaces; f++) {
			for (int v = 0; v < face[f].nVerts; v++) {
				face[f].vert[v].colorIndex = colorIdx;
			}
		}
	}
	void setupMaterial(float ambient[], float diffuse[], float specular[], float shiness)
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiness);
	}
	void CalculateNorms()
	{
		float mx, my, mz;
		for (int f = 0; f < numFaces; f++) {
			mx = my = mz = 0;
			for (int v = 0; v < face[f].nVerts; v++) {
				int iv = face[f].vert[v].vertIndex;
				int next = face[f].vert[(v + 1) % face[f].nVerts].vertIndex;
				mx += (pt[iv].y - pt[next].y) * (pt[iv].z + pt[next].z);
				my += (pt[iv].z - pt[next].z) * (pt[iv].x + pt[next].x);
				mz += (pt[iv].x - pt[next].x) * (pt[iv].y + pt[next].y);
			}
			face[f].norm.set(mx, my, mz);
			face[f].norm.normalize();
		}
	}
	void DrawWireframe();
	void DrawColor();
	void DrawPoint();
	void Draw();
	void DrawStyle(int choose);
	void CreateShape(float L2, float L1, float R, float H, int op); //Sinh vien tu dien tham so
};
#pragma region Mesh

void Mesh::CreateShape(float L2, float L1, float R, float H, int op) //Sinh vien tu dien tham so
{
	//Sinh vien viet ma nguon
	GLfloat angle = 2.0 * PI / (2.0 * nSegment);
	int p = 0;
	numVerts = (nSegment + 1) * 4 + 8;
	pt = new Point3[numVerts];
	int i = nSegment + 1;
	//Dau mut 1/2 tru tron
	for (int j = 0; j <= nSegment; j++) {
		pt[p + i].set(
			R * cos(angle * j) - R,
			H * op,
			R * sin(angle * j) + L2);
		pt[p].set(
			R * cos(angle * j) - R,
			0,
			R * sin(angle * j) + L2);
		p++;
	}
	//Goc co 1/4 tru tron
	int n1 = p + i;
	p = n1;
	for (int j = 0; j <= nSegment; j++) {
		pt[p + i].set(
			-2 * R * cos(angle * 0.5 * j),
			H * op,
			-2 * R * sin(angle * 0.5 * j));
		pt[p].set(
			-2 * R * cos(angle * 0.5 * j),
			0,
			-2 * R * sin(angle * 0.5 * j));
		p++;
	}
	int n2 = p + i;
	p = n2;
	//thanh ngang ngan L1
	//canh dung goc phai xa
	pt[p++].set(L1, 0, -2 * R);		//n2
	pt[p++].set(L1, H * op, -2 * R);	//n2+1
	//canh dung goc phai gan
	pt[p++].set(L1, 0, 0);				//n2+2
	pt[p++].set(L1, H * op, 0);			//n2+3
	//canh dung (x,z)=(0,0)
	pt[p++].set(0, 0, 0);				//n2+4
	pt[p++].set(0, H * op, 0);			//n2+5
	//Tam dau tron
	pt[p++].set(-R, 0, L2);			//n2+6 duoi
	pt[p].set(-R, H * op, L2);			//n2+7 tren

	//Faces
	numFaces = nSegment * 6 + 9;
	face = new Face[numFaces];
	int f = 0;
#pragma region MatBen
	//MAT_BEN ne
	for (int j = 0; j < nSegment; j++) {
		face[f].nVerts = 4;
		face[f].vert = new VertexID[face[f].nVerts];
		face[f].vert[0].vertIndex = j;
		face[f].vert[1].vertIndex = j + 1;
		face[f].vert[2].vertIndex = j + i + 1;
		face[f].vert[3].vertIndex = j + i;
		for (int id = 0; id < 4; id++) {
			face[f].vert[id].colorIndex = MAT_BEN;
		}
		f++;
	}
	face[f].nVerts = 4;
	face[f].vert = new VertexID[face[f].nVerts];
	face[f].vert[0].vertIndex = n1;
	face[f].vert[1].vertIndex = n1 + i;
	face[f].vert[2].vertIndex = n1 - 1;
	face[f].vert[3].vertIndex = n1 - i - 1;
	for (int id = 0; id < 4; id++) {
		face[f].vert[id].colorIndex = MAT_BEN;
	}
	f++;
	for (int j = 0; j < nSegment; j++) {
		face[f].nVerts = 4;
		face[f].vert = new VertexID[face[f].nVerts];
		face[f].vert[0].vertIndex = n1 + j;
		face[f].vert[1].vertIndex = n1 + j + 1;
		face[f].vert[2].vertIndex = n1 + j + i + 1;
		face[f].vert[3].vertIndex = n1 + j + i;
		for (int id = 0; id < 4; id++) {
			face[f].vert[id].colorIndex = MAT_BEN;
		}
		f++;
	}
	face[f].nVerts = 4;
	face[f].vert = new VertexID[face[f].nVerts];
	face[f].vert[0].vertIndex = n2 - 1;
	face[f].vert[1].vertIndex = n2 + 1;
	face[f].vert[2].vertIndex = n2;
	face[f].vert[3].vertIndex = n2 - i - 1;
	for (int id = 0; id < 4; id++) {
		face[f].vert[id].colorIndex = MAT_BEN;
	}
	f++;
	face[f].nVerts = 4;
	face[f].vert = new VertexID[face[f].nVerts];
	face[f].vert[0].vertIndex = n2;
	face[f].vert[1].vertIndex = n2 + 1;
	face[f].vert[2].vertIndex = n2 + 3;
	face[f].vert[3].vertIndex = n2 + 2;
	for (int id = 0; id < 4; id++) {
		face[f].vert[id].colorIndex = MAT_BEN;
	}
	f++;
	face[f].nVerts = 4;
	face[f].vert = new VertexID[face[f].nVerts];
	face[f].vert[0].vertIndex = n2 + 2;
	face[f].vert[1].vertIndex = n2 + 3;
	face[f].vert[2].vertIndex = n2 + 5;
	face[f].vert[3].vertIndex = n2 + 4;
	for (int id = 0; id < 4; id++) {
		face[f].vert[id].colorIndex = MAT_BEN;
	}
	f++;
	face[f].nVerts = 4;
	face[f].vert = new VertexID[face[f].nVerts];
	face[f].vert[0].vertIndex = n2 + 4;
	face[f].vert[1].vertIndex = n2 + 5;
	face[f].vert[2].vertIndex = i;
	face[f].vert[3].vertIndex = 0;
	for (int id = 0; id < 4; id++) {
		face[f].vert[id].colorIndex = MAT_BEN;
	}
	f++;
#pragma endregion

#pragma region MatTren
	//MAT_TREN
	for (int j = 0; j < nSegment; j++) {
		face[f].nVerts = 3;
		face[f].vert = new VertexID[face[f].nVerts];
		face[f].vert[0].vertIndex = n2 + 7;
		face[f].vert[1].vertIndex = j + i;
		face[f].vert[2].vertIndex = j + i + 1;
		for (int id = 0; id < 3; id++) {
			face[f].vert[id].colorIndex = MAT_TREN;
		}
		f++;
	}
	face[f].nVerts = 4;
	face[f].vert = new VertexID[face[f].nVerts];
	face[f].vert[0].vertIndex = n1 + i;
	face[f].vert[1].vertIndex = n1 - 1;
	face[f].vert[2].vertIndex = n1 - i;
	face[f].vert[3].vertIndex = n2 + 5;
	for (int id = 0; id < 4; id++) {
		face[f].vert[id].colorIndex = MAT_TREN;
	}
	f++;
	for (int j = 0; j < nSegment; j++) {
		face[f].nVerts = 3;
		face[f].vert = new VertexID[face[f].nVerts];
		face[f].vert[0].vertIndex = n2 + 5;
		face[f].vert[1].vertIndex = j + n1 + i;
		face[f].vert[2].vertIndex = j + n1 + i + 1;
		for (int id = 0; id < 3; id++) {
			face[f].vert[id].colorIndex = MAT_TREN;
		}
		f++;
	}
	face[f].nVerts = 4;
	face[f].vert = new VertexID[face[f].nVerts];
	face[f].vert[0].vertIndex = n2 - 1;
	face[f].vert[1].vertIndex = n2 + 1;
	face[f].vert[2].vertIndex = n2 + 3;
	face[f].vert[3].vertIndex = n2 + 5;
	for (int id = 0; id < 4; id++) {
		face[f].vert[id].colorIndex = MAT_TREN;
	}
	f++;
#pragma endregion

#pragma region MatDuoi
	//MAT_DAY
	for (int j = 0; j < nSegment; j++) {
		face[f].nVerts = 3;
		face[f].vert = new VertexID[face[f].nVerts];
		face[f].vert[0].vertIndex = n2 + 6;
		face[f].vert[1].vertIndex = j;
		face[f].vert[2].vertIndex = j + 1;
		for (int id = 0; id < 3; id++) {
			face[f].vert[id].colorIndex = MAT_DAY;
		}
		f++;
	}
	face[f].nVerts = 4;
	face[f].vert = new VertexID[face[f].nVerts];
	face[f].vert[0].vertIndex = n1;
	face[f].vert[1].vertIndex = n1 - i - 1;
	face[f].vert[2].vertIndex = 0;
	face[f].vert[3].vertIndex = n2 + 4;
	for (int id = 0; id < 4; id++) {
		face[f].vert[id].colorIndex = MAT_DAY;
	}
	f++;
	for (int j = 0; j < nSegment; j++) {
		face[f].nVerts = 3;
		face[f].vert = new VertexID[face[f].nVerts];
		face[f].vert[0].vertIndex = n2 + 4;
		face[f].vert[1].vertIndex = j + n1;
		face[f].vert[2].vertIndex = j + n1 + 1;
		for (int id = 0; id < 3; id++) {
			face[f].vert[id].colorIndex = MAT_DAY;
		}
		f++;
	}
	face[f].nVerts = 4;
	face[f].vert = new VertexID[face[f].nVerts];
	face[f].vert[0].vertIndex = n2 - i - 1;
	face[f].vert[1].vertIndex = n2;
	face[f].vert[2].vertIndex = n2 + 2;
	face[f].vert[3].vertIndex = n2 + 4;
	for (int id = 0; id < 4; id++) {
		face[f].vert[id].colorIndex = MAT_DAY;
	}
	//f++;
#pragma endregion

}

void Mesh::DrawWireframe()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int f = 0; f < numFaces; f++)
	{
		glColor3f(0, 0, 1);
		glBegin(GL_POLYGON);
		for (int v = 0; v < face[f].nVerts; v++)
		{
			int	iv = face[f].vert[v].vertIndex;

			glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
		}
		glEnd();
	}
}

void Mesh::DrawColor()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int f = 0; f < numFaces; f++)
	{
		glBegin(GL_POLYGON);
		for (int v = 0; v < face[f].nVerts; v++)
		{
			int		iv = face[f].vert[v].vertIndex;
			int		ic = face[f].vert[v].colorIndex;

			//ic = f % COLORNUM;

			glColor3f(ColorArr[ic][0], ColorArr[ic][1], ColorArr[ic][2]);
			glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
		}
		glEnd();
	}
}
void Mesh::DrawPoint()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glPointSize(3);
	glColor3f(0, 0, 0);
	for (int f = 0; f < numVerts; f++)
	{
		glBegin(GL_POINTS);
		glVertex3f(pt[f].x, pt[f].y, pt[f].z);
		glEnd();
	}
}
void Mesh::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int f = 0; f < numFaces; f++) {
		glBegin(GL_POLYGON);
		for (int v = 0; v < face[f].nVerts; v++) {
			int		iv = face[f].vert[v].vertIndex;
			glNormal3f(face[f].norm.x, face[f].norm.y, face[f].norm.z);
			glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
		}
		glEnd();
	}
}
void Mesh::DrawStyle(int choose) {
	switch (choose) {
	case 1:
		this->DrawWireframe();
		break;
	case 2:
		this->DrawColor();
		break;
	case 3:
		this->Draw();
		break;
	}
}
#pragma endregion

//////////////////////////////////////////////////////////////////////
#pragma region initThongSo
int		screenWidth = 1000;
int		screenHeight = 600;

float	R = 1.0f;
float	L1 = 2.5 * R;
float	L2 = 4.5 * R;
float	H = 1.5 * R;

/// <summary>
/// drawStyle	1		Ve khung day
///				2		Ve mau 2D
///				3		Ve mau 3D
/// </summary>
int		drawStyle = 3;
float	scale = 1;
float	translate = 1.2 * H;
bool	visible = false;
Mesh	shape;
Mesh	shapeOppo;
Mesh	shadows;
GLfloat ambient[4] = { 0.5, 0.0, 0.0, 1.0 };
GLfloat diffuse[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat specular[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat shiness = 50.0f;

//light position
GLfloat light_position[] = { 5.0f,5.0f,5.0f, 0.0f };
GLfloat light_position1[] = { -10.0f,5.0f,-10.0f, 0.0f };
GLfloat light_position0[] = { 5.0f,-5.0f,5.0f, 0.0f };
GLfloat light_position10[] = { -10.0f,-5.0f,-10.0f, 0.0f };
#pragma endregion

#pragma region shadow
static GLfloat floorPlane[4];
static GLfloat shadow[4][4];
GLfloat floorVertices[4][3] = {
  { -1.0, 0.001, 1.0 },
  { 1.0, 0.001, 1.0 },
  { 1.0, 0.001, -1.0 },
  { -1.0, 0.001, -1.0 },
};

/* Create a matrix that will project the desired shadow. */
void shadowMatrix(GLfloat shadow[4][4], GLfloat groundplane[4], GLfloat light[4]) {
	GLfloat dot;

	/* Find dot product between light position vector and ground plane normal. */
	dot = groundplane[X] * light[X] + groundplane[Y] * light[Y] + groundplane[Z] * light[Z] + groundplane[T] * light[T];

	shadow[0][0] = dot - light[X] * groundplane[X];
	shadow[1][0] = 0.f - light[X] * groundplane[Y];
	shadow[2][0] = 0.f - light[X] * groundplane[Z];
	shadow[3][0] = 0.f - light[X] * groundplane[T];

	shadow[X][1] = 0.f - light[Y] * groundplane[X];
	shadow[1][1] = dot - light[Y] * groundplane[Y];
	shadow[2][1] = 0.f - light[Y] * groundplane[Z];
	shadow[3][1] = 0.f - light[Y] * groundplane[T];

	shadow[X][2] = 0.f - light[Z] * groundplane[X];
	shadow[1][2] = 0.f - light[Z] * groundplane[Y];
	shadow[2][2] = dot - light[Z] * groundplane[Z];
	shadow[3][2] = 0.f - light[Z] * groundplane[T];

	shadow[X][3] = 0.f - light[T] * groundplane[X];
	shadow[1][3] = 0.f - light[T] * groundplane[Y];
	shadow[2][3] = 0.f - light[T] * groundplane[Z];
	shadow[3][3] = dot - light[T] * groundplane[T];

}
void findPlane(GLfloat plane[4], GLfloat v0[3], GLfloat v1[3], GLfloat v2[3]) {
	GLfloat vector0[3], vector1[3];

	/* Need 2 vectors to find cross product. */
	vector0[X] = v1[X] - v0[X];
	vector0[Y] = v1[Y] - v0[Y];
	vector0[Z] = v1[Z] - v0[Z];

	vector1[X] = v2[X] - v0[X];
	vector1[Y] = v2[Y] - v0[Y];
	vector1[Z] = v2[Z] - v0[Z];

	/* find cross product to get A, B, and C of plane equation */
	plane[A] = vector0[Y] * vector1[Z] - vector0[Z] * vector1[Y];
	plane[B] = -(vector0[X] * vector1[Z] - vector0[Z] * vector1[X]);
	plane[C] = vector0[X] * vector1[Y] - vector0[Y] * vector1[X];

	plane[D] = -(plane[A] * v0[X] + plane[B] * v0[Y] + plane[C] * v0[Z]);
}
#pragma endregion

#pragma region Background
//background
int matrix[nSegment * 2][nSegment * 2] = { 0 };
void drawEleBG(int x, int z, float op) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glColor4f(0.85f, 0.85f, 0.85f, op);
	float half = bgSize / 2.0 - bgSize / 100.0;
	glNormal3f(0.0f, 1.0f, 0.0f);

	glVertex3f(x + half, 0, z + half);
	glVertex3f(x + half, 0, z - half);
	glVertex3f(x - half, 0, z - half);
	glVertex3f(x - half, 0, z + half);

	glEnd();
}

void drawFullBG(float op) {
	glDisable(GL_LIGHTING);

	int limit = 20;
	for (int x = -limit; x < limit; x += 2)
	{
		for (int z = -limit; z < limit; z += 2)
		{
			int idx = matrix[int(x) + limit][int(z) + limit];
			//drawEleBG(x, z);
			drawEleBG(x, z, op);
		}
	}

	
	if (drawStyle == 3) {
		glEnable(GL_LIGHTING);
	}
}

#pragma endregion

void createObj() {
	shapeOppo.CreateShape(L2, L1, R, H, -1);
	//shapeOppo.CalculateNorms();

	shape.CreateShape(L2, L1, R, H, 1);
	//shape.CalculateNorms();

	shadows.CreateShape(L2, L1, R, H, 1);
}

#pragma region drawObj
void drawShape() {
	glPushMatrix();
	int c = 0;
	glTranslatef(0, translate, 0);
	glRotatef(shape.rotateX, 1, 0, 0);
	glRotatef(shape.rotateY, 0, 1, 0);
	glRotatef(shape.rotateZ, 0, 0, 1);
	glScalef(scale, scale, scale);

	GLfloat diffuseA[] = { ColorArr[c][0],ColorArr[c][1],ColorArr[c][2], 1.0 };
	shape.setupMaterial(ambient, diffuseA, specular, shiness);
	shape.CalculateNorms();
	shape.DrawStyle(drawStyle);
	glPopMatrix();
}
void drawShapeOppo() {

	int c = 0;
	glTranslatef(0, -translate, 0);
	glRotatef(shapeOppo.rotateX, 1, 0, 0);
	glRotatef(shapeOppo.rotateY, 0, 1, 0);
	glRotatef(shapeOppo.rotateZ, 0, 0, 1);
	glScalef(scale, scale, scale);

	GLfloat diffuseA[] = { ColorArr[c][0],ColorArr[c][1],ColorArr[c][2], 1.0 };
	shapeOppo.setupMaterial(ambient, diffuseA, specular, shiness);
	shapeOppo.CalculateNorms();
	shapeOppo.DrawStyle(drawStyle);

}
#pragma endregion

void drawObj() {
	drawShapeOppo();
	drawShape();
}
//Setup Camera
float	camera_angle; // Góc quay camera xung quanh trục Oy
float	camera_height; // Chiều cao camera so với mặt phẳng xOz
float	camera_dis; //Khoảng cách đến trục Oy
float	camera_X, camera_Y, camera_Z;
float	lookAt_X, lookAt_Y, lookAt_Z;


void setupLight();
void myKeyboard(unsigned char key, int x, int y);
void mySpecialKeyboard(int key, int x, int y);


void drawInvisibleFromBottom() {
	#pragma region VebongCach1
	//tu duoi khong nhin thay ben tren
	glPushMatrix();
	glScalef(1, 1, 1);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position10);
	drawShapeOppo();
	glPopMatrix();

	//ve bong
	shadowMatrix(shadow, floorPlane, light_position);
	glPushMatrix();
	if (drawStyle != 1) {
		glDisable(GL_LIGHTING);

		glMultMatrixf((GLfloat*)shadow);

		glTranslatef(0, translate, 0);
		glRotatef(shape.rotateX, 1, 0, 0);
		glRotatef(shape.rotateY, 0, 1, 0);
		glRotatef(shape.rotateZ, 0, 0, 1);
		glScalef(scale, scale, scale);

		shadows.SetColor(7);
		shadows.DrawStyle(2);

		glEnable(GL_LIGHTING);
	}
	glPopMatrix();

	//ve nen check
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawFullBG(bgOpacity);
	glDisable(GL_BLEND);

	glPushMatrix();
	glScalef(1, -1, 1);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	drawShapeOppo();
	glPopMatrix();
#pragma endregion
}
void drawVisibleFromBottom() {
#pragma region VebongCach2
	glPushMatrix();
	glScalef(1, 1, 1);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	drawShape();
	glPopMatrix();

	glPushMatrix();
	glScalef(1, -1, 1);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position10);
	drawShape();
	glPopMatrix();

	//tao bong
	shadowMatrix(shadow, floorPlane, light_position);
	glPushMatrix();
	if (drawStyle != 1) {
		glDisable(GL_LIGHTING);

		glMultMatrixf((GLfloat*)shadow);

		glTranslatef(0, translate, 0);
		glRotatef(shape.rotateX, 1, 0, 0);
		glRotatef(shape.rotateY, 0, 1, 0);
		glRotatef(shape.rotateZ, 0, 0, 1);
		glScalef(scale, scale, scale);

		shadows.SetColor(7);
		shadows.DrawStyle(2);

		glEnable(GL_LIGHTING);
	}
	glPopMatrix();
	//ve nen
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawFullBG(bgOpacity);
	glDisable(GL_BLEND);

#pragma endregion
}
void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	camera_X = camera_dis * sinf(camera_angle * PI / 180);
	camera_Y = camera_height;
	camera_Z = camera_dis * cosf(camera_angle * PI / 180);
	if (camera_dis == 0) gluLookAt(camera_X, camera_Y, camera_Z, lookAt_X, lookAt_Y, lookAt_Z, sinf(camera_angle * PI / 180), 0, cosf(camera_angle * PI / 180));
	else gluLookAt(camera_X, camera_Y, camera_Z, lookAt_X, lookAt_Y, lookAt_Z, 0, 1, 0);

	glViewport(0, 0, screenWidth, screenHeight);
	
	///////////////////////////////////////////////////////////////////
	if (visible) drawVisibleFromBottom();
	else drawInvisibleFromBottom();
	glFlush();
	glutSwapBuffers();
}
void myInit()
{
	camera_angle = 40;  // Góc quay camera xung quanh trục Oy
	camera_height = 9; // Chiều cao camera so với mặt phẳng xOz
	camera_dis = 20;	// Khoảng cách đến trục Oy

	lookAt_X = 0;
	lookAt_Y = 1;
	lookAt_Z = 0;

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float r = screenWidth * 1.0 / screenHeight;
	glFrustum(-r, r, -1, 1, 2, 50.0);
	glEnable(GL_NORMALIZE);
	GLfloat lmodel_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	findPlane(floorPlane, floorVertices[0], floorVertices[1], floorVertices[2]);
	setupLight();
}

int main(int argc, char** argv[])
{
	printMenu();
	glutInit(&argc, (char**)argv); //initialize the tool kit
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//set the display mode
	glutInitWindowSize(screenWidth, screenHeight); //set window size
	glutInitWindowPosition(200, 50); // set window position on screen
	glutCreateWindow("Nguyen Thi Xuan Mai - 1811073"); // open the screen window

	createObj();
	myInit();
	glutKeyboardFunc(myKeyboard);
	glutDisplayFunc(myDisplay);
	glutSpecialFunc(mySpecialKeyboard);

	glutMainLoop();
	return 0;
}

void printMenu() {
	cout << "1		: Che do hien thi khung day" << endl;
	cout << "2		: Che do hien thi to mau 2D" << endl;
	cout << "3		: Che do hien thi to mau 3D" << endl;
	cout << "U, u	: Di chuyen len tren" << endl;
	cout << "D, d	: Di chuyen xuong duoi" << endl;
	cout << "I, i	: Phong to" << endl;
	cout << "O, o	: Thu nho" << endl;
	cout << "X, x	: Xoay quanh truc X" << endl;
	cout << "Y, y	: Xoay quanh truc Y" << endl;
	cout << "Z, z	: Xoay quanh truc Z" << endl;
	cout << "+		: Tang khoang cach camera" << endl;
	cout << "-		: Giam khoang cach camera" << endl;
	cout << "up arrow  : Tang chieu cao camera" << endl;
	cout << "down arrow: Giam chieu cao camera" << endl;
	cout << "<-        : Quay camera theo chieu kim dong ho" << endl;
	cout << "->        : Quay camera nguoc chieu kim dong ho" << endl;
	cout << "V, v	: Bat tat che do nhin thay ben tren tu phia duoi (mac dinh la khong nhin thay)" << endl;
}
void setupLight() {
	//Light
	glEnable(GL_LIGHTING);
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	//GLfloat light_position[] = { 10,10,10, 0.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	//glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_LIGHT1);
	//glEnable(GL_LIGHT2);
}
void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'U':
	case 'u':
		if (translate <= 2.5 * H)translate += 0.1;
		break;
	case 'D':
	case 'd':
		if (translate >= 0.5 * H)translate -= 0.1;
		break;
	case 'I':
	case 'i':
		if (scale <= 2) scale += 0.1;
		break;
	case 'O':
	case 'o':
		if (scale >= 0.4) scale -= 0.1;
		break;
	case 'X':
	case 'x':
		shape.rotateX += 5;
		shapeOppo.rotateX -= 5;
		break;
	case 'Y':
	case 'y':
		shape.rotateY += 5;
		shapeOppo.rotateY += 5;
		break;
	case 'Z':
	case 'z':
		shape.rotateZ += 5;
		shapeOppo.rotateZ -= 5;
		break;
	case '+':
		camera_dis += 0.2f;
		break;
	case '-':
		camera_dis -= 0.2f;
		break;
	case '1':
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		drawStyle = 1;
		break;
	case '2':
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		drawStyle = 2;
		break;
	case '3':
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		drawStyle = 3;
		break;
	case 'V':
	case 'v':
		visible = (!visible);
	}
	glutPostRedisplay();
}
void mySpecialKeyboard(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		camera_height += 0.2;
		break;
	case GLUT_KEY_DOWN:
		camera_height -= 0.2;
		break;
	case GLUT_KEY_RIGHT:
		camera_angle -= 5;
		break;
	case GLUT_KEY_LEFT:
		camera_angle += 5;
		break;
	}
	glutPostRedisplay();
}