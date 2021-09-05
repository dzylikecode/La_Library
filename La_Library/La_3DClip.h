#pragma once
#include "La_3DTexture.h"

// general clipping flags for polygons
#define CLIP_POLY_X_PLANE           0x0001 // cull on the x clipping planes
#define CLIP_POLY_Y_PLANE           0x0002 // cull on the y clipping planes
#define CLIP_POLY_Z_PLANE           0x0004 // cull on the z clipping planes
#define CLIP_OBJECT_XYZ_PLANES      (CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE)

// defines for light types version 2.0
#define LIGHTV2_ATTR_AMBIENT      0x0001    // basic ambient light
#define LIGHTV2_ATTR_INFINITE     0x0002    // infinite light source
#define LIGHTV2_ATTR_DIRECTIONAL  0x0002    // infinite light source (alias)
#define LIGHTV2_ATTR_POINT        0x0004    // point light source
#define LIGHTV2_ATTR_SPOTLIGHT1   0x0008    // spotlight type 1 (simple)
#define LIGHTV2_ATTR_SPOTLIGHT2   0x0010    // spotlight type 2 (complex)

#define LIGHTV2_STATE_ON          1         // light on
#define LIGHTV2_STATE_OFF         0         // light off


// transformation control flags
#define TRANSFORM_COPY_LOCAL_TO_TRANS   3 // copy data as is, no transform

// TYPES ///////////////////////////////////////////////////////////////////

// version 2.0 of light structure
typedef struct LIGHTV2_TYP
{
	int state; // state of light
	int id;    // id of light
	int attr;  // type of light, and extra qualifiers

	RGBAV1 c_ambient;   // ambient light intensity
	RGBAV1 c_diffuse;   // diffuse light intensity
	RGBAV1 c_specular;  // specular light intensity

	VECTOR4D  pos;       // position of light world/transformed
	VECTOR4D  tpos;
	VECTOR4D dir;       // direction of light world/transformed
	VECTOR4D tdir;
	float kc, kl, kq;   // attenuation factors
	float spot_inner;   // inner angle for spot light
	float spot_outer;   // outer angle for spot light
	float pf;           // power factor/falloff for spot lights

	int   iaux1, iaux2; // auxiliary vars for future expansion
	float faux1, faux2;
	void* ptr;

	void set(
		int           index,      // index of light to create (0..MAX_LIGHTS-1)
		int          _state,      // state of light
		int          _attr,       // type of light, and extra qualifiers
		RGBAV1       _c_ambient,  // ambient light intensity
		RGBAV1       _c_diffuse,  // diffuse light intensity
		RGBAV1       _c_specular, // specular light intensity
		VECTOR4D& _pos,        // position of light
		VECTOR4D& _dir,        // direction of light
		float        _kc,         // attenuation factors
		float        _kl,
		float        _kq,
		float        _spot_inner, // inner angle for spot light
		float        _spot_outer, // outer angle for spot light
		float        _pf)         // power factor/falloff for spot lights
	{
		// this function initializes a light based on the flags sent in _attr, values that
		// aren't needed are set to 0 by caller, nearly identical to version 1.0, however has
		// new support for transformed light coordinates 

		// all good, initialize the light (many fields may be dead)
		this->state = _state;      // state of light
		this->id = index;       // id of light
		this->attr = _attr;       // type of light, and extra qualifiers

		this->c_ambient = _c_ambient;  // ambient light intensity
		this->c_diffuse = _c_diffuse;  // diffuse light intensity
		this->c_specular = _c_specular; // specular light intensity

		this->kc = _kc;         // constant, linear, and quadratic attenuation factors
		this->kl = _kl;
		this->kq = _kq;
			this->pos= _pos;  // position of light
			this->tpos= _pos;
			this->dir= _dir;  // direction of light
			// normalize it
			Normalize(this->dir);
			this->tdir=this->dir;
		this->spot_inner = _spot_inner; // inner angle for spot light
		this->spot_outer = _spot_outer; // outer angle for spot light
		this->pf = _pf;         // power factor/falloff for spot lights
	}

} LIGHTV2, * LIGHTV2_PTR;



