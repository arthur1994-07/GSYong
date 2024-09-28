#include "CallbackFunctions.h"

//
// write_callback function could be called several times for a Request!!
// Therefore, result string MUST be appended to userp but not just copy.
//
int	cntTmp = 0;
char tmpBuffer[65535];

size_t write_callback( void *ptr, size_t size, size_t nmemb, void *userp )
{ 
	/*
	This function gets called by libcurl as soon as there is data received that needs to be saved.
	The size of the data pointed to by ptr is size multiplied with nmemb,
	it will not be zero terminated.
	*/

	size_t nLength = min( (size * nmemb), MAX_BUFFER_LENGTH );

	//VECBUFFER *pVec = (VECBUFFER *) userp;

	char *p = (char *) userp;

	//char cBuffer[MAX_BUFFER_LENGTH];
	//memset(cBuffer, 0, sizeof(cBuffer));
	
	memcpy (p + cntTmp, (char *)ptr, nLength);


	char tmp[2048];
	memset(tmp, 0, 2048);

	strcpy(tmp, p);



	cntTmp += nLength;
	
	
	//char *begin = (char *) ptr;  //cBuffer;
	//char *end = begin + nLength; //cBuffer + nLength;
	//pVec->insert(pVec->end(), begin, end);


	//VECBUFFER_ITER iv;

	

	//pVec->insert(pVec->end(), nLength, btBuffer);

	//string strTmp;
	//strTmp = m_pInstance->utf8_to_ansi( (char *)btBuffer, size * nmemb );





	//string *str = (string *) userp;
	//str->append( (char *) ptr, size*nmemb );

	//BYTE btBuffer[8192];
	//memcpy (btBuffer, (char *)ptr, size * nmemb);

	//string strTmp;
	//strTmp = m_pInstance->utf8_to_ansi( (char *)btBuffer, size * nmemb );
	
	//str->append( strTmp );
	


	return nLength;//cntTmp;
}
