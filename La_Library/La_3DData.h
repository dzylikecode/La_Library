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
***************************************************************************************
*/

#pragma once
#include "La_Math_Andre.h"
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

// TYPES //////////////////////////////////////////////////////

// a polygon based on an external vertex list
 struct POLY4DV1
{
	int state;    // state information
	int attr;     // physical attributes of polygon
	int color;    // color of polygon

	VECTOR4D* vlist; // the vertex list itself
	int vert[3];       // the indices into the vertex list

};

// a self contained polygon used for the render list
struct POLYF4DV1
{
	int state;    // state information
	int attr;     // physical attributes of polygon
	int color;    // color of polygon

	VECTOR4D vlist[3];  // the vertices of this triangle
	VECTOR4D tvlist[3]; // the vertices after transformation if needed

	POLYF4DV1* next; // pointer to next polygon in list??
	POLYF4DV1* prev; // pointer to previous polygon in list??

};

// an object based on a vertex list and list of polygons
struct OBJECT4DV1
{
	int  id;           // numeric id of this object
	char name[64];     // ASCII name of object just for kicks
	int  state;        // state of object
	int  attr;         // attributes of object
	float avg_radius;  // average radius of object used for collision detection
	float max_radius;  // maximum radius of object

	VECTOR4D world_pos;  // position of object in world

	VECTOR4D dir;       // rotation angles of object in local
						// cords or unit direction vector user defined???

	VECTOR4D ux, uy, uz;  // local axes to track full orientation
						// this is updated automatically during
						// rotation calls

	int num_vertices;   // number of vertices of this object

	VECTOR4D vlist_local[OBJECT4DV1_MAX_VERTICES]; // array of local vertices
	VECTOR4D vlist_trans[OBJECT4DV1_MAX_VERTICES]; // array of transformed vertices

	int num_polys;        // number of polygons in object mesh
	POLY4DV1 plist[OBJECT4DV1_MAX_POLYS];  // array of polygons

};

// camera version 1
struct CAM4DV1
{
	int state;      // state of camera
	int attr;       // camera attributes

	VECTOR4D pos;    // world position of camera used by both camera models

	VECTOR4D dir;   // angles or look at direction of camera for simple 
					// euler camera models, elevation and heading for
					// uvn model

	VECTOR4D u;     // extra vectors to track the camera orientation
	VECTOR4D v;     // for more complex UVN camera model
	VECTOR4D n;

	VECTOR4D target; // look at target

	float view_dist;  // focal length 

	float fov;          // field of view for both horizontal and vertical axes

	// 3d clipping planes
	// if view volume is NOT 90 degree then general 3d clipping
	// must be employed
	float near_clip_z;     // near z=constant clipping plane
	float far_clip_z;      // far z=constant clipping plane

	PLANE3D rt_clip_plane;  // the right clipping plane
	PLANE3D lt_clip_plane;  // the left clipping plane
	PLANE3D tp_clip_plane;  // the top clipping plane
	PLANE3D bt_clip_plane;  // the bottom clipping plane                        

	float viewplane_width;     // width and height of view plane to project onto
	float viewplane_height;    // usually 2x2 for normalized projection or 
							   // the exact same size as the viewport or screen window

	// remember screen and viewport are synonomous 
	float viewport_width;     // size of screen/viewport
	float viewport_height;
	float viewport_center_x;  // center of view port (final image destination)
	float viewport_center_y;

	// aspect ratio
	float aspect_ratio;

	// these matrices are not necessarily needed based on the method of
	// transformation, for example, a manual perspective or screen transform
	// and or a concatenated perspective/screen, however, having these 
	// matrices give us more flexibility         

	MATRIX4X4 mcam;   // storage for the world to camera transform matrix
	MATRIX4X4 mper;   // storage for the camera to perspective transform matrix
	MATRIX4X4 mscr;   // storage for the perspective to screen transform matrix
	void set(
		int cam_attr,          // attributes
		VECTOR4D& cam_pos,   // initial camera position
		VECTOR4D& cam_dir,  // initial camera angles
		VECTOR4D* cam_target, // UVN target
		float near_clip_z,     // near and far clipping planes
		float far_clip_z,
		float fov,             // field of view in degrees
		float viewport_width,  // size of final screen viewport
		float viewport_height)
	{
		this->attr = cam_attr;             
		this->pos = cam_pos; 
		this->dir = cam_dir; // direction vector or angles for euler camera
		// for UVN camera
		this->u.set(1, 0, 0);  // set to +x
		this->v.set(0, 1, 0);  // set to +y
		this->n.set(0, 0, 1);  // set to +z        

		if (cam_target != NULL)this->target = *cam_target; // UVN target
		else Zero(this->target);

		this->near_clip_z = near_clip_z;     // near z=constant clipping plane
		this->far_clip_z = far_clip_z;      // far z=constant clipping plane

		this->viewport_width = viewport_width;   // dimensions of viewport
		this->viewport_height = viewport_height;

		this->viewport_center_x = (viewport_width - 1) / 2; // center of viewport
		this->viewport_center_y = (viewport_height - 1) / 2;

		this->aspect_ratio = (float)viewport_width / (float)viewport_height;

		// set all camera matrices to identity matrix
		Identity(this->mcam);
		Identity(this->mper);
		Identity(this->mscr);

		// set independent vars
		this->fov = fov;

		// set the viewplane dimensions up, they will be 2 x (2/ar)
		this->viewplane_width = 2.0;
		this->viewplane_height = 2.0 / this->aspect_ratio;

		// now we know fov and we know the viewplane dimensions plug into formula and
		// solve for view distance parameters
		float tan_fov_div2 = tan(DEG_TO_RAD(fov / 2));

		this->view_dist = (0.5) * (this->viewplane_width) * tan_fov_div2;

		// test for 90 fov first since it's easy :)
		if (fov == 90.0)
		{
			// set up the clipping planes -- easy for 90 degrees!
			VECTOR3D pt_origin(0, 0, 0); // point on the plane

			VECTOR3D vn; // normal to plane   法线

			// right clipping plane 
			vn.set(1, 0, -1); // x=z plane
			this->rt_clip_plane.set(pt_origin, vn, true);

			// left clipping plane
			vn.set(-1, 0, -1); // -x=z plane
			this->lt_clip_plane.set(pt_origin, vn, true);

			// top clipping plane
			vn.set(0, 1, -1); // y=z plane
			this->tp_clip_plane.set(pt_origin, vn, true);

			// bottom clipping plane
			vn.set(0, -1, -1); // -y=z plane
			this->bt_clip_plane.set(pt_origin, vn, true);
		}
		else
		{
			// now compute clipping planes yuck!
			VECTOR3D pt_origin(0, 0, 0); // point on the plane

			VECTOR3D vn; // normal to plane

			// right clipping plane, check the math on graph paper 
			vn.set(this->view_dist, 0, -this->viewplane_width / 2.0);
			this->rt_clip_plane.set(pt_origin, vn, true);

			// left clipping plane, we can simply reflect the right normal about
			// the z axis since the planes are symetric about the z axis
			// thus invert x only
			vn.set(-this->view_dist, 0, -this->viewplane_width / 2.0);
			this->lt_clip_plane.set(pt_origin, vn, true);

			// top clipping plane, same construction
			vn.set(0, this->view_dist, -this->viewplane_width / 2.0);
			this->tp_clip_plane.set(pt_origin, vn, true);

			// bottom clipping plane, same inversion
			vn.set(0, -this->view_dist, -this->viewplane_width / 2.0);
			this->bt_clip_plane.set(pt_origin, vn, true);
		}
	}
};

// object to hold the render list, this way we can have more
// than one render list at a time
struct RENDERLIST4DV1
{
	int state; // state of renderlist ???
	int attr;  // attributes of renderlist ???

	// the render list is an array of pointers each pointing to 
	// a self contained "renderable" polygon face POLYF4DV1
	POLYF4DV1* poly_ptrs[RENDERLIST4DV1_MAX_POLYS];

	// additionally to cut down on allocatation, de-allocation
	// of polygons each frame, here's where the actual polygon
	// faces will be stored
	POLYF4DV1 poly_data[RENDERLIST4DV1_MAX_POLYS];

	int num_polys; // number of polys in render list
};





float ComputeRadius(OBJECT4DV1& obj);
bool LoadPLG(OBJECT4DV1& obj, const char* filename, const VECTOR4D& pos = VECTOR4D(0, 0, 0), const VECTOR4D& scale = VECTOR4D(1, 1, 1), const VECTOR4D& rot = VECTOR4D(0, 0, 0));