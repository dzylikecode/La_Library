#pragma once
#include "La_3DData.h"
#include "La_3DLight.h"

// defines for texture mapper triangular analysis
#define TRI_TYPE_NONE           0
#define TRI_TYPE_FLAT_TOP       1 
#define TRI_TYPE_FLAT_BOTTOM	2
#define TRI_TYPE_FLAT_MASK      3
#define TRI_TYPE_GENERAL        4
#define INTERP_LHS              0
#define INTERP_RHS              1
#define MAX_VERTICES_PER_POLY   6


// defines for polygons and faces version 2

// attributes of polygons and polygon faces
#define POLY4DV2_ATTR_2SIDED                0x0001
#define POLY4DV2_ATTR_TRANSPARENT           0x0002
#define POLY4DV2_ATTR_8BITCOLOR             0x0004
#define POLY4DV2_ATTR_RGB16                 0x0008
#define POLY4DV2_ATTR_RGB24                 0x0010

#define POLY4DV2_ATTR_SHADE_MODE_PURE       0x0020
#define POLY4DV2_ATTR_SHADE_MODE_CONSTANT   0x0020 // (alias)
#define POLY4DV2_ATTR_SHADE_MODE_EMISSIVE   0x0020 // (alias)

#define POLY4DV2_ATTR_SHADE_MODE_FLAT       0x0040
#define POLY4DV2_ATTR_SHADE_MODE_GOURAUD    0x0080
#define POLY4DV2_ATTR_SHADE_MODE_PHONG      0x0100
#define POLY4DV2_ATTR_SHADE_MODE_FASTPHONG  0x0100 // (alias)
#define POLY4DV2_ATTR_SHADE_MODE_TEXTURE    0x0200 

// new
#define POLY4DV2_ATTR_ENABLE_MATERIAL       0x0800 // use a real material for lighting
#define POLY4DV2_ATTR_DISABLE_MATERIAL      0x1000 // use basic color only for lighting (emulate version 1.0)

// states of polygons and faces
#define POLY4DV2_STATE_NULL               0x0000
#define POLY4DV2_STATE_ACTIVE             0x0001  
#define POLY4DV2_STATE_CLIPPED            0x0002  
#define POLY4DV2_STATE_BACKFACE           0x0004  
#define POLY4DV2_STATE_LIT                0x0008

// (new) used for simple model formats to override/control the lighting
#define VERTEX_FLAGS_OVERRIDE_MASK          0xf000 // this masks these bits to extract them
#define VERTEX_FLAGS_OVERRIDE_CONSTANT      0x1000
#define VERTEX_FLAGS_OVERRIDE_EMISSIVE      0x1000 //(alias)
#define VERTEX_FLAGS_OVERRIDE_PURE          0x1000
#define VERTEX_FLAGS_OVERRIDE_FLAT          0x2000
#define VERTEX_FLAGS_OVERRIDE_GOURAUD       0x4000
#define VERTEX_FLAGS_OVERRIDE_TEXTURE       0x8000

#define VERTEX_FLAGS_INVERT_TEXTURE_U       0x0080   // invert u texture coordinate 
#define VERTEX_FLAGS_INVERT_TEXTURE_V       0x0100   // invert v texture coordinate
#define VERTEX_FLAGS_INVERT_SWAP_UV         0x0800   // swap u and v texture coordinates



// defines for objects version 2
// objects use dynamic allocation now, but keep as max values
#define OBJECT4DV2_MAX_VERTICES           4096  // 64
#define OBJECT4DV2_MAX_POLYS              8192 // 128

// states for objects
#define OBJECT4DV2_STATE_NULL             0x0000
#define OBJECT4DV2_STATE_ACTIVE           0x0001
#define OBJECT4DV2_STATE_VISIBLE          0x0002 
#define OBJECT4DV2_STATE_CULLED           0x0004

// new
#define OBJECT4DV2_ATTR_SINGLE_FRAME      0x0001 // single frame object (emulates ver 1.0)
#define OBJECT4DV2_ATTR_MULTI_FRAME       0x0002 // multi frame object for .md2 support etc.
#define OBJECT4DV2_ATTR_TEXTURES          0x0004 // flags if object contains textured polys?


// render list defines ver 2.0
#define RENDERLIST4DV2_MAX_POLYS          32768

// defines for vertices, these are "hints" to the transform and
// lighting systems to help determine if a particular vertex has
// a valid normal that must be rotated, or a texture coordinate
// that must be clipped etc., this helps us minmize load during lighting
// and rendering since we can determine exactly what kind of vertex we
// are dealing with, something like a (direct3d) flexible vertex format in 
// as much as it can hold:
// point
// point + normal
// point + normal + texture coordinates
#define VERTEX4DTV1_ATTR_NULL             0x0000 // this vertex is empty
#define VERTEX4DTV1_ATTR_POINT            0x0001
#define VERTEX4DTV1_ATTR_NORMAL           0x0002
#define VERTEX4DTV1_ATTR_TEXTURE          0x0004

// these are some defines for conditional compilation of the new rasterizers
// I don't want 80 million different functions, so I have decided to 
// use some conditionals to change some of the logic in each
// these names aren't necessarily the most accurate, but 3 should be enough
#define RASTERIZER_ACCURATE    0 // sub-pixel accurate with fill convention
#define RASTERIZER_FAST        1 // 
#define RASTERIZER_FASTEST     2

// set this to the mode you want the engine to use
#define RASTERIZER_MODE        RASTERIZER_ACCURATE

// TYPES ///////////////////////////////////////////////////////////////////

// integer 2D vector, point without the w ////////////////////////
struct VECTOR2DI
{
	union
	{
		int M[2]; 
		struct{int x, y;}; 
	}; 
};

// integer 3D vector, point without the w ////////////////////////
struct VECTOR3DI
{
	union
	{
		int M[3]; 
		struct{int x, y, z;}; 
	}; 

};

// integer 4D homogenous vector, point with w ////////////////////
struct VECTOR4DI
{
	union
	{
		int M[4]; 
		struct{int x, y, z, w;}; 
	}; 

};


// 4D homogeneous vertex with 2 texture coordinates, and vertex normal ////////////////
// normal can be interpreted as vector or point
struct VERTEX4DTV1
{
	union
	{
		float M[12];          
		struct
		{
			float x, y, z, w;     // point
			float nx, ny, nz, nw; // normal (vector or point)
			float u0, v0;       // texture coordinates 

			float i;           // final vertex intensity after lighting
			int   attr;        // attributes/ extra texture coordinates
		};                
		struct
		{
			VECTOR4D  v;       // the vertex
			VECTOR4D  n;       // the normal
			VECTOR4D  t;       // texture coordinates
		};

	}; 
public:
	VERTEX4DTV1() {};
	VERTEX4DTV1(const VERTEX4DTV1& vertv1) :v(vertv1.v), n(vertv1.n), t(vertv1.t) {};
	VERTEX4DTV1 operator=(const VERTEX4DTV1& vertv1)
	{
		if (this != &vertv1)
		{
			for (int i = 0; i < 12; i++)
				M[i] = vertv1.M[i];
		}
		return *this;
	}
};

// a self contained polygon used for the render list version 2 /////////////////////////
struct POLYF4DV2
{
	int      state;           // state information
	int      attr;            // physical attributes of polygon
	int      color;           // color of polygon
	int      lit_color[3];    // holds colors after lighting, 0 for flat shading
							  // 0,1,2 for vertex colors after vertex lighting
	IMAGE texture; // pointer to the texture information for simple texture mapping

	int      mati;    // material index (-1) for no material  (new)

	float    nlength; // length of the polygon normal if not normalized (new)
	VECTOR4D normal;  // the general polygon normal (new)

	float    avg_z;   // average z of vertices, used for simple sorting (new)

	VERTEX4DTV1 vlist[3];  // the vertices of this triangle 
	VERTEX4DTV1 tvlist[3]; // the vertices after transformation if needed 

	POLYF4DV2* next;   // pointer to next polygon in list??
	POLYF4DV2* prev;   // pointer to previous polygon in list??

};

// a polygon ver 2.0 based on an external vertex list  //////////////////////////////////
struct POLY4DV2
{
	int state;           // state information
	int attr;            // physical attributes of polygon
	int color;           // color of polygon
	int lit_color[3];    // holds colors after lighting, 0 for flat shading
						 // 0,1,2 for vertex colors after vertex lighting

	IMAGE texture; // pointer to the texture information for simple texture mapping

	int mati;              // material index (-1) no material (new)

	VERTEX4DTV1* vlist; // the vertex list itself 
	VECTOR2D*     tlist; // the texture list itself (new)
	int vert[3];           // the indices into the vertex list
	int text[3];           // the indices into the texture coordinate list (new)
	float nlength;         // length of normal (new)

};

// an object ver 2.0 based on a vertex list and list of polygons //////////////////////////
// this new object has a lot more flexibility and it supports "framed" animation
// that is this object can hold hundreds of frames of an animated mesh as long as
// the mesh has the same polygons and geometry, but with changing vertex positions
// similar to the Quake II .md2 format
struct OBJECT4DV2   //这其实相当于一个 sprite
{
	int   id;           // numeric id of this object
	char  name[64];     // ASCII name of object just for kicks
	int   state;        // state of object
	int   attr;         // attributes of object
	int   mati;         // material index overide (-1) - no material (new)
	float* avg_radius;  // [OBJECT4DV2_MAX_FRAMES];   // average radius of object used for collision detection
	float* max_radius;  // [OBJECT4DV2_MAX_FRAMES];   // maximum radius of object

	VECTOR4D world_pos;  // position of object in world

	VECTOR4D dir;       // rotation angles of object in local
						// cords or unit direction vector user defined???

	VECTOR4D ux, uy, uz;  // local axes to track full orientation
						// this is updated automatically during
						// rotation calls

	int num_vertices;   // number of vertices per frame of this object
	int num_frames;     // number of frames
	int total_vertices; // total vertices, redudant, but it saves a multiply in a lot of places
	int curr_frame;     // current animation frame (0) if single frame

	VERTEX4DTV1* vlist_local; // [OBJECT4DV1_MAX_VERTICES]; // array of local vertices
	VERTEX4DTV1* vlist_trans; // [OBJECT4DV1_MAX_VERTICES]; // array of transformed vertices

	// these are needed to track the "head" of the vertex list for mult-frame objects
	VERTEX4DTV1* head_vlist_local;
	VERTEX4DTV1* head_vlist_trans;

	// texture coordinates list (new)
	VECTOR2D* tlist;       // 3*num polys at max

	IMAGE texture; // pointer to the texture information for simple texture mapping (new)

	int num_polys;           // number of polygons in object mesh
	POLY4DV2* plist;      // ptr to polygons (new)

	int   ivar1, ivar2;      // auxiliary vars
	float fvar1, fvar2;      // auxiliary vars

	// METHODS //////////////////////////////////////////////////

	// setting the frame is so important that it should be a member function
	// calling functions without doing this can wreak havok!
	int setFrame(int frame);
public:
	int set(
		int _num_vertices,
		int _num_polys,
		int _num_frames,
		bool destroy = false)
	{
		// this function does nothing more than allocate the memory for an OBJECT4DV2
		// based on the sent data, later we may want to create more robust initializers
		// but the problem is that we don't want to tie the initializer to anthing yet
		// in 99% of cases this all will be done by the call to load object
		// we just might need this function if we manually want to build an object???


		// first destroy the object if it exists
		if (destroy)
			close();

		// allocate memory for vertex lists
		if (!(vlist_local = (VERTEX4DTV1*)malloc(sizeof(VERTEX4DTV1) * _num_vertices * _num_frames)))
			return(0);

		// clear data
		memset((void*)vlist_local, 0, sizeof(VERTEX4DTV1) * _num_vertices * _num_frames);

		if (!(vlist_trans = (VERTEX4DTV1*)malloc(sizeof(VERTEX4DTV1) * _num_vertices * _num_frames)))
			return(0);

		// clear data
		memset((void*)vlist_trans, 0, sizeof(VERTEX4DTV1) * _num_vertices * _num_frames);

		// number of texture coordinates always 3*number of polys
		if (!(tlist = (VECTOR2D*)malloc(sizeof(VECTOR2D) * _num_polys * 3)))
			return(0);

		// clear data
		memset((void*)tlist, 0, sizeof(VECTOR2D) * _num_polys * 3);


		// allocate memory for radii arrays
		if (!(avg_radius = (float*)malloc(sizeof(float) * _num_frames)))
			return(0);

		// clear data
		memset((void*)avg_radius, 0, sizeof(float) * _num_frames);


		if (!(max_radius = (float*)malloc(sizeof(float) * _num_frames)))
			return(0);

		// clear data
		memset((void*)max_radius, 0, sizeof(float) * _num_frames);

		// allocate memory for polygon list
		if (!(plist = (POLY4DV2*)malloc(sizeof(POLY4DV2) * _num_polys)))
			return(0);

		// clear data
		memset((void*)plist, 0, sizeof(POLY4DV2) * _num_polys);

		// alias head pointers
		head_vlist_local = vlist_local;
		head_vlist_trans = vlist_trans;

		// set some internal variables
		num_frames = _num_frames;
		num_polys = _num_polys;
		num_vertices = _num_vertices;
		total_vertices = _num_vertices * _num_frames;

		return(1);
	} 
	void close()
	{
		// this function destroys the sent object, basically frees the memory
		// if any that has been allocated

		// local vertex list
		if (head_vlist_local)
			free(head_vlist_local);

		// transformed vertex list
		if (head_vlist_trans)
			free(head_vlist_trans);

		// texture coordinate list
		if (tlist)
			free(tlist);

		// polygon list
		if (plist)
			free(plist);

		// object radii arrays
		if (avg_radius)
			free(avg_radius);

		if (max_radius)
			free(max_radius);

		// now clear out object completely
		memset((void*)this, 0, sizeof(OBJECT4DV2));
	}
	~OBJECT4DV2() { close(); }
};

// object to hold the render list version 2.0, this way we can have more
// than one render list at a time
struct RENDERLIST4DV2
{
	int state; // state of renderlist ???
	int attr;  // attributes of renderlist ???

	// the render list is an array of pointers each pointing to 
	// a self contained "renderable" polygon face POLYF4DV2
	POLYF4DV2* poly_ptrs[RENDERLIST4DV2_MAX_POLYS];

	// additionally to cut down on allocatation, de-allocation
	// of polygons each frame, here's where the actual polygon
	// faces will be stored
	POLYF4DV2 poly_data[RENDERLIST4DV2_MAX_POLYS];

	int num_polys; // number of polys in render list

};

// floating point comparison
#define FCMP(a,b) ( (fabs(a-b) < EPSILON_E3) ? 1 : 0)

inline void Copy(VERTEX4DTV1& vdst, VERTEX4DTV1& vsrc){vdst = vsrc;}

inline void Init(VERTEX4DTV1& vdst, VERTEX4DTV1& vsrc){vdst = vsrc;}

inline float LengthFast2(const VECTOR4D& va)
{
	// this function computes the distance from the origin to x,y,z

	int temp;  // used for swaping
	int x, y, z; // used for algorithm

	// make sure values are all positive
	x = fabs(va.x) * 1024;
	y = fabs(va.y) * 1024;
	z = fabs(va.z) * 1024;

	// sort values
	if (y < x) SWAP(x, y, temp);
	if (z < y) SWAP(y, z, temp);
	if (y < x) SWAP(x, y, temp);

	int dist = (z + 11 * (y >> 5) + (x >> 2));

	// compute distance with 8% error
	return((float)(dist >> 10));

} 

// EXTERNALS ///////////////////////////////////////////////////////////////

extern UCHAR logbase2ofx[513];

extern char texture_path[80]; // root path to ALL textures, make current directory for now




inline void Translate(OBJECT4DV2& obj, VECTOR4D& vt){Add(obj.world_pos, vt, obj.world_pos);}
void Scale(OBJECT4DV2& obj, VECTOR4D& vs, int all_frames);
void Transform(OBJECT4DV2& obj,  // object to transform
	MATRIX4X4& mt,    // transformation matrix
	int coord_select,    // selects coords to transform
	int transform_basis, // flags if vector orientation
						 // should be transformed too
	int all_frames = 0);

void RotateXYZ(OBJECT4DV2& obj, float theta_x, float theta_y, float theta_z, int all_frames);
void ModelToWorld(OBJECT4DV2& obj, int coord_select = TRANSFORM_LOCAL_TO_TRANS, int all_frames = 0);
int Cull(OBJECT4DV2& obj, CAM4DV1& cam, int cull_flags);
void RemoveBackfaces(RENDERLIST4DV2& rend_list, CAM4DV1& cam);
void RemoveBackfaces(OBJECT4DV2& obj, CAM4DV1& cam);
void WorldToCamera(OBJECT4DV2& obj, CAM4DV1& cam);
void CameraToPerspective(RENDERLIST4DV2& rend_list, CAM4DV1& cam);
void CameraToPerspectiveScreen(RENDERLIST4DV2& rend_list, CAM4DV1& cam);
void PerspectiveToScreen(RENDERLIST4DV2& rend_list, CAM4DV1& cam);
void WorldToCamera(RENDERLIST4DV2& rend_list, CAM4DV1& cam);
void CameraToPerspective(OBJECT4DV2& obj, CAM4DV1& cam);
void CameraToPerspectiveScreen(OBJECT4DV2& obj, CAM4DV1& cam);
void PerspectiveToScreen(OBJECT4DV2& obj, CAM4DV1& cam);
void ConvertFromHomogeneous4D(OBJECT4DV2& obj);
int Insert(RENDERLIST4DV2& rend_list, POLY4DV2& poly);
int Insert(RENDERLIST4DV2& rend_list, POLYF4DV2& poly);
int Insert(RENDERLIST4DV2& rend_list, OBJECT4DV2& obj, int insert_local = 0);
void Reset(OBJECT4DV2& obj);
void DrawWire(OBJECT4DV2& obj);
void Sort(RENDERLIST4DV2& rend_list, int sort_method = SORT_POLYLIST_AVGZ);
int LightWorld(OBJECT4DV2& obj, CAM4DV1& cam, LIGHTV1* lights, int max_lights);
int LightWorld(RENDERLIST4DV2& rend_list, CAM4DV1& cam, LIGHTV1* lights, int max_lights);
float ComputeRadius(OBJECT4DV2& obj);
int ComputeVertexNormals(OBJECT4DV2& obj);
int ComputePolyNormals(OBJECT4DV2& obj);
int LoadCOB(OBJECT4DV2& obj, const char* filename, VECTOR4D& scale, VECTOR4D& pos, VECTOR4D& rot, int vertex_flags, MATV1* materials, int& num_materials);
int Load3DSASC(OBJECT4DV2& obj, const char* filename, VECTOR4D& scale, VECTOR4D& pos, VECTOR4D& rot, int vertex_flags);
int LoadPLG(OBJECT4DV2& obj, const char* filename, VECTOR4D& scale, VECTOR4D& pos, VECTOR4D& rot, int vertex_flags);
int LoadPLG(OBJECT4DV2& obj, const char* filename, VECTOR4D& scale, VECTOR4D& pos, VECTOR4D& rot, int vertex_flags);
void DrawWire(RENDERLIST4DV2& rend_list);
void DrawGouraudTriangle(POLYF4DV2& face, GRAPHIC::SURFACE* surface = nullptr);
void DrawTexturedTriangle(POLYF4DV2& face, GRAPHIC::SURFACE* surface = nullptr);
void DrawTexturedTriangleFS(POLYF4DV2& face, GRAPHIC::SURFACE* surface = nullptr);
void DrawTriangle2(float x1, float y1,
	float x2, float y2,
	float x3, float y3,
	int color,
	GRAPHIC::SURFACE* surface = nullptr);
int LoadPCXImage(const char* filename, IMAGE& image);
int LoadBitmapFrom(const char* filename, IMAGE& image);
int LoadImageFrom(const char* filename, IMAGE& image);

void Reset(RENDERLIST4DV2& rend_list);
void DrawSolid(RENDERLIST4DV2& rend_list);
void DrawSolid(OBJECT4DV2& obj);
