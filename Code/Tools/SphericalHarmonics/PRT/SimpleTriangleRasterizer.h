#if defined(OFFLINE_COMPUTATION)

#pragma once

// Helper class
//
// clipping is done in integer
//

typedef unsigned int uint32;

class CSimpleTriangleRasterizer
{
public:

	struct IRasterizeSink
	{
		virtual ~IRasterizeSink() {}
		//! is called once per triangle for the first possible visible line
		//! /param iniStartY 
		virtual void Triangle( const int)
		{
		}

		//! callback function
		//! /param infXLeft included - not clipped against left and right border
		//! /param infXRight excluded - not clipped against left and right border
		//! /param iniXLeft included
		//! /param iniXRight excluded
		//! /param iniY 
		virtual void Line( const float infXLeft, const float infXRight,
						   const int iniXLeft, const int iniXRight, const int iniY )=0;
	};

	// -----------------------------------------------------

	//! implementation sink sample
	class CDWORDFlatFill: public IRasterizeSink
	{
	public:

		//! constructor
		CDWORDFlatFill( uint32 *inpBuffer, const uint32 indwPitchInPixels, uint32 indwValue )
		{
			m_dwValue=indwValue;
			m_pBuffer=inpBuffer;
			m_dwPitchInPixels=indwPitchInPixels;
		}

		virtual void Triangle( const int iniY )
		{
			m_pBufferLine=&m_pBuffer[iniY*m_dwPitchInPixels];
		}

		virtual void Line( const float, const float,
						   const int iniLeft, const int iniRight, const int)
		{
			uint32 *mem=&m_pBufferLine[iniLeft];

			for(int x=iniLeft;x<iniRight;x++)
				*mem++=m_dwValue;

			m_pBufferLine+=m_dwPitchInPixels;
		}

	private:
		uint32 m_dwValue;					//!< fill value
		uint32 *m_pBufferLine;			//!< to get rid of the multiplication per line

		uint32 m_dwPitchInPixels;	//!< in DWORDS, not in Bytes
		uint32 *m_pBuffer;					//!< pointer to the buffer
	};

	// -----------------------------------------------------

	//! constructor
	//! /param iniWidth excluded
	//! /param iniHeight excluded
	CSimpleTriangleRasterizer( const int iniWidth, const int iniHeight )
	{
		m_iMinX=0;
		m_iMinY=0;
		m_iMaxX=iniWidth-1;
		m_iMaxY=iniHeight-1;
	}
/*
	//! constructor 
	//! /param iniMinX included
	//! /param iniMinY included
	//! /param iniMaxX included
	//! /param iniMaxY included
	CSimpleTriangleRasterizer( const int iniMinX, const int iniMinY, const int iniMaxX, const int iniMaxY )
	{
		m_iMinX=iniMinX;
		m_iMinY=iniMinY;
		m_iMaxX=iniMaxX;
		m_iMaxY=iniMaxY;
	}
*/
	//! simple triangle filler with clipping (optimizable), not subpixel correct
	//! /param pBuffer pointer o the color buffer
	//! /param indwWidth width of the color buffer
	//! /param indwHeight height of the color buffer
	//! /param x array of the x coordinates of the three vertices
	//! /param y array of the x coordinates of the three vertices
	//! /param indwValue value of the triangle
	void DWORDFlatFill( uint32 *inpBuffer, const uint32 indwPitchInPixels, float x[3], float y[3], uint32 indwValue, bool inbConservative )
	{
		CDWORDFlatFill pix(inpBuffer,indwPitchInPixels,indwValue);

		if(inbConservative)
			CallbackFillConservative(x,y,&pix);
		  else
			CallbackFillSubpixelCorrect(x,y,&pix);
	}

	// Rectangle around triangle - more stable - use for debugging purpose
	void CallbackFillRectConservative( float x[3], float y[3], IRasterizeSink *inpSink );


	//! subpixel correct triangle filler (conservative or not conservative)
	//! \param pBuffer points to the uint32
	//! \param indwWidth width of the buffer pBuffer points to
	//! \param indwHeight height of the buffer pBuffer points to
	//! \param x array of the x coordinates of the three vertices
	//! \param y array of the x coordinates of the three vertices
	//! \param inpSink pointer to the sink interface (is called per triangle and per triangle line)
	void CallbackFillConservative( float x[3], float y[3], IRasterizeSink *inpSink );

	//! subpixel correct triangle filler (conservative or not conservative)
	//! \param pBuffer points to the uint32
	//! \param indwWidth width of the buffer pBuffer points to
	//! \param indwHeight height of the buffer pBuffer points to
	//! \param x array of the x coordinates of the three vertices
	//! \param y array of the x coordinates of the three vertices
	//! \param inpSink pointer to the sink interface (is called per triangle and per triangle line)
	void CallbackFillSubpixelCorrect( float x[3], float y[3], IRasterizeSink *inpSink );

	//!
	//! /param inoutfX
	//! /param inoutfY
	//! /param infAmount could be positive or negative
	static void ShrinkTriangle( float inoutfX[3], float inoutfY[3], float infAmount );

private:

	// Clipping Rect;

	int		m_iMinX;		//!< minimum x value included 
	int		m_iMinY;		//!< minimum y value included
	int		m_iMaxX;		//!< maximum x value included
	int		m_iMaxY;		//!< maximum x value included

	void lambertHorizlineConservative( float fx1, float fx2, int y, IRasterizeSink *inpSink );
	void lambertHorizlineSubpixelCorrect( float fx1, float fx2, int y, IRasterizeSink *inpSink );
	void CopyAndSortY( const float infX[3], const float infY[3], float outfX[3],float outfY[3] );
};


// extension ideas:
// * callback with coverage mask (possible non ordered sampling)
// * z-buffer behavior
// * gouraud shading
// * texture mapping with nearest/bicubic/bilinear filter
// * further primitives: thick line, ellipse
// * build a template version
// * 

#endif