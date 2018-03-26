/************************************************************************/
/* 
Author:

lourking. .All rights reserved.

Create Time:

	1,3th,2014

Module Name:

	dsTools.h 

Abstract: 

*/
/************************************************************************/


#ifndef __DSTOOLS_H__
#define __DSTOOLS_H__

namespace dsTools{

	template<class T>
	inline T Exchange2Objects(T &_value1, T &_value2){//返回第一个 值
		T temp = _value1;
		_value1 = _value2;
		_value2 = temp;
		return temp;
	}

	inline long GetApproximation(float f){
		if(f -(long)f > 0.5)
			return (long)f + 1;
		else
			return (long)f;
	}

};




#endif /*__DSTOOLS_H__*/