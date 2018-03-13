#include <iostream>  
using namespace std;
//test dzl
class CURLCode
{
public:
	CURLCode() {};
	virtual ~CURLCode() {};
	string Encode(const char* lpszData);
	string Decode(string inTmp);

private:
	inline BYTE toHex(const BYTE &x) { return x > 9 ? x + 55 : x + 48; }
	inline BYTE fromHex(const BYTE &x) { return x > 64 ? x - 55 : x - 48; }
};
//Test test
string CURLCode::Encode(const char* lpszData)
{
	string strResult = "";

	unsigned char* pInTmp = (unsigned char*)lpszData;
	// do encoding
	while (*pInTmp)
	{
		if (isalnum(*pInTmp))
			strResult += *pInTmp;
		else
			if (isspace(*pInTmp))
				strResult += '+';
			else
			{
				strResult += '%';
				strResult += toHex(*pInTmp >> 4);
				strResult += toHex(*pInTmp % 16);
			}
		pInTmp++;
	}
	return strResult;
}

string CURLCode::Decode(string inTmp)
{
	string strResult = "";
	int inlen = inTmp.length();
	for (int i = 0; i<inlen; i++)
	{
		if (inTmp.at(i) == '%')
		{
			i++;
			char c = fromHex(inTmp.at(i++));
			c = c << 4;
			c += fromHex(inTmp.at(i));
			strResult += c;
		}
		else if (inTmp.at(i) == '+')
			strResult += ' ';
		else
			strResult += inTmp.at(i);
	}
	return strResult;
}