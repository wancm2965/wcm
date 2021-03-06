// Client.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "eXosip2/eXosip.h"
#include <stdlib.h>
//#include <winsock2.h>
#include <string>
using namespace std;
int main(int argc, char*argv[])
{
	struct eXosip_t* context_eXosip;
	eXosip_event_t* je;
	osip_message_t* reg = nullptr;
	osip_message_t* invite = nullptr;
	osip_message_t* ack = nullptr;
	osip_message_t* info = nullptr;
	osip_message_t* message = nullptr;
	int call_id, dialog_id;
	int i, flag;
	int flag1 = 1;
	std::string identity = "sip:140@127.0.0.1";
	std::string registart = "sip:133@127.0.0.1:15061";
	std::string source_call = "sip:140@127.0.0.1";
	std::string dest_call = "sip:133@127.0.0.1:15061";

	char command;
	char tmp[4096];

	printf("r  向服务器注册\n\n");
	printf("c  取消注册\n\n");
	printf("i  发起呼叫请求\n\n");
	printf("h  挂断\n\n");
	printf("q  退出程序\n\n");
	printf("s  执行方法INFO\n\n");
	printf("m  执行方法MESSAGE\n\n");
	context_eXosip = eXosip_malloc();
	if (context_eXosip)
	{
		i = eXosip_init(context_eXosip);
		if (i != 0)
		{
			printf("init fail \n");
			return -1;
		}
		printf("init success\n");
		i = eXosip_listen_addr(context_eXosip, IPPROTO_UDP, NULL, 15060, AF_INET, 0);
		if (i != 0)
		{
			eXosip_quit(context_eXosip);
			osip_free(context_eXosip);
			return -1;
		}
		flag = 1;

		while (flag)
		{
			printf("Please input the command:\n");
			scanf("%c", &command);
			getchar();
			switch (command)
			{
			case 'r':
			{

			}
			break;
			case 'i':
			{
				i = eXosip_call_build_initial_invite(context_eXosip, &invite, dest_call.c_str(), source_call.c_str(), NULL, "This is a call for conversation");
				if (i != 0)
				{
					printf("Inital INVITE failed \n");
					break;
				}
				_snprintf(tmp, 4096,
					"v=0\r\n"
					"o=anonymous 0 0 IN IP4 0.0.0.0\r\n"
					"t=1 10\r\n"
					"a=username:rainfish\r\n"
					"a=password:123\r\n");
				osip_message_set_body(invite, tmp, strlen(tmp));
				osip_message_set_content_type(invite, "application/sdp");

				eXosip_lock(context_eXosip);
				i = eXosip_call_send_initial_invite(context_eXosip, invite);
				eXosip_unlock(context_eXosip);
				flag1 = 1;
				while (flag1)
				{
					je = eXosip_event_wait(context_eXosip, 0, 200);
					if (je == nullptr)
					{
						printf("No response or the time is over \n");
						break;
					}
					switch (je->type)
					{
					case EXOSIP_CALL_INVITE:
					{
						printf("a new invite received!\n");
					}
					break;
					case EXOSIP_CALL_PROCEEDING:
					{
						printf("proceeding \n");
					}
					break;
					case EXOSIP_CALL_RINGING:
					{
						printf("ringing \n");
						printf("callid is%d,dialogid is %d\n", je->cid, je->did);
					}
					break;
					case EXOSIP_CALL_ANSWERED:
					{
						printf("ok! connected!\n");
						call_id = je->cid;
						dialog_id = je->did;

						printf("callid is %d,dialogid is %d\n", call_id, dialog_id);
						eXosip_call_build_ack(context_eXosip, je->did, &ack);
						eXosip_call_send_ack(context_eXosip, je->did, ack);
						flag1 = 0;
					}
					break;
					case EXOSIP_CALL_CLOSED:
					{
						printf("the other sid closed\n");
					}
					break;
					case EXOSIP_CALL_ACK:
					{
						printf("ACK received\n");
					}
					break;
					default:
						break;
					}
				}
				eXosip_event_free(je);
			}
			break;
			case 'c':
			{
				
			}
			break;
			case 'h':
			{
				printf("Holded \n");
				eXosip_lock(context_eXosip);
				eXosip_call_terminate(context_eXosip, call_id, dialog_id);
				eXosip_unlock(context_eXosip);
			}
			break;
			case 'q':
			{
				eXosip_quit(context_eXosip);
				osip_free(context_eXosip);
				printf("Exit the progam\n");
				flag = 0;
			}
			break;
			case 's':
			{
				eXosip_call_build_info(context_eXosip, dialog_id, &info);
				_snprintf(tmp, 4096, "\nThis is a sip message(Method:INFO)");
				osip_message_set_body(info, tmp, strlen(tmp));
				osip_message_set_content_type(info, "text/plain");
				eXosip_call_send_request(context_eXosip, dialog_id, info);
			}
			break;
			case 'm':
			{
				printf("the method: MESSAGE \n");
				eXosip_message_build_request(context_eXosip, &message, "MESSAGE", dest_call.c_str(), source_call.c_str(), NULL);

				_snprintf(tmp, 4096, "This is a sip message(Method:MESSAGE)");
				osip_message_set_body(message, tmp, strlen(tmp));

				osip_message_set_content_type(message, "text/xml");
				eXosip_message_send_request(context_eXosip, message);
			}
			break;
			default:
				break;
			}
		}
	}
	
    return 0;
}

