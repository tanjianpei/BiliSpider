#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "WolframLibrary.h"

typedef struct
{
	int aid;//视频编号 
	int mid;//上传者编号
	int pubdate;//投稿时间 
	int duration;//视频时长  
	int otherstat;//其它静态信息 
	int view;//播放数 
	int danmuku;//弹幕数 
	int coin;//硬币数 
	int favorite;//收藏数 
	int share;//分享数 
	int reply;//回复数 
	int like;//点赞数 	
}Video;

DLLEXPORT int BinaryCombine(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res)
{
	char *dir = MArgument_getUTF8String(Args[0]),
		*name = MArgument_getUTF8String(Args[1]),
		outaddr[200] = "";
	MTensor T = MArgument_getMTensor(Args[2]);
	mint limit = MArgument_getInteger(Args[3]),
		length = libData->MTensor_getFlattenedLength(T),
		*mdata = libData->MTensor_getIntegerData(T);
	sprintf(outaddr, "%s%s", dir, name);
	int i, j;
	Video *video1 = (Video*)malloc(limit * sizeof(Video)),
		*video2 = (Video*)malloc(limit * sizeof(Video));
	memset(video1, 0, limit * sizeof(Video));
	for (i = 0; i<length; i++)
	{
		char inaddr[200] = "";
		sprintf(inaddr, "%s%lld", dir, mdata[i]);
		FILE* input = fopen(inaddr, "rb");
		int number = fread(video2, sizeof(Video), limit, input);
		for (j = 0; j<number; j++)video1[video2[j].aid - 1] = video2[j];
		fclose(input);
	}
	for (i = 0, j = 0; i<limit; i++)if (video1[i].aid>0)
	{
		video2[j] = video1[i];
		j++;
	}
	FILE* output = fopen(outaddr, "wb");
	fwrite(video2, sizeof(Video), j, output);
	fclose(output);
	free(video1);
	free(video2);
	video1 = NULL;
	video2 = NULL;
	MArgument_setInteger(Res, j);
	return LIBRARY_NO_ERROR;
}

DLLEXPORT int VideoDataRead(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res)
{
	char *address = MArgument_getUTF8String(Args[0]);
	MTensor Select = MArgument_getMTensor(Args[1]), T;
	mint *s = libData->MTensor_getIntegerData(Select),
		length = libData->MTensor_getFlattenedLength(Select),
		limit = MArgument_getInteger(Args[2]);
	FILE* input = fopen(address, "rb");
	int i, j,
		*buffer = (int*)malloc(12 * limit * sizeof(int));
	int number = fread(buffer, sizeof(int), 12 * limit, input) / 12;
	mint dims[2] = { number,length };
	libData->MTensor_new(MType_Integer, 2, dims, &T);
	mint* mtdata = libData->MTensor_getIntegerData(T);
	for (j = 0; j < length; j++)
	{
		if (s[j]<13)
		{
			for (i = 0; i<number; i++)mtdata[length*i + j] = buffer[12 * i + s[j] - 1];
		}
		else if (s[j] == 51)
		{
			for (i = 0; i<number; i++)mtdata[length*i + j] = buffer[12 * i + 4] >> 16;
		}
		else if (s[j] == 52)
		{
			for (i = 0; i<number; i++)mtdata[length*i + j] = (buffer[12 * i + 4] >> 8) & 255;
		}
		else if (s[j] == 53)
		{
			for (i = 0; i<number; i++)mtdata[length*i + j] = (buffer[12 * i + 4] >> 7) & 1;
		}
		else if (s[j] == 54)
		{
			for (i = 0; i<number; i++)mtdata[length*i + j] = buffer[12 * i + 4] & 128;
		}
	}
	fclose(input);
	free(buffer);
	buffer = NULL;
	MArgument_setMTensor(Res, T);
	return LIBRARY_NO_ERROR;
}