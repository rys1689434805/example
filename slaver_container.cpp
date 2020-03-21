#include <cstdio>
#include <stdlib.h>
#include <hiredis/hiredis.h> 
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h> //这个libevent中没有定义stdlib会用到stdlib中的东西,所以要放到后面
#include <event.h>
#include <string>

using namespace std;

void my_callback(struct redisAsyncContext *pc, void *reply, void *private_data)
{
	string port;
	string room_no;

	redisReply *my_reply = (redisReply *)reply;

	if ("message" != string(my_reply->element[0]->str))
	{
		return;  //前面会有第一条订阅成功的消息 这样判断会扔掉第一条没用的消息
	}
	else
	{
		//reply �а���IP�������
		string orig_content(my_reply->element[2]->str);
		int pos = orig_content.find(':');
		string IP = orig_content.substr(0, pos);
		
		string my_ip(getenv("MY_IP"));  //这个MY_IP是需要在运行程序是指定当前主机的IP
		if (IP != my_ip)  //把截取出来的端口判断是不是自己的，是的话就
		{
			return;
		}
		room_no = orig_content.substr(pos + 1, orig_content.size() - pos - 1);
	}

	//ִ这个room_no就是要传到容器中的脚本，当容器退出后也一起把他的存在redis中对应的信息删除
	string cmd = "./create_room.sh room_no" + room_no;
	auto pf = popen(cmd.c_str(), "r");  //popen执行脚本后可以使用fread到返回的结果
	if (pf != NULL)
	{
		char buff[1024];
		fread(buff, 1, sizeof(buff), pf);
		port.append(buff);
		pclose(pf);
	}
	
	auto spc = redisConnect("192.168.64.148", 6379);   //ͬ连接的是主服务器的redis，这里智能同步订阅
	if (NULL != spc)
	{
		freeReplyObject( redisCommand(spc, "publish server_port %s", port.c_str()));  //同步发布执行脚本创建容器返回的端口号
		redisFree(spc);																	//主服务器的CGI中堵塞等待这一条端口的发布
	}
}

int main()
{
	auto base = event_base_new();

	auto pc = redisAsyncConnect("192.168.64.148", 6379);  //异步连接主服器上面的redis，因为还要发布执行脚本创建容器成功返回端口号
	redisLibeventAttach(pc, base);

	redisAsyncCommand(pc, my_callback, NULL, "subscribe create_room");  //异步订阅creat_room通道

	event_base_dispatch(base);

    return 0;
}