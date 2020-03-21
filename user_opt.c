#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <hiredis/hiredis.h>
using namespace std;
void AddUser(const char *_name, const char *_password)
{
    char buff[1024] = {0};

    sprintf(buff, "./add_user.sh %s %s", _name, _password);

    system(buff);   //这个函数会调用脚本，返回结果
}
void AddUserMd5(const char *_name, const char *_password)
{
    char buff[1024] = {0};

    sprintf(buff, "echo '%s:%s' >> userfile", _name, _password);

    system(buff);
}
bool CheckUser(const char *_name, const char *_password_dgst)
{
    bool bRet = false;

    if (fork() > 0)
    {
        int iStatus = 0;
        wait(&iStatus);
        if (0 == iStatus)   //子进程也就是这里的execlp执行脚本成功就是正常退出，说明用户存在
        {
            bRet = true;
        }
    }
    else     //执行脚本echo $? 为0表示成功，不是0就是失败
    {
        execlp("./check_user.sh", "./check_user.sh", _name, _password_dgst, NULL);  //如果用户不存这里就会异常退出被父进程wait到
    }


    return bRet;
}
void DelUser(const char *_name)
{
    char buff[1024] = {0};

    sprintf(buff, "./del_user.sh %s", _name);

    system(buff);
}

static int g_cur_select = 0;
std::string create_container(int room_no)   //创建容器传入的房间号是为了给容器中的脚本退出就删除存在redis中的对应信息
{
    //return "x.x.x.x xxxx"
    //
    //1 confirm which server to create container
    std::string port;
    std::string server_array[2] = {"192.168.64.148", "192.168.64.149"};
    std::string cur_server = server_array[g_cur_select];
    g_cur_select++;
    g_cur_select %= sizeof(server_array)/sizeof(server_array[0]);  //永远轮循均衡分配给每个服务器
    //2 send create container req to that server
    //publish a piece of message to channel "create_room"
    auto pc = redisConnect("127.0.0.1", 6379);
    if (NULL != pc)
    {
        freeReplyObject(redisCommand(pc, "publish create_room %s:%d", cur_server.c_str(), room_no));  //发布执行的服务器号和房间号
        //3 wait for new container's port
        freeReplyObject(redisCommand(pc, "subscribe server_port"));
        redisReply*reply = NULL;
        if (REDIS_OK == redisGetReply(pc, (void **)&reply))   //redis会堵塞在这里等待订阅的端口返回（同步）
        {
            port.append(reply->element[2]->str);   
            freeReplyObject(reply);
        }
        redisFree(pc);
    }


    //4 return IP + PORT
    return cur_server + " " + port;
}

std::string check_room(const char *_room_no)
{
    //return "x.x.x.x xxxx"
    std::string ret;    //房间号是存在主服务器的redis中

    auto pc = redisConnect("127.0.0.1", 6379);
    if (NULL != pc)
    {
        redisReply *reply = (redisReply *)redisCommand(pc, "get room_no%s", _room_no);
        if (reply->str != NULL)
            ret.append(reply->str);
        freeReplyObject(reply);
        redisFree(pc);
    }
    return ret;
}
