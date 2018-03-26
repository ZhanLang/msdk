// Copyright (c) 2007 Michael Kazakov
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
template <class _T>
inline void CHost::PutNewImplicitCreated(_T *_obj)
{
	m_ImplicitCreated.push( new impl::ConcreteImplicitCreatedHolder<_T>(_obj) );	
};

template <class _Ret>
inline _Ret CHost::CallLuaFunction(const std::string &_name)
{
	return impl::CallLuaFunction<_Ret>::Do(this, _name, 0);
};

template <class _Ret, class _P1>
inline _Ret CHost::CallLuaFunction(const std::string &_name, _P1 _p1)
{
	return impl::CallLuaFunction<_Ret, _P1>::Do(this, _name, 1, _p1);
};

template <class _Ret, class _P1, class _P2>
inline _Ret CHost::CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2)
{
	return impl::CallLuaFunction<_Ret, _P1, _P2>::Do(this, _name, 2, _p1, _p2);
};

template <class _Ret, class _P1, class _P2, class _P3>
inline _Ret CHost::CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3)
{
	return impl::CallLuaFunction<_Ret, _P1, _P2, _P3>::Do(this, _name, 3, _p1, _p2, _p3);
};

template <class _Ret, class _P1, class _P2, class _P3, class _P4>
inline _Ret CHost::CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4)
{
	return impl::CallLuaFunction<_Ret, _P1, _P2, _P3, _P4>::Do(this, _name, 4, _p1, _p2, _p3, _p4);
};

template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5>
inline _Ret CHost::CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5)
{
	return impl::CallLuaFunction<_Ret, _P1, _P2, _P3, _P4, _P5>::Do(this, _name, 5, _p1, _p2, _p3, _p4, _p5);
};

template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6>
inline _Ret CHost::CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6)
{
	return impl::CallLuaFunction<_Ret, _P1, _P2, _P3, _P4, _P5, _P6>::Do(this, _name, 6, _p1, _p2, _p3, _p4, _p5, _p6);
};

template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7>
inline _Ret CHost::CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7)
{
	return impl::CallLuaFunction<_Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7>::Do(this, _name, 7, _p1, _p2, _p3, _p4, _p5, _p6, _p7);
};

template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8>
inline _Ret CHost::CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8)
{
	return impl::CallLuaFunction<_Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8>::Do(this, _name, 8, _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8);
};

template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9>
inline _Ret CHost::CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8, _P9 _p9)
{
	return impl::CallLuaFunction<_Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9>::Do(this, _name, 9, _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9);
};

template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9, class _P10>
inline _Ret CHost::CallLuaFunction(const std::string &_name, _P1 _p1, _P2 _p2, _P3 _p3, _P4 _p4, _P5 _p5, _P6 _p6, _P7 _p7, _P8 _p8, _P9 _p9, _P10 _p10)
{
	return impl::CallLuaFunction<_Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9, _P10>::Do(this, _name, 10, _p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9, _p10);
};
