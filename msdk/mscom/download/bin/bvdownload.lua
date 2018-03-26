

--虚拟机句柄
_g_LuaVM = nil;

--主函数，用来初始化全局变量
function main(_luaVm)
	_g_LuaVM = _luaVm;
	
	--获取用户
	local _templateVm = _g_LuaVM;
	
	--local_templateVm = _g_LuaVM.ConvertToLuaVM(_g_LuaVM);
	local userid  = _templateVm.GetEnvParamString("userid");
	
	--获取网站地址
	local website = _templateVm.GetEnvParamString("website");
	
	log.log("user id = " .. userid);
	
	
	
	local _osinfo = winfunc.osinfo();
	--[[
		OS_Unknown = -1,
		OS_Lower = 4,
		OS_WinXP = 5,
		OS_Vista = 6,
		OS_Win7 = 7,
		OS_Win8 = 8,
		OS_Advanced = 100,
	]]
	
	osver = _osinfo.GetOsVersion(); 	-- get os version return value type is int
	osinfo = _osinfo.GetOsInfo();		-- get os string info eg. Microsoft Windows 7 Ultimate Edition 0x0035d0780x0315926876010x03159280, 64-bit
	disksize = _osinfo.GetDiskSize();		-- get full disk size . return value type is string
	freesize = _osinfo.GetDiskFreeSize();	-- get free disk size . return value type is string
	username = _osinfo.GetUserName();		-- get username . return value type is string.
	computername = _osinfo.GetComputerName();	-- get computer name . return value type is string.
	is64 = _osinfo.IsWow64();			-- the os is wow64.
	
	strIs64 = nil;
	if(is64 == true) then
		strIs64 = "true";
	else
		strIs64 = "false"
	end
	
	strInfo = string.format("the os info[osver=%d,osinfo=%s,disksize=%s,freesize=%s,usename=%s,computername=%s,is64=%s,userid=%s]", 
	osver,osinfo.c_str(), disksize.c_str(), freesize.c_str(), username.c_str(), computername.c_str(), strIs64, userid.c_str());
	
	log.log(strInfo);
	
end


--实现HTTP请求
--host ： 请求的域名
--path ： 请求的路径
--param ： 参数 table 类型
local function _http_request(host, path, param)
	local url = host .. "/";
	if path and string.len(path) then 
		url = url .. path;
	end
	
	if param and table.getn(param) then
		local first_param = true;
		url = url .. "?";
		for key, value in pairs(param) do
			if first_param then
				url = url .. key .. "=" .. http.encode_url(value);
				first_param = false;
			else
				url = url .. "&" .. key .. "=" .. http.encode_url(value);
			end
		end  
	end
	return http.http_request(url);
end

function http_request(url)
	local _host = "http://www.KYan.com.cn";
	local _path = "home/updata/check";
	local _param = 
		{
			["usrid"] 	 = "{940CEE0A-9233-28FE-95EE-0C943392FE28}",
			["usrname"]  = "magj",
			["prodid"] 	 = "{7C0A7FB2-C4F7-461f-A527-CBA0D3367C1A}",
			["prodname"] = "kyan",
			["prodver"]  = "1.0.2.1",
			["oper"]	 = "kyan.com.cn",
		};
	local _context = _http_request(_host, _path, _param);
	return _context;
end


--下载文件，成功返回true
function http_download(url, path)
	if not http.http_download(url, path) then
		log.error("download file(" .. path .. ")failed.");
		return false;
	end
	return true;
end