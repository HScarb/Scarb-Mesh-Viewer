/*
MyMeshViewer
*/
#include <OVM/OVMCore/IO/MeshIO.h>
#include <OVM/OVMCore/Mesh/THMesh_ArrayKernelT.h>
//#include "OVM/OVMCore/IO/MeshIO.h"
//#include "OVM/OVMCore/Mesh/THMesh_ArrayKernelT.h"
#include <iostream>
#include <string>
#include <gl/glut.h>
#include "glCamera.h"
#include <vector>
using namespace std;
#define M_PI 3.14159265358979323846
#define UNIT 0.3f
#define MAXARRAYSUM 100000
typedef OVM::THMesh_ArrayKernelT<> Mesh;

typedef struct V3f
{
public:
	V3f()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	V3f(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	float x, y, z;
}V3f;

static float c = M_PI / 180.0f;
static int du = 90, oldmy = -1, oldmx = -1;
static float r = 5.0f, h = 0.0f;
static GLCamera * cam = nullptr;
string file_name;
string file_format;
Mesh mesh;
int v_cnt;
GLushort * item_indices;
GLushort * wire_indices;
GLfloat * vertices;
GLfloat * linesArray;
GLfloat * verticesArray;


GLboolean mouserdown = GL_FALSE;
GLboolean mouseldown = GL_FALSE;
GLboolean mousemdown = GL_FALSE;

vector<V3f> vertexVector;

void setupPointers()
{
	v_cnt = 0;
	for (Mesh::HedronIter h_it = mesh.hedrons_begin(); h_it != mesh.hedrons_end(); ++h_it)
	{
		// --- using the circulator to access the vertices of a hedron ---
		for (Mesh::HedronVertexIter hv_it = mesh.hedron_vertex_iter(h_it); hv_it; ++hv_it)
		{
			/*Mesh::Point v;
			v = mesh.point(hv_it.handle());
			vertices[v_cnt++] = v[0] * UNIT;
			vertices[v_cnt++] = v[1] * UNIT;
			vertices[v_cnt++] = v[2] * UNIT;*/
			v_cnt += 3;
		}
	}
	vertices = new GLfloat[v_cnt];
	v_cnt = 0;
	for (Mesh::HedronIter h_it = mesh.hedrons_begin(); h_it != mesh.hedrons_end(); ++h_it)
	{
		// --- using the circulator to access the vertices of a hedron ---
		for (Mesh::HedronVertexIter hv_it = mesh.hedron_vertex_iter(h_it); hv_it; ++hv_it)
		{
			Mesh::Point v;
			v = mesh.point(hv_it.handle());
			vertices[v_cnt++] = v[0] * UNIT;
			vertices[v_cnt++] = v[1] * UNIT;
			vertices[v_cnt++] = v[2] * UNIT;
			vertexVector.push_back(V3f(v[0], v[1], v[2]));
		}
	}
	v_cnt /= 3;
	cout << "Total vertices: " << v_cnt << endl;
	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	if (file_format == "hex")
	{
		const int indexNum = v_cnt * 3;
		item_indices = new GLushort[indexNum];
		wire_indices = new GLushort[indexNum];
		cout << "indexNum: " << indexNum << endl;
		const int step1 = 24;
		const int step2 = 8;

		for (int cnt = 0; cnt < (v_cnt / 8); cnt++)
		{
			// item indices
			item_indices[cnt * step1 + 0] = 0 + step2 * cnt;
			item_indices[cnt * step1 + 1] = 1 + step2 * cnt;
			item_indices[cnt * step1 + 2] = 2 + step2 * cnt;
			item_indices[cnt * step1 + 3] = 3 + step2 * cnt;
			item_indices[cnt * step1 + 4] = 0 + step2 * cnt;
			item_indices[cnt * step1 + 5] = 1 + step2 * cnt;
			item_indices[cnt * step1 + 6] = 5 + step2 * cnt;
			item_indices[cnt * step1 + 7] = 4 + step2 * cnt;
			item_indices[cnt * step1 + 8] = 5 + step2 * cnt;
			item_indices[cnt * step1 + 9] = 4 + step2 * cnt;
			item_indices[cnt * step1 + 10] = 7 + step2 * cnt;
			item_indices[cnt * step1 + 11] = 6 + step2 * cnt;
			item_indices[cnt * step1 + 12] = 2 + step2 * cnt;
			item_indices[cnt * step1 + 13] = 3 + step2 * cnt;
			item_indices[cnt * step1 + 14] = 7 + step2 * cnt;
			item_indices[cnt * step1 + 15] = 6 + step2 * cnt;
			item_indices[cnt * step1 + 16] = 1 + step2 * cnt;
			item_indices[cnt * step1 + 17] = 2 + step2 * cnt;
			item_indices[cnt * step1 + 18] = 6 + step2 * cnt;
			item_indices[cnt * step1 + 19] = 5 + step2 * cnt;
			item_indices[cnt * step1 + 20] = 0 + step2 * cnt;
			item_indices[cnt * step1 + 21] = 3 + step2 * cnt;
			item_indices[cnt * step1 + 22] = 7 + step2 * cnt;
			item_indices[cnt * step1 + 23] = 4 + step2 * cnt;
			// wire indices
			wire_indices[cnt * step1 + 0] = 0 + step2 * cnt;
			wire_indices[cnt * step1 + 1] = 1 + step2 * cnt;
			wire_indices[cnt * step1 + 2] = 1 + step2 * cnt;
			wire_indices[cnt * step1 + 3] = 2 + step2 * cnt;
			wire_indices[cnt * step1 + 4] = 2 + step2 * cnt;
			wire_indices[cnt * step1 + 5] = 3 + step2 * cnt;
			wire_indices[cnt * step1 + 6] = 3 + step2 * cnt;
			wire_indices[cnt * step1 + 7] = 0 + step2 * cnt;
			wire_indices[cnt * step1 + 8] = 0 + step2 * cnt;
			wire_indices[cnt * step1 + 9] = 4 + step2 * cnt;
			wire_indices[cnt * step1 + 10] = 4 + step2 * cnt;
			wire_indices[cnt * step1 + 11] = 5 + step2 * cnt;
			wire_indices[cnt * step1 + 12] = 5 + step2 * cnt;
			wire_indices[cnt * step1 + 13] = 6 + step2 * cnt;
			wire_indices[cnt * step1 + 14] = 6 + step2 * cnt;
			wire_indices[cnt * step1 + 15] = 7 + step2 * cnt;
			wire_indices[cnt * step1 + 16] = 7 + step2 * cnt;
			wire_indices[cnt * step1 + 17] = 4 + step2 * cnt;
			wire_indices[cnt * step1 + 18] = 3 + step2 * cnt;
			wire_indices[cnt * step1 + 19] = 7 + step2 * cnt;
			wire_indices[cnt * step1 + 20] = 2 + step2 * cnt;
			wire_indices[cnt * step1 + 21] = 6 + step2 * cnt;
			wire_indices[cnt * step1 + 22] = 1 + step2 * cnt;
			wire_indices[cnt * step1 + 23] = 5 + step2 * cnt;
		}
	}
	// tet
	else
	{
		const int indexNum = v_cnt * 3;
		item_indices = new GLushort[indexNum];
		wire_indices = new GLushort[indexNum];
		cout << "indexNum: " << indexNum << endl;
		const int step1 = 12;
		const int step2 = 4;
		for (int cnt = 0; cnt < (v_cnt / 4); cnt++)
		{
			item_indices[cnt * step1 + 0] = 0 + step2 * cnt;
			item_indices[cnt * step1 + 1] = 1 + step2 * cnt;
			item_indices[cnt * step1 + 2] = 2 + step2 * cnt;
			item_indices[cnt * step1 + 3] = 0 + step2 * cnt;
			item_indices[cnt * step1 + 4] = 1 + step2 * cnt;
			item_indices[cnt * step1 + 5] = 3 + step2 * cnt;
			item_indices[cnt * step1 + 6] = 0 + step2 * cnt;
			item_indices[cnt * step1 + 7] = 2 + step2 * cnt;
			item_indices[cnt * step1 + 8] = 3 + step2 * cnt;
			item_indices[cnt * step1 + 9] = 1 + step2 * cnt;
			item_indices[cnt * step1 + 10] = 2 + step2 * cnt;
			item_indices[cnt * step1 + 11] = 3 + step2 * cnt;

			wire_indices[cnt * step1 + 0] = 0 + step2 * cnt;
			wire_indices[cnt * step1 + 1] = 1 + step2 * cnt;
			wire_indices[cnt * step1 + 2] = 0 + step2 * cnt;
			wire_indices[cnt * step1 + 3] = 2 + step2 * cnt;
			wire_indices[cnt * step1 + 4] = 0 + step2 * cnt;
			wire_indices[cnt * step1 + 5] = 3 + step2 * cnt;
			wire_indices[cnt * step1 + 6] = 1 + step2 * cnt;
			wire_indices[cnt * step1 + 7] = 2 + step2 * cnt;
			wire_indices[cnt * step1 + 8] = 2 + step2 * cnt;
			wire_indices[cnt * step1 + 9] = 3 + step2 * cnt;
			wire_indices[cnt * step1 + 10] = 3 + step2 * cnt;
			wire_indices[cnt * step1 + 11] = 1 + step2 * cnt;
		}
	}
/*	for (int i = 0; i < v_cnt;i++)
	{
		cout << "vertices[" << i << "] = " << vertices[i] / UNIT << endl;
	}*/
}

// 将顶点数据读取，并且为DrawArrays排序
void setupPointersByArray()
{
	v_cnt = 0;
	for (Mesh::HedronIter h_it = mesh.hedrons_begin(); h_it != mesh.hedrons_end(); ++h_it)
	{
		// --- using the circulator to access the vertices of a hedron ---
		for (Mesh::HedronVertexIter hv_it = mesh.hedron_vertex_iter(h_it); hv_it; ++hv_it)
		{
			/*Mesh::Point v;
			v = mesh.point(hv_it.handle());
			vertices[v_cnt++] = v[0] * UNIT;
			vertices[v_cnt++] = v[1] * UNIT;
			vertices[v_cnt++] = v[2] * UNIT;*/
			v_cnt += 3;
		}
	}
	vertices = new GLfloat[v_cnt];
	v_cnt = 0;
	for (Mesh::HedronIter h_it = mesh.hedrons_begin(); h_it != mesh.hedrons_end(); ++h_it)
	{
		// --- using the circulator to access the vertices of a hedron ---
		for (Mesh::HedronVertexIter hv_it = mesh.hedron_vertex_iter(h_it); hv_it; ++hv_it)
		{
			Mesh::Point v;
			v = mesh.point(hv_it.handle());
			vertices[v_cnt++] = v[0] * UNIT;
			vertices[v_cnt++] = v[1] * UNIT;
			vertices[v_cnt++] = v[2] * UNIT;
			vertexVector.push_back(V3f(v[0], v[1], v[2]));
		}
	}
	v_cnt /= 3;
	cout << "Total vertices: " << v_cnt << endl;

	/*glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);*/

	if (file_format == "hex")
	{
		const int indexNum = v_cnt * 9;
		verticesArray = new GLfloat[indexNum];
		linesArray = new GLfloat[indexNum];
		cout << "indexNum: " << indexNum << endl;
		const int step1 = 72;
		const int step2 = 24;
		const int dim = 3;
		int i = 0;



		// 绘制面
		// cnt 六面体编号
		for (int cnt = 0; cnt < (v_cnt / 8); cnt++)
		{
			i = 0;
			// bottom
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 2];
			// front					
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 2];
			// right					
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 2];
			// back						
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 2];
			// left						
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 2];
			// top						
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 2];
		}

		// 绘制边
		for (int cnt = 0; cnt < (v_cnt / 8); cnt++)
		{
			i = 0;
			// bottom
			// 0-1
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 2];
			// 1-2
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 2];
			// 2-3
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 2];
			// 3-0
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 2];
			// top
			// 4-5
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 2];
			// 5-6
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 2];
			// 6-7
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 2];
			// 7-4
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 2];
			// front
			// 2-6
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 6 * dim + 2];
			// 3-7
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 7 * dim + 2];
			// back
			// 0-4
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 4 * dim + 2];
			// 1-5
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 5 * dim + 2];
		}
	}
	// tet
	else
	{
		const int indexNum = v_cnt * 9;
		verticesArray = new GLfloat[indexNum];
		linesArray = new GLfloat[indexNum];
		cout << "indexNum: " << indexNum << endl;
		const int step1 = 36;
		const int step2 = 12;
		const int dim = 3;
		int i = 0;

		// 绘制面
		for (int cnt = 0; cnt < (v_cnt / 4); cnt++)
		{
			i = 0;
			// bottom
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 2];
			// left
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 2];
			// back
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 2];
			// front
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 2];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 0];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 1];
			verticesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 2];
		}
		// 绘制边
		for (int cnt = 0; cnt < (v_cnt / 4); cnt++)
		{
			i = 0;
			// bottom
			// 0-1
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 2];
			// 1-2
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 2];
			// 2-0
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 2];
			// left
			// 0-3
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 0 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 2];
			// 2-3
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 2 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 2];
			// back
			// 1-3
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 1 * dim + 2];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 0];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 1];
			linesArray[cnt * step1 + (i++)] = vertices[cnt * step2 + 3 * dim + 2];
		}
	}
}

void RotateX(float angle)
{
	float d = cam->getDist();
	int cnt = 100;
	float theta = angle / cnt;
	float slide_d = -2 * d * sin(theta * M_PI / 360);
	cam->yaw(theta / 2);
	for (; cnt != 0; --cnt)
	{
		cam->slide(slide_d, 0, 0);
		cam->yaw(theta);
	}
	cam->yaw(-theta / 2);
}

void RotateY(float angle)
{
	float d = cam->getDist();
	int cnt = 100;
	float theta = angle / cnt;
	float slide_d = 2 * d * sin(theta * M_PI / 360);
	cam->pitch(theta / 2);
	for (; cnt != 0; --cnt)
	{
		cam->slide(0, slide_d, 0);
		cam->pitch(theta);
	}
	cam->pitch(-theta / 2);
}

void drawtet(V3f p1, V3f p2, V3f p3, V3f p4, V3f itemColor, V3f wireColor)
{
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(itemColor.x, itemColor.y, itemColor.z);
	glVertex3f(p1.x * UNIT, p1.y * UNIT, p1.z * UNIT);
	glVertex3f(p2.x * UNIT, p2.y * UNIT, p2.z * UNIT);
	glVertex3f(p3.x * UNIT, p3.y * UNIT, p3.z * UNIT);
	glVertex3f(p4.x * UNIT, p4.y * UNIT, p4.z * UNIT);
	glVertex3f(p2.x * UNIT, p2.y * UNIT, p2.z * UNIT);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(itemColor.x, itemColor.y, itemColor.z);
	glVertex3f(p2.x * UNIT, p2.y * UNIT, p2.z * UNIT);
	glVertex3f(p3.x * UNIT, p3.y * UNIT, p3.z * UNIT);
	glVertex3f(p4.x * UNIT, p4.y * UNIT, p4.z * UNIT);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(wireColor.x, wireColor.y, wireColor.z);
	glVertex3f(p1.x * UNIT, p1.y * UNIT, p1.z * UNIT);
	glVertex3f(p2.x * UNIT, p2.y * UNIT, p2.z * UNIT);
	glVertex3f(p1.x * UNIT, p1.y * UNIT, p1.z * UNIT);
	glVertex3f(p3.x * UNIT, p3.y * UNIT, p3.z * UNIT);
	glVertex3f(p1.x * UNIT, p1.y * UNIT, p1.z * UNIT);
	glVertex3f(p4.x * UNIT, p4.y * UNIT, p4.z * UNIT);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glColor3f(wireColor.x, wireColor.y, wireColor.z);
	glVertex3f(p2.x * UNIT, p2.y * UNIT, p2.z * UNIT);
	glVertex3f(p3.x * UNIT, p3.y * UNIT, p3.z * UNIT);
	glVertex3f(p4.x * UNIT, p4.y * UNIT, p4.z * UNIT);
	glEnd();
}

void drawhex(V3f p1, V3f p2, V3f p3, V3f p4, V3f p5, V3f p6, V3f p7, V3f p8,
	V3f itemColor, V3f wireColor)
{
	glBegin(GL_QUAD_STRIP);
	glColor3f(itemColor.x, itemColor.y, itemColor.z);
	glVertex3f(p1.x * UNIT, p1.y * UNIT, p1.z * UNIT);
	glVertex3f(p2.x * UNIT, p2.y * UNIT, p2.z * UNIT);
	glVertex3f(p4.x * UNIT, p4.y * UNIT, p4.z * UNIT);
	glVertex3f(p3.x * UNIT, p3.y * UNIT, p3.z * UNIT);
	glVertex3f(p8.x * UNIT, p8.y * UNIT, p8.z * UNIT);
	glVertex3f(p7.x * UNIT, p7.y * UNIT, p7.z * UNIT);
	glVertex3f(p5.x * UNIT, p5.y * UNIT, p5.z * UNIT);
	glVertex3f(p6.x * UNIT, p6.y * UNIT, p6.z * UNIT);
	glVertex3f(p1.x * UNIT, p1.y * UNIT, p1.z * UNIT);
	glVertex3f(p2.x * UNIT, p2.y * UNIT, p2.z * UNIT);
	glEnd();
	glBegin(GL_QUADS);
	glColor3f(itemColor.x, itemColor.y, itemColor.z);
	glVertex3f(p2.x * UNIT, p2.y * UNIT, p2.z * UNIT);
	glVertex3f(p3.x * UNIT, p3.y * UNIT, p3.z * UNIT);
	glVertex3f(p7.x * UNIT, p7.y * UNIT, p7.z * UNIT);
	glVertex3f(p6.x * UNIT, p6.y * UNIT, p6.z * UNIT);
	glVertex3f(p1.x * UNIT, p1.y * UNIT, p1.z * UNIT);
	glVertex3f(p4.x * UNIT, p4.y * UNIT, p4.z * UNIT);
	glVertex3f(p8.x * UNIT, p8.y * UNIT, p8.z * UNIT);
	glVertex3f(p5.x * UNIT, p5.y * UNIT, p5.z * UNIT);
	glEnd();
	/* wire */
	glBegin(GL_LINE_STRIP);
	glColor3f(wireColor.x, wireColor.y, wireColor.z);
	glVertex3f(p1.x * UNIT, p1.y * UNIT, p1.z * UNIT);
	glVertex3f(p2.x * UNIT, p2.y * UNIT, p2.z * UNIT);
	glVertex3f(p3.x * UNIT, p3.y * UNIT, p3.z * UNIT);
	glVertex3f(p4.x * UNIT, p4.y * UNIT, p4.z * UNIT);
	glVertex3f(p1.x * UNIT, p1.y * UNIT, p1.z * UNIT);
	glVertex3f(p5.x * UNIT, p5.y * UNIT, p5.z * UNIT);
	glVertex3f(p6.x * UNIT, p6.y * UNIT, p6.z * UNIT);
	glVertex3f(p7.x * UNIT, p7.y * UNIT, p7.z * UNIT);
	glVertex3f(p8.x * UNIT, p8.y * UNIT, p8.z * UNIT);
	glVertex3f(p5.x * UNIT, p5.y * UNIT, p5.z * UNIT);
	glEnd();
	glBegin(GL_LINES);
	glColor3f(wireColor.x, wireColor.y, wireColor.z);
	glVertex3f(p2.x * UNIT, p2.y * UNIT, p2.z * UNIT);
	glVertex3f(p6.x * UNIT, p6.y * UNIT, p6.z * UNIT);
	glVertex3f(p3.x * UNIT, p3.y * UNIT, p3.z * UNIT);
	glVertex3f(p7.x * UNIT, p7.y * UNIT, p7.z * UNIT);
	glVertex3f(p4.x * UNIT, p4.y * UNIT, p4.z * UNIT);
	glVertex3f(p8.x * UNIT, p8.y * UNIT, p8.z * UNIT);
	glEnd();
}

// glBegin glEnd
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);

	glLoadIdentity();
	//	gluLookAt(r*cos(c*du), h, r*sin(c*du), 0, 0, 0, 0, 1, 0); //从视点看远点,y轴方向(0,1,0)是上方向  
	//	gluLookAt(r*cos(c*du), 0, r*sin(c*du) + 50, 0, 0, 0, 0, 1, 0); //从视点看远点,y轴方向(0,1,0)是上方向  
	cam->setModelViewMatrix();


	if(file_format == "hex")
	{
		int i = 0;
		while (i < vertexVector.size())
		{
			drawhex(
				vertexVector[i++],
				vertexVector[i++],
				vertexVector[i++],
				vertexVector[i++],
				vertexVector[i++],
				vertexVector[i++],
				vertexVector[i++],
				vertexVector[i++],
				V3f(0.3f, 0.8f, 0.3f),
				V3f(1.0f, 1.0f, 1.0f));
		}
	}
	else
	{
		int i = 0;
		while (i < vertexVector.size())
		{
			drawtet(vertexVector[i++],
				vertexVector[i++],
				vertexVector[i++],
				vertexVector[i++],
				V3f(0.3f, 0.8f, 0.3f),
				V3f(1.0f, 1.0f, 1.0f));
		}
	}
	
	glPopMatrix();
	glutSwapBuffers();
}

// glDrawElements
void display2()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	
/*	glPushMatrix();
	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);*/


	glLoadIdentity();
	//	gluLookAt(r*cos(c*du), h, r*sin(c*du), 0, 0, 0, 0, 1, 0); //从视点看远点,y轴方向(0,1,0)是上方向  
	//	gluLookAt(r*cos(c*du), 0, r*sin(c*du) + 50, 0, 0, 0, 0, 1, 0); //从视点看远点,y轴方向(0,1,0)是上方向  
	cam->setModelViewMatrix();
	
	// hex
	if(file_format == "hex")
	{
		const int indexNum = v_cnt * 3;

		glColor3f(0.3f, 0.8f, 0.3f);
		glDrawElements(GL_QUADS, indexNum, GL_UNSIGNED_SHORT, item_indices);
		glColor3f(1.0f, 1.0f, 1.0f);
		glDrawElements(GL_LINES, indexNum, GL_UNSIGNED_SHORT, wire_indices);
	}
	// tet
	else
	{
		const int indexNum = v_cnt * 3;

		glColor3f(0.3f, 0.8f, 0.3f);
		glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_SHORT, item_indices);
		glColor3f(1.0f, 1.0f, 1.0f);
		glDrawElements(GL_LINES, indexNum, GL_UNSIGNED_SHORT, wire_indices);
	}

	/*glEnable(GL_COLOR_MATERIAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisableClientState(GL_VERTEX_ARRAY);*/

	glutSwapBuffers();
}

// glDrawArray
void display3()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH, GL_NICEST);

	glLoadIdentity();
	cam->setModelViewMatrix();

	// hex
	if (file_format == "hex")
	{
		const int indexNum = v_cnt * 3;
		// 绘制面
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, verticesArray);
		glColor3f(0.3f, 0.8f, 0.3f);
		glDrawArrays(GL_QUADS, 0, v_cnt * 3);
		// 绘制边
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, linesArray);
		glColor3f(1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_LINES, 0, v_cnt * 3);
	}
	// tet
	else
	{
		const int indexNum = v_cnt * 3;

		// 绘制面
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, verticesArray);
		glColor3f(0.3f, 0.8f, 0.3f);
		glDrawArrays(GL_TRIANGLES, 0, v_cnt * 3);
		// 绘制边
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, linesArray);
		glColor3f(1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_LINES, 0, v_cnt * 3);
	}

	glutSwapBuffers();
}
void Mouse(int button, int state, int x, int y)		// mouse click
{
	if (state == GLUT_DOWN)	// 第一次鼠标按下时记录在窗口中的坐标
	{
		if (button == GLUT_RIGHT_BUTTON)
			mouserdown = GL_TRUE;
		else if (button == GLUT_LEFT_BUTTON)
			mouseldown = GL_TRUE;
		else if (button == GLUT_MIDDLE_BUTTON)
			mousemdown = GL_TRUE;
	}
	else
	{
		if (button == GLUT_RIGHT_BUTTON)
			mouserdown = GL_FALSE;
		else if (button == GLUT_LEFT_BUTTON)
			mouseldown = GL_FALSE;
		else if (button == GLUT_MIDDLE_BUTTON)
			mousemdown = GL_FALSE;
	}
	oldmx = x, oldmy = y;
}

void onMouseMove(int x, int y)
{
	int dx = x - oldmx;
	int dy = y - oldmy;
	if (mouseldown == GL_TRUE)
	{
		RotateX(dx);
		RotateY(dy);
	}
	else if (mouserdown == GL_TRUE)
	{
		cam->roll(dx);
	}
	else if (mousemdown == GL_TRUE)
	{
		cam->slide(-dx, dy, 0);
	}
	oldmx = x;
	oldmy = y;
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	Vector3d pos(0.0, 0.0, 100.0);
	Vector3d target(0.0, 0.0, 0.0);
	Vector3d up(0.0, 1.0, 0.0);
	cam = new GLCamera(pos, target, up);
	setupPointersByArray();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//	gluPerspective(75.0f, (float)w / h, 1.0f, 1000.0f);
	cam->setShape(45.0, (GLfloat)w / (GLfloat)h, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char * argv[])
{
	/* Load file*/
	cout << "Please enter a file name: ";
	cin >> file_name;
	cout << "hex or tet: ";
	cin >> file_format;

	if (!OVM::IO::read_mesh(mesh, file_name))
	{
		cerr << "Cannot open the file!" << endl;
		return 0;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	glutCreateWindow("OpenGL");
	init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display3);
	glutIdleFunc(display3);  //设置不断调用显示函数
	glutMouseFunc(Mouse);
	glutMotionFunc(onMouseMove);
	glutMainLoop();
	return 0;
}