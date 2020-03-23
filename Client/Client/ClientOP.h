#pragma once
#include <string>
using namespace std;

struct ClientInfo
{
	string serverID;
	string clientID;
	string serverIP;
	unsigned short port;
	int maxNode;
	string shmKey;
};

class ClientOP
{
public:
	// 初始化数据, 从磁盘的配置文件中
	ClientOP(string fileName);
	~ClientOP();

	// 秘钥协商
	bool seckeyAgree();

	// 秘钥校验
	void seckeyCheck();

	// 秘钥注销
	void seckeyZhuXiao();

private:
	ClientInfo m_info;
};

