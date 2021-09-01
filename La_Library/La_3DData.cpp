
#include "La_Graphic.h"
#include "La_3DData.h"
#include "La_Log.h"


#define  IsInvalidPoly(cur)     ( (cur == nullptr)  ||   \
								 !(cur->state & POLY4DV1_STATE_ACTIVE) || \
								  (cur->state & POLY4DV1_STATE_CLIPPED)|| \
								  (cur->state & POLY4DV1_STATE_BACKFACE) )
namespace
{
	//获取PLG的一行的数据，空白行和注释会清除掉
	char* GetLinePLG(char* buffer, int maxlength, FILE* fp)
	{
		int index = 0;  // general index
		int length = 0; // general length

		// enter into parsing loop
		while (1)
		{
			// read the next line
			if (!fgets(buffer, maxlength, fp)) return(nullptr);

			// kill the whitespace  找到第一个不是空格的开头
			for (length = strlen(buffer), index = 0; isspace(buffer[index]); index++);

			// test if this was a blank line or a comment
			if (index >= length || buffer[index] == '#') continue;

			// at this point we have a good line
			return(&buffer[index]);
		}
	}

}


/// <summary>
/// 这是在模型中计算，中心就设置为原点
/// </summary>
/// <returns>
/// 最大半径
/// </returns>
float ComputeRadius(OBJECT4DV1& obj)
{
	obj.avg_radius = 0;
	obj.max_radius = 0;

	for (int vertex = 0; vertex < obj.num_vertices; vertex++)
	{
		// update the average and maximum radius
		float dist_to_vertex =
			sqrt(obj.vlist_local[vertex].x * obj.vlist_local[vertex].x +
				obj.vlist_local[vertex].y * obj.vlist_local[vertex].y +
				obj.vlist_local[vertex].z * obj.vlist_local[vertex].z);

		// accumulate total radius
		obj.avg_radius += dist_to_vertex;

		// update maximum radius   
		if (dist_to_vertex > obj.max_radius)
			obj.max_radius = dist_to_vertex;

	} 
	//计算平均半径
	obj.avg_radius /= obj.num_vertices;

	return(obj.max_radius);
}


bool LoadPLG(OBJECT4DV1& obj, const TCHAR* filename, const VECTOR4D& pos, const VECTOR4D& scale, const VECTOR4D& rot)
{
	FILE* fp;  
	char buffer[256];  

	char* token_string;  // pointer to actual token text, ready for parsing

	// Step 1: clear out the object and initialize it a bit
	memset(&obj, 0, sizeof(OBJECT4DV1));

	// set state of object to active and visible
	obj.state = OBJECT4DV1_STATE_ACTIVE | OBJECT4DV1_STATE_VISIBLE;

	// set position of object
	obj.world_pos.x = pos.x;
	obj.world_pos.y = pos.y;
	obj.world_pos.z = pos.z;
	obj.world_pos.w = pos.w;

	// Step 2: open the file for reading
	if (!(fp = fopen(TToA(filename), "r")))
	{
		ERROR_MSG(TEXT("Couldn't open PLG file %s."), filename);
		return(false);
	}

	// Step 3: get the first token string which should be the object descriptor
	if (!(token_string = GetLinePLG(buffer, 255, fp)))
	{
		ERROR_MSG(TEXT("PLG file error with file %s (object descriptor invalid)."), filename);
		return(false);
	}

	ERROR_MSG(TEXT("Object Descriptor: %s"), token_string);

	// parse out the info object
	sscanf(token_string, "%s %d %d", obj.name, &obj.num_vertices, &obj.num_polys);

	// Step 4: load the vertex list
	for (int vertex = 0; vertex < obj.num_vertices; vertex++)
	{
		// get the next vertex
		if (!(token_string = GetLinePLG(buffer, 255, fp)))
		{
			ERROR_MSG(TEXT("PLG file error with file %s (vertex list invalid)."), filename);
			return(0);
		}

	// parse out vertex
		sscanf(token_string, "%f %f %f", &obj.vlist_local[vertex].x,
			&obj.vlist_local[vertex].y,
			&obj.vlist_local[vertex].z);
		obj.vlist_local[vertex].w = 1;

		// scale vertices
		obj.vlist_local[vertex].x *= scale.x;
		obj.vlist_local[vertex].y *= scale.y;
		obj.vlist_local[vertex].z *= scale.z;

		ERROR_MSG(TEXT("\nVertex %d = %f, %f, %f, %f"), vertex,
			obj.vlist_local[vertex].x,
			obj.vlist_local[vertex].y,
			obj.vlist_local[vertex].z,
			obj.vlist_local[vertex].w);
	}

// compute average and max radius
	ComputeRadius(obj);

	ERROR_MSG(TEXT("\nObject average radius = %f, max radius = %f"),
		obj.avg_radius, obj.max_radius);

	int poly_surface_desc = 0; // PLG/PLX surface descriptor
	int poly_num_verts = 0; // number of vertices for current poly (always 3)  一般都是三角形
	char tmp_string[8];        // temp string to hold surface descriptor in and
							   // test if it need to be converted from hex

	// Step 5: load the polygon list
	for (int poly = 0; poly < obj.num_polys; poly++)
	{
		// get the next polygon descriptor
		if (!(token_string = GetLinePLG(buffer, 255, fp)))
		{
			ERROR_MSG(TEXT("PLG file error with file %s (polygon descriptor invalid)."), filename);
			return(0);
		} 

		ERROR_MSG(TEXT("\nPolygon %d:"), poly);

		sscanf(token_string, "%s %d %d %d %d", tmp_string,
			&poly_num_verts, // should always be 3 
			&obj.plist[poly].vert[0],
			&obj.plist[poly].vert[1],
			&obj.plist[poly].vert[2]);

		//处理16进制或者十进制
		if (tmp_string[0] == '0' && toupper(tmp_string[1]) == 'X')
			sscanf(tmp_string, "%x", &poly_surface_desc);
		else poly_surface_desc = atoi(tmp_string);

		//这挺多余的，因为在同一个类当中，可以用 nullptr 来表示是 vlist_local
		obj.plist[poly].vlist = obj.vlist_local;

		ERROR_MSG(TEXT("\nSurface Desc = 0x%.4x, num_verts = %d, vert_indices [%d, %d, %d]"),
			poly_surface_desc,
			poly_num_verts,
			obj.plist[poly].vert[0],
			obj.plist[poly].vert[1],
			obj.plist[poly].vert[2]);	

		//最高位
		if ((poly_surface_desc & PLX_COLOR_MODE_RGB_FLAG))
		{
			// this is an RGB 4.4.4 surface
			SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_RGB16);

			// now extract color and copy into polygon color
			// 0x0RGB is the format, 4 bits per pixel 
			int red = ((poly_surface_desc & 0x0f00) >> 8);
			int green = ((poly_surface_desc & 0x00f0) >> 4);
			int blue = (poly_surface_desc & 0x000f);

			obj.plist[poly].color = RGB_DX(red * 16, green * 16, blue * 16);
			ERROR_MSG(TEXT("\nRGB color = [%d, %d, %d]"), red, green, blue);
		}
		else
		{
			// this is an 8-bit color indexed surface
			SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_8BITCOLOR);

			// and simple extract the last 8 bits and that's the color index
			obj.plist[poly].color = (poly_surface_desc & 0x00ff);

			ERROR_MSG(TEXT("\n8-bit color index = %d"), obj.plist[poly].color);
		} 

		// 接连的两个位
		int shade_mode = (poly_surface_desc & PLX_SHADE_MODE_MASK);

		// set polygon shading mode
		switch (shade_mode)
		{
		case PLX_SHADE_MODE_PURE_FLAG: 
		{
			SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PURE);
			ERROR_MSG(TEXT("\nShade mode = pure"));
		} break;

		case PLX_SHADE_MODE_FLAT_FLAG: 
		{
			SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_FLAT);
			ERROR_MSG(TEXT("\nShade mode = flat"));

		} break;

		case PLX_SHADE_MODE_GOURAUD_FLAG: 
		{
			SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_GOURAUD);
			ERROR_MSG(TEXT("\nShade mode = gouraud"));
		} break;

		case PLX_SHADE_MODE_PHONG_FLAG: 
		{
			SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PHONG);
			ERROR_MSG(TEXT("\nShade mode = phong"));
		} break;

		default: break;
		} 

		// 高四位的最后一位
		if ((poly_surface_desc & PLX_2SIDED_FLAG))
		{
			SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_2SIDED);
			ERROR_MSG(TEXT("\n2 sided."));
		}
		else
		{
			ERROR_MSG(TEXT("\n1 sided.")); // one sided
		}
		obj.plist[poly].state = POLY4DV1_STATE_ACTIVE;
	} 
	fclose(fp);

	return(true);
} 

/// <summary>
/// 
/// </summary>
/// <param name="coord_select">
/// 指定变换的坐标
/// </param>
void Transform(RENDERLIST4DV1& rend_list, const MATRIX4X4& mt, int coord_select)
{
	//无需变换的面

	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY://vlist 变换并放入其中
	{
		for (int poly = 0; poly < rend_list.num_polys; poly++)
		{
			// acquire current polygon
			POLYF4DV1* curr_poly = rend_list.poly_ptrs[poly];

			if (IsInvalidPoly(curr_poly))
				continue; // move onto next poly

			//vert * mt = vert
			for (int vertex = 0; vertex < 3; vertex++)
			{
				VECTOR4D presult;
				Mul(curr_poly->vlist[vertex], mt, presult);
				curr_poly->vlist[vertex] = presult;
			}
		}

	} break;

	case TRANSFORM_TRANS_ONLY:   //tlist 变换并放入其中
	{
		for (int poly = 0; poly < rend_list.num_polys; poly++)
		{
			POLYF4DV1* curr_poly = rend_list.poly_ptrs[poly];

			if (IsInvalidPoly(curr_poly))
				continue;

			for (int vertex = 0; vertex < 3; vertex++)
			{
				VECTOR4D presult;
				Mul(curr_poly->tvlist[vertex], mt, presult);
				curr_poly->tvlist[vertex] = presult;
			}
		}
	} break;

	case TRANSFORM_LOCAL_TO_TRANS: // vlist 变换 放入 tlist 中
	{
		for (int poly = 0; poly < rend_list.num_polys; poly++)
		{
			POLYF4DV1* curr_poly = rend_list.poly_ptrs[poly];
			if (IsInvalidPoly(curr_poly))
				continue;
			for (int vertex = 0; vertex < 3; vertex++)
			{
				Mul(curr_poly->vlist[vertex], mt, curr_poly->tvlist[vertex]);
			}
		}
	} break;

	default: break;
	}
}

/// <summary>
/// 
/// </summary>
/// <param name="transform_basis">
/// 是否对朝向的向量变换
/// </param>
void Transform(OBJECT4DV1& obj, const MATRIX4X4& mt, int coord_select, bool transform_basis)
{
	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
	{
		for (int vertex = 0; vertex < obj.num_vertices; vertex++)
		{
			VECTOR4D presult;
			Mul(obj.vlist_local[vertex], mt, presult);
			obj.vlist_local[vertex] = presult;
		}
	} break;

	case TRANSFORM_TRANS_ONLY:
	{
		for (int vertex = 0; vertex < obj.num_vertices; vertex++)
		{
			VECTOR4D presult;
			Mul(obj.vlist_trans[vertex], mt, presult);
			obj.vlist_trans[vertex] = presult;
		}
	} break;

	case TRANSFORM_LOCAL_TO_TRANS:
	{
		for (int vertex = 0; vertex < obj.num_vertices; vertex++)
		{
			VECTOR4D presult;
			Mul(obj.vlist_local[vertex], mt, obj.vlist_trans[vertex]);
		}
	} break;

	default: break;

	}

	//如果不变换，则朝向向量将不再有效
	if (transform_basis)
	{
		VECTOR4D vresult;

		Mul(obj.ux, mt, vresult);
		obj.ux = vresult;

		Mul(obj.uy, mt, vresult);
		obj.uy = vresult;

		Mul(obj.uz, mt, vresult);
		obj.uz = vresult;
	}
}


//储存在 tran 中
void ModelToWorld(OBJECT4DV1& obj, int coord_select)
{
	if (coord_select == TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int vertex = 0; vertex < obj.num_vertices; vertex++)
		{
			Add(obj.vlist_local[vertex], obj.world_pos, obj.vlist_trans[vertex]);
		}
	}
	else
	{   // TRANSFORM_TRANS_ONLY
		for (int vertex = 0; vertex < obj.num_vertices; vertex++)
		{
			Add(obj.vlist_trans[vertex], obj.world_pos, obj.vlist_trans[vertex]);
		}
	}
}


void ModelToWorld(RENDERLIST4DV1& rend_list, VECTOR4D& world_pos, int coord_select)
{
	if (coord_select == TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int poly = 0; poly < rend_list.num_polys; poly++)
		{
			POLYF4DV1* curr_poly = rend_list.poly_ptrs[poly];

			if (IsInvalidPoly(curr_poly))
				continue;
			for (int vertex = 0; vertex < 3; vertex++)
			{
				Add(curr_poly->vlist[vertex], world_pos, curr_poly->tvlist[vertex]);
			}
		}
	}
	else // TRANSFORM_TRANS_ONLY
	{
		for (int poly = 0; poly < rend_list.num_polys; poly++)
		{
			POLYF4DV1* curr_poly = rend_list.poly_ptrs[poly];
			if (IsInvalidPoly(curr_poly))
				continue;

			for (int vertex = 0; vertex < 3; vertex++)
			{
				Add(curr_poly->tvlist[vertex], world_pos, curr_poly->tvlist[vertex]);
			}
		}
	}
}


void CAM4DV1:: set(
	int cam_attr,
	VECTOR4D& cam_pos,
	VECTOR4D& cam_dir,		// initial camera angles
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

	if (cam_target != nullptr)this->target = *cam_target; // UVN target
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

	this->viewplane_width = 2.0;
	this->viewplane_height = 2.0 / this->aspect_ratio;

	// now we know fov and we know the viewplane dimensions plug into formula and
	// solve for view distance parameters
	float tan_fov_div2 = tan(DEG_TO_RAD(fov / 2));

	this->view_dist = (0.5) * (this->viewplane_width) * tan_fov_div2;

	if (fov == 90.0)
	{
		VECTOR3D pt_origin(0, 0, 0); 

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


/// <summary>
/// 根据欧拉角创建变换矩阵
/// </summary>
/// <param name="cam_rot_seq"></param>
void BuildEuler(CAM4DV1& cam, int cam_rot_seq)
{
	MATRIX4X4 mt_inv,  // inverse camera translation matrix
		mx_inv,  // inverse camera x axis rotation matrix
		my_inv,  // inverse camera y axis rotation matrix
		mz_inv,  // inverse camera z axis rotation matrix
		mrot,    // concatenated inverse rotation matrices
		mtmp;    // temporary working matrix


// step 1: create the inverse translation matrix for the camera
// position
	mt_inv.set(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cam.pos.x, -cam.pos.y, -cam.pos.z, 1);

	// step 2: create the inverse rotation sequence for the camera
	// remember either the transpose of the normal rotation matrix or
	// plugging negative values into each of the rotations will result
	// in an inverse matrix

	// first compute all 3 rotation matrices

	// extract out euler angles
	float theta_x = cam.dir.x;
	float theta_y = cam.dir.y;
	float theta_z = cam.dir.z;

	// compute the sine and cosine of the angle x
	float cos_theta = CosFast(theta_x);  // no change since cos(-x) = cos(x)
	float sin_theta = -SinFast(theta_x); // sin(-x) = -sin(x)

	// set the matrix up 
	mx_inv.set(1, 0, 0, 0,
		0, cos_theta, sin_theta, 0,
		0, -sin_theta, cos_theta, 0,
		0, 0, 0, 1);

	// compute the sine and cosine of the angle y
	cos_theta = CosFast(theta_y);  // no change since cos(-x) = cos(x)
	sin_theta = -SinFast(theta_y); // sin(-x) = -sin(x)

	// set the matrix up 
	my_inv.set(cos_theta, 0, -sin_theta, 0,
		0, 1, 0, 0,
		sin_theta, 0, cos_theta, 0,
		0, 0, 0, 1);

	// compute the sine and cosine of the angle z
	cos_theta = CosFast(theta_z);  // no change since cos(-x) = cos(x)
	sin_theta = -SinFast(theta_z); // sin(-x) = -sin(x)

	// set the matrix up 
	mz_inv.set(cos_theta, sin_theta, 0, 0,
		-sin_theta, cos_theta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	// now compute inverse camera rotation sequence
	switch (cam_rot_seq)
	{
	case CAM_ROT_SEQ_XYZ:
	{
		Mul(mx_inv, my_inv, mtmp);
		Mul(mtmp, mz_inv, mrot);
	} break;

	case CAM_ROT_SEQ_YXZ:
	{
		Mul(my_inv, mx_inv, mtmp);
		Mul(mtmp, mz_inv, mrot);
	} break;

	case CAM_ROT_SEQ_XZY:
	{
		Mul(mx_inv, mz_inv, mtmp);
		Mul(mtmp, my_inv, mrot);
	} break;

	case CAM_ROT_SEQ_YZX:
	{
		Mul(my_inv, mz_inv, mtmp);
		Mul(mtmp, mx_inv, mrot);
	} break;

	case CAM_ROT_SEQ_ZYX:
	{
		Mul(mz_inv, my_inv, mtmp);
		Mul(mtmp, mx_inv, mrot);
	} break;

	case CAM_ROT_SEQ_ZXY:
	{
		Mul(mz_inv, mx_inv, mtmp);
		Mul(mtmp, my_inv, mrot);

	} break;

	default: break;
	}

	Mul(mt_inv, mrot, cam.mcam);
}

void BuildUVN(CAM4DV1& cam, int mode)
{
	// this creates a camera matrix based on a look at vector n,
	// look up vector v, and a look right (or left) u
	// and stores it in the sent camera object, all values are
	// extracted out of the camera object itself
	// mode selects how uvn is computed
	// UVN_MODE_SIMPLE - low level simple model, use the target and view reference point
	// UVN_MODE_SPHERICAL - spherical mode, the x,y components will be used as the
	//     elevation and heading of the view vector respectively
	//     along with the view reference point as the position
	//     as usual

	MATRIX4X4 mt_inv,  // inverse camera translation matrix
		mt_uvn,  // the final uvn matrix
		mtmp;    // temporary working matrix

// step 1: create the inverse translation matrix for the camera
// position
	mt_inv.set(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cam.pos.x, -cam.pos.y, -cam.pos.z, 1);


	// step 2: determine how the target point will be computed
	if (mode == UVN_MODE_SPHERICAL)
	{
		// extract elevation and heading 
		float phi = cam.dir.x; // elevation   仰角  与 y 轴的夹角
		float theta = cam.dir.y; // heading

		// compute trig functions once
		float sin_phi = SinFast(phi);
		float cos_phi = CosFast(phi);

		float sin_theta = SinFast(theta);
		float cos_theta = CosFast(theta);

		// now compute the target point on a unit sphere x,y,z
		cam.target.x = -1 * sin_phi * sin_theta;
		cam.target.y = 1 * cos_phi;
		cam.target.z = 1 * sin_phi * cos_theta;
	}

	// at this point, we have the view reference point, the target and that's
	// all we need to recompute u,v,n
	// Step 1: n = <target position - view reference point>
	cam.n.set(cam.pos, cam.target);

	// Step 2: Let v = <0,1,0>
	cam.v.set(0, 1, 0);

	// Step 3: u = (v x n)
	Cross(cam.v, cam.n, cam.u);

	// Step 4: v = (n x u)
	Cross(cam.n, cam.u, cam.v);

	// Step 5: normalize all vectors
	Normalize(cam.u);
	Normalize(cam.v);
	Normalize(cam.n);


	// build the UVN matrix by placing u,v,n as the columns of the matrix
	mt_uvn.set(
		cam.u.x, cam.v.x, cam.n.x, 0,
		cam.u.y, cam.v.y, cam.n.y, 0,
		cam.u.z, cam.v.z, cam.n.z, 0,
		0, 0, 0, 1);

	//觉得需要 inverse mt_uvn 才对
	// now multiply the translation matrix and the uvn matrix and store in the 
	// final camera matrix mcam
	Mul(mt_inv, mt_uvn, cam.mcam);
}


void WorldToCamera(OBJECT4DV1& obj, CAM4DV1& cam)
{
	for (int vertex = 0; vertex < obj.num_vertices; vertex++)
	{
		VECTOR4D presult;
		Mul(obj.vlist_trans[vertex], cam.mcam, presult);
		obj.vlist_trans[vertex] = presult;
	}
}


//在物体剔除，局部到世界坐标变换，背景消除之后进行
void WorldToCamera(RENDERLIST4DV1& rend_list, CAM4DV1& cam)
{
	for (int poly = 0; poly < rend_list.num_polys; poly++)
	{
		POLYF4DV1* curr_poly = rend_list.poly_ptrs[poly];
		if (IsInvalidPoly(curr_poly))
			continue;
		for (int vertex = 0; vertex < 3; vertex++)
		{
			VECTOR4D presult;
			Mul(curr_poly->tvlist[vertex], cam.mcam, presult);
			curr_poly->tvlist[vertex] = presult;
		}
	}
}


/// <summary>
/// 物体剔除
/// </summary>
/// <param name="cull_flags">
/// 可以 or 运算
/// </param>
bool Cull(OBJECT4DV1& obj, const CAM4DV1& cam, int cull_flags)
{
	// step 1: transform the center of the object's bounding
	// sphere into camera space

	VECTOR4D sphere_pos; // hold result of transforming center of bounding sphere

	// transform point
	Mul(obj.world_pos, cam.mcam, sphere_pos);

	// step 2:  based on culling flags remove the object
	if (cull_flags & CULL_OBJECT_Z_PLANE)
	{
		// cull only based on z clipping planes

		// test far plane
		if (((sphere_pos.z - obj.max_radius) > cam.far_clip_z) ||
			((sphere_pos.z + obj.max_radius) < cam.near_clip_z))
		{
			SET_BIT(obj.state, OBJECT4DV1_STATE_CULLED);
			return(true);
		}
	}

	if (cull_flags & CULL_OBJECT_X_PLANE)
	{
		// test the the right and left clipping planes against the leftmost and rightmost
		// points of the bounding sphere
		float z_test = (0.5) * cam.viewplane_width * sphere_pos.z / cam.view_dist;

		if (((sphere_pos.x - obj.max_radius) > z_test) || // right side
			((sphere_pos.x + obj.max_radius) < -z_test))  // left side, note sign change
		{
			SET_BIT(obj.state, OBJECT4DV1_STATE_CULLED);
			return(true);
		}
	}

	if (cull_flags & CULL_OBJECT_Y_PLANE)
	{
		// test the the top and bottom clipping planes against the bottommost and topmost
		// points of the bounding sphere
		float z_test = (0.5) * cam.viewplane_height * sphere_pos.z / cam.view_dist;

		if (((sphere_pos.y - obj.max_radius) > z_test) || // top side
			((sphere_pos.y + obj.max_radius) < -z_test))  // bottom side, note sign change
		{
			SET_BIT(obj.state, OBJECT4DV1_STATE_CULLED);
			return(true);
		}
	}
	// return failure to cull
	return(false);

}


/// <summary>
/// 主要是重置状态，为变换做准备   每帧都需要
/// </summary>
void Reset(OBJECT4DV1& obj)
{
	// reset object's culled flag
	RESET_BIT(obj.state, OBJECT4DV1_STATE_CULLED);

	// now the clipped and backface flags for the polygons 
	for (int poly = 0; poly < obj.num_polys; poly++)
	{
		POLY4DV1& curr_poly = obj.plist[poly];

		if (!(curr_poly.state & POLY4DV1_STATE_ACTIVE))
			continue;

		// reset clipped and backface flags
		RESET_BIT(curr_poly.state, POLY4DV1_STATE_CLIPPED);
		RESET_BIT(curr_poly.state, POLY4DV1_STATE_BACKFACE);

	}
}


/// <summary>
/// 只对单面多边形有效，通过标记状态达到目的
/// </summary>
/// <param name="cam"></param>
void RemoveBackfaces(OBJECT4DV1& obj, const CAM4DV1& cam)
{
	// test if the object is culled
	if (obj.state & OBJECT4DV1_STATE_CULLED)
		return;

	// process each poly in mesh
	for (int poly = 0; poly < obj.num_polys; poly++)
	{
		// acquire polygon
		POLY4DV1& curr_poly = obj.plist[poly];
		if (!(curr_poly.state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly.state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly.attr & POLY4DV1_ATTR_2SIDED) ||
			(curr_poly.state & POLY4DV1_STATE_BACKFACE))
			continue;

		// extract vertex indices into master list, rember the polygons are 
		// NOT self contained, but based on the vertex list stored in the object
		// itself
		int vindex_0 = curr_poly.vert[0];
		int vindex_1 = curr_poly.vert[1];
		int vindex_2 = curr_poly.vert[2];

		// we will use the transformed polygon vertex list since the backface removal
		// only makes sense at the world coord stage further of the pipeline 

		// we need to compute the normal of this polygon face, and recall
		// that the vertices are in cw order, u = p0.p1, v=p0.p2, n=uxv
		VECTOR4D u, v, n;

		// build u, v
		u.set(obj.vlist_trans[vindex_0], obj.vlist_trans[vindex_1]);
		v.set(obj.vlist_trans[vindex_0], obj.vlist_trans[vindex_2]);

		// compute cross product
		Cross(u, v, n);

		// now create eye vector to viewpoint
		VECTOR4D view;
		view.set(obj.vlist_trans[vindex_0], cam.pos);

		// 角度相差 90度之上就是背面了，看不见了
		float dp = Dot(n, view);

		// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
		if (dp <= 0.0) SET_BIT(curr_poly.state, POLY4DV1_STATE_BACKFACE);
	}
}


void RemoveBackfaces(RENDERLIST4DV1& rend_list, CAM4DV1& cam)
{
	for (int poly = 0; poly < rend_list.num_polys; poly++)
	{
		POLYF4DV1* curr_poly = rend_list.poly_ptrs[poly];
		if ((curr_poly == NULL) || !(curr_poly->state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly->state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly->attr & POLY4DV1_ATTR_2SIDED) ||
			(curr_poly->state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

			// we need to compute the normal of this polygon face, and recall
			// that the vertices are in cw order, u = p0->p1, v=p0.p2, n=uxv
		VECTOR4D u, v, n;

		// build u, v
		u.set(curr_poly->tvlist[0], curr_poly->tvlist[1]);
		v.set(curr_poly->tvlist[0], curr_poly->tvlist[2]);

		// compute cross product
		Cross(u, v, n);

		// now create eye vector to viewpoint
		VECTOR4D view;
		view.set(curr_poly->tvlist[0], cam.pos);

		// and finally, compute the dot product
		float dp = Dot(n, view);

		// if the sign is > 0 then visible, 0 = scathing, < 0 invisible
		if (dp <= 0.0) SET_BIT(curr_poly->state, POLY4DV1_STATE_BACKFACE);
	}
}


void CameraToPerspective(OBJECT4DV1& obj, const CAM4DV1& cam)
{
	// this is one way to do it, you might instead transform
	// the global list of polygons in the render list since you 
	// are guaranteed that those polys represent geometry that 
	// has passed thru backfaces culling (if any)
	// finally this function is really for experimental reasons only
	// you would probably never let an object stay intact this far down
	// the pipeline, since it's probably that there's only a single polygon
	// that is visible! But this function has to transform the whole mesh!

	for (int vertex = 0; vertex < obj.num_vertices; vertex++)
	{
		float z = obj.vlist_trans[vertex].z;

		// cam.aspect_ratio 可以看作是在透视变换的基础上进行了拉伸
		// 目的是抵消透视平面到屏幕平面的拉伸作用
		obj.vlist_trans[vertex].x = cam.view_dist * obj.vlist_trans[vertex].x / z;
		obj.vlist_trans[vertex].y = cam.view_dist * obj.vlist_trans[vertex].y * cam.aspect_ratio / z;
		// z = z, so no change
		//同一条直线上的点变成了相同的 x y ，而 z 保留了
		//也就是斜线变成了平行于 z 轴的直线
		//于是视点棱锥形状变成了立方形状
	}
}

void ConvertFromHomogeneous4D(OBJECT4DV1& obj)
{
	for (int vertex = 0; vertex < obj.num_vertices; vertex++)
	{
		obj.vlist_trans[vertex].divByW();
	}
}

void CameraToPerspective(RENDERLIST4DV1& rend_list, const CAM4DV1& cam)
{
	// NOTE: this is not a matrix based function
	// this function transforms each polygon in the global render list
	// into perspective coordinates, based on the 
	// sent camera object, 
	// you would use this function instead of the object based function
	// if you decided earlier in the pipeline to turn each object into 
	// a list of polygons and then add them to the global render list

	for (int poly = 0; poly < rend_list.num_polys; poly++)
	{
		// acquire current polygon
		POLYF4DV1* curr_poly = rend_list.poly_ptrs[poly];

		if (IsInvalidPoly(curr_poly))
			continue;
		for (int vertex = 0; vertex < 3; vertex++)
		{
			float z = curr_poly->tvlist[vertex].z;

			// transform the vertex by the view parameters in the camera
			curr_poly->tvlist[vertex].x = cam.view_dist * curr_poly->tvlist[vertex].x / z;
			curr_poly->tvlist[vertex].y = cam.view_dist * curr_poly->tvlist[vertex].y * cam.aspect_ratio / z;
			// z = z, so no change
		}
	}
}

void ConvertFromHomogeneous4D(RENDERLIST4DV1& rend_list)
{
	for (int poly = 0; poly < rend_list.num_polys; poly++)
	{
		POLYF4DV1* curr_poly = rend_list.poly_ptrs[poly];
		if (IsInvalidPoly(curr_poly))
			continue;

		for (int vertex = 0; vertex < 3; vertex++)
		{
			curr_poly->tvlist[vertex].divByW();
		}
	}
}

void PerspectiveToScreen(OBJECT4DV1& obj, CAM4DV1& cam)
{
	// this is one way to do it, you might instead transform
	// the global list of polygons in the render list since you 
	// are guaranteed that those polys represent geometry that 
	// has passed thru backfaces culling (if any)
	// finally this function is really for experimental reasons only
	// you would probably never let an object stay intact this far down
	// the pipeline, since it's probably that there's only a single polygon
	// that is visible! But this function has to transform the whole mesh!
	// this function would be called after a perspective
	// projection was performed on the object


	float alpha = (0.5 * cam.viewport_width - 0.5);
	float beta = (0.5 * cam.viewport_height - 0.5);

	for (int vertex = 0; vertex < obj.num_vertices; vertex++)
	{
		// assumes the vertex is in perspective normalized coords from -1 to 1
		// on each axis, simple scale them to viewport and invert y axis and project
		// to screen

		// x y 面前有因子，说明有拉伸，而且比值不为1 ，说明有各项异性的拉伸
		// 透视的拉伸因子会抵消这个
		obj.vlist_trans[vertex].x = alpha + alpha * obj.vlist_trans[vertex].x;
		obj.vlist_trans[vertex].y = beta - beta * obj.vlist_trans[vertex].y;
	}
}

void PerspectiveToScreen(RENDERLIST4DV1& rend_list, const CAM4DV1& cam)
{
	// NOTE: this is not a matrix based function
	// this function transforms the perspective coordinates of the render
	// list into screen coordinates, based on the sent viewport in the camera
	// assuming that the viewplane coordinates were normalized
	// you would use this function instead of the object based function
	// if you decided earlier in the pipeline to turn each object into 
	// a list of polygons and then add them to the global render list
	// you would only call this function if you previously performed
	// a normalized perspective transform
	for (int poly = 0; poly < rend_list.num_polys; poly++)
	{
		POLYF4DV1* curr_poly = rend_list.poly_ptrs[poly];

		if (IsInvalidPoly(curr_poly))
			continue;

		float alpha = (0.5 * cam.viewport_width - 0.5);
		float beta = (0.5 * cam.viewport_height - 0.5);

		for (int vertex = 0; vertex < 3; vertex++)
		{
			curr_poly->tvlist[vertex].x = alpha + alpha * curr_poly->tvlist[vertex].x;
			curr_poly->tvlist[vertex].y = beta - beta * curr_poly->tvlist[vertex].y;
		}
	}
}

void CameraToPerspectiveScreen(OBJECT4DV1& obj, const CAM4DV1& cam)
{
	// NOTE: this is not a matrix based function
	// this function transforms the camera coordinates of an object
	// into Screen scaled perspective coordinates, based on the 
	// sent camera object, that is, view_dist_h and view_dist_v 
	// should be set to cause the desired (width X height)
	// projection of the vertices, but the function totally 
	// disregards the polygons themselves,
	// it only works on the vertices in the vlist_trans[] list
	// this is one way to do it, you might instead transform
	// the global list of polygons in the render list since you 
	// are guaranteed that those polys represent geometry that 
	// has passed thru backfaces culling (if any)
	// finally this function is really for experimental reasons only
	// you would probably never let an object stay intact this far down
	// the pipeline, since it's probably that there's only a single polygon
	// that is visible! But this function has to transform the whole mesh!
	// finally, the function also inverts the y axis, so the coordinates
	// generated from this function ARE screen coordinates and ready for
	// rendering

	float alpha = (0.5 * cam.viewport_width - 0.5);
	float beta = (0.5 * cam.viewport_height - 0.5);

	for (int vertex = 0; vertex < obj.num_vertices; vertex++)
	{
		float z = obj.vlist_trans[vertex].z;

		// transform the vertex by the view parameters in the camera
		obj.vlist_trans[vertex].x = cam.view_dist * obj.vlist_trans[vertex].x / z;
		obj.vlist_trans[vertex].y = cam.view_dist * obj.vlist_trans[vertex].y / z;
		// z = z, so no change

		// now the coordinates are in the range x:(-viewport_width/2 to viewport_width/2)
		// and y:(-viewport_height/2 to viewport_height/2), thus we need a translation and
		// since the y-axis is inverted, we need to invert y to complete the screen 
		// transform:
		obj.vlist_trans[vertex].x = obj.vlist_trans[vertex].x + alpha;
		obj.vlist_trans[vertex].y = -obj.vlist_trans[vertex].y + beta;
	}
}

void CameraToPerspectiveScreen(RENDERLIST4DV1& rend_list, const CAM4DV1& cam)
{
	// NOTE: this is not a matrix based function
	// this function transforms the camera coordinates of an object
	// into Screen scaled perspective coordinates, based on the 
	// sent camera object, that is, view_dist_h and view_dist_v 
	// should be set to cause the desired (viewport_width X viewport_height)
	// it only works on the vertices in the tvlist[] list
	// finally, the function also inverts the y axis, so the coordinates
	// generated from this function ARE screen coordinates and ready for
	// rendering

	for (int poly = 0; poly < rend_list.num_polys; poly++)
	{
		POLYF4DV1* curr_poly = rend_list.poly_ptrs[poly];

		if (IsInvalidPoly(curr_poly))
			continue;

		float alpha = (0.5 * cam.viewport_width - 0.5);
		float beta = (0.5 * cam.viewport_height - 0.5);

		for (int vertex = 0; vertex < 3; vertex++)
		{
			float z = curr_poly->tvlist[vertex].z;

			// transform the vertex by the view parameters in the camera
			curr_poly->tvlist[vertex].x = cam.view_dist * curr_poly->tvlist[vertex].x / z;
			curr_poly->tvlist[vertex].y = cam.view_dist * curr_poly->tvlist[vertex].y / z;
			// z = z, so no change

			// now the coordinates are in the range x:(-viewport_width/2 to viewport_width/2)
			// and y:(-viewport_height/2 to viewport_height/2), thus we need a translation and
			// since the y-axis is inverted, we need to invert y to complete the screen 
			// transform:
			curr_poly->tvlist[vertex].x = curr_poly->tvlist[vertex].x + alpha;
			curr_poly->tvlist[vertex].y = -curr_poly->tvlist[vertex].y + beta;

		}
	}
}


void Scale(OBJECT4DV1& obj, const VECTOR4D& vs)
{
	// NOTE: Not matrix based
	// this function scales and object without matrices 
	// modifies the object's local vertex list 
	// additionally the radii is updated for the object

	// for each vertex in the mesh scale the local coordinates by
	// vs on a componentwise basis, that is, sx, sy, sz
	for (int vertex = 0; vertex < obj.num_vertices; vertex++)
	{
		obj.vlist_local[vertex].x *= vs.x;
		obj.vlist_local[vertex].y *= vs.y;
		obj.vlist_local[vertex].z *= vs.z;
		// leave w unchanged, always equal to 1
	} 

// now since the object is scaled we have to do something with 
// the radii calculation, but we don't know how the scaling
// factors relate to the original major axis of the object,
// therefore for scaling factors all ==1 we will simple multiply
// which is correct, but for scaling factors not equal to 1, we
// must take the largest scaling factor and use it to scale the
// radii with since it's the worst case scenario of the new max and
// average radii

	// find max scaling factor
	float scale = max(vs.x, vs.y); scale = max(scale, vs.z);

	// now scale
	obj.max_radius *= scale;
	obj.avg_radius *= scale;

} 


void BuildXYZRotation(const float theta_x, const float theta_y,const float theta_z,MATRIX4X4& mrot) 
{
	// this helper function takes a set if euler angles and computes
	// a rotation matrix from them, usefull for object and camera
	// work, also  we will do a little testing in the function to determine
	// the rotations that need to be performed, since there's no
	// reason to perform extra matrix multiplies if the angles are
	// zero!

	MATRIX4X4 mx, my, mz, mtmp;       // working matrices
	float sin_theta = 0, cos_theta = 0;   // used to initialize matrices
	int rot_seq = 0;                  // 1 for x, 2 for y, 4 for z

	// step 0: fill in with identity matrix
	Identity(mrot);

	// step 1: based on zero and non-zero rotation angles, determine
	// rotation sequence
	if (fabs(theta_x) > EPSILON_E5) // x
		rot_seq = rot_seq | 1;

	if (fabs(theta_y) > EPSILON_E5) // y
		rot_seq = rot_seq | 2;

	if (fabs(theta_z) > EPSILON_E5) // z
		rot_seq = rot_seq | 4;

	// now case on sequence
	switch (rot_seq)
	{
	case 0: // no rotation
	{
		// what a waste!
		return;
	} break;

	case 1: // x rotation
	{
		// compute the sine and cosine of the angle
		cos_theta = CosFast(theta_x);
		sin_theta = SinFast(theta_x);

		// set the matrix up 
		mx.set(1, 0, 0, 0,
			0, cos_theta, sin_theta, 0,
			0, -sin_theta, cos_theta, 0,
			0, 0, 0, 1);

		// that's it, copy to output matrix
		mrot = mx;

		return;

	} break;

	case 2: // y rotation
	{
		// compute the sine and cosine of the angle
		cos_theta = CosFast(theta_y);
		sin_theta = SinFast(theta_y);

		// set the matrix up 
		my.set(cos_theta, 0, -sin_theta, 0,
			0, 1, 0, 0,
			sin_theta, 0, cos_theta, 0,
			0, 0, 0, 1);


		// that's it, copy to output matrix
		mrot = my;
		return;

	} break;

	case 3: // xy rotation
	{
		// compute the sine and cosine of the angle for x
		cos_theta = CosFast(theta_x);
		sin_theta = SinFast(theta_x);

		// set the matrix up 
		mx.set(1, 0, 0, 0,
			0, cos_theta, sin_theta, 0,
			0, -sin_theta, cos_theta, 0,
			0, 0, 0, 1);

		// compute the sine and cosine of the angle for y
		cos_theta = CosFast(theta_y);
		sin_theta = SinFast(theta_y);

		// set the matrix up 
		my.set(cos_theta, 0, -sin_theta, 0,
			0, 1, 0, 0,
			sin_theta, 0, cos_theta, 0,
			0, 0, 0, 1);

		// concatenate matrices 
		Mul(mx, my, mrot);
		return;

	} break;

	case 4: // z rotation
	{
		// compute the sine and cosine of the angle
		cos_theta = CosFast(theta_z);
		sin_theta = SinFast(theta_z);

		// set the matrix up 
		mz.set( cos_theta, sin_theta, 0, 0,
			-sin_theta, cos_theta, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);


		// that's it, copy to output matrix
		mrot = mz;
		return;

	} break;

	case 5: // xz rotation
	{
		// compute the sine and cosine of the angle x
		cos_theta = CosFast(theta_x);
		sin_theta = SinFast(theta_x);

		// set the matrix up 
		mx.set(1, 0, 0, 0,
			0, cos_theta, sin_theta, 0,
			0, -sin_theta, cos_theta, 0,
			0, 0, 0, 1);

		// compute the sine and cosine of the angle z
		cos_theta = CosFast(theta_z);
		sin_theta = SinFast(theta_z);

		// set the matrix up 
		mz.set(cos_theta, sin_theta, 0, 0,
			-sin_theta, cos_theta, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

		// concatenate matrices 
		Mul(mx, mz, mrot);
		return;

	} break;

	case 6: // yz rotation
	{
		// compute the sine and cosine of the angle y
		cos_theta = CosFast(theta_y);
		sin_theta = SinFast(theta_y);

		// set the matrix up 
		my.set(cos_theta, 0, -sin_theta, 0,
			0, 1, 0, 0,
			sin_theta, 0, cos_theta, 0,
			0, 0, 0, 1);

		// compute the sine and cosine of the angle z
		cos_theta = CosFast(theta_z);
		sin_theta = SinFast(theta_z);

		// set the matrix up 
		mz.set(cos_theta, sin_theta, 0, 0,
			-sin_theta, cos_theta, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

		// concatenate matrices 
		Mul(my, mz, mrot);
		return;

	} break;

	case 7: // xyz rotation
	{
		// compute the sine and cosine of the angle x
		cos_theta = CosFast(theta_x);
		sin_theta = SinFast(theta_x);

		// set the matrix up 
		mx.set(1, 0, 0, 0,
			0, cos_theta, sin_theta, 0,
			0, -sin_theta, cos_theta, 0,
			0, 0, 0, 1);

		// compute the sine and cosine of the angle y
		cos_theta = CosFast(theta_y);
		sin_theta = SinFast(theta_y);

		// set the matrix up 
		my.set(cos_theta, 0, -sin_theta, 0,
			0, 1, 0, 0,
			sin_theta, 0, cos_theta, 0,
			0, 0, 0, 1);

		// compute the sine and cosine of the angle z
		cos_theta = CosFast(theta_z);
		sin_theta = SinFast(theta_z);

		// set the matrix up 
		mz.set(cos_theta, sin_theta, 0, 0,
			-sin_theta, cos_theta, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

		// concatenate matrices, watch order!
		Mul(mx, my, mtmp);
		Mul(mtmp, mz, mrot);

	} break;

	default: break;

	} 
}                                   



void RotateXYZ(OBJECT4DV1& obj,const float theta_x, const float theta_y, const float theta_z)
{
	// this function rotates and object parallel to the
	// XYZ axes in that order or a subset thereof, without
	// matrices (at least externally sent)
	// modifies the object's local vertex list 
	// additionally it rotates the unit directional vectors
	// that track the objects orientation, also note that each
	// time this function is called it calls the rotation generation
	// function, this is wastefull if a number of object are being rotated
	// by the same matrix, therefore, if that's the case, then generate the
	// rotation matrix, store it, and call the general Transform_OBJECT4DV1()
	// with the matrix

	MATRIX4X4 mrot; // used to store generated rotation matrix

	// generate rotation matrix, no way to avoid rotation with a matrix
	// too much math to do manually!
	BuildXYZRotation(theta_x, theta_y, theta_z, mrot);

	// now simply rotate each point of the mesh in local/model coordinates
	for (int vertex = 0; vertex < obj.num_vertices; vertex++)
	{
		VECTOR4D presult; 
		Mul(obj.vlist_local[vertex], mrot, presult);
		obj.vlist_local[vertex]=presult;
	} 

// now rotate orientation basis for object
	VECTOR4D vresult; // use to rotate each orientation vector axis

	// rotate ux of basis
	Mul(obj.ux, mrot, vresult);
	obj.ux=vresult;

	// rotate uy of basis
	Mul(obj.uy, mrot, vresult);
	obj.uy=vresult;

	// rotate uz of basis
	Mul(obj.uz, mrot, vresult);
	obj.uz=vresult;

} 


bool Insert(RENDERLIST4DV1& rend_list,const POLY4DV1& poly)
{
	// converts the sent POLY4DV1 into a FACE4DV1 and inserts it
	// into the render list

	// step 0: are we full?
	if (rend_list.num_polys >= RENDERLIST4DV1_MAX_POLYS)
		return(false);

	// step 1: copy polygon into next opening in polygon render list

	// point pointer to polygon structure
	rend_list.poly_ptrs[rend_list.num_polys] = &rend_list.poly_data[rend_list.num_polys];

	// copy fields
	rend_list.poly_data[rend_list.num_polys].state = poly.state;
	rend_list.poly_data[rend_list.num_polys].attr = poly.attr;
	rend_list.poly_data[rend_list.num_polys].color = poly.color;

	// now copy vertices, be careful! later put a loop, but for now
	// know there are 3 vertices always!
	rend_list.poly_data[rend_list.num_polys].tvlist[0]=poly.vlist[poly.vert[0]];

	rend_list.poly_data[rend_list.num_polys].tvlist[1]=poly.vlist[poly.vert[1]];

	rend_list.poly_data[rend_list.num_polys].tvlist[2]=poly.vlist[poly.vert[2]];

	// and copy into local vertices too
	rend_list.poly_data[rend_list.num_polys].vlist[0]=poly.vlist[poly.vert[0]];

	rend_list.poly_data[rend_list.num_polys].vlist[1]=poly.vlist[poly.vert[1]];

	rend_list.poly_data[rend_list.num_polys].vlist[2]=poly.vlist[poly.vert[2]];

	// now the polygon is loaded into the next free array position, but
	// we need to fix up the links

	// test if this is the first entry
	if (rend_list.num_polys == 0)
	{
		// set pointers to null, could loop them around though to self
		rend_list.poly_data[0].next = NULL;
		rend_list.poly_data[0].prev = NULL;
	} 
	else
	{
		// first set this node to point to previous node and next node (null)
		rend_list.poly_data[rend_list.num_polys].next = NULL;
		rend_list.poly_data[rend_list.num_polys].prev =
			&rend_list.poly_data[rend_list.num_polys - 1];

		// now set previous node to point to this node
		rend_list.poly_data[rend_list.num_polys - 1].next =
			&rend_list.poly_data[rend_list.num_polys];
	} 


 // increment number of polys in list
	rend_list.num_polys++;

	// return successful insertion
	return(true);
} 


bool Insert(RENDERLIST4DV1& rend_list,const POLYF4DV1& poly)
{
	// inserts the sent polyface POLYF4DV1 into the render list

	// step 0: are we full?
	if (rend_list.num_polys >= RENDERLIST4DV1_MAX_POLYS)
		return(false);

	// step 1: copy polygon into next opening in polygon render list

	// point pointer to polygon structure
	rend_list.poly_ptrs[rend_list.num_polys] = &rend_list.poly_data[rend_list.num_polys];

	// copy face right into array, thats it
	memcpy((void*)&rend_list.poly_data[rend_list.num_polys], (void*)&poly, sizeof(POLYF4DV1));

	// now the polygon is loaded into the next free array position, but
	// we need to fix up the links
	// test if this is the first entry
	if (rend_list.num_polys == 0)
	{
		// set pointers to null, could loop them around though to self
		rend_list.poly_data[0].next = NULL;
		rend_list.poly_data[0].prev = NULL;
	} // end if
	else
	{
		// first set this node to point to previous node and next node (null)
		rend_list.poly_data[rend_list.num_polys].next = NULL;
		rend_list.poly_data[rend_list.num_polys].prev =
			&rend_list.poly_data[rend_list.num_polys - 1];

		// now set previous node to point to this node
		rend_list.poly_data[rend_list.num_polys - 1].next =
			&rend_list.poly_data[rend_list.num_polys];
	} 

 // increment number of polys in list
	rend_list.num_polys++;

	return(true);
} 


bool Insert(RENDERLIST4DV1& rend_list,OBJECT4DV1& obj,const int insert_local)
{
	// converts the entire object into a face list and then inserts
	// the visible, active, non-clipped, non-culled polygons into
	// the render list, also note the flag insert_local control 
	// whether or not the vlist_local or vlist_trans vertex list
	// is used, thus you can insert an object "raw" totally untranformed
	// if you set insert_local to 1, default is 0, that is you would
	// only insert an object after at least the local to world transform

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

		// first is this polygon even visible?
		if (!(curr_poly.state & POLY4DV1_STATE_ACTIVE) ||
			(curr_poly.state & POLY4DV1_STATE_CLIPPED) ||
			(curr_poly.state & POLY4DV1_STATE_BACKFACE))
			continue; // move onto next poly

			// override vertex list polygon refers to
			// the case that you want the local coords used
			// first save old pointer
		VECTOR4D* vlist_old = curr_poly.vlist;

		if (insert_local) curr_poly.vlist = obj.vlist_local;
		else curr_poly.vlist = obj.vlist_trans;

		// now insert this polygon
		if (!Insert(rend_list, curr_poly))
		{
			// fix vertex list pointer
			curr_poly.vlist = vlist_old;

			// the whole object didn't fit!
			return(false);
		} 
	 // fix vertex list pointer
		curr_poly.vlist = vlist_old;

	} 
	return(true);
}



//没有考虑隐藏线消除
void DrawWire(OBJECT4DV1& obj)
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

		// draw the lines now
		GRAPHIC::DrawLine(obj.vlist_trans[vindex_0].x, obj.vlist_trans[vindex_0].y,
			obj.vlist_trans[vindex_1].x, obj.vlist_trans[vindex_1].y,
			obj.plist[poly].color);

		GRAPHIC::DrawLine(obj.vlist_trans[vindex_1].x, obj.vlist_trans[vindex_1].y,
			obj.vlist_trans[vindex_2].x, obj.vlist_trans[vindex_2].y,
			obj.plist[poly].color);

		GRAPHIC::DrawLine(obj.vlist_trans[vindex_2].x, obj.vlist_trans[vindex_2].y,
			obj.vlist_trans[vindex_0].x, obj.vlist_trans[vindex_0].y,
			obj.plist[poly].color);
	} 
} 

void DrawWire(RENDERLIST4DV1& rend_list)
{
	// this function "executes" the render list or in other words
	// draws all the faces in the list in wire frame 16bit mode
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

		 // draw the triangle edge one, note that clipping was already set up
		 // by 2D initialization, so line clipper will clip all polys out
		 // of the 2D screen/window boundary
		GRAPHIC::DrawLine(rend_list.poly_ptrs[poly]->tvlist[0].x,
			rend_list.poly_ptrs[poly]->tvlist[0].y,
			rend_list.poly_ptrs[poly]->tvlist[1].x,
			rend_list.poly_ptrs[poly]->tvlist[1].y,
			rend_list.poly_ptrs[poly]->color);

		GRAPHIC::DrawLine(rend_list.poly_ptrs[poly]->tvlist[1].x,
			rend_list.poly_ptrs[poly]->tvlist[1].y,
			rend_list.poly_ptrs[poly]->tvlist[2].x,
			rend_list.poly_ptrs[poly]->tvlist[2].y,
			rend_list.poly_ptrs[poly]->color);

		GRAPHIC::DrawLine(rend_list.poly_ptrs[poly]->tvlist[2].x,
			rend_list.poly_ptrs[poly]->tvlist[2].y,
			rend_list.poly_ptrs[poly]->tvlist[0].x,
			rend_list.poly_ptrs[poly]->tvlist[0].y,
			rend_list.poly_ptrs[poly]->color);
	} 
}







