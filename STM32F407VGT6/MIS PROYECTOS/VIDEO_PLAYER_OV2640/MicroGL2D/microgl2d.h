/*
  * Author: VadRov
  * Copyright (C) 2022, VadRov, all rights reserved.
  *
  * Graphics library for working with primitive objects in two-dimensional space microGL2D
  * Version: mini (stripped down)
  *
  * Free distribution allowed.
  *For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
  *
  * Supports such simple graphical primitive objects as:
  *  - triangle;
  *  - rectangle;
  * - circle;
  *  - text.
  * Filling of primitives with a single color, gradient, texture is available.
  * Properties of primitive objects, gradient and texture are set and can be changed at any time.
  *Textures can have different formats (see description of types). Transparency available (from 0 (opaque) to 255 (absolutely transparent))
  * primitive objects.
  * For textures, for example, the following properties are available:
  * - rotation at an arbitrary angle;
  * - repetition along given coordinate axes;
  * - flip (mirror image) relative to the specified coordinate axes.
  * The gradient can be linear (with an arbitrary angle deg of the straight line passing through the center of the object) and radial.
  * Key points are specified for the gradient. The key point offset from the origin is specified in % (from 0% to 100%), and
  * the color at the key point is specified in the R8G8B8 format, for example, 0xFFF8F2, where R = 0xFF, G = 0xF8, B = 0xF2.
 *
 *  https://www.youtube.com/c/VadRov
 *  https://zen.yandex.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

#ifndef MICROGL2D_H_
#define MICROGL2D_H_

#include "fonts.h"

//Types of primitive objects
typedef enum {
	MGL_OBJ_TYPE_TRIANGLE,		//triangle
	MGL_OBJ_TYPE_FILLTRIANGLE,	//filled triangle
	MGL_OBJ_TYPE_RECTANGLE,	    //rectangle
	MGL_OBJ_TYPE_FILLRECTANGLE,	//filled rectangle
	MGL_OBJ_TYPE_CIRCLE,		//circle
	MGL_OBJ_TYPE_FILLCIRCLE,	//filled circle
	MGL_OBJ_TYPE_TEXT,			//text
} MGL_OBJ_TYPES;

//Gradient types
typedef enum {
	MGL_GRADIENT_LINEAR = 0, //linear gradient
	MGL_GRADIENT_RADIAL,	 //radial gradient
} MGL_GRADIENT_TYPES;

//Gradient Key Point
typedef struct {
	uint8_t offset;			//point displacement from the origin, as a percentage relative to the linear size: 0 - 100%
	uint32_t color;			//key point color
	uint8_t flag_mix;		//a flag that determines how colors are mixed when moving from one key point
							//to the other: !=0 - mix, 0 - do not mix.
	void *next;				//pointer to the next key point (this is how the list of gradient key points is formed)
} MGL_GRADIENT_POINT;

//Gradient data
typedef struct {
	MGL_GRADIENT_TYPES g_type;		 //gradient type
	int deg;						 //corner
	MGL_GRADIENT_POINT *points_list; //list with gradient key points (list of color changes)
} MGL_GRADIENT;

//Image Color Mode
typedef enum {
	MGL_IMAGE_COLOR_R3G3B2 = 0,	//8 bits without transparency: 3 bits each for r, g and 2 bits for b (256 colors)
	MGL_IMAGE_COLOR_R5G6B5,		//16 bits without transparency: 5 bits each for r, b and 6 bits for g (65536 colors)
	MGL_IMAGE_COLOR_A4R4G4B4,	//16 bits with transparency: 4 bits per color component and 4 bits per transparency component (4096 colors)
	MGL_IMAGE_COLOR_R8G8B8,		//24 bits without transparency: 8 bits per color component (16777216 colors)
	MGL_IMAGE_COLOR_A8R8G8B8,	//32 bits with transparency: 8 bits per color component and 8 bits per transparency component (16777216 colors)
} MGL_IMAGE_COLOR_MODES;

//Image data
typedef struct {
	const void *data;			//pointer to an array with color information
	int w, h;					//image width and height
	MGL_IMAGE_COLOR_MODES mode; //color type
} MGL_IMAGE;

//Texture properties
#define	MGL_TEXTURE_REPEAT_X	1	//repeat on x-axis (the image is repeated on the x-axis when the texture width is smaller than the object width)
#define	MGL_TEXTURE_REPEAT_Y	2	//repeat along y axis (the image is repeated along the y axis when the texture height is less than the object height)
#define	MGL_TEXTURE_FLIP_X		4	//mirror x-axis
#define	MGL_TEXTURE_FLIP_Y		8	//mirror y-axis

//Texture data
typedef struct {
	MGL_IMAGE *image;	//pointer to image data
	int alpha;			//texture rotation angle, degrees
	uint8_t features;	//texture properties
} MGL_TEXTURE;

//Graphics object handler
typedef struct {
	MGL_OBJ_TYPES obj_type; //Object type
	void *object;			//pointer to a data object
	uint8_t transparency;   //object transparency (0 - completely opaque, ..., 255 - invisible)
	uint8_t plane;			//object position - plan: front, back
							//(0 - in the foreground - above all, ..., 255 - background - below all)
	uint8_t visible;		//visibility: visible (!=0) or invisible (0)
	MGL_GRADIENT *gradient;	//gradient (defines the colors and direction of the gradient)
	MGL_TEXTURE *texture;  	//texture (defines the texture image data and its properties)
	char *name;				//object name
	void *parent;			//pointer to parent object
	void *next;				//pointer to next object
	void *prev;				//pointer to previous object
} MGL_OBJ;

//Triangle data
typedef struct {
	int x1, y1, x2, y2, x3, y3;	//vertex coordinates
	uint32_t color;				//color
} MGL_OBJ_TRIANGLE;

//Rectangle data
typedef struct {
	int x1, y1, x2, y2;	//vertex coordinates (top left and bottom right)
	uint32_t color;		//color
} MGL_OBJ_RECTANGLE;

//Circle/circle data
typedef struct {
	int x, y, r;	//center coordinates and radius
	uint32_t color;	//color
} MGL_OBJ_CIRCLE;

//Text data
typedef struct {
	int x, y;		//coordinates of the starting position of the print, relative to the upper left vertex of the block
	char *txt;		//pointer to a line of text
	FontDef *font;	//font pointer
	uint8_t bold;	//character thickness flag (0 - regular, !=0 - thick)
	uint32_t color;	//color
} MGL_OBJ_TEXT;

//Adds an object
MGL_OBJ* MGL_ObjectAdd(MGL_OBJ *obj, MGL_OBJ_TYPES type);
//Deletes an object
MGL_OBJ* MGL_ObjectDelete(MGL_OBJ *obj);
//Deletes all objects in the list (interrelated objects that refer to each other)
void MGL_ObjectsListDelete(MGL_OBJ *obj);
//Sets triangle parameters
void MGL_SetTriangle(MGL_OBJ *obj, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
//Sets rectangle parameters
void MGL_SetRectangle(MGL_OBJ *obj, int x1, int y1, int x2, int y2, uint32_t color);
//Sets circle parameters
void MGL_SetCircle(MGL_OBJ *obj, int x, int y, int r, uint32_t color);
//Sets text options
void MGL_SetText(MGL_OBJ *obj, int x, int y, char *txt, FontDef *font, uint8_t bold, uint32_t color);
//Sets the texture for an object
void MGL_ObjectSetTexture(MGL_OBJ *obj, MGL_TEXTURE *texture);
//Sets a gradient for an object
void MGL_ObjectSetGradient(MGL_OBJ *obj, MGL_GRADIENT *gradient);
//Creates a gradient
MGL_GRADIENT* MGL_GradientCreate(MGL_GRADIENT_TYPES type);
//Adds a key point to the gradient
void MGL_GradientAddColor(MGL_GRADIENT *gradient, uint8_t offset, uint32_t color, uint8_t flag_mix);
//Removes the gradient
void MGL_GradientDelete(MGL_GRADIENT *gradient);
//Sets the gradient angle
void MGL_GradientSetDeg(MGL_GRADIENT *gradient, int deg);
//Sets the transparency of an object
void MGL_ObjectSetTransparency(MGL_OBJ *obj, uint8_t tr);
//Sets the object's outline (foremost 0...backmost 255)
void MGL_ObjectSetPlane(MGL_OBJ *obj, uint8_t plane);
//Sets the visibility of an object (0 - invisible, != 0 - visible)
void MGL_ObjectSetVisible(MGL_OBJ *obj, uint8_t visible);
//Moves the object a distance along the x axis by dx, along the y axis by dy
void MGL_ObjectMove(MGL_OBJ *obj, int dx, int dy);
//Moves all objects in the list by a distance along the x axis to dx, along the y axis to dy
void MGL_ObjectListMove(MGL_OBJ *obj_list, int dx, int dy);
//Sets transparency for list objects (0 - completely opaque. 255 - invisible)
void MGL_ObjectListTransparency(MGL_OBJ *obj_list, uint8_t tr);

//Draws objects (parts thereof) that are in the current output window into the buffer
void MGL_RenderObjects(MGL_OBJ *obj, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *data);
#endif /* MICROGL2D_H_ */
