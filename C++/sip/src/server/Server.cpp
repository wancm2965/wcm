// Server.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "eXosip2/eXosip.h"
#include <stdlib.h>
//#include <WinSock2.h>
int main()
{
	eXosip_event_t* je = nullptr;
	osip_message_t* ack = nullptr;
	osip_message_t* invite = nullptr;
	osip_message_t* answer = nullptr;
	sdp_message_t* remote_sdp = nullptr;
	int call_id, dialog_id;
	int i, j;
	int id;
	//char* sour_call = "sip:140@127.0.0.1";
	//char* dest_call = "sip:133@127.0.0.1:15060";
	char command;
	char tmp[4096];
	char localip[128];
	int pos = 0;
	struct eXosip_t* context_eXosip;
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
		i = eXosip_listen_addr(context_eXosip, IPPROTO_UDP, NULL, 15061, AF_INET, 0);
		if (i != 0)
		{
			eXosip_quit(context_eXosip);
			osip_free(context_eXosip);
			return -1;
		}
		for (;;)
		{
			je = eXosip_event_wait(context_eXosip, 0, 50);
			eXosip_lock(context_eXosip);
			eXosip_default_action(context_eXosip, je);
			eXosip_automatic_refresh(context_eXosip);
			eXosip_unlock(context_eXosip);
			if (je == nullptr)
			{
				continue;
			}
			switch (je->type)
			{
			case EXOSIP_MESSAGE_NEW:
			{
				printf("EXOSIP_MESSAGE_NEW\n");
				if (MSG_IS_MESSAGE(je->request))
				{
					{
						osip_body_t*body;
						osip_message_get_body(je->request, 0, &body);
						printf("I get the msg is %s\n", body->body);
					}
					eXosip_message_build_answer(context_eXosip, je->tid, 200, &answer);
					eXosip_message_send_answer(context_eXosip, je->tid, 200, answer);
				}
			}
			break;
			case EXOSIP_CALL_INVITE:
			{
				printf("Received a INVITE msg from %s:%s,username is %s,password is %s \n", je->request->req_uri->host, je->request->req_uri->port, je->request->req_uri->username, je->request->req_uri->password);
				remote_sdp = eXosip_get_remote_sdp(context_eXosip,je->did);
				call_id = je->cid;
				dialog_id = je->did;
				eXosip_lock(context_eXosip);
				eXosip_call_send_answer(context_eXosip, je->tid, 180, nullptr);
				i = eXosip_call_build_answer(context_eXosip, je->tid, 200, &answer);
				if (i != 0)
				{
					printf("this request msg is invalid! Can't response\n");
					eXosip_call_send_answer(context_eXosip, je->tid, 400, nullptr);
				}
				else
				{
					_snprintf(tmp, 4096,
						"v=0\r\n"
						"o=anonymous 0 0 IN IP4 0.0.0.0\r\n"
						"t=1 10\r\n"
						"a=username:rainfish\r\n"
						"a=password:123\r\n");
					osip_message_set_body(answer, tmp, strlen(tmp));
					osip_message_set_content_type(answer, "application/sdp");
					eXosip_call_send_answer(context_eXosip, je->tid, 200, answer);
					printf("send 200 over\n");
				}
				eXosip_unlock(context_eXosip);

				printf("the INFO is : \n");
				while (!osip_list_eol(&(remote_sdp->a_attributes),pos))
				{
					sdp_attribute_t*at;
					at = (sdp_attribute_t*)osip_list_get(&remote_sdp->a_attributes, pos);
					printf("%s:%s\n", at->a_att_field, at->a_att_value);
					pos++;
				}
			}
			break;
			case EXOSIP_CALL_ACK:
			{
				printf("ACK revcieved\n");
			}
			break;
			case EXOSIP_CALL_CLOSED:
			{
				printf("the remote hold the session\n");
				i = eXosip_call_build_answer(context_eXosip, je->tid, 200, &answer);
				if (i =! 0)
				{
					printf("this request msg is invalid  can,t response\n");
					eXosip_call_send_answer(context_eXosip, je->tid, 400, nullptr);
				}
				else
				{
					eXosip_call_send_answer(context_eXosip, je->tid, 200, answer);
					printf("bye send 200 over\n");
				}
			}
			break;
			case EXOSIP_CALL_MESSAGE_NEW:
			{
				printf("EXOSIP_CALL_MESSAGE_NEW\n");
				if (MSG_IS_INFO(je->request))
				{
					eXosip_lock(context_eXosip);
					i = eXosip_call_build_answer(context_eXosip, je->tid, 200, &answer);
					if (i == 0)
					{
						eXosip_call_send_answer(context_eXosip, je->tid, 200, answer);
					}
					eXosip_unlock(context_eXosip);

					{
						osip_body_t*body;
						osip_message_get_body(je->request, 0, &body);
						printf("the body is %s\n", body->body);
					}
				}
			}
			break;
			default:
				printf("Could not parse the msg!\n");
				break;
			}
		}
	}
    return 0;
}

