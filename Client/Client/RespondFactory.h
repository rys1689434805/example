#pragma once
#include "CodecFactory.h"
#include "RespondCodec.h"
#include <iostream>

class RespondFactory :
	public CodecFactory
{
public:
	// 创建解码对象
	RespondFactory(std::string enc);
	// 创建编码对象
	RespondFactory(RespondInfo* info);
	Codec* createCodec();

	~RespondFactory();

private:
	bool m_flag;
	std::string m_encStr;
	RespondInfo* m_info;
};

