dbg("start");


function TestZip()
	local zfile, err = zip.open("image.zip");
	print("File list begin")
	for file in zfile:files() do
		local fileName = lc.u82a(file.filename);
		print(fileName);
		local f1, err = zfile:open(file.filename);
		if file.isdir then --Ä¿Â¼£¿
			--print(fileName .. "is dir");
			lfs.mkdir(fileName);
		else
			local currFileContents = f1:read("*a") ;
			local hBinaryOutput = io.open(fileName, "wb");
			if hBinaryOutput == nil then
				print("hBinaryOutput == nil");
			else
				hBinaryOutput:write(currFileContents)
				hBinaryOutput:close()
			end
		end
		--print(file.filename);
	end
	
	print("File list ended OK!")
	
end

function TestReg()
	local r = sys.win32.registry();
	r:open("HKEY_LOCAL_MACHINE",[[Software\Microsoft\Windows\CurrentVersion\Setup]]);
	for i, k, v in r:values() do
		print(tostring(k) .. ' = "' .. tostring(v) .. '"\n')
	end
  if r:set("TEST_SET", 666) then
    error("'Access denied.' expected")
  end
  r:close()
  print("OK")
	
end

TestReg();
--TestZip();