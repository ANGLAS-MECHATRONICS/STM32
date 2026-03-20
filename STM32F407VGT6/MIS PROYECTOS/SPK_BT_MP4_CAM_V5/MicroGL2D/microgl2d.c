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
  * The gradient can be linear (with an arbitrary angle of inclination deg) and radial.
  * Key points are specified for the gradient. The key point offset from the origin is specified in % (from 0% to 100%), and
  * the color at the key point is specified in the R8G8B8 format, for example, 0xFFF8F2, where R = 0xFF, G = 0xF8, B = 0xF2.
 *
 *  https://www.youtube.com/c/VadRov
 *  https://zen.yandex.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

#include "main.h"
#include "microgl2d.h"
#include "mystring.h"
#include <stdlib.h>
#include "display.h"

#define min(x1,x2)	(x1 < x2 ? x1 : x2)
#define max(x1,x2)	(x1 > x2 ? x1 : x2)

#define min3(x1,x2,x3)	min(min(x1,x2),x3)
#define max3(x1,x2,x3)	max(max(x1,x2),x3)

#define min4(x1,x2,x3,x4)	min(min3(x1,x2,x3),x4)
#define max4(x1,x2,x3,x4)	max(max3(x1,x2,x3),x4)

#define MGL_fabs(x)	(x < 0 ? -x : x)

//Table of sines from 0 to 90 degrees (values multiplied by 32768)
const uint16_t sin_tbl_int[91] = {	    0,   572,  1144,  1715,  2286,  2856,  3425,  3993,  4560,  5126,
									 5690,  6252,  6813,  7371,  7927,  8481,  9032,  9580, 10126, 10668,
									11207, 11743, 12275, 12803, 13328, 13848, 14365, 14876, 15384, 15886,
									16384, 16877, 17364, 17847, 18324, 18795, 19261, 19720, 20174, 20622,
									21063, 21498, 21926, 22348, 22763, 23170, 23571, 23965, 24351, 24730,
									25102, 25466, 25822, 26170, 26510, 26842, 27166, 27482, 27789, 28088,
									28378, 28660, 28932, 29197, 29452, 29698, 29935, 30163, 30382, 30592,
									30792, 30983, 31164, 31336, 31499, 31651, 31795, 31928, 32052, 32166,
									32270, 32365, 32449, 32524, 32588, 32643, 32688, 32723, 32748, 32763,
									32768 };

//Returns the sine of the angle multiplied by 32768.
//angle - angle in degrees
static int MGL_sin(int angle)
{
	if (MGL_fabs(angle) > 360) angle %= 360;
	if (angle < 0) angle += 360;
	if (angle >=   0 && angle <=  90) return (int)sin_tbl_int[angle];
	if (angle >=  91 && angle <= 180) return (int)sin_tbl_int[180 - angle];
	if (angle >= 181 && angle <= 270) return -(int)sin_tbl_int[angle - 181];
	return -(int)sin_tbl_int[360 - angle]; //271...360
}

//Returns the cosine of the angle multiplied by 32768.
//angle - angle in degrees
static int MGL_cos(int angle)
{
	return MGL_sin(angle + 90);
}

//Calculate square root using FPU if available
#if defined (__CORTEX_M) && defined (__FPU_USED) && (__FPU_USED == 1U)
static inline float MGL_sqrt(float x)
{
	float res;
	asm volatile ("vsqrt.f32 %0, %1" : "=&t" (res) : "t" (x));
	return res;
}
//Or if FPU is not available, then calculate the square root using the algorithm
#else
static inline float MGL_sqrt(float x)
{
  union
  {
    int i;
    float x;
  } u;

  u.x = x;
  u.i = (1<<29) + (u.i >> 1) - (1<<22);
  return u.x;
}
#endif

//Creates a gradient and returns a pointer to it
MGL_GRADIENT* MGL_GradientCreate(MGL_GRADIENT_TYPES type)
{
	MGL_GRADIENT *gradient = (MGL_GRADIENT *)malloc(sizeof(MGL_GRADIENT));
	gradient->g_type = type;
	gradient->deg = 0;
	gradient->points_list = 0;
	return gradient;
}

//Deletes the gradient
void MGL_GradientDelete(MGL_GRADIENT *gradient)
{
	if (!gradient) return;
	MGL_GRADIENT_POINT *point = gradient->points_list, *next;
	while (point) {
		next = point->next;
		free(point);
		point = next;
	}
	free(gradient);
}

//Adds a key point to the gradient.    يضيف نقطة رئيسية إلى التدرج.
void MGL_GradientAddColor(MGL_GRADIENT *gradient, uint8_t offset, uint32_t color, uint8_t flag_mix)
{
	if (offset > 100) return;
	MGL_GRADIENT_POINT *point = (MGL_GRADIENT_POINT*)malloc(sizeof(MGL_GRADIENT_POINT));
	MGL_GRADIENT_POINT *prev = 0;
	MGL_GRADIENT_POINT *ptr = gradient->points_list;
	point->offset = offset;
	point->color = color;
	point->flag_mix = flag_mix;
	point->next = 0;
	if (!ptr) {
		gradient->points_list = point;
		return;
	}
	while (ptr) {
		if (ptr->offset == offset) {		//If the key point exists, then
			ptr->color = color;				//update her data
			ptr->flag_mix = flag_mix;
			free(point);
			return;
		}
		else if (ptr->offset > offset) {	//If the location of the new key point
			point->next = ptr;				//precedes the location of the current keypoint in the list,
			if (prev) {						//then insert a new key point between the previous and current
				prev->next = point;			//key points of the list.
			}
			else {
				gradient->points_list = point;
			}
			return;
		}
		prev = ptr;
		ptr = ptr->next;
	}
	prev->next = point;	//Add a key point to the end of the list
}

//Sets the angle for a linear gradient
inline void MGL_GradientSetDeg(MGL_GRADIENT *gradient, int deg)
{
	gradient->deg = deg;
}

//Creates an object of the specified type and returns a pointer to it
MGL_OBJ* MGL_ObjectAdd(MGL_OBJ *obj_list, MGL_OBJ_TYPES type)
{
	MGL_OBJ *obj = (MGL_OBJ *)calloc(1, sizeof(MGL_OBJ));
	obj->obj_type = type;
	obj->visible = 1;
	switch (type) {
		case MGL_OBJ_TYPE_TRIANGLE:
		case MGL_OBJ_TYPE_FILLTRIANGLE:
			obj->object = calloc(1, sizeof(MGL_OBJ_TRIANGLE));
			break;
		case MGL_OBJ_TYPE_RECTANGLE:
		case MGL_OBJ_TYPE_FILLRECTANGLE:
			obj->object = calloc(1, sizeof(MGL_OBJ_RECTANGLE));
			break;
		case MGL_OBJ_TYPE_CIRCLE:
		case MGL_OBJ_TYPE_FILLCIRCLE:
			obj->object = calloc(1, sizeof(MGL_OBJ_CIRCLE));
			break;
		case MGL_OBJ_TYPE_TEXT:
			obj->object = calloc(1, sizeof(MGL_OBJ_TEXT));
			break;
		default:
			free (obj);
			return 0;
	}
	obj->next = obj->prev = 0;
	if (!obj_list) return obj;
	MGL_OBJ *prev = obj_list;
	while (prev->next) {
		prev = (MGL_OBJ *)prev->next;
	}
	obj->prev = (void*)prev;
	prev->next = (void*)obj;
	return obj;
}

//Deletes an object
MGL_OBJ* MGL_ObjectDelete(MGL_OBJ *obj)
{
	if (!obj) return 0;
	MGL_OBJ *ptr = 0;
	void *prev = obj->prev;
	void *next = obj->next;
	if (prev) {
		((MGL_OBJ *)prev)->next = next;
	}
	else {
		ptr = next;
	}
	if (next) {
		((MGL_OBJ *)next)->prev = prev;
	}
	if (obj->object) free (obj->object);
	free(obj);
	return ptr;
}

//Deletes all objects in the list
//(interrelated objects referencing each other)
void MGL_ObjectsListDelete(MGL_OBJ *obj)
{
	if (!obj) return;
	while (obj) {
		obj = MGL_ObjectDelete(obj);
	}
}

//Sets the texture for the specified object
inline void MGL_ObjectSetTexture(MGL_OBJ *obj, MGL_TEXTURE *texture)
{
	obj->texture = texture;
}

//Sets the gradient for the specified object
inline void MGL_ObjectSetGradient(MGL_OBJ *obj, MGL_GRADIENT *gradient)
{
	obj->gradient = gradient;
}

//Sets the transparency of an object
inline void MGL_ObjectSetTransparency(MGL_OBJ *obj, uint8_t tr)
{
	obj->transparency = tr;
}

//Sets the site plan
inline void MGL_ObjectSetPlane(MGL_OBJ *obj, uint8_t plane)
{
	obj->plane = plane;
}

//Sets the visibility of an object
inline void MGL_ObjectSetVisible(MGL_OBJ *obj, uint8_t visible)
{
	obj->visible = visible;
}

//Sets rectangle parameters
void MGL_SetRectangle(MGL_OBJ *obj, int x1, int y1, int x2, int y2, uint32_t color)
{
	int tmp;
	if (x1 > x2) {
		tmp = x1; x1 = x2; x2 = tmp;
	}
	if (y1 > y2) {
		tmp = y1; y1 = y2; y2 = tmp;
	}
	MGL_OBJ_RECTANGLE *obj_rectangle = (MGL_OBJ_RECTANGLE*)obj->object;
	obj_rectangle->x1 = x1;
	obj_rectangle->y1 = y1;
	obj_rectangle->x2 = x2;
	obj_rectangle->y2 = y2;
	obj_rectangle->color = color;
}

//Sets triangle parameters
void MGL_SetTriangle(MGL_OBJ *obj, int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)
{
	int tmp;
	if (y1 > y2) {
		tmp = y1; y1 = y2; y2 = tmp;
		tmp = x1; x1 = x2; x2 = tmp;
	}
	if (y1 > y3) {
		tmp = y1; y1 = y3; y3 = tmp;
		tmp = x1; x1 = x3; x3 = tmp;
	}
	if (y2 > y3) {
		tmp = y2; y2 = y3; y3 = tmp;
		tmp = x2; x2 = x3; x3 = tmp;
	}
	MGL_OBJ_TRIANGLE *obj_triangle = (MGL_OBJ_TRIANGLE*)obj->object;
	obj_triangle->x1 = x1;
	obj_triangle->y1 = y1;
	obj_triangle->x2 = x2;
	obj_triangle->y2 = y2;
	obj_triangle->x3 = x3;
	obj_triangle->y3 = y3;
	obj_triangle->color = color;
}

//Sets circle parameters
void MGL_SetCircle(MGL_OBJ *obj, int x, int y, int r, uint32_t color)
{
	MGL_OBJ_CIRCLE *obj_circle = (MGL_OBJ_CIRCLE*)obj->object;
	obj_circle->x = x;
	obj_circle->y = y;
	obj_circle->r = r;
	obj_circle->color = color;
}

//Sets text options
void MGL_SetText(MGL_OBJ *obj, int x, int y, char *txt, FontDef *font, uint8_t bold, uint32_t color)
{
	MGL_OBJ_TEXT *obj_txt = (MGL_OBJ_TEXT*)obj->object;
	obj_txt->x = x;
	obj_txt->y = y;
	obj_txt->txt = txt;
	obj_txt->font = font;
	obj_txt->bold = bold;
	obj_txt->color = color;
}

//Types of intersections
typedef enum {
	LINES_NO_INTERSECT = 0, //do not intersect
	LINES_INTERSECT,		//intersect
	LINES_MATCH				//coincide (overlap)
} INTERSECTION_TYPES;

//Defining the type of intersection and coordinates (along the x axis) of the intersection of a segment with coordinates (x1,y1)-(x2,y2)
//with horizontal line y = y0
//Returns the type of intersection, and in the variables x_min, x_max - the coordinate or range of the intersection (if overlapping) along the x axis,
//in match increments the number of overlaps (we count the results from all necessary calls)
static INTERSECTION_TYPES MGL_LinesIntersection(int x1, int y1, int x2, int y2, int y0, int *x_min, int *x_max, uint8_t *match)
{
	if (y1 == y2) { //special case - the segment is parallel to the x axis
		if (y0 == y1) { //match checking
			*x_min = min(x1, x2);
			*x_max = max(x1, x2);
			(*match)++;
			return LINES_MATCH;
		}
		return LINES_NO_INTERSECT;
	}
	if (x1 == x2) { //special case - the segment is parallel to the y axis
		if (min(y1, y2) <= y0 && y0 <= max(y1, y2))	{
			*x_min = *x_max = x1;
			return LINES_INTERSECT;
		}
		return LINES_NO_INTERSECT;
	}
	*x_min = *x_max = (x2 - x1) * (y0 - y1) / (y2 - y1) + x1;
	if (min(x1, x2) <= *x_min && *x_min <= max(x1, x2)) return LINES_INTERSECT;
	return LINES_NO_INTERSECT;
}

//Calculates the color at a gradient point
static inline uint32_t MGL_Color_gradient(uint32_t color1, uint32_t color2, uint16_t a)
{
	uint32_t color;
	a++;
	asm volatile (	"uxtb	r0, %[color1], ror #16		\n\t"
					"uxtb	r1, %[color2], ror #16		\n\t"
					"subs	r1, r0, r1					\n\t"
					"mul	r1, %[a], r1				\n\t"
					"sub	r0, r0, r1, asr #8			\n\t"
					"uxtb	r1, %[color1], ror #8		\n\t"
					"uxtb	r2, %[color2], ror #8		\n\t"
					"subs	r2, r1, r2					\n\t"
					"mul	r2, %[a], r2				\n\t"
					"sub	r1, r1, r2, asr #8			\n\t"
					"uxtb	r2, %[color1]				\n\t"
					"uxtb	r3, %[color2]				\n\t"
					"subs	r3, r2, r3					\n\t"
					"mul	r3, %[a], r3				\n\t"
					"sub	r2, r2, r3, asr #8			\n\t"
					"orr	r2, r2, r1, lsl #8			\n\t"
					"orr	%[color], r2, r0, lsl #16	\n\t"
					:[color]"=&r"(color)
					:[color1]"r"(color1), [color2]"r"(color2), [a]"r"(a)
					: "r0", "r1", "r2", "r3");
	return color;
	/*
		uint8_t r_col1 = (color1 >> 16) & 0xff;
		uint8_t g_col1 = (color1 >> 8) & 0xff;
		uint8_t b_col1 = color1 & 0xff;

		uint8_t r_col2 = (color2 >>16) & 0xff;
		uint8_t g_col2 = (color2 >> 8) & 0xff;
		uint8_t b_col2 = color2 & 0xff;

		r_col1 -= (a * (r_col1 - r_col2)) >> 8;
		g_col1 -= (a * (g_col1 - g_col2)) >> 8;
		b_col1 -= (a * (b_col1 - b_col2)) >> 8;

		return (r_col1 << 16) | (g_col1 << 8) | b_col1;
	*/
}

//Calculates the resulting color at a point when overlaying objects, taking into account transparency
static inline uint16_t MGL_NewColor(uint16_t col_f, uint32_t color_obj, uint16_t a)
{
	/*
	uint16_t color;
	asm volatile (	"uxtb	r3, %[color_obj], ror #16		\n\t"	//r
					"uxtb	r4, %[color_obj], ror #8		\n\t"	//g
					"uxtb	r5, %[color_obj]				\n\t"	//b
					"cbz	%[a], 1f						\n\t"
					"add	%[a], %[a], #1					\n\t"
					"mov	r7, #0xf8						\n\t"
					"and	r6, r7, %[col_f], lsr #8		\n\t"
					"subs	r6, r3, r6						\n\t"
					"mul	r6, %[a], r6					\n\t"
					"sub	r3, r3, r6, asr #8				\n\t"
					"and	r6, r7, %[col_f], lsr #3		\n\t"
					"subs	r6, r1, r6						\n\t"
					"mul	r6, %[a], r6					\n\t"
					"sub	r4, r4, r6, asr #8				\n\t"
					"and	r6, r7, %[col_f], lsl #3		\n\t"
					"subs	r6, r5, r6						\n\t"
					"mul	r6, %[a], r6					\n\t"
					"sub	r5, r5, r6, asr #8				\n\t"
					"1:										\n\t"
					"lsrs	%[color], r5, #3				\n\t"
					"lsrs	r4, r4, #3						\n\t"
					"lsrs	r3, r3, #3						\n\t"
					"orr	%[color], %[color], r4, lsl #6	\n\t"
					"orr	%[color], %[color], r5, lsl #11	\n\t"
					:[a]"+r"(a), [color]"=&r"(color)
					:[col_f]"r"(col_f), [color_obj]"r"(color_obj)
					: "r3", "r4", "r5", "r6", "r7");
	return color;
	*/
	uint8_t r = (color_obj >> 16) & 0xff;
	uint8_t g = (color_obj >> 8)  & 0xff;
	uint8_t b = color_obj & 0xff;
	if (a) {
		a++;
		uint8_t r_f = (col_f >> 8) & 0xf8;
		uint8_t g_f = (col_f >> 3) & 0xf8;
		uint8_t b_f = (col_f << 3) & 0xf8;
		r -= (a * (r - r_f)) >> 8;
		g -= (a * (g - g_f)) >> 8;
		b -= (a * (b - b_f)) >> 8;
	}
	return ((r & 0xf8) << 8) | ((g & 0xf8) << 3) | (b >> 3);
}

//Callback for selecting pixel color components
typedef void (*GRAPH_getcolor)(const void *data, uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b);

//Functions for extracting pixel color components depending on the color format
static void GRAPH_getcolor_unknow(const void *pix_col, uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b)
{
	*a = *r = *g = *b = 0;
}

static void GRAPH_getcolor_R3G3B2(const void *pix_col, uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint8_t c8 = *((uint8_t*)pix_col);
	*a = 255; //absolutely opaque absolutely transparent 0
	*r = c8 & 0b11100000;
	*g = (c8 << 3) & 0b11100000;
	*b = (c8 << 6) & 0b11000000;
}

static void GRAPH_getcolor_R5G6B5(const void *pix_col, uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint16_t c16 = *((uint16_t*)pix_col);
	*a = 255; //absolutely opaque absolutely transparent 0    معتم تمامًا وشفاف تمامًا 0
	*r = (c16 >> 8) & 0xF8;
	*g = (c16 >> 3) & 0xF8;
	*b = (c16 << 3) & 0xF8;
}

static void GRAPH_getcolor_A4R4G4B4(const void *pix_col, uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint16_t c16 = *((uint16_t*)pix_col);
	*a = (c16 >> 8) & 0xF0;
	*r = (c16 >> 4) & 0xF0;
	*g = c16 & 0xF0;
	*b = (c16 << 4) & 0xF0;
}

static void GRAPH_getcolor_R8G8B8(const void *pix_col, uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint8_t *c24 = (uint8_t*)pix_col;
	*a = 255; //absolutely opaque 255, absolutely transparent 0
	*r = c24[0];
	*g = c24[1];
	*b = c24[2];
}

static void GRAPH_getcolor_A8R8G8B8(const void *pix_col, uint8_t *a, uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint32_t c32 = *((uint32_t*)pix_col);
	*a = c32 >> 24;
	*r = (c32 >> 16) & 0xFF;
	*g = (c32 >> 8) & 0xFF;
	*b = c32 & 0xFF;
}

static void MGL_setcolorbuffer(MGL_OBJ *obj, uint16_t *buffer,
							   int x0, int y, int x_start, int x_end,
							   int x_min, int y_min, int x_w, int y_h,
						   	   uint32_t color)
{
	uint8_t tr = obj->transparency;
	uint8_t a = 0;
	uint32_t col_gr = 0;
	int x;

	if (!obj->texture && !obj->gradient) {
		for (x = x_start; x <= x_end; x++) {
			buffer[x - x0] = MGL_NewColor(buffer[x - x0], color, tr);
		}
		return;
	}

	if (obj->texture) {
		MGL_IMAGE *image = obj->texture->image;
		if (!image) return;
		int i_h = image->h, i_w = image->w;
		int j1, i1, j1_n, i1_n;
		int alpha = obj->texture->alpha, sin_a = 0, cos_a = 0, i_cos = 0, i_sin = 0;
		const void *pix_col;
		uint32_t pix_col32;
		uint8_t r = 0, g = 0, b = 0;
		uint8_t data_width;
		GRAPH_getcolor GetColor;
		if (obj->texture->features & MGL_TEXTURE_REPEAT_Y) {
			i1 = (y - y_min) % i_h;
		}
		else {
			i1 = (i_h * (y - y_min)) / y_h;
		}
		if (obj->texture->features & MGL_TEXTURE_FLIP_Y) {
			i1 = i_h - i1 - 1;
		}
		switch(image->mode) {
			case MGL_IMAGE_COLOR_R3G3B2:
				data_width = 1;
				GetColor = GRAPH_getcolor_R3G3B2;
				break;
			case MGL_IMAGE_COLOR_R5G6B5:
				data_width = 2;
				GetColor = GRAPH_getcolor_R5G6B5;
				break;
			case MGL_IMAGE_COLOR_A4R4G4B4:
				data_width = 2;
				GetColor = GRAPH_getcolor_A4R4G4B4;
				break;
			case MGL_IMAGE_COLOR_R8G8B8:
				data_width = 3;
				GetColor = GRAPH_getcolor_R8G8B8;
				break;
			case MGL_IMAGE_COLOR_A8R8G8B8:
				data_width = 4;
				GetColor = GRAPH_getcolor_A8R8G8B8;
				break;
			default:
				data_width = 0;
				GetColor = GRAPH_getcolor_unknow;
				break;
		}
		if (alpha != 0)	{
			sin_a = MGL_sin(alpha);
			cos_a = MGL_cos(alpha);
			i_sin = (i1 - (i_h >> 1)) * sin_a;
			i_cos = (i1 - (i_h >> 1)) * cos_a;
		}
		for (x = x_start; x <= x_end; x++) {
			if (obj->texture->features & MGL_TEXTURE_REPEAT_X) {
				j1 = (x - x_min) % i_w;
			}
			else {
				j1 = (i_w * (x - x_min)) / x_w;
			}
			if (obj->texture->features & MGL_TEXTURE_FLIP_X) {
				j1 = i_w - j1 - 1;
			}
			if (alpha != 0)	{
				j1_n = (((j1 - (i_w >> 1)) * cos_a - i_sin)>>15) + (i_w >> 1);
				i1_n = (((j1 - (i_w >> 1)) * sin_a + i_cos)>>15) + (i_h >> 1);
				if (i1_n < 0) i1_n = 0;
				if (j1_n < 0) j1_n = 0;
				if (i1_n >= i_h) i1_n = i_h - 1;
				if (j1_n >= i_w) j1_n = i_w - 1;
				pix_col = image->data + (i1_n * i_w + j1_n) * data_width;
			}
			else {
				pix_col = image->data + (i1 * i_w + j1) * data_width;
			}
			GetColor(pix_col, &a, &r, &g, &b);
			if (!a) {	//Absolutely transparent pixel texture
				continue;
			}
			pix_col32 = (r << 16) | (g << 8) | b;
			buffer[x - x0] = MGL_NewColor(buffer[x - x0], pix_col32, tr);
		}
	}

	if (!obj->gradient) return;
	uint32_t color1, color2;
	int pr1, pr2, pr11, pr12;
	MGL_GRADIENT_POINT *list = obj->gradient->points_list;
	pr1 = list->offset;
	color1 = list->color;
	while (list->next) {
		list = (MGL_GRADIENT_POINT *)list->next;
	}
	pr2 = list->offset;
	color2 = list->color;
	if (obj->gradient->g_type == MGL_GRADIENT_LINEAR && obj->gradient->deg == 0) {
		uint8_t yp = 100 - 100 * (y - y_min) / y_h;
		if (yp <= pr1) col_gr = color1;
		else if (yp >= pr2 || (pr2 == pr1)) col_gr = color2;
		else {
			list = obj->gradient->points_list;
			pr11 = list->offset;
			while (list->next) {
				pr12 = ((MGL_GRADIENT_POINT *)list->next)->offset;
				if (pr11 <= yp && yp <= pr12)	{
					if (list->flag_mix && !(pr12 == pr11)) {
						a = (255 * (yp - pr11)) / (pr12 - pr11);
						col_gr = MGL_Color_gradient(list->color, ((MGL_GRADIENT_POINT *)list->next)->color, a);
					}
					else {
						col_gr = list->color;
					}
					break;
				}
				pr11 = pr12;
				list = (MGL_GRADIENT_POINT *)list->next;
			}
		}
		for (x = x_start; x <= x_end; x++) {
			buffer[x - x0] = MGL_NewColor(buffer[x - x0], col_gr, tr);
		}
		return;
	}
	if (obj->gradient->g_type == MGL_GRADIENT_LINEAR && obj->gradient->deg == 90) {
		uint8_t xp;
		for (x = x_start; x <= x_end; x++) {
			xp = (100 * (x - x_min)) / x_w;
			if (xp <= pr1) col_gr = color1;
			else if (xp >= pr2 || (pr2 == pr1)) col_gr = color2;
			else {
				list = obj->gradient->points_list;
				pr11 = list->offset;
				while (list->next) {
					pr12 = ((MGL_GRADIENT_POINT *)list->next)->offset;
					if (pr11 <= xp && xp <= pr12) {
						if (list->flag_mix && !(pr12 == pr11)) {
							a = (255 * (xp - pr11)) / (pr12 - pr11);
							col_gr = MGL_Color_gradient(list->color, ((MGL_GRADIENT_POINT *)list->next)->color, a);
						}
						else {
							col_gr = list->color;
						}
						break;
					}
					pr11 = pr12;
					list = (MGL_GRADIENT_POINT *)list->next;
				}
			}
			buffer[x - x0] = MGL_NewColor(buffer[x - x0], col_gr, tr);
		}
		return;
	}
	int dist, a_y = 0, sin_a = 0, cos_a = 0, len;
	int deg = obj->gradient->deg;
	if (obj->gradient->g_type == MGL_GRADIENT_LINEAR) {
		sin_a = MGL_sin(deg);
		cos_a = MGL_cos(deg);
		len = MGL_fabs(y_h * cos_a) + MGL_fabs(x_w * sin_a);
		a_y = (y - y_min - (y_h >> 1)) * cos_a;
	}
	else {
		len = ((int)MGL_sqrt(x_w * x_w + y_h * y_h)) >> 1;
	}
	list = obj->gradient->points_list;
	for (x = x_start; x <= x_end; x++) {
		if (obj->gradient->g_type == MGL_GRADIENT_LINEAR) {
			dist = 50 - 100 * ((x_min + (x_w >> 1) - x) * sin_a + a_y) / len;
		}
		else {
			dist = 100 * MGL_sqrt((x - x_min - (x_w >> 1)) * (x - x_min - (x_w >> 1)) + (y - y_min - (y_h >> 1)) * (y - y_min - (y_h >> 1))) / len;
		}
		if (dist <= pr1) col_gr = color1;
		else if (dist >= pr2 || (pr2 == pr1)) col_gr = color2;
		else {
			list = obj->gradient->points_list;
			pr11 = list->offset;
			while (list->next) {
				pr12 = ((MGL_GRADIENT_POINT *)list->next)->offset;
				if (pr11 <= dist && dist <= pr12) {
					if (list->flag_mix && !(pr12 == pr11)) {
						a = (255 * (dist - pr11)) / (pr12 - pr11);
						col_gr = MGL_Color_gradient(list->color, ((MGL_GRADIENT_POINT *)list->next)->color, a);
					}
					else {
						col_gr = list->color;
					}
					break;
				}
				pr11 = pr12;
				list = (MGL_GRADIENT_POINT *)list->next;
			}
		}
		buffer[x - x0] = MGL_NewColor(buffer[x - x0], col_gr, tr);
	}
}

static inline void MGL_RenderObj(MGL_OBJ *obj, uint16_t *render_buf, int x0, int x1, int y)
{
	if (!obj->visible) return;
	MGL_OBJ_TRIANGLE *obj_triangle;
	MGL_OBJ_RECTANGLE *obj_rectangle;
	MGL_OBJ_CIRCLE *obj_circle;
	MGL_OBJ_TEXT *obj_text;
	int x, x_start, x_end, xmin, xmax;
	int x_mas[8], x_min, x_max;
	uint8_t c_mas, match;
	uint32_t col_gr;
	switch(obj->obj_type) {
		case MGL_OBJ_TYPE_TRIANGLE:
		case MGL_OBJ_TYPE_FILLTRIANGLE:
			obj_triangle = (MGL_OBJ_TRIANGLE*)obj->object;
			if (y < obj_triangle->y1 ||	y > obj_triangle->y3) return;
			xmin = min3(obj_triangle->x1, obj_triangle->x2, obj_triangle->x3);
			xmax = max3(obj_triangle->x1, obj_triangle->x2, obj_triangle->x3);
			if (xmax < x0 || xmin > x1) return;
			c_mas = match = 0;
			if (MGL_LinesIntersection(obj_triangle->x1, obj_triangle->y1, obj_triangle->x2, obj_triangle->y2, y, &x_mas[c_mas], &x_mas[c_mas + 1], &match)) {
				c_mas += 2;
			}
			if (MGL_LinesIntersection(obj_triangle->x2, obj_triangle->y2, obj_triangle->x3, obj_triangle->y3, y, &x_mas[c_mas], &x_mas[c_mas + 1], &match)) {
				c_mas += 2;
			}
			if (MGL_LinesIntersection(obj_triangle->x3, obj_triangle->y3, obj_triangle->x1, obj_triangle->y1, y, &x_mas[c_mas], &x_mas[c_mas + 1], &match)) {
				c_mas += 2;
			}
			if (!c_mas) return;
			x_min = x_max = x_mas[0];
			while (c_mas) {
				x_min = min(x_min, x_mas[c_mas - 2]);
				x_max = max(x_max, x_mas[c_mas - 1]);
				c_mas -= 2;
			}
			x_start = x_min < x0 ? x0: x_min;
			x_end = x_max > x1? x1: x_max;
			if (obj->obj_type == MGL_OBJ_TYPE_FILLTRIANGLE || match) {
				MGL_setcolorbuffer(obj, render_buf, x0, y,
							   	   x_start, x_end, xmin, obj_triangle->y1,
								   xmax - xmin + 1, obj_triangle->y3 - obj_triangle->y1 + 1,
								   obj_triangle->color);
			}
			else {
				MGL_setcolorbuffer(obj, render_buf, x0, y,
								   x_start, x_start, xmin, obj_triangle->y1,
								   xmax - xmin + 1, obj_triangle->y3 - obj_triangle->y1 + 1,
								   obj_triangle->color);
				MGL_setcolorbuffer(obj, render_buf, x0, y,
								   x_end, x_end, xmin, obj_triangle->y1,
								   xmax - xmin + 1, obj_triangle->y3 - obj_triangle->y1 + 1,
								   obj_triangle->color);
			}
			break;
		case MGL_OBJ_TYPE_RECTANGLE:
		case MGL_OBJ_TYPE_FILLRECTANGLE:
			obj_rectangle = (MGL_OBJ_RECTANGLE*)obj->object;
			if (y < obj_rectangle->y1 || y > obj_rectangle->y2) return;
			if (obj_rectangle->x2 < x0 || obj_rectangle->x1 > x1) return;
			x_start = obj_rectangle->x1 < x0 ? x0: obj_rectangle->x1;
			x_end = obj_rectangle->x2 > x1 ? x1: obj_rectangle->x2;
			if (obj->obj_type == MGL_OBJ_TYPE_FILLRECTANGLE || y == obj_rectangle->y1 || y == obj_rectangle->y2) {
				MGL_setcolorbuffer(obj, render_buf, x0, y,
								   x_start, x_end, obj_rectangle->x1, obj_rectangle->y1,
								   obj_rectangle->x2 - obj_rectangle->x1 + 1, obj_rectangle->y2 - obj_rectangle->y1 + 1,
								   obj_rectangle->color);
			}
			else {
				if (x_start == obj_rectangle->x1) {
					MGL_setcolorbuffer(obj, render_buf, x0, y,
									   x_start, x_start, obj_rectangle->x1, obj_rectangle->y1,
									   obj_rectangle->x2 - obj_rectangle->x1 + 1, obj_rectangle->y2 - obj_rectangle->y1 + 1,
									   obj_rectangle->color);
				}
				if (x_end == obj_rectangle->x2) {
					MGL_setcolorbuffer(obj, render_buf, x0, y,
									   x_end, x_end, obj_rectangle->x1, obj_rectangle->y1,
									   obj_rectangle->x2 - obj_rectangle->x1 + 1, obj_rectangle->y2 - obj_rectangle->y1 + 1,
									   obj_rectangle->color);
				}
			}
			break;
		case MGL_OBJ_TYPE_CIRCLE:
		case MGL_OBJ_TYPE_FILLCIRCLE:
			obj_circle = (MGL_OBJ_CIRCLE*)obj->object;
			if (y < obj_circle->y - obj_circle->r + 1 ||
				y > obj_circle->y + obj_circle->r - 1)	return;
			if (x0 > obj_circle->x + obj_circle->r - 1 ||
				x1 < obj_circle->x - obj_circle->r + 1)	return;
			float tmp_s = obj_circle->r * obj_circle->r - (y - obj_circle->y) * (y - obj_circle->y);
			if (tmp_s < 0) return;
			tmp_s = MGL_sqrt(tmp_s);
			x_start = obj_circle->x - tmp_s;
			x_end = obj_circle->x + tmp_s;
			if (x_start < x0) x_start = x0;
			if (x_end > x1) x_end = x1;
			MGL_setcolorbuffer(obj, render_buf, x0, y,
							   x_start, x_end, obj_circle->x - obj_circle->r, obj_circle->y - obj_circle->r,
							   2 * obj_circle->r, 2 * obj_circle->r, obj_circle->color);
			break;
		case MGL_OBJ_TYPE_TEXT:
			obj_text = (MGL_OBJ_TEXT*)obj->object;
			if ( y >= obj_text->y &&
				 y < obj_text->y + obj_text->font->height) {
				int row = y - obj_text->y, col;
				FontDef *font = obj_text->font;
				char *txt = obj_text->txt;
				int x_txt = obj_text->x;
				int bytes_per_line = ((font->width - 1) >> 3) + 1;
				if (bytes_per_line > 4) { //Support character widths up to 32 pixels (4 bytes per line)
					return;
				}
				int k = 1 << ((bytes_per_line << 3 ) - 1), num_sym;
				int len_txt = my_strlen(txt);
				int tt = x_txt + len_txt*font->width - 1;
				if (tt < x0 || x_txt > x1) return;
				x_start = x_txt < x0 ? x0: x_txt;
				x_end = tt > x1 ? x1: tt;
				uint32_t tmp;
				const void *b;
				char ch;
				col_gr = obj_text->color;
				for (x = x_start; x <= x_end; x++) {
					num_sym = (x - x_txt) / font->width;
					if (num_sym >= len_txt) return;
					ch = txt[num_sym];
					ch = ch < font->firstcode || ch > font->lastcode ? 0: ch - font->firstcode;
					col = (x - x_txt) % font->width;
					b = font->data + bytes_per_line * (ch * font->height + row);
					if (bytes_per_line == 1)      { tmp = *((uint8_t*)b);  }
					else if (bytes_per_line == 2) { tmp = *((uint16_t*)b); }
					else if (bytes_per_line == 3) { tmp = (*((uint8_t*)b)) | ((*((uint8_t*)(b + 1))) << 8) |  ((*((uint8_t*)(b + 2))) << 16); }
					else { tmp = *((uint32_t*)b); }
					if (obj_text->bold) tmp |= tmp >> 1;
					if ((tmp << col) & k) {
						MGL_setcolorbuffer(obj, render_buf, x0, y,
										   x, x, x_txt, obj_text->y,
										   tt - x_txt + 1, font->height, col_gr);
					}
				}
			}
			break;
		default:
			break;
	}
}

//Draws objects (parts thereof) that are in the current output window into the buffer
void MGL_RenderObjects(MGL_OBJ *obj, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *data)
{
	uint16_t *render_ptr = data;
	MGL_OBJ *obj_ptr;
	for (uint16_t y = y0; y <= y1; y++) {
		obj_ptr = obj;
		while (obj_ptr) {
			MGL_RenderObj(obj_ptr, render_ptr, x0, x1, y);
			obj_ptr = obj_ptr->next;
		}
		render_ptr += (x1 - x0) + 1;
	}
}

//Moves the object a distance along the x axis by dx, along the y axis by dy
void MGL_ObjectMove(MGL_OBJ *obj, int dx, int dy)
{
	switch(obj->obj_type) {
		case MGL_OBJ_TYPE_TRIANGLE:
		case MGL_OBJ_TYPE_FILLTRIANGLE:
			((MGL_OBJ_TRIANGLE*)obj->object)->x1 += dx;
			((MGL_OBJ_TRIANGLE*)obj->object)->x2 += dx;
			((MGL_OBJ_TRIANGLE*)obj->object)->x3 += dx;
			((MGL_OBJ_TRIANGLE*)obj->object)->y1 += dy;
			((MGL_OBJ_TRIANGLE*)obj->object)->y2 += dy;
			((MGL_OBJ_TRIANGLE*)obj->object)->y3 += dy;
			break;
		case MGL_OBJ_TYPE_RECTANGLE:
		case MGL_OBJ_TYPE_FILLRECTANGLE:
			((MGL_OBJ_RECTANGLE*)obj->object)->x1 += dx;
			((MGL_OBJ_RECTANGLE*)obj->object)->x2 += dx;
			((MGL_OBJ_RECTANGLE*)obj->object)->y1 += dy;
			((MGL_OBJ_RECTANGLE*)obj->object)->y2 += dy;
			break;
		case MGL_OBJ_TYPE_CIRCLE:
		case MGL_OBJ_TYPE_FILLCIRCLE:
			((MGL_OBJ_CIRCLE*)obj->object)->x += dx;
			((MGL_OBJ_CIRCLE*)obj->object)->y += dy;
			break;
		case MGL_OBJ_TYPE_TEXT:
			((MGL_OBJ_TEXT*)obj->object)->x += dx;
			((MGL_OBJ_TEXT*)obj->object)->y += dy;
			break;
		default:
			break;
	}
}

//Moves all objects in the list by a distance along the x axis to dx, along the y axis to dy
void MGL_ObjectListMove(MGL_OBJ *obj_list, int dx, int dy)
{
	while(obj_list)	{
		MGL_ObjectMove(obj_list, dx, dy);
		obj_list = obj_list->next;
	}
}

//Sets transparency for list objects
//0 - completely opaque. 255 - invisible
void MGL_ObjectListTransparency(MGL_OBJ *obj_list, uint8_t tr)
{
	while(obj_list)	{
		MGL_ObjectSetTransparency(obj_list, tr);
		obj_list = obj_list->next;
	}
}

