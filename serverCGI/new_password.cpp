#include <stdlib.h>
#include <string>
#include <fcgi_stdio.h>
#include "CJsonObject.hpp"
#include "user_opt.h"

using namespace std;

int main()
{
    while (FCGI_Accept() >= 0)
    {
        string username;
        string password;
        int json_len = atoi(getenv("CONTENT_LENGTH"));
        char *buff = (char *)calloc(1UL, json_len);
        
        fread(buff, 1, json_len, stdin);
        string json_string(buff);
        neb::CJsonObject json(json_string);

        json.Get("username", username);
        json.Get("password", password);

        printf("Content-Type:application/json\r\n");


        neb::CJsonObject reply;
        DelUser(username.c_str());
        AddUserMd5(username.c_str(), password.c_str());
        reply.Add("login_result", "OK");
        string reply_string = reply.ToFormattedString();
        printf("Content-Length:%d\r\n\r\n", reply_string.size());

        printf("%s\r\n", reply_string.c_str());

        free(buff);

    }

}
