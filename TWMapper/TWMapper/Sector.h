#pragma once


// CSector command target

class CSector
{
public:
	CSector();
	virtual ~CSector();

	long sectorNumber;
	long warps[6];
	long sectorCount;
	BOOL stardock;
	BOOL rylos;
	BOOL alphaCentauri;
	BOOL sol;

// Operations
	BOOL Equals(long a_sector);
public:
// Overrides
//	DECLARE_SERIAL(CSector)
//	virtual void Serialize(CArchive& ar);
};


