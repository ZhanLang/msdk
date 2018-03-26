// downloader.h

#include "pch.h"
#include "stdafx.h"
#include <boost/filesystem/fstream.hpp>
#include "libtorrent/extensions/metadata_transfer.hpp"
#include "libtorrent/create_torrent.hpp"

#include "downloader.h"
#include "Timer.h"
#include "MainThread.h"
#include "P2PClientTool.h"
#include "P2PManager.h"

using std::string;
using std::endl;

using namespace libtorrent;
using boost::filesystem::path;
typedef unsigned int uint32_t;
typedef sha1_hash RID;


CCriticalSetionObject g_downLock;
downloader::MainThread * main_thread = NULL;
boost::shared_ptr<session> ses;
std::map<RID, torrent_handle> handles;
std::map<torrent_handle, RID> rhandles;
std::map<RID, torrent_handle> downloaders;
LPFileLengthListener fl_handler = NULL;
LPDownloadInfoListener di_handler = NULL;
LPUploadInfoListener ui_handler = NULL;
LPFileDownloadCompleteListener fdc_handler = NULL;
int max_connections_per_torrent = 50;
std::string temp_file_ext(".rs");
std::string hash_dirp;

// alert_timer，每500ms处理一次抛出的alert
downloader::PeridoicTimer * alert_timer = NULL;
downloader::PeridoicTimer * downer_timer = NULL;
downloader::PeridoicTimer * uploader_timer = NULL;


// zcg+
std::string get_hex_string_from_rid(RID rid)
{
	char szrid[41] = {0};
	std::string srid = rid.to_string();
	to_hex(srid.c_str(), srid.size(), szrid);

	return std::string(szrid);
}


// 实际执行操作的函数，都是在主线程执行
void StartDownloadImp(const RID rid, const std::vector<string> trackers, const std::vector<std::string> urls, path const save_path);
void PauseDownloadImp(const RID rid);
void ResumeDownloadImp(const RID rid);
void StopDownloadImp(const RID rid, bool remove_files);

// 时钟回调函数
void OnAlertTimer();
void OnDownloadInfoTimer();
void OnUploadInfoTimer();

void SHARED_EXPORT StartUp()
{
	hash_dirp = CP2PClientTool::GetModuleDirectoryA();
	hash_dirp += P2P_META_DIRP;

	main_thread = new downloader::MainThread();

	ses.reset(new session(fingerprint("Star", LIBTORRENT_VERSION_MAJOR, LIBTORRENT_VERSION_MINOR, 0, 0)
		, session::start_default_features | session::add_default_plugins
		, alert::all_categories
//#ifdef NDEBUG
		& ~(alert::peer_notification
		+ alert::port_mapping_notification
		/*+ alert::storage_notification*/   // for 'file_rename_failed_alert'
		+ alert::tracker_notification
		+ alert::debug_notification
		+ alert::ip_block_notification
		+ alert::performance_warning
		+ alert::stats_notification)
#if defined TORRENT_VERBOSE_LOGGING || defined TORRENT_LOGGING || defined TORRENT_ERROR_LOGGING
		, CP2PClientTool::GetModuleDirectoryA()
#endif
//#endif
		));
	ses->add_extension(&libtorrent::create_metadata_plugin);

	session_settings session_s;
	session_s.user_agent = "Star/0.1";
	session_s.announce_to_all_trackers = false;
	session_s.announce_to_all_tiers = false;

	// TODO(herain)2011-2-13:This is dangaous settings. Watch out!
	session_s.tracker_completion_timeout = 10;
	session_s.tracker_receive_timeout = 10;
	session_s.stop_tracker_timeout = 1;
	session_s.connection_speed = 20;

#ifndef TORRENT_DISABLE_DHT
	session_s.use_dht_as_fallback = false;
#endif

	ses->set_alert_queue_size_limit(2000);
	ses->set_settings(session_s);

	pe_settings pe_s;
	pe_s.in_enc_policy = pe_settings::enabled;
	pe_s.out_enc_policy = pe_settings::enabled;
	pe_s.allowed_enc_level = pe_settings::rc4;
	pe_s.prefer_rc4 = true;
	//ses->set_pe_settings(pe_s);

	//ses->listen_on(std::make_pair(11935, 15000));

#ifndef TORRENT_DISABLE_DHT
	ses->add_dht_router(std::make_pair(
		std::string("router.bittorrent.com"), 6881));
	ses->add_dht_router(std::make_pair(
		std::string("router.utorrent.com"), 6881));
	ses->add_dht_router(std::make_pair(
		std::string("router.bitcomet.com"), 6881));

	ses->start_dht();
#endif

	ses->start_lsd();
	ses->start_upnp();
	ses->start_natpmp();

	alert_timer = new downloader::PeridoicTimer(main_thread->ios(), 500, OnAlertTimer);
}

void SHARED_EXPORT ClearUp()
{
	if(alert_timer != NULL)
	{
		delete alert_timer;
		alert_timer = NULL;
	}

	if(downer_timer != NULL)
	{
		delete downer_timer;
		downer_timer = NULL;
	}

	if(uploader_timer != NULL)
	{
		delete uploader_timer;
		uploader_timer = NULL;
	}

	if(main_thread != NULL)
	{
		delete main_thread;
		main_thread = NULL;
	}

	for(std::map<RID, torrent_handle>::iterator iter = handles.begin();
		iter != handles.end();)
	{
		StopDownloadImp(iter->first, false);
		iter = handles.begin();
	}

	CBaseLockHandler lh(g_downLock);
	handles.clear();
	rhandles.clear();
	ses.reset();
}

void SHARED_EXPORT StartDownloadWithRID(const unsigned char* strRID, int trkc, const char** trkv,
										const char* strSaveFilePath, size_t svlen)
{
	RID rid((const char *)strRID);
	std::vector<string> trackers;
	for (int i = 0; i < trkc; ++i)
	{
		trackers.push_back(string(trkv[i]));
	}

	string save_path(strSaveFilePath, svlen);

	if(main_thread != NULL)
	{
		std::vector<std::string> urls;
		main_thread->post(boost::bind(&StartDownloadImp, rid, trackers, urls, save_path));
	}
}

void SHARED_EXPORT StartDownloadWithRIDx(const unsigned char* strRID, const std::vector<std::string> & trackers, 
										 const std::vector<std::string> & urls, const std::string & save_path)
{
	RID rid((const char *)strRID);
	if(main_thread != NULL)
	{
		main_thread->post(boost::bind(&StartDownloadImp, rid, trackers, urls, save_path));
	}
}

void SHARED_EXPORT PauseDownload(const unsigned char* strRID)
{
	RID rid((const char *)strRID);
	if(main_thread != NULL)
	{
		main_thread->post(boost::bind(&PauseDownloadImp, rid));
	}
}

void SHARED_EXPORT ResumeDownload(const unsigned char* strRID)
{
	RID rid((const char *)strRID);
	if(main_thread != NULL)
	{
		main_thread->post(boost::bind(&ResumeDownloadImp, rid));
	}
}

void SHARED_EXPORT StopDownload(const unsigned char* strRID, BOOL remove_files)
{
	RID rid((const char *)strRID);
	if(main_thread != NULL)
	{
		main_thread->post(boost::bind(&StopDownloadImp, rid, remove_files));
	}
}

void SHARED_EXPORT SetOption(DownloaderOption option, void * data)
{
	switch ((int)option)
	{
	case Opt_MaxUpSpeedInBps:
		{
			unsigned int uMaxUpSpeedInBytes = *(unsigned int*)data;
			ses->set_upload_rate_limit(uMaxUpSpeedInBytes);
			break;
		}
	case Opt_MaxDownSpeedInBps:
		{
			unsigned int uMaxDownSpeedInBytes = *(unsigned int*)data;
			ses->set_download_rate_limit(uMaxDownSpeedInBytes);
			break;
		}
	case Opt_ListenPort:
		{
			unsigned int port = *(unsigned int*)data;
			ses->listen_on(std::make_pair(port, port));
			break;
		}
	case Opt_MaxConnectNum:
		{
			unsigned int uMaxConnectNum = *(unsigned int*)data;
			// The number of connections is set to a hard minimum of at least two connections per torrent, 
			// so if you set a too low connections limit, and open too many torrents, the limit will not be met. 
			// The number of uploads is at least one per torrent.
			ses->set_max_connections(uMaxConnectNum);
			break;
		}
	default:
		break;
	};
}

void SHARED_EXPORT GetOption(DownloaderOption option, void * data)
{
	switch ((int)option)
	{
	case Opt_MaxUpSpeedInBps:
		*(int*)data = ses->upload_rate_limit();
		break;

	case Opt_MaxDownSpeedInBps:
		*(int*)data = ses->download_rate_limit();
		break;

	case Opt_ListenPort:
		*(int*)data = ses->listen_port();
		break;

	case Opt_MaxConnectNum:
		*(int*)data = ses->max_connections();
		break;

	default:
		break;
	};
}

void SHARED_EXPORT SetFileLengthListener(LPFileLengthListener listener)
{
	fl_handler = listener;
}

void SHARED_EXPORT SetDownloadInfoListener(LPDownloadInfoListener listener, unsigned int interval_ms)
{
	downer_timer = new downloader::PeridoicTimer(main_thread->ios(), interval_ms, OnDownloadInfoTimer);
	di_handler = listener;
	if (downer_timer->interval() != interval_ms)
	{
		downer_timer->interval(interval_ms);
	}
}

void SHARED_EXPORT SetUploadInfoListener(LPUploadInfoListener listener)
{
	uploader_timer = new downloader::PeridoicTimer(main_thread->ios(), 1000, OnUploadInfoTimer);
	ui_handler = listener;
}

void SHARED_EXPORT SetFileDownloadCompleteListener(LPFileDownloadCompleteListener listener)
{
	fdc_handler = listener;
}


int save_file(boost::filesystem::path const& filename, std::vector<char>& v)
{
	using namespace libtorrent;

	file f;
	error_code ec;
	if (!f.open(filename, file::write_only, ec)) return -1;
	if (ec) return -1;
	file::iovec_t b = {&v[0], v.size()};
	size_type written = f.writev(0, &b, 1, ec);
	if (written != v.size()) return -3;
	if (ec) return -3;
	return 0;
}

bool restore_file_name(const torrent_handle & h, int index = 0)
{
	const torrent_info & ti = h.get_torrent_info();
	const file_entry   & fe = ti.file_at(index);
	if (fe.path.extension() == temp_file_ext)
	{
		h.rename_file(index, fe.path.parent_path()/fe.path.stem());
		return true;
	}

	return false;
}

void file_completion_handle(const RID & rid, unsigned int down_size)
{
	if(fdc_handler) fdc_handler((const unsigned char*)rid.to_string().c_str(), down_size);

	//#ifdef _DEBUG
	//	torrent_info ti = a.handle.get_torrent_info();
	//	std::string filename(ti.file_at(0).path.filename());
	//	extern void TestP2PInfo(const RID & rid, libtorrent::torrent_status & status, std::string filename);
	//	TestP2PInfo(a.handle.info_hash(), a.handle.status(), filename);
	//#endif

	std::map<RID, torrent_handle>::iterator iter = downloaders.find(rid);
	if(iter != downloaders.end())
	{
		downloaders.erase(iter);
	}

	assert(downloaders.end() == downloaders.find(rid));
}

struct my_alert_handler
{
	// alert::status_notification
	void operator()(metadata_received_alert const & a) const
	{
		CBaseLockHandler lh(g_downLock);
		if(!a.handle.is_valid())
		{
			return ;
		}

		// save metafile to local
		std::string meta = hash_dirp + 
			get_hex_string_from_rid(a.handle.info_hash()) + ".hash";

		create_torrent ct(a.handle.get_torrent_info());
		std::ofstream out(meta.c_str(), std::ios_base::binary);
		if(out)
		{
			bencode(std::ostream_iterator<char>(out), ct.generate());
			BOOL bRet = SetFileAttributesA(meta.c_str(), FILE_ATTRIBUTE_HIDDEN);
		}
		return;
	}

	// alert::progress_notification
	void operator()(file_completed_alert const& a) const
	{
		CBaseLockHandler lh(g_downLock);
		if(rhandles.end() == rhandles.find(a.handle))
		{
			MSP2PLOG(MSP2PLOG_ERROR, "my_alert_handler::file_completed_alert() invalidate handle ...");
			return ;
		}

		std::string rename = "ok";
		try
		{
			if(restore_file_name(a.handle, a.index)) rename = "no";
			else file_completion_handle(a.handle.info_hash(), 
					a.handle.status().all_time_download > 0 || a.handle.status().total_payload_download > 0 ? 
					a.handle.status().total_wanted : 0);

			MSP2PLOG(MSP2PLOG_TEST, "file_completed_alert(), rid = '%s', renamed: %s...", 
				get_hex_string_from_rid(a.handle.info_hash()).c_str(), rename.c_str());
		}
		catch(...)
		{
			MSP2PLOG(MSP2PLOG_ERROR, "my_alert_handler::file_completed_alert() exception ...");
		}
	}

	// alert::status_notification
	void operator()(torrent_finished_alert const& a) const
	{
		CBaseLockHandler lh(g_downLock);
		if(rhandles.end() == rhandles.find(a.handle))
		{
			MSP2PLOG(MSP2PLOG_ERROR, "my_alert_handler::torrent_finished_alert() invalidate handle ...");
			return ;
		}

		try
		{
			fs::path cfg_file_path(a.handle.save_path() / (get_hex_string_from_rid(a.handle.info_hash()) + ".cfg"));
			if (fs::exists(cfg_file_path))
			{
				fs::remove(cfg_file_path);
			}

			// 此处可能文件还需要重命名，判断是否进行完成回调
			// 此时如果文件名已经被重命名结束，则会造成完成消息回调冗余**
			// 比较麻烦，放到 file_completed_alert 中
			MSP2PLOG(MSP2PLOG_TEST, "torrent_finished_alert(), rid = '%s'...", 
				get_hex_string_from_rid(a.handle.info_hash()).c_str());
		}
		catch(...)
		{
			MSP2PLOG(MSP2PLOG_ERROR, "my_alert_handler::torrent_finished_alert() exception ...");
		}
	}

	// alert::status_notification
	void operator()(state_changed_alert const& a) const
	{
		CBaseLockHandler lh(g_downLock);
		if(rhandles.end() == rhandles.find(a.handle))
		{
			MSP2PLOG(MSP2PLOG_ERROR, "my_alert_handler::state_changed_alert() invalidate handle ...");
			return ;
		}

		try
		{
			if (a.state == torrent_status::downloading)
			{
				if (fl_handler)
				{
					torrent_status ts = a.handle.status();
					fl_handler((const unsigned char*)a.handle.info_hash().to_string().c_str(), 
						ts.total_wanted);
				}

				torrent_info ti = a.handle.get_torrent_info();
				std::vector<libtorrent::size_type> progress;
				a.handle.file_progress(progress, torrent_handle::piece_granularity);
				for(int i = 0; i < ti.num_files(); ++i)
				{
					file_entry fe = ti.file_at(i);
					std::string file_ext = fe.path.extension();
					if (fe.size != progress[i] && file_ext != temp_file_ext)
					{
						a.handle.rename_file(i, fe.path.file_string() + temp_file_ext);
					}
				}
			} 
		}
		catch(...)
		{
			MSP2PLOG(MSP2PLOG_ERROR, "my_alert_handler::state_changed_alert() exception ...");
		}
	}

	// alert::storage_notification
	void operator()(save_resume_data_alert const & a) const 
	{
		CBaseLockHandler lh(g_downLock);
		if(rhandles.end() == rhandles.find(a.handle))
		{
			MSP2PLOG(MSP2PLOG_ERROR, "my_alert_handler::state_changed_alert() invalidate handle ...");
			return ;
		}

		try
		{
			torrent_handle h = a.handle;
			if (a.resume_data)
			{
				std::vector<char> out;
				bencode(std::back_inserter(out), *a.resume_data);

				save_file(h.save_path() / (get_hex_string_from_rid(h.info_hash()) + ".cfg"), out);
			}
		}
		catch(...)
		{
			MSP2PLOG(MSP2PLOG_ERROR, "my_alert_handler::save_resume_data_alert() exception ...");
		}
	}

	void operator()(save_resume_data_failed_alert const & a) const 
	{
	}

	void operator()(file_renamed_alert const & a) const
	{
		try
		{
			if (a.handle.is_finished())
			{
				file_completion_handle(a.handle.info_hash(), 
					a.handle.status().all_time_download > 0 || a.handle.status().total_payload_download > 0 ? 
					a.handle.status().total_wanted : 0);
			}

			MSP2PLOG(MSP2PLOG_TEST, "file_renamed_alert(), rid = '%s', err = %d, msg = %s", 
				get_hex_string_from_rid(a.handle.info_hash()).c_str(), 
				a.index, a.name.c_str());
		}
		catch(...)
		{
			MSP2PLOG(MSP2PLOG_ERROR, "my_alert_handler::file_renamed_alert() exception ...");
		}
	}

	void operator()(file_rename_failed_alert const & a) const
	{
		try
		{
			if (a.handle.is_finished())
			{
				file_completion_handle(a.handle.info_hash(), 
					a.handle.status().all_time_download > 0 || a.handle.status().total_payload_download > 0 ? 
					a.handle.status().total_wanted : 0);
			}

			MSP2PLOG(MSP2PLOG_TEST, "file_rename_failed_alert(), rid = '%s', err = %d, msg = %s", 
				get_hex_string_from_rid(a.handle.info_hash()).c_str(), 
				a.index, a.message().c_str());
		}
		catch(...)
		{
			MSP2PLOG(MSP2PLOG_ERROR, "my_alert_handler::file_rename_failed_alert() exception ...");
		}
	}

	void handle_alert(const std::auto_ptr<alert>& alert_)
	{
		#define handler_dispatch(T)  \
			{ \
				if(T * p = alert_cast<T>(alert_.get())) \
				{ \
					(*this)(*p); \
					return ; \
				} \
			}

		handler_dispatch(file_completed_alert);
		handler_dispatch(torrent_finished_alert);
		handler_dispatch(state_changed_alert);
		handler_dispatch(metadata_received_alert);
		handler_dispatch(file_renamed_alert);
		handler_dispatch(file_rename_failed_alert);
		handler_dispatch(save_resume_data_alert);
		handler_dispatch(save_resume_data_failed_alert);
	}
};

void StartDownloadImp(const RID rid, const std::vector<string> trackers, const std::vector<std::string> urls, path const save_path)
{
	assert(!trackers.empty());

	CBaseLockHandler lh(g_downLock);
	add_torrent_params p;

	// zcg+
	error_code ec;
	std::string metafile = hash_dirp + get_hex_string_from_rid(rid) + ".hash";
	if(CP2PClientTool::FindFileA(metafile.c_str()))
	{
		p.ti = new torrent_info(metafile.c_str(), ec);
	}
	else
	{
		p.info_hash = rid;
	}

	p.save_path = save_path;
	p.duplicate_is_error = true;			// zcg+
	p.auto_managed = false;
	//p.tracker_url = trackers[0].c_str();	// zcg+, tracker[0] = url

	lazy_entry resume_data;
	std::string filename = (save_path / (get_hex_string_from_rid(rid) + ".cfg")).string();
	std::vector<char> buf;
	if (load_file(filename.c_str(), buf) == 0)
		p.resume_data = &buf;

	torrent_handle h = ses->add_torrent(p, ec);

	if (h.is_valid())
	{
		handles.insert(std::make_pair(rid, h));
		rhandles.insert(std::make_pair(h, rid));
		downloaders.insert(std::make_pair(rid, h));

		for (int i = 0; i < (int)trackers.size(); ++i)
		{
			announce_entry ae(trackers[i]);
			h.add_tracker(ae);
		}

		h.set_max_connections(max_connections_per_torrent);
		h.set_max_uploads(-1);
#ifndef TORRENT_DISABLE_RESOLVE_COUNTRIES
		h.resolve_countries(true);
#endif
		for(std::vector<std::string>::const_iterator it = urls.begin(); 
			it != urls.end(); it++)
		{
			h.add_url_seed(it->c_str());
		}

		h.resume();

		MSP2PLOG(MSP2PLOG_TEST, "StartDownloadImp(), add_torrent ok, rid = %s, create by %s", 
			get_hex_string_from_rid(rid).c_str(), p.ti ? "torrent" : "hash");
	}
	else
	{
		MSP2PLOG(MSP2PLOG_ERROR, "StartDownloadImp(), add_torrent failed, rid = %s", 
			get_hex_string_from_rid(rid).c_str());
		if (!rid.is_all_zeros() && fdc_handler)
		{
			fdc_handler(rid.begin(), -1);
		}
	}
}

void PauseDownloadImp(const RID rid)
{
	CBaseLockHandler lh(g_downLock);
	std::map<RID, torrent_handle>::iterator iter = handles.find(rid);
	if (iter != handles.end())
	{
		torrent_handle & th = iter->second;
		fdc_handler(rid.begin(), -2);
		th.pause();
	}
	else
	{
		if (!rid.is_all_zeros() && fdc_handler)
		{
			fdc_handler(rid.begin(), -1);
		}
	}
}

void ResumeDownloadImp(const RID rid)
{
	CBaseLockHandler lh(g_downLock);
	std::map<RID, torrent_handle>::iterator iter = handles.find(rid);
	if (iter != handles.end())
	{
		torrent_handle & th = iter->second;
		th.resume();
	}
	else
	{
		if (!rid.is_all_zeros() && fdc_handler)
		{
			fdc_handler(rid.begin(), -1);
		}
	}
}

void StopDownloadImp(const RID rid, bool remove_files)
{
	CBaseLockHandler lh(g_downLock);
	std::map<RID, torrent_handle>::iterator iter = handles.find(rid);
	if (iter != handles.end())
	{
		fdc_handler(rid.begin(), -2);

		torrent_handle th = iter->second;

		MSP2PLOG(MSP2PLOG_NOTICE, "StopDownloadImp(), Rid = %s, status = %d", 
			CP2PClientTool::HashToString(*(P2PHash_t*)&rid), th.status().state);
		if (!remove_files && torrent_status::downloading == th.status().state)
		{
			th.save_resume_data();
			my_alert_handler alert_handler;
			while(true)
			{
				alert const* a = ses->wait_for_alert(seconds(10));
				if (a == 0) break;
				std::auto_ptr<alert> alert_ptr = ses->pop_alert();

				try
				{
					alert_handler.handle_alert(alert_ptr);

					if (alert_cast<save_resume_data_failed_alert>(a) || 
						alert_cast<save_resume_data_alert>(a))
					{
						break;
					}

					//handle_alert<file_completed_alert
					//			, torrent_finished_alert
					//			, state_changed_alert
					//		>::handle_alert(alert_ptr, alert_handler);
				}
				catch(...)
				{
				}
			}
		}

		rhandles.erase(rhandles.find(iter->second));
		std::map<RID, torrent_handle>::iterator iterx = downloaders.find(iter->first);
		if(iterx != downloaders.end())
		{
			downloaders.erase(iterx);
		}

		ses->remove_torrent(th, remove_files ? session::delete_files : session::none);
		handles.erase(iter);
	}
	else
	{
		// maybe unknown to stop
		if (!rid.is_all_zeros() && fdc_handler)
		{
			fdc_handler(rid.begin(), -1);
		}
	}
}

void OnDownloadInfoTimer()
{
	std::map<RID, torrent_handle> tmp_downers;
	g_downLock.Lock();
	tmp_downers = downloaders;
	g_downLock.Unlock();

	std::map<RID, torrent_handle>::const_iterator iter = tmp_downers.begin();
	for (; iter != tmp_downers.end(); ++iter)
	{
		torrent_status ts = iter->second.status();
		if (ts.state == torrent_status::downloading)
		{
			if (!iter->second.is_paused() && !ses->is_paused())
			{
				di_handler(iter->first.begin(), 
					ts.total_wanted_done > 0 ? ts.total_wanted_done : ts.total_payload_download, // ???
					ts.download_payload_rate, ts.active_time);
			}
		}
		else if(ts.state == torrent_status::seeding)
		{
			// zcg+, 
			// drop suffix, ???
			// fdc_handler(iter->first.begin(), iter->second.status().total_wanted);
		}
		else
		{
			di_handler(iter->first.begin(), 0, 0, ts.active_time);
		}
	}
}

void OnUploadInfoTimer()
{
	try
	{
		if (!ses)
			return;

		CBaseLockHandler lh(g_downLock);
		std::map<RID, torrent_handle>::const_iterator iter = handles.begin();
		for (; iter != handles.end(); ++iter)
		{
			torrent_status ts = iter->second.status();

			// zcg+ ???
			if(ts.all_time_upload > 0 || ts.upload_payload_rate > 0)
			{
				ui_handler(iter->first.begin(), ts.all_time_upload, ts.upload_payload_rate, ts.seeding_time);
			}
		}
	}
	catch(...)
	{
		MSP2PLOG(MSP2PLOG_ERROR, "OnUploadInfoTimer() exception ...");
	}
}

void OnAlertTimer()
{
	using namespace libtorrent;

	my_alert_handler alert_handler;
	std::auto_ptr<alert> alert_ptr;
	alert_ptr = ses->pop_alert();    
	while (alert_ptr.get())
	{
		try
		{
			// zcg+
			//#ifdef _DEBUG
			//	MSP2PLOG(MSP2PLOG_NOTICE, "alert type = %s, "
			//		"message = %s", 
			//		alert_ptr->what(), 
			//		alert_ptr->message());
			//#endif

			alert_handler.handle_alert(alert_ptr);
		}
		catch(...)
		{
		}

		alert_ptr = ses->pop_alert();
	}
}

void TestP2PInfo(const RID & rid, libtorrent::torrent_status & status, std::string filename)
{
	MSP2PLOG(MSP2PLOG_NOTICE, "P2P Download Details Info (Test): 'rid' = '%s' ...\n"
			"---------------------------------------------------------------------------------------------------\n"
			"        time:      active = %d,       finish = %d,     seeding = %d,    /*announce_interval = %d*/\n"
			"        rate:      down_payload = %d, down = %d,       up_payload = %d, up = %d\n"
			"        size:      alltime_down = %lld, alltime_up = %lld, total_down = %lld, total_up = %lld, total_payload_down = %lld, total_payload_up = %lld\n"
			"                   total_done = %lld, total_wanted = %lld, total_wanted_done = %lld, total_redundant_byte = %lld, total_failed_byte = %lld\n"
			"        progress:  progress = %f,     progress_ppm = %d\n"
			"        peers:     connections = %d,  candidates = %d, peers = %d, seeds = %d, uploads = %d, completed = %d, incomplete = %d, \n"
			"        seeds:     mode = %d,         rank = %d,       list_peers = %d, list_seeds = %d\n"
			"        status:    state = %d,        priority = %d,   paused = %d, incoming = %d, storage = %d, block_size = %d, num_pieces = %d\n"
			"        bandwidth: down = %d,         up = %d\n"
			"        hole:      num = %d\n"
			"        limit:     connections = %d,  uploads = %d\n"
			"        current_tracker = %s\n"
			"        filenmae        = %s\n"
			"---------------------------------------------------------------------------------------------------\n", 
			get_hex_string_from_rid(rid).c_str(), 
			status.active_time, status.finished_time, status.seeding_time, /*status.announce_interval*/0, 
			status.download_payload_rate, status.download_rate, status.upload_payload_rate, status.upload_rate, 
			status.all_time_download, status.all_time_upload, status.total_download, status.total_upload, status.total_payload_download, status.total_payload_upload, 
			status.total_done, status.total_wanted, status.total_wanted_done, status.total_redundant_bytes, status.total_failed_bytes, 
			status.progress, status.progress_ppm, 
			status.num_connections, status.connect_candidates, status.num_peers, status.num_seeds, status.num_uploads, status.num_complete, status.num_incomplete, 
			status.seed_mode, status.seed_rank, status.list_peers, status.list_seeds, 
			status.state, status.priority, status.paused, status.has_incoming, status.storage_mode, status.block_size, status.num_pieces, 
			status.down_bandwidth_queue, status.up_bandwidth_queue, 
			status.sparse_regions, 
			status.connections_limit, status.uploads_limit, 
			status.current_tracker.c_str(), filename.c_str());
}


void SHARED_EXPORT GetState(const unsigned char* rid, DownloaderState option, void * data, size_t & len)
{
	CBaseLockHandler lh(g_downLock);
	std::map<RID, torrent_handle>::const_iterator it = handles.find(*(RID*)rid);
	if(it == handles.end() || !it->second.is_valid())
	{
		return ;
	}

	torrent_handle h = it->second;

	switch(option)
	{
	case Stat_FileSavePath:
		if(data == NULL)
		{
			len = h.save_path().string().length();
			break;
		}

		strcpy((char*)data, h.save_path().string().c_str());
		break;

	case Stat_FileName:
		if(data == NULL)
		{
			len = h.name().length();
			break;
		}

		strcpy((char*)data, h.name().c_str());
		break;

	case Stat_FileLength:
		*(int*)data = h.status().total_wanted;
		break;

	case Stat_DownloadSize:
		*(int*)data = h.status().total_wanted_done > 0 ? 
			h.status().total_wanted_done : h.status().total_payload_download;
		break;

	case Stat_DownloadTimes:
		*(int*)data = h.status().active_time;
		break;

	case Stat_DownloadRate:
		*(int*)data = h.status().download_payload_rate;
		break;

	case Stat_UploadSize:
		*(int*)data = h.status().total_payload_upload;
		break;

	case Stat_UploadTimes:
		*(int*)data = h.status().active_time;
		break;

	case Stat_UploadRate:
		*(int*)data = h.status().upload_payload_rate;
		break;

	case Stat_Seeds:
		*(int*)data = h.status().list_peers;
		break;

	case Stat_Peers:
		*(int*)data = h.status().list_seeds;
		break;

	default:
		break;
	}
}
