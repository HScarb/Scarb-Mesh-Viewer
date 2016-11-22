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

#ifndef _OVM_IO_MANAGER_H_
#define _OVM_IO_MANAGER_H_

#include <OVM/OVMCore/system/config.h>

#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>

#define _INPUTLINESIZE_ 4096

namespace OVM
{
//---------------------------------------------------------------------------------------------------------------------
	namespace IO
	{
		class _IOManager_
		{
		public:
			_IOManager_()
			{
			}
		public:
			friend _IOManager_ & IOManager();
		//-------------------------------------------------------------------------------------------------------------
		public:
			template < class MeshT >
			bool read_mesh(MeshT & _mesh, const std::string & _filename)
			{
				if (_filename.find(".mesh") == (_filename.size() - 5))
				{					
					return read_MEdit_file(_mesh, _filename);
				}
				else if (_filename.find(".hex") == (_filename.size() - 4))
				{
					return read_hex_mesh(_mesh, _filename);
				}
				else if (_filename.find(".nmesh") == (_filename.size() - 6))
				{
					return read_Netgen_mesh(_mesh, _filename);
				}				
				return true;				
			}
			template < class MeshT >
			bool write_mesh(const MeshT & _mesh, const std::string & _filename)
			{
				if (_filename.find(".mesh") == (_filename.size() - 5))
				{
					//write_MEdit_mesh(_mesh, _filename);
					//return write_mesh_opp_half_edges(_mesh, _filename.substr(0, _filename.find('.')) + "_opp.txt");
					return write_MEdit_mesh(_mesh, _filename);
				}
				return true;				
			}
			template < class MeshT >
			bool write_mesh_opp_hes(const MeshT & _mesh, const std::string & _filename)
			{
				return write_mesh_opp_half_edges(_mesh, _filename.substr(0, _filename.find('.')) + "_opp.txt");
			}

		//-------------------------------------------------------------------------------------------------------------
		protected:

			/** read the MEdit mesh file.
			*   \param _mesh the mesh
			*   \param _filename the name of the file
			*/
			template < class MeshT >
			bool read_MEdit_file(MeshT & _mesh, const std::string & _filename)
			{
				if (_filename.find(".mesh") != _filename.size() - 5)
				{
					std::cerr << "Error : the format of the file is invalid \n";

					return false;
				}	

				std::ifstream f(_filename.c_str());
				if (!f.is_open())
				{
					std::cerr << "Error : File not found ! \n";
					return false;
				}

				int lineCount;		// 行号?
				int dim;			// 维度
				unsigned int nv;	// vertex 顶点个数
				unsigned int nf;		// face: Triangles/Quadrilaterals 面的个数
				unsigned int nh;		// hedron: Tetrahedra/Hexahedra 三维图形的个数
				unsigned int corners;	// 面的角数量，三角形为3，四边形为4
				unsigned int mt;
				unsigned int nt;		// 
				mt = 0;
				char buffer[_INPUTLINESIZE_];
				char * buff;
				char * buf;
				lineCount = 0; 
				nv = 0;
				nf = 0;
				nh = 0;
				lineCount = 0;
				corners = 0;								
				dim = 0;
				std::vector<typename MeshT::Point> pc;		// 所有点的容器，存放所有顶点
				std::vector<unsigned int> indices;			// 储存顶点的下标顺序?
				typename MeshT::Scalar cdt[4];				// 4维容器,用于存放一个点的坐标
				int idx[9];

				//--- read the medit file
				while ((buff = read_line_chars(buffer, f, lineCount)) != NULL)		// 每次读一行
				{
					if (buff[0] != '#')
					{
						//--- read the dim sector
						if (!dim)
						{					
							// buf = Dimension的位置
							buf = strstr(buff, "Dimension");		// strstr()返回str2是否是str1的子串。如果是，则该函数返回str2在str1中首次出现的地址；否则，返回NULL。
							if (buf)		// 如果找到Dimension
							{
								buf = trim_str(buf);				// 去掉空格制表符等符号
								if (strlen(buf) != 9)
								{									
									buf = find_next_sub_str(buf);
									dim = (int)strtod(buf, & buf);	// 将字符串转换成浮点数
								}
								else
								{
									f >> dim;						// 接收维度
								}
								if ((dim != 2) && (dim != 3))		// 如果不是2维和3维，报错
								{
									std::cerr << "Unknown dimension...\n";
									f.close();
									return false;
								}						
							}

						}
						//--- read the vertices sector
						if (dim && (!nv))
						{
							buf = strstr(buff, "Vertices");
							if (buf)
							{
								f >> nv;
								if (!nv)
								{
									f.close();
									return false;
								}
								pc.resize(nv, typename MeshT::Point(0, 0, 0));
								for (unsigned int k = 0; k < nv; k ++)
								{
									f >> cdt[0] >> cdt[1] >> cdt[2] >> cdt[3];
									pc[k] = typename  MeshT::Point(cdt);
								}						
							}
						}
						//--- read the face sector
						if (nv && (!nf))
						{
							buf = strstr(buff, "Triangles");
							if (buf)
							{
								f >> nf;
								corners = 3;
							}
							else
							{
								buf = strstr(buff, "Quadrilaterals");
								if (buf)
								{
									f >> nf;
									corners = 4;
								}								
							}							
						}
						//--- read the hedron sector
						if (nv && (!nh))
						{
							buf = strstr(buff, "Tetrahedra");
							if (buf)
							{
								mt = 5;
								f >> nh;
							}
							else if ((buf = strstr(buff, "Hexahedra")) != NULL)
							{
								mt = 9;
								f >> nh;
							}
							if (mt)
							{

								nt = mt - 1;
								indices.reserve(nh);
								//--- read the indices 
								for (unsigned int i = 0; i < nh; i ++)
								{
									for (unsigned int k = 0; k < mt; ++ k)
									{	
										f >> idx[k];
									}
									for (unsigned int k = 0; k < nt; ++ k)
									{
										indices.push_back(idx[k] - 1);
									}								
								}
							}
							
						} // end of the indices sector
						
					} // end of the '#' check

				} // end of while
				f.close();
				if ( mt == 5 )
				{
					_mesh.set_mesh_type(1);
				}
				else if ( mt == 9 )
				{
					_mesh.set_mesh_type(2);
				}
				_mesh.set_points(pc);
				return _mesh.build_topology(indices);
			}

			/** read the HEX mesh file.
			*   \param _mesh the mesh
			*   \param _filename the name of the file
			*/
			template < class MeshT >
			bool read_hex_mesh(MeshT & _mesh, const std::string & _filename)
			{
				if (_filename.find(".hex") != _filename.size() - 4)
				{
					std::cerr << "Error : not a .hex file!" << std::endl;
					return false;
				}
				std::ifstream fs(_filename.c_str());

				int lineCount;
				char   buffer[_INPUTLINESIZE_];
				char * buff;
				char * next_str;
				unsigned int nv;
				unsigned int nt;
				int firstLineNumber;
				lineCount = 0;
				nv = 0;
				nt = 0;
				firstLineNumber = 1;

				std::vector<typename MeshT::Point> pc;
				std::vector<unsigned int>  vc;
				double v[3];
				int idx[9];



				while ((buff = read_line_chars(buffer, fs, lineCount)) != NULL) 
				{
					//trim_str(buffer);
					if (buff[0] != '#') 
					{	
						if ((buffer[0] == 'v') || (buffer[0] == 'V'))
						{
							next_str = buff;
							for (unsigned int i = 0; i < 3; i ++)
							{
								next_str = find_next_sub_str(next_str);
								//std::istringstream ss(std::string(next_str));
								//ss >> v[i];
								v[i] = atof(next_str);
							}
							pc.push_back(typename MeshT::Point(v[0], v[1], v[2]));
							++ nv;


						}

						if ((buffer[0] == 'h') || (buffer[0] == 'H'))
						{
							next_str = buffer;
							for (unsigned int i = 0; i < 8; i ++)
							{
								next_str = find_next_sub_str(next_str);
								//std::istringstream ss(std::string(next_str));
								//ss >> idx[i];
								idx[i] = atoi(next_str);
								vc.push_back(idx[i] - 1);
							}
							++ nt;
							//break;
						}

					} // end of if '#'
				} // end of while
				fs.close();
				_mesh.set_points(pc);
				_mesh.set_mesh_type(2);


				return _mesh.build_topology(vc);

			}
			template <typename MeshT>
			bool read_NG_mesh(MeshT & _mesh, const std::string & _file_name)
			{
				if (_file_name.find(".nmesh") != _file_name.size() - 6)
				{
					std::cerr << "Error : the format of the file is invalid \n";

					return false;
				}	

				std::ifstream f(_file_name.c_str());
				if (!f.is_open())
				{
					std::cerr << "Error : File not found ! \n";
					return false;
				}

				std::vector<typename MeshT::Point> pc;
				std::vector< unsigned int > vc;
				char buffer[_INPUTLINESIZE_];
				char * buff;
				char * next_str;
				int line_number;
				unsigned int nv;
				unsigned int ne;
				unsigned int nf;
				MeshT::Scalar cdt[3];
				unsigned int idx[4];
				unsigned int eid;
				

				f >> nv;
				for (unsigned int i = 0; i < nv; i ++)
				{
					f >> cdt[0] >> cdt[1] >> cdt[2];

					MeshT::Point p(cdt[0], cdt[1], cdt[2]);	
					pc.push_back(p);
				}
				f >> ne;
				for (unsigned int i = 0; i < ne; i ++)
				{
					//buff = read_line_chars(buffer, f, line_number);
					//if ((buff != NULL) && (buff[0] != '#'))
					//{
					//	next_str = buff;
					//}
					f >> eid >> idx[0] >> idx[1] >> idx[2] >> idx[3];
					for (unsigned int k = 0; k < 4; k ++)
					{
						vc.push_back(idx[k] - 1);
					}
				}
				f.close();
				_mesh.set_points(pc);
				_mesh.set_mesh_type(1);


				return _mesh.build_topology(vc);
			}
			template < class MeshT >
			bool read_Netgen_mesh(MeshT & _mesh, const std::string & _file_name)
			{


				std::ifstream f(_file_name.c_str());
				if (!f.is_open())
				{
					std::cerr << "Error : File not found ! \n";
					return false;
				}

				std::vector<typename MeshT::Point> pc;
				std::vector< unsigned int > vc;
				char buffer[_INPUTLINESIZE_];
				char * buff;
				char * next_str;
				int line_number;
				unsigned int nv;
				unsigned int ne;
				unsigned int nf;
				MeshT::Scalar cdt[3];
				unsigned int idx[4];
				unsigned int eid;

				f >> nv;
				for (unsigned int i = 0; i < nv; i ++)
				{
					f >> cdt[0] >> cdt[1] >> cdt[2];

					MeshT::Point p(cdt[0], cdt[1], cdt[2]);	
					pc.push_back(p);
				}
				f >> ne;
				for (unsigned int i = 0; i < ne; i ++)
				{
					//buff = read_line_chars(buffer, f, line_number);
					//if ((buff != NULL) && (buff[0] != '#'))
					//{
					//	next_str = buff;
					//}
					f >> eid >> idx[0] >> idx[1] >> idx[2] >> idx[3];
					for (unsigned int k = 0; k < 4; k ++)
					{
						vc.push_back(idx[k] - 1);
					}
				}
				_mesh.set_points(pc);
				_mesh.set_mesh_type(1);
				if (!_mesh.build_topology(vc))
				{
					f.close();
					return false;
				}
				std::vector<unsigned int> v_idx;

				//--- read surface triangles ---//
				f >> nf;
				nf *= 4;
				v_idx.resize(nf);

				for (unsigned int i = 0; i < nf; ++ i)
				{
					f >> v_idx[i];				
				}

				f.close();

				//--- request the region property ---//

				if (!_mesh.has_hf_regions())
				{
					_mesh.request_hf_regions();
				}
				_mesh.build_region_property(v_idx);
				return true;
			}
			
		//-------------------------------------------------------------------------------------------------------------
		protected:
			template < class MeshT >
			bool write_MEdit_mesh(const MeshT & _mesh, const std::string & _filename)
			{
				if (_filename.find(".mesh") != _filename.size() - 5)
				{
					std::cerr << "Error : the format of the file is invalid \n";

					return false;
				}
				std::ofstream f(_filename.c_str());
				if (!f.is_open())
				{
					std::cerr << "Error : cannot open the file !\n";
					return false;
				}
				f << "MeshVersionFormatted 1" << std::endl;
				f << "Dimension" << std::endl;
				f << 3 << std::endl;

				//--- write the vertices sector
				f << "Vertices\n";
				f << _mesh.n_vertices() << std::endl;

				f.setf(std::ios_base::left, std::ios_base::adjustfield);

				typename MeshT::ConstVertexIter v_end(_mesh.vertices_end()); 
				typename MeshT::Point cdt;
				for (typename MeshT::ConstVertexIter v_it = _mesh.vertices_begin(); v_it != v_end; ++ v_it)
				{
					cdt = _mesh.point(v_it);
					f.width(20);
					f << cdt[0] << " ";
					f.width(20);
					f << cdt[1] << " ";
					f.width(20);
					f << cdt[2] << " ";
					f.width(20);
					f << 0;
					f << std::endl;
				}

				
				//--- triangles 
				if (_mesh.mesh_type() & 0x0001)
				{
					f << "Triangles" << std::endl;
				}
				//--- quadrilaterals
				else if (_mesh.mesh_type() & 0x0002)
				{
					f << "Quadrilaterals" << std::endl;
				}
				f << _mesh.n_half_faces() << std::endl;

				//--- tetrahedrons
				if (_mesh.mesh_type() & 0x0001)
				{
					f << "Tetrahedra" << std::endl;
				}
				//--- hexahedrons
				else if (_mesh.mesh_type() & 0x0002)
				{	
					f << "Hexahedra" << std::endl;
				}

				f << _mesh.n_hedrons() << std::endl;

				
				typename MeshT::ConstHedronIter h_end(_mesh.hedrons_end());
				for (typename MeshT::ConstHedronIter h_it = _mesh.hedrons_begin(); h_it != h_end; ++ h_it)
				{
					for (typename MeshT::ConstHedronVertexIter chv_it = _mesh.const_hedron_vertex_iter(h_it); chv_it; ++ chv_it)
					{
						f.width(15);
						f << chv_it.handle() + 1 << " ";
					}
					f << 0 << std::endl;
				}

				//--- end  of the file
				f << "End";
				f.close();
				
				return true;
			}
			template < class MeshT >
			bool write_mesh_opp_half_edges(const MeshT & _mesh, const std::string & _filename)
			{
				std::ofstream f(_filename.c_str());
				if (!f.is_open())
				{
					f << "Error : cannot open the file ! \n";
					return false;
				}

				f.setf(std::ios_base::left, std::ios_base::adjustfield);
				
				
				typename MeshT::ConstHalfEdgeIter he_end(_mesh.half_edges_end());
				typename MeshT::HalfEdgeHandle heh;
				for (typename MeshT::ConstHalfEdgeIter he_it = _mesh.half_edges_begin(); he_it != he_end; ++ he_it)
				{
					heh = _mesh.handle(*he_it);
					f.width(10);
					f << heh.idx() << " ";
					//typename MeshT::Point p = _mesh.point(_mesh.from_vertex_handle(heh));
					//for (unsigned int i = 0; i < 3; i ++)
					//{
					//	f.width(20);
					//	f << p[i] << " ";
					//}

					//f << " --- ";

					heh = _mesh.cw_opp_half_edge_handle(heh);
					f.width(10);
					f << heh.idx() << " ";

					heh = _mesh.handle(*he_it);
					heh = _mesh.ccw_opp_half_edge_handle(heh);
					f.width(10);
					f << heh.idx() << " ";

					//p = _mesh.point(_mesh.from_vertex_handle(heh));
					//for (unsigned int i = 0; i < 3; i ++)
					//{
					//	f.width(20);
					//	f << p[i] << " ";
					//}
					



					f << std::endl;
				}
				f.close();
				return true;
			}
			
			
		//-------------------------------------------------------------------------------------------------------------
		protected:
			/** read_line_chars()   Read a nonempty line from a file.                            
			*                                                                           
			* A line is considered "nonempty" if it contains something more than white  
			* spaces.  If a line is considered empty, it will be dropped and the next   
			* line will be read, this process ends until reaching the end-of-file or a  
			* non-empty line.  Return NULL if it is the end-of-file, otherwise, return  
			* a pointer to the first non-whitespace character of the line.              
			*/
			char * read_line_chars(char *  _result, std::ifstream & _infile, int & _linenumber);
			/** find_next_sub_str() find the next field of a numbering string
			*
			* Jumps past the current field by searching for whitespace or a comma, then
			* jumps past the whitespace or the comma to find the next field that looks 
			* like a number.                                                           
			*                                                                          
			*/
			char * find_next_sub_str(char * _str);
			/** trim_str() Trim a string, skip the space and tab char
			*/
			char * trim_str(char * _str);
		};

		extern _IOManager_ * _IO_Manager_Instance_;
		_IOManager_ & IOManager();

	}
//---------------------------------------------------------------------------------------------------------------------
} // namespace OVM
#endif