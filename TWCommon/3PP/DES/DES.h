// DES.h: interface for the CDES class.
//

#if !defined(AFX_DES_H__0ECC5BE2_B77B_11D2_A3C4_3CB908C10000__INCLUDED_)
#define AFX_DES_H__0ECC5BE2_B77B_11D2_A3C4_3CB908C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum DES_Mode {
	DES_DEFAULT,
	DES_NORMAL,
	DES_3DES,
	DES_EC_M,
	DES_EC_S,
	DES_EC_S3
};

class CDES
{
public:
	CDES();
	virtual ~CDES();

	void SetMode(DES_Mode mode);
	void SetKey(int nKey, unsigned char *pKey, int size=7);
	void Encrypt(unsigned char *pData, DES_Mode mode=DES_DEFAULT,
		unsigned char *pKey1=NULL, unsigned char *pKey2=NULL);
	void Decrypt(unsigned char *pData, DES_Mode mode=DES_DEFAULT,
		unsigned char *pKey1=NULL, unsigned char *pKey2=NULL);
	void ECB(unsigned char *pData, int cnt, BOOL bEncrypt);
	void CBC(unsigned char *pData, int cnt, unsigned char *pIV, BOOL bEncrypt);

private:
	void ExecDES(unsigned char *pData, unsigned char *pKey, int nKeySize,
		BOOL bKeyPC1, BOOL bViaKeyPerm);
	void HashFunc(unsigned char *pData, DES_Mode mode=DES_DEFAULT,
		unsigned char *pKey1=NULL, BOOL bInv=FALSE);
	void DoIP(unsigned char *pData);
	void DoIP_1(unsigned char *pData);
	void DoPC1(unsigned char *pKey, int nKeySize, BOOL bInv=FALSE);

private:
	static unsigned char *DES_pFunc;
	static unsigned char *DES_pKey;
	static unsigned char *DES_pData;
	static unsigned char *DES_pRes;
	static unsigned char *DES_pECW;
	DES_Mode m_mode;
	unsigned char m_key[2][8];
	int m_keysize[2];
};

#endif // !defined(AFX_DES_H__0ECC5BE2_B77B_11D2_A3C4_3CB908C10000__INCLUDED_)
