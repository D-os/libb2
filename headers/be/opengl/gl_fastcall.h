#ifndef __FASTCALL_H__
#define __FASTCALL_H__

extern struct __glContextRec * __gl;

#define GLIM_BEGIN( gc, prim ) { 															\
		GLuint t1, t2;																		\
		__asm__ __volatile__( 																\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/ 							\
		"call * 0x0060(%%eax) \n\t"		/* glBegin				*/ 							\
		: "=c"(t1), "=d"(t2)																\
		: "0"(gc), "1"(prim) 																\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_END( gc ) {																			\
		GLuint t1;																					\
		__asm__ __volatile__ ( 																		\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/ 									\
		"call * 0x0064(%%eax) \n\t"		/* glEnd				*/ 									\
		: "=c"(t1)																					\
		: "0"(gc) 																					\
		: "%eax", "%edx", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

/***********************************************************************************/
/* End Misc 																	   */
/* 																				   */
/* Start Normal																	   */
/***********************************************************************************/

#define GLIM_NORMAL3F( gc, x, y, z ) { 								\
		GLuint t1;													\
		__asm__ __volatile__ ( 										\
		"flds %3 \n\t"					/* z					*/	\
		"flds %2 \n\t"					/* y					*/	\
		"flds %1 \n\t"					/* x					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0040(%%eax) \n\t"		/* normal3fv			*/	\
		: "=c"(t1) 													\
		: "m"(x), "m"(y), "m"(z), "0"(gc) 							\
		: "%eax", "%edx", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_NORMAL3FV( gc, v ) { 									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"flds 8(%2) \n\t"				/* z					*/ 	\
		"flds 4(%2) \n\t"				/* y					*/	\
		"flds (%2) \n\t"				/* x					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0040(%%eax) \n\t"		/* normal3fv			*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

/***********************************************************************************/
/* End Normal 																	   */
/* 																				   */
/* Start Vertex																	   */
/***********************************************************************************/

#define GLIM_VERTEX2F( gc, x, y ) {									\
		GLuint t1;													\
		__asm__ __volatile__ ( 										\
		"fldz \n\t"						/* z					*/	\
		"flds %2 \n\t"					/* y					*/	\
		"flds %1 \n\t"					/* x					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0058(%%eax) \n\t"		/* vertex3fv			*/	\
		: "=c"(t1) 													\
		: "m"(x), "m"(y), "0"(gc) 									\
		: "%eax", "%edx", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_VERTEX2FV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"fldz \n\t"						/* z					*/	\
		"flds 4(%2) \n\t"				/* y					*/	\
		"flds (%2) \n\t"				/* x					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0058(%%eax) \n\t"		/* vertex3fv			*/	\
		: "=d"(t1), "=c"(t2)										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_VERTEX3F( gc, x, y, z ) {								\
		GLuint t1;													\
		__asm__ __volatile__ ( 										\
		"flds %3 \n\t"					/* z					*/	\
		"flds %2 \n\t"					/* y					*/	\
		"flds %1 \n\t"					/* x					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0058(%%eax) \n\t"		/* vertex3fv			*/	\
		: "=c"(t1) 													\
		: "m"(x), "m"(y), "m"(z), "0"(gc) 							\
		: "%eax", "%edx", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_VERTEX3FV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"flds 8(%2) \n\t"				/* z					*/	\
		"flds 4(%2) \n\t"				/* y					*/	\
		"flds (%2) \n\t"				/* x					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0058(%%eax) \n\t"		/* vertex3fv			*/	\
		: "=d"(t1), "=c"(t2)										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }


#define GLIM_VERTEX4F( gc, x, y, z, w ) {							\
		GLuint t1;													\
		__asm__ __volatile__ ( 										\
		"flds %4 \n\t"					/* w					*/	\
		"flds %3 \n\t"					/* z					*/	\
		"flds %2 \n\t"					/* y					*/	\
		"flds %1 \n\t"					/* x					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x005C(%%eax) \n\t"		/* vertex3fv			*/	\
		: "=c"(t1) 													\
		: "m"(x), "m"(y), "m"(z), "m"(w), "0"(gc)					\
		: "%eax", "%edx", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_VERTEX4FV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"flds 12(%2) \n\t"				/* w					*/	\
		"flds 8(%2) \n\t"				/* z					*/	\
		"flds 4(%2) \n\t"				/* y					*/	\
		"flds (%2) \n\t"				/* x					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x005C(%%eax) \n\t"		/* vertex3fv			*/	\
		: "=d"(t1), "=c"(t2)										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

/***********************************************************************************/
/* End Vertex 																	   */
/* 																				   */
/* Start Texture																   */
/***********************************************************************************/

#define GLIM_TEXCOORD1F( gc, s ) {									\
		GLuint t1;													\
		__asm__ __volatile__ ( 										\
		"flds %1 \n\t"					/* s					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0044(%%eax) \n\t"		/* texCoord1			*/	\
		: "=c"(t1)													\
		: "m"(s), "0"(gc) 											\
		: "%eax", "%edx", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_TEXCOORD1FV( gc, v ) { 								\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"flds (%2) \n\t"				/* s					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0044(%%eax) \n\t"		/* texCoord1			*/	\
		: "=d"(t1), "=c"(t2)										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_TEXCOORD2F( gc, s, t ) {								\
		GLuint t1;													\
		__asm__ __volatile__ ( 										\
		"flds %2 \n\t"					/* t					*/	\
		"flds %1 \n\t"					/* s					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0048(%%eax) \n\t"		/* texCoord2			*/	\
		: "=c"(t1)													\
		: "m"(s), "m"(t), "0"(gc)									\
		: "%eax", "%edx", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_TEXCOORD2FV( gc, v ) { 								\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"flds 4(%2) \n\t"				/* t					*/	\
		"flds (%2) \n\t"				/* s					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0048(%%eax) \n\t"		/* texCoord2			*/	\
		: "=d"(t1), "=c"(t2)										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_TEXCOORD3F( gc, s, t, r ) {							\
		GLuint t1;													\
		__asm__ __volatile__ ( 										\
		"flds %3 \n\t"					/* r					*/	\
		"flds %2 \n\t"					/* t					*/	\
		"flds %1 \n\t"					/* s					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x004C(%%eax) \n\t"		/* texCoord3			*/	\
		: "=c"(t1)													\
		: "m"(s), "m"(t), "m"(r), "0"(gc)							\
		: "%eax", "%edx", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_TEXCOORD3FV( gc, v ) { 								\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"flds 8(%2) \n\t"				/* r					*/	\
		"flds 4(%2) \n\t"				/* t					*/	\
		"flds (%2) \n\t"				/* s					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x004C(%%eax) \n\t"		/* texCoord3			*/	\
		: "=d"(t1), "=c"(t2)										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_TEXCOORD4F( gc, s, t, r, q ) {							\
		GLuint t1;													\
		__asm__ __volatile__ ( 										\
		"flds %4 \n\t"					/* q					*/	\
		"flds %3 \n\t"					/* r					*/	\
		"flds %2 \n\t"					/* t					*/	\
		"flds %1 \n\t"					/* s					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0050(%%eax) \n\t"		/* texCoord4			*/	\
		: "=c"(t1)													\
		: "m"(s), "m"(t), "m"(r), "m"(q), "0"(gc)					\
		: "%eax", "%edx", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_TEXCOORD4FV( gc, v ) { 								\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"flds 12(%2) \n\t"				/* q					*/	\
		"flds 8(%2) \n\t"				/* r					*/	\
		"flds 4(%2) \n\t"				/* t					*/	\
		"flds (%2) \n\t"				/* s					*/	\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0050(%%eax) \n\t"		/* texCoord4			*/	\
		: "=d"(t1), "=c"(t2)										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

/***********************************************************************************/
/* End Texture 																	   */
/* 																				   */
/* Start Color																	   */
/***********************************************************************************/

#define GLIM_COLOR3BV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0000(%%eax) \n\t"		/* Color3bv				*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }
#if 0
#define GLIM_COLOR3DV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0004(%%eax) \n\t"		/* Color3dv				*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }
#endif
#define GLIM_COLOR3FV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0008(%%eax) \n\t"		/* Color3fv				*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_COLOR3IV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x000C(%%eax) \n\t"		/* Color3iv				*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_COLOR3SV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0010(%%eax) \n\t"		/* Color3sv				*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_COLOR3UBV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0014(%%eax) \n\t"		/* Color3ubv			*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_COLOR3UIV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0018(%%eax) \n\t"		/* Color3uiv			*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_COLOR3USV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x001C(%%eax) \n\t"		/* Color3usv			*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }


#define GLIM_COLOR4BV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0020(%%eax) \n\t"		/* Color4bv				*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#if 0
#define GLIM_COLOR4DV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0024(%%eax) \n\t"		/* Color4dv				*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }
#endif

#define GLIM_COLOR4FV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0028(%%eax) \n\t"		/* Color4fv				*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_COLOR4IV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x002C(%%eax) \n\t"		/* Color4iv				*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_COLOR4SV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0030(%%eax) \n\t"		/* Color4sv				*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_COLOR4UBV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0034(%%eax) \n\t"		/* Color4ubv			*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_COLOR4UIV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x0038(%%eax) \n\t"		/* Color4uiv			*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#define GLIM_COLOR4USV( gc, v ) {									\
		GLuint t1, t2;												\
		__asm__ __volatile__ ( 										\
		"movl (%%ecx), %%eax \n\t"		/* procTable			*/	\
		"call * 0x003C(%%eax) \n\t"		/* Color4usv			*/	\
		: "=d"(t1), "=c"(t2) 										\
		: "0"(v), "1"(gc) 											\
		: "%eax", "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)" ); }

#endif
