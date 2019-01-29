/* Shared Use License: This file is owned by Derivative Inc. (Derivative) and
 * can only be used, and/or modified for use, in conjunction with
 * Derivative's TouchDesigner software, and only if you are a licensee who has
 * accepted Derivative's TouchDesigner license or assignment agreement (which
 * also govern the use of this file).  You may share a modified version of this
 * file with another authorized licensee of Derivative's TouchDesigner software.
 * Otherwise, no redistribution or sharing of this file, with or without
 * modification, is permitted.
 */

 /*
  * Filename: TOP_SharedMemHeader.h
  */

#ifndef TOP_SHARED_MEM_HEADER_H
#define TOP_SHARED_MEM_HEADER_H

#define TOP_SHM_MAGIC_NUMBER 	0xe95df673

#define TOP_SHM_VERSION_NUMBER 	2

  // Valid values for dataFormat
  // When using fixed point data try to use a BGRA ordering
  // When using floating point data, try to use RGBA order
  /****************************/
#ifndef GL_RGB 
#define GL_RED				0x1903
#define GL_RG				0x8227
#define GL_RGB				0x1907
#define GL_RGBA				0x1908
#define GL_LUMINANCE_ALPHA	0x190A
#define GL_ALPHA			0x1906
#define GL_LUMINANCE		0x1909
#endif 
#ifndef GL_EXT_bgra
#define GL_BGR_EXT			0x80E0
#define GL_BGRA_EXT			0x80E1 
#endif 
#ifndef GL_EXT_abgr
#define GL_ABGR_EXT			0x8000
#endif 
/***************************/

// Valid values for dataType
/**************************/
#ifndef GL_UNSIGNED_BYTE
#define GL_UNSIGNED_BYTE	0x1401
#define GL_UNSIGNED_SHORT	0x1403
#define GL_FLOAT			0x1406 
#endif 
#ifndef GL_ARB_half_float_pixel
	// CPUs dont have a native half-float format, but if you are loading
	// data that was saved in this format, you can upload it directly.
	// No point bothering with this unless you are also using a 16-bit
	// pixelFormat
#define GL_HALF_FLOAT_ARB	0x140B
#endif
/**************************/


// Valid values for pixelFormat
/*****************************/
#ifndef GL_RGBA8
#define GL_RGBA8			0x8058
#define GL_ALPHA8			0x803C
#define GL_ALPHA16			0x803E
#define GL_RGB10_A2			0x8059
#define GL_RGBA16			0x805B
#endif 
#ifndef GL_ARB_texture_float
#define GL_RGBA16F_ARB		0x881A    		
#define GL_RGBA32F_ARB		0x8814
#define GL_ALPHA16F_ARB		0x881C
#define GL_ALPHA32F_ARB		0x8816
#define GL_RGB32F_ARB		0x8815
#endif 
#ifndef GL_EXT_packed_float
#define GL_R11F_G11F_B10F_EXT	0x8C3A
#endif 
#ifndef GL_ARB_texture_rg
#define GL_R8				0x8229
#define GL_R16				0x822A
#define GL_R16F				0x822D
#define GL_R32F				0x822E
#define GL_RG8				0x822B
#define GL_RG16				0x822C
#define GL_RG16F			0x822F
#define GL_RG32F			0x8230
#endif 
/*****************************/

// If you add new members to this after it's released, add them after dataOffset
class TOP_SharedMemHeader
{
public:
	/* Begin version 1 */
	// Magic number to make sure we are looking at the correct memory
	// must be set to TOP_SHM_MAGIC_NUMBER (0xe95df673)
	int							magicNumber;

	// version number of this header, must be set to TOP_SHM_VERSION_NUMBER
	int							version;

	// image width
	int							width;

	// image height
	int							height;

	// X aspect of the image
	float						aspectx;

	// Y aspect of the image
	float						aspecty;

	// Format of the image data in memory (RGB, RGBA, BGR, BGRA etc.)
	int							dataFormat;

	// The data type of the image data in memory (unsigned char, float)
	int							dataType;

	// The desired pixel format of the texture to be created in TouchDesigner (RGBA8, RGBA16, RGBA32 etc.)
	int							pixelFormat;

	// This offset (in bytes) is the diffrence between the start of this header,
	// and the start of the image data
	// The SENDER is required to set this. Unless you are doing something custom
	// you should set this to calcDataOffset();
	// If you are the RECEIVER, don't change this value.
	int							dataOffset;

	/* End version 1 */


	// Both the sender and the reciever can use this to get the pointer to the actual
	// image data (as long as dataOffset is set beforehand).
	void						*getImage()
	{
		char *c = (char*)this;
		c += dataOffset;
		return (void*)c;
	}


	int							calcDataOffset()
	{
		return sizeof(TOP_SharedMemHeader);
	}

};

#endif
