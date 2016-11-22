/*---------------------------------------------------------------------------------------------------------------------
*                 ----                  __                        __    __                      __    *               *
*            --           --              \                      /     |  \                    /  |   *               *
*         -                   -            \                    /      |   \                  /   |   *               *
*      -                         -          \                  /       |    \                /    |   *               *
*    -                             -         \                /        |     \              /     |   *               *
*   |                               |         \              /         |      \            /      |   *               *
*   |                               |          \            /          |       \          /       |   *               *
*   |                               |           \          /           |        \        /        |   *               *
*    -                             -             \        /            |         \      /         |   *               *
*      -                         -                \      /             |          \    /          |   *               *
*         -                   -                    \    /              |           \__/           |   *               *     
*           --            --                        \__/              _|                          |__ *               * 
*                 ----                                                                                *               *
*----------------------------------------------------------------------------------------------------------------------
* OpenVolumetricMesh (OVM) library, Copyright (C) 2010-2012, Chuhua Xian                                              *
* All rights reserved                                                                                                 *
*                                                                                                                     *
* Code author: Chuhua Xian                                                                                            *
* Version: 1.0                                                                                                        *
* License                                                                                                             *  
*                                                                                                                     *
*    This file is part of OpenVolumericMesh (OVM).                                                                    *
*                                                                                                                     *
*    OpenVolumericMesh (OVM)is free software: you can redistribute it and/or modify                                   *
*    it under the terms of the GNU Lesser General Public License as                                                   *
*    published by the Free Software Foundation, either version 3 of                                                   *
*    the License, or (at your option) any later version.                                                              *
*                                                                                                                     *
*   OpenVolumericMesh distributed in the hope that it will be useful,                                                 *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of                                                    *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                                     * 
*   GNU Lesser General Public License for more details.                                                               *
*   This project is created by Chuhua Xian                                                                            *
*   Developers : Chuhua Xian,   chuhuaxian@gmail.com                                                                  *
*                                                                                                                     *
/--------------------------------------------------------------------------------------------------------------------*/


#ifndef _OVM_GOPOLOGY_H_
#define _OVM_GOPOLOGY_H_

#include <OVM/OVMCore/system/config.h>
#include <OVM/OVMCore/Mesh/Handles.h>


/// namespace OVM
namespace OVM
{
//---------------------------------------------------------------------------------------------------------------------
	/** definition of mesh items for use in the mesh*/

	typedef PolyhedronHandle HedronHandle;


	/// definition of the vertex
	class  Vertex 
	{
		friend class TopologyKernel;
	public:
		Vertex(int _heh = -1) : heh_(_heh)
		{
		}
	private:
		HalfEdgeHandle heh_;
	};

	/// definition of the half edge
	class HalfEdge
	{
		friend class TopologyKernel;
	public:
		HalfEdge(int _fh = -1, int _vh = -1, int _opp_heh = -1) : fh_(_fh), vh_(_vh), opp_heh_(_opp_heh)
		{
		}
	private:
		HalfFaceHandle     fh_;
		HalfEdgeHandle opp_heh_;
		VertexHandle   vh_;
	};

	/// definition of the half face
	class HalfFace
	{
		friend class TopologyKernel;
	public:
		HalfFace(int _heh = -1, int _opp_hfh = -1) : heh_(_heh), opp_hfh_(_opp_hfh)
		{
		}
	private:
		HalfEdgeHandle heh_;
		HalfFaceHandle opp_hfh_;
	};

	/// definition of Face
	class Face 
	{
		friend class TopologyKernel;
	public:
		Face(int _hfh0 = -1, int _hfh1 = -1)
		{
			hfh_[0] = HalfFaceHandle(_hfh0);
			hfh_[1] = HalfFaceHandle(_hfh1);
		}
	private:
		HalfFaceHandle hfh_[2];
	};

	/// definition of the hedron
	class Hedron 
	{
		friend class TopologyKernel;
	public:
		Hedron(int _hfh = -1) : hfh_(_hfh)
		{
		}
	private:
		HalfFaceHandle hfh_;
	};
//---------------------------------------------------------------------------------------------------------------------
} // namespace OVM


#endif