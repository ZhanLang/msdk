// Copyright (c) 2007-2008 Michael Kazakov
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
/////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>


namespace mluabind
{

namespace impl
{

struct GenericMethodPolicy
{
	GenericMethodPolicy()
	{
		m_Group.push_back(this);
	};

	GenericMethodPolicy(int)
	{
		// dummy constructor for default parameter
	};

	inline void BeforePerformCall(CHost *_host, lua_State *L, LuaCustomVariable *_lcv, bool _is_const)
	{
		GroupT::iterator i = m_Group.begin(), e = m_Group.end();
		for(;i!=e;++i)
			(*i)->BeforePerformCallV(_host, L, _lcv, _is_const);
	};

	virtual void BeforePerformCallV(CHost *_host, lua_State *L, LuaCustomVariable *_lcv, bool _is_const)
	{
	};

	inline int AfterPerformCall(CHost *_host, lua_State *L, LuaCustomVariable *_lcv, bool _is_const, int _res_num)
	{
		GroupT::iterator i = m_Group.begin(), e = m_Group.end();
		for(;i!=e;++i)
			_res_num = (*i)->AfterPerformCallV(_host, L, _lcv, _is_const, _res_num);
		return _res_num;
	};

	virtual int AfterPerformCallV(CHost *_host, lua_State *L, LuaCustomVariable *_lcv, bool _is_const, int _res_num)
	{
		return _res_num;
	};


	inline GenericMethodPolicy &operator +(GenericMethodPolicy &_right)
	{
		m_Group.push_back(&_right);
		return *this;
	};

	typedef std::vector<GenericMethodPolicy*> GroupT;
	GroupT m_Group;
};

struct AdoptPolicy : GenericMethodPolicy
{
	// -1: return value
	// 0: this
	// 1, 2, 3 etc: params
	AdoptPolicy(int _ind):
		m_Ind(_ind)
	{
		assert(_ind >= -1);
	};

	virtual int AfterPerformCallV(CHost *_host, lua_State *L, LuaCustomVariable *_lcv, bool _is_const, int _res_num)
	{
		if(m_Ind == -1 && _res_num == 1)
		{
			// should set owning to lua
			if(lua_isuserdata(L, -1) && !lua_islightuserdata(L, -1))
			{
				LuaCustomVariable *lcv = (LuaCustomVariable *)lua_touserdata(L, -1);
				lcv->SetOwning(true);
			}
		}
		return _res_num;
	};

	virtual void BeforePerformCallV(CHost *_host, lua_State *L, LuaCustomVariable *_lcv, bool _is_const)
	{
		if(m_Ind == 0 && _lcv != 0)
		{
			_lcv->SetOwning(false);
		}
		else if(m_Ind > 0)
		{
			if(lua_gettop(L) >= m_Ind)
			{
				if(lua_isuserdata(L, m_Ind) && !lua_islightuserdata(L, m_Ind))
				{
					LuaCustomVariable *lcv = (LuaCustomVariable *)lua_touserdata(L, m_Ind);
					lcv->SetOwning(false);
				}
			}
		}
	};

	int m_Ind;
};

}; // namespace impl


inline impl::GenericMethodPolicy &Adopt(int _ind)
{
	return *(new impl::AdoptPolicy(_ind));
};



}; // namespace mluabind
