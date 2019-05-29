/*
** Copyright 1991-1993, Silicon Graphics, Inc.
** All Rights Reserved.
** 
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of Silicon Graphics, Inc.
** 
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
*/

#ifndef __glu_h__
#define __glu_h__

#include <GL/gl.h>

/*************************************************************/

/* Boolean */
#define GLU_FALSE                            0
#define GLU_TRUE                             1

/* Version */
#define GLU_VERSION_1_1                      1
#define GLU_VERSION_1_2                      1

/* StringName */
#define GLU_VERSION                          100800
#define GLU_EXTENSIONS                       100801

/* ErrorCode */
#define GLU_INVALID_ENUM                     100900
#define GLU_INVALID_VALUE                    100901
#define GLU_OUT_OF_MEMORY                    100902
#define GLU_INCOMPATIBLE_GL_VERSION          100903

/* NurbsDisplay */
/*      GLU_FILL */
#define GLU_OUTLINE_POLYGON                  100240
#define GLU_OUTLINE_PATCH                    100241

/* NurbsCallback */
#define GLU_ERROR                            100103

/* NurbsError */
#define GLU_NURBS_ERROR1                     100251
#define GLU_NURBS_ERROR2                     100252
#define GLU_NURBS_ERROR3                     100253
#define GLU_NURBS_ERROR4                     100254
#define GLU_NURBS_ERROR5                     100255
#define GLU_NURBS_ERROR6                     100256
#define GLU_NURBS_ERROR7                     100257
#define GLU_NURBS_ERROR8                     100258
#define GLU_NURBS_ERROR9                     100259
#define GLU_NURBS_ERROR10                    100260
#define GLU_NURBS_ERROR11                    100261
#define GLU_NURBS_ERROR12                    100262
#define GLU_NURBS_ERROR13                    100263
#define GLU_NURBS_ERROR14                    100264
#define GLU_NURBS_ERROR15                    100265
#define GLU_NURBS_ERROR16                    100266
#define GLU_NURBS_ERROR17                    100267
#define GLU_NURBS_ERROR18                    100268
#define GLU_NURBS_ERROR19                    100269
#define GLU_NURBS_ERROR20                    100270
#define GLU_NURBS_ERROR21                    100271
#define GLU_NURBS_ERROR22                    100272
#define GLU_NURBS_ERROR23                    100273
#define GLU_NURBS_ERROR24                    100274
#define GLU_NURBS_ERROR25                    100275
#define GLU_NURBS_ERROR26                    100276
#define GLU_NURBS_ERROR27                    100277
#define GLU_NURBS_ERROR28                    100278
#define GLU_NURBS_ERROR29                    100279
#define GLU_NURBS_ERROR30                    100280
#define GLU_NURBS_ERROR31                    100281
#define GLU_NURBS_ERROR32                    100282
#define GLU_NURBS_ERROR33                    100283
#define GLU_NURBS_ERROR34                    100284
#define GLU_NURBS_ERROR35                    100285
#define GLU_NURBS_ERROR36                    100286
#define GLU_NURBS_ERROR37                    100287

/* NurbsProperty */
#define GLU_AUTO_LOAD_MATRIX                 100200
#define GLU_CULLING                          100201
#define GLU_SAMPLING_TOLERANCE               100203
#define GLU_DISPLAY_MODE                     100204
#define GLU_PARAMETRIC_TOLERANCE             100202
#define GLU_SAMPLING_METHOD                  100205
#define GLU_U_STEP                           100206
#define GLU_V_STEP                           100207
#define GLU_OBJECT_SPACE_ERROR_TOLERANCE     100208

/* NurbsSampling */
#define GLU_PATH_LENGTH                      100215
#define GLU_PARAMETRIC_ERROR                 100216
#define GLU_DOMAIN_DISTANCE                  100217
#define GLU_OBJECT_SPACE                     100218

/* NurbsTrim */
#define GLU_MAP1_TRIM_2                      100210
#define GLU_MAP1_TRIM_3                      100211

/* QuadricDrawStyle */
#define GLU_POINT                            100010
#define GLU_LINE                             100011
#define GLU_FILL                             100012
#define GLU_SILHOUETTE                       100013

/* QuadricCallback */
/*      GLU_ERROR */

/* QuadricNormal */
#define GLU_SMOOTH                           100000
#define GLU_FLAT                             100001
#define GLU_NONE                             100002

/* QuadricOrientation */
#define GLU_OUTSIDE                          100020
#define GLU_INSIDE                           100021

/* TessCallback */
#define GLU_TESS_BEGIN                       100100
#define GLU_BEGIN                            100100
#define GLU_TESS_VERTEX                      100101
#define GLU_VERTEX                           100101
#define GLU_TESS_END                         100102
#define GLU_END                              100102
#define GLU_TESS_ERROR                       100103
#define GLU_TESS_EDGE_FLAG                   100104
#define GLU_EDGE_FLAG                        100104
#define GLU_TESS_COMBINE                     100105
#define GLU_TESS_BEGIN_DATA                  100106
#define GLU_TESS_VERTEX_DATA                 100107
#define GLU_TESS_END_DATA                    100108
#define GLU_TESS_ERROR_DATA                  100109
#define GLU_TESS_EDGE_FLAG_DATA              100110
#define GLU_TESS_COMBINE_DATA                100111

/* TessContour */
#define GLU_CW                               100120
#define GLU_CCW                              100121
#define GLU_INTERIOR                         100122
#define GLU_EXTERIOR                         100123
#define GLU_UNKNOWN                          100124

/* TessProperty */
#define GLU_TESS_WINDING_RULE                100140
#define GLU_TESS_BOUNDARY_ONLY               100141
#define GLU_TESS_TOLERANCE                   100142

/* TessError */
#define GLU_TESS_ERROR1                      100151
#define GLU_TESS_ERROR2                      100152
#define GLU_TESS_ERROR3                      100153
#define GLU_TESS_ERROR4                      100154
#define GLU_TESS_ERROR5                      100155
#define GLU_TESS_ERROR6                      100156
#define GLU_TESS_ERROR7                      100157
#define GLU_TESS_ERROR8                      100158
#define GLU_TESS_MISSING_BEGIN_POLYGON       100151
#define GLU_TESS_MISSING_BEGIN_CONTOUR       100152
#define GLU_TESS_MISSING_END_POLYGON         100153
#define GLU_TESS_MISSING_END_CONTOUR         100154
#define GLU_TESS_COORD_TOO_LARGE             100155
#define GLU_TESS_NEED_COMBINE_CALLBACK       100156

/* TessWinding */
#define GLU_TESS_WINDING_ODD                 100130
#define GLU_TESS_WINDING_NONZERO             100131
#define GLU_TESS_WINDING_POSITIVE            100132
#define GLU_TESS_WINDING_NEGATIVE            100133
#define GLU_TESS_WINDING_ABS_GEQ_TWO         100134

/*************************************************************/


#ifdef __cplusplus
class GLUnurbs;
class GLUquadric;
class GLUtesselator;
extern "C" {
#else
typedef struct GLUnurbs GLUnurbs;
typedef struct GLUquadric GLUquadric;
typedef struct GLUtesselator GLUtesselator;
#endif

typedef struct GLUnurbs GLUnurbsObj;
typedef struct GLUquadric GLUquadricObj;
typedef struct GLUtesselator GLUtesselatorObj;
typedef struct GLUtesselator GLUtriangulatorObj;

#define GLU_TESS_MAX_COORD 1.0e150

_IMPEXP_GL void gluBeginCurve (GLUnurbs* nurb);
_IMPEXP_GL void gluBeginPolygon (GLUtesselator* tess);
_IMPEXP_GL void gluBeginSurface (GLUnurbs* nurb);
_IMPEXP_GL void gluBeginTrim (GLUnurbs* nurb);
_IMPEXP_GL GLint gluBuild1DMipmaps (GLenum target, GLint component, GLsizei width, GLenum format, GLenum type, const void *data);
_IMPEXP_GL GLint gluBuild2DMipmaps (GLenum target, GLint component, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);
_IMPEXP_GL void gluCylinder (GLUquadric* quad, GLdouble base, GLdouble top, GLdouble height, GLint slices, GLint stacks);
_IMPEXP_GL void gluDeleteNurbsRenderer (GLUnurbs* nurb);
_IMPEXP_GL void gluDeleteQuadric (GLUquadric* quad);
_IMPEXP_GL void gluDeleteTess (GLUtesselator* tess);
_IMPEXP_GL void gluDisk (GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops);
_IMPEXP_GL void gluEndCurve (GLUnurbs* nurb);
_IMPEXP_GL void gluEndPolygon (GLUtesselator* tess);
_IMPEXP_GL void gluEndSurface (GLUnurbs* nurb);
_IMPEXP_GL void gluEndTrim (GLUnurbs* nurb);
_IMPEXP_GL const GLubyte * gluErrorString (GLenum error);
_IMPEXP_GL void gluGetNurbsProperty (GLUnurbs* nurb, GLenum property, GLfloat* data);
_IMPEXP_GL const GLubyte * gluGetString (GLenum name);
_IMPEXP_GL void gluGetTessProperty (GLUtesselator* tess, GLenum which, GLdouble* data);
_IMPEXP_GL void gluLoadSamplingMatrices (GLUnurbs* nurb, const GLfloat *model, const GLfloat *perspective, const GLint *view);
_IMPEXP_GL void gluLookAt (GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY, GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ);
_IMPEXP_GL GLUnurbs* gluNewNurbsRenderer (void);
_IMPEXP_GL GLUquadric* gluNewQuadric (void);
_IMPEXP_GL GLUtesselator* gluNewTess (void);
_IMPEXP_GL void gluNextContour (GLUtesselator* tess, GLenum type);
_IMPEXP_GL void gluNurbsCallback (GLUnurbs* nurb, GLenum which, GLvoid (*CallBackFunc)());
_IMPEXP_GL void gluNurbsCurve (GLUnurbs* nurb, GLint knotCount, GLfloat *knots, GLint stride, GLfloat *control, GLint order, GLenum type);
_IMPEXP_GL void gluNurbsProperty (GLUnurbs* nurb, GLenum property, GLfloat value);
_IMPEXP_GL void gluNurbsSurface (GLUnurbs* nurb, GLint sKnotCount, GLfloat* sKnots, GLint tKnotCount, GLfloat* tKnots, GLint sStride, GLint tStride, GLfloat* control, GLint sOrder, GLint tOrder, GLenum type);
_IMPEXP_GL void gluOrtho2D (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
_IMPEXP_GL void gluPartialDisk (GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops, GLdouble start, GLdouble sweep);
_IMPEXP_GL void gluPerspective (GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
_IMPEXP_GL void gluPickMatrix (GLdouble x, GLdouble y, GLdouble delX, GLdouble delY, GLint *viewport);
_IMPEXP_GL GLint gluProject (GLdouble objX, GLdouble objY, GLdouble objZ, const GLdouble *model, const GLdouble *proj, const GLint *view, GLdouble* winX, GLdouble* winY, GLdouble* winZ);
_IMPEXP_GL void gluPwlCurve (GLUnurbs* nurb, GLint count, GLfloat* data, GLint stride, GLenum type);
_IMPEXP_GL void gluQuadricCallback (GLUquadric* quad, GLenum which, GLvoid (*CallBackFunc)());
_IMPEXP_GL void gluQuadricDrawStyle (GLUquadric* quad, GLenum draw);
_IMPEXP_GL void gluQuadricNormals (GLUquadric* quad, GLenum normal);
_IMPEXP_GL void gluQuadricOrientation (GLUquadric* quad, GLenum orientation);
_IMPEXP_GL void gluQuadricTexture (GLUquadric* quad, GLboolean texture);
_IMPEXP_GL GLint gluScaleImage (GLenum format, GLsizei wIn, GLsizei hIn, GLenum typeIn, const void *dataIn, GLsizei wOut, GLsizei hOut, GLenum typeOut, GLvoid* dataOut);
_IMPEXP_GL void gluSphere (GLUquadric* quad, GLdouble radius, GLint slices, GLint stacks);
_IMPEXP_GL void gluTessBeginContour (GLUtesselator* tess);
_IMPEXP_GL void gluTessBeginPolygon (GLUtesselator* tess, GLvoid* data);
_IMPEXP_GL void gluTessCallback (GLUtesselator* tess, GLenum which, GLvoid (*CallBackFunc)());
_IMPEXP_GL void gluTessEndContour (GLUtesselator* tess);
_IMPEXP_GL void gluTessEndPolygon (GLUtesselator* tess);
_IMPEXP_GL void gluTessNormal (GLUtesselator* tess, GLdouble valueX, GLdouble valueY, GLdouble valueZ);
_IMPEXP_GL void gluTessProperty (GLUtesselator* tess, GLenum which, GLdouble data);
_IMPEXP_GL void gluTessVertex (GLUtesselator* tess, GLdouble *location, GLvoid* data);
_IMPEXP_GL GLint gluUnProject (GLdouble winX, GLdouble winY, GLdouble winZ, const GLdouble *model, const GLdouble *proj, const GLint *view, GLdouble* objX, GLdouble* objY, GLdouble* objZ);

#ifdef __cplusplus
}
#endif

#endif /* __glu_h__ */
