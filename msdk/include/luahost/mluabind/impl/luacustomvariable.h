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

namespace mluabind
{

namespace impl
{

struct GenericClass;

struct LuaCustomVariable
{
	inline LuaCustomVariable(bool _is_own, bool _is_const, const GenericClass *_gc, void *_ptr = 0):
		is_own(_is_own),
		is_const(_is_const),
		gclass(_gc),
		ptr(_ptr)
		{};
	inline LuaCustomVariable() {};
	inline bool IsOwn() const { return is_own; };
	inline bool IsConst() const { return is_const; };
	inline void SetConst(bool _c) { is_const = _c; };
	inline void SetOwning(bool _o) { is_own = _o; };
	inline void SetValue(void *_val) {ptr = _val; };
	inline void *value() const { return ptr; }; 
	inline const GenericClass *getclass() const { return gclass; };
private:
	void *ptr;
	const GenericClass *gclass;
	bool is_own;
	bool is_const;
};

}; // namespace impl

}; // namespace mluabind
