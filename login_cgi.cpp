#include <stdlib.h>
#include <string>
#include <fcgi_stdio.h>
#include "CJsonObject.hpp"
#include "user_opt.h"
#include <hiredis/hiredis.h>

using namespace std;
static int cur_room_no = 0;
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
                cur_room_no++;
                string room_info = create_container(cur_room_no);
                // add a record: room_noX room_info
                auto pc = redisConnect("127.0.0.1", 6379);
                if (NULL != pc)
                {
                    freeReplyObject(redisCommand(pc, "set room_no%d %s", cur_room_no, room_info.c_str()));
                    redisFree(pc);
                }
                
                reply.Add("server_IP", room_info.substr(0,room_info.find(' ')));
                reply.Add("server_PORT", room_info.substr(room_info.find(' ') + 1, room_info.size() - room_info.find(' ') - 1));
                reply.Add("room_no", cur_room_no);
            }
            else if ("follow_room" == opt)
            {
                string room_no;
                json.Get("room_no", room_no);
                string room_info  = check_room(room_no.c_str());

                if (room_info.size() <= 0)
                {
                    reply.Add("follow_result", "Failed");
                }
                else
                {
                    reply.Add("follow_result", "OK");
                    
                    reply.Add("server_IP", room_info.substr(0,room_info.find(' ')));
                    reply.Add("server_PORT", room_info.substr(room_info.find(' ') + 1, room_info.size() - room_info.find(' ') - 1));
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
