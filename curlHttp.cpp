#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <assert.h>
#include "curl/curl.h"
//#include <openssl/hmac.h>
#include <time.h>
#include "md5.cpp"
#include "HMAC_SHA1.cpp"
#include "base64.cpp"
#include "CURLCode.cpp"
#include <iostream>
//#include "writelog.cpp"
using namespace std;

#pragma comment(lib, "ws2_32.lib")  
#pragma comment(lib, "wldap32.lib")  
#pragma comment(lib, "libcurl.lib")  

// reply of the requery  
size_t req_reply(void *ptr, size_t size, size_t nmemb, void *stream)
{
	//cout << "----->reply" << endl;
	string *str = (string*)stream;
	//cout << *str << endl;
	(*str).append((char*)ptr, size*nmemb);
	return size * nmemb;
}

// http GET  
CURLcode curl_get_req(const std::string &url, std::string &response)
{
	// init curl  
	CURL *curl = curl_easy_init();
	// res code  
	CURLcode res;
	if (curl)
	{
		// set params  
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3); // set transport and time out time  
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
		// start req  
		res = curl_easy_perform(curl);
	}
	// release curl  
	curl_easy_cleanup(curl);
	return res;
}

// http POST  
CURLcode curl_post_req(const string &url, const string &postParams, string &response)
{
	// init curl  
	CURL *curl = curl_easy_init();
	// res code  
	CURLcode res;
	if (curl)
	{
		// set params  
		curl_easy_setopt(curl, CURLOPT_POST, 1); // post req  
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url  
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postParams.c_str()); // params  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_HEADER, false);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
		// start req  
		res = curl_easy_perform(curl);
	}
	// release curl  
	curl_easy_cleanup(curl);
	return res;
}

string generateKey(int n)
{
	int i, randno;
	string key = "";
	char stardstring[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

	srand((unsigned)time(NULL));//srand()函数产生一个以当前时间开始的随机种子.应该放在for等循环语句前面 不然要很长时间等待

	for(i = 0;i<n;i++)
	{ 
		randno = rand() % 62;
		key += stardstring[randno];
	}
	return key; 
}

unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}

std::string UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

string get_secret(const string key) {
	string str = "#nfe$123#";

	return md5(str) + md5(key) + md5(str+key);
}

/*
* 生成签名
*/
string get_signature(const string str , const string key)
{
	//unsigned char digest[EVP_MAX_MD_SIZE] = { '\0' };
	unsigned int digest_len = 0;
	//string digest = "";
	BYTE digest[20];

	// Using sha1 hash engine here.  
	// You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc  
	/*HMAC(EVP_sha1(), key.data(), key.size(), (unsigned char*)str.data(), str.size(), digest.c_str(), &digest_len);
	return base64_encode(reinterpret_cast<const unsigned char*>(digest.c_str()), digest_len);*/
	CHMAC_SHA1 *obj = new CHMAC_SHA1();
	obj->HMAC_SHA1((BYTE*)str.c_str() , str.size() , (BYTE*)key.c_str() , key.size() , digest);
	//cout << digest.c_str() << endl;
	delete obj;
	//cout << sizeof(digest) / sizeof(BYTE) << endl;

	return base64_encode((unsigned char*)digest , sizeof(digest) / sizeof(BYTE));
}

bool getResponse(const string eqid, const string els_type , const string s_status , const string s_feedback , const string or_data , const string have_time , string &error)
{
	string postUrlStr = "https://api.nfe.com/Info";
	string key = generateKey(32);
	string secret = get_secret(key);

	string postParams = UrlEncode("els_type") + "=" + UrlEncode(els_type) + "&" + UrlEncode("eqid") + "=" + UrlEncode(eqid) + "&" + UrlEncode("have_time")
		+ "=" + UrlEncode(have_time) + "&" + UrlEncode("key") + "=" + UrlEncode(key) + "&" + UrlEncode("or_data") + "=" + UrlEncode(or_data)
		+ "&" + UrlEncode("s_feedback") + "=" + UrlEncode(s_feedback) + "&" + UrlEncode("s_status") + "=" + UrlEncode(s_status);

	/*log_file();
	write_log("eqid=%s\n", eqid.c_str());
	write_log("postParams=%s\n", postParams.c_str());
	close_file();*/


	string str = "https://api.nfe.com";
	str = UrlEncode(str);
	str += "&" + postParams;

	string sig = get_signature(str, secret);

	CURLCode* curlCode = new CURLCode();
	postParams += "&sig=" + curlCode->Encode(sig.c_str());
	string postResponseStr;
	// global init  
	curl_global_init(CURL_GLOBAL_ALL);
	CURLcode res = curl_post_req(postUrlStr, postParams, postResponseStr);
	bool flag;
	if (res != CURLE_OK)
	{
		//请求出错
		//cerr << "curl_easy_perform() failed: " + string(curl_easy_strerror(res)) << endl;
		flag = false;
		error = "curl_easy_perform() failed: " + string(curl_easy_strerror(res));
	}
	else
	{
		//请求成功
		//cout << postResponseStr << endl;
		if (postResponseStr.compare("ok") == 0)
		{
			flag = true;
		}
		else
		{
			flag = false;
			error = postResponseStr;
		}
		
	}


	// global release  
	curl_global_cleanup();
	return flag;
}

/**
* g++ curlHttp.cpp -l curl
*/
/*int main()
{
	
	string eqid = "C50755D1";
	string s_status = "1";
	string els_type = "1";
	string s_feedback = "ssdddss";
	string or_data = "fsdaf";
	string have_time = "1520416063";

	string error = "";
	if (getResponse(eqid, els_type, s_status, s_feedback, or_data, have_time, error)) {
		printf("ok\n");
	}
	else {
		printf("%s\n", error.c_str());
	}

	return 0;
}*/