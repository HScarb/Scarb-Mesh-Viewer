#include "glCamera.h"

GLCamera::GLCamera()
{
}

GLCamera::GLCamera(const Vector3d& pos, const Vector3d& target, const Vector3d& up)
{
	m_pos = pos;
	m_target = target;
	m_up = up;
	n = Vector3d(pos.x() - target.x(), pos.y() - target.y(), pos.z() - target.z());
	u = Vector3d(up.cross(n).x(), up.cross(n).y(), up.cross(n).z());
	v = Vector3d(n.cross(u).x(), n.cross(u).y(), n.cross(u).z());

	n.normalize();
	u.normalize();
	v.normalize();

	setModelViewMatrix();
}

void GLCamera::setModelViewMatrix()
{
	double m[16];
	m[0] = u.x(); m[4] = u.y(); m[8] = u.z(); m[12] = -m_pos.dot(u);
	m[1] = v.x(); m[5] = v.y(); m[9] = v.z(); m[13] = -m_pos.dot(v);
	m[2] = n.x(); m[6] = n.y(); m[10] = n.z(); m[14] = -m_pos.dot(n);
	m[3] = 0;  m[7] = 0;  m[11] = 0;  m[15] = 1.0;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(m);			// 用矩阵替换原视点矩阵
}

void GLCamera::setShape(float viewAngle, float aspect, float Near, float Far)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(viewAngle, aspect, Near, Far);
}

void GLCamera::slide(float du, float dv, float dn)
{
	m_pos(0) = m_pos(0) + du*u.x() + dv*v.x() + dn*n.x();
	m_pos(1) = m_pos(1) + du*u.y() + dv*v.y() + dn*n.y();
	m_pos(2) = m_pos(2) + du*u.z() + dv*v.z() + dn*n.z();
	m_target(0) = m_target(0) + du*u.x() + dv*v.x() + dn*n.x();
	m_target(1) = m_target(0) + du*u.y() + dv*v.y() + dn*n.y();
	m_target(2) = m_target(0) + du*u.z() + dv*v.z() + dn*n.z();
	setModelViewMatrix();
}

void GLCamera::roll(float angle)
{
	float cs = cos(angle*3.14159265 / 180);
	float sn = sin(angle*3.14159265 / 180);
	Vector3d t(u);
	Vector3d s(v);
	u.x() = cs*t.x() - sn*s.x();
	u.y() = cs*t.y() - sn*s.y();
	u.z() = cs*t.z() - sn*s.z();

	v.x() = sn*t.x() + cs*s.x();
	v.y() = sn*t.y() + cs*s.y();
	v.z() = sn*t.z() + cs*s.z();

	setModelViewMatrix();          //每次计算完坐标轴变化后调用此函数更新视点矩阵  
}

void GLCamera::yaw(float angle)
{
	float cs = cos(angle*3.14159265 / 180);
	float sn = sin(angle*3.14159265 / 180);
	Vector3d t(n);
	Vector3d s(u);

	n.x() = cs*t.x() - sn*s.x();
	n.y() = cs*t.y() - sn*s.y();
	n.z() = cs*t.z() - sn*s.z();

	u.x() = sn*t.x() + cs*s.x();
	u.y() = sn*t.y() + cs*s.y();
	u.z() = sn*t.z() + cs*s.z();

	setModelViewMatrix();
}

void GLCamera::pitch(float angle)
{
	float cs = cos(angle*3.14159265 / 180);
	float sn = sin(angle*3.14159265 / 180);
	Vector3d t(v);
	Vector3d s(n);

	v.x() = cs*t.x() - sn*s.x();
	v.y() = cs*t.y() - sn*s.y();
	v.z() = cs*t.z() - sn*s.z();

	n.x() = sn*t.x() + cs*s.x();
	n.y() = sn*t.y() + cs*s.y();
	n.z() = sn*t.z() + cs*s.z();


	setModelViewMatrix();
}

float GLCamera::getDist()
{
	float dist = pow(m_pos.x(), 2) + pow(m_pos.y(), 2) + pow(m_pos.z(), 2);
	return pow(dist, 0.5);
}