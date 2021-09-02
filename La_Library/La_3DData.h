/*
***************************************************************************************
*  程    序: 
*
*  作    者: LaDzy
*
*  邮    箱: mathbewithcode@gmail.com
*
*  编译环境: Visual Studio 2019
*
*  创建时间: 2021/08/29 17:49:22
*  最后修改: 
*
*  简    介: 
* PLG 格式 :
*			#开始是一行注释    一行是一个完整的信息
*			第一个信息    物体描述   ：     名字     顶点数    面数
*			紧随其后是顶点列表  一行一个顶点
*			接着面的列表 ：一行一个面    开头是面的描述符   然后是点数   再就是点的索引
* 
* 一般的，渲染列表是世界坐标的，因为是将物体转化为世界坐标，
* 然后传给渲染列表
*
***************************************************************************************
*/

#pragma once
#include "La_Math_Andre.h"
#include <tchar.h>
// DEFINES ////////////////////////////////////////////////////

// defines for enhanced PLG file format -> PLX
// the surface descriptor is still 16-bit now in the following format
// d15                      d0
//   CSSD | RRRR| GGGG | BBBB

// C is the RGB/indexed color flag
// SS are two bits that define the shading mode
// D is the double sided flag
// and RRRR, GGGG, BBBB are the red, green, blue bits for RGB mode
// or GGGGBBBB is the 8-bit color index for 8-bit mode

// bit masks to simplify testing????
#define PLX_RGB_MASK          0x8000   // mask to extract RGB or indexed color
#define PLX_SHADE_MODE_MASK   0x6000   // mask to extract shading mode
#define PLX_2SIDED_MASK       0x1000   // mask for double sided
#define PLX_COLOR_MASK        0x0fff   // xxxxrrrrggggbbbb, 4-bits per channel RGB
									   // xxxxxxxxiiiiiiii, indexed mode 8-bit index

// these are the comparision flags after masking
// color mode of polygon
#define PLX_COLOR_MODE_RGB_FLAG     0x8000   // this poly uses RGB color
#define PLX_COLOR_MODE_INDEXED_FLAG 0x0000   // this poly uses an indexed 8-bit color 

// double sided flag
#define PLX_2SIDED_FLAG              0x1000   // this poly is double sided
#define PLX_1SIDED_FLAG              0x0000   // this poly is single sided

// shading mode of polygon
#define PLX_SHADE_MODE_PURE_FLAG      0x0000  // this poly is a constant color
#define PLX_SHADE_MODE_CONSTANT_FLAG  0x0000  // alias
#define PLX_SHADE_MODE_FLAT_FLAG      0x2000  // this poly uses flat shading
#define PLX_SHADE_MODE_GOURAUD_FLAG   0x4000  // this poly used gouraud shading
#define PLX_SHADE_MODE_PHONG_FLAG     0x6000  // this poly uses phong shading
#define PLX_SHADE_MODE_FASTPHONG_FLAG 0x6000  // this poly uses phong shading (alias)


// defines for polygons and faces version 1

// attributes of polygons and polygon faces
#define POLY4DV1_ATTR_2SIDED              0x0001
#define POLY4DV1_ATTR_TRANSPARENT         0x0002
#define POLY4DV1_ATTR_8BITCOLOR           0x0004
#define POLY4DV1_ATTR_RGB16               0x0008
#define POLY4DV1_ATTR_RGB24               0x0010

#define POLY4DV1_ATTR_SHADE_MODE_PURE       0x0020
#define POLY4DV1_ATTR_SHADE_MODE_CONSTANT   0x0020 // (alias)
#define POLY4DV1_ATTR_SHADE_MODE_FLAT       0x0040
#define POLY4DV1_ATTR_SHADE_MODE_GOURAUD    0x0080
#define POLY4DV1_ATTR_SHADE_MODE_PHONG      0x0100
#define POLY4DV1_ATTR_SHADE_MODE_FASTPHONG  0x0100 // (alias)
#define POLY4DV1_ATTR_SHADE_MODE_TEXTURE    0x0200 


// states of polygons and faces
#define POLY4DV1_STATE_ACTIVE             0x0001
#define POLY4DV1_STATE_CLIPPED            0x0002
#define POLY4DV1_STATE_BACKFACE           0x0004

// defines for objects version 1
#define OBJECT4DV1_MAX_VERTICES           1024  // 64
#define OBJECT4DV1_MAX_POLYS              1024 // 128

// states for objects
#define OBJECT4DV1_STATE_ACTIVE           0x0001
#define OBJECT4DV1_STATE_VISIBLE          0x0002 
#define OBJECT4DV1_STATE_CULLED           0x0004

// attributes for objects

// render list defines
#define RENDERLIST4DV1_MAX_POLYS          32768// 16384

// transformation control flags
#define TRANSFORM_LOCAL_ONLY       0  // perform the transformation in place on the
									  // local/world vertex list 
#define TRANSFORM_TRANS_ONLY       1  // perfrom the transformation in place on the 
									  // "transformed" vertex list

#define TRANSFORM_LOCAL_TO_TRANS   2  // perform the transformation to the local
									  // vertex list, but store the results in the
									  // transformed vertex list

// general culling flags
#define CULL_OBJECT_X_PLANE           0x0001 // cull on the x clipping planes
#define CULL_OBJECT_Y_PLANE           0x0002 // cull on the y clipping planes
#define CULL_OBJECT_Z_PLANE           0x0004 // cull on the z clipping planes
#define CULL_OBJECT_XYZ_PLANES        (CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE)

// defines for camera rotation sequences
#define CAM_ROT_SEQ_XYZ  0
#define CAM_ROT_SEQ_YXZ  1
#define CAM_ROT_SEQ_XZY  2
#define CAM_ROT_SEQ_YZX  3
#define CAM_ROT_SEQ_ZYX  4
#define CAM_ROT_SEQ_ZXY  5

// defines for special types of camera projections
#define CAM_PROJ_NORMALIZED        0x0001
#define CAM_PROJ_SCREEN            0x0002
#define CAM_PROJ_FOV90             0x0004

#define CAM_MODEL_EULER            0x0008
#define CAM_MODEL_UVN              0x0010

#define UVN_MODE_SIMPLE            0 
#define UVN_MODE_SPHERICAL         1


// a polygon based on an external vertex list
 struct POLY4DV1
{
	int state;    
	int attr;    
	int color;    
	int lightColor;
	VECTOR4D* vlist; 
	int vert[3];       //索引
};

// a self contained polygon used for the render list
struct POLYF4DV1
{
	int state;    
	int attr;    
	int color;   

	VECTOR4D vlist[3];  // 模型定点
	VECTOR4D tvlist[3]; // 变换后的顶点

	POLYF4DV1* next; 
	POLYF4DV1* prev; 
};

// an object based on a vertex list and list of polygons
struct OBJECT4DV1
{
	int  id;           
	char name[64];     
	int  state;        
	int  attr;         
	float avg_radius;  // average radius of object used for collision detection
	float max_radius;  // maximum radius of object

	VECTOR4D world_pos;  

	VECTOR4D dir;       //单位方向向量 或者 旋转角

	VECTOR4D ux, uy, uz;  //朝向坐标，随着物体旋转而旋转

	int num_vertices;   

	VECTOR4D vlist_local[OBJECT4DV1_MAX_VERTICES];  //模型
	VECTOR4D vlist_trans[OBJECT4DV1_MAX_VERTICES];  //变换后的

	int num_polys;        
	POLY4DV1 plist[OBJECT4DV1_MAX_POLYS];  // array of polygons
};

// camera version 1
struct CAM4DV1
{
	int state;      
	int attr;       

	VECTOR4D pos;    

	VECTOR4D dir;   // angles or look at direction of camera for simple 
					// euler camera models, elevation and heading for
					// uvn model

	VECTOR4D u;    //朝向向量(欧拉相机和 uvn 都是 )
	VECTOR4D v;    //  v n 是给 uvn 的
	VECTOR4D n;

	VECTOR4D target; // look at target  注视点，比如相机在运动，但是盯着的地方不动

	float view_dist;  // focal length 

	float fov;          // field of view for both horizontal and vertical axes

	float near_clip_z;     // near z=constant clipping plane
	float far_clip_z;      // far z=constant clipping plane

	PLANE3D rt_clip_plane;  // the right clipping plane
	PLANE3D lt_clip_plane;  // the left clipping plane
	PLANE3D tp_clip_plane;  // the top clipping plane
	PLANE3D bt_clip_plane;  // the bottom clipping plane                        

	float viewplane_width;     //对归一化的 为 2 * (2 / ar) ，否则和窗口大小相同
	float viewplane_height;    

	// remember screen and viewport are synonymous
	float viewport_width;     // size of screen/viewport
	float viewport_height;
	float viewport_center_x;  // center of view port (final image destination)
	float viewport_center_y;

	float aspect_ratio;   //宽高比      

	MATRIX4X4 mcam;   // storage for the world to camera transform matrix
	MATRIX4X4 mper;   // storage for the camera to perspective transform matrix
	MATRIX4X4 mscr;   // storage for the perspective to screen transform matrix
	void set(
		int cam_attr,
		VECTOR4D& cam_pos,
		VECTOR4D& cam_dir,		// initial camera angles
		VECTOR4D* cam_target, // UVN target
		float near_clip_z,     // near and far clipping planes
		float far_clip_z,
		float fov,             // field of view in degrees
		float viewport_width,  // size of final screen viewport
		float viewport_height);
};

// object to hold the render list, this way we can have more
// than one render list at a time
struct RENDERLIST4DV1
{
	int state; 
	int attr;  

	POLYF4DV1* poly_ptrs[RENDERLIST4DV1_MAX_POLYS];   //索引

	POLYF4DV1 poly_data[RENDERLIST4DV1_MAX_POLYS];    //数据

	int num_polys; 
};





float ComputeRadius(OBJECT4DV1& obj);
bool LoadPLG(OBJECT4DV1& obj, const TCHAR* filename, const VECTOR4D& pos = VECTOR4D(0, 0, 0), const VECTOR4D& scale = VECTOR4D(1, 1, 1), const VECTOR4D& rot = VECTOR4D(0, 0, 0));
inline void Reset(RENDERLIST4DV1& rend_list) { rend_list.num_polys = 0; }
inline void Translate(OBJECT4DV1& obj, const VECTOR4D& vt){Add(obj.world_pos, vt, obj.world_pos);}
void Transform(RENDERLIST4DV1& rend_list, const MATRIX4X4& mt, int coord_select);
void Transform(OBJECT4DV1& obj, const MATRIX4X4& mt, int coord_select, bool transform_basis = true);
void ModelToWorld(OBJECT4DV1& obj, int coord_select = TRANSFORM_LOCAL_TO_TRANS);
void ModelToWorld(RENDERLIST4DV1& rend_list, VECTOR4D& world_pos, int coord_select = TRANSFORM_LOCAL_TO_TRANS);
inline void BuildModelToWorld(const VECTOR4D& vpos, MATRIX4X4& m)
{
	m.set(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		vpos.x, vpos.y, vpos.z, 1);
}
void BuildEuler(CAM4DV1& cam, int cam_rot_seq);
void BuildUVN(CAM4DV1& cam, int mode);
void WorldToCamera(OBJECT4DV1& obj, CAM4DV1& cam);
void WorldToCamera(RENDERLIST4DV1& rend_list, CAM4DV1& cam);
bool Cull(OBJECT4DV1& obj, const CAM4DV1& cam, int cull_flags);
void Reset(OBJECT4DV1& obj);
void RemoveBackfaces(OBJECT4DV1& obj, const CAM4DV1& cam);
void RemoveBackfaces(RENDERLIST4DV1& rend_list, CAM4DV1& cam);
void CameraToPerspective(OBJECT4DV1& obj, const CAM4DV1& cam);
inline void BuildCameraToPerspective(CAM4DV1& cam, MATRIX4X4& m)
{
	m.set(cam.view_dist, 0, 0, 0,
		0, cam.view_dist * cam.aspect_ratio, 0, 0,
		0, 0, 1, 1,
		0, 0, 0, 0);
}
void ConvertFromHomogeneous4D(OBJECT4DV1& obj);
void CameraToPerspective(RENDERLIST4DV1& rend_list, const CAM4DV1& cam);
void ConvertFromHomogeneous4D(RENDERLIST4DV1& rend_list);
void PerspectiveToScreen(OBJECT4DV1& obj, CAM4DV1& cam);
//需要齐次化
inline void BuildPerspectiveToScreen4D(CAM4DV1& cam, MATRIX4X4& m)
{
	// this function builds up a perspective to screen transformation
	// matrix, the function assumes that you want to perform the
	// transform in homogeneous coordinates and at raster time there will be 
	// a 4D.3D homogenous conversion and of course only the x,y points
	// will be considered for the 2D rendering, thus you would use this
	// function's matrix is your perspective coordinates were still 
	// in homgeneous form whene this matrix was applied, additionally
	// the point of this matrix to to scale and translate the perspective
	// coordinates to screen coordinates, thus the matrix is built up
	// assuming that the perspective coordinates are in normalized form for
	// a (2x2)/aspect_ratio viewplane, that is, x: -1 to 1, y:-1/aspect_ratio to 1/aspect_ratio

	float alpha = (0.5 * cam.viewport_width - 0.5);
	float beta = (0.5 * cam.viewport_height - 0.5);

	m.set(alpha, 0, 0, 0,
		0, -beta, 0, 0,
		alpha, beta, 1, 0,
		0, 0, 0, 1);
}
//无需齐次化，实际上是2D
inline void BuildPerspectiveToScreen(CAM4DV1& cam, MATRIX4X4& m)
{
	float alpha = (0.5 * cam.viewport_width - 0.5);
	float beta = (0.5 * cam.viewport_height - 0.5);

	m.set(alpha, 0, 0, 0,
		0, -beta, 0, 0,
		alpha, beta, 1, 0,
		0, 0, 0, 1);

}
void PerspectiveToScreen(RENDERLIST4DV1& rend_list, const CAM4DV1& cam);
void CameraToPerspectiveScreen(OBJECT4DV1& obj, const CAM4DV1& cam);
void CameraToPerspectiveScreen(RENDERLIST4DV1& rend_list, const CAM4DV1& cam);
inline void BuildCameraToScreen(CAM4DV1& cam, MATRIX4X4& m)
{
	float alpha = (0.5 * cam.viewport_width - 0.5);
	float beta = (0.5 * cam.viewport_height - 0.5);

	m.set(cam.view_dist, 0, 0, 0,
		0, -cam.view_dist, 0, 0,
		alpha, beta, 1, 1,
		0, 0, 0, 0);

}
void DrawWire(OBJECT4DV1& obj);
void DrawWire(RENDERLIST4DV1& rend_list);
void BuildXYZRotation(const float theta_x, const float theta_y, const float theta_z, MATRIX4X4& mrot);
void RotateXYZ(OBJECT4DV1& obj, const float theta_x, const float theta_y, const float theta_z);
bool Insert(RENDERLIST4DV1& rend_list, const POLY4DV1& poly);
bool Insert(RENDERLIST4DV1& rend_list, const POLYF4DV1& poly);
bool Insert(RENDERLIST4DV1& rend_list, OBJECT4DV1& obj, const int insert_local = 0);



