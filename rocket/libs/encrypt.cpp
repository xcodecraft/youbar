#include "common.h"
#include "baselib/encrypt.h"
#include <windows.h>
#include <wincrypt.h>
#include "boost/shared_array.hpp"

#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
#define KEYLENGTH  0x00800000
#define ENCRYPT_ALGORITHM CALG_RC4
#define ENCRYPT_BLOCK_SIZE 8 

using namespace std;
using namespace yyou;
using namespace boost; 

#define CY_CRYPT_KEY "caiyunlovecaiyun"
class encypt_win  : public smart_def<encypt_win> 
{
	HCRYPTPROV _crypt_prov; 
	HCRYPTHASH _crypt_hash; 
	HCRYPTKEY _crypt_key;
	void build_cypt_key(const char *  passwd)
	{
		// Create a hash object. 
		sys_err err(L"build_cypt_key");
		WAPI_NOT_FALSE(CryptCreateHash( _crypt_prov, CALG_MD5, 0, 0, &_crypt_hash),err);
		// ����һ���Ự��Կ��session key�� �Ự��ԿҲ�жԳ���Կ�����ڶԳƼ����㷨��
		// ��ע: һ��Session��ָ�ӵ��ú���CryptAcquireContext�����ú���
		//   CryptReleaseContext �ڼ�Ľ׶Ρ��Ự��Կֻ�ܴ�����һ���Ự���̣�
		// ��������������һ��ɢ��
		WAPI_NOT_FALSE(CryptHashData( _crypt_hash, (BYTE *)passwd, strlen(passwd) , 0),err);

		//--------------------------------------------------------------------
		// ͨ��ɢ�����ɻỰ��Կ
		WAPI_NOT_FALSE(CryptDeriveKey( _crypt_prov, ENCRYPT_ALGORITHM, _crypt_hash, KEYLENGTH, &_crypt_key),err);
	}
	void free_cypt_key()
	{
		if(_crypt_key) 
			CryptDestroyKey(_crypt_key); 
		if(_crypt_hash) 
			CryptDestroyHash(_crypt_hash); 

	}

public:
	encypt_win()
	{
		sys_err err(L"init encypt_win error");
		//���»��һ��CSP���
		if(!CryptAcquireContext( &_crypt_prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		{
			WAPI_NOT_FALSE(CryptAcquireContext( &_crypt_prov, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET),err);//������Կ����

		}

	}

	~encypt_win()
	{

		if(_crypt_prov) 
			CryptReleaseContext(_crypt_prov, 0);

	}
	std::wstring encode( std::wstring src ,const char *  passwd    )
	{ 
		sys_err err(L"encypt_win encode");
		DWORD dwBlockLen; 
		DWORD dwBufferLen; 
		DWORD dwCount=src.length() * 2;

		build_cypt_key(passwd);


		dwBlockLen =  src.length() * 2   - ( (src.length() * 2) % ENCRYPT_BLOCK_SIZE); 
		if(ENCRYPT_BLOCK_SIZE > 1) 
			dwBufferLen = dwBlockLen + ENCRYPT_BLOCK_SIZE; 
		else 
			dwBufferLen = dwBlockLen; 


		boost::shared_array<byte> buf(new byte[dwBufferLen]);
		memset(buf.get(),0,dwBufferLen);
		memcpy(buf.get(),src.c_str(), src.length()*2 );

		WAPI_NOT_FALSE(CryptEncrypt(
			_crypt_key,			//��Կ
			0,				//�������ͬʱ����ɢ�кͼ��ܣ����ﴫ��һ��ɢ�ж���
			TRUE,	//��������һ�������ܵĿ飬����TRUE.���������. //��FALSE����ͨ���ж��Ƿ��ļ�β�������Ƿ�Ϊ //���һ�顣
			0,				//����
			buf.get(),		//���뱻�������ݣ�������ܺ������
			&dwCount,		//���뱻��������ʵ�ʳ��ȣ�������ܺ����ݳ���
			dwBufferLen),err);	//pbBuffer�Ĵ�С��

		free_cypt_key();

		
		DWORD dwBase64Len = 0;
		CryptBinaryToString(buf.get(), dwCount, CRYPT_STRING_BASE64, NULL, &dwBase64Len);
		dwBase64Len +=1;
		boost::shared_array<wchar_t> wbuf(new wchar_t[dwBase64Len]);
		memset(wbuf.get(),0,dwBase64Len*2);
		CryptBinaryToString(buf.get(), dwCount, CRYPT_STRING_BASE64, wbuf.get(), &dwBase64Len);
		//remove ODOA
		void * beg = wbuf.get()  + dwBase64Len -2 ;
		memset(beg ,0,4);
		return wstring(wbuf.get());



	} 

	std::wstring  decode( std::wstring src ,const char *  passwd    )
	{ 
		//DWORD dwBlockLen; 
		DWORD dwBufferLen; 

		
		DWORD dwCount= 0;
		CryptStringToBinary(src.c_str(),src.length(),CRYPT_STRING_BASE64,NULL,&dwCount,NULL,NULL);
		boost::shared_array<byte> src_byte(new byte[dwCount]);
		memset(src_byte.get(),0,dwCount);

		CryptStringToBinary(src.c_str(),src.length(),CRYPT_STRING_BASE64,src_byte.get(),&dwCount,NULL,NULL);
		build_cypt_key(passwd);


		/*
		dwBlockLen =  dwCount  - ( dwCount % ENCRYPT_BLOCK_SIZE); 
		if(ENCRYPT_BLOCK_SIZE > 1) 
			dwBufferLen = dwBlockLen + ENCRYPT_BLOCK_SIZE; 
		else 
			dwBufferLen = dwBlockLen; 

		*/
		dwBufferLen= dwCount;

		boost::shared_array<byte> buf(new byte[dwBufferLen]);
		memset(buf.get(),0,dwBufferLen);
		memcpy(buf.get(),src_byte.get(), dwCount );

		CryptDecrypt( _crypt_key, 0, TRUE, 0, buf.get(), &dwBufferLen);	
		free_cypt_key();
		int c_len =  dwBufferLen/2 +1  ;
		boost::shared_array<wchar_t> c(new wchar_t[c_len]);
		memset(c.get(),0,c_len*2);
		memcpy(c.get(),buf.get(),dwBufferLen);

		return wstring(c.get());



	} 
};


class encrypt_utls_impl
{
public:
	encypt_win  win_impl;
	encrypt_utls_impl(xlogger_holder* logger)
	{

	}


	std::wstring encode( const std::wstring& str,xlogger_holder* logger )
	{
		return win_impl.encode(str,CY_CRYPT_KEY);
	}
	std::wstring decode( const std::wstring& str ,xlogger_holder* logger)
	{

		return win_impl.decode(str,CY_CRYPT_KEY);

	}
};
encrypt_utls::encrypt_utls(xlogger_holder* logger)
{
	_impl = new encrypt_utls_impl(logger);
}
encrypt_utls::~encrypt_utls()
{
	delete _impl;
}

std::wstring encrypt_utls::encode( const std::wstring& str, xlogger_holder* logger )
{
	return _impl->encode(str,logger);
}
std::wstring encrypt_utls::decode( const std::wstring& str, xlogger_holder* logger )
{
	return _impl->decode(str,logger);
}
