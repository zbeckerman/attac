#pragma once


//Abstract Base class for SHape objects
// CShape command target

class CShape : public CObject
{
public:
	CShape();
	virtual ~CShape();

//Attributes
public:
	//color
	double		m_RedColor,m_GreenColor,m_BlueColor;

	//Shape Selection
	BOOL		m_Select;
public:	
	//Members for object point and transformation
	CRect		m_rect;
	double		m_objx; 
  	double		m_objy; 
  	double		m_objz; 
  	double		m_modelMatrix[16];
  	double		m_projMatrix[16];
  	int			m_viewport[4];

  	//Members for storing projected point
	double		m_winx;
  	double		m_winy;
  	double		m_winz; 
	double		m_xc,m_yc,m_zc;
	BOOL		m_visible;
// Operations
public:
// Methods
	
// Overrides
	//DECLARE_SERIAL(CShape)
	virtual void Serialize(CArchive& ar)=0;
	virtual void Draw()=0;
	BOOL Intersects(CShape *s);
	virtual CRect GetRect()=0;
	
};
typedef CTypedPtrList<CObList, CShape*> CShapeList;


