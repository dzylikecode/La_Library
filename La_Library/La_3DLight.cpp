#include "La_3DLight.h"
#include "La_Log.h"

MATV1 materials[MAX_MATERIALS]; // materials in system
int num_materials;              // current number of materials



/// <summary>
/// 不考虑隐藏面消除，坐标为屏幕坐标，不需要将物体转化为多边形
/// </summary>
/// <param name="obj"></param>
void DrawSolid(OBJECT4DV1& obj)
{
	
	for (int poly = 0; poly < obj.num_polys; poly++)
	{
		if (!(obj.plist[poly].state & POLY4DV1_STATE_ACTIVE) ||
			(obj.plist[poly].state & POLY4DV1_STATE_CLIPPED) ||
			(obj.plist[poly].state & POLY4DV1_STATE_BACKFACE))
			continue; 

		 // extract vertex indices into master list, rember the polygons are 
		 // NOT self contained, but based on the vertex list stored in the object
		 // itself
		int vindex_0 = obj.plist[poly].vert[0];
		int vindex_1 = obj.plist[poly].vert[1];
		int vindex_2 = obj.plist[poly].vert[2];

		// draw the triangle
		GRAPHIC::FillTriangle(obj.vlist_trans[vindex_0].x, obj.vlist_trans[vindex_0].y,
			obj.vlist_trans[vindex_1].x, obj.vlist_trans[vindex_1].y,
			obj.vlist_trans[vindex_2].x, obj.vlist_trans[vindex_2].y,
			obj.plist[poly].color);

	} 
} 


void DrawSolid(RENDERLIST4DV1& rend_list)
{
	// this function "executes" the render list or in other words
	// draws all the faces in the list in wire frame 8bit mode
	// note there is no need to sort wire frame polygons, but 
	// later we will need to, so hidden surfaces stay hidden
	// also, we leave it to the function to determine the bitdepth
	// and call the correct rasterizer

	// at this point, all we have is a list of polygons and it's time
	// to draw them
	for (int poly = 0; poly < rend_list.num_polys; poly++)
	{
		if (!(rend_list.poly_ptrs[poly]->state & POLY4DV1_STATE_ACTIVE) ||
			(rend_list.poly_ptrs[poly]->state & POLY4DV1_STATE_CLIPPED) ||
			(rend_list.poly_ptrs[poly]->state & POLY4DV1_STATE_BACKFACE))
			continue; 

		GRAPHIC::FillTriangle(rend_list.poly_ptrs[poly]->tvlist[0].x, rend_list.poly_ptrs[poly]->tvlist[0].y,
			rend_list.poly_ptrs[poly]->tvlist[1].x, rend_list.poly_ptrs[poly]->tvlist[1].y,
			rend_list.poly_ptrs[poly]->tvlist[2].x, rend_list.poly_ptrs[poly]->tvlist[2].y,
			rend_list.poly_ptrs[poly]->color);
	} 
} 



/// <summary>
/// 
/// </summary>
/// <param name="insert_local">
/// 是使用local 还是 trans 的点， trans的话就是世界坐标
/// </param>
/// <param name="lighting_on">
/// 之前是否执行了光照计算
/// </param>
/// <returns></returns>
bool Insert2(RENDERLIST4DV1& rend_list, OBJECT4DV1& obj, int insert_local, int lighting_on)
{
	// converts the entire object into a face list and then inserts
	// the visible, active, non-clipped, non-culled polygons into
	// the render list, also note the flag insert_local control 
	// whether or not the vlist_local or vlist_trans vertex list
	// is used, thus you can insert an object "raw" totally untranformed
	// if you set insert_local to 1, default is 0, that is you would
	// only insert an object after at least the local to world transform
	// the last parameter is used to control if their has been
	// a lighting step that has generated a light value stored
	// in the upper 16-bits of color, if lighting_on = 1 then
	// this value is used to overwrite the base color of the 
	// polygon when its sent to the rendering list

	unsigned int base_color; // save base color of polygon

	// is this objective inactive or culled or invisible?
	if (!(obj.state & OBJECT4DV1_STATE_ACTIVE) ||
		(obj.state & OBJECT4DV1_STATE_CULLED) ||
		!(obj.state & OBJECT4DV1_STATE_VISIBLE))
		return(0);

	// the object is valid, let's rip it apart polygon by polygon
	for (int poly = 0; poly < obj.num_polys; poly++)
	{
		// acquire polygon
		POLY4DV1& curr_poly = obj.plist[poly];

		if (!(curr_poly.state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly.state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly.state & POLY4DV1_STATE_BACKFACE))
			continue;

		// override vertex list polygon refers to
		// the case that you want the local coords used
		// first save old pointer
		VECTOR4D* vlist_old = curr_poly.vlist;

		if (insert_local)
			curr_poly.vlist = obj.vlist_local;
		else
			curr_poly.vlist = obj.vlist_trans;

		// test if we should overwrite color with upper 16-bits
		if (lighting_on == 1)
		{
			// save color for a sec
			base_color = (curr_poly.color);
			curr_poly.color = curr_poly.lightColor;
		}

		// now insert this polygon
		if (!Insert(rend_list, curr_poly))
		{
			// fix vertex list pointer
			curr_poly.vlist = vlist_old;

			// the whole object didn't fit!
			return(false);
		}

		// test if we should overwrite color with upper 16-bits
		if (lighting_on == 1)  //已经插入成功了，恢复颜色
		{
			// fix color upc
			curr_poly.color = (base_color);
		}
		// fix vertex list pointer
		curr_poly.vlist = vlist_old;
	}
	return(true);
}


bool LightWorld(OBJECT4DV1& obj, CAM4DV1& cam,     LIGHTV1* lights,  int max_lights)      
{
	// 16-bit version of function
	// function lights an object based on the sent lights and camera. the function supports
	// constant/pure shading (emmisive), flat shading with ambient, infinite, point lights, and spot lights
	// note that this lighting function is rather brute force and simply follows the math, however
	// there are some clever integer operations that are used in scale 256 rather than going to floating
	// point, but why? floating point and ints are the same speed, HOWEVER, the conversion to and from floating
	// point can be cycle intensive, so if you can keep your calcs in ints then you can gain some speed
	// also note, type 1 spot lights are simply point lights with direction, the "cone" is more of a function
	// of the falloff due to attenuation, but they still look like spot lights
	// type 2 spot lights are implemented with the intensity having a dot product relationship with the
	// angle from the surface point to the light direction just like in the optimized model, but the pf term
	// that is used for a concentration control must be 1,2,3,.... integral and non-fractional


	int r_base, g_base, b_base,  // base color being lit
		r_sum, g_sum, b_sum,   // sum of lighting process over all lights
		shaded_color;            // final color

	float dp,     // dot product 
		dist,   // distance from light to surface
		i,      // general intensities
		nl,     // length of normal
		atten;  // attenuation computations

  // test if the object is culled
	if (!(obj.state & OBJECT4DV1_STATE_ACTIVE) ||
		(obj.state & OBJECT4DV1_STATE_CULLED) ||
		!(obj.state & OBJECT4DV1_STATE_VISIBLE))
		return(0);

	// process each poly in mesh
	for (int poly = 0; poly < obj.num_polys; poly++)
	{
		// acquire polygon
		POLY4DV1& curr_poly = obj.plist[poly];

		if (!(curr_poly.state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly.state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly.state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

		 // extract vertex indices into master list, rember the polygons are 
		 // NOT self contained, but based on the vertex list stored in the object
		 // itself
		int vindex_0 = curr_poly.vert[0];
		int vindex_1 = curr_poly.vert[1];
		int vindex_2 = curr_poly.vert[2];

		// we will use the transformed polygon vertex list since the backface removal
		// only makes sense at the world coord stage further of the pipeline 

		// test the lighting mode of the polygon (use flat for flat, gouraud))
		if (curr_poly.attr & POLY4DV1_ATTR_SHADE_MODE_FLAT || curr_poly.attr & POLY4DV1_ATTR_SHADE_MODE_GOURAUD)
		{
			// step 1: extract the base color out in RGB mode
			DisRGB(curr_poly.color, r_base, g_base, b_base);

			// initialize color sum
			r_sum = 0;
			g_sum = 0;
			b_sum = 0;

			// loop thru lights
			for (int curr_light = 0; curr_light < max_lights; curr_light++)
			{
				// is this light active
				if (lights[curr_light].state == LIGHTV1_STATE_OFF)
					continue;

				// what kind of light are we dealing with
				if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)
				{
					// simply multiply each channel against the color of the 
					// polygon then divide by 256 to scale back to 0..255
					// use a shift in real life!!! >> 8
					r_sum += ((lights[curr_light].c_ambient.r * r_base) / 256);
					g_sum += ((lights[curr_light].c_ambient.g * g_base) / 256);
					b_sum += ((lights[curr_light].c_ambient.b * b_base) / 256);

					// there better only be one ambient light!

				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE)
				{
					// infinite lighting, we need the surface normal, and the direction
					// of the light source

					// we need to compute the normal of this polygon face, and recall
					// that the vertices are in cw order, u=p0.p1, v=p0.p2, n=uxv
					VECTOR4D u, v, n;

					// build u, v
					u.set(obj.vlist_trans[vindex_0], obj.vlist_trans[vindex_1]);
					v.set(obj.vlist_trans[vindex_0], obj.vlist_trans[vindex_2]);

					// compute cross product
					Cross(u, v, n);

					// at this point, we are almost ready, but we have to normalize the normal vector!
					// this is a key optimization we can make later, we can pre-compute the length of all polygon
					// normals, so this step can be optimized
					// compute length of normal
					nl = LengthFast(n);

					// ok, recalling the lighting model for infinite lights
					// I(d)dir = I0dir * Cldir
					// and for the diffuse model
					// Itotald =   Rsdiffuse*Idiffuse * (n . l)
					// so we basically need to multiple it all together
					// notice the scaling by 128, I want to avoid floating point calculations, not because they 
					// are slower, but the conversion to and from cost cycles

					dp = Dot(n, lights[curr_light].dir);

					// only add light if dp > 0
					if (dp > 0)
					{
						i = 128 * dp / nl;
						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_POINT)
				{
					// perform point light computations
					// light model for point light is once again:
					//              I0point * Clpoint
					//  I(d)point = ___________________
					//              kc +  kl*d + kq*d2              
					//
					//  Where d = |p - s|
					// thus it's almost identical to the infinite light, but attenuates as a function
					// of distance from the point source to the surface point being lit

					// we need to compute the normal of this polygon face, and recall
					// that the vertices are in cw order, u=p0.p1, v=p0.p2, n=uxv
					VECTOR4D u, v, n, l;

					// build u, v
					u.set(obj.vlist_trans[vindex_0], obj.vlist_trans[vindex_1]);
					v.set(obj.vlist_trans[vindex_0], obj.vlist_trans[vindex_2]);

					// compute cross product
					Cross(u, v, n);

					// at this point, we are almost ready, but we have to normalize the normal vector!
					// this is a key optimization we can make later, we can pre-compute the length of all polygon
					// normals, so this step can be optimized
					// compute length of normal
					nl = LengthFast(n);

					// compute vector from surface to light
					l.set(obj.vlist_trans[vindex_0], lights[curr_light].pos);

					// compute distance and attenuation
					dist = LengthFast(l);

					// and for the diffuse model
					// Itotald =   Rsdiffuse*Idiffuse * (n . l)
					// so we basically need to multiple it all together
					// notice the scaling by 128, I want to avoid floating point calculations, not because they 
					// are slower, but the conversion to and from cost cycles
					dp = Dot(n, l);

					// only add light if dp > 0
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);

						i = 128 * dp / (nl * dist * atten);

						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)  //也就是点光源和平行光结合
				{
					// perform spotlight/point computations simplified model that uses
					// point light WITH a direction to simulate a spotlight
					// light model for point light is once again:
					//              I0point * Clpoint
					//  I(d)point = ___________________
					//              kc +  kl*d + kq*d2              
					//
					//  Where d = |p - s|
					// thus it's almost identical to the infinite light, but attenuates as a function
					// of distance from the point source to the surface point being lit

					// we need to compute the normal of this polygon face, and recall
					// that the vertices are in cw order, u=p0.p1, v=p0.p2, n=uxv
					VECTOR4D u, v, n, l;

					// build u, v
					u.set(obj.vlist_trans[vindex_0], obj.vlist_trans[vindex_1]);
					v.set(obj.vlist_trans[vindex_0], obj.vlist_trans[vindex_2]);

					// compute cross product (we need -n, so do vxu)
					Cross(v, u, n);

					// at this point, we are almost ready, but we have to normalize the normal vector!
					// this is a key optimization we can make later, we can pre-compute the length of all polygon
					// normals, so this step can be optimized
					// compute length of normal
					nl = LengthFast(n);

					// compute vector from surface to light
					l.set(obj.vlist_trans[vindex_0], lights[curr_light].pos);

					// compute distance and attenuation
					dist = LengthFast(l);

					// and for the diffuse model
					// Itotald =   Rsdiffuse*Idiffuse * (n . l)
					// so we basically need to multiple it all together
					// notice the scaling by 128, I want to avoid floating point calculations, not because they 
					// are slower, but the conversion to and from cost cycles

					// note that I use the direction of the light here rather than a the vector to the light
					// thus we are taking orientation into account which is similar to the spotlight model
					dp = Dot(n, lights[curr_light].dir); //这里不大相同

					// only add light if dp > 0
					if (dp > 0)
					{
						atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);

						i = 128 * dp / (nl * atten);

						r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
						g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
						b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
					}
				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2) // simple version
				{
					// perform spot light computations
					// light model for spot light simple version is once again:
					//         	     I0spotlight * Clspotlight * MAX( (l . s), 0)^pf                     
					// I(d)spotlight = __________________________________________      
					//               		 kc + kl*d + kq*d2        
					// Where d = |p - s|, and pf = power factor

					// thus it's almost identical to the point, but has the extra term in the numerator
					// relating the angle between the light source and the point on the surface

					// we need to compute the normal of this polygon face, and recall
					// that the vertices are in cw order, u=p0.p1, v=p0.p2, n=uxv
					VECTOR4D u, v, n, d, s;

					// build u, v
					u.set(obj.vlist_trans[vindex_0], obj.vlist_trans[vindex_1]);
					v.set(obj.vlist_trans[vindex_0], obj.vlist_trans[vindex_2]);

					// compute cross product (v x u, to invert n)
					Cross(v, u, n);

					// at this point, we are almost ready, but we have to normalize the normal vector!
					// this is a key optimization we can make later, we can pre-compute the length of all polygon
					// normals, so this step can be optimized
					// compute length of normal
					nl = LengthFast(n);

					// and for the diffuse model
					// Itotald =   Rsdiffuse*Idiffuse * (n . l)
					// so we basically need to multiple it all together
					// notice the scaling by 128, I want to avoid floating point calculations, not because they 
					// are slower, but the conversion to and from cost cycles
					dp = Dot(n, lights[curr_light].dir);  

					// only add light if dp > 0
					if (dp > 0)
					{
						// compute vector from light to surface (different from l which IS the light dir)
						s.set(lights[curr_light].pos, obj.vlist_trans[vindex_0]);

						// compute length of s (distance to light source) to normalize s for lighting calc
						dist = LengthFast(s);

						// compute spot light term (s . l)
						float dpsl = Dot(s, lights[curr_light].dir) / dist;

						// proceed only if term is positive
						if (dpsl > 0)
						{
							// compute attenuation
							atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);

							// for speed reasons, pf exponents that are less that 1.0 are out of the question, and exponents
							// must be integral
							float dpsl_exp = dpsl;

							// exponentiate for positive integral powers
							for (int e_index = 1; e_index < (int)lights[curr_light].pf; e_index++)
								dpsl_exp *= dpsl;

							// now dpsl_exp holds (dpsl)^pf power which is of course (s . l)^pf 

							i = 128 * dp * dpsl_exp / (nl * atten);

							r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
							g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
							b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);

						}

					}

				}

			}

			// make sure colors aren't out of range
			if (r_sum > 255) r_sum = 255;
			if (g_sum > 255) g_sum = 255;
			if (b_sum > 255) b_sum = 255;

			// write the color
			shaded_color = RGB_DX(r_sum, g_sum, b_sum);
			curr_poly.lightColor = shaded_color;

		}
		else // assume POLY4DV1_ATTR_SHADE_MODE_CONSTANT
		{
			// emmisive shading only, copy base color into upper 16-bits
			// without any change
			curr_poly.lightColor = curr_poly.color;
		}

	}

	return(true);

}


bool LightWorld(RENDERLIST4DV1& rend_list,  CAM4DV1& cam,     LIGHTV1* lights,  int max_lights)      
{
	// 16-bit version of function
	// function lights the enture rendering list based on the sent lights and camera. the function supports
	// constant/pure shading (emmisive), flat shading with ambient, infinite, point lights, and spot lights
	// note that this lighting function is rather brute force and simply follows the math, however
	// there are some clever integer operations that are used in scale 256 rather than going to floating
	// point, but why? floating point and ints are the same speed, HOWEVER, the conversion to and from floating
	// point can be cycle intensive, so if you can keep your calcs in ints then you can gain some speed
	// also note, type 1 spot lights are simply point lights with direction, the "cone" is more of a function
	// of the falloff due to attenuation, but they still look like spot lights
	// type 2 spot lights are implemented with the intensity having a dot product relationship with the
	// angle from the surface point to the light direction just like in the optimized model, but the pf term
	// that is used for a concentration control must be 1,2,3,.... integral and non-fractional

	// also note since we are dealing with a rendering list and not object, the final lit color is
	// immediately written over the real color

	int r_base, g_base, b_base,  // base color being lit
		r_sum, g_sum, b_sum,   // sum of lighting process over all lights
		shaded_color;            // final color

	float dp,     // dot product 
		dist,   // distance from light to surface
		i,      // general intensities
		nl,     // length of normal
		atten;  // attenuation computations


  // for each valid poly, light it...
	for (int poly = 0; poly < rend_list.num_polys; poly++)
	{
		// acquire polygon
		POLYF4DV1& curr_poly = *rend_list.poly_ptrs[poly];

		// light this polygon if and only if it's not clipped, not culled,
		// active, and visible
		if (!(curr_poly.state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly.state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly.state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly


		 // we will use the transformed polygon vertex list since the backface removal
		 // only makes sense at the world coord stage further of the pipeline 

		 // test the lighting mode of the polygon (use flat for flat, gouraud))
		if (curr_poly.attr & POLY4DV1_ATTR_SHADE_MODE_FLAT || curr_poly.attr & POLY4DV1_ATTR_SHADE_MODE_GOURAUD)
		{
			// step 1: extract the base color out in RGB mode
			DisRGB(curr_poly.color, r_base, g_base, b_base);

		 // initialize color sum
			r_sum = 0;
			g_sum = 0;
			b_sum = 0;

			// loop thru lights
			for (int curr_light = 0; curr_light < max_lights; curr_light++)
			{
				// is this light active
				if (lights[curr_light].state == LIGHTV1_STATE_OFF)
					continue;

				// what kind of light are we dealing with
				if (lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)
				{
					// simply multiply each channel against the color of the 
					// polygon then divide by 256 to scale back to 0..255
					// use a shift in real life!!! >> 8
					r_sum += ((lights[curr_light].c_ambient.r * r_base) / 256);
					g_sum += ((lights[curr_light].c_ambient.g * g_base) / 256);
					b_sum += ((lights[curr_light].c_ambient.b * b_base) / 256);

					// there better only be one ambient light!

				}
				else if (lights[curr_light].attr & LIGHTV1_ATTR_INFINITE)
					{
						// infinite lighting, we need the surface normal, and the direction
						// of the light source

						// we need to compute the normal of this polygon face, and recall
						// that the vertices are in cw order, u=p0.p1, v=p0.p2, n=uxv
						VECTOR4D u, v, n;

						// build u, v
						u.set(curr_poly.tvlist[0], curr_poly.tvlist[1]);
						v.set(curr_poly.tvlist[0], curr_poly.tvlist[2]);

						// compute cross product
						Cross(u, v, n);

						// at this point, we are almost ready, but we have to normalize the normal vector!
						// this is a key optimization we can make later, we can pre-compute the length of all polygon
						// normals, so this step can be optimized
						// compute length of normal
						nl = LengthFast(n);

						// ok, recalling the lighting model for infinite lights
						// I(d)dir = I0dir * Cldir
						// and for the diffuse model
						// Itotald =   Rsdiffuse*Idiffuse * (n . l)
						// so we basically need to multiple it all together
						// notice the scaling by 128, I want to avoid floating point calculations, not because they 
						// are slower, but the conversion to and from cost cycles

						dp = Dot(n, lights[curr_light].dir);

						// only add light if dp > 0
						if (dp > 0)
						{
							i = 128 * dp / nl;
							r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
							g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
							b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
						} 

					} 
					else if (lights[curr_light].attr & LIGHTV1_ATTR_POINT)
						{
							// perform point light computations
							// light model for point light is once again:
							//              I0point * Clpoint
							//  I(d)point = ___________________
							//              kc +  kl*d + kq*d2              
							//
							//  Where d = |p - s|
							// thus it's almost identical to the infinite light, but attenuates as a function
							// of distance from the point source to the surface point being lit

							// we need to compute the normal of this polygon face, and recall
							// that the vertices are in cw order, u=p0.p1, v=p0.p2, n=uxv
							VECTOR4D u, v, n, l;

							// build u, v
							u.set(curr_poly.tvlist[0], curr_poly.tvlist[1]);
							v.set(curr_poly.tvlist[0], curr_poly.tvlist[2]);

							// compute cross product
							Cross(u, v, n);

							// at this point, we are almost ready, but we have to normalize the normal vector!
							// this is a key optimization we can make later, we can pre-compute the length of all polygon
							// normals, so this step can be optimized
							// compute length of normal
							nl = LengthFast(n);

							// compute vector from surface to light
							l.set(curr_poly.tvlist[0], lights[curr_light].pos);

							// compute distance and attenuation
							dist = LengthFast(l);

							// and for the diffuse model
							// Itotald =   Rsdiffuse*Idiffuse * (n . l)
							// so we basically need to multiple it all together
							// notice the scaling by 128, I want to avoid floating point calculations, not because they 
							// are slower, but the conversion to and from cost cycles
							dp = Dot(n, l);

							// only add light if dp > 0
							if (dp > 0)
							{
								atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);

								i = 128 * dp / (nl * dist * atten);

								r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
								g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
								b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
							} 

						} 
						else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)
							{
								// perform spotlight/point computations simplified model that uses
								// point light WITH a direction to simulate a spotlight
								// light model for point light is once again:
								//              I0point * Clpoint
								//  I(d)point = ___________________
								//              kc +  kl*d + kq*d2              
								//
								//  Where d = |p - s|
								// thus it's almost identical to the infinite light, but attenuates as a function
								// of distance from the point source to the surface point being lit

								// we need to compute the normal of this polygon face, and recall
								// that the vertices are in cw order, u=p0.p1, v=p0.p2, n=uxv
								VECTOR4D u, v, n, l;

								// build u, v
								u.set(curr_poly.tvlist[0], curr_poly.tvlist[1]);
								v.set(curr_poly.tvlist[0], curr_poly.tvlist[2]);

								// compute cross product (we need -n, so do vxu)
								Cross(v, u, n);

								// at this point, we are almost ready, but we have to normalize the normal vector!
								// this is a key optimization we can make later, we can pre-compute the length of all polygon
								// normals, so this step can be optimized
								// compute length of normal
								nl = LengthFast(n);

								// compute vector from surface to light
								l.set(curr_poly.tvlist[0], lights[curr_light].pos);

								// compute distance and attenuation
								dist = LengthFast(l);

								// and for the diffuse model
								// Itotald =   Rsdiffuse*Idiffuse * (n . l)
								// so we basically need to multiple it all together
								// notice the scaling by 128, I want to avoid floating point calculations, not because they 
								// are slower, but the conversion to and from cost cycles

								// note that I use the direction of the light here rather than a the vector to the light
								// thus we are taking orientation into account which is similar to the spotlight model
								dp =Dot(n, lights[curr_light].dir);

								// only add light if dp > 0
								if (dp > 0)
								{
									atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);

									i = 128 * dp / (nl * atten);

									r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
									g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
									b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);
								} 

							} 
							else if (lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2) // simple version
								{
									// perform spot light computations
									// light model for spot light simple version is once again:
									//         	     I0spotlight * Clspotlight * MAX( (l . s), 0)^pf                     
									// I(d)spotlight = __________________________________________      
									//               		 kc + kl*d + kq*d2        
									// Where d = |p - s|, and pf = power factor

									// thus it's almost identical to the point, but has the extra term in the numerator
									// relating the angle between the light source and the point on the surface

									// we need to compute the normal of this polygon face, and recall
									// that the vertices are in cw order, u=p0.p1, v=p0.p2, n=uxv
									VECTOR4D u, v, n, d, s;

									// build u, v
									u.set(curr_poly.tvlist[0], curr_poly.tvlist[1]);
									v.set(curr_poly.tvlist[0], curr_poly.tvlist[2]);

									// compute cross product (v x u, to invert n)
									Cross(v, u, n);

									// at this point, we are almost ready, but we have to normalize the normal vector!
									// this is a key optimization we can make later, we can pre-compute the length of all polygon
									// normals, so this step can be optimized
									// compute length of normal
									nl = LengthFast(n);

									// and for the diffuse model
									// Itotald =   Rsdiffuse*Idiffuse * (n . l)
									// so we basically need to multiple it all together
									// notice the scaling by 128, I want to avoid floating point calculations, not because they 
									// are slower, but the conversion to and from cost cycles
									dp = Dot(n, lights[curr_light].dir);

									// only add light if dp > 0
									if (dp > 0)
									{
										// compute vector from light to surface (different from l which IS the light dir)
										s.set(lights[curr_light].pos, curr_poly.tvlist[0]);

										// compute length of s (distance to light source) to normalize s for lighting calc
										dist = LengthFast(s);

										// compute spot light term (s . l)
										float dpsl = Dot(s, lights[curr_light].dir) / dist;

										// proceed only if term is positive
										if (dpsl > 0)
										{
											// compute attenuation
											atten = (lights[curr_light].kc + lights[curr_light].kl * dist + lights[curr_light].kq * dist * dist);

											// for speed reasons, pf exponents that are less that 1.0 are out of the question, and exponents
											// must be integral
											float dpsl_exp = dpsl;

											// exponentiate for positive integral powers
											for (int e_index = 1; e_index < (int)lights[curr_light].pf; e_index++)
												dpsl_exp *= dpsl;

											// now dpsl_exp holds (dpsl)^pf power which is of course (s . l)^pf 

											i = 128 * dp * dpsl_exp / (nl * atten);

											r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
											g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
											b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);

										} 
									} 
								}
			} 
		// make sure colors aren't out of range
			if (r_sum > 255) r_sum = 255;
			if (g_sum > 255) g_sum = 255;
			if (b_sum > 255) b_sum = 255;

			// write the color over current color
			curr_poly.color = RGB_DX(r_sum, g_sum, b_sum);

		} 
		else // assume POLY4DV1_ATTR_SHADE_MODE_CONSTANT
		{
			// emmisive shading only, do nothing
			// ...
		} 

	} 
	return(true);

} 

int CompareAvgZ(const void* arg1, const void* arg2)
{
	// this function comapares the average z's of two polygons and is used by the
	// depth sort surface ordering algorithm

	float z1, z2;

	POLYF4DV1* poly_1, * poly_2;

	// dereference the poly pointers
	poly_1 = *((POLYF4DV1**)(arg1));
	poly_2 = *((POLYF4DV1**)(arg2));

	// compute z average of each polygon
	z1 = (float)0.33333 * (poly_1->tvlist[0].z + poly_1->tvlist[1].z + poly_1->tvlist[2].z);

	// now polygon 2
	z2 = (float)0.33333 * (poly_2->tvlist[0].z + poly_2->tvlist[1].z + poly_2->tvlist[2].z);

	// compare z1 and z2, such that polys' will be sorted in descending Z order
	if (z1 > z2)
		return(-1);
	else if (z1 < z2)
		return(1);
	else
		return(0);
}


int CompareNearZ(const void* arg1, const void* arg2)
{
	// this function comapares the closest z's of two polygons and is used by the
	// depth sort surface ordering algorithm

	float z1, z2;

	POLYF4DV1* poly_1, * poly_2;

	// dereference the poly pointers
	poly_1 = *((POLYF4DV1**)(arg1));
	poly_2 = *((POLYF4DV1**)(arg2));

	// compute the near z of each polygon
	z1 = min(poly_1->tvlist[0].z, poly_1->tvlist[1].z);
	z1 = min(z1, poly_1->tvlist[2].z);

	z2 = min(poly_2->tvlist[0].z, poly_2->tvlist[1].z);
	z2 = min(z2, poly_2->tvlist[2].z);

	// compare z1 and z2, such that polys' will be sorted in descending Z order
	if (z1 > z2)
		return(-1);
	else if (z1 < z2)
		return(1);
	else
		return(0);

}


int CompareFarZ(const void* arg1, const void* arg2)
{
	// this function comapares the farthest z's of two polygons and is used by the
	// depth sort surface ordering algorithm

	float z1, z2;

	POLYF4DV1* poly_1, * poly_2;

	// dereference the poly pointers
	poly_1 = *((POLYF4DV1**)(arg1));
	poly_2 = *((POLYF4DV1**)(arg2));

	// compute the near z of each polygon
	z1 = max(poly_1->tvlist[0].z, poly_1->tvlist[1].z);
	z1 = max(z1, poly_1->tvlist[2].z);

	z2 = max(poly_2->tvlist[0].z, poly_2->tvlist[1].z);
	z2 = max(z2, poly_2->tvlist[2].z);

	// compare z1 and z2, such that polys' will be sorted in descending Z order
	if (z1 > z2)
		return(-1);
	else if (z1 < z2)
		return(1);
	else
		return(0);

}

/// <summary>
/// 画家算法，在相机空间中使用
/// </summary>
void Sort(RENDERLIST4DV1& rend_list, int sort_method)
{
	// this function sorts the rendering list based on the polygon z-values 
	// the specific sorting method is controlled by sending in control flags
	// #define SORT_POLYLIST_AVGZ  0 - sorts on average of all vertices
	// #define SORT_POLYLIST_NEARZ 1 - sorts on closest z vertex of each poly
	// #define SORT_POLYLIST_FARZ  2 - sorts on farthest z vertex of each poly

	switch (sort_method)
	{
	case SORT_POLYLIST_AVGZ:  //  - sorts on average of all vertices
	{
		qsort((void*)rend_list.poly_ptrs, rend_list.num_polys, sizeof(POLYF4DV1*), CompareAvgZ);
	} break;

	case SORT_POLYLIST_NEARZ: // - sorts on closest z vertex of each poly
	{
		qsort((void*)rend_list.poly_ptrs, rend_list.num_polys, sizeof(POLYF4DV1*), CompareNearZ);
	} break;

	case SORT_POLYLIST_FARZ:  //  - sorts on farthest z vertex of each poly
	{
		qsort((void*)rend_list.poly_ptrs, rend_list.num_polys, sizeof(POLYF4DV1*), CompareFarZ);
	} break;

	default: break;
	}
}










