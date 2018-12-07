/*
 * unpifi.h
 *
 *  Created on: Nov 16, 2018
 *      Author: yifeifan
 */

#ifndef UNPIFI_H_
#define UNPIFI_H_

#include "unp.h"
#include <net/if.h>

#define IFI_NAME 16         /* same as IFNAMESIZ in <net/if.h> */
#define IFI_HADDR 8          /* allow for 64-bit in futrue */

struct ifi_info{
	char ifi_name[IFI_NAME];    /*interface name, Null-terminated */
	short ifi_index;            /*interface index */
	short ifi_mtu;              /*interface mtu */
	u_char ifi_haddr[IFI_HADDR]; /*hardware address */
	u_short ifi_hlen;           /*bytes in hardware address:0,6,8 */
	short ifi_flags;            /* IFF_XXX constans form */
	short ifi_myflags;          /* our own flags */
	struct sockaddr *ifi_addr;   /* primary address */
	struct sockaddr *ifi_barddr; /*broadcast address */
	struct sockaddr *ifidestadr; /*destiantion address */
	struct ifi_info *ifi_next;   /*next of those structures */
};

#define IFIALIAS 1              /*IFI_ADDR IS AN alias */

struct ifi_info *get_ifi_info(int,int);
struct ifi_info *Get_ifi_info(int,int);

void free_ifi_info(struct ifi_info *);




#endif /* UNPIFI_H_ */
