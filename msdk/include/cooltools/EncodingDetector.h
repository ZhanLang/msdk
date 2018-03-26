/***********************************************************************
 * 功能描述：	编码检测器
 * 详细说明：	根据字节特点猜测文本编码，给出各种编码的可能性排序
 *				目前支持UTF-8、GB2312/GBK、Big5
 * 创建时间：	2009-04-25
 * 最后修改：	2009-04-25
 * 程序编写：	田雨(从java代码移植，增加注释)
 ***********************************************************************/
/////////////////////////////////////////////STL INCLUDE///////////////////////////////////
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>
#include <algorithm>
#include <iosfwd>
#include <vector>
#include <set>
#include <istream>
#include <streambuf>
#include <deque>
#include <cctype>
#include <iterator>
#include <map>
/////////////////////////////////////////////STL INCLUDE///////////////////////////////////
// using System;
// using System.Collections;
// using System.Collections.Generic;
// using System.Text;
using namespace std;

typedef unsigned char byte;

namespace rlTitleFix
{
	/// <summary>
	/// 编码检测器
	/// </summary>
	public class EncodingDetector
	{
	private:
		static int GBFreq[94][94];// = new int[94][94];			//GB2312共94个区94个位
		static int GBKFreq[126][191];// = new int[126,191];
		static int Big5Freq[94][158];// = new int[94][158];

		/// <summary>
		/// 静态构造函数
		/// </summary>
		EncodingDetector()
		{
			InitFreq();
		}

		/// <summary>
		/// 根据字节数组检测文本编码
		/// </summary>
		/// <param name="bytes">字节数组</param>
		/// <returns>编码</returns>
	public:
		static std::string Detect(byte bytes[])
		{
			typedef std::map<std::string, int> mapList;
			mapList probability;
			int maxScore = 0;
			std::string strEncode = "";

// 			Encoding encoding = Encoding.Default;
// 			Dictionary<string, int> probability = new Dictionary<string, int>();

			probability["utf-8"] = UTF8_Probability(bytes);
			probability["gb2312"] = GBK_Probability(bytes);
			probability["big5"] = Big5_Probability(bytes);

			/*
			//gb2312实际已被GBK所代替，gb18030与gbk差异也不大
			probability["gb2312"] = GB2312_Probability(bytes);
			probability["gb18030"] = GB18030_Probability(bytes);
			*/

			//从各种编码中找可能性最大的
// 			foreach(string key in probability.Keys)
// 			{
// 				if(probability[key] > maxScore)
// 				{
// 					encoding = Encoding.GetEncoding(key);
// 					maxScore = probability[key];
// 				}
// 			}
// 
// 			return encoding;
			for (mapList::iterator it = probability.begin(); it != probability.end(); it++) 
			{
				if (it->second > maxScore)
				{
					strEncode = it->first;
					maxScore = it->second;
				}
			}
			return strEncode;
		}

		/// <summary>
		/// 检测utf8的可能性
		/// </summary>
		/// <param name="bytes">字节数组</param>
		/// <returns>可能性(0-100)</returns>
		static int UTF8_Probability(byte bytes[])
		{
			/* utf8特点：
			 * 变长编码，每个字符占1~3字节
			 * 0000-007F之间表示为 0xxxxxxx
			 * 0080-07FF之间表示为 110xxxxx 10xxxxxx
			 * 0800-FFFF之间表示为 1110xxxx 10xxxxxx 10xxxxxx
			 */

			int score = 0;
			int bytesLen = sizeof( bytes );
			int utf8Bytes = 0;
			int asciiBytes = 0;

			for(int i = 0; i < bytesLen; ++i)
			{
				if((bytes[i] & (byte)0x7F) == bytes[i])
				{
					//匹配0xxxxxxx模板，Ascii字符
					asciiBytes++;
				}
				else if(bytes[i] >= 0xC0 && bytes[i] <= 0xDF &&
					(i + 1) < bytesLen && bytes[i + 1] >= 0x80 && bytes[i + 1] <= 0xBF)
				{
					//匹配110xxxxx 10xxxxxx模板，第一字节0xC0-0xDF，第二字节0x80-0xBF
					utf8Bytes += 2;
					i++;
				}
				else if(bytes[i] >= 0xE0 && bytes[i] <= 0xEF
					&& (i + 2) < bytesLen && bytes[i + 1] >= 0x80 && bytes[i + 1] <= 0xBF && bytes[i + 2] >= 0x80 && bytes[i + 2] <= 0xBF)
				{
					//匹配1110xxxxx 10xxxxxx 10xxxxxx模板，第一字节0xE0-0xEF，第二第三字节均为0x80-0xBF
					utf8Bytes += 3;
					i += 2;
				}
			}

			if(asciiBytes == bytesLen)
			{
				//如果全是ascii字符，证明这不是一个utf8编码，虽然utf8在0x7F以下的部分与ascii一致
				score = 0;
			}
			else
			{
				//计算utf8字符数与其它字符数(ascii除外)的比例
				score = (int)(100 * ((float)utf8Bytes / (float)(bytesLen - asciiBytes)));

				//如果比例大于98%，或者比例在95%以上且utf8字符数大于30，认为计算出的比例可作为有效性，否则可能性为0
				//注意下面的表达式是上述逻辑的变形 !(A | (B & C)) = !A & (!B | !C)
				if(score <= 98 && (score <= 95 || utf8Bytes <= 30))
					score = 0;
			}

			return score;
		}

		/// <summary>
		/// 检测gb2312的可能性
		/// </summary>
		/// <param name="bytes">字节数组</param>
		/// <returns>可能性(0-100)</returns>
		static int GB2312_Probability(byte bytes[])
		{
			/* gb2312特点：
			 * 定长编码，每个字符占2字节，第一字节称为高字节，第二字节称为低字节，兼容ascii(单字节)
			 * 高字节在0xA1至0xF7之间，低字节在0xA1至0xFE之间
			 * 高低字节分别减0xA0可得到区位码，0-9区是符号数字区，10-15区和88-94区有空白
			 * 16-55区是一级汉字，按拼音排序，56-87区是二级汉字，按部首排序
			 * 对汉字来说，高字节在0xB0-0xF7之间
			 */

			int bytesLen = sizeof( bytes );
			int dbChars = 1,gbChars = 1;			//双字节字符数，GB码字符数
			long gbFreq = 0,totalFreq = 1;
			float rangeVal = 0,freqVal = 0;
			int row,column;

			for(int i = 0; i < bytesLen - 1; ++i)
			{
				if(bytes[i] >= 0x80)				//小于0x80为ascii，不考虑，否则为双字节字符
				{
					dbChars++;		//找到一个双字节字符

					//高字节在0xA1至0xF7之间，低字节在0xA1至0xFE之间
					if(bytes[i] >= (byte)0xA1 && bytes[i] <= (byte)0xF7 && bytes[i + 1] >= (byte)0xA1 && bytes[i + 1] <= (byte)0xFE)
					{
						gbChars++;					//是一个GB码字符(范围上匹配)
						totalFreq += 500;
						row = bytes[i] - 0xA1;		//高低字节分别减0xA0得到区位码，注意要再减1得到以0起始的索引
						column = bytes[i + 1] - 0xA1;
						if(GBFreq[row][column] != 0)
						{
							//命中GBFreq中的元素，要加对应的分
							gbFreq += GBFreq[row][column];
						}
						else if(row >= 15 && row <= 54)
						{
							//16区至55区为一级汉字区，要加200分
							gbFreq += 200;
						}
					}
					++i;	//由于一次检查了2个字节，要把i再自增一次
				}
			}

			rangeVal = 50 * ((float)gbChars / (float)dbChars);		//范围分，0-50，看GB码字符占双字节字符的比例
			freqVal = 50 * ((float)gbFreq / (float)totalFreq);		//匹配分，0-50，看GB码字符的得分情况

			return (int)(rangeVal + freqVal);		//最终返回的可能性即上面两个得分的和
		}

		/// <summary>
		/// 检测gbk的可能性
		/// </summary>
		/// <param name="bytes">字节数组</param>
		/// <returns>可能性(0-100)</returns>
		static int GBK_Probability(byte bytes[])
		{
			/* gbk特点：
			 * 定长编码，每个字符占2字节，第一字节称为高字节，第二字节称为低字节，向下兼容gb2312、ascii(单字节)
			 * 汉字区	包括
			 * GBK/2：OxBOA1-F7FE， 收录GB2312汉字6763个，按原序排列
			 * GBK/3：Ox8140-AOFE，收录CJK汉字6080个
			 * GBK/4：OxAA40-FEAO，收录CJK汉字和增补的汉字8160个
			 * 图形符号区　包括
			 * GBK/1：OxA1A1-A9FE，除GB2312的符号外，还增补了其它符号
			 * GBK/5：OxA840-A9AO，扩充非汉字区
			 */

			int bytesLen = sizeof( bytes );
			int dbChars = 1, gbChars = 1;			//双字节字符数，GB码字符数
			long gbFreq = 0, totalFreq = 1;
			float rangeVal = 0, freqVal = 0;
			int row, column;

			for(int i = 0; i < bytesLen - 1; i++)
			{
				if(bytes[i] >= 0x80)				//小于0x80为ascii，不考虑，否则为双字节字符
				{
					dbChars++;		//找到一个双字节字符

					//高字节在0xA1至0xF7之间，低字节在0xA1至0xFE之间，这是兼容gb2312的部分
					if(bytes[i] >= (byte)0xA1 && bytes[i] <= (byte)0xF7 && bytes[i + 1] >= (byte)0xA1 && bytes[i + 1] <= (byte)0xFE)
					{
						gbChars++;					//是一个GBK/GB2312码字符(范围上匹配)
						totalFreq += 500;
						row = bytes[i] - 0xA1;		//高低字节分别减0xA0得到区位码，注意要再减1得到以0起始的索引
						column = bytes[i + 1] - 0xA1;

						if(GBFreq[row,column] != 0)
						{
							//命中GBFreq中的元素，要加对应的分
							gbFreq += GBFreq[row][column];
						}
						else if(row >= 15 && row <= 54)
						{
							//16区至55区为一级汉字区，要加200分，注意row是以0为起始的，实际为15至54
							gbFreq += 200;
						}
					}
					//GBK扩展部分，高字节在0x81至0xFE之间，低字节有2个区间，一是0x80至0xFE之间，二是0x40至0x7E之间
					else if(bytes[i] >= (byte)0x81 && bytes[i] <= (byte)0xFE
						&& ((bytes[i + 1] >= (byte)0x80 && bytes[i + 1] <= (byte)0xFE) || (bytes[i + 1] >= (byte)0x40 && bytes[i + 1] <= (byte)0x7E)))
					{
						gbChars++;					//是一个GBK码字符(范围上匹配)
						totalFreq += 500;
						row = bytes[i] - 0x81;			//高字节减0x80得到区位码，注意要再减1得到以0起始的索引
						column = bytes[i + 1] - 0x40;	//低字节减0x3F得到区位码，注意要再减1得到以0起始的索引

						if(GBKFreq[row,column] != 0)
						{
							//命中GBKFreq中的元素，要加对应的分
							gbFreq += GBKFreq[row][column];
						}
					}
					++i;	//由于一次检查了2个字节，要把i再自增一次
				}
			}

			rangeVal = 50 * ((float)gbChars / (float)dbChars);		//范围分，0-50，看GB码字符占双字节字符的比例
			freqVal = 50 * ((float)gbFreq / (float)totalFreq);		//匹配分，0-50，看GB码字符的得分情况

			return (int)(rangeVal + freqVal);		//最终返回的可能性即上面两个得分的和
		}

		/// <summary>
		/// 检测gb18030的可能性
		/// </summary>
		/// <param name="bytes">字节数组</param>
		/// <returns>可能性(0-100)</returns>
		static int GB18030_Probability(byte bytes[])
		{
			/* gb18030特点：
			 * 变长编码，每个字符占1、2、4字节，向下兼容gb2312与gbk
			 * 一字节部分 0x00-0x7F，兼容ascii
			 * 两字节部分 首字节 0x81-0xFE，尾字节 0x40-0x7E 或 0x80-0xFE  与GBK基本兼容
			 * 四字节部分 一三字节 0x81-0xFE，二四字节 0x30-0x39
			 */

			int bytesLen = sizeof( bytes );
			int dbChars = 1, gbChars = 1;
			long gbFreq = 0, totalFreq = 1;
			float rangeVal = 0, freqVal = 0;
			int row, column;

			for(int i = 0; i < bytesLen - 1; i++)
			{
				if(bytes[i] >= 0x80)
				{
					dbChars++;
					if(bytes[i] >= (byte)0xA1 && bytes[i] <= (byte)0xF7 && // Original GB range
						(i + 1) < bytesLen && bytes[i + 1] >= (byte)0xA1 && bytes[i + 1] <= (byte)0xFE)
					{
						gbChars++;
						totalFreq += 500;
						row = bytes[i] - 0xA1;
						column = bytes[i + 1] - 0xA1;

						if(GBFreq[row][column] != 0)
						{
							gbFreq += GBFreq[row][column];
						}
						else if(row >= 15 && row < 55)
						{
							gbFreq += 200;
						}
					}
					else if(bytes[i] >= (byte)0x81 && bytes[i] <= (byte)0xFE
						&& (i + 1) < bytesLen && ((bytes[i + 1] >= (byte)0x80 && bytes[i + 1] <= (byte)0xFE) || (bytes[i + 1] >= (byte)0x40 && bytes[i + 1] <= (byte)0x7E)))
					{
						gbChars++;
						totalFreq += 500;
						row = bytes[i] - 0x81;
						column = bytes[i + 1] - 0x40;

						if(GBKFreq[row][column] != 0)
						{
							gbFreq += GBKFreq[row][column];
						}
					}
					else if(bytes[i] >= (byte)0x81 && bytes[i] <= (byte)0xFE
						&& (i + 3) < bytesLen && bytes[i + 1] >= (byte)0x30 && bytes[i + 1] <= (byte)0x39
						&& bytes[i + 2] >= (byte)0x81 && bytes[i + 2] <= (byte)0xFE && bytes[i + 3] >= (byte)0x30 && bytes[i + 3] <= (byte)0x39)
					{
						gbChars++;
					}
					++i;
				}
			}

			rangeVal = 50 * ((float)gbChars / (float)dbChars);
			freqVal = 50 * ((float)gbFreq / (float)totalFreq);

			return (int)(rangeVal + freqVal) - 1;
		}

		/// <summary>
		/// 检测big5的可能性
		/// </summary>
		/// <param name="bytes">字节数组</param>
		/// <returns>可能性(0-100)</returns>
		static int Big5_Probability(byte bytes[])
		{
			/* big5特点：
			 * 定长编码，每个字符占2字节，第一字节称为高字节，第二字节称为低字节，兼容ascii(单字节)
			 * 高字节在0xA0至0xFE之间，低字节有2个区间，一是0x40至0x7E之间，二是0xA1至0xFE之间
			 * 常用字位于A440-C67E，5401个，次常用字位于C940-F9D5，7652个
			 */

			int bytesLen = sizeof( bytes );
			int dbChars = 1, bfChars = 1;			//双字节字符数，Big5码字符数
			long bfFreq = 0, totalFreq = 1;
			float rangeVal = 0, freqVal = 0;
			int row, column;

			for(int i = 0; i < bytesLen - 1; ++i)
			{
				if(bytes[i] >= 0x80)				//小于0x80为ascii，不考虑，否则为双字节字符
				{
					dbChars++;

					//高字节在0xA1至0xF9之间，低字节在0x40至0x7E之间或0xA1至0xFE之间
					if(bytes[i] >= (byte)0xA1 && bytes[i] <= (byte)0xF9
						&& ((bytes[i + 1] >= (byte)0x40 && bytes[i + 1] <= (byte)0x7E) || (bytes[i + 1] >= (byte)0xA1 && bytes[i + 1] <= (byte)0xFE)))
					{
						bfChars++;					//是一个big5码字符(范围上匹配)
						totalFreq += 500;
						row = bytes[i] - 0xA1;		//高字节减0xA0得到区码，注意要再减1得到以0起始的索引

						//低字节要根据不同的范围减去0x3F或0x60，同样注意要再减1
						if(bytes[i + 1] >= 0x40 && bytes[i + 1] <= 0x7E)
						{
							column = bytes[i + 1] - 0x40;
						}
						else
						{
							column = bytes[i + 1] - 0x61;
						}

						if(Big5Freq[row][column] != 0)
						{
							//命中Big5Freq中的元素，要加对应的分
							bfFreq += Big5Freq[row][column];
						}
						else if(row >= 3 && row <= 37)
						{
							//3-37区为常用字(常用字位于A440-C67E)，加200分
							bfFreq += 200;
						}
					}
					++i;	//由于一次检查了2个字节，要把i再自增一次
				}
			}
			rangeVal = 50 * ((float)bfChars / (float)dbChars);		//范围分，0-50，看GB码字符占双字节字符的比例
			freqVal = 50 * ((float)bfFreq / (float)totalFreq);		//匹配分，0-50，看GB码字符的得分情况

			return (int)(rangeVal + freqVal);		//最终返回的可能性即上面两个得分的和
		}

		/// <summary>
		/// 初始化频率检查数组
		/// </summary>
		static void InitFreq()
		{
			for(int i = 0; i < 94; i++)
			{
				for(int j = 0; j < 94; j++)
					GBFreq[i][j] = 0;
			}

			for(int i = 0; i < 126; i++)
			{
				for(int j = 0; j < 191; j++)
					GBKFreq[i][j] = 0;
			}

			for(int i = 0; i < 94; i++)
			{
				for(int j = 0; j < 158; j++)
					Big5Freq[i][j] = 0;
			}

			GBFreq[20][35] = 599;			GBFreq[49][26] = 598;			GBFreq[41][38] = 597;
			GBFreq[17][26] = 596;			GBFreq[32][42] = 595;			GBFreq[39][42] = 594;
			GBFreq[45][49] = 593;			GBFreq[51][57] = 592;			GBFreq[50][47] = 591;
			GBFreq[42][90] = 590;			GBFreq[52][65] = 589;			GBFreq[53][47] = 588;
			GBFreq[19][82] = 587;			GBFreq[31][19] = 586;			GBFreq[40][46] = 585;
			GBFreq[24][89] = 584;			GBFreq[23][85] = 583;			GBFreq[20][28] = 582;
			GBFreq[42][20] = 581;			GBFreq[34][38] = 580;			GBFreq[45][9] = 579;
			GBFreq[54][50] = 578;			GBFreq[25][44] = 577;			GBFreq[35][66] = 576;
			GBFreq[20][55] = 575;			GBFreq[18][85] = 574;			GBFreq[20][31] = 573;
			GBFreq[49][17] = 572;			GBFreq[41][16] = 571;			GBFreq[35][73] = 570;
			GBFreq[20][34] = 569;			GBFreq[29][44] = 568;			GBFreq[35][38] = 567;
			GBFreq[49][9] = 566;			GBFreq[46][33] = 565;			GBFreq[49][51] = 564;
			GBFreq[40][89] = 563;			GBFreq[26][64] = 562;			GBFreq[54][51] = 561;
			GBFreq[54][36] = 560;			GBFreq[39][4] = 559;			GBFreq[53][13] = 558;
			GBFreq[24][92] = 557;			GBFreq[27][49] = 556;			GBFreq[48][6] = 555;
			GBFreq[21][51] = 554;			GBFreq[30][40] = 553;			GBFreq[42][92] = 552;
			GBFreq[31][78] = 551;			GBFreq[25][82] = 550;			GBFreq[47][0] = 549;
			GBFreq[34][19] = 548;			GBFreq[47][35] = 547;			GBFreq[21][63] = 546;
			GBFreq[43][75] = 545;			GBFreq[21][87] = 544;			GBFreq[35][59] = 543;
			GBFreq[25][34] = 542;			GBFreq[21][27] = 541;			GBFreq[39][26] = 540;
			GBFreq[34][26] = 539;			GBFreq[39][52] = 538;			GBFreq[50][57] = 537;
			GBFreq[37][79] = 536;			GBFreq[26][24] = 535;			GBFreq[22][1] = 534;
			GBFreq[18][40] = 533;			GBFreq[41][33] = 532;			GBFreq[53][26] = 531;
			GBFreq[54][86] = 530;			GBFreq[20][16] = 529;			GBFreq[46][74] = 528;
			GBFreq[30][19] = 527;			GBFreq[45][35] = 526;			GBFreq[45][61] = 525;
			GBFreq[30][9] = 524;			GBFreq[41][53] = 523;			GBFreq[41][13] = 522;
			GBFreq[50][34] = 521;			GBFreq[53][86] = 520;			GBFreq[47][47] = 519;
			GBFreq[22][28] = 518;			GBFreq[50][53] = 517;			GBFreq[39][70] = 516;
			GBFreq[38][15] = 515;			GBFreq[42][88] = 514;			GBFreq[16][29] = 513;
			GBFreq[27][90] = 512;			GBFreq[29][12] = 511;			GBFreq[44][22] = 510;
			GBFreq[34][69] = 509;			GBFreq[24][10] = 508;			GBFreq[44][11] = 507;
			GBFreq[39][92] = 506;			GBFreq[49][48] = 505;			GBFreq[31][46] = 504;
			GBFreq[19][50] = 503;			GBFreq[21][14] = 502;			GBFreq[32][28] = 501;
			GBFreq[18][3] = 500;			GBFreq[53][9] = 499;			GBFreq[34][80] = 498;
			GBFreq[48][88] = 497;			GBFreq[46][53] = 496;			GBFreq[22][53] = 495;
			GBFreq[28][10] = 494;			GBFreq[44][65] = 493;			GBFreq[20][10] = 492;
			GBFreq[40][76] = 491;			GBFreq[47][8] = 490;			GBFreq[50][74] = 489;
			GBFreq[23][62] = 488;			GBFreq[49][65] = 487;			GBFreq[28][87] = 486;
			GBFreq[15][48] = 485;			GBFreq[22][7] = 484;			GBFreq[19][42] = 483;
			GBFreq[41][20] = 482;			GBFreq[26][55] = 481;			GBFreq[21][93] = 480;
			GBFreq[31][76] = 479;			GBFreq[34][31] = 478;			GBFreq[20][66] = 477;
			GBFreq[51][33] = 476;			GBFreq[34][86] = 475;			GBFreq[37][67] = 474;
			GBFreq[53][53] = 473;			GBFreq[40][88] = 472;			GBFreq[39][10] = 471;
			GBFreq[24][3] = 470;			GBFreq[27][25] = 469;			GBFreq[26][15] = 468;
			GBFreq[21][88] = 467;			GBFreq[52][62] = 466;			GBFreq[46][81] = 465;
			GBFreq[38][72] = 464;			GBFreq[17][30] = 463;			GBFreq[52][92] = 462;
			GBFreq[34][90] = 461;			GBFreq[21][7] = 460;			GBFreq[36][13] = 459;
			GBFreq[45][41] = 458;			GBFreq[32][5] = 457;			GBFreq[26][89] = 456;
			GBFreq[23][87] = 455;			GBFreq[20][39] = 454;			GBFreq[27][23] = 453;
			GBFreq[25][59] = 452;			GBFreq[49][20] = 451;			GBFreq[54][77] = 450;
			GBFreq[27][67] = 449;			GBFreq[47][33] = 448;			GBFreq[41][17] = 447;
			GBFreq[19][81] = 446;			GBFreq[16][66] = 445;			GBFreq[45][26] = 444;
			GBFreq[49][81] = 443;			GBFreq[53][55] = 442;			GBFreq[16][26] = 441;
			GBFreq[54][62] = 440;			GBFreq[20][70] = 439;			GBFreq[42][35] = 438;
			GBFreq[20][57] = 437;			GBFreq[34][36] = 436;			GBFreq[46][63] = 435;
			GBFreq[19][45] = 434;			GBFreq[21][10] = 433;			GBFreq[52][93] = 432;
			GBFreq[25][2] = 431;			GBFreq[30][57] = 430;			GBFreq[41][24] = 429;
			GBFreq[28][43] = 428;			GBFreq[45][86] = 427;			GBFreq[51][56] = 426;
			GBFreq[37][28] = 425;			GBFreq[52][69] = 424;			GBFreq[43][92] = 423;
			GBFreq[41][31] = 422;			GBFreq[37][87] = 421;			GBFreq[47][36] = 420;
			GBFreq[16][16] = 419;			GBFreq[40][56] = 418;			GBFreq[24][55] = 417;
			GBFreq[17][1] = 416;			GBFreq[35][57] = 415;			GBFreq[27][50] = 414;
			GBFreq[26][14] = 413;			GBFreq[50][40] = 412;			GBFreq[39][19] = 411;
			GBFreq[19][89] = 410;			GBFreq[29][91] = 409;			GBFreq[17][89] = 408;
			GBFreq[39][74] = 407;			GBFreq[46][39] = 406;			GBFreq[40][28] = 405;
			GBFreq[45][68] = 404;			GBFreq[43][10] = 403;			GBFreq[42][13] = 402;
			GBFreq[44][81] = 401;			GBFreq[41][47] = 400;			GBFreq[48][58] = 399;
			GBFreq[43][68] = 398;			GBFreq[16][79] = 397;			GBFreq[19][5] = 396;
			GBFreq[54][59] = 395;			GBFreq[17][36] = 394;			GBFreq[18][0] = 393;
			GBFreq[41][5] = 392;			GBFreq[41][72] = 391;			GBFreq[16][39] = 390;
			GBFreq[54][0] = 389;			GBFreq[51][16] = 388;			GBFreq[29][36] = 387;
			GBFreq[47][5] = 386;			GBFreq[47][51] = 385;			GBFreq[44][7] = 384;
			GBFreq[35][30] = 383;			GBFreq[26][9] = 382;			GBFreq[16][7] = 381;
			GBFreq[32][1] = 380;			GBFreq[33][76] = 379;			GBFreq[34][91] = 378;
			GBFreq[52][36] = 377;			GBFreq[26][77] = 376;			GBFreq[35][48] = 375;
			GBFreq[40][80] = 374;			GBFreq[41][92] = 373;			GBFreq[27][93] = 372;
			GBFreq[15][17] = 371;			GBFreq[16][76] = 370;			GBFreq[51][12] = 369;
			GBFreq[18][20] = 368;			GBFreq[15][54] = 367;			GBFreq[50][5] = 366;
			GBFreq[33][22] = 365;			GBFreq[37][57] = 364;			GBFreq[28][47] = 363;
			GBFreq[42][31] = 362;			GBFreq[18][2] = 361;			GBFreq[43][64] = 360;
			GBFreq[23][47] = 359;			GBFreq[28][79] = 358;			GBFreq[25][45] = 357;
			GBFreq[23][91] = 356;			GBFreq[22][19] = 355;			GBFreq[25][46] = 354;
			GBFreq[22][36] = 353;			GBFreq[54][85] = 352;			GBFreq[46][20] = 351;
			GBFreq[27][37] = 350;			GBFreq[26][81] = 349;			GBFreq[42][29] = 348;
			GBFreq[31][90] = 347;			GBFreq[41][59] = 346;			GBFreq[24][65] = 345;
			GBFreq[44][84] = 344;			GBFreq[24][90] = 343;			GBFreq[38][54] = 342;
			GBFreq[28][70] = 341;			GBFreq[27][15] = 340;			GBFreq[28][80] = 339;
			GBFreq[29][8] = 338;			GBFreq[45][80] = 337;			GBFreq[53][37] = 336;
			GBFreq[28][65] = 335;			GBFreq[23][86] = 334;			GBFreq[39][45] = 333;
			GBFreq[53][32] = 332;			GBFreq[38][68] = 331;			GBFreq[45][78] = 330;
			GBFreq[43][7] = 329;			GBFreq[46][82] = 328;			GBFreq[27][38] = 327;
			GBFreq[16][62] = 326;			GBFreq[24][17] = 325;			GBFreq[22][70] = 324;
			GBFreq[52][28] = 323;			GBFreq[23][40] = 322;			GBFreq[28][50] = 321;
			GBFreq[42][91] = 320;			GBFreq[47][76] = 319;			GBFreq[15][42] = 318;
			GBFreq[43][55] = 317;			GBFreq[29][84] = 316;			GBFreq[44][90] = 315;
			GBFreq[53][16] = 314;			GBFreq[22][93] = 313;			GBFreq[34][10] = 312;
			GBFreq[32][53] = 311;			GBFreq[43][65] = 310;			GBFreq[28][7] = 309;
			GBFreq[35][46] = 308;			GBFreq[21][39] = 307;			GBFreq[44][18] = 306;
			GBFreq[40][10] = 305;			GBFreq[54][53] = 304;			GBFreq[38][74] = 303;
			GBFreq[28][26] = 302;			GBFreq[15][13] = 301;			GBFreq[39][34] = 300;
			GBFreq[39][46] = 299;			GBFreq[42][66] = 298;			GBFreq[33][58] = 297;
			GBFreq[15][56] = 296;			GBFreq[18][51] = 295;			GBFreq[49][68] = 294;
			GBFreq[30][37] = 293;			GBFreq[51][84] = 292;			GBFreq[51][9] = 291;
			GBFreq[40][70] = 290;			GBFreq[41][84] = 289;			GBFreq[28][64] = 288;
			GBFreq[32][88] = 287;			GBFreq[24][5] = 286;			GBFreq[53][23] = 285;
			GBFreq[42][27] = 284;			GBFreq[22][38] = 283;			GBFreq[32][86] = 282;
			GBFreq[34][30] = 281;			GBFreq[38][63] = 280;			GBFreq[24][59] = 279;
			GBFreq[22][81] = 278;			GBFreq[32][11] = 277;			GBFreq[51][21] = 276;
			GBFreq[54][41] = 275;			GBFreq[21][50] = 274;			GBFreq[23][89] = 273;
			GBFreq[19][87] = 272;			GBFreq[26][7] = 271;			GBFreq[30][75] = 270;
			GBFreq[43][84] = 269;			GBFreq[51][25] = 268;			GBFreq[16][67] = 267;
			GBFreq[32][9] = 266;			GBFreq[48][51] = 265;			GBFreq[39][7] = 264;
			GBFreq[44][88] = 263;			GBFreq[52][24] = 262;			GBFreq[23][34] = 261;
			GBFreq[32][75] = 260;			GBFreq[19][10] = 259;			GBFreq[28][91] = 258;
			GBFreq[32][83] = 257;			GBFreq[25][75] = 256;			GBFreq[53][45] = 255;
			GBFreq[29][85] = 254;			GBFreq[53][59] = 253;			GBFreq[16][2] = 252;
			GBFreq[19][78] = 251;			GBFreq[15][75] = 250;			GBFreq[51][42] = 249;
			GBFreq[45][67] = 248;			GBFreq[15][74] = 247;			GBFreq[25][81] = 246;
			GBFreq[37][62] = 245;			GBFreq[16][55] = 244;			GBFreq[18][38] = 243;
			GBFreq[23][23] = 242;			GBFreq[38][30] = 241;			GBFreq[17][28] = 240;
			GBFreq[44][73] = 239;			GBFreq[23][78] = 238;			GBFreq[40][77] = 237;
			GBFreq[38][87] = 236;			GBFreq[27][19] = 235;			GBFreq[38][82] = 234;
			GBFreq[37][22] = 233;			GBFreq[41][30] = 232;			GBFreq[54][9] = 231;
			GBFreq[32][30] = 230;			GBFreq[30][52] = 229;			GBFreq[40][84] = 228;
			GBFreq[53][57] = 227;			GBFreq[27][27] = 226;			GBFreq[38][64] = 225;
			GBFreq[18][43] = 224;			GBFreq[23][69] = 223;			GBFreq[28][12] = 222;
			GBFreq[50][78] = 221;			GBFreq[50][1] = 220;			GBFreq[26][88] = 219;
			GBFreq[36][40] = 218;			GBFreq[33][89] = 217;			GBFreq[41][28] = 216;
			GBFreq[31][77] = 215;			GBFreq[46][1] = 214;			GBFreq[47][19] = 213;
			GBFreq[35][55] = 212;			GBFreq[41][21] = 211;			GBFreq[27][10] = 210;
			GBFreq[32][77] = 209;			GBFreq[26][37] = 208;			GBFreq[20][33] = 207;
			GBFreq[41][52] = 206;			GBFreq[32][18] = 205;			GBFreq[38][13] = 204;
			GBFreq[20][18] = 203;			GBFreq[20][24] = 202;			GBFreq[45][19] = 201;
			GBFreq[18][53] = 200;

			GBKFreq[52][132] = 600;			GBKFreq[73][135] = 599;			GBKFreq[49][123] = 598;
			GBKFreq[77][146] = 597;			GBKFreq[81][123] = 596;			GBKFreq[82][144] = 595;
			GBKFreq[51][179] = 594;			GBKFreq[83][154] = 593;			GBKFreq[71][139] = 592;
			GBKFreq[64][139] = 591;			GBKFreq[85][144] = 590;			GBKFreq[52][125] = 589;
			GBKFreq[88][25] = 588;			GBKFreq[81][106] = 587;			GBKFreq[81][148] = 586;
			GBKFreq[62][137] = 585;			GBKFreq[94][0] = 584;			GBKFreq[1][64] = 583;
			GBKFreq[67][163] = 582;			GBKFreq[20][190] = 581;			GBKFreq[57][131] = 580;
			GBKFreq[29][169] = 579;			GBKFreq[72][143] = 578;			GBKFreq[0][173] = 577;
			GBKFreq[11][23] = 576;			GBKFreq[61][141] = 575;			GBKFreq[60][123] = 574;
			GBKFreq[81][114] = 573;			GBKFreq[82][131] = 572;			GBKFreq[67][156] = 571;
			GBKFreq[71][167] = 570;			GBKFreq[20][50] = 569;			GBKFreq[77][132] = 568;
			GBKFreq[84][38] = 567;			GBKFreq[26][29] = 566;			GBKFreq[74][187] = 565;
			GBKFreq[62][116] = 564;			GBKFreq[67][135] = 563;			GBKFreq[5][86] = 562;
			GBKFreq[72][186] = 561;			GBKFreq[75][161] = 560;			GBKFreq[78][130] = 559;
			GBKFreq[94][30] = 558;			GBKFreq[84][72] = 557;			GBKFreq[1][67] = 556;
			GBKFreq[75][172] = 555;			GBKFreq[74][185] = 554;			GBKFreq[53][160] = 553;
			GBKFreq[123][14] = 552;			GBKFreq[79][97] = 551;			GBKFreq[85][110] = 550;
			GBKFreq[78][171] = 549;			GBKFreq[52][131] = 548;			GBKFreq[56][100] = 547;
			GBKFreq[50][182] = 546;			GBKFreq[94][64] = 545;			GBKFreq[106][74] = 544;
			GBKFreq[11][102] = 543;			GBKFreq[53][124] = 542;			GBKFreq[24][3] = 541;
			GBKFreq[86][148] = 540;			GBKFreq[53][184] = 539;			GBKFreq[86][147] = 538;
			GBKFreq[96][161] = 537;			GBKFreq[82][77] = 536;			GBKFreq[59][146] = 535;
			GBKFreq[84][126] = 534;			GBKFreq[79][132] = 533;			GBKFreq[85][123] = 532;
			GBKFreq[71][101] = 531;			GBKFreq[85][106] = 530;			GBKFreq[6][184] = 529;
			GBKFreq[57][156] = 528;			GBKFreq[75][104] = 527;			GBKFreq[50][137] = 526;
			GBKFreq[79][133] = 525;			GBKFreq[76][108] = 524;			GBKFreq[57][142] = 523;
			GBKFreq[84][130] = 522;			GBKFreq[52][128] = 521;			GBKFreq[47][44] = 520;
			GBKFreq[52][152] = 519;			GBKFreq[54][104] = 518;			GBKFreq[30][47] = 517;
			GBKFreq[71][123] = 516;			GBKFreq[52][107] = 515;			GBKFreq[45][84] = 514;
			GBKFreq[107][118] = 513;		GBKFreq[5][161] = 512;			GBKFreq[48][126] = 511;
			GBKFreq[67][170] = 510;			GBKFreq[43][6] = 509;			GBKFreq[70][112] = 508;
			GBKFreq[86][174] = 507;			GBKFreq[84][166] = 506;			GBKFreq[79][130] = 505;
			GBKFreq[57][141] = 504;			GBKFreq[81][178] = 503;			GBKFreq[56][187] = 502;
			GBKFreq[81][162] = 501;			GBKFreq[53][104] = 500;			GBKFreq[123][35] = 499;
			GBKFreq[70][169] = 498;			GBKFreq[69][164] = 497;			GBKFreq[109][61] = 496;
			GBKFreq[73][130] = 495;			GBKFreq[62][134] = 494;			GBKFreq[54][125] = 493;
			GBKFreq[79][105] = 492;			GBKFreq[70][165] = 491;			GBKFreq[71][189] = 490;
			GBKFreq[23][147] = 489;			GBKFreq[51][139] = 488;			GBKFreq[47][137] = 487;
			GBKFreq[77][123] = 486;			GBKFreq[86][183] = 485;			GBKFreq[63][173] = 484;
			GBKFreq[79][144] = 483;			GBKFreq[84][159] = 482;			GBKFreq[60][91] = 481;
			GBKFreq[66][187] = 480;			GBKFreq[73][114] = 479;			GBKFreq[85][56] = 478;
			GBKFreq[71][149] = 477;			GBKFreq[84][189] = 476;			GBKFreq[104][31] = 475;
			GBKFreq[83][82] = 474;			GBKFreq[68][35] = 473;			GBKFreq[11][77] = 472;
			GBKFreq[15][155] = 471;			GBKFreq[83][153] = 470;			GBKFreq[71][1] = 469;
			GBKFreq[53][190] = 468;			GBKFreq[50][135] = 467;			GBKFreq[3][147] = 466;
			GBKFreq[48][136] = 465;			GBKFreq[66][166] = 464;			GBKFreq[55][159] = 463;
			GBKFreq[82][150] = 462;			GBKFreq[58][178] = 461;			GBKFreq[64][102] = 460;
			GBKFreq[16][106] = 459;			GBKFreq[68][110] = 458;			GBKFreq[54][14] = 457;
			GBKFreq[60][140] = 456;			GBKFreq[91][71] = 455;			GBKFreq[54][150] = 454;
			GBKFreq[78][177] = 453;			GBKFreq[78][117] = 452;			GBKFreq[104][12] = 451;
			GBKFreq[73][150] = 450;			GBKFreq[51][142] = 449;			GBKFreq[81][145] = 448;
			GBKFreq[66][183] = 447;			GBKFreq[51][178] = 446;			GBKFreq[75][107] = 445;
			GBKFreq[65][119] = 444;			GBKFreq[69][176] = 443;			GBKFreq[59][122] = 442;
			GBKFreq[78][160] = 441;			GBKFreq[85][183] = 440;			GBKFreq[105][16] = 439;
			GBKFreq[73][110] = 438;			GBKFreq[104][39] = 437;			GBKFreq[119][16] = 436;
			GBKFreq[76][162] = 435;			GBKFreq[67][152] = 434;			GBKFreq[82][24] = 433;
			GBKFreq[73][121] = 432;			GBKFreq[83][83] = 431;			GBKFreq[82][145] = 430;
			GBKFreq[49][133] = 429;			GBKFreq[94][13] = 428;			GBKFreq[58][139] = 427;
			GBKFreq[74][189] = 426;			GBKFreq[66][177] = 425;			GBKFreq[85][184] = 424;
			GBKFreq[55][183] = 423;			GBKFreq[71][107] = 422;			GBKFreq[11][98] = 421;
			GBKFreq[72][153] = 420;			GBKFreq[2][137] = 419;			GBKFreq[59][147] = 418;
			GBKFreq[58][152] = 417;			GBKFreq[55][144] = 416;			GBKFreq[73][125] = 415;
			GBKFreq[52][154] = 414;			GBKFreq[70][178] = 413;			GBKFreq[79][148] = 412;
			GBKFreq[63][143] = 411;			GBKFreq[50][140] = 410;			GBKFreq[47][145] = 409;
			GBKFreq[48][123] = 408;			GBKFreq[56][107] = 407;			GBKFreq[84][83] = 406;
			GBKFreq[59][112] = 405;			GBKFreq[124][72] = 404;			GBKFreq[79][99] = 403;
			GBKFreq[3][37] = 402;			GBKFreq[114][55] = 401;			GBKFreq[85][152] = 400;
			GBKFreq[60][47] = 399;			GBKFreq[65][96] = 398;			GBKFreq[74][110] = 397;
			GBKFreq[86][182] = 396;			GBKFreq[50][99] = 395;			GBKFreq[67][186] = 394;
			GBKFreq[81][74] = 393;			GBKFreq[80][37] = 392;			GBKFreq[21][60] = 391;
			GBKFreq[110][12] = 390;			GBKFreq[60][162] = 389;			GBKFreq[29][115] = 388;
			GBKFreq[83][130] = 387;			GBKFreq[52][136] = 386;			GBKFreq[63][114] = 385;
			GBKFreq[49][127] = 384;			GBKFreq[83][109] = 383;			GBKFreq[66][128] = 382;
			GBKFreq[78][136] = 381;			GBKFreq[81][180] = 380;			GBKFreq[76][104] = 379;
			GBKFreq[56][156] = 378;			GBKFreq[61][23] = 377;			GBKFreq[4][30] = 376;
			GBKFreq[69][154] = 375;			GBKFreq[100][37] = 374;			GBKFreq[54][177] = 373;
			GBKFreq[23][119] = 372;			GBKFreq[71][171] = 371;			GBKFreq[84][146] = 370;
			GBKFreq[20][184] = 369;			GBKFreq[86][76] = 368;			GBKFreq[74][132] = 367;
			GBKFreq[47][97] = 366;			GBKFreq[82][137] = 365;			GBKFreq[94][56] = 364;
			GBKFreq[92][30] = 363;			GBKFreq[19][117] = 362;			GBKFreq[48][173] = 361;
			GBKFreq[2][136] = 360;			GBKFreq[7][182] = 359;			GBKFreq[74][188] = 358;
			GBKFreq[14][132] = 357;			GBKFreq[62][172] = 356;			GBKFreq[25][39] = 355;
			GBKFreq[85][129] = 354;			GBKFreq[64][98] = 353;			GBKFreq[67][127] = 352;
			GBKFreq[72][167] = 351;			GBKFreq[57][143] = 350;			GBKFreq[76][187] = 349;
			GBKFreq[83][181] = 348;			GBKFreq[84][10] = 347;			GBKFreq[55][166] = 346;
			GBKFreq[55][188] = 345;			GBKFreq[13][151] = 344;			GBKFreq[62][124] = 343;
			GBKFreq[53][136] = 342;			GBKFreq[106][57] = 341;			GBKFreq[47][166] = 340;
			GBKFreq[109][30] = 339;			GBKFreq[78][114] = 338;			GBKFreq[83][19] = 337;
			GBKFreq[56][162] = 336;			GBKFreq[60][177] = 335;			GBKFreq[88][9] = 334;
			GBKFreq[74][163] = 333;			GBKFreq[52][156] = 332;			GBKFreq[71][180] = 331;
			GBKFreq[60][57] = 330;			GBKFreq[72][173] = 329;			GBKFreq[82][91] = 328;
			GBKFreq[51][186] = 327;			GBKFreq[75][86] = 326;			GBKFreq[75][78] = 325;
			GBKFreq[76][170] = 324;			GBKFreq[60][147] = 323;			GBKFreq[82][75] = 322;
			GBKFreq[80][148] = 321;			GBKFreq[86][150] = 320;			GBKFreq[13][95] = 319;
			GBKFreq[0][11] = 318;			GBKFreq[84][190] = 317;			GBKFreq[76][166] = 316;
			GBKFreq[14][72] = 315;			GBKFreq[67][144] = 314;			GBKFreq[84][44] = 313;
			GBKFreq[72][125] = 312;			GBKFreq[66][127] = 311;			GBKFreq[60][25] = 310;
			GBKFreq[70][146] = 309;			GBKFreq[79][135] = 308;			GBKFreq[54][135] = 307;
			GBKFreq[60][104] = 306;			GBKFreq[55][132] = 305;			GBKFreq[94][2] = 304;
			GBKFreq[54][133] = 303;			GBKFreq[56][190] = 302;			GBKFreq[58][174] = 301;
			GBKFreq[80][144] = 300;			GBKFreq[85][113] = 299;

			Big5Freq[9][89] = 600;			Big5Freq[11][15] = 599;			Big5Freq[3][66] = 598;
			Big5Freq[6][121] = 597;			Big5Freq[3][0] = 596;			Big5Freq[5][82] = 595;
			Big5Freq[3][42] = 594;			Big5Freq[5][34] = 593;			Big5Freq[3][8] = 592;
			Big5Freq[3][6] = 591;			Big5Freq[3][67] = 590;			Big5Freq[7][139] = 589;
			Big5Freq[23][137] = 588;		Big5Freq[12][46] = 587;			Big5Freq[4][8] = 586;
			Big5Freq[4][41] = 585;			Big5Freq[18][47] = 584;			Big5Freq[12][114] = 583;
			Big5Freq[6][1] = 582;			Big5Freq[22][60] = 581;			Big5Freq[5][46] = 580;
			Big5Freq[11][79] = 579;			Big5Freq[3][23] = 578;			Big5Freq[7][114] = 577;
			Big5Freq[29][102] = 576;		Big5Freq[19][14] = 575;			Big5Freq[4][133] = 574;
			Big5Freq[3][29] = 573;			Big5Freq[4][109] = 572;			Big5Freq[14][127] = 571;
			Big5Freq[5][48] = 570;			Big5Freq[13][104] = 569;		Big5Freq[3][132] = 568;
			Big5Freq[26][64] = 567;			Big5Freq[7][19] = 566;			Big5Freq[4][12] = 565;
			Big5Freq[11][124] = 564;		Big5Freq[7][89] = 563;			Big5Freq[15][124] = 562;
			Big5Freq[4][108] = 561;			Big5Freq[19][66] = 560;			Big5Freq[3][21] = 559;
			Big5Freq[24][12] = 558;			Big5Freq[28][111] = 557;		Big5Freq[12][107] = 556;
			Big5Freq[3][112] = 555;			Big5Freq[8][113] = 554;			Big5Freq[5][40] = 553;
			Big5Freq[26][145] = 552;		Big5Freq[3][48] = 551;			Big5Freq[3][70] = 550;
			Big5Freq[22][17] = 549;			Big5Freq[16][47] = 548;			Big5Freq[3][53] = 547;
			Big5Freq[4][24] = 546;			Big5Freq[32][120] = 545;		Big5Freq[24][49] = 544;
			Big5Freq[24][142] = 543;		Big5Freq[18][66] = 542;			Big5Freq[29][150] = 541;
			Big5Freq[5][122] = 540;			Big5Freq[5][114] = 539;			Big5Freq[3][44] = 538;
			Big5Freq[10][128] = 537;		Big5Freq[15][20] = 536;			Big5Freq[13][33] = 535;
			Big5Freq[14][87] = 534;			Big5Freq[3][126] = 533;			Big5Freq[4][53] = 532;
			Big5Freq[4][40] = 531;			Big5Freq[9][93] = 530;			Big5Freq[15][137] = 529;
			Big5Freq[10][123] = 528;		Big5Freq[4][56] = 527;			Big5Freq[5][71] = 526;
			Big5Freq[10][8] = 525;			Big5Freq[5][16] = 524;			Big5Freq[5][146] = 523;
			Big5Freq[18][88] = 522;			Big5Freq[24][4] = 521;			Big5Freq[20][47] = 520;
			Big5Freq[5][33] = 519;			Big5Freq[9][43] = 518;			Big5Freq[20][12] = 517;
			Big5Freq[20][13] = 516;			Big5Freq[5][156] = 515;			Big5Freq[22][140] = 514;
			Big5Freq[8][146] = 513;			Big5Freq[21][123] = 512;		Big5Freq[4][90] = 511;
			Big5Freq[5][62] = 510;			Big5Freq[17][59] = 509;			Big5Freq[10][37] = 508;
			Big5Freq[18][107] = 507;		Big5Freq[14][53] = 506;			Big5Freq[22][51] = 505;
			Big5Freq[8][13] = 504;			Big5Freq[5][29] = 503;			Big5Freq[9][7] = 502;
			Big5Freq[22][14] = 501;			Big5Freq[8][55] = 500;			Big5Freq[33][9] = 499;
			Big5Freq[16][64] = 498;			Big5Freq[7][131] = 497;			Big5Freq[34][4] = 496;
			Big5Freq[7][101] = 495;			Big5Freq[11][139] = 494;		Big5Freq[3][135] = 493;
			Big5Freq[7][102] = 492;			Big5Freq[17][13] = 491;			Big5Freq[3][20] = 490;
			Big5Freq[27][106] = 489;		Big5Freq[5][88] = 488;			Big5Freq[6][33] = 487;
			Big5Freq[5][139] = 486;			Big5Freq[6][0] = 485;			Big5Freq[17][58] = 484;
			Big5Freq[5][133] = 483;			Big5Freq[9][107] = 482;			Big5Freq[23][39] = 481;
			Big5Freq[5][23] = 480;			Big5Freq[3][79] = 479;			Big5Freq[32][97] = 478;
			Big5Freq[3][136] = 477;			Big5Freq[4][94] = 476;			Big5Freq[21][61] = 475;
			Big5Freq[23][123] = 474;		Big5Freq[26][16] = 473;			Big5Freq[24][137] = 472;
			Big5Freq[22][18] = 471;			Big5Freq[5][1] = 470;			Big5Freq[20][119] = 469;
			Big5Freq[3][7] = 468;			Big5Freq[10][79] = 467;			Big5Freq[15][105] = 466;
			Big5Freq[3][144] = 465;			Big5Freq[12][80] = 464;			Big5Freq[15][73] = 463;
			Big5Freq[3][19] = 462;			Big5Freq[8][109] = 461;			Big5Freq[3][15] = 460;
			Big5Freq[31][82] = 459;			Big5Freq[3][43] = 458;			Big5Freq[25][119] = 457;
			Big5Freq[16][111] = 456;		Big5Freq[7][77] = 455;			Big5Freq[3][95] = 454;
			Big5Freq[24][82] = 453;			Big5Freq[7][52] = 452;			Big5Freq[9][151] = 451;
			Big5Freq[3][129] = 450;			Big5Freq[5][87] = 449;			Big5Freq[3][55] = 448;
			Big5Freq[8][153] = 447;			Big5Freq[4][83] = 446;			Big5Freq[3][114] = 445;
			Big5Freq[23][147] = 444;		Big5Freq[15][31] = 443;			Big5Freq[3][54] = 442;
			Big5Freq[11][122] = 441;		Big5Freq[4][4] = 440;			Big5Freq[34][149] = 439;
			Big5Freq[3][17] = 438;			Big5Freq[21][64] = 437;			Big5Freq[26][144] = 436;
			Big5Freq[4][62] = 435;			Big5Freq[8][15] = 434;			Big5Freq[35][80] = 433;
			Big5Freq[7][110] = 432;			Big5Freq[23][114] = 431;		Big5Freq[3][108] = 430;
			Big5Freq[3][62] = 429;			Big5Freq[21][41] = 428;			Big5Freq[15][99] = 427;
			Big5Freq[5][47] = 426;			Big5Freq[4][96] = 425;			Big5Freq[20][122] = 424;
			Big5Freq[5][21] = 423;			Big5Freq[4][157] = 422;			Big5Freq[16][14] = 421;
			Big5Freq[3][117] = 420;			Big5Freq[7][129] = 419;			Big5Freq[4][27] = 418;
			Big5Freq[5][30] = 417;			Big5Freq[22][16] = 416;			Big5Freq[5][64] = 415;
			Big5Freq[17][99] = 414;			Big5Freq[17][57] = 413;			Big5Freq[8][105] = 412;
			Big5Freq[5][112] = 411;			Big5Freq[20][59] = 410;			Big5Freq[6][129] = 409;
			Big5Freq[18][17] = 408;			Big5Freq[3][92] = 407;			Big5Freq[28][118] = 406;
			Big5Freq[3][109] = 405;			Big5Freq[31][51] = 404;			Big5Freq[13][116] = 403;
			Big5Freq[6][15] = 402;			Big5Freq[36][136] = 401;		Big5Freq[12][74] = 400;
			Big5Freq[20][88] = 399;			Big5Freq[36][68] = 398;			Big5Freq[3][147] = 397;
			Big5Freq[15][84] = 396;			Big5Freq[16][32] = 395;			Big5Freq[16][58] = 394;
			Big5Freq[7][66] = 393;			Big5Freq[23][107] = 392;		Big5Freq[9][6] = 391;
			Big5Freq[12][86] = 390;			Big5Freq[23][112] = 389;		Big5Freq[37][23] = 388;
			Big5Freq[3][138] = 387;			Big5Freq[20][68] = 386;			Big5Freq[15][116] = 385;
			Big5Freq[18][64] = 384;			Big5Freq[12][139] = 383;		Big5Freq[11][155] = 382;
			Big5Freq[4][156] = 381;			Big5Freq[12][84] = 380;			Big5Freq[18][49] = 379;
			Big5Freq[25][125] = 378;		Big5Freq[25][147] = 377;		Big5Freq[15][110] = 376;
			Big5Freq[19][96] = 375;			Big5Freq[30][152] = 374;		Big5Freq[6][31] = 373;
			Big5Freq[27][117] = 372;		Big5Freq[3][10] = 371;			Big5Freq[6][131] = 370;
			Big5Freq[13][112] = 369;		Big5Freq[36][156] = 368;		Big5Freq[4][60] = 367;
			Big5Freq[15][121] = 366;		Big5Freq[4][112] = 365;			Big5Freq[30][142] = 364;
			Big5Freq[23][154] = 363;		Big5Freq[27][101] = 362;		Big5Freq[9][140] = 361;
			Big5Freq[3][89] = 360;			Big5Freq[18][148] = 359;		Big5Freq[4][69] = 358;
			Big5Freq[16][49] = 357;			Big5Freq[6][117] = 356;			Big5Freq[36][55] = 355;
			Big5Freq[5][123] = 354;			Big5Freq[4][126] = 353;			Big5Freq[4][119] = 352;
			Big5Freq[9][95] = 351;			Big5Freq[5][24] = 350;			Big5Freq[16][133] = 349;
			Big5Freq[10][134] = 348;		Big5Freq[26][59] = 347;			Big5Freq[6][41] = 346;
			Big5Freq[6][146] = 345;			Big5Freq[19][24] = 344;			Big5Freq[5][113] = 343;
			Big5Freq[10][118] = 342;		Big5Freq[34][151] = 341;		Big5Freq[9][72] = 340;
			Big5Freq[31][25] = 339;			Big5Freq[18][126] = 338;		Big5Freq[18][28] = 337;
			Big5Freq[4][153] = 336;			Big5Freq[3][84] = 335;			Big5Freq[21][18] = 334;
			Big5Freq[25][129] = 333;		Big5Freq[6][107] = 332;			Big5Freq[12][25] = 331;
			Big5Freq[17][109] = 330;		Big5Freq[7][76] = 329;			Big5Freq[15][15] = 328;
			Big5Freq[4][14] = 327;			Big5Freq[23][88] = 326;			Big5Freq[18][2] = 325;
			Big5Freq[6][88] = 324;			Big5Freq[16][84] = 323;			Big5Freq[12][48] = 322;
			Big5Freq[7][68] = 321;			Big5Freq[5][50] = 320;			Big5Freq[13][54] = 319;
			Big5Freq[7][98] = 318;			Big5Freq[11][6] = 317;			Big5Freq[9][80] = 316;
			Big5Freq[16][41] = 315;			Big5Freq[7][43] = 314;			Big5Freq[28][117] = 313;
			Big5Freq[3][51] = 312;			Big5Freq[7][3] = 311;			Big5Freq[20][81] = 310;
			Big5Freq[4][2] = 309;			Big5Freq[11][16] = 308;			Big5Freq[10][4] = 307;
			Big5Freq[10][119] = 306;		Big5Freq[6][142] = 305;			Big5Freq[18][51] = 304;
			Big5Freq[8][144] = 303;			Big5Freq[10][65] = 302;			Big5Freq[11][64] = 301;
			Big5Freq[11][130] = 300;		Big5Freq[9][92] = 299;			Big5Freq[18][29] = 298;
			Big5Freq[18][78] = 297;			Big5Freq[18][151] = 296;		Big5Freq[33][127] = 295;
			Big5Freq[35][113] = 294;		Big5Freq[10][155] = 293;		Big5Freq[3][76] = 292;
			Big5Freq[36][123] = 291;		Big5Freq[13][143] = 290;		Big5Freq[5][135] = 289;
			Big5Freq[23][116] = 288;		Big5Freq[6][101] = 287;			Big5Freq[14][74] = 286;
			Big5Freq[7][153] = 285;			Big5Freq[3][101] = 284;			Big5Freq[9][74] = 283;
			Big5Freq[3][156] = 282;			Big5Freq[4][147] = 281;			Big5Freq[9][12] = 280;
			Big5Freq[18][133] = 279;		Big5Freq[4][0] = 278;			Big5Freq[7][155] = 277;
			Big5Freq[9][144] = 276;			Big5Freq[23][49] = 275;			Big5Freq[5][89] = 274;
			Big5Freq[10][11] = 273;			Big5Freq[3][110] = 272;			Big5Freq[3][40] = 271;
			Big5Freq[29][115] = 270;		Big5Freq[9][100] = 269;			Big5Freq[21][67] = 268;
			Big5Freq[23][145] = 267;		Big5Freq[10][47] = 266;			Big5Freq[4][31] = 265;
			Big5Freq[4][81] = 264;			Big5Freq[22][62] = 263;			Big5Freq[4][28] = 262;
			Big5Freq[27][39] = 261;			Big5Freq[27][54] = 260;			Big5Freq[32][46] = 259;
			Big5Freq[4][76] = 258;			Big5Freq[26][15] = 257;			Big5Freq[12][154] = 256;
			Big5Freq[9][150] = 255;			Big5Freq[15][17] = 254;			Big5Freq[5][129] = 253;
			Big5Freq[10][40] = 252;			Big5Freq[13][37] = 251;			Big5Freq[31][104] = 250;
			Big5Freq[3][152] = 249;			Big5Freq[5][22] = 248;			Big5Freq[8][48] = 247;
			Big5Freq[4][74] = 246;			Big5Freq[6][17] = 245;			Big5Freq[30][82] = 244;
			Big5Freq[4][116] = 243;			Big5Freq[16][42] = 242;			Big5Freq[5][55] = 241;
			Big5Freq[4][64] = 240;			Big5Freq[14][19] = 239;			Big5Freq[35][82] = 238;
			Big5Freq[30][139] = 237;		Big5Freq[26][152] = 236;		Big5Freq[32][32] = 235;
			Big5Freq[21][102] = 234;		Big5Freq[10][131] = 233;		Big5Freq[9][128] = 232;
			Big5Freq[3][87] = 231;			Big5Freq[4][51] = 230;			Big5Freq[10][15] = 229;
			Big5Freq[4][150] = 228;			Big5Freq[7][4] = 227;			Big5Freq[7][51] = 226;
			Big5Freq[7][157] = 225;			Big5Freq[4][146] = 224;			Big5Freq[4][91] = 223;
			Big5Freq[7][13] = 222;			Big5Freq[17][116] = 221;		Big5Freq[23][21] = 220;
			Big5Freq[5][106] = 219;			Big5Freq[14][100] = 218;		Big5Freq[10][152] = 217;
			Big5Freq[14][89] = 216;			Big5Freq[6][138] = 215;			Big5Freq[12][157] = 214;
			Big5Freq[10][102] = 213;		Big5Freq[19][94] = 212;			Big5Freq[7][74] = 211;
			Big5Freq[18][128] = 210;		Big5Freq[27][111] = 209;		Big5Freq[11][57] = 208;
			Big5Freq[3][131] = 207;			Big5Freq[30][23] = 206;			Big5Freq[30][126] = 205;
			Big5Freq[4][36] = 204;			Big5Freq[26][124] = 203;		Big5Freq[4][19] = 202;
			Big5Freq[9][152] = 201;
		}
	};
}
