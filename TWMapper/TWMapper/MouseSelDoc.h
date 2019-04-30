// MouseSelDoc.h : interface of the CMouseSelDoc class
//


#pragma once
#include "Sphere.h"
#include "Line.h"
#include "Axis.h"

class CMouseSelDoc : public CDocument
{
protected: // create from serialization only
	CMouseSelDoc();
	DECLARE_DYNCREATE(CMouseSelDoc)

// Attributes
public:
	//Members for Serilization
	double m_RedColor,m_GreenColor,m_BlueColor,m_dRadius;
    CShapeList m_ShapeList; 
	int m_count;
	
	// Position, rotation ,zooming
	float m_xRotation;
	float m_yRotation;
	float m_zRotation;
	BOOL m_xyRotation;

	float m_xTranslation;
	float m_yTranslation;
	float m_zTranslation;

	
	float m_SpeedTranslation;
	float m_SpeedRotation;
	double m_y;

// Operations
public:
	BOOL isInShapeList (CSphere *obj);
	BOOL isInShapeList (CLine *obj);
	CSphere* getSphereFromList (long name);


// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CMouseSelDoc();
	void InitDoc();
	void RenderScene();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};


