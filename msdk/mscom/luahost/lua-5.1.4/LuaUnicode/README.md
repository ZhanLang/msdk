##LuaUnicode
===
ANSI <-> UNICODE <-> UTF-8的lua扩展

目前仅支持5.1版本的Lua(5.1.5已测试通过)，5.2-5.3由于变更了新的c函数注册机制，暂时没有时间更新

##How to Compile
===
假设MinGW安装在C:\MinGW

```bash
gcc Lua_Unicode.c -I"C:\MinGW\include\lua" -L"C:\MinGW\lib" -lmingw32 -llua -lcomdlg32 -shared -o C:\Lua_Unicode.dll
```

##How to Use
===
--假定dll文件位于C:\Lua_Unicode.dll

--假定中文路径的文件名为C:\哈哈.xls

--假定C:\Unicode.csv为一个UTF-16LE编码文件

```lua
require "luacom"
local unicode = package.loadlib("C:\\Lua_Unicode.dll","luaopen_Unicode")

if(unicode)then
    unicode()
    print("OK!")
else
    print("Not found!")
    -- Error
end

a2u =Unicode.a2u
u2a =Unicode.u2a
u2u8 =Unicode.u2u8
u82u =Unicode.u82u
u82a =Unicode.u82a
a2u8 =Unicode.a2u8
getallfilewc = Unicode.getallfilewc
getallfilews = Unicode.getallfilews
getfilesizew= Unicode.getfilesizew
cmd =Pear.runcmd
opendialog =Pear.opendialog

print(getfilesizew("C:\\Unicode.csv"))
local x = os.clock()
getallfilewc("C:\\Unicode.csv")
print(string.format("elapsed time: %.2f\n",os.clock() - x ))

local x = os.clock()
getallfilews("C:\\Unicode.csv")
print(string.format("elapsed time: %.2f\n",os.clock() - x ))
print(a2u8("哈哈.xls"))

local oExcel = luacom.CreateObject("Excel.Application")
if oExcel == nilthen error("Objectis not create") end
    oExcel.Visible = 1
    oExcel.WorkBooks:Open(a2u8("C:\\哈哈.xls"),nil,0)
    oExcel.ActiveWorkbook.Sheets(1):Select()

t=opendialog("All Files(*.*)\0*.*\0")
print(t)

c=cmd( "ping 202.102.192.68")
print(c)

d=cmd("dir")
print(d)

e=cmd("ipconfig")
print (e)
```