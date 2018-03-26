#pragma once

#ifndef __COMPRESS_H__
#define __COMPRESS_H__

/************************************************************************
文件说明：
	ZIP压缩、解压缩内存

使用说明：
	ULONG uSize = CZipCompress::Zip(inBuffer, uInSize,0);获取压缩后所需要的空间
	ULONG outSize = CZipCompress::Unzip(outBuffer,uSize,0);获取解压缩后所需要的空间
************************************************************************/
class CZipCompress
{
protected:
	enum
	{
		N = 4096,
		F = 18,
		THRESHOLD = 2,
		NIL = N,
	};

public:
	unsigned char buffer[N+F-1];
	int mpos,mlen;
	int lson[N+1],rson[N+257],dad[N+1];
	unsigned char *InData,*OutData;         //输入输出数据指针
	unsigned long InDataSize;               //输入数据长度
	unsigned long InSize,OutSize;           //已输入输出数据长度


	CZipCompress();
	void Encode();							//压缩数据
	void Decode();							//解压数据

private:
	int  GetByte();							//获取一个字节的数据
	void PutByte(unsigned char c);			//写入一个字节的数据
	void InitTree();						//初始化串表
	void InsertNode(int r);					//插入一个表项
	void DeleteNode(int p);					//删除一个表项
	

public:
	static ULONG CZipCompress::Zip(unsigned char *in,unsigned long insize,unsigned char *out);
	static ULONG CZipCompress::Unzip(unsigned char *in,unsigned long insize,unsigned char *out);
};

/*_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-*/
CZipCompress::CZipCompress()
{
	memset(buffer,0,N+F-1);
	memset(lson,0, N+1);
	memset(rson,0, N+257);
	memset(dad,0,N+1);
	mpos = 0; mlen = 0;
	InData = 0; OutData = 0;
	InDataSize = 0;
	InSize = 0;OutSize = 0;
}
int CZipCompress::GetByte()
{
	if(InSize++>=InDataSize) return -1;
	return *InData++;
}

void CZipCompress::PutByte(unsigned char c)
{
	OutSize++;
	if (OutData)
	{
		*OutData++=c;
	}
}

void CZipCompress::InitTree()
{
	int i;
	for(i=N+1;i<=N+256;i++)rson[i]=NIL;
	for(i=0;i<N;i++)dad[i]=NIL;
}

void CZipCompress::InsertNode(int r)		
{
	int i,p,cmp;
	unsigned char *key;
	cmp=1;key=&buffer[r];p=N+1+key[0];
	rson[r]=lson[r]=NIL;mlen=0;
	while(1)
	{
		if(cmp>=0)
		{
			if(rson[p]!=NIL)
				p=rson[p];
			else
			{
				rson[p]=r;dad[r]=p;
				return;
			}
		}
		else
		{
			if(lson[p]!=NIL)
				p=lson[p];
			else
			{
				lson[p]=r;dad[r]=p;
				return;
			}
		}
		for(i=1;i<F;i++)
			if((cmp=key[i]-buffer[p+i])!=0)
				break;
		if(i>mlen)
		{
			mpos=p;
			if((mlen=i)>=F)
				break;
		}
	}
	dad[r]=dad[p];lson[r]=lson[p];rson[r]=rson[p];
	dad[lson[p]]=r;dad[rson[p]]=r;
	if(rson[dad[p]]==p)
		rson[dad[p]]=r;
	else
		lson[dad[p]]=r;
	dad[p]=NIL;
}

void CZipCompress::DeleteNode(int p)		
{
	int q;
	if(dad[p]==NIL)
		return;
	if(rson[p]==NIL)
		q=lson[p];
	else if(lson[p]==NIL)
		q=rson[p];
	else
	{
		q=lson[p];
		if(rson[q]!=NIL)
		{
			do
			{
				q=rson[q];
			}while(rson[q]!=NIL);
			rson[dad[q]]=lson[q];dad[lson[q]]=dad[q];
			lson[q]=lson[p];dad[lson[p]]=q;
		}
		rson[q]=rson[p];dad[rson[p]]=q;
	}
	dad[q]=dad[p];
	if(rson[dad[p]]==p)
		rson[dad[p]]=q;
	else
		lson[dad[p]]=q;
	dad[p]=NIL;
}


void CZipCompress::Encode()
{
	int i,c,len,r,s,lml,cbp;
	unsigned char codebuf[17],mask;
	InitTree();
	codebuf[0]=0;
	cbp=mask=1;
	s=0;r=N-F;
	for(i=s;i<r;i++)buffer[i]=' ';
	for(len=0;len<F&&(c=GetByte())!=-1;len++)buffer[r+len]=c;
	if(len==0)return;
	for(i=1;i<=F;i++)InsertNode(r-i);
	InsertNode(r);
	do
	{
		if(mlen>len)mlen=len;
		if(mlen<=THRESHOLD)
		{
			mlen=1;codebuf[0]|=mask;codebuf[cbp++]=buffer[r];
		}else
		{
			codebuf[cbp++]=(unsigned char)mpos;
			codebuf[cbp++]=(unsigned char)(((mpos>>4)&0xF0)|(mlen-(THRESHOLD+1)));
		}
		if((mask<<=1)==0)
		{
			for(i=0;i<cbp;i++)PutByte(codebuf[i]);
			codebuf[0]=0;cbp=mask=1;
		}
		lml=mlen;
		for(i=0;i<lml&&(c=GetByte())!=-1;i++)
		{
			DeleteNode(s);buffer[s]=c;
			if(s<F-1)buffer[s+N]=c;
			s=(s+1)&(N-1);r=(r+1)&(N-1);
			InsertNode(r);
		}
		while(i++<lml)
		{
			DeleteNode(s);
			s=(s+1)&(N-1);r=(r+1)&(N-1);
			if(--len)InsertNode(r);
		}
	}while(len>0);
	if(cbp>1)for(i=0;i<cbp;i++)PutByte(codebuf[i]);
}


void CZipCompress::Decode()
{
	int i,j,k,r,c;
	unsigned int flags;
	for(i=0;i<N-F;i++)buffer[i]=' ';
	r=N-F;flags=0;
	for(;;)
	{
		if(((flags>>=1)&256)==0)
		{
			if((c=GetByte())==-1)break;
			flags=c|0xFF00;
		}
		if(flags&1)
		{
			if((c=GetByte())==-1)break;
			PutByte(c);buffer[r++]=c;r&=(N-1);
		}else
		{
			if((i=GetByte())==-1)break;
			if((j=GetByte())==-1)break;
			i|=((j&0xF0)<<4);j=(j&0x0F)+THRESHOLD;
			for(k=0;k<=j;k++)
			{
				c=buffer[(i+k)&(N-1)];
				PutByte(c);buffer[r++]=c;r&=(N-1);
			}
		}
	}
}

ULONG CZipCompress::Zip(unsigned char *in,unsigned long insize,unsigned char *out)
{
	CZipCompress zip;
	zip.InData=in;
	zip.InDataSize=insize;
	zip.InSize=0;
	zip.OutData=out;
	zip.OutSize=0;

	zip.Encode();
	return zip.OutSize;
}
ULONG CZipCompress::Unzip(unsigned char *in,unsigned long insize,unsigned char *out)
{
	CZipCompress zip;
	zip.InData=in;
	zip.InDataSize=insize;
	zip.InSize=0;
	zip.OutData=out;
	zip.OutSize=0;
	zip.Decode();
	return zip.OutSize;
}

#endif // __COMPRESS_H__