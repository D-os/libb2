/*******************************************************************************
/
/	File:		GLView.h
/
/	Copyright 1993-98, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/

#ifndef BGLVIEW_H
#define BGLVIEW_H

#include <GL/gl.h>
#define BGL_RGB			0
#define BGL_INDEX		1 
#define BGL_SINGLE		0
#define BGL_DOUBLE		2
#define BGL_DIRECT		0
#define BGL_INDIRECT	4
#define BGL_ACCUM		8
#define BGL_ALPHA		16
#define BGL_DEPTH		32
#define BGL_OVERLAY		64
#define BGL_UNDERLAY	128
#define BGL_STENCIL		512

#ifdef __cplusplus

#include <AppKit.h>
#include <interface/Window.h>
#include <interface/View.h>
#include <interface/Bitmap.h>
#include <game/WindowScreen.h>
#include <game/DirectWindow.h>

#if NUOPENGL
	struct __glContextRec;
#endif

typedef struct __glPerWindowDataRec __glPerWindowData;


class BGLView : public BView {
public:

					BGLView(BRect rect, char *name,
						ulong resizingMode, ulong mode,
						ulong options);
virtual				~BGLView();

		void		LockGL();
		void		UnlockGL();
		void		SwapBuffers();
		void		SwapBuffers( bool vSync );
		BView *     EmbeddedView();
		status_t    CopyPixelsOut(BPoint source, BBitmap *dest);
		status_t    CopyPixelsIn(BBitmap *source, BPoint dest);
virtual void        ErrorCallback(GLenum errorCode);
		
virtual	void		Draw(BRect updateRect);

virtual void		AttachedToWindow();
virtual void        AllAttached();
virtual void        DetachedFromWindow();
virtual void        AllDetached();
 
virtual void		FrameResized(float width, float height);
virtual status_t    Perform(perform_code d, void *arg);

/* The public methods below, for the moment,
   are just pass-throughs to BView */

virtual status_t    Archive(BMessage *data, bool deep = true) const;

virtual void        MessageReceived(BMessage *msg);
virtual void        SetResizingMode(uint32 mode);

virtual void        Show();
virtual void        Hide();

virtual BHandler   *ResolveSpecifier(BMessage *msg, int32 index,
							BMessage *specifier, int32 form,
							const char *property);
virtual status_t    GetSupportedSuites(BMessage *data);

/* New public functions */
		void		DirectConnected( direct_buffer_info *info );
		void		EnableDirectMode( bool enabled );

		void * getGC() {return m_gc;}

private:

virtual void        _ReservedGLView1();
virtual void        _ReservedGLView2(); 
virtual void        _ReservedGLView3(); 
virtual void        _ReservedGLView4(); 
virtual void        _ReservedGLView5(); 
virtual void        _ReservedGLView6(); 
virtual void        _ReservedGLView7(); 
virtual void        _ReservedGLView8(); 

                    BGLView(const BGLView &);
	    BGLView     &operator=(const BGLView &);

		void        dither_front();
		bool        confirm_dither();
		void        draw(BRect r);
		
#if NUOPENGL
		struct __glContextRec 	*m_gc;
#else
		void *		m_gc;
#endif
		uint32		m_options;
		uint32      m_ditherCount;
		BLocker		m_drawLock;
		BLocker     m_displayLock;
#if OLD_GLVIEW
		BView *		m_embeddedFront;
		BView *     m_embeddedBack;
#else
		void *		m_clip_info;
		__glPerWindowData *m_window_data;
#endif
		BBitmap *   m_ditherMap;
		BRect       m_bounds;
		int16 *     m_errorBuffer[2];
		uint64      _reserved[8];

	/* Direct Window stuff */
private:	
		void 		drawScanline( int x1, int x2, int y, void *data );
#if NUOPENGL
static	void 		scanlineHandler(struct __glContextRec *gc, GLint x1, GLint x2);
#else
static	void 		scanlineHandler(struct rasStateRec *state, GLint x1, GLint x2);
#endif
		void		lock_draw();
		void		unlock_draw();
		bool		validateView();
};

#endif
#endif





