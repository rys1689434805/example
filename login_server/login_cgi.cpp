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
        if (true == CheckUser(username.c_str(), password.c_str()))
        {
            //reply ok to client
            reply.Add("login_result", "OK");
            string opt;
            json.Get("opt", opt);
            if ("create_room" == opt)
            {
                string room_no = create_container();
                reply.Add("room_no", room_no);
            }
            else if ("follow_room" == opt)
            {
                string room_no;
                json.Get("room_no", room_no);
                if (true == check_room(room_no.c_str()))
                {
                    reply.Add("follow_result", "OK");
                }
                else
                {
                    reply.Add("follow_result", "Failed");
                }
            }
        }
        else
        {
            //replay failed to client
            reply.Add("login_result", "Failed");
        }
        string reply_string = reply.ToFormattedString();
        printf("Content-Length:%d\r\n\r\n", reply_string.size());

        printf("%s\r\n", reply_string.c_str());

        free(buff);

    }

}
