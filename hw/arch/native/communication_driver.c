#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "hw/communication_driver.h"
#include "hw_configs.h"


static struct sockaddr_in si_me, si_other;
static int socket_ctx, socket_size = sizeof(si_other);

communication_driver_status_e init_communication_driver() 
{
	
	if ((socket_ctx = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		return COM_DRIVER_ERROR_INIT;
	}
	
	memset((char*)&si_me, 0x00, sizeof(si_me));
	
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(UDP_PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(socket_ctx, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
	{
		return COM_DRIVER_ERROR_INIT;
	}

	return COM_DRIVER_SUCCESS;
}

communication_driver_status_e listen_to_connection(data_block_t *packet) 
{
    
	if ((packet->size = recvfrom(socket_ctx, packet->data, 
			MAX_TRANSMISSION_DATA_SIZE, 0, (struct sockaddr *) &si_other, &socket_size)) == -1)
	{
		return COM_DRIVER_ERROR_LISTEN_CONNECTION;
	}
	return COM_DRIVER_SUCCESS;
}

communication_driver_status_e send_to(data_block_t *packet) 
{
    if (sendto(socket_ctx, packet->data, packet->size, 0, (struct sockaddr*) &si_other, socket_size) == -1)
	{
		return COM_DRIVER_ERROR_SEND_DATA;
	}
	printf("sent\n");
	return COM_DRIVER_SUCCESS;
}

communication_driver_status_e close_communication() 
{
	if (close(socket_ctx) == -1) 
	{
		return COM_DRIVER_ERROR_CLOSE_CONNECTION;
	}
	return COM_DRIVER_SUCCESS;
}