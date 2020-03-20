#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
using namespace std;
void AddUser(const char *_name, const char *_password)
{
    char buff[1024] = {0};

    sprintf(buff, "./add_user.sh %s %s", _name, _password);

    system(buff);
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
        if (0 == iStatus)
        {
            bRet = true;
        }
    }
    else
    {
        execlp("./check_user.sh", "./check_user.sh", _name, _password_dgst, NULL);
    }


    return bRet;
}
void DelUser(const char *_name)
{
    char buff[1024] = {0};

    sprintf(buff, "./del_user.sh %s", _name);

    system(buff);
}


std::string create_container()
{
    string ret;
    FILE *pf = popen("./create_room.sh", "r");
    if (NULL != pf)
    {
        char buff[1024];
        fread(buff, 1, sizeof(buff), pf);
        ret.append(buff);
        ret.pop_back();
        pclose(pf);
    }

    return ret;
}

bool check_room(const char *_room_no)
{
    bool bRet = false;

    if (fork() > 0)
    {
        int iStatus = 0;
        wait(&iStatus);
        if (0 == iStatus)
        {
            bRet = true;
        }
    }
    else
    {
        execlp("./check_room.sh", "./check_room.sh", _room_no, NULL);
    }


    return bRet;

}
