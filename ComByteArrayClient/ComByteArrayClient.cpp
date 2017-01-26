// ComByteArrayClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void ManipulateData(char* pBuff, int cbBuff);
void ManipulateData(CComSafeArray<BYTE> *pSafeArray);

int main()
{
	CoInitialize(NULL);

	IImageProvider *pImgProvider;
	CoCreateInstance(__uuidof(ImageProvider), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IImageProvider), (LPVOID*)&pImgProvider);

	int imageId = 0;

	// with SafeArray
	{
		SAFEARRAY *pSafeArray;
		auto hr = pImgProvider->GetImage(0, &pSafeArray);
		assert(hr == S_OK);
		CComSafeArray<BYTE> safeArray;
		safeArray.Attach(pSafeArray);
		ManipulateData(&safeArray);
		// CComSafeArray will free the memory allocated by pSafeArray
	}

	// with CoTaskMemAlloc
	{
		char *pData;
		int cbData;
		auto hr = pImgProvider->GetImageAsUnmanaged(0, (long*)&cbData, (long*)&pData);
		assert(hr == S_OK);

		ManipulateData(pData, cbData);
		CoTaskMemFree(pData);
	}

	// with caller allocate
	{
		int cbData;
		auto hr = pImgProvider->GetImageAsUnmanagedPreallocated(imageId, (long*)&cbData, (long)nullptr);
		assert(hr == E_INVALIDARG);

		char *pData = new char[cbData];
		hr = pImgProvider->GetImageAsUnmanagedPreallocated(imageId, (long*)&cbData, (long)pData);
		assert(hr == S_OK);

		ManipulateData(pData, cbData);

		delete[] pData;
	}

	system("pause");

	CoUninitialize();
    return 0;
}

void ManipulateData(char* pBuff, int cbBuff)
{
	char hash = 0;
	for (int i = 0; i < cbBuff; i++)
	{
		hash ^= pBuff[i];
	}

	std::cout << "Hash is " << +hash << std::endl;
}

void ManipulateData(CComSafeArray<BYTE> *pSafeArray)
{
	BYTE hash = 0;
	for (ULONG i = 0; i < pSafeArray->GetCount(); i++)
	{
		hash ^= pSafeArray->GetAt(i);
	}

	std::cout << "Hash is " << +hash << std::endl;
}