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
*  创建时间: 2021/09/02 13:17:36
*  最后修改: 
*
*  简    介: 使用父类，虚类来实现比较合理
*
***************************************************************************************
*/

#pragma once
#include <windows.h>
#include "La_3DData.h"
#include "La_Graphic.h"
#define VERTEX_FLAGS_INVERT_X               0x0001   // inverts the Z-coordinates
#define VERTEX_FLAGS_INVERT_Y               0x0002   // inverts the Z-coordinates
#define VERTEX_FLAGS_INVERT_Z               0x0004   // inverts the Z-coordinates
#define VERTEX_FLAGS_SWAP_YZ                0x0008   // transforms a RHS model to a LHS model
#define VERTEX_FLAGS_SWAP_XZ                0x0010   
#define VERTEX_FLAGS_SWAP_XY                0x0020
#define VERTEX_FLAGS_INVERT_WINDING_ORDER   0x0040   // invert winding order from cw to ccw or ccw to cc


#define VERTEX_FLAGS_TRANSFORM_LOCAL        0x0200   // if file format has local transform then do it!
#define VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD  0x0400  // if file format has local to world then do it!


// defines for materials, follow our polygon attributes as much as possible
#define MATV1_ATTR_2SIDED                 0x0001
#define MATV1_ATTR_TRANSPARENT            0x0002
#define MATV1_ATTR_8BITCOLOR              0x0004
#define MATV1_ATTR_RGB16                  0x0008
#define MATV1_ATTR_RGB24                  0x0010

#define MATV1_ATTR_SHADE_MODE_CONSTANT    0x0020
#define MATV1_ATTR_SHADE_MODE_EMMISIVE    0x0020 // alias
#define MATV1_ATTR_SHADE_MODE_FLAT        0x0040
#define MATV1_ATTR_SHADE_MODE_GOURAUD     0x0080
#define MATV1_ATTR_SHADE_MODE_FASTPHONG   0x0100
#define MATV1_ATTR_SHADE_MODE_TEXTURE     0x0200

// defines for material system
#define MAX_MATERIALS                     256

// states of materials
#define MATV1_STATE_ACTIVE                0x0001
#define MATV1_STATE_INACTIVE              0x0001

// defines for light types
#define LIGHTV1_ATTR_AMBIENT      0x0001    // basic ambient light
#define LIGHTV1_ATTR_INFINITE     0x0002    // infinite light source
#define LIGHTV1_ATTR_DIRECTIONAL  0x0002    // infinite light source (alias)
#define LIGHTV1_ATTR_POINT        0x0004    // point light source
#define LIGHTV1_ATTR_SPOTLIGHT1   0x0008    // spotlight type 1 (simple)
#define LIGHTV1_ATTR_SPOTLIGHT2   0x0010    // spotlight type 2 (complex)

#define LIGHTV1_STATE_ON          1         // light on
#define LIGHTV1_STATE_OFF         0         // light off

#define MAX_LIGHTS                8         // good luck with 1!

// polygon sorting, and painters algorithm defines

// flags for sorting algorithm
#define SORT_POLYLIST_AVGZ  0  // sorts on average of all vertices
#define SORT_POLYLIST_NEARZ 1  // sorts on closest z vertex of each poly
#define SORT_POLYLIST_FARZ  2  // sorts on farthest z vertex of each poly



class IMAGE
{
public:
	COLOR* pbuffer;
	int width, height;
public:
	IMAGE() :pbuffer(nullptr) {};
	void release(){ if (pbuffer) delete[] pbuffer; }
	~IMAGE() { release(); }
};

// RGB+alpha color
struct RGBAV1
{
public:
	union
	{
		int rgba;                    // compressed format
		UCHAR rgba_M[4];             // array format
		struct { UCHAR a, b, g, r; }; // explict name format
	}; 
//public:
//	RGBAV1 operator=(const COLOR)
};

#define RGBA(r,g,b, a)   ((a) + ((b) << 8) + ((g) << 16) + ((r) << 24) )

// a first version of a "material"
struct MATV1
{
	int state;          
	int id;              // id of this material, index into material array
	char name[64];      
	int  attr;           // attributes, the modes for shading, constant, flat, 
						 // gouraud, fast phong, environment, textured etc.
						 // and other special flags...

	RGBAV1 color;            // color of material
	float ka, kd, ks, power; // ambient, diffuse, specular, 
							 // coefficients, note they are 
							 // separate and scalars since many 
							 // modelers use this format
							 // along with specular power

	RGBAV1 ra, rd, rs;       // the reflectivities/colors pre-
							 // multiplied, to more match our 
							 // definitions, each is basically
							 // computed by multiplying the 
							 // color by the k's, eg:
							 // rd = color*kd etc.

	char texture_file[80];   // file location of texture
	IMAGE texture;    // actual texture map (if any)

	int   iaux1, iaux2;      // auxiliary vars for future expansion
	float faux1, faux2;
	void* ptr;
};


// first light structure
struct LIGHTV1
{
	int state; 
	int id;    
	int attr;  // type of light, and extra qualifiers

	RGBAV1 c_ambient;   // ambient light intensity
	RGBAV1 c_diffuse;   // diffuse light intensity
	RGBAV1 c_specular;  // specular light intensity

	VECTOR4D  pos;       // position of light
	VECTOR4D dir;       // direction of light
	float kc, kl, kq;   // attenuation factors
	float spot_inner;   // inner angle for spot light
	float spot_outer;   // outer angle for spot light
	float pf;           // power factor/falloff for spot lights

	int   iaux1, iaux2; // auxiliary vars for future expansion
	float faux1, faux2;
	void* ptr;

	void set(
		int          _state,      // state of light
		int          _attr,       // type of light, and extra qualifiers
		RGBAV1       _c_ambient,  // ambient light intensity
		RGBAV1       _c_diffuse,  // diffuse light intensity
		RGBAV1       _c_specular, // specular light intensity
		const VECTOR4D& _pos,        // position of light
		const VECTOR4D& _dir,        // direction of light
		float        _kc,         // attenuation factors
		float        _kl,
		float        _kq,
		float        _spot_inner, // inner angle for spot light
		float        _spot_outer, // outer angle for spot light
		float        _pf)         // power factor/falloff for spot lights
	{

		// all good, initialize the light (many fields may be dead)
		this->state = _state;      // state of light
		this->attr = _attr;       // type of light, and extra qualifiers

		this->c_ambient = _c_ambient;  // ambient light intensity
		this->c_diffuse = _c_diffuse;  // diffuse light intensity
		this->c_specular = _c_specular; // specular light intensity

		this->kc = _kc;         // constant, linear, and quadratic attenuation factors
		this->kl = _kl;
		this->kq = _kq;

		this->pos = _pos;  // position of light
		this->dir = _dir;
		Normalize(this->dir);
		this->spot_inner = _spot_inner; // inner angle for spot light
		this->spot_outer = _spot_outer; // outer angle for spot light
		this->pf = _pf;         // power factor/falloff for spot lights
	} 

};


void DrawSolid(OBJECT4DV1& obj);
void DrawSolid(RENDERLIST4DV1& rend_list);
bool Insert2(RENDERLIST4DV1& rend_list, OBJECT4DV1& obj, int insert_local, int lighting_on);
bool LightWorld(OBJECT4DV1& obj, CAM4DV1& cam, LIGHTV1* lights, int max_lights);
bool LightWorld(RENDERLIST4DV1& rend_list, CAM4DV1& cam, LIGHTV1* lights, int max_lights);
void Sort(RENDERLIST4DV1& rend_list, int sort_method);







#define SIGN(x) ((x) > 0 ? (1) : (-1))





