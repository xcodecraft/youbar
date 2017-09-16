#include "common.h"
#include "baselib/encrypt_str.h"

int HexStrToint(const std::string &m_HexStr)
{
	int len=m_HexStr.size(),result=0;
	char ch;
	for(int i=0;i<len;i++)
	{
		ch=(char)m_HexStr.at(i);
		if((ch<='F'&&ch>='A')||(ch>='0'&&ch<='9')||(ch<='f'&&ch>='a'))
		{
			if(ch<='9'&&ch>='0')
				result=result*16+(ch-0x30);
			else if(ch<='F'&&ch>='A')
				result=result*16+(10+ch-'A');
			else
				result=result*16+(10+ch-'a');
		}
		else
			return result;
	}
	return result;
}

BYTE key[8]= {0xB2 , 0x09 , 0xAA , 0x55 , 0x93 , 0x55 , 0x84 , 0x47};
std::string enc::encrypt( std::string text )
{
	std::string res = "";
	int len = text.size();
	int j = 0;
	for (int i = 0; i < len ; i++)
	{
		BYTE bt = (BYTE)text.at(i) ^ 0xFF;
		char buffer[3] = "";
		sprintf_s(buffer,"%x" ,bt);
		std::string bts = buffer;
		res = res + bts;
		j = (j + 1) % 8 ;
	}
	return res;
}

std::string enc::decrypt( std::string text )
{
	std::string res = "";
	int len = text.size() / 2;
	int j = 0;
	for (int i = 0; i < len ; i++)
	{
		char buffer[3] = "";
		std::string subs = text.substr(i*2 ,2);
		char bt = HexStrToint(subs) ^ 0xFF;
		sprintf_s(buffer,"%c" ,bt);
		std::string bts = buffer;
		res = res + bts;
		j = (j + 1) % 8 ;
	}
	return res;
}