#pragma once
#ifndef CVVIMAGE_CLASS_DEF
#define CVVIMAGE_CLASS_DEF
#include "opencv2\opencv.hpp"
/* CvvImage class definition */
class  CvvImage
{
public:
   CvvImage();
   virtual ~CvvImage();
   /* Create image (BGR or grayscale) */
   virtual bool  Create( int width, int height, int bits_per_pixel, int image_origin = 0 );
   /* Load image from specified file */
   virtual bool  Load( const char* filename, int desired_color = 1 );
   /* Load rectangle from the file */
   virtual bool  LoadRect( const char* filename,
      int desired_color, CvRect r );
#if defined WIN32 || defined _WIN32
   virtual bool  LoadRect( const char* filename,
      int desired_color, RECT r )
   {
      return LoadRect( filename, desired_color,
         cvRect( r.left, r.top, r.right - r.left, r.bottom - r.top ));
   }
#endif
   /* Save entire image to specified file. */
   virtual bool  Save( const char* filename );
   /* Get copy of input image ROI */
   virtual void  CopyOf( CvvImage& image, int desired_color = -1 );
   virtual void  CopyOf( IplImage* img, int desired_color = -1 );
   IplImage* GetImage() { return m_img; };
   virtual void  Destroy(void);
   /* width and height of ROI */
   int Width() { return !m_img ? 0 : !m_img->roi ? m_img->width : m_img->roi->width; };
   int Height() { return !m_img ? 0 : !m_img->roi ? m_img->height : m_img->roi->height;};
   int Bpp() { return m_img ? (m_img->depth & 255)*m_img->nChannels : 0; };
   virtual void  Fill( int color );
   /* draw to highgui window */
   virtual void  Show( const char* window );

#if defined WIN32 || defined _WIN32
   /* draw part of image to the specified DC */
   virtual void  Show( HDC dc, int x, int y, int width, int height,
      int from_x = 0, int from_y = 0 );
   /* draw the current image ROI to the specified rectangle of the destination DC */
   virtual void  DrawToHDC( HDC hDCDst, RECT* pDstRect );
#endif
protected:
   IplImage*  m_img;
};
typedef CvvImage CImage;
#endif