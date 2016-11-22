/*
	Scarb Mesh Viewer
*/
#include <iostream>
#include <string>
#include <fstream>
#include <gl/glut.h>
#include "glCamera.h"

#include <OVM/OVMCore/IO/MeshIO.h>
#include <OVM/OVMCore/Mesh/THMesh_ArrayKernelT.h>
using namespace std;
#define M_PI 3.14159265358979323846
#define UNIT 3.0f
typedef OVM::THMesh_ArrayKernelT<> Mesh;

struct V3f
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
};
static float c = M_PI / 180.0f;
static int du = 90, oldmy = -1, oldmx = -1;
static float r = 5.0f, h = 0.0f;
static GLCamera * cam = nullptr;
GLboolean mouserdown = GL_FALSE;
GLboolean mouseldown = GL_FALSE;
GLboolean mousemdown = GL_FALSE;

int main()
{
	std::string file_name;
	std::cout << "Please enter a file name: ";
	std::cin >> file_name;	
	ofstream outfile("out.txt");
	int v_cnt = 0;

	Mesh mesh;
	if (!OVM::IO::read_mesh(mesh, file_name))
	{
		std::cerr << "Cannot open the file!" << std::endl;
		return 0;
	}
	for (Mesh::HedronIter h_it = mesh.hedrons_begin(); h_it != mesh.hedrons_end(); ++ h_it)
	{
		//--- using the circulator to access the vertices of a hedron ---//
		for (Mesh::HedronVertexIter hv_it = mesh.hedron_vertex_iter(h_it); hv_it; ++ hv_it)
		{
			Mesh::Point v;
			v = mesh.point(hv_it.handle());
			v_cnt += 3;
			outfile << "Coordinate[" << v_cnt / 3 << "]: " << v[0] << ", " << v[1] << ", " << v[2] << std::endl;
		}
	}
	std::cout << "v_cnt = " << v_cnt << std::endl;
	outfile.close();

	GLfloat * vertices = new GLfloat[v_cnt];
	v_cnt = 0;
	for (Mesh::HedronIter h_it = mesh.hedrons_begin(); h_it != mesh.hedrons_end(); ++h_it)
	{
		//--- using the circulator to access the vertices of a hedron ---//
		for (Mesh::HedronVertexIter hv_it = mesh.hedron_vertex_iter(h_it); hv_it; ++hv_it)
		{
			Mesh::Point v;
			v = mesh.point(hv_it.handle());
			vertices[v_cnt++] = v[0] * UNIT;
			vertices[v_cnt++] = v[1] * UNIT;
			vertices[v_cnt++] = v[2] * UNIT;

		}
	}

	return 0;
}
/*
int main()
{
	/* Load file#1#
	cout << "Please enter a file name: ";
	cin >> file_name;
	cout << "hex or tet: ";
	cin >> file_format;

	if (!OVM::IO::read_mesh(mesh, file_name))
	{
		cerr << "Cannot open the file!" << endl;
		return 0;
	}
}*/