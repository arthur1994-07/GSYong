#include "pch.h"

#include "oAuth/oauth.h"
#include "../../=cURL/include/curl/curl.h"

#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Json/json_spirit.h"

#include "../../SigmaCore/DebugInclude.h"

#include <openssl/pem.h>

namespace TexasHoldem
{
	/*
	std::string get_param_string( std::list< std::string >& params, BOOL separate )
	{
		std::list< std::string >::iterator iter;
		std::string str;

		params.sort();

		for( iter = params.begin( ); iter != params.end( ); iter++ )
		{
			str.append( *iter );
			if ( separate )
				str.append( "&" );
		}

		if ( separate )
			str.erase( str.size() - 1, 1 );

		return str;
	}

	int RequestLogin()
	{
		std::string strPrivateKey = 
			"-----BEGIN RSA PRIVATE KEY-----\n"
			"MIICXAIBAAKBgQDBFI5gNcWbNGsL6mGQEDmYcZ0LubVySZtqtBLwXmsDOsKl9uSG\n"
			"DWBMOPCbjgyhjLaCgqEfNN3KGiWeAz2Gb+AFxHHwxOZo9wrdkuLB1vwqG2fOUDZo\n"
			"7aMuYmEFVp8T4pdzhOx/n1oWXWGKHsyE3UvbNgOcWLv0rX1kKQ7TVBitMQIDAQAB\n"
			"AoGABovZ3+0yy/ZVAsxpjmbebQq9Fl6vhjZ0rIN6eOfanPtrChF64gimES+FCOEi\n"
			"EIAi1/3x1IsIUgfc6qSVe29jmv9RKtXFkJuClsPgU0m/HXXMIw6nMHsaNs4rv4qq\n"
			"NnFIMQmWVQuwSKhV2SvtMXzLWTuq/xyF/nVdk4qvVXWXVCECQQDzJ82Nl41q69WY\n"
			"oQLfNszTVhaKu/2sjPoeFPRpJlRMix4la8NACboNqL9e9geiGWd2rR+ak/eFwK5V\n"
			"gGQ62GA1AkEAy0eV28i1LFuZyCEfRsJ11OdztKD/1+8K+gmByY3R2Qc6NWh37yzV\n"
			"50gsA4pJYzHB+WDEOrAOR8MOvVW7uRLwjQJAKmV/RrftFX6HYLR6Trc8YYD6Lzh2\n"
			"E7cFCWe38fmX6zy6heXTRoArq6QHsiQN0qrBSoUHJKm2F+EvxIK9dzExHQJBALmG\n"
			"cwItUxTR4S9i822cKviN6O8RBY9pLqZ3KMnwMnaV/w0hdFyImgzEtM7iVgo/Ll7Q\n"
			"byd06QObxYcNiQ/ndj0CQGFr5DLq/F6HyYLHkLrmYcTtR2z3OS24vEqna5aRUcfh\n"
			"qKHh8HgVwqsrpiE/RyWykghvsrl6MAsEiEQhVDWM5DI=\n"
			"-----END RSA PRIVATE KEY-----\n";

		std::string strPublicKey = 
			"-----BEGIN PUBLIC KEY-----\n"
			"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDBFI5gNcWbNGsL6mGQEDmYcZ0L\n"
			"ubVySZtqtBLwXmsDOsKl9uSGDWBMOPCbjgyhjLaCgqEfNN3KGiWeAz2Gb+AFxHHw\n"
			"xOZo9wrdkuLB1vwqG2fOUDZo7aMuYmEFVp8T4pdzhOx/n1oWXWGKHsyE3UvbNgOc\n"
			"WLv0rX1kKQ7TVBitMQIDAQAB\n"
			"-----END PUBLIC KEY-----\n";

		std::string strPrivateKeyutf8 = sc::string::ansi_to_utf8( strPrivateKey );
		std::string strPublicKeyutf8 = sc::string::ansi_to_utf8( strPublicKey );

		RSA* rsaPublic = NULL;
		RSA* rsaPrivate = NULL;

		{
			BIO* bufio;
			const char* p = strPublicKeyutf8.c_str();
			int byteCount = strPublicKeyutf8.size();

			bufio = BIO_new_mem_buf( ( void* )p, byteCount );
			rsaPublic = PEM_read_bio_RSA_PUBKEY( bufio, 0, 0, 0 );
		}

		{
			BIO *bufio;
			const char *p = strPrivateKeyutf8.c_str();
			int byteCount = strPrivateKeyutf8.size();
			bufio = BIO_new_mem_buf( ( void* )p, byteCount );
			PEM_read_bio_RSAPrivateKey( bufio, &rsaPrivate, 0, NULL );
		}

		std::list< std::string > params;
		json_spirit::Object oReqData;

		oReqData.push_back( json_spirit::Pair( "SgNum", 1 ) );
		oReqData.push_back( json_spirit::Pair( "ChaNum", static_cast< int >( 40895 ) ) );
		oReqData.push_back( json_spirit::Pair( "AuthKey", "1D7EB02D-F72F-49D3-A150-00D9F072883D" ) );

		std::string strReqData, strReqDataBase64Encoded;
		strReqData = write_formatted( oReqData );
		strReqDataBase64Encoded = oauth_encode_base64( strReqData.size(), ( const unsigned char* )strReqData.c_str() );

		for ( int loop = 0; loop < 10; loop++ )
		{
			unsigned char szTo[256] = { 0, };
			std::string strFrom = strReqData;
			int nEncrypted = 0;
			nEncrypted = RSA_public_encrypt( strFrom.size(), ( const unsigned char* )strFrom.c_str(), szTo, rsaPublic, RSA_PKCS1_PADDING );

			std::string strTo( reinterpret_cast< char* >( szTo ) );
			if ( nEncrypted == static_cast< int >( strTo.size() ) )
			{
				std::string strToBase64Encoded = oauth_encode_base64( strTo.size(), ( const unsigned char* )strTo.c_str() );
				std::string stCertificate;
				json_spirit::Object oData;
				oData.push_back( json_spirit::Pair( "ReqData", strReqDataBase64Encoded.c_str() ) );
				oData.push_back( json_spirit::Pair( "Signature", strToBase64Encoded.c_str() ) );

				stCertificate = write_formatted( oData );
				std::string stCertificateBase64Encoded = oauth_encode_base64( stCertificate.size(), ( const unsigned char* )stCertificate.c_str() );
				std::string stCertificateBase64EncodedUrlEncoded = sc::string::UrlEncodeString( stCertificateBase64Encoded.c_str() );
				params.push_back( "ReqData=" + stCertificateBase64EncodedUrlEncoded );
				break;
			}
		}

		std::string targetUrl = "http://casinox_admin.mincoms.com/RanMiniHoldem/index.htm";
		curl_global_init( CURL_GLOBAL_ALL ) ;

		CURL* ctx = curl_easy_init() ;
		if ( NULL == ctx )
		{
			return -1;
		}

		// Target URL: 
		curl_easy_setopt( ctx , CURLOPT_URL,  targetUrl.c_str() ) ;

		std::string query = get_param_string( params, TRUE );
		std::string queryutf8 = sc::string::ansi_to_utf8( query );
		curl_easy_setopt( ctx, CURLOPT_POST, TRUE );
		curl_easy_setopt( ctx, CURLOPT_POSTFIELDS, query.c_str( ) );

		// no progress bar:
		curl_easy_setopt( ctx , CURLOPT_NOPROGRESS , 1 ) ;

		// 응답데이터를 표준출력으로 보낸다. 
		curl_easy_setopt( ctx , CURLOPT_WRITEDATA , stdout ) ;

		CURLcode rc = curl_easy_perform( ctx ) ;

		// "curl_easy_strerror()" available in curl v7.12.x and later
		if ( CURLE_OK != rc )
		{
			std::cerr << '\t' << "Error from cURL: " << curl_easy_strerror( rc ) << std::endl ;
		}

		// cleanup
		curl_easy_cleanup( ctx );
		curl_global_cleanup();
		return 0;
	}
	*/
	std::string GetPostArgument( int ServerGroup, DWORD dwChaDbNum, const std::string& strGUID )
	{
		std::string strPublicKey = 
			"-----BEGIN PUBLIC KEY-----\n"
			"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDBFI5gNcWbNGsL6mGQEDmYcZ0L\n"
			"ubVySZtqtBLwXmsDOsKl9uSGDWBMOPCbjgyhjLaCgqEfNN3KGiWeAz2Gb+AFxHHw\n"
			"xOZo9wrdkuLB1vwqG2fOUDZo7aMuYmEFVp8T4pdzhOx/n1oWXWGKHsyE3UvbNgOc\n"
			"WLv0rX1kKQ7TVBitMQIDAQAB\n"
			"-----END PUBLIC KEY-----\n";

		std::string strPublicKeyutf8 = sc::string::ansi_to_utf8( strPublicKey );

		RSA* rsaPublic = NULL;
		RSA* rsaPrivate = NULL;

		// load public key
		BIO* bufio;
		const char* p = strPublicKeyutf8.c_str();
		int byteCount = strPublicKeyutf8.size();

		bufio = BIO_new_mem_buf( ( void* )p, byteCount );
		rsaPublic = PEM_read_bio_RSA_PUBKEY( bufio, 0, 0, 0 );

		std::string param;
		json_spirit::Object oReqData;

		oReqData.push_back( json_spirit::Pair( "SgNum", ServerGroup ) );
		oReqData.push_back( json_spirit::Pair( "ChaNum", static_cast< int >( dwChaDbNum ) ) );
		oReqData.push_back( json_spirit::Pair( "AuthKey", strGUID.c_str() ) );

		std::string strReqData, strReqDataBase64Encoded;
		strReqData = write_formatted( oReqData );
		strReqDataBase64Encoded = oauth_encode_base64( strReqData.size(), ( const unsigned char* )strReqData.c_str() );

		for ( int loop = 0; loop < 10; loop++ )
		{
			unsigned char szTo[256] = { 0, };
			std::string strFrom = strReqData;
			int nEncrypted = 0;
			nEncrypted = RSA_public_encrypt( strFrom.size(), ( const unsigned char* )strFrom.c_str(), szTo, rsaPublic, RSA_PKCS1_PADDING );

			std::string strTo( reinterpret_cast< char* >( szTo ) );
			if ( nEncrypted == static_cast< int >( strTo.size() ) )
			{
				std::string strToBase64Encoded = oauth_encode_base64( strTo.size(), ( const unsigned char* )strTo.c_str() );
				std::string stCertificate;
				json_spirit::Object oData;
				oData.push_back( json_spirit::Pair( "ReqData", strReqDataBase64Encoded.c_str() ) );
				oData.push_back( json_spirit::Pair( "Signature", strToBase64Encoded.c_str() ) );

				stCertificate = write_formatted( oData );
				std::string stCertificateBase64Encoded = oauth_encode_base64( stCertificate.size(), ( const unsigned char* )stCertificate.c_str() );
				std::string stCertificateBase64EncodedUrlEncoded = sc::string::UrlEncodeString( stCertificateBase64Encoded.c_str() );
				param = "ReqData=" + stCertificateBase64EncodedUrlEncoded;
				break;
			}
		}
		return param;
	}
}