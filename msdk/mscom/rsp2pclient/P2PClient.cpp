#include "stdafx.h"
#include "P2PClient.h"
#include "P2PManager.h"
#include "UrlToRid.h"
#include "P2PDownloader.h"


CMsP2PClient::CMsP2PClient()
	: m_manager(NULL)
{
	m_manager = CMsP2PManager::GetManager();
}

CMsP2PClient::~CMsP2PClient()
{
	CMsP2PManager::DestroyManager();
}

STDMETHODIMP CMsP2PClient::Init(void*)
{
	return m_manager->Init();
}

STDMETHODIMP CMsP2PClient::Uninit()
{
	return m_manager->Uninit();
}

STDMETHODIMP CMsP2PClient::init_class(IUnknown *prot, IUnknown *punkOuter)
{
	//RASSERT(!punkOuter, E_INVALIDARG);
	//RASSERT(prot, E_INVALIDARG);

	//return init_class_outer(prot, this, CLSID_MSP2PClient);
	return S_OK;
}

STDMETHODIMP CMsP2PClient::StartWithURL(const P2PString url, const P2PString strSaveFilePathName, unsigned char *urlhash)
{
	// get urlhash
	assert(urlhash);

	int type = Type_UrlHash;
	P2PHash_t temphash = {0};
	if(url != NULL)
	{
		if(*(P2PHash_t*)urlhash == *(P2PHash_t*)NullUrlHash)
		{
			CSHA sha;
			sha.Hash(url, urlhash);
			type = Type_UrlHash;
		}
		else if(*(P2PHash_t*)urlhash == *(P2PHash_t*)HttpUrlHash)
		{
			CSHA sha;
			sha.Hash(url, urlhash);
			type = Type_Http;
		}
		else 
		{
			CSHA sha;
			sha.Hash(url, temphash.hash);

			if(temphash != *(P2PHash_t*)urlhash && IsValidHash(*(P2PHash_t*)urlhash))
			{
				type = Type_Rid;
			}
		}
	}
	else
	{
		// rid ? 
		if(IsValidHash(*(P2PHash_t*)urlhash)) type = Type_Rid;
		else return E_FAIL;
	}

	// add into manager
	if(S_OK == m_manager->AddDownloader(urlhash, type, url, strSaveFilePathName))
	{
		// 'strSaveFilePathName' can not be 'NULL'
		if(NULL != strSaveFilePathName && wcslen(strSaveFilePathName) > 0)
		{
			m_manager->SetState(urlhash, Opt_DownFileSavePath, (void*)strSaveFilePathName, wcslen(strSaveFilePathName));
		}

		m_manager->SetState(urlhash, Opt_DownRemoveFile,(void*)FALSE, sizeof(FALSE));
		return m_manager->SetState(urlhash, Opt_DownState, (void*)State_Downloading, sizeof(State_Downloading));
	}

	return E_FAIL;
}

STDMETHODIMP CMsP2PClient::Stop(const unsigned char *urlhash, BOOL remove_files)
{
	m_manager->SetState(urlhash, Opt_DownRemoveFile,  reinterpret_cast<void* &>(remove_files), sizeof(BOOL));
	if(m_manager->SetState(urlhash, Opt_DownState, (void*)State_Stop, sizeof(int)) == S_OK)
	{
		m_manager->DelDownloader(urlhash);
	}

	return S_OK;
}

STDMETHODIMP CMsP2PClient::Pause(const unsigned char *urlhash)
{
	return m_manager->SetState(urlhash, Opt_DownState, (void*)State_Pause, sizeof(int));
}

STDMETHODIMP CMsP2PClient::Resume(const unsigned char *urlhash)
{
	return m_manager->SetState(urlhash, Opt_DownState, (void*)State_Resume, sizeof(int));
}

STDMETHODIMP CMsP2PClient::GetStat(const unsigned char *urlhash, StateMessageID statid, void *data, int nlen)
{
	return m_manager->GetState(urlhash, statid, data, nlen);
}

STDMETHODIMP CMsP2PClient::SetStat(const unsigned char* urlhash, StateMessageID statid, void* data, int nlen)
{
	return m_manager->SetState(urlhash, (DownloadOptionID)statid, data, nlen);
}

STDMETHODIMP CMsP2PClient::SetOption(P2POption option, void *data, int nlen)
{
	return m_manager->SetOpt(option, data, nlen);
}

STDMETHODIMP CMsP2PClient::GetOption(P2POption option, void *data, int nlen)
{
	return m_manager->GetOpt(option, data, nlen);
}

STDMETHODIMP CMsP2PClient::SetNotify(IMSP2PClientNotify *data)
{
	return m_manager->SetNotify(data);
}
