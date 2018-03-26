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

#include "genericclass.h"

namespace mluabind
{

class CHost;

namespace impl
{

template<class _Class>
struct CustomClass : GenericClass
{
	typedef CustomClass<_Class> Me;

	inline CustomClass (const char* _luaname) :
		GenericClass(_luaname, &Me::MetaGC, typeid(Me)) {};

	///////////////////////////////
	// inheritance/smartptr section
	template <class _Base> Me &Base() {
		return InsertConverter(new ClassInheritanceConvertAdapter(typeid(CustomClass<_Base>), &InheritedCaster<_Class, _Base>::UpCast, &InheritedCaster<_Class, _Base>::DownCast)), *this; };

	template <class _Base> Me &BaseNoDynCast() {
		return InsertConverter(new ClassUpCastConvertAdapter(typeid(CustomClass<_Base>), &InheritedCaster<_Class, _Base>::UpCast)), *this; };

	template <class _Base> Me &SmartPtr(){
		return InsertConverter(new ClassUpCastConvertAdapter(typeid(CustomClass<_Base>), &SmartPtrUpCaster<_Class, _Base>::UpCast)), *this; };

	template <class _Base> Me &Convert(){
		return InsertConverter(new ClassUpCastConvertAdapter(typeid(CustomClass<_Base>), &ConvertUpCaster<_Class, _Base>::UpCast)), *this; };

	////////////////////////
	// class methods section
	template <class _Ret> inline Me &Method(const char* _name, _Ret (_Class::*_F) (), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass0<_Class, false, _Ret>(_name, _F, _p)), *this; };
	template <class _Ret> inline Me &Method(const char* _name, _Ret (_Class::*_F) () const, const GenericMethodPolicy &_p = GenericMethodPolicy(0) ) {
		return InsertMethod(_name, new MethodClass0<_Class, true, _Ret>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass1<_Class, false, _Ret, _P1>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1) const, const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass1<_Class, true, _Ret, _P1>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass2<_Class, false, _Ret, _P1, _P2>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2) const, const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass2<_Class, true, _Ret, _P1, _P2>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass3<_Class, false, _Ret, _P1, _P2, _P3>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3) const, const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass3<_Class, true, _Ret, _P1, _P2, _P3>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3, class _P4> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3, _P4), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass4<_Class, false, _Ret, _P1, _P2, _P3, _P4>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3, class _P4> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3, _P4) const, const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass4<_Class, true, _Ret, _P1, _P2, _P3, _P4>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3, _P4, _P5), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass5<_Class, false, _Ret, _P1, _P2, _P3, _P4, _P5>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3, _P4, _P5) const, const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass5<_Class, true, _Ret, _P1, _P2, _P3, _P4, _P5>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3, _P4, _P5, _P6), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass6<_Class, false, _Ret, _P1, _P2, _P3, _P4, _P5, _P6>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3, _P4, _P5, _P6) const, const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass6<_Class, true, _Ret, _P1, _P2, _P3, _P4, _P5, _P6>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3, _P4, _P5, _P6, _P7), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass7<_Class, false, _Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3, _P4, _P5, _P6, _P7) const, const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass7<_Class, true, _Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass8<_Class, false, _Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8) const, const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass8<_Class, true, _Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass9<_Class, false, _Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9> inline Me &Method(const char* _name, _Ret (_Class::*_F) (_P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9) const, const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new MethodClass9<_Class, true, _Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9>(_name, _F, _p)), *this; };

	/////////////////////////
	// global methods section
	template <class _Ret> inline Me &Method(const char* _name, _Ret (*_F) (_Class*), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new GlobalMethodClass0<_Class, false, _Ret>(_name, _F, _p)), *this; };
	template <class _Ret> inline Me &Method(const char* _name, _Ret (*_F) (const _Class*), const GenericMethodPolicy &_p = GenericMethodPolicy(0) ) {
		return InsertMethod(_name, new GlobalMethodClass0<_Class, true, _Ret>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1> inline Me &Method(const char* _name, _Ret (*_F) (_Class*, _P1), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new GlobalMethodClass1<_Class, false, _Ret, _P1>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1> inline Me &Method(const char* _name, _Ret (*_F) (const _Class*, _P1), const GenericMethodPolicy &_p = GenericMethodPolicy(0) ) {
		return InsertMethod(_name, new GlobalMethodClass1<_Class, true, _Ret, _P1>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2> inline Me &Method(const char* _name, _Ret (*_F) (_Class*, _P1, _P2), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new GlobalMethodClass2<_Class, false, _Ret, _P1, _P2>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2> inline Me &Method(const char* _name, _Ret (*_F) (const _Class*, _P1, _P2), const GenericMethodPolicy &_p = GenericMethodPolicy(0) ) {
		return InsertMethod(_name, new GlobalMethodClass2<_Class, true, _Ret, _P1, _P2>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3> inline Me &Method(const char* _name, _Ret (*_F) (_Class*, _P1, _P2, _P3), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new GlobalMethodClass3<_Class, false, _Ret, _P1, _P2, _P3>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3> inline Me &Method(const char* _name, _Ret (*_F) (const _Class*, _P1, _P2, _P3), const GenericMethodPolicy &_p = GenericMethodPolicy(0) ) {
		return InsertMethod(_name, new GlobalMethodClass3<_Class, true, _Ret, _P1, _P2, _P3>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3, class _P4> inline Me &Method(const char* _name, _Ret (*_F) (_Class*, _P1, _P2, _P3, _P4), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new GlobalMethodClass4<_Class, false, _Ret, _P1, _P2, _P3, _P4>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3, class _P4> inline Me &Method(const char* _name, _Ret (*_F) (const _Class*, _P1, _P2, _P3, _P4), const GenericMethodPolicy &_p = GenericMethodPolicy(0) ) {
		return InsertMethod(_name, new GlobalMethodClass4<_Class, true, _Ret, _P1, _P2, _P3, _P4>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5> inline Me &Method(const char* _name, _Ret (*_F) (_Class*, _P1, _P2, _P3, _P4, _P5), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new GlobalMethodClass5<_Class, false, _Ret, _P1, _P2, _P3, _P4, _P5>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5> inline Me &Method(const char* _name, _Ret (*_F) (const _Class*, _P1, _P2, _P3, _P4, _P5), const GenericMethodPolicy &_p = GenericMethodPolicy(0) ) {
		return InsertMethod(_name, new GlobalMethodClass5<_Class, true, _Ret, _P1, _P2, _P3, _P4, _P5>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6> inline Me &Method(const char* _name, _Ret (*_F) (_Class*, _P1, _P2, _P3, _P4, _P5, _P6), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new GlobalMethodClass6<_Class, false, _Ret, _P1, _P2, _P3, _P4, _P5, _P6>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6> inline Me &Method(const char* _name, _Ret (*_F) (const _Class*, _P1, _P2, _P3, _P4, _P5, _P6), const GenericMethodPolicy &_p = GenericMethodPolicy(0) ) {
		return InsertMethod(_name, new GlobalMethodClass6<_Class, true, _Ret, _P1, _P2, _P3, _P4, _P5, _P6>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7> inline Me &Method(const char* _name, _Ret (*_F) (_Class*, _P1, _P2, _P3, _P4, _P5, _P6, _P7), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new GlobalMethodClass7<_Class, false, _Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7> inline Me &Method(const char* _name, _Ret (*_F) (const _Class*, _P1, _P2, _P3, _P4, _P5, _P6, _P7), const GenericMethodPolicy &_p = GenericMethodPolicy(0) ) {
		return InsertMethod(_name, new GlobalMethodClass7<_Class, true, _Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8> inline Me &Method(const char* _name, _Ret (*_F) (_Class*, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new GlobalMethodClass8<_Class, false, _Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8> inline Me &Method(const char* _name, _Ret (*_F) (const _Class*, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8), const GenericMethodPolicy &_p = GenericMethodPolicy(0) ) {
		return InsertMethod(_name, new GlobalMethodClass8<_Class, true, _Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8>(_name, _F, _p)), *this; };

	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9> inline Me &Method(const char* _name, _Ret (*_F) (_Class*, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertMethod(_name, new GlobalMethodClass9<_Class, false, _Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9>(_name, _F, _p)), *this; };
	template <class _Ret, class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9> inline Me &Method(const char* _name, _Ret (*_F) (const _Class*, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9), const GenericMethodPolicy &_p = GenericMethodPolicy(0) ) {
		return InsertMethod(_name, new GlobalMethodClass9<_Class, true, _Ret, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9>(_name, _F, _p)), *this; };

	///////////////////////
	// constructors section
	inline Me &Constructor(const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertConstructor(new ConstructorClass0<_Class>(_p)), *this; };

	template<class _P1> inline Me &Constructor(const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertConstructor(new ConstructorClass1<_Class, _P1>(_p)), *this; };

	template<class _P1, class _P2> inline Me &Constructor(const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertConstructor(new ConstructorClass2<_Class, _P1, _P2>(_p)), *this; };

	template<class _P1, class _P2, class _P3> inline Me &Constructor(const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertConstructor(new ConstructorClass3<_Class, _P1, _P2, _P3>(_p)), *this; };

	template<class _P1, class _P2, class _P3, class _P4> inline Me &Constructor(const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertConstructor(new ConstructorClass4<_Class, _P1, _P2, _P3, _P4>(_p)), *this; };

	template<class _P1, class _P2, class _P3, class _P4, class _P5> inline Me &Constructor(const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertConstructor(new ConstructorClass5<_Class, _P1, _P2, _P3, _P4, _P5>(_p)), *this; };

	template<class _P1, class _P2, class _P3, class _P4, class _P5, class _P6> inline Me &Constructor(const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertConstructor(new ConstructorClass6<_Class, _P1, _P2, _P3, _P4, _P5, _P6>(_p)), *this; };

	template<class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7> inline Me &Constructor(const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertConstructor(new ConstructorClass7<_Class, _P1, _P2, _P3, _P4, _P5, _P6, _P7>(_p)), *this; };

	template<class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8> inline Me &Constructor(const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertConstructor(new ConstructorClass8<_Class, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8>(_p)), *this; };

	template<class _P1, class _P2, class _P3, class _P4, class _P5, class _P6, class _P7, class _P8, class _P9> inline Me &Constructor(const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertConstructor(new ConstructorClass9<_Class, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9>(_p)), *this; };

	/////////////////////////////
	// internal operators section
	template <class _Ret, class _Right> inline Me& Operator(int _type, _Ret (_Class::*_F) (_Right), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertOperator(_type, new MethodClass1<_Class, false, _Ret, _Right>(g_LuaOperatorNames[_type], _F, _p, true)), *this; };

	template <class _Ret, class _Right> inline Me& Operator(int _type, _Ret (_Class::*_F) (_Right) const, const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertOperator(_type, new MethodClass1<_Class, true, _Ret, _Right>(g_LuaOperatorNames[_type], _F, _p, true)), *this; };

	template <class _Ret> inline Me& Operator(int _type, _Ret (_Class::*_F) (), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertOperator(_type, new MethodClass0<_Class, false, _Ret>(g_LuaOperatorNames[_type], _F, _p, true)), *this; };

	template <class _Ret> inline Me& Operator(int _type, _Ret (_Class::*_F) () const, const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertOperator(_type, new MethodClass0<_Class, true, _Ret>(g_LuaOperatorNames[_type], _F, _p, true)), *this; };

	/////////////////////////////
	// external operators section
	template <class _Ret, class _Right> inline Me& Operator(int _type, _Ret (*_F) (_Class*, _Right), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertOperator(_type, new GlobalMethodClass1<_Class, false, _Ret, _Right>(g_LuaOperatorNames[_type], _F, _p, true)), *this; };

	template <class _Ret, class _Right> inline Me& Operator(int _type, _Ret (*_F) (const _Class*, _Right), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertOperator(_type, new GlobalMethodClass1<_Class, true, _Ret, _Right>(g_LuaOperatorNames[_type], _F, _p, true)), *this; };

	template <class _Ret> inline Me& Operator(int _type, _Ret (*_F) (_Class*), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertOperator(_type, new GlobalMethodClass0<_Class, false, _Ret>(g_LuaOperatorNames[_type], _F, _p, true)), *this; };

	template <class _Ret> inline Me& Operator(int _type, _Ret (*_F) (const _Class*), const GenericMethodPolicy &_p = GenericMethodPolicy(0)) {
		return InsertOperator(_type, new GlobalMethodClass0<_Class, true, _Ret>(g_LuaOperatorNames[_type], _F, _p, true)), *this; };

	///////////////////////////////////
	// flexi operators section
	template<int _Op, bool _const_self, bool _first_self, class _Other> inline Me& Operator(const impl::BinaryOperatorBaseHolder<_Op, _const_self, _first_self, _Other>&){
		return InsertOperator(_Op, new impl::BinaryOperatorGenerator<_Class, _Op, _const_self, _first_self, _Other>), *this; };

	///////////////////////////////////////////////
	// Member, Enums and Free Constant declarations
	template <class _P> inline Me& Member(const char* _name, _P _Class::*_obj){
		return InsertMember(_name, new MemberClass<_Class, _P>(_obj)), *this; };

	template <class _R, class _P> inline Me& Member(const char* _name, _R (_Class::*_getter) () const, void (_Class::*_setter) (_P)) {
		return InsertMember(_name, new PropertyClass<_Class, _P, _R>(_getter, _setter)), *this; };

	template <class _R> inline Me& Member(const char* _name, _R (_Class::*_getter) () const){
		return InsertMember(_name, new PropertyClass<_Class, int, _R>(_getter, 0)), *this; };

	template <class _R, class _P> inline Me& Member(const char* _name, _R (*_getter) (const _Class*), void (*_setter) (_Class*, _P)) {
		return InsertMember(_name, new FreePropertyClass<_Class, _P, _R>(_getter, _setter)), *this; };

	template <class _R> inline Me& Member(const char* _name, _R (*_getter) (const _Class*)){
		return InsertMember(_name, new FreePropertyClass<_Class, int, _R>(_getter, 0)), *this; };

	template <class _P> Me& Constant(const char* _name, _P _p){
		return InsertConstant(_name, new CustomConstant<_P>(_p, _name)), *this; };

	template <class _P>	inline Me& Enum(const char* _name, _P _p){
		return Constant(_name, (lua_Number) _p); };

	static int ConstructObjectByCopyConstructor(lua_State *L, CHost *_host, GenericClass *_this, const _Class& _obj, bool _const)
	{
		LuaCustomVariable *s = _this->ConstructLuaUserdataObject(L);
		s->SetValue((void*) new _Class(_obj));
		s->SetConst(_const);
		return 1;
	};

private:

	static int MetaGC(lua_State* L)
	{
		assert(lua_isuserdata(L, -1) != 0);
		assert(lua_islightuserdata(L, -1) == 0);

		LuaCustomVariable *ud = (LuaCustomVariable *)lua_touserdata(L, -1);
		if(ud->IsOwn())
		{
			assert( ud->value() != 0 );
			delete (_Class *)ud->value();
		}

		return 0;
	}
};

template <class _P>
struct CustomConstant : GenericConstant
{
	_P m_Obj;

	inline CustomConstant(_P&_p, const char* _name): 
		GenericConstant(_name, _name),
		m_Obj(_p)
		{};

	void PutOnStack(lua_State *L)
	{
		CreateCustomLuaVariable<impl::ResolvCallType<_P>::CallType, _P>
			::Do(L, CHost::GetFromLua(L), m_Obj, impl::ResolvCallType<_P>::IsConst);
	};			 
};

}; // namespace impl

template <class C>
inline impl::CustomClass<C> &Class(const char* _str = "")
{
	return *(new impl::CustomClass<C>(_str));
};

template <class _P>
inline impl::GenericConstant& Constant(const char* _name, _P _p)
{
	return *(new impl::CustomConstant<_P>(_p, _name));
};

inline impl::GenericConstant& Enum(const char* _name, lua_Number _p)
{
	return Constant(_name, _p);
};

}; // namespace mluabind
