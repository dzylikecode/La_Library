#include "La_3DFile.h"
#include "La_3DData.h"
#include "La_Parser.h"
#include "La_3DLight.h"
#include "La_GeometryBase.h"
int Load3DSASC(OBJECT4DV1& obj, const char* filename, VECTOR4D& scale, VECTOR4D& pos, VECTOR4D& rot, int vertex_flags)
{
	// this function loads a 3D Studi .ASC file object in off disk, additionally
	// it allows the caller to scale, position, and rotate the object
	// to save extra calls later for non-dynamic objects
	// create a parser object
	CPARSERV1 parser;

	char seps[16];          // seperators for token scanning
	char token_buffer[256]; // used as working buffer for token
	char* token;            // pointer to next token

	int r, g, b;              // working colors


	// Step 1: clear out the object and initialize it a bit
	memset(&obj, 0, sizeof(OBJECT4DV1));

	// set state of object to active and visible
	obj.state = OBJECT4DV1_STATE_ACTIVE | OBJECT4DV1_STATE_VISIBLE;

	// set position of object is caller requested position
	obj.world_pos.x = pos.x;
	obj.world_pos.y = pos.y;
	obj.world_pos.z = pos.z;
	obj.world_pos.w = pos.w;



	// Step 2: open the file for reading using the parser
	if (!parser.open(filename))
	{
		ERROR_MSG("Couldn't open .ASC file %s.", filename);
		return(0);
	}

	// Step 3: 

	// lets find the name of the object first 
	while (1)
	{
		// get the next line, we are looking for "Named object:"
		if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			ERROR_MSG("Image 'name' not found in .ASC file %s.", filename);
			return(0);
		}

		// check for pattern?  
		if (parser.match(parser.buffer, "['Named'] ['object:']"))
		{
			// at this point we have the string with the name in it, parse it out by finding 
			// name between quotes "name...."
			strcpy(token_buffer, parser.buffer);
			strcpy(seps, "\"");
			strtok(token_buffer, seps);

			// this will be the token between the quotes
			token = strtok(NULL, seps);

			// copy name into structure
			strcpy(obj.name, token);
			ERROR_MSG("\nASC Reader Object Name: %s", obj.name);

			break;
		}
	}

	// step 4: get number of vertices and polys in object
	while (1)
	{
		// get the next line, we are looking for "Tri-mesh, Vertices:" 
		if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			ERROR_MSG("'Tri-mesh' line not found in .ASC file %s.", filename);
			return(0);
		}

		// check for pattern?  
		if (parser.match(parser.buffer, "['Tri-mesh,'] ['Vertices:'] [i] ['Faces:'] [i]"))
		{
			// simply extract the number of vertices and polygons from the pattern matching 
			// output arrays
			obj.num_vertices = parser.pints[0];
			obj.num_polys = parser.pints[1];

			ERROR_MSG("\nASC Reader Num Vertices: %d, Num Polys: %d",
				obj.num_vertices, obj.num_polys);
			break;

		}
	}

	// Step 5: load the vertex list

	// advance parser to vertex list denoted by:
	// "Vertex list:"
	while (1)
	{
		// get next line
		if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			ERROR_MSG("\n'Vertex list:' line not found in .ASC file %s.", filename);
			return(0);
		}

		// check for pattern?  
		if (parser.match(parser.buffer, "['Vertex'] ['list:']"))
		{
			ERROR_MSG("\nASC Reader found vertex list in .ASC file %s.", filename);
			break;
		}
	}

	// now read in vertex list, format:
	// "Vertex: d  X:d.d Y:d.d  Z:d.d"
	for (int vertex = 0; vertex < obj.num_vertices; vertex++)
	{
		// hunt for vertex
		while (1)
		{
			// get the next vertex
			if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				ERROR_MSG("\nVertex list ended abruptly! in .ASC file %s.", filename);
				return(0);
			}

			// strip all ":XYZ", make this easier, note use of input and output as same var, this is legal
			// since the output is guaranteed to be the same length or shorter as the input :)
			StripChars(parser.buffer, parser.buffer, ":XYZ");

			// check for pattern?  
			if (parser.match(parser.buffer, "['Vertex'] [i] [f] [f] [f]"))
			{
				// at this point we have the x,y,z in the the pfloats array locations 0,1,2
				obj.vlist_local[vertex].x = parser.pfloats[0];
				obj.vlist_local[vertex].y = parser.pfloats[1];
				obj.vlist_local[vertex].z = parser.pfloats[2];
				obj.vlist_local[vertex].w = 1;

				// do vertex swapping right here, allow muliple swaps, why not!
				// defines for vertex re-ordering flags

				//#define VERTEX_FLAGS_INVERT_X   1    // inverts the Z-coordinates
				//#define VERTEX_FLAGS_INVERT_Y   2    // inverts the Z-coordinates
				//#define VERTEX_FLAGS_INVERT_Z   4    // inverts the Z-coordinates
				//#define VERTEX_FLAGS_SWAP_YZ    8    // transforms a RHS model to a LHS model
				//#define VERTEX_FLAGS_SWAP_XZ    16   // ???
				//#define VERTEX_FLAGS_SWAP_XY    32
				//#define VERTEX_FLAGS_INVERT_WINDING_ORDER 64  // invert winding order from cw to ccw or ccw to cc

				float temp_f; // used for swapping

				// invert signs?
				if (vertex_flags & VERTEX_FLAGS_INVERT_X)
					obj.vlist_local[vertex].x = -obj.vlist_local[vertex].x;

				if (vertex_flags & VERTEX_FLAGS_INVERT_Y)
					obj.vlist_local[vertex].y = -obj.vlist_local[vertex].y;

				if (vertex_flags & VERTEX_FLAGS_INVERT_Z)
					obj.vlist_local[vertex].z = -obj.vlist_local[vertex].z;

				// swap any axes?
				if (vertex_flags & VERTEX_FLAGS_SWAP_YZ)
					SWAP(obj.vlist_local[vertex].y, obj.vlist_local[vertex].z, temp_f);

				if (vertex_flags & VERTEX_FLAGS_SWAP_XZ)
					SWAP(obj.vlist_local[vertex].x, obj.vlist_local[vertex].z, temp_f);

				if (vertex_flags & VERTEX_FLAGS_SWAP_XY)
					SWAP(obj.vlist_local[vertex].x, obj.vlist_local[vertex].y, temp_f);

				ERROR_MSG("\nVertex %d = %f, %f, %f, %f", vertex,
					obj.vlist_local[vertex].x,
					obj.vlist_local[vertex].y,
					obj.vlist_local[vertex].z,
					obj.vlist_local[vertex].w);

				// scale vertices

				obj.vlist_local[vertex].x *= scale.x;
				obj.vlist_local[vertex].y *= scale.y;
				obj.vlist_local[vertex].z *= scale.z;

				// found vertex, break out of while for next pass
				break;

			}
		}

	}

	// compute average and max radius
	ComputeRadius(obj);

	ERROR_MSG("\nObject average radius = %f, max radius = %f",
		obj.avg_radius, obj.max_radius);

	// step 6: load in the polygons
	// poly list starts off with:
	// "Face list:"
	while (1)
	{
		// get next line
		if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			ERROR_MSG("\n'Face list:' line not found in .ASC file %s.", filename);
			return(0);
		}

		// check for pattern?  
		if (parser.match(parser.buffer, "['Face'] ['list:']"))
		{
			ERROR_MSG("\nASC Reader found face list in .ASC file %s.", filename);
			break;
		}
	}

	// now read each face in format:
	// Face ddd:    A:ddd B:ddd C:ddd AB:1|0 BC:1|0 CA:1|
	// Material:"rdddgdddbddda0"
	// the A, B, C part is vertex 0,1,2 but the AB, BC, CA part
	// has to do with the edges and the vertex ordering  顶点环绕方向
	// the material indicates the color, and has an 'a0' tacked on the end???

	int  poly_surface_desc = 0; // ASC surface descriptor/material in this case
	int  poly_num_verts = 0; // number of vertices for current poly (always 3)
	char tmp_string[8];         // temp string to hold surface descriptor in and
								// test if it need to be converted from hex

	for (int poly = 0; poly < obj.num_polys; poly++)
	{
		// hunt until next face is found
		while (1)
		{
			// get the next polygon face
			if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				ERROR_MSG("\nface list ended abruptly! in .ASC file %s.", filename);
				return(0);
			}

			// strip all ":ABC", make this easier, note use of input and output as same var, this is legal
			// since the output is guaranteed to be the same length or shorter as the input :)
			StripChars(parser.buffer, parser.buffer, ":ABC");

			// check for pattern?  
			if (parser.match(parser.buffer, "['Face'] [i] [i] [i] [i]"))
			{
				// at this point we have the vertex indices in the the pints array locations 1,2,3, 
				// 0 contains the face number

				// insert polygon, check for winding order invert
				if (vertex_flags & VERTEX_FLAGS_INVERT_WINDING_ORDER)
				{
					poly_num_verts = 3;
					obj.plist[poly].vert[0] = parser.pints[3];
					obj.plist[poly].vert[1] = parser.pints[2];
					obj.plist[poly].vert[2] = parser.pints[1];
				}
				else
				{ // leave winding order alone
					poly_num_verts = 3;
					obj.plist[poly].vert[0] = parser.pints[1];
					obj.plist[poly].vert[1] = parser.pints[2];
					obj.plist[poly].vert[2] = parser.pints[3];
				}

				// point polygon vertex list to object's vertex list
				// note that this is redundant since the polylist is contained
				// within the object in this case and its up to the user to select
				// whether the local or transformed vertex list is used when building up
				// polygon geometry, might be a better idea to set to NULL in the context
				// of polygons that are part of an object
				obj.plist[poly].vlist = obj.vlist_local;

				// found the face, break out of while for another pass
				break;

			}
		}

		// hunt until next material for face is found
		while (1)
		{
			// get the next polygon material (the "page xxx" breaks mess everything up!!!)
			if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				ERROR_MSG("\nmaterial list ended abruptly! in .ASC file %s.", filename);
				return(0);
			} // end if

		 // Material:"rdddgdddbddda0"
		 // replace all ':"rgba', make this easier, note use of input and output as same var, this is legal
		 // since the output is guaranteed to be the same length or shorter as the input :)
		 // the result will look like:
		 // "M t ri l   ddd ddd ddd 0" 
		 // which we can parse!
			ReplaceChars(parser.buffer, parser.buffer, ":\"rgba", ' ');

			// check for pattern?  
			if (parser.match(parser.buffer, "[i] [i] [i]"))
			{
				// at this point we have the red, green, and blue components in the the pints array locations 0,1,2, 
				r = parser.pints[0];
				g = parser.pints[1];
				b = parser.pints[2];

				// set all the attributes of polygon as best we can with this format
				// SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_2SIDED);

				// we need to know what color depth we are dealing with, so check
				// the bits per pixel, this assumes that the system has already
				// made the call to DDraw_Init() or set the bit depth

					// cool, 16 bit mode
				SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_RGB24);
				obj.plist[poly].color = RGB_DX(r, g, b);
				ERROR_MSG("\nPolygon 16-bit");

				// for now manually set shading mode
				//SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PURE);
				//SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_GOURAUD);
				//SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_PHONG);
				SET_BIT(obj.plist[poly].attr, POLY4DV1_ATTR_SHADE_MODE_FLAT);

				// set polygon to active
				obj.plist[poly].state = POLY4DV1_STATE_ACTIVE;

				// found the material, break out of while for another pass
				break;

			}
		}

		ERROR_MSG("\nPolygon %d:", poly);
		ERROR_MSG("\nSurface Desc = [RGB]=[%d, %d, %d], vert_indices [%d, %d, %d]",
			r, g, b,
			obj.plist[poly].vert[0],
			obj.plist[poly].vert[1],
			obj.plist[poly].vert[2]);

	}


	return(1);

}



int LoadCOB(OBJECT4DV1& obj, char* filename, VECTOR4D& scale, VECTOR4D& pos, VECTOR4D& rot, int vertex_flags, MATV1* materials, int& num_materials)
{
	// this function loads a Caligari TrueSpace .COB file object in off disk, additionally
	// it allows the caller to scale, position, and rotate the object
	// to save extra calls later for non-dynamic objects, note that this function 
	// works with a OBJECT4DV1 which has no support for textures, or materials, etc, however we will
	// still parse them and get them ready for the next incarnation objects, so we can
	// re-use this code to support those features

	// create a parser object
	CPARSERV1 parser;

	char seps[16];          // seperators for token scanning
	char token_buffer[256]; // used as working buffer for token
	char* token;            // pointer to next token

	int r, g, b;              // working colors

	// cache for texture vertices
	GRAPHIC::VERTEX2D texture_vertices[1024];

	int num_texture_vertices = 0;

	MATRIX4X4 mat_local,  // storage for local transform if user requests it in cob format
		mat_world;  // "   " for local to world " "

// initialize matrices
	Identity(mat_local);
	Identity(mat_world);

	// Step 1: clear out the object and initialize it a bit
	memset(&obj, 0, sizeof(OBJECT4DV1));

	// set state of object to active and visible
	obj.state = OBJECT4DV1_STATE_ACTIVE | OBJECT4DV1_STATE_VISIBLE;

	// set position of object is caller requested position
		// set position of object
	obj.world_pos.x = pos.x;
	obj.world_pos.y = pos.y;
	obj.world_pos.z = pos.z;
	obj.world_pos.w = pos.w;



	// Step 2: open the file for reading using the parser
	if (!parser.open(filename))
	{
		ERROR_MSG("Couldn't open .COB file %s.", filename);
		return(0);
	}

	// Step 3: 

	// lets find the name of the object first 
	while (1)
	{
		// get the next line, we are looking for "Name"
		if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			ERROR_MSG("Image 'name' not found in .COB file %s.", filename);
			return(0);
		} 
	 // check for pattern?  
		if (parser.match(parser.buffer, "['Name'] [s>0]"))
		{
			// name should be in second string variable, index 1
			strcpy(obj.name, parser.pstrings[1]);
			ERROR_MSG("\nCOB Reader Object Name: %s", obj.name);

			break;
		}
	}

	// step 4: get local and world transforms and store them

	// center 0 0 0     局部坐标系
	// x axis 1 0 0
	// y axis 0 1 0
	// z axis 0 0 1

	while (1)
	{
		// get the next line, we are looking for "center"
		if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			ERROR_MSG("Center not found in .COB file %s.", filename);
			return(0);
		} 
	 // check for pattern?  
		if (parser.match(parser.buffer, "['center'] [f] [f] [f]"))
		{
			// the "center" holds the translation factors, so place in
			// last row of homogeneous matrix, note that these are row vectors
			// that we need to drop in each column of matrix
			mat_local.M[3][0] = -parser.pfloats[0]; // center x
			mat_local.M[3][1] = -parser.pfloats[1]; // center y
			mat_local.M[3][2] = -parser.pfloats[2]; // center z

			// ok now, the next 3 lines should be the x,y,z transform vectors
			// so build up   

			// "x axis" 
			parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			parser.match(parser.buffer, "['x'] ['axis'] [f] [f] [f]");

			// place row in x column of transform matrix
			mat_local.M[0][0] = parser.pfloats[0]; // rxx
			mat_local.M[1][0] = parser.pfloats[1]; // rxy
			mat_local.M[2][0] = parser.pfloats[2]; // rxz

			// "y axis" 
			parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			parser.match(parser.buffer, "['y'] ['axis'] [f] [f] [f]");

			// place row in y column of transform matrix
			mat_local.M[0][1] = parser.pfloats[0]; // ryx
			mat_local.M[1][1] = parser.pfloats[1]; // ryy
			mat_local.M[2][1] = parser.pfloats[2]; // ryz

			// "z axis" 
			parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			parser.match(parser.buffer, "['z'] ['axis'] [f] [f] [f]");

			// place row in z column of transform matrix
			mat_local.M[0][2] = parser.pfloats[0]; // rzx
			mat_local.M[1][2] = parser.pfloats[1]; // rzy
			mat_local.M[2][2] = parser.pfloats[2]; // rzz

			ERROR_MSG("Local COB Matrix:\n%s", mat_local);

			break;
		} 
	}

// now "Transform"
	while (1)
	{
		// get the next line, we are looking for "Transform"
		if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			ERROR_MSG("Transform not found in .COB file %s.", filename);
			return(0);
		} 

	 // check for pattern?  
		if (parser.match(parser.buffer, "['Transform']"))
		{

			// "x axis" 
			parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			parser.match(parser.buffer, "[f] [f] [f]");

			// place row in x column of transform matrix
			mat_world.M[0][0] = parser.pfloats[0]; // rxx
			mat_world.M[1][0] = parser.pfloats[1]; // rxy
			mat_world.M[2][0] = parser.pfloats[2]; // rxz

			// "y axis" 
			parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			parser.match(parser.buffer, "[f] [f] [f]");

			// place row in y column of transform matrix
			mat_world.M[0][1] = parser.pfloats[0]; // ryx
			mat_world.M[1][1] = parser.pfloats[1]; // ryy
			mat_world.M[2][1] = parser.pfloats[2]; // ryz

			// "z axis" 
			parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS);
			parser.match(parser.buffer, "[f] [f] [f]");

			// place row in z column of transform matrix
			mat_world.M[0][2] = parser.pfloats[0]; // rzx
			mat_world.M[1][2] = parser.pfloats[1]; // rzy
			mat_world.M[2][2] = parser.pfloats[2]; // rzz

			ERROR_MSG("World COB Matrix:\n%s", mat_world);

			// no need to read in last row, since it's always 0,0,0,1 and we don't use it anyway
			break;

		}
	}
	// step 6: get number of vertices and polys in object
	while (1)
	{
		// get the next line, we are looking for "World Vertices" 
		if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			ERROR_MSG("'World Vertices' line not found in .COB file %s.", filename);
			return(0);
		} 

	 // check for pattern?  
		if (parser.match(parser.buffer, "['World'] ['Vertices'] [i]"))
		{
			// simply extract the number of vertices from the pattern matching 
			// output arrays
			obj.num_vertices = parser.pints[0];

			ERROR_MSG("\nCOB Reader Num Vertices: %d", obj.num_vertices);
			break;

		}
	}

	// Step 7: load the vertex list
	// now read in vertex list, format:
	// "d.d d.d d.d"
	for (int vertex = 0; vertex < obj.num_vertices; vertex++)
	{
		// hunt for vertex
		while (1)
		{
			// get the next vertex
			if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				ERROR_MSG("\nVertex list ended abruptly! in .COB file %s.", filename);
				return(0);
			} // end if

		 // check for pattern?  
			if (parser.match(parser.buffer, "[f] [f] [f]"))
			{
				// at this point we have the x,y,z in the the pfloats array locations 0,1,2
				obj.vlist_local[vertex].x = parser.pfloats[0];
				obj.vlist_local[vertex].y = parser.pfloats[1];
				obj.vlist_local[vertex].z = parser.pfloats[2];
				obj.vlist_local[vertex].w = 1;

				// do vertex swapping right here, allow muliple swaps, why not!
				// defines for vertex re-ordering flags

				//#define VERTEX_FLAGS_INVERT_X   1    // inverts the Z-coordinates
				//#define VERTEX_FLAGS_INVERT_Y   2    // inverts the Z-coordinates
				//#define VERTEX_FLAGS_INVERT_Z   4    // inverts the Z-coordinates
				//#define VERTEX_FLAGS_SWAP_YZ    8    // transforms a RHS model to a LHS model
				//#define VERTEX_FLAGS_SWAP_XZ    16   // ???
				//#define VERTEX_FLAGS_SWAP_XY    32
				//#define VERTEX_FLAGS_INVERT_WINDING_ORDER 64  // invert winding order from cw to ccw or ccw to cc
				//#define VERTEX_FLAGS_TRANSFORM_LOCAL         512   // if file format has local transform then do it!
				//#define VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD  1024  // if file format has local to world then do it!

				VECTOR4D temp_vector; // temp for calculations

				// now apply local and world transformations encoded in COB format
				if (vertex_flags & VERTEX_FLAGS_TRANSFORM_LOCAL)  //局部坐标变换
				{
					Mul(obj.vlist_local[vertex], mat_local, temp_vector);
					obj.vlist_local[vertex] = temp_vector;
				}

				if (vertex_flags & VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD) //世界坐标变换
				{
					Mul(obj.vlist_local[vertex], mat_world, temp_vector);
					obj.vlist_local[vertex] = temp_vector;
				}

				float temp_f; // used for swapping

				// invert signs?
				if (vertex_flags & VERTEX_FLAGS_INVERT_X)
					obj.vlist_local[vertex].x = -obj.vlist_local[vertex].x;

				if (vertex_flags & VERTEX_FLAGS_INVERT_Y)
					obj.vlist_local[vertex].y = -obj.vlist_local[vertex].y;

				if (vertex_flags & VERTEX_FLAGS_INVERT_Z)
					obj.vlist_local[vertex].z = -obj.vlist_local[vertex].z;

				// swap any axes?
				if (vertex_flags & VERTEX_FLAGS_SWAP_YZ)
					SWAP(obj.vlist_local[vertex].y, obj.vlist_local[vertex].z, temp_f);

				if (vertex_flags & VERTEX_FLAGS_SWAP_XZ)
					SWAP(obj.vlist_local[vertex].x, obj.vlist_local[vertex].z, temp_f);

				if (vertex_flags & VERTEX_FLAGS_SWAP_XY)
					SWAP(obj.vlist_local[vertex].x, obj.vlist_local[vertex].y, temp_f);

				// scale vertices
				obj.vlist_local[vertex].x *= scale.x;
				obj.vlist_local[vertex].y *= scale.y;
				obj.vlist_local[vertex].z *= scale.z;

				ERROR_MSG("\nVertex %d = %f, %f, %f, %f", vertex,
					obj.vlist_local[vertex].x,
					obj.vlist_local[vertex].y,
					obj.vlist_local[vertex].z,
					obj.vlist_local[vertex].w);

				// found vertex, break out of while for next pass
				break;

			}
		}
	}
	// compute average and max radius
	ComputeRadius(obj);

	ERROR_MSG("\nObject average radius = %f, max radius = %f",
		obj.avg_radius, obj.max_radius);


	// step 8: get number of texture vertices
	while (1)
	{
		// get the next line, we are looking for "Texture Vertices ddd" 
		if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			ERROR_MSG("'Texture Vertices' line not found in .COB file %s.", filename);
			return(0);
		}
		// check for pattern?  
		if (parser.match(parser.buffer, "['Texture'] ['Vertices'] [i]"))
		{
			// simply extract the number of texture vertices from the pattern matching 
			// output arrays
			num_texture_vertices = parser.pints[0];

			ERROR_MSG("\nCOB Reader Texture Vertices: %d", num_texture_vertices);
			break;

		}
	}
	// Step 9: load the texture vertex list in format "U V"
	// "d.d d.d"
	for (int tvertex = 0; tvertex < num_texture_vertices; tvertex++)
	{
		// hunt for texture
		while (1)
		{
			// get the next vertex
			if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				ERROR_MSG("\nTexture Vertex list ended abruptly! in .COB file %s.", filename);
				return(0);
			}
			// check for pattern?  
			if (parser.match(parser.buffer, "[f] [f]"))
			{
				// at this point we have the U V in the the pfloats array locations 0,1 for this 
				// texture vertex, although we do nothing with them at this point with this parser
				texture_vertices[tvertex].x = parser.pfloats[0];
				texture_vertices[tvertex].y = parser.pfloats[1];

				ERROR_MSG("\nTexture Vertex %d: U=%f, V=%f", tvertex,
					texture_vertices[tvertex].x,
					texture_vertices[tvertex].y);

				// found vertex, break out of while for next pass
				break;

			}
		}
	}
	int poly_material[OBJECT4DV1_MAX_POLYS]; // this holds the material index for each polygon
											 // we need these indices since when reading the file
											 // we read the polygons BEFORE the materials, so we need
											 // this data, so we can go back later and extract the material
											 // that each poly WAS assigned and get the colors out, since
											 // objects and polygons do not currenlty support materials


	int material_index_referenced[MAX_MATERIALS];   // used to track if an index has been used yet as a material 
													// reference. since we don't know how many materials, we need
													// a way to count them up, but if we have seen a material reference
													// more than once then we don't increment the total number of materials
													// this array is for this

	// clear out reference array
	memset(material_index_referenced, 0, sizeof(material_index_referenced));


	// step 10: load in the polygons
	// poly list starts off with:
	// "Faces ddd:"
	while (1)
	{
		// get next line
		if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
		{
			ERROR_MSG("\n'Faces' line not found in .ASC file %s.", filename);
			return(0);
		} 

	 // check for pattern?  
		if (parser.match(parser.buffer, "['Faces'] [i]"))
		{
			ERROR_MSG("\nCOB Reader found face list in .COB file %s.", filename);

			// finally set number of polys
			obj.num_polys = parser.pints[0];

			break;
		}
	}

	// now read each face in format:
	// Face verts nn flags ff mat mm
	// the nn is the number of vertices, always 3
	// the ff is the flags, unused for now, has to do with holes
	// the mm is the material index number 


	int poly_surface_desc = 0; // ASC surface descriptor/material in this case
	int poly_num_verts = 0; // number of vertices for current poly (always 3)
	int num_materials_object = 0; // number of materials for this object

	for (int poly = 0; poly < obj.num_polys; poly++)
	{
		// hunt until next face is found
		while (1)
		{
			// get the next polygon face
			if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				ERROR_MSG("\nface list ended abruptly! in .COB file %s.", filename);
				return(0);
			}
			// check for pattern?  
			if (parser.match(parser.buffer, "['Face'] ['verts'] [i] ['flags'] [i] ['mat'] [i]"))
			{
				// at this point we have the number of vertices for the polygon, the flags, and it's material index
				// in the integer output array locations 0,1,2

				// store the material index for this polygon for retrieval later, but make sure adjust the 
				// the index to take into consideration that the data in parser.pints[2] is 0 based, and we need
				// an index relative to the entire library, so we simply need to add num_materials to offset the 
				// index properly, but we will leave this reference zero based for now... and fix up later
				poly_material[poly] = parser.pints[2];  //面的材料标记

				// update the reference array
				if (material_index_referenced[poly_material[poly]] == 0)
				{
					// mark as referenced
					material_index_referenced[poly_material[poly]] = 1;

					// increment total number of materials for this object
					num_materials_object++;
				}

				// test if number of vertices is 3
				if (parser.pints[0] != 3)
				{
					ERROR_MSG("\nface not a triangle! in .COB file %s.", filename);
					return(0);
				}
				// now read out the vertex indices and texture indices format:
				// <vindex0, tindex0>  <vindex1, tindex1> <vindex1, tindex1> 
				if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
				{
					ERROR_MSG("\nface list ended abruptly! in .COB file %s.", filename);
					return(0);
				}
				// lets replace ",<>" with ' ' to make extraction easy  第一个是顶点索引，第二个是纹理索引
				ReplaceChars(parser.buffer, parser.buffer, ",<>", ' ');
				parser.match(parser.buffer, "[i] [i] [i] [i] [i] [i]");

				// 0,2,4 holds vertex indices
				// 1,3,5 holds texture indices -- unused for now, no place to put them!

			   // insert polygon, check for winding order invert
				if (vertex_flags & VERTEX_FLAGS_INVERT_WINDING_ORDER)
				{
					poly_num_verts = 3;
					obj.plist[poly].vert[0] = parser.pints[4];
					obj.plist[poly].vert[1] = parser.pints[2];
					obj.plist[poly].vert[2] = parser.pints[0];
				}
				else
				{ // leave winding order alone
					poly_num_verts = 3;
					obj.plist[poly].vert[0] = parser.pints[0];
					obj.plist[poly].vert[1] = parser.pints[2];
					obj.plist[poly].vert[2] = parser.pints[4];
				}

				// point polygon vertex list to object's vertex list
				// note that this is redundant since the polylist is contained
				// within the object in this case and its up to the user to select
				// whether the local or transformed vertex list is used when building up
				// polygon geometry, might be a better idea to set to NULL in the context
				// of polygons that are part of an object
				obj.plist[poly].vlist = obj.vlist_local;


				// set polygon to active
				obj.plist[poly].state = POLY4DV1_STATE_ACTIVE;

				// found the face, break out of while for another pass
				break;

			}
		}

		ERROR_MSG("\nPolygon %d:", poly);
		ERROR_MSG("\nLocal material Index=%d, total materials for object = %d, vert_indices [%d, %d, %d]",
			poly_material[poly],
			num_materials_object,
			obj.plist[poly].vert[0],
			obj.plist[poly].vert[1],
			obj.plist[poly].vert[2]);
	}

	// now find materials!!! and we are out of here!
	for (int curr_material = 0; curr_material < num_materials_object; curr_material++)
	{
		// hunt for the material header "mat# ddd"
		while (1)
		{
			// get the next polygon material 
			if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
			{
				ERROR_MSG("\nmaterial list ended abruptly! in .COB file %s.", filename);
				return(0);
			}

			// check for pattern?  
			if (parser.match(parser.buffer, "['mat#'] [i]"))
			{
				// extract the material that is being defined 
				int material_index = parser.pints[0];

				// get color of polygon, although it might be irrelevant for a textured surface
				while (1)
				{
					// get the next line
					if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						ERROR_MSG("\nRGB color ended abruptly! in .COB file %s.", filename);
						return(0);
					}

					// replace the , comma's if there are any with spaces
					ReplaceChars(parser.buffer, parser.buffer, ",", ' ', 1);

					// look for "rgb float,float,float"
					if (parser.match(parser.buffer, "['rgb'] [f] [f] [f]"))
					{
						// extract data and store color in material libary
						// pfloats[] 0,1,2,3, has data
						materials[material_index + num_materials].color.r = (int)(parser.pfloats[0] * 255 + 0.5);
						materials[material_index + num_materials].color.g = (int)(parser.pfloats[1] * 255 + 0.5);
						materials[material_index + num_materials].color.b = (int)(parser.pfloats[2] * 255 + 0.5);

						break; // while looking for rgb
					}
				}

				// extract out lighting constants for the heck of it, they are on a line like this:
				// "alpha float ka float ks float exp float ior float"
				// alpha is transparency           0 - 1
				// ka is ambient coefficient       0 - 1
				// ks is specular coefficient      0 - 1
				// exp is highlight power exponent 0 - 1
				// ior is index of refraction (unused)

				// although our engine will have minimal support for these, we might as well get them
				while (1)
				{
					// get the next line
					if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						ERROR_MSG("\nmaterial properties ended abruptly! in .COB file %s.", filename);
						return(0);
					}

					// look for "alpha float ka float ks float exp float ior float"
					if (parser.match(parser.buffer, "['alpha'] [f] ['ka'] [f] ['ks'] [f] ['exp'] [f]"))
					{
						// extract data and store in material libary
						// pfloats[] 0,1,2,3, has data
						materials[material_index + num_materials].color.a = (UCHAR)(parser.pfloats[0] * 255 + 0.5);
						materials[material_index + num_materials].ka = parser.pfloats[1];
						materials[material_index + num_materials].kd = 1; // hard code for now
						materials[material_index + num_materials].ks = parser.pfloats[2];
						materials[material_index + num_materials].power = parser.pfloats[3];

						// compute material reflectivities in pre-multiplied format to help engine
						for (int rgb_index = 0; rgb_index < 3; rgb_index++)
						{
							// ambient reflectivity
							materials[material_index + num_materials].ra.rgba_M[rgb_index] =
								((UCHAR)(materials[material_index + num_materials].ka *
									(float)materials[material_index + num_materials].color.rgba_M[rgb_index] + 0.5));


							// diffuse reflectivity
							materials[material_index + num_materials].rd.rgba_M[rgb_index] =
								((UCHAR)(materials[material_index + num_materials].kd *
									(float)materials[material_index + num_materials].color.rgba_M[rgb_index] + 0.5));


							// specular reflectivity
							materials[material_index + num_materials].rs.rgba_M[rgb_index] =
								((UCHAR)(materials[material_index + num_materials].ks *
									(float)materials[material_index + num_materials].color.rgba_M[rgb_index] + 0.5));
						} 

						break;
					}
				}

				// now we need to know the shading model, it's a bit tricky, we need to look for the lines
				// "Shader class: color" first, then after this line is:
				// "Shader name: "xxxxxx" (xxxxxx) "
				// where the xxxxx part will be "plain color" and "plain" for colored polys 
				// or "texture map" and "caligari texture"  for textures
				// THEN based on that we hunt for "Shader class: reflectance" which is where the type
				// of shading is encoded, we look for the "Shader name: "xxxxxx" (xxxxxx) " again, 
				// and based on it's value we map it to our shading system as follows:
				// "constant" . MATV1_ATTR_SHADE_MODE_CONSTANT 
				// "matte"    . MATV1_ATTR_SHADE_MODE_FLAT
				// "plastic"  . MATV1_ATTR_SHADE_MODE_GOURAUD
				// "phong"    . MATV1_ATTR_SHADE_MODE_FASTPHONG 
				// and in the case that in the "color" class, we found a "texture map" then the "shading mode" is
				// "texture map" . MATV1_ATTR_SHADE_MODE_TEXTURE 
				// which must be logically or'ed with the other previous modes

				//  look for the "shader class: color"
				while (1)
				{
					// get the next line
					if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						ERROR_MSG("\nshader class ended abruptly! in .COB file %s.", filename);
						return(0);
					}

					if (parser.match(parser.buffer, "['Shader'] ['class:'] ['color']"))
					{
						break;
					}
				}

				// now look for the shader name for this class
				// Shader name: "plain color" or Shader name: "texture map"
				while (1)
				{
					// get the next line
					if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						ERROR_MSG("\nshader name ended abruptly! in .COB file %s.", filename);
						return(0);
					}
					// replace the " with spaces
					ReplaceChars(parser.buffer, parser.buffer, "\"", ' ', 1);

					// is this a "plain color" poly?
					if (parser.match(parser.buffer, "['Shader'] ['name:'] ['plain'] ['color']"))
					{
						// not much to do this is default, we need to wait for the reflectance type
						// to tell us the shading mode

						break;
					}

					// is this a "texture map" poly?
					if (parser.match(parser.buffer, "['Shader'] ['name:'] ['texture'] ['map']"))
					{
						// set the texture mapping flag in material
						SET_BIT(materials[material_index + num_materials].attr, MATV1_ATTR_SHADE_MODE_TEXTURE);

						// almost done, we need the file name of the darn texture map, its in this format:
						// file name: string "D:\Source\models\textures\wall01.bmp"

						// of course the filename in the quotes will change
						// so lets hunt until we find it...
						while (1)
						{
							// get the next line
							if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
							{
								ERROR_MSG("\ncouldnt find texture name! in .COB file %s.", filename);
								return(0);
							}

							// replace the " with spaces
							ReplaceChars(parser.buffer, parser.buffer, "\"", ' ', 1);

							// is this the file name?
							if (parser.match(parser.buffer, "['file'] ['name:'] ['string'] [s>0]"))
							{
								// ok, simply convert to a real file name by changing the slashes
								ReplaceChars(parser.pstrings[3], parser.pstrings[3], "\\", '/', 1);

								// and save the filename
								strcpy(materials[material_index + num_materials].texture_file, parser.pstrings[3]);

								break;
							}

						}

						break;
					}

				}

				// alright, finally! Now we need to know what the actual shader type, now in the COB format
				// I have decided that in the "reflectance" class that's where we will look at what kind
				// of shader is supposed to be used on the polygon

				//  look for the "Shader class: reflectance"
				while (1)
				{
					// get the next line
					if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						ERROR_MSG("\nshader reflectance class not found in .COB file %s.", filename);
						return(0);
					}

					// look for "Shader class: reflectance"
					if (parser.match(parser.buffer, "['Shader'] ['class:'] ['reflectance']"))
					{
						// now we know the next "shader name" is what we are looking for so, break

						break;
					}

				}

				// looking for "Shader name: "xxxxxx" (xxxxxx) " again, 
				// and based on it's value we map it to our shading system as follows:
				// "constant" . MATV1_ATTR_SHADE_MODE_CONSTANT 
				// "matte"    . MATV1_ATTR_SHADE_MODE_FLAT
				// "plastic"  . MATV1_ATTR_SHADE_MODE_GOURAUD
				// "phong"    . MATV1_ATTR_SHADE_MODE_FASTPHONG 
				// and in the case that in the "color" class, we found a "texture map" then the "shading mode" is
				// "texture map" . MATV1_ATTR_SHADE_MODE_TEXTURE 
				// which must be logically or'ed with the other previous modes
				while (1)
				{
					// get the next line
					if (!parser.getline(PARSER_STRIP_EMPTY_LINES | PARSER_STRIP_WS_ENDS))
					{
						ERROR_MSG("\nshader name ended abruptly! in .COB file %s.", filename);
						return(0);
					}

					// get rid of those quotes
					ReplaceChars(parser.buffer, parser.buffer, "\"", ' ', 1);

					// did we find the name?
					if (parser.match(parser.buffer, "['Shader'] ['name:'] [s>0]"))
					{
						// figure out which shader to use
						if (strcmp(parser.pstrings[2], "constant") == 0)
						{
							// set the shading mode flag in material
							SET_BIT(materials[material_index + num_materials].attr, MATV1_ATTR_SHADE_MODE_CONSTANT);
						}
						else if (strcmp(parser.pstrings[2], "matte") == 0)
						{
							// set the shading mode flag in material
							SET_BIT(materials[material_index + num_materials].attr, MATV1_ATTR_SHADE_MODE_FLAT);
						}
						else
							if (strcmp(parser.pstrings[2], "plastic") == 0)
							{
								// set the shading mode flag in material
								SET_BIT(materials[curr_material + num_materials].attr, MATV1_ATTR_SHADE_MODE_GOURAUD);
							}
							else if (strcmp(parser.pstrings[2], "phong") == 0)
							{
								// set the shading mode flag in material
								SET_BIT(materials[material_index + num_materials].attr, MATV1_ATTR_SHADE_MODE_FASTPHONG);
							}
							else
							{
								// set the shading mode flag in material
								SET_BIT(materials[material_index + num_materials].attr, MATV1_ATTR_SHADE_MODE_FLAT);
							}

						break;
					}

				}

				// found the material, break out of while for another pass
				break;

			}

		}

	}

	// at this point poly_material[] holds all the indices for the polygon materials (zero based, so they need fix up)
	// and we must access the materials array to fill in each polygon with the polygon color, etc.
	// now that we finally have the material libary loaded
	for (int curr_poly = 0; curr_poly < obj.num_polys; curr_poly++)
	{
		ERROR_MSG("\nfixing poly material %d from index %d to index %d", curr_poly,
			poly_material[curr_poly],
			poly_material[curr_poly] + num_materials);
		// fix up offset
		poly_material[curr_poly] = poly_material[curr_poly] + num_materials;

		// we need to know what color depth we are dealing with, so check
		// the bits per pixel, this assumes that the system has already
		// made the call to DDraw_Init() or set the bit depth

			// cool, 16 bit mode
		SET_BIT(obj.plist[curr_poly].attr, POLY4DV1_ATTR_RGB24);
		obj.plist[curr_poly].color = RGB_DX(materials[poly_material[curr_poly]].color.r,
			materials[poly_material[curr_poly]].color.g,
			materials[poly_material[curr_poly]].color.b);
		ERROR_MSG("\nPolygon 16-bit");


		// now set all the shading flags
		// figure out which shader to use
		if (materials[poly_material[curr_poly]].attr & MATV1_ATTR_SHADE_MODE_CONSTANT)
		{
			// set shading mode
			SET_BIT(obj.plist[curr_poly].attr, POLY4DV1_ATTR_SHADE_MODE_CONSTANT);
		}
		else if (materials[poly_material[curr_poly]].attr & MATV1_ATTR_SHADE_MODE_FLAT)
		{
			// set shading mode
			SET_BIT(obj.plist[curr_poly].attr, POLY4DV1_ATTR_SHADE_MODE_FLAT);
		}
		else if (materials[poly_material[curr_poly]].attr & MATV1_ATTR_SHADE_MODE_GOURAUD)
		{
			// set shading mode
			SET_BIT(obj.plist[curr_poly].attr, POLY4DV1_ATTR_SHADE_MODE_GOURAUD);
		}
		else if (materials[poly_material[curr_poly]].attr & MATV1_ATTR_SHADE_MODE_FASTPHONG)
		{
			// set shading mode
			SET_BIT(obj.plist[curr_poly].attr, POLY4DV1_ATTR_SHADE_MODE_FASTPHONG);
		}
		else
		{
			// set shading mode
			SET_BIT(obj.plist[curr_poly].attr, POLY4DV1_ATTR_SHADE_MODE_GOURAUD);
		}

		if (materials[poly_material[curr_poly]].attr & MATV1_ATTR_SHADE_MODE_TEXTURE)
		{
			// set shading mode
			SET_BIT(obj.plist[curr_poly].attr, POLY4DV1_ATTR_SHADE_MODE_TEXTURE);
		}

	}

	// local object materials have been added to database, update total materials in system
	num_materials += num_materials_object;

	for (int curr_material = 0; curr_material < num_materials; curr_material++)
	{
		ERROR_MSG("\nMaterial %d", curr_material);

		ERROR_MSG("\nint  state    = %d", materials[curr_material].state);
		ERROR_MSG("\nint  id       = %d", materials[curr_material].id);
		ERROR_MSG("\nchar name[64] = %s", materials[curr_material].name);
		ERROR_MSG("\nint  attr     = %d", materials[curr_material].attr);
		ERROR_MSG("\nint r         = %d", materials[curr_material].color.r);
		ERROR_MSG("\nint g         = %d", materials[curr_material].color.g);
		ERROR_MSG("\nint b         = %d", materials[curr_material].color.b);
		ERROR_MSG("\nint alpha     = %d", materials[curr_material].color.a);
		ERROR_MSG("\nint color     = %d", materials[curr_material].attr);
		ERROR_MSG("\nfloat ka      = %f", materials[curr_material].ka);
		ERROR_MSG("\nkd            = %f", materials[curr_material].kd);
		ERROR_MSG("\nks            = %f", materials[curr_material].ks);
		ERROR_MSG("\npower         = %f", materials[curr_material].power);
		ERROR_MSG("\nchar texture_file = %s\n", materials[curr_material].texture_file);
	}

	return(1);
}

