#ifndef __gl_h_
#define __gl_h_

#include <BeBuild.h>

#if ( __INTEL__ && __GNUC__ )
//	#include <gl_fastcall.h>
	#define __GL_USE_FASTCALL__  0
#else
	#define __GL_USE_FASTCALL__  0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
** Copyright 1996 Silicon Graphics, Inc.
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

typedef unsigned long GLenum;
typedef unsigned char GLboolean;
typedef unsigned long GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef long GLint;
typedef long GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned long GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;

/*************************************************************/

/* Version */
#define GL_VERSION_1_1                    1

/* AccumOp */
#define GL_ACCUM                          0x0100
#define GL_LOAD                           0x0101
#define GL_RETURN                         0x0102
#define GL_MULT                           0x0103
#define GL_ADD                            0x0104

/* AlphaFunction */
#define GL_NEVER                          0x0200
#define GL_LESS                           0x0201
#define GL_EQUAL                          0x0202
#define GL_LEQUAL                         0x0203
#define GL_GREATER                        0x0204
#define GL_NOTEQUAL                       0x0205
#define GL_GEQUAL                         0x0206
#define GL_ALWAYS                         0x0207

/* AttribMask */
#define GL_CURRENT_BIT                    0x00000001
#define GL_POINT_BIT                      0x00000002
#define GL_LINE_BIT                       0x00000004
#define GL_POLYGON_BIT                    0x00000008
#define GL_POLYGON_STIPPLE_BIT            0x00000010
#define GL_PIXEL_MODE_BIT                 0x00000020
#define GL_LIGHTING_BIT                   0x00000040
#define GL_FOG_BIT                        0x00000080
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_ACCUM_BUFFER_BIT               0x00000200
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_VIEWPORT_BIT                   0x00000800
#define GL_TRANSFORM_BIT                  0x00001000
#define GL_ENABLE_BIT                     0x00002000
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_HINT_BIT                       0x00008000
#define GL_EVAL_BIT                       0x00010000
#define GL_LIST_BIT                       0x00020000
#define GL_TEXTURE_BIT                    0x00040000
#define GL_SCISSOR_BIT                    0x00080000
#define GL_ALL_ATTRIB_BITS                0x000fffff

/* BeginMode */
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007
#define GL_QUAD_STRIP                     0x0008
#define GL_POLYGON                        0x0009

/* BlendEquationModeEXT */
/*      GL_LOGIC_OP */
/*      GL_FUNC_ADD_EXT */
/*      GL_MIN_EXT */
/*      GL_MAX_EXT */
/*      GL_FUNC_SUBTRACT_EXT */
/*      GL_FUNC_REVERSE_SUBTRACT_EXT */

/* BlendingFactorDest */
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305
/*      GL_CONSTANT_COLOR_EXT */
/*      GL_ONE_MINUS_CONSTANT_COLOR_EXT */
/*      GL_CONSTANT_ALPHA_EXT */
/*      GL_ONE_MINUS_CONSTANT_ALPHA_EXT */

/* BlendingFactorSrc */
/*      GL_ZERO */
/*      GL_ONE */
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_SRC_ALPHA_SATURATE             0x0308
/*      GL_SRC_ALPHA */
/*      GL_ONE_MINUS_SRC_ALPHA */
/*      GL_DST_ALPHA */
/*      GL_ONE_MINUS_DST_ALPHA */
/*      GL_CONSTANT_COLOR_EXT */
/*      GL_ONE_MINUS_CONSTANT_COLOR_EXT */
/*      GL_CONSTANT_ALPHA_EXT */
/*      GL_ONE_MINUS_CONSTANT_ALPHA_EXT */

/* Boolean */
#define GL_TRUE                           1
#define GL_FALSE                          0

/* ClearBufferMask */
/*      GL_COLOR_BUFFER_BIT */
/*      GL_ACCUM_BUFFER_BIT */
/*      GL_STENCIL_BUFFER_BIT */
/*      GL_DEPTH_BUFFER_BIT */

/* ClientArrayType */
/*      GL_VERTEX_ARRAY */
/*      GL_NORMAL_ARRAY */
/*      GL_COLOR_ARRAY */
/*      GL_INDEX_ARRAY */
/*      GL_TEXTURE_COORD_ARRAY */
/*      GL_EDGE_FLAG_ARRAY */

/* ClipPlaneName */
#define GL_CLIP_PLANE0                    0x3000
#define GL_CLIP_PLANE1                    0x3001
#define GL_CLIP_PLANE2                    0x3002
#define GL_CLIP_PLANE3                    0x3003
#define GL_CLIP_PLANE4                    0x3004
#define GL_CLIP_PLANE5                    0x3005

/* ColorMaterialFace */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_FRONT_AND_BACK */

/* ColorMaterialParameter */
/*      GL_AMBIENT */
/*      GL_DIFFUSE */
/*      GL_SPECULAR */
/*      GL_EMISSION */
/*      GL_AMBIENT_AND_DIFFUSE */

/* ColorPointerType */
/*      GL_BYTE */
/*      GL_UNSIGNED_BYTE */
/*      GL_SHORT */
/*      GL_UNSIGNED_SHORT */
/*      GL_INT */
/*      GL_UNSIGNED_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* CullFaceMode */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_FRONT_AND_BACK */

/* DataType */
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_2_BYTES                        0x1407
#define GL_3_BYTES                        0x1408
#define GL_4_BYTES                        0x1409
#define GL_DOUBLE                         0x140A
/* 1.2 defs */
#define GL_UNSIGNED_BYTE_3_3_2            0x8032
#define GL_UNSIGNED_BYTE_2_3_3_REV        0x8362
#define GL_UNSIGNED_SHORT_5_6_5           0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV       0x8364
#define GL_UNSIGNED_SHORT_4_4_4_4         0x8033
#define GL_UNSIGNED_SHORT_4_4_4_4_REV     0x8365
#define GL_UNSIGNED_SHORT_5_5_5_1         0x8034
#define GL_UNSIGNED_SHORT_1_5_5_5_REV     0x8366
#define GL_UNSIGNED_INT_8_8_8_8           0x8035
#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
#define GL_UNSIGNED_INT_10_10_10_2        0x8036
#define GL_UNSIGNED_INT_2_10_10_10_REV    0x8368


/* DepthFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

/* DrawBufferMode */
#define GL_NONE                           0
#define GL_FRONT_LEFT                     0x0400
#define GL_FRONT_RIGHT                    0x0401
#define GL_BACK_LEFT                      0x0402
#define GL_BACK_RIGHT                     0x0403
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405
#define GL_LEFT                           0x0406
#define GL_RIGHT                          0x0407
#define GL_FRONT_AND_BACK                 0x0408
#define GL_AUX0                           0x0409
#define GL_AUX1                           0x040A
#define GL_AUX2                           0x040B
#define GL_AUX3                           0x040C

/* Enable */
/*      GL_FOG */
/*      GL_LIGHTING */
/*      GL_TEXTURE_1D */
/*      GL_TEXTURE_2D */
/*      GL_LINE_STIPPLE */
/*      GL_POLYGON_STIPPLE */
/*      GL_CULL_FACE */
/*      GL_ALPHA_TEST */
/*      GL_BLEND */
/*      GL_INDEX_LOGIC_OP */
/*      GL_COLOR_LOGIC_OP */
/*      GL_DITHER */
/*      GL_STENCIL_TEST */
/*      GL_DEPTH_TEST */
/*      GL_CLIP_PLANE0 */
/*      GL_CLIP_PLANE1 */
/*      GL_CLIP_PLANE2 */
/*      GL_CLIP_PLANE3 */
/*      GL_CLIP_PLANE4 */
/*      GL_CLIP_PLANE5 */
/*      GL_LIGHT0 */
/*      GL_LIGHT1 */
/*      GL_LIGHT2 */
/*      GL_LIGHT3 */
/*      GL_LIGHT4 */
/*      GL_LIGHT5 */
/*      GL_LIGHT6 */
/*      GL_LIGHT7 */
/*      GL_TEXTURE_GEN_S */
/*      GL_TEXTURE_GEN_T */
/*      GL_TEXTURE_GEN_R */
/*      GL_TEXTURE_GEN_Q */
/*      GL_MAP1_VERTEX_3 */
/*      GL_MAP1_VERTEX_4 */
/*      GL_MAP1_COLOR_4 */
/*      GL_MAP1_INDEX */
/*      GL_MAP1_NORMAL */
/*      GL_MAP1_TEXTURE_COORD_1 */
/*      GL_MAP1_TEXTURE_COORD_2 */
/*      GL_MAP1_TEXTURE_COORD_3 */
/*      GL_MAP1_TEXTURE_COORD_4 */
/*      GL_MAP2_VERTEX_3 */
/*      GL_MAP2_VERTEX_4 */
/*      GL_MAP2_COLOR_4 */
/*      GL_MAP2_INDEX */
/*      GL_MAP2_NORMAL */
/*      GL_MAP2_TEXTURE_COORD_1 */
/*      GL_MAP2_TEXTURE_COORD_2 */
/*      GL_MAP2_TEXTURE_COORD_3 */
/*      GL_MAP2_TEXTURE_COORD_4 */
/*      GL_POINT_SMOOTH */
/*      GL_LINE_SMOOTH */
/*      GL_POLYGON_SMOOTH */
/*      GL_SCISSOR_TEST */
/*      GL_COLOR_MATERIAL */
/*      GL_NORMALIZE */
/*      GL_AUTO_NORMAL */
/*      GL_VERTEX_ARRAY */
/*      GL_NORMAL_ARRAY */
/*      GL_COLOR_ARRAY */
/*      GL_INDEX_ARRAY */
/*      GL_TEXTURE_COORD_ARRAY */
/*      GL_EDGE_FLAG_ARRAY */
/*      GL_POLYGON_OFFSET_POINT */
/*      GL_POLYGON_OFFSET_LINE */
/*      GL_POLYGON_OFFSET_FILL */

/* ErrorCode */
#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_STACK_OVERFLOW                 0x0503
#define GL_STACK_UNDERFLOW                0x0504
#define GL_OUT_OF_MEMORY                  0x0505

/* FeedBackMode */
#define GL_2D                             0x0600
#define GL_3D                             0x0601
#define GL_3D_COLOR                       0x0602
#define GL_3D_COLOR_TEXTURE               0x0603
#define GL_4D_COLOR_TEXTURE               0x0604

/* FeedBackToken */
#define GL_PASS_THROUGH_TOKEN             0x0700
#define GL_POINT_TOKEN                    0x0701
#define GL_LINE_TOKEN                     0x0702
#define GL_POLYGON_TOKEN                  0x0703
#define GL_BITMAP_TOKEN                   0x0704
#define GL_DRAW_PIXEL_TOKEN               0x0705
#define GL_COPY_PIXEL_TOKEN               0x0706
#define GL_LINE_RESET_TOKEN               0x0707

/* FogMode */
/*      GL_LINEAR */
#define GL_EXP                            0x0800
#define GL_EXP2                           0x0801

/* FogParameter */
/*      GL_FOG_COLOR */
/*      GL_FOG_DENSITY */
/*      GL_FOG_END */
/*      GL_FOG_INDEX */
/*      GL_FOG_MODE */
/*      GL_FOG_START */

/* FrontFaceDirection */
#define GL_CW                             0x0900
#define GL_CCW                            0x0901

/* GetMapTarget */
#define GL_COEFF                          0x0A00
#define GL_ORDER                          0x0A01
#define GL_DOMAIN                         0x0A02

/* GetPixelMap */
/*      GL_PIXEL_MAP_I_TO_I */
/*      GL_PIXEL_MAP_S_TO_S */
/*      GL_PIXEL_MAP_I_TO_R */
/*      GL_PIXEL_MAP_I_TO_G */
/*      GL_PIXEL_MAP_I_TO_B */
/*      GL_PIXEL_MAP_I_TO_A */
/*      GL_PIXEL_MAP_R_TO_R */
/*      GL_PIXEL_MAP_G_TO_G */
/*      GL_PIXEL_MAP_B_TO_B */
/*      GL_PIXEL_MAP_A_TO_A */

/* GetPointerTarget */
/*      GL_VERTEX_ARRAY_POINTER */
/*      GL_NORMAL_ARRAY_POINTER */
/*      GL_COLOR_ARRAY_POINTER */
/*      GL_INDEX_ARRAY_POINTER */
/*      GL_TEXTURE_COORD_ARRAY_POINTER */
/*      GL_EDGE_FLAG_ARRAY_POINTER */

/* GetTarget */
#define GL_CURRENT_COLOR                  0x0B00
#define GL_CURRENT_INDEX                  0x0B01
#define GL_CURRENT_NORMAL                 0x0B02
#define GL_CURRENT_TEXTURE_COORDS         0x0B03
#define GL_CURRENT_RASTER_COLOR           0x0B04
#define GL_CURRENT_RASTER_INDEX           0x0B05
#define GL_CURRENT_RASTER_TEXTURE_COORDS  0x0B06
#define GL_CURRENT_RASTER_POSITION        0x0B07
#define GL_CURRENT_RASTER_POSITION_VALID  0x0B08
#define GL_CURRENT_RASTER_DISTANCE        0x0B09
#define GL_POINT_SMOOTH                   0x0B10
#define GL_POINT_SIZE                     0x0B11
#define GL_POINT_SIZE_RANGE               0x0B12
#define GL_POINT_SIZE_GRANULARITY         0x0B13
#define GL_LINE_SMOOTH                    0x0B20
#define GL_LINE_WIDTH                     0x0B21
#define GL_LINE_WIDTH_RANGE               0x0B22
#define GL_LINE_WIDTH_GRANULARITY         0x0B23
#define GL_LINE_STIPPLE                   0x0B24
#define GL_LINE_STIPPLE_PATTERN           0x0B25
#define GL_LINE_STIPPLE_REPEAT            0x0B26
#define GL_LIST_MODE                      0x0B30
#define GL_MAX_LIST_NESTING               0x0B31
#define GL_LIST_BASE                      0x0B32
#define GL_LIST_INDEX                     0x0B33
#define GL_POLYGON_MODE                   0x0B40
#define GL_POLYGON_SMOOTH                 0x0B41
#define GL_POLYGON_STIPPLE                0x0B42
#define GL_EDGE_FLAG                      0x0B43
#define GL_CULL_FACE                      0x0B44
#define GL_CULL_FACE_MODE                 0x0B45
#define GL_FRONT_FACE                     0x0B46
#define GL_LIGHTING                       0x0B50
#define GL_LIGHT_MODEL_LOCAL_VIEWER       0x0B51
#define GL_LIGHT_MODEL_TWO_SIDE           0x0B52
#define GL_LIGHT_MODEL_AMBIENT            0x0B53
#define GL_SHADE_MODEL                    0x0B54
#define GL_COLOR_MATERIAL_FACE            0x0B55
#define GL_COLOR_MATERIAL_PARAMETER       0x0B56
#define GL_COLOR_MATERIAL                 0x0B57
#define GL_FOG                            0x0B60
#define GL_FOG_INDEX                      0x0B61
#define GL_FOG_DENSITY                    0x0B62
#define GL_FOG_START                      0x0B63
#define GL_FOG_END                        0x0B64
#define GL_FOG_MODE                       0x0B65
#define GL_FOG_COLOR                      0x0B66
#define GL_DEPTH_RANGE                    0x0B70
#define GL_DEPTH_TEST                     0x0B71
#define GL_DEPTH_WRITEMASK                0x0B72
#define GL_DEPTH_CLEAR_VALUE              0x0B73
#define GL_DEPTH_FUNC                     0x0B74
#define GL_ACCUM_CLEAR_VALUE              0x0B80
#define GL_STENCIL_TEST                   0x0B90
#define GL_STENCIL_CLEAR_VALUE            0x0B91
#define GL_STENCIL_FUNC                   0x0B92
#define GL_STENCIL_VALUE_MASK             0x0B93
#define GL_STENCIL_FAIL                   0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL        0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS        0x0B96
#define GL_STENCIL_REF                    0x0B97
#define GL_STENCIL_WRITEMASK              0x0B98
#define GL_MATRIX_MODE                    0x0BA0
#define GL_NORMALIZE                      0x0BA1
#define GL_VIEWPORT                       0x0BA2
#define GL_MODELVIEW_STACK_DEPTH          0x0BA3
#define GL_PROJECTION_STACK_DEPTH         0x0BA4
#define GL_TEXTURE_STACK_DEPTH            0x0BA5
#define GL_MODELVIEW_MATRIX               0x0BA6
#define GL_PROJECTION_MATRIX              0x0BA7
#define GL_TEXTURE_MATRIX                 0x0BA8
#define GL_ATTRIB_STACK_DEPTH             0x0BB0
#define GL_CLIENT_ATTRIB_STACK_DEPTH      0x0BB1
#define GL_ALPHA_TEST                     0x0BC0
#define GL_ALPHA_TEST_FUNC                0x0BC1
#define GL_ALPHA_TEST_REF                 0x0BC2
#define GL_DITHER                         0x0BD0
#define GL_BLEND_DST                      0x0BE0
#define GL_BLEND_SRC                      0x0BE1
#define GL_BLEND                          0x0BE2
#define GL_LOGIC_OP_MODE                  0x0BF0
#define GL_INDEX_LOGIC_OP                 0x0BF1
#define GL_COLOR_LOGIC_OP                 0x0BF2
#define GL_AUX_BUFFERS                    0x0C00
#define GL_DRAW_BUFFER                    0x0C01
#define GL_READ_BUFFER                    0x0C02
#define GL_SCISSOR_BOX                    0x0C10
#define GL_SCISSOR_TEST                   0x0C11
#define GL_INDEX_CLEAR_VALUE              0x0C20
#define GL_INDEX_WRITEMASK                0x0C21
#define GL_COLOR_CLEAR_VALUE              0x0C22
#define GL_COLOR_WRITEMASK                0x0C23
#define GL_INDEX_MODE                     0x0C30
#define GL_RGBA_MODE                      0x0C31
#define GL_DOUBLEBUFFER                   0x0C32
#define GL_STEREO                         0x0C33
#define GL_RENDER_MODE                    0x0C40
#define GL_PERSPECTIVE_CORRECTION_HINT    0x0C50
#define GL_POINT_SMOOTH_HINT              0x0C51
#define GL_LINE_SMOOTH_HINT               0x0C52
#define GL_POLYGON_SMOOTH_HINT            0x0C53
#define GL_FOG_HINT                       0x0C54
#define GL_TEXTURE_GEN_S                  0x0C60
#define GL_TEXTURE_GEN_T                  0x0C61
#define GL_TEXTURE_GEN_R                  0x0C62
#define GL_TEXTURE_GEN_Q                  0x0C63
#define GL_PIXEL_MAP_I_TO_I               0x0C70
#define GL_PIXEL_MAP_S_TO_S               0x0C71
#define GL_PIXEL_MAP_I_TO_R               0x0C72
#define GL_PIXEL_MAP_I_TO_G               0x0C73
#define GL_PIXEL_MAP_I_TO_B               0x0C74
#define GL_PIXEL_MAP_I_TO_A               0x0C75
#define GL_PIXEL_MAP_R_TO_R               0x0C76
#define GL_PIXEL_MAP_G_TO_G               0x0C77
#define GL_PIXEL_MAP_B_TO_B               0x0C78
#define GL_PIXEL_MAP_A_TO_A               0x0C79
#define GL_PIXEL_MAP_I_TO_I_SIZE          0x0CB0
#define GL_PIXEL_MAP_S_TO_S_SIZE          0x0CB1
#define GL_PIXEL_MAP_I_TO_R_SIZE          0x0CB2
#define GL_PIXEL_MAP_I_TO_G_SIZE          0x0CB3
#define GL_PIXEL_MAP_I_TO_B_SIZE          0x0CB4
#define GL_PIXEL_MAP_I_TO_A_SIZE          0x0CB5
#define GL_PIXEL_MAP_R_TO_R_SIZE          0x0CB6
#define GL_PIXEL_MAP_G_TO_G_SIZE          0x0CB7
#define GL_PIXEL_MAP_B_TO_B_SIZE          0x0CB8
#define GL_PIXEL_MAP_A_TO_A_SIZE          0x0CB9
#define GL_UNPACK_SWAP_BYTES              0x0CF0
#define GL_UNPACK_LSB_FIRST               0x0CF1
#define GL_UNPACK_ROW_LENGTH              0x0CF2
#define GL_UNPACK_SKIP_ROWS               0x0CF3
#define GL_UNPACK_SKIP_PIXELS             0x0CF4
#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_PACK_SWAP_BYTES                0x0D00
#define GL_PACK_LSB_FIRST                 0x0D01
#define GL_PACK_ROW_LENGTH                0x0D02
#define GL_PACK_SKIP_ROWS                 0x0D03
#define GL_PACK_SKIP_PIXELS               0x0D04
#define GL_PACK_ALIGNMENT                 0x0D05
#define GL_MAP_COLOR                      0x0D10
#define GL_MAP_STENCIL                    0x0D11
#define GL_INDEX_SHIFT                    0x0D12
#define GL_INDEX_OFFSET                   0x0D13
#define GL_RED_SCALE                      0x0D14
#define GL_RED_BIAS                       0x0D15
#define GL_ZOOM_X                         0x0D16
#define GL_ZOOM_Y                         0x0D17
#define GL_GREEN_SCALE                    0x0D18
#define GL_GREEN_BIAS                     0x0D19
#define GL_BLUE_SCALE                     0x0D1A
#define GL_BLUE_BIAS                      0x0D1B
#define GL_ALPHA_SCALE                    0x0D1C
#define GL_ALPHA_BIAS                     0x0D1D
#define GL_DEPTH_SCALE                    0x0D1E
#define GL_DEPTH_BIAS                     0x0D1F
#define GL_MAX_EVAL_ORDER                 0x0D30
#define GL_MAX_LIGHTS                     0x0D31
#define GL_MAX_CLIP_PLANES                0x0D32
#define GL_MAX_TEXTURE_SIZE               0x0D33
#define GL_MAX_PIXEL_MAP_TABLE            0x0D34
#define GL_MAX_ATTRIB_STACK_DEPTH         0x0D35
#define GL_MAX_MODELVIEW_STACK_DEPTH      0x0D36
#define GL_MAX_NAME_STACK_DEPTH           0x0D37
#define GL_MAX_PROJECTION_STACK_DEPTH     0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH        0x0D39
#define GL_MAX_VIEWPORT_DIMS              0x0D3A
#define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH  0x0D3B
#define GL_SUBPIXEL_BITS                  0x0D50
#define GL_INDEX_BITS                     0x0D51
#define GL_RED_BITS                       0x0D52
#define GL_GREEN_BITS                     0x0D53
#define GL_BLUE_BITS                      0x0D54
#define GL_ALPHA_BITS                     0x0D55
#define GL_DEPTH_BITS                     0x0D56
#define GL_STENCIL_BITS                   0x0D57
#define GL_ACCUM_RED_BITS                 0x0D58
#define GL_ACCUM_GREEN_BITS               0x0D59
#define GL_ACCUM_BLUE_BITS                0x0D5A
#define GL_ACCUM_ALPHA_BITS               0x0D5B
#define GL_NAME_STACK_DEPTH               0x0D70
#define GL_AUTO_NORMAL                    0x0D80
#define GL_MAP1_COLOR_4                   0x0D90
#define GL_MAP1_INDEX                     0x0D91
#define GL_MAP1_NORMAL                    0x0D92
#define GL_MAP1_TEXTURE_COORD_1           0x0D93
#define GL_MAP1_TEXTURE_COORD_2           0x0D94
#define GL_MAP1_TEXTURE_COORD_3           0x0D95
#define GL_MAP1_TEXTURE_COORD_4           0x0D96
#define GL_MAP1_VERTEX_3                  0x0D97
#define GL_MAP1_VERTEX_4                  0x0D98
#define GL_MAP2_COLOR_4                   0x0DB0
#define GL_MAP2_INDEX                     0x0DB1
#define GL_MAP2_NORMAL                    0x0DB2
#define GL_MAP2_TEXTURE_COORD_1           0x0DB3
#define GL_MAP2_TEXTURE_COORD_2           0x0DB4
#define GL_MAP2_TEXTURE_COORD_3           0x0DB5
#define GL_MAP2_TEXTURE_COORD_4           0x0DB6
#define GL_MAP2_VERTEX_3                  0x0DB7
#define GL_MAP2_VERTEX_4                  0x0DB8
#define GL_MAP1_GRID_DOMAIN               0x0DD0
#define GL_MAP1_GRID_SEGMENTS             0x0DD1
#define GL_MAP2_GRID_DOMAIN               0x0DD2
#define GL_MAP2_GRID_SEGMENTS             0x0DD3
#define GL_TEXTURE_1D                     0x0DE0
#define GL_TEXTURE_2D                     0x0DE1
#define GL_FEEDBACK_BUFFER_POINTER        0x0DF0
#define GL_FEEDBACK_BUFFER_SIZE           0x0DF1
#define GL_FEEDBACK_BUFFER_TYPE           0x0DF2
#define GL_SELECTION_BUFFER_POINTER       0x0DF3
#define GL_SELECTION_BUFFER_SIZE          0x0DF4
#define GL_THREADED						  0x0DF5
/*      GL_TEXTURE_BINDING_1D */
/*      GL_TEXTURE_BINDING_2D */
/*      GL_VERTEX_ARRAY */
/*      GL_NORMAL_ARRAY */
/*      GL_COLOR_ARRAY */
/*      GL_INDEX_ARRAY */
/*      GL_TEXTURE_COORD_ARRAY */
/*      GL_EDGE_FLAG_ARRAY */
/*      GL_VERTEX_ARRAY_SIZE */
/*      GL_VERTEX_ARRAY_TYPE */
/*      GL_VERTEX_ARRAY_STRIDE */
/*      GL_NORMAL_ARRAY_TYPE */
/*      GL_NORMAL_ARRAY_STRIDE */
/*      GL_COLOR_ARRAY_SIZE */
/*      GL_COLOR_ARRAY_TYPE */
/*      GL_COLOR_ARRAY_STRIDE */
/*      GL_INDEX_ARRAY_TYPE */
/*      GL_INDEX_ARRAY_STRIDE */
/*      GL_TEXTURE_COORD_ARRAY_SIZE */
/*      GL_TEXTURE_COORD_ARRAY_TYPE */
/*      GL_TEXTURE_COORD_ARRAY_STRIDE */
/*      GL_EDGE_FLAG_ARRAY_STRIDE */
/*      GL_POLYGON_OFFSET_FACTOR */
/*      GL_POLYGON_OFFSET_UNITS */

/* GetTextureParameter */
/*      GL_TEXTURE_MAG_FILTER */
/*      GL_TEXTURE_MIN_FILTER */
/*      GL_TEXTURE_WRAP_S */
/*      GL_TEXTURE_WRAP_T */
#define GL_TEXTURE_WIDTH                  0x1000
#define GL_TEXTURE_HEIGHT                 0x1001
#define GL_TEXTURE_INTERNAL_FORMAT        0x1003
#define GL_TEXTURE_BORDER_COLOR           0x1004
#define GL_TEXTURE_BORDER                 0x1005
/*      GL_TEXTURE_RED_SIZE */
/*      GL_TEXTURE_GREEN_SIZE */
/*      GL_TEXTURE_BLUE_SIZE */
/*      GL_TEXTURE_ALPHA_SIZE */
/*      GL_TEXTURE_LUMINANCE_SIZE */
/*      GL_TEXTURE_INTENSITY_SIZE */
/*      GL_TEXTURE_PRIORITY */
/*      GL_TEXTURE_RESIDENT */

/* HintMode */
#define GL_DONT_CARE                      0x1100
#define GL_FASTEST                        0x1101
#define GL_NICEST                         0x1102

/* HintTarget */
/*      GL_PERSPECTIVE_CORRECTION_HINT */
/*      GL_POINT_SMOOTH_HINT */
/*      GL_LINE_SMOOTH_HINT */
/*      GL_POLYGON_SMOOTH_HINT */
/*      GL_FOG_HINT */

/* IndexPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* LightModelParameter */
/*      GL_LIGHT_MODEL_AMBIENT */
/*      GL_LIGHT_MODEL_LOCAL_VIEWER */
/*      GL_LIGHT_MODEL_TWO_SIDE */

/* LightName */
#define GL_LIGHT0                         0x4000
#define GL_LIGHT1                         0x4001
#define GL_LIGHT2                         0x4002
#define GL_LIGHT3                         0x4003
#define GL_LIGHT4                         0x4004
#define GL_LIGHT5                         0x4005
#define GL_LIGHT6                         0x4006
#define GL_LIGHT7                         0x4007

/* LightParameter */
#define GL_AMBIENT                        0x1200
#define GL_DIFFUSE                        0x1201
#define GL_SPECULAR                       0x1202
#define GL_POSITION                       0x1203
#define GL_SPOT_DIRECTION                 0x1204
#define GL_SPOT_EXPONENT                  0x1205
#define GL_SPOT_CUTOFF                    0x1206
#define GL_CONSTANT_ATTENUATION           0x1207
#define GL_LINEAR_ATTENUATION             0x1208
#define GL_QUADRATIC_ATTENUATION          0x1209

/* InterleavedArrays */
/*      GL_V2F */
/*      GL_V3F */
/*      GL_C4UB_V2F */
/*      GL_C4UB_V3F */
/*      GL_C3F_V3F */
/*      GL_N3F_V3F */
/*      GL_C4F_N3F_V3F */
/*      GL_T2F_V3F */
/*      GL_T4F_V4F */
/*      GL_T2F_C4UB_V3F */
/*      GL_T2F_C3F_V3F */
/*      GL_T2F_N3F_V3F */
/*      GL_T2F_C4F_N3F_V3F */
/*      GL_T4F_C4F_N3F_V4F */

/* ListMode */
#define GL_COMPILE                        0x1300
#define GL_COMPILE_AND_EXECUTE            0x1301

/* ListNameType */
/*      GL_BYTE */
/*      GL_UNSIGNED_BYTE */
/*      GL_SHORT */
/*      GL_UNSIGNED_SHORT */
/*      GL_INT */
/*      GL_UNSIGNED_INT */
/*      GL_FLOAT */
/*      GL_2_BYTES */
/*      GL_3_BYTES */
/*      GL_4_BYTES */

/* LogicOp */
#define GL_CLEAR                          0x1500
#define GL_AND                            0x1501
#define GL_AND_REVERSE                    0x1502
#define GL_COPY                           0x1503
#define GL_AND_INVERTED                   0x1504
#define GL_NOOP                           0x1505
#define GL_XOR                            0x1506
#define GL_OR                             0x1507
#define GL_NOR                            0x1508
#define GL_EQUIV                          0x1509
#define GL_INVERT                         0x150A
#define GL_OR_REVERSE                     0x150B
#define GL_COPY_INVERTED                  0x150C
#define GL_OR_INVERTED                    0x150D
#define GL_NAND                           0x150E
#define GL_SET                            0x150F

/* MapTarget */
/*      GL_MAP1_COLOR_4 */
/*      GL_MAP1_INDEX */
/*      GL_MAP1_NORMAL */
/*      GL_MAP1_TEXTURE_COORD_1 */
/*      GL_MAP1_TEXTURE_COORD_2 */
/*      GL_MAP1_TEXTURE_COORD_3 */
/*      GL_MAP1_TEXTURE_COORD_4 */
/*      GL_MAP1_VERTEX_3 */
/*      GL_MAP1_VERTEX_4 */
/*      GL_MAP2_COLOR_4 */
/*      GL_MAP2_INDEX */
/*      GL_MAP2_NORMAL */
/*      GL_MAP2_TEXTURE_COORD_1 */
/*      GL_MAP2_TEXTURE_COORD_2 */
/*      GL_MAP2_TEXTURE_COORD_3 */
/*      GL_MAP2_TEXTURE_COORD_4 */
/*      GL_MAP2_VERTEX_3 */
/*      GL_MAP2_VERTEX_4 */

/* MaterialFace */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_FRONT_AND_BACK */

/* MaterialParameter */
#define GL_EMISSION                       0x1600
#define GL_SHININESS                      0x1601
#define GL_AMBIENT_AND_DIFFUSE            0x1602
#define GL_COLOR_INDEXES                  0x1603
/*      GL_AMBIENT */
/*      GL_DIFFUSE */
/*      GL_SPECULAR */

/* MatrixMode */
#define GL_MODELVIEW                      0x1700
#define GL_PROJECTION                     0x1701
#define GL_TEXTURE                        0x1702

/* MeshMode1 */
/*      GL_POINT */
/*      GL_LINE */

/* MeshMode2 */
/*      GL_POINT */
/*      GL_LINE */
/*      GL_FILL */

/* NormalPointerType */
/*      GL_BYTE */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* PixelCopyType */
#define GL_COLOR                          0x1800
#define GL_DEPTH                          0x1801
#define GL_STENCIL                        0x1802

/* PixelFormat */
#define GL_COLOR_INDEX                    0x1900
#define GL_STENCIL_INDEX                  0x1901
#define GL_DEPTH_COMPONENT                0x1902
#define GL_RED                            0x1903
#define GL_GREEN                          0x1904
#define GL_BLUE                           0x1905
#define GL_ALPHA                          0x1906
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_LUMINANCE                      0x1909
#define GL_LUMINANCE_ALPHA                0x190A
/* 1.2 defs */
#define GL_BGR                            0x80E0
#define GL_BGRA                           0x80E1

/*      GL_ABGR_EXT */

/* PixelInternalFormat */
/*      GL_ALPHA4 */
/*      GL_ALPHA8 */
/*      GL_ALPHA12 */
/*      GL_ALPHA16 */
/*      GL_LUMINANCE4 */
/*      GL_LUMINANCE8 */
/*      GL_LUMINANCE12 */
/*      GL_LUMINANCE16 */
/*      GL_LUMINANCE4_ALPHA4 */
/*      GL_LUMINANCE6_ALPHA2 */
/*      GL_LUMINANCE8_ALPHA8 */
/*      GL_LUMINANCE12_ALPHA4 */
/*      GL_LUMINANCE12_ALPHA12 */
/*      GL_LUMINANCE16_ALPHA16 */
/*      GL_INTENSITY */
/*      GL_INTENSITY4 */
/*      GL_INTENSITY8 */
/*      GL_INTENSITY12 */
/*      GL_INTENSITY16 */
/*      GL_R3_G3_B2 */
/*      GL_RGB4 */
/*      GL_RGB5 */
/*      GL_RGB8 */
/*      GL_RGB10 */
/*      GL_RGB12 */
/*      GL_RGB16 */
/*      GL_RGBA2 */
/*      GL_RGBA4 */
/*      GL_RGB5_A1 */
/*      GL_RGBA8 */
/*      GL_RGB10_A2 */
/*      GL_RGBA12 */
/*      GL_RGBA16 */

/* PixelMap */
/*      GL_PIXEL_MAP_I_TO_I */
/*      GL_PIXEL_MAP_S_TO_S */
/*      GL_PIXEL_MAP_I_TO_R */
/*      GL_PIXEL_MAP_I_TO_G */
/*      GL_PIXEL_MAP_I_TO_B */
/*      GL_PIXEL_MAP_I_TO_A */
/*      GL_PIXEL_MAP_R_TO_R */
/*      GL_PIXEL_MAP_G_TO_G */
/*      GL_PIXEL_MAP_B_TO_B */
/*      GL_PIXEL_MAP_A_TO_A */

/* PixelStore */
/*      GL_UNPACK_SWAP_BYTES */
/*      GL_UNPACK_LSB_FIRST */
/*      GL_UNPACK_ROW_LENGTH */
/*      GL_UNPACK_SKIP_ROWS */
/*      GL_UNPACK_SKIP_PIXELS */
/*      GL_UNPACK_ALIGNMENT */
/*      GL_PACK_SWAP_BYTES */
/*      GL_PACK_LSB_FIRST */
/*      GL_PACK_ROW_LENGTH */
/*      GL_PACK_SKIP_ROWS */
/*      GL_PACK_SKIP_PIXELS */
/*      GL_PACK_ALIGNMENT */

/* PixelTransfer */
/*      GL_MAP_COLOR */
/*      GL_MAP_STENCIL */
/*      GL_INDEX_SHIFT */
/*      GL_INDEX_OFFSET */
/*      GL_RED_SCALE */
/*      GL_RED_BIAS */
/*      GL_GREEN_SCALE */
/*      GL_GREEN_BIAS */
/*      GL_BLUE_SCALE */
/*      GL_BLUE_BIAS */
/*      GL_ALPHA_SCALE */
/*      GL_ALPHA_BIAS */
/*      GL_DEPTH_SCALE */
/*      GL_DEPTH_BIAS */

/* PixelType */
#define GL_BITMAP                         0x1A00
/*      GL_BYTE */
/*      GL_UNSIGNED_BYTE */
/*      GL_SHORT */
/*      GL_UNSIGNED_SHORT */
/*      GL_INT */
/*      GL_UNSIGNED_INT */
/*      GL_FLOAT */

/* PolygonMode */
#define GL_POINT                          0x1B00
#define GL_LINE                           0x1B01
#define GL_FILL                           0x1B02

/* ReadBufferMode */
/*      GL_FRONT_LEFT */
/*      GL_FRONT_RIGHT */
/*      GL_BACK_LEFT */
/*      GL_BACK_RIGHT */
/*      GL_FRONT */
/*      GL_BACK */
/*      GL_LEFT */
/*      GL_RIGHT */
/*      GL_AUX0 */
/*      GL_AUX1 */
/*      GL_AUX2 */
/*      GL_AUX3 */

/* RenderingMode */
#define GL_RENDER                         0x1C00
#define GL_FEEDBACK                       0x1C01
#define GL_SELECT                         0x1C02

/* ShadingModel */
#define GL_FLAT                           0x1D00
#define GL_SMOOTH                         0x1D01

/* StencilFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

/* StencilOp */
/*      GL_ZERO */
#define GL_KEEP                           0x1E00
#define GL_REPLACE                        0x1E01
#define GL_INCR                           0x1E02
#define GL_DECR                           0x1E03
/*      GL_INVERT */

/* StringName */
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03

/* TextureCoordName */
#define GL_S                              0x2000
#define GL_T                              0x2001
#define GL_R                              0x2002
#define GL_Q                              0x2003

/* TexCoordPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* TextureEnvMode */
#define GL_MODULATE                       0x2100
#define GL_DECAL                          0x2101
/*      GL_BLEND */
/*      GL_REPLACE */

/* TextureEnvParameter */
#define GL_TEXTURE_ENV_MODE               0x2200
#define GL_TEXTURE_ENV_COLOR              0x2201

/* TextureEnvTarget */
#define GL_TEXTURE_ENV                    0x2300

/* TextureGenMode */
#define GL_EYE_LINEAR                     0x2400
#define GL_OBJECT_LINEAR                  0x2401
#define GL_SPHERE_MAP                     0x2402

/* TextureGenParameter */
#define GL_TEXTURE_GEN_MODE               0x2500
#define GL_OBJECT_PLANE                   0x2501
#define GL_EYE_PLANE                      0x2502

/* TextureMagFilter */
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601

/* TextureMinFilter */
/*      GL_NEAREST */
/*      GL_LINEAR */
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703

/* TextureParameterName */
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
/*      GL_TEXTURE_BORDER_COLOR */
/*      GL_TEXTURE_PRIORITY */

/* TextureTarget */
/*      GL_TEXTURE_1D */
/*      GL_TEXTURE_2D */
/*      GL_PROXY_TEXTURE_1D */
/*      GL_PROXY_TEXTURE_2D */

/* TextureWrapMode */
#define GL_CLAMP                          0x2900
#define GL_REPEAT                         0x2901

/* VertexPointerType */
/*      GL_SHORT */
/*      GL_INT */
/*      GL_FLOAT */
/*      GL_DOUBLE */

/* ClientAttribMask */
#define GL_CLIENT_PIXEL_STORE_BIT         0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT        0x00000002
#define GL_CLIENT_ALL_ATTRIB_BITS         0xffffffff

/* polygon_offset */
#define GL_POLYGON_OFFSET_FACTOR          0x8038
#define GL_POLYGON_OFFSET_UNITS           0x2A00
#define GL_POLYGON_OFFSET_POINT           0x2A01
#define GL_POLYGON_OFFSET_LINE            0x2A02
#define GL_POLYGON_OFFSET_FILL            0x8037

/* texture */
#define GL_ALPHA4                         0x803B
#define GL_ALPHA8                         0x803C
#define GL_ALPHA12                        0x803D
#define GL_ALPHA16                        0x803E
#define GL_LUMINANCE4                     0x803F
#define GL_LUMINANCE8                     0x8040
#define GL_LUMINANCE12                    0x8041
#define GL_LUMINANCE16                    0x8042
#define GL_LUMINANCE4_ALPHA4              0x8043
#define GL_LUMINANCE6_ALPHA2              0x8044
#define GL_LUMINANCE8_ALPHA8              0x8045
#define GL_LUMINANCE12_ALPHA4             0x8046
#define GL_LUMINANCE12_ALPHA12            0x8047
#define GL_LUMINANCE16_ALPHA16            0x8048
#define GL_INTENSITY                      0x8049
#define GL_INTENSITY4                     0x804A
#define GL_INTENSITY8                     0x804B
#define GL_INTENSITY12                    0x804C
#define GL_INTENSITY16                    0x804D
#define GL_R3_G3_B2                       0x2A10
#define GL_RGB4                           0x804F
#define GL_RGB5                           0x8050
#define GL_RGB8                           0x8051
#define GL_RGB10                          0x8052
#define GL_RGB12                          0x8053
#define GL_RGB16                          0x8054
#define GL_RGBA2                          0x8055
#define GL_RGBA4                          0x8056
#define GL_RGB5_A1                        0x8057
#define GL_RGBA8                          0x8058
#define GL_RGB10_A2                       0x8059
#define GL_RGBA12                         0x805A
#define GL_RGBA16                         0x805B
#define GL_TEXTURE_RED_SIZE               0x805C
#define GL_TEXTURE_GREEN_SIZE             0x805D
#define GL_TEXTURE_BLUE_SIZE              0x805E
#define GL_TEXTURE_ALPHA_SIZE             0x805F
#define GL_TEXTURE_LUMINANCE_SIZE         0x8060
#define GL_TEXTURE_INTENSITY_SIZE         0x8061
#define GL_PROXY_TEXTURE_1D               0x8063
#define GL_PROXY_TEXTURE_2D               0x8064

/* texture_object */
#define GL_TEXTURE_PRIORITY               0x8066
#define GL_TEXTURE_RESIDENT               0x8067
#define GL_TEXTURE_BINDING_1D             0x8068
#define GL_TEXTURE_BINDING_2D             0x8069

/* vertex_array */
#define GL_VERTEX_ARRAY                   0x8074
#define GL_NORMAL_ARRAY                   0x8075
#define GL_COLOR_ARRAY                    0x8076
#define GL_INDEX_ARRAY                    0x8077
#define GL_TEXTURE_COORD_ARRAY            0x8078
#define GL_EDGE_FLAG_ARRAY                0x8079
#define GL_VERTEX_ARRAY_SIZE              0x807A
#define GL_VERTEX_ARRAY_TYPE              0x807B
#define GL_VERTEX_ARRAY_STRIDE            0x807C
#define GL_NORMAL_ARRAY_TYPE              0x807E
#define GL_NORMAL_ARRAY_STRIDE            0x807F
#define GL_COLOR_ARRAY_SIZE               0x8081
#define GL_COLOR_ARRAY_TYPE               0x8082
#define GL_COLOR_ARRAY_STRIDE             0x8083
#define GL_INDEX_ARRAY_TYPE               0x8085
#define GL_INDEX_ARRAY_STRIDE             0x8086
#define GL_TEXTURE_COORD_ARRAY_SIZE       0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE       0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE     0x808A
#define GL_EDGE_FLAG_ARRAY_STRIDE         0x808C
#define GL_VERTEX_ARRAY_POINTER           0x808E
#define GL_NORMAL_ARRAY_POINTER           0x808F
#define GL_COLOR_ARRAY_POINTER            0x8090
#define GL_INDEX_ARRAY_POINTER            0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER    0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER        0x8093
#define GL_V2F                            0x2A20
#define GL_V3F                            0x2A21
#define GL_C4UB_V2F                       0x2A22
#define GL_C4UB_V3F                       0x2A23
#define GL_C3F_V3F                        0x2A24
#define GL_N3F_V3F                        0x2A25
#define GL_C4F_N3F_V3F                    0x2A26
#define GL_T2F_V3F                        0x2A27
#define GL_T4F_V4F                        0x2A28
#define GL_T2F_C4UB_V3F                   0x2A29
#define GL_T2F_C3F_V3F                    0x2A2A
#define GL_T2F_N3F_V3F                    0x2A2B
#define GL_T2F_C4F_N3F_V3F                0x2A2C
#define GL_T4F_C4F_N3F_V4F                0x2A2D

/* Extensions */
#define GL_EXT_abgr                       1
#define GL_EXT_blend_color                1
#define GL_EXT_blend_minmax               1
#define GL_EXT_blend_subtract             1

/* EXT_abgr */
#define GL_ABGR_EXT                       0x8000

/* EXT_blend_color */
#define GL_CONSTANT_COLOR_EXT             0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR_EXT   0x8002
#define GL_CONSTANT_ALPHA_EXT             0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA_EXT   0x8004
#define GL_BLEND_COLOR_EXT                0x8005

/* EXT_blend_minmax */
#define GL_FUNC_ADD_EXT                   0x8006
#define GL_MIN_EXT                        0x8007
#define GL_MAX_EXT                        0x8008
#define GL_BLEND_EQUATION_EXT             0x8009

/* EXT_blend_subtract */
#define GL_FUNC_SUBTRACT_EXT              0x800A
#define GL_FUNC_REVERSE_SUBTRACT_EXT      0x800B

/* ARB_multitexture */ 
#define GL_ACTIVE_TEXTURE_ARB               0x84E0 
#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1 
#define GL_MAX_TEXTURE_UNITS_ARB            0x84E2 
#define GL_TEXTURE0_ARB                     0x84C0 
#define GL_TEXTURE1_ARB                     0x84C1 
#define GL_TEXTURE2_ARB                     0x84C2 
#define GL_TEXTURE3_ARB                     0x84C3 
#define GL_TEXTURE4_ARB                     0x84C4 
#define GL_TEXTURE5_ARB                     0x84C5 
#define GL_TEXTURE6_ARB                     0x84C6 
#define GL_TEXTURE7_ARB                     0x84C7 
#define GL_TEXTURE8_ARB                     0x84C8 
#define GL_TEXTURE9_ARB                     0x84C9 
#define GL_TEXTURE10_ARB                    0x84CA 
#define GL_TEXTURE11_ARB                    0x84CB 
#define GL_TEXTURE12_ARB                    0x84CC 
#define GL_TEXTURE13_ARB                    0x84CD 
#define GL_TEXTURE14_ARB                    0x84CE 
#define GL_TEXTURE15_ARB                    0x84CF 
#define GL_TEXTURE16_ARB                    0x84D0 
#define GL_TEXTURE17_ARB                    0x84D1 
#define GL_TEXTURE18_ARB                    0x84D2 
#define GL_TEXTURE19_ARB                    0x84D3 
#define GL_TEXTURE20_ARB                    0x84D4 
#define GL_TEXTURE21_ARB                    0x84D5 
#define GL_TEXTURE22_ARB                    0x84D6 
#define GL_TEXTURE23_ARB                    0x84D7 
#define GL_TEXTURE24_ARB                    0x84D8 
#define GL_TEXTURE25_ARB                    0x84D9 
#define GL_TEXTURE26_ARB                    0x84DA 
#define GL_TEXTURE27_ARB                    0x84DB 
#define GL_TEXTURE28_ARB                    0x84DC 
#define GL_TEXTURE29_ARB                    0x84DD 
#define GL_TEXTURE30_ARB                    0x84DE 
#define GL_TEXTURE31_ARB                    0x84DF

/* For compatibility with OpenGL v1.0 */
#define GL_LOGIC_OP GL_INDEX_LOGIC_OP
#define GL_TEXTURE_COMPONENTS GL_TEXTURE_INTERNAL_FORMAT

/*************************************************************/

_IMPEXP_GL void glAccum (GLenum op, GLfloat value);
_IMPEXP_GL void glAlphaFunc (GLenum func, GLclampf ref);
_IMPEXP_GL GLboolean glAreTexturesResident (GLsizei n, const GLuint *textures, GLboolean *residences);
_IMPEXP_GL void glArrayElement (GLint i);
_IMPEXP_GL void glBindTexture (GLenum target, GLuint texture);
_IMPEXP_GL void glBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
_IMPEXP_GL void glBlendColorEXT (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
_IMPEXP_GL void glBlendEquationEXT (GLenum mode);
_IMPEXP_GL void glBlendFunc (GLenum sfactor, GLenum dfactor);
_IMPEXP_GL void glCallList (GLuint list);
_IMPEXP_GL void glCallLists (GLsizei n, GLenum type, const GLvoid *lists);
_IMPEXP_GL void glClear (GLbitfield mask);
_IMPEXP_GL void glClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
_IMPEXP_GL void glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
_IMPEXP_GL void glClearDepth (GLclampd depth);
_IMPEXP_GL void glClearIndex (GLfloat c);
_IMPEXP_GL void glClearStencil (GLint s);
_IMPEXP_GL void glClipPlane (GLenum plane, const GLdouble *equation);
_IMPEXP_GL void glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
_IMPEXP_GL void glColorMaterial (GLenum face, GLenum mode);
_IMPEXP_GL void glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
_IMPEXP_GL void glCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
_IMPEXP_GL void glCopyTexImage1D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
_IMPEXP_GL void glCopyTexImage2D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
_IMPEXP_GL void glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
_IMPEXP_GL void glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
_IMPEXP_GL void glCullFace (GLenum mode);
_IMPEXP_GL void glDeleteLists (GLuint list, GLsizei range);
_IMPEXP_GL void glDeleteTextures (GLsizei n, const GLuint *textures);
_IMPEXP_GL void glDepthFunc (GLenum func);
_IMPEXP_GL void glDepthMask (GLboolean flag);
_IMPEXP_GL void glDepthRange (GLclampd zNear, GLclampd zFar);
_IMPEXP_GL void glDisable (GLenum cap);
_IMPEXP_GL void glDisableClientState (GLenum array);
_IMPEXP_GL void glDrawArrays (GLenum mode, GLint first, GLsizei count);
_IMPEXP_GL void glDrawBuffer (GLenum mode);
_IMPEXP_GL void glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
_IMPEXP_GL void glDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
_IMPEXP_GL void glEdgeFlag (GLboolean flag);
_IMPEXP_GL void glEdgeFlagPointer (GLsizei stride, const GLvoid *pointer);
_IMPEXP_GL void glEdgeFlagv (const GLboolean *flag);
_IMPEXP_GL void glEnable (GLenum cap);
_IMPEXP_GL void glEnableClientState (GLenum array);
_IMPEXP_GL void glEndList (void);
_IMPEXP_GL void glEvalCoord1d (GLdouble u);
_IMPEXP_GL void glEvalCoord1dv (const GLdouble *u);
_IMPEXP_GL void glEvalCoord1f (GLfloat u);
_IMPEXP_GL void glEvalCoord1fv (const GLfloat *u);
_IMPEXP_GL void glEvalCoord2d (GLdouble u, GLdouble v);
_IMPEXP_GL void glEvalCoord2dv (const GLdouble *u);
_IMPEXP_GL void glEvalCoord2f (GLfloat u, GLfloat v);
_IMPEXP_GL void glEvalCoord2fv (const GLfloat *u);
_IMPEXP_GL void glEvalMesh1 (GLenum mode, GLint i1, GLint i2);
_IMPEXP_GL void glEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
_IMPEXP_GL void glEvalPoint1 (GLint i);
_IMPEXP_GL void glEvalPoint2 (GLint i, GLint j);
_IMPEXP_GL void glFeedbackBuffer (GLsizei size, GLenum type, GLfloat *buffer);
_IMPEXP_GL void glFinish (void);
_IMPEXP_GL void glFlush (void);
_IMPEXP_GL void glFogf (GLenum pname, GLfloat param);
_IMPEXP_GL void glFogfv (GLenum pname, const GLfloat *params);
_IMPEXP_GL void glFogi (GLenum pname, GLint param);
_IMPEXP_GL void glFogiv (GLenum pname, const GLint *params);
_IMPEXP_GL void glFrontFace (GLenum mode);
_IMPEXP_GL void glFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
_IMPEXP_GL GLuint glGenLists (GLsizei range);
_IMPEXP_GL void glGenTextures (GLsizei n, GLuint *textures);
_IMPEXP_GL void glGetBooleanv (GLenum pname, GLboolean *params);
_IMPEXP_GL void glGetClipPlane (GLenum plane, GLdouble *equation);
_IMPEXP_GL void glGetDoublev (GLenum pname, GLdouble *params);
_IMPEXP_GL GLenum glGetError (void);
_IMPEXP_GL void glGetFloatv (GLenum pname, GLfloat *params);
_IMPEXP_GL void glGetIntegerv (GLenum pname, GLint *params);
_IMPEXP_GL void glGetLightfv (GLenum light, GLenum pname, GLfloat *params);
_IMPEXP_GL void glGetLightiv (GLenum light, GLenum pname, GLint *params);
_IMPEXP_GL void glGetMapdv (GLenum target, GLenum query, GLdouble *v);
_IMPEXP_GL void glGetMapfv (GLenum target, GLenum query, GLfloat *v);
_IMPEXP_GL void glGetMapiv (GLenum target, GLenum query, GLint *v);
_IMPEXP_GL void glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params);
_IMPEXP_GL void glGetMaterialiv (GLenum face, GLenum pname, GLint *params);
_IMPEXP_GL void glGetPixelMapfv (GLenum map, GLfloat *values);
_IMPEXP_GL void glGetPixelMapuiv (GLenum map, GLuint *values);
_IMPEXP_GL void glGetPixelMapusv (GLenum map, GLushort *values);
_IMPEXP_GL void glGetPointerv (GLenum pname, GLvoid* *params);
_IMPEXP_GL void glGetPolygonStipple (GLubyte *mask);
_IMPEXP_GL const GLubyte * glGetString (GLenum name);
_IMPEXP_GL void glGetTexEnvfv (GLenum target, GLenum pname, GLfloat *params);
_IMPEXP_GL void glGetTexEnviv (GLenum target, GLenum pname, GLint *params);
_IMPEXP_GL void glGetTexGendv (GLenum coord, GLenum pname, GLdouble *params);
_IMPEXP_GL void glGetTexGenfv (GLenum coord, GLenum pname, GLfloat *params);
_IMPEXP_GL void glGetTexGeniv (GLenum coord, GLenum pname, GLint *params);
_IMPEXP_GL void glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
_IMPEXP_GL void glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
_IMPEXP_GL void glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
_IMPEXP_GL void glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
_IMPEXP_GL void glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
_IMPEXP_GL void glHint (GLenum target, GLenum mode);
_IMPEXP_GL void glIndexMask (GLuint mask);
_IMPEXP_GL void glIndexPointer (GLenum type, GLsizei stride, const GLvoid *pointer);
_IMPEXP_GL void glIndexd (GLdouble c);
_IMPEXP_GL void glIndexdv (const GLdouble *c);
_IMPEXP_GL void glIndexf (GLfloat c);
_IMPEXP_GL void glIndexfv (const GLfloat *c);
_IMPEXP_GL void glIndexi (GLint c);
_IMPEXP_GL void glIndexiv (const GLint *c);
_IMPEXP_GL void glIndexs (GLshort c);
_IMPEXP_GL void glIndexsv (const GLshort *c);
_IMPEXP_GL void glIndexub (GLubyte c);
_IMPEXP_GL void glIndexubv (const GLubyte *c);
_IMPEXP_GL void glInitNames (void);
_IMPEXP_GL void glInterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer);
_IMPEXP_GL GLboolean glIsEnabled (GLenum cap);
_IMPEXP_GL GLboolean glIsList (GLuint list);
_IMPEXP_GL GLboolean glIsTexture (GLuint texture);
_IMPEXP_GL void glLightModelf (GLenum pname, GLfloat param);
_IMPEXP_GL void glLightModelfv (GLenum pname, const GLfloat *params);
_IMPEXP_GL void glLightModeli (GLenum pname, GLint param);
_IMPEXP_GL void glLightModeliv (GLenum pname, const GLint *params);
_IMPEXP_GL void glLightf (GLenum light, GLenum pname, GLfloat param);
_IMPEXP_GL void glLightfv (GLenum light, GLenum pname, const GLfloat *params);
_IMPEXP_GL void glLighti (GLenum light, GLenum pname, GLint param);
_IMPEXP_GL void glLightiv (GLenum light, GLenum pname, const GLint *params);
_IMPEXP_GL void glLineStipple (GLint factor, GLushort pattern);
_IMPEXP_GL void glLineWidth (GLfloat width);
_IMPEXP_GL void glListBase (GLuint base);
_IMPEXP_GL void glLoadIdentity (void);
_IMPEXP_GL void glLoadMatrixd (const GLdouble *m);
_IMPEXP_GL void glLoadMatrixf (const GLfloat *m);
_IMPEXP_GL void glLoadName (GLuint name);
_IMPEXP_GL void glLogicOp (GLenum opcode);
_IMPEXP_GL void glMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
_IMPEXP_GL void glMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
_IMPEXP_GL void glMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
_IMPEXP_GL void glMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
_IMPEXP_GL void glMapGrid1d (GLint un, GLdouble u1, GLdouble u2);
_IMPEXP_GL void glMapGrid1f (GLint un, GLfloat u1, GLfloat u2);
_IMPEXP_GL void glMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
_IMPEXP_GL void glMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
_IMPEXP_GL void glMaterialf (GLenum face, GLenum pname, GLfloat param);
_IMPEXP_GL void glMaterialfv (GLenum face, GLenum pname, const GLfloat *params);
_IMPEXP_GL void glMateriali (GLenum face, GLenum pname, GLint param);
_IMPEXP_GL void glMaterialiv (GLenum face, GLenum pname, const GLint *params);
_IMPEXP_GL void glMatrixMode (GLenum mode);
_IMPEXP_GL void glMultMatrixd (const GLdouble *m);
_IMPEXP_GL void glMultMatrixf (const GLfloat *m);
_IMPEXP_GL void glNewList (GLuint list, GLenum mode);
_IMPEXP_GL void glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer);
_IMPEXP_GL void glOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
_IMPEXP_GL void glPassThrough (GLfloat token);
_IMPEXP_GL void glPixelMapfv (GLenum map, GLsizei mapsize, const GLfloat *values);
_IMPEXP_GL void glPixelMapuiv (GLenum map, GLsizei mapsize, const GLuint *values);
_IMPEXP_GL void glPixelMapusv (GLenum map, GLsizei mapsize, const GLushort *values);
_IMPEXP_GL void glPixelStoref (GLenum pname, GLfloat param);
_IMPEXP_GL void glPixelStorei (GLenum pname, GLint param);
_IMPEXP_GL void glPixelTransferf (GLenum pname, GLfloat param);
_IMPEXP_GL void glPixelTransferi (GLenum pname, GLint param);
_IMPEXP_GL void glPixelZoom (GLfloat xfactor, GLfloat yfactor);
_IMPEXP_GL void glPointSize (GLfloat size);
_IMPEXP_GL void glPolygonMode (GLenum face, GLenum mode);
_IMPEXP_GL void glPolygonOffset (GLfloat factor, GLfloat units);
_IMPEXP_GL void glPolygonStipple (const GLubyte *mask);
_IMPEXP_GL void glPopAttrib (void);
_IMPEXP_GL void glPopClientAttrib (void);
_IMPEXP_GL void glPopMatrix (void);
_IMPEXP_GL void glPopName (void);
_IMPEXP_GL void glPrioritizeTextures (GLsizei n, const GLuint *textures, const GLclampf *priorities);
_IMPEXP_GL void glPushAttrib (GLbitfield mask);
_IMPEXP_GL void glPushClientAttrib (GLbitfield mask);
_IMPEXP_GL void glPushMatrix (void);
_IMPEXP_GL void glPushName (GLuint name);
_IMPEXP_GL void glRasterPos2fv (const GLfloat *v);
_IMPEXP_GL void glRasterPos3fv (const GLfloat *v);
_IMPEXP_GL void glRasterPos4fv (const GLfloat *v);
_IMPEXP_GL void glReadBuffer (GLenum mode);
_IMPEXP_GL void glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
_IMPEXP_GL void glRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
_IMPEXP_GL void glRectdv (const GLdouble *v1, const GLdouble *v2);
_IMPEXP_GL void glRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
_IMPEXP_GL void glRectfv (const GLfloat *v1, const GLfloat *v2);
_IMPEXP_GL void glRecti (GLint x1, GLint y1, GLint x2, GLint y2);
_IMPEXP_GL void glRectiv (const GLint *v1, const GLint *v2);
_IMPEXP_GL void glRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2);
_IMPEXP_GL void glRectsv (const GLshort *v1, const GLshort *v2);
_IMPEXP_GL GLint glRenderMode (GLenum mode);
_IMPEXP_GL void glRotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
_IMPEXP_GL void glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
_IMPEXP_GL void glScaled (GLdouble x, GLdouble y, GLdouble z);
_IMPEXP_GL void glScalef (GLfloat x, GLfloat y, GLfloat z);
_IMPEXP_GL void glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
_IMPEXP_GL void glSelectBuffer (GLsizei size, GLuint *buffer);
_IMPEXP_GL void glShadeModel (GLenum mode);
_IMPEXP_GL void glStencilFunc (GLenum func, GLint ref, GLuint mask);
_IMPEXP_GL void glStencilMask (GLuint mask);
_IMPEXP_GL void glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
_IMPEXP_GL void glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
_IMPEXP_GL void glTexEnvf (GLenum target, GLenum pname, GLfloat param);
_IMPEXP_GL void glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params);
_IMPEXP_GL void glTexEnvi (GLenum target, GLenum pname, GLint param);
_IMPEXP_GL void glTexEnviv (GLenum target, GLenum pname, const GLint *params);
_IMPEXP_GL void glTexGend (GLenum coord, GLenum pname, GLdouble param);
_IMPEXP_GL void glTexGendv (GLenum coord, GLenum pname, const GLdouble *params);
_IMPEXP_GL void glTexGenf (GLenum coord, GLenum pname, GLfloat param);
_IMPEXP_GL void glTexGenfv (GLenum coord, GLenum pname, const GLfloat *params);
_IMPEXP_GL void glTexGeni (GLenum coord, GLenum pname, GLint param);
_IMPEXP_GL void glTexGeniv (GLenum coord, GLenum pname, const GLint *params);
_IMPEXP_GL void glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
_IMPEXP_GL void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
_IMPEXP_GL void glTexParameterf (GLenum target, GLenum pname, GLfloat param);
_IMPEXP_GL void glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
_IMPEXP_GL void glTexParameteri (GLenum target, GLenum pname, GLint param);
_IMPEXP_GL void glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
_IMPEXP_GL void glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
_IMPEXP_GL void glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
_IMPEXP_GL void glTranslated (GLdouble x, GLdouble y, GLdouble z);
_IMPEXP_GL void glTranslatef (GLfloat x, GLfloat y, GLfloat z);
_IMPEXP_GL void glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
_IMPEXP_GL void glViewport (GLint x, GLint y, GLsizei width, GLsizei height);

/* Multitexture exts */
_IMPEXP_GL void glMultiTexCoord1dARB (GLenum tmu, GLdouble s);
_IMPEXP_GL void glMultiTexCoord1dvARB (GLenum tmu, const GLdouble *v);
_IMPEXP_GL void glMultiTexCoord1fARB (GLenum tmu, GLfloat s);
_IMPEXP_GL void glMultiTexCoord1fvARB (GLenum tmu, const GLfloat *v);
_IMPEXP_GL void glMultiTexCoord1iARB (GLenum tmu, GLint s);
_IMPEXP_GL void glMultiTexCoord1ivARB (GLenum tmu, const GLint *v);
_IMPEXP_GL void glMultiTexCoord1sARB (GLenum tmu, GLshort s);
_IMPEXP_GL void glMultiTexCoord1svARB (GLenum tmu, const GLshort *v);
_IMPEXP_GL void glMultiTexCoord2dARB (GLenum tmu, GLdouble s, GLdouble t);
_IMPEXP_GL void glMultiTexCoord2dvARB (GLenum tmu, const GLdouble *v);
_IMPEXP_GL void glMultiTexCoord2fARB (GLenum tmu, GLfloat s, GLfloat t);
_IMPEXP_GL void glMultiTexCoord2fvARB (GLenum tmu, const GLfloat *v);
_IMPEXP_GL void glMultiTexCoord2iARB (GLenum tmu, GLint s, GLint t);
_IMPEXP_GL void glMultiTexCoord2ivARB (GLenum tmu, const GLint *v);
_IMPEXP_GL void glMultiTexCoord2sARB (GLenum tmu, GLshort s, GLshort t);
_IMPEXP_GL void glMultiTexCoord2svARB (GLenum tmu, const GLshort *v);
_IMPEXP_GL void glMultiTexCoord3dARB (GLenum tmu, GLdouble s, GLdouble t, GLdouble r);
_IMPEXP_GL void glMultiTexCoord3dvARB (GLenum tmu, const GLdouble *v);
_IMPEXP_GL void glMultiTexCoord3fARB (GLenum tmu, GLfloat s, GLfloat t, GLfloat r);
_IMPEXP_GL void glMultiTexCoord3fvARB (GLenum tmu, const GLfloat *v);
_IMPEXP_GL void glMultiTexCoord3iARB (GLenum tmu, GLint s, GLint t, GLint r);
_IMPEXP_GL void glMultiTexCoord3ivARB (GLenum tmu, const GLint *v);
_IMPEXP_GL void glMultiTexCoord3sARB (GLenum tmu, GLshort s, GLshort t, GLshort r);
_IMPEXP_GL void glMultiTexCoord3svARB (GLenum tmu, const GLshort *v);
_IMPEXP_GL void glMultiTexCoord4dARB (GLenum tmu, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
_IMPEXP_GL void glMultiTexCoord4dvARB (GLenum tmu, const GLdouble *v);
_IMPEXP_GL void glMultiTexCoord4fARB (GLenum tmu, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
_IMPEXP_GL void glMultiTexCoord4fvARB (GLenum tmu, const GLfloat *v);
_IMPEXP_GL void glMultiTexCoord4iARB (GLenum tmu, GLint s, GLint t, GLint r, GLint q);
_IMPEXP_GL void glMultiTexCoord4ivARB (GLenum tmu, const GLint *v);
_IMPEXP_GL void glMultiTexCoord4sARB (GLenum tmu, GLshort s, GLshort t, GLshort r, GLshort q);
_IMPEXP_GL void glMultiTexCoord4svARB (GLenum tmu, const GLshort *v);
_IMPEXP_GL void glActiveTextureARB(GLenum tmu);


#ifndef __PRIVATE_GL_NO_STATICS__

#if __GL_USE_FASTCALL__
static inline void glNormal3fv(const GLfloat *v)
{
	GLIM_NORMAL3FV( __gl, v );
}
static inline void glVertex2fv (const GLfloat *v)
{
	GLIM_VERTEX2FV( __gl, v );
}
static inline void glVertex2f (GLfloat x, GLfloat y)
{
	GLIM_VERTEX2F( __gl, x, y );
}
static inline void glVertex3fv (const GLfloat *v)
{
	GLIM_VERTEX3FV( __gl, v );
}
static inline void glVertex3f (GLfloat x, GLfloat y, GLfloat z)
{
	GLIM_VERTEX3F( __gl, x, y, z );
}
static inline void glVertex4fv (const GLfloat *v)
{
	GLIM_VERTEX4FV( __gl, v );
}
static inline void glVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GLIM_VERTEX4F( __gl, x, y, z, w );
}
static inline void glTexCoord1fv (const GLfloat *v)
{
	GLIM_TEXCOORD1FV( __gl, v );
}
static inline void glTexCoord1f (GLfloat s)
{
	GLIM_TEXCOORD1F( __gl, s );
}
static inline void glTexCoord2fv (const GLfloat *v)
{
	GLIM_TEXCOORD2FV( __gl, v );
}
static inline void glTexCoord2f (GLfloat s, GLfloat t)
{
	GLIM_TEXCOORD2F( __gl, s, t );
}
static inline void glTexCoord3fv (const GLfloat *v)
{
	GLIM_TEXCOORD3FV( __gl, v );
}
static inline void glTexCoord3f (GLfloat s, GLfloat t, GLfloat r)
{
	GLIM_TEXCOORD3F( __gl, s, t, r );
}
static inline void glTexCoord4fv (const GLfloat *v)
{
	GLIM_TEXCOORD4FV( __gl, v );
}
static inline void glTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	GLIM_TEXCOORD4F( __gl, s, t, r, q );
}
static inline void glColor3bv (const GLbyte *v)
{
	GLIM_COLOR3BV( __gl, v );
}
static inline void glColor3dv (const GLdouble *v)
{
	glColor3f( v[0], v[1], v[2] );
}
static inline void glColor3fv (const GLfloat *v)
{
	GLIM_COLOR3FV( __gl, v );
}
static inline void glColor3iv (const GLint *v)
{
	GLIM_COLOR3IV( __gl, v );
}
static inline void glColor3sv (const GLshort *v)
{
	GLIM_COLOR3SV( __gl, v );
}
static inline void glColor3ubv (const GLubyte *v)
{
	GLIM_COLOR3UBV( __gl, v );
}
static inline void glColor3uiv (const GLuint *v)
{
	GLIM_COLOR3UIV( __gl, v );
}
static inline void glColor3usv (const GLushort *v)
{
	GLIM_COLOR3USV( __gl, v );
}
static inline void glColor4bv (const GLbyte *v)
{
	GLIM_COLOR4BV( __gl, v );
}
static inline void glColor4dv (const GLdouble *v)
{
	glColor4f( v[0], v[1], v[2], v[3] );
}
static inline void glColor4fv (const GLfloat *v)
{
	GLIM_COLOR4FV( __gl, v );
}
static inline void glColor4sv (const GLshort *v)
{
	GLIM_COLOR4SV( __gl, v );
}
static inline void glColor4iv (const GLint *v)
{
	GLIM_COLOR4IV( __gl, v );
}
static inline void glColor4ubv (const GLubyte *v)
{
	GLIM_COLOR4UBV( __gl, v );
}
static inline void glColor4uiv (const GLuint *v)
{
	GLIM_COLOR4UIV( __gl, v );
}
static inline void glColor4usv (const GLushort *v)
{
	GLIM_COLOR4USV( __gl, v );
}
static inline void glBegin( GLenum prim )
{
	GLIM_BEGIN( __gl, prim );
}

static inline void glEnd()
{
	GLIM_END( __gl );
}
#else
_IMPEXP_GL void glNormal3fv(const GLfloat *v);
_IMPEXP_GL void glVertex2fv (const GLfloat *v);
_IMPEXP_GL void glVertex2f (GLfloat x, GLfloat y);
_IMPEXP_GL void glVertex3fv (const GLfloat *v);
_IMPEXP_GL void glVertex3f (GLfloat x, GLfloat y, GLfloat z);
_IMPEXP_GL void glVertex4fv (const GLfloat *v);
_IMPEXP_GL void glVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
_IMPEXP_GL void glTexCoord1fv (const GLfloat *v);
_IMPEXP_GL void glTexCoord1f (GLfloat s);
_IMPEXP_GL void glTexCoord2fv (const GLfloat *v);
_IMPEXP_GL void glTexCoord2f (GLfloat s, GLfloat t);
_IMPEXP_GL void glTexCoord3fv (const GLfloat *v);
_IMPEXP_GL void glTexCoord3f (GLfloat s, GLfloat t, GLfloat r);
_IMPEXP_GL void glTexCoord4fv (const GLfloat *v);
_IMPEXP_GL void glTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q);
_IMPEXP_GL void glColor3bv (GLbyte *v);
_IMPEXP_GL void glColor3dv (GLdouble *v);
_IMPEXP_GL void glColor3fv (const GLfloat *v);
_IMPEXP_GL void glColor3iv (GLint *v);
_IMPEXP_GL void glColor3sv (GLshort *v);
_IMPEXP_GL void glColor3ubv (GLubyte *v);
_IMPEXP_GL void glColor3uiv (GLuint *v);
_IMPEXP_GL void glColor3usv (GLushort *v);
_IMPEXP_GL void glColor4bv (GLbyte *v);
_IMPEXP_GL void glColor4dv (GLdouble *v);
_IMPEXP_GL void glColor4fv (const GLfloat *v);
_IMPEXP_GL void glColor4sv (GLshort *v);
_IMPEXP_GL void glColor4iv (GLint *v);
_IMPEXP_GL void glColor4ubv (GLubyte *v);
_IMPEXP_GL void glColor4uiv (GLuint *v);
_IMPEXP_GL void glColor4usv (GLushort *v);
_IMPEXP_GL void glBegin( GLenum prim );
_IMPEXP_GL void glEnd();
#endif

static inline void glNormal3b (GLbyte nx, GLbyte ny, GLbyte nz)
{
	GLfloat v[3];
	v[0] = nx;
	v[1] = ny;
	v[2] = nz;
	glNormal3fv( v );
}
static inline void glNormal3bv (const GLbyte *v)
{
	GLfloat vv[3];
	vv[0] = v[0];
	vv[1] = v[1];
	vv[2] = v[2];
	glNormal3fv( vv );
}
static inline void glNormal3d (GLdouble nx, GLdouble ny, GLdouble nz)
{
	GLfloat v[3];
	v[0] = nx;
	v[1] = ny;
	v[2] = nz;
	glNormal3fv( v );
}
static inline void glNormal3dv (const GLdouble *v)
{
	GLfloat vv[3];
	vv[0] = v[0];
	vv[1] = v[1];
	vv[2] = v[2];
	glNormal3fv( vv );
}
static inline void glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
	GLfloat v[3];
	v[0] = nx;
	v[1] = ny;
	v[2] = nz;
	glNormal3fv( v );
}
static inline void glNormal3i (GLint nx, GLint ny, GLint nz)
{
	GLfloat v[3];
	v[0] = nx;
	v[1] = ny;
	v[2] = nz;
	glNormal3fv( v );
}
static inline void glNormal3iv (const GLint *v)
{
	GLfloat vv[3];
	vv[0] = v[0];
	vv[1] = v[1];
	vv[2] = v[2];
	glNormal3fv( vv );
}
static inline void glNormal3s (GLshort nx, GLshort ny, GLshort nz)
{
	GLfloat v[3];
	v[0] = nx;
	v[1] = ny;
	v[2] = nz;
	glNormal3fv( v );
}
static inline void glNormal3sv (const GLshort *v)
{
	GLfloat vv[3];
	vv[0] = v[0];
	vv[1] = v[1];
	vv[2] = v[2];
	glNormal3fv( vv );
}

static inline void glVertex2d (GLdouble x, GLdouble y)
{
	glVertex2f( x, y );
}
static inline void glVertex2dv (const GLdouble *v)
{
	glVertex2f( v[0], v[1] );
}
static inline void glVertex2i (GLint x, GLint y)
{
	glVertex2f( x, y );
}
static inline void glVertex2iv (const GLint *v)
{
	glVertex2f( v[0], v[1] );
}
static inline void glVertex2s (GLshort x, GLshort y)
{
	glVertex2f( x, y );
}
static inline void glVertex2sv (const GLshort *v)
{
	glVertex2f( v[0], v[1] );
}

static inline void glVertex3d (GLdouble x, GLdouble y, GLdouble z)
{
	glVertex3f( x, y, z );
}
static inline void glVertex3dv (const GLdouble *v)
{
	glVertex3f( v[0], v[1], v[2] );
}
static inline void glVertex3i (GLint x, GLint y, GLint z)
{
	glVertex3f( x, y, z );
}
static inline void glVertex3iv (const GLint *v)
{
	glVertex3f( v[0], v[1], v[2] );
}
static inline void glVertex3s (GLshort x, GLshort y, GLshort z)
{
	glVertex3f( x, y, z );
}
static inline void glVertex3sv (const GLshort *v)
{
	glVertex3f( v[0], v[1], v[2] );
}

static inline void glVertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	glVertex4f( x, y, z, w );
}
static inline void glVertex4dv (const GLdouble *v)
{
	glVertex4f( v[0], v[1], v[2], v[3] );
}
static inline void glVertex4i (GLint x, GLint y, GLint z, GLint w)
{
	glVertex4f( x, y, z, w );
}
static inline void glVertex4iv (const GLint *v)
{
	glVertex4f( v[0], v[1], v[2], v[3] );
}
static inline void glVertex4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
	glVertex4f( x, y, z, w );
}
static inline void glVertex4sv (const GLshort *v)
{
	glVertex4f( v[0], v[1], v[2], v[3] );
}




static inline void glTexCoord1d (GLdouble s)
{
	glTexCoord1f( s );
}
static inline void glTexCoord1dv (const GLdouble *v)
{
	glTexCoord1f( *v );
}
static inline void glTexCoord1i (GLint s)
{
	glTexCoord1f( s );
}
static inline void glTexCoord1iv (const GLint *v)
{
	glTexCoord1f( *v );
}
static inline void glTexCoord1s (GLshort s)
{
	glTexCoord1f( s );
}
static inline void glTexCoord1sv (const GLshort *v)
{
	glTexCoord1f( *v );
}


static inline void glTexCoord2d (GLdouble s, GLdouble t)
{
	glTexCoord2f( s, t );
}
static inline void glTexCoord2dv (const GLdouble *v)
{
	glTexCoord2f( v[0], v[1] );
}
static inline void glTexCoord2i (GLint s, GLint t)
{
	glTexCoord2f( s, t );
}
static inline void glTexCoord2iv (const GLint *v)
{
	glTexCoord2f( v[0], v[1] );
}
static inline void glTexCoord2s (GLshort s, GLshort t)
{
	glTexCoord2f( s, t );
}
static inline void glTexCoord2sv (const GLshort *v)
{
	glTexCoord2f( v[0], v[1] );
}

static inline void glTexCoord3d (GLdouble s, GLdouble t, GLdouble r)
{
	glTexCoord3f( s, t, r );
}
static inline void glTexCoord3dv (const GLdouble *v)
{
	glTexCoord3f( v[0], v[1], v[2] );
}
static inline void glTexCoord3i (GLint s, GLint t, GLint r)
{
	glTexCoord3f( s, t, r );
}
static inline void glTexCoord3iv (const GLint *v)
{
	glTexCoord3f( v[0], v[1], v[2] );
}
static inline void glTexCoord3s (GLshort s, GLshort t, GLshort r)
{
	glTexCoord3f( s, t, r );
}
static inline void glTexCoord3sv (const GLshort *v)
{
	glTexCoord3f( v[0], v[1], v[2] );
}

static inline void glTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
	glTexCoord4f( s, t, r, q );
}
static inline void glTexCoord4dv (const GLdouble *v)
{
	glTexCoord4f( v[0], v[1], v[2], v[3] );
}
static inline void glTexCoord4i (GLint s, GLint t, GLint r, GLint q)
{
	glTexCoord4f( s, t, r, q );
}
static inline void glTexCoord4iv (const GLint *v)
{
	glTexCoord4f( v[0], v[1], v[2], v[3] );
}
static inline void glTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q)
{
	glTexCoord4f( s, t, r, q );
}
static inline void glTexCoord4sv (const GLshort *v)
{
	glTexCoord4f( v[0], v[1], v[2], v[3] );
}


static inline void glRasterPos2d (GLdouble x, GLdouble y)
{
	GLfloat vv[2];
	vv[0] = x;
	vv[1] = y;
	glRasterPos2fv( vv );
}
static inline void glRasterPos2dv (const GLdouble *v)
{
	GLfloat vv[2];
	vv[0] = v[0];
	vv[1] = v[1];
	glRasterPos2fv( vv );
}
static inline void glRasterPos2f (GLfloat x, GLfloat y)
{
	GLfloat vv[2];
	vv[0] = x;
	vv[1] = y;
	glRasterPos2fv( vv );
}
static inline void glRasterPos2i (GLint x, GLint y)
{
	GLfloat vv[2];
	vv[0] = x;
	vv[1] = y;
	glRasterPos2fv( vv );
}
static inline void glRasterPos2iv (const GLint *v)
{
	GLfloat vv[2];
	vv[0] = v[0];
	vv[1] = v[1];
	glRasterPos2fv( vv );
}
static inline void glRasterPos2s (GLshort x, GLshort y)
{
	GLfloat vv[2];
	vv[0] = x;
	vv[1] = y;
	glRasterPos2fv( vv );
}
static inline void glRasterPos2sv (const GLshort *v)
{
	GLfloat vv[2];
	vv[0] = v[0];
	vv[1] = v[1];
	glRasterPos2fv( vv );
}

static inline void glRasterPos3d (GLdouble x, GLdouble y, GLdouble z)
{
	GLfloat vv[3];
	vv[0] = x;
	vv[1] = y;
	vv[2] = z;
	glRasterPos3fv( vv );
}
static inline void glRasterPos3dv (const GLdouble *v)
{
	GLfloat vv[3];
	vv[0] = v[0];
	vv[1] = v[1];
	vv[2] = v[2];
	glRasterPos3fv( vv );
}
static inline void glRasterPos3f (GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat vv[3];
	vv[0] = x;
	vv[1] = y;
	vv[2] = z;
	glRasterPos3fv( vv );
}
static inline void glRasterPos3i (GLint x, GLint y, GLint z)
{
	GLfloat vv[3];
	vv[0] = x;
	vv[1] = y;
	vv[2] = z;
	glRasterPos3fv( vv );
}
static inline void glRasterPos3iv (const GLint *v)
{
	GLfloat vv[3];
	vv[0] = v[0];
	vv[1] = v[1];
	vv[2] = v[2];
	glRasterPos3fv( vv );
}
static inline void glRasterPos3s (GLshort x, GLshort y, GLshort z)
{
	GLfloat vv[3];
	vv[0] = x;
	vv[1] = y;
	vv[2] = z;
	glRasterPos3fv( vv );
}
static inline void glRasterPos3sv (const GLshort *v)
{
	GLfloat vv[3];
	vv[0] = v[0];
	vv[1] = v[1];
	vv[2] = v[2];
	glRasterPos3fv( vv );
}

static inline void glRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	GLfloat vv[4];
	vv[0] = x;
	vv[1] = y;
	vv[2] = z;
	vv[3] = w;
	glRasterPos4fv( vv );
}
static inline void glRasterPos4dv (const GLdouble *v)
{
	GLfloat vv[4];
	vv[0] = v[0];
	vv[1] = v[1];
	vv[2] = v[2];
	vv[3] = v[3];
	glRasterPos4fv( vv );
}
static inline void glRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GLfloat vv[4];
	vv[0] = x;
	vv[1] = y;
	vv[2] = z;
	vv[3] = w;
	glRasterPos4fv( vv );
}
static inline void glRasterPos4i (GLint x, GLint y, GLint z, GLint w)
{
	GLfloat vv[4];
	vv[0] = x;
	vv[1] = y;
	vv[2] = z;
	vv[3] = w;
	glRasterPos4fv( vv );
}
static inline void glRasterPos4iv (const GLint *v)
{
	GLfloat vv[4];
	vv[0] = v[0];
	vv[1] = v[1];
	vv[2] = v[2];
	vv[3] = v[3];
	glRasterPos4fv( vv );
}
static inline void glRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
	GLfloat vv[4];
	vv[0] = x;
	vv[1] = y;
	vv[2] = z;
	vv[3] = w;
	glRasterPos4fv( vv );
}
static inline void glRasterPos4sv (const GLshort *v)
{
	GLfloat vv[4];
	vv[0] = v[0];
	vv[1] = v[1];
	vv[2] = v[2];
	vv[3] = v[3];
	glRasterPos4fv( vv );
}

static inline void glColor3b (GLbyte red, GLbyte green, GLbyte blue)
{
	GLbyte c[3];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	glColor3bv( c );
}
static inline void glColor3d (GLdouble red, GLdouble green, GLdouble blue)
{
	GLdouble c[3];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	glColor3dv( c );
}
static inline void glColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
	GLfloat c[3];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	glColor3fv( c );
}
static inline void glColor3i (GLint red, GLint green, GLint blue)
{
	GLint c[3];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	glColor3iv( c );
}
static inline void glColor3s (GLshort red, GLshort green, GLshort blue)
{
	GLshort c[3];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	glColor3sv( c );
}
static inline void glColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
	GLubyte c[3];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	glColor3ubv( c );
}
static inline void glColor3ui (GLuint red, GLuint green, GLuint blue)
{
	GLuint c[3];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	glColor3uiv( c );
}
static inline void glColor3us (GLushort red, GLushort green, GLushort blue)
{
	GLushort c[3];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	glColor3usv( c );
}
static inline void glColor4b (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{
	GLbyte c[4];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	c[3] = alpha;
	glColor4bv( c );
}
static inline void glColor4d (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
	GLdouble c[4];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	c[3] = alpha;
	glColor4dv( c );
}
static inline void glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	GLfloat c[4];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	c[3] = alpha;
	glColor4fv( c );
}
static inline void glColor4i (GLint red, GLint green, GLint blue, GLint alpha)
{
	GLint c[4];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	c[3] = alpha;
	glColor4iv( c );
}
static inline void glColor4s (GLshort red, GLshort green, GLshort blue, GLshort alpha)
{
	GLshort c[4];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	c[3] = alpha;
	glColor4sv( c );
}
static inline void glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	GLubyte c[4];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	c[3] = alpha;
	glColor4ubv( c );
}
static inline void glColor4ui (GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
	GLuint c[4];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	c[3] = alpha;
	glColor4uiv( c );
}
static inline void glColor4us (GLushort red, GLushort green, GLushort blue, GLushort alpha)
{
	GLushort c[4];
	c[0] = red;
	c[1] = green;
	c[2] = blue;
	c[3] = alpha;
	glColor4usv( c );
}



#endif


#ifdef __cplusplus
}
#endif

#endif /* __gl_h_ */
