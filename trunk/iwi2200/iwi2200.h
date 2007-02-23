/* 
     Intel wireless/Pro 2200bg/2945abg device driver for Darwin.
	 
     Copyright(C) 2007  Marco Rodrigues <jalavoui @ gmail. com> 
			    Kazuhisa TAKEI <kazuhisa @ gmail. com>. All right reserved.
									
     This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
   
   this file contain parts from original source at http://prdownloads.sourceforge.net/ipw2200/ipw2200-1.2.1.tgz?download

*/

#ifndef __iwi2200_h__
#define __iwi2200_h__

#include "defines.h"
#include "iwieth.h"

#define CONFIG_IPW2200_QOS
// #define TX_QUEUE_CHECK


#undef IW_RX_REPLACING
//#define IWI_NOLOG
//#define IWI_DEBUG_NORMAL
//#define IWI_DEBUG_FULL_MODE

#define IWI_LOG(...) IOLog("iwi2200: " __VA_ARGS__)

#if defined(IWI_DEBUG_FULL_MODE) || defined(IWI_DEBUG_NORMAL)
	#define IWI_DEBUG(fmt,...)IWI_LOG(" %s() " fmt, __FUNCTION__, ##__VA_ARGS__)
//	#define IWI_DEBUG(fmt,...)  \
//		do { IWI_LOG(" %s() " fmt, __FUNCTION__, ##__VA_ARGS__); \
//		      if(priv->status){ IWI_LOG("priv->status 0x010x\n",priv->status); } \
//		}while(0)
#else
//	#define IWI_DEBUG(...) IWI_LOG(__VA_ARGS__)
	#define IWI_DEBUG(...) do{ }while(0)
#endif

#if defined(IWI_DEBUG_FULL_MODE)
	#define IWI_DEBUG_FULL(...) IWI_DEBUG(__VA_ARGS__)
#else
          #define IWI_DEBUG_FULL(...) do{ }while(0)
#endif


#define IEEE80211_DEBUG_MGMT(...) IWI_DEBUG("(80211_MGMT) "  __VA_ARGS__)
#define IEEE80211_DEBUG_SCAN(...) IWI_DEBUG("(80211_SCAN) "  __VA_ARGS__)


//#if defined(IWI_DEBUG_NORMAL)
	#define IWI_WARNING(...) IWI_LOG(" W " __VA_ARGS__)
	#define IWI_ERR(...) IWI_LOG(" E " __VA_ARGS__)
//#else
//	#define IWI_WARNING(...) do{ }while(0)
//	#define IWI_ERR(...) do{ }while(0)
//#endif


#define IWI_DEBUG_FN(...) IWI_DEBUG(__VA_ARGS__)

// #define IWI_DEBUG_FN(fmt,...) IWI_DEBUG(" %s " fmt, __FUNCTION__, ##__VA_ARGS__)


//#define IWI_DEBUG_STATUS(priv)  IWI_DEBUG("priv->status 0x%08x\n",priv->status)
#define IWI_DEBUG_STATUS(priv) do{ }while(0)

#define min_t(type,x,y) \
        ({ type __x = (x); type __y = (y); __x < __y ? __x: __y; })
		
inline static int snprint_line(char *buf, size_t count,
			const u8 * data, u32 len, u32 ofs)
{
	int out, i, j, l;
	char c;

	out = snprintf(buf, count, "%08X", ofs);

	for (l = 0, i = 0; i < 2; i++) {
		out += snprintf(buf + out, count - out, " ");
		for (j = 0; j < 8 && l < len; j++, l++)
			out += snprintf(buf + out, count - out, "%02X ",
					data[(i * 8 + j)]);
		for (; j < 8; j++)
			out += snprintf(buf + out, count - out, "   ");
	}

	out += snprintf(buf + out, count - out, " ");
	for (l = 0, i = 0; i < 2; i++) {
		out += snprintf(buf + out, count - out, " ");
		for (j = 0; j < 8 && l < len; j++, l++) {
			c = data[(i * 8 + j)];
//			if (!isascii(c) || !isprint(c))
				c = '.';

			out += snprintf(buf + out, count - out, "%c", c);
		}

		for (; j < 8; j++)
			out += snprintf(buf + out, count - out, " ");
	}

	return out;
}



inline static int snprintk_buf(u8 * output, size_t size, const u8 * data, size_t len)
{
	size_t out = size;
	u32 ofs = 0;
	int total = 0;

	while (size && len) {
		out = snprint_line((char *)output, size, &data[ofs],
				   min_t(size_t, len, 16U), ofs);

		ofs += 16;
		output += out;
		size -= out;
		len -= min_t(size_t, len, 16U);
		total += out;
	}
	return total;
}

inline void printk_buf(const u8 * data, u32 len)
{
	char line[81];
	u32 ofs = 0;

	while (len) {
		snprint_line(line, sizeof(line), &data[ofs],
			     min(len, 16U), ofs);
		IWI_LOG("%s\n", line);
		ofs += 16;
		len -= min(len, 16U);
	}
}

#ifdef IWI_DEBUG_FULL_MODE
	#define IWI_DEBUG_DUMP(...) printk_buf(__VA_ARGS__)
#else
	#define IWI_DEBUG_DUMP(...) do{ }while(0)
#endif

#define IWI_DUMP_MBUF(f, skb, len) \
    IWI_DEBUG_FULL(" %d(%s) DumpMbuf m_data 0x%08x datastart 0x%08x pktlen %d m_len  %d args len %d\n", \
        f , __FUNCTION__, mbuf_data(skb) ,mbuf_datastart(skb)  ,mbuf_len(skb) , mbuf_pkthdr_len(skb) , len  )


inline void skb_reserve(mbuf_t skb, int len)
{
	//skb->data += len;
	//skb->tail += len;
	/*        if (mbuf_len(skb)==0)
{
		void *data=(UInt8*)mbuf_data(skb)+len;
		mbuf_setdata(skb,data,mbuf_len(skb)+len);
} */
	IWI_DUMP_MBUF(1,skb,len); 
	void *data = (UInt8*)mbuf_data(skb) + len;
	IWI_DUMP_MBUF(2,skb,len);
	mbuf_setdata(skb,data, mbuf_len(skb));// m_len is not changed.
}

inline void *skb_put(mbuf_t skb, unsigned int len)
{
	/*unsigned char *tmp = skb->tail;
	SKB_LINEAR_ASSERT(skb);
	skb->tail += len;
	skb->len  += len;
	return tmp;*/
	void *data = (UInt8*)mbuf_data(skb) + mbuf_len(skb);
	//mbuf_prepend(&skb,len,1); /* no prepend work */
	IWI_DUMP_MBUF(1,skb,len);  
	if(mbuf_trailingspace(skb) > len ){
		mbuf_setlen(skb,mbuf_len(skb)+len);
		if(mbuf_flags(skb) & MBUF_PKTHDR)
			mbuf_pkthdr_setlen(skb,mbuf_pkthdr_len(skb)+len); 
	}
	IWI_DUMP_MBUF(2,skb,len);  
	return data;
}

inline void *skb_push(mbuf_t skb, unsigned int len)
{
	/*skb->data -= len;
	skb->len  += len;
	if (unlikely(skb->data<skb->head))
	skb_under_panic(skb, len, current_text_addr());
	return skb->data;*/
	/* void *data=(UInt8*)mbuf_data(skb)-len;
	mbuf_setdata(skb,data,mbuf_len(skb)+len); */
	IWI_DUMP_MBUF(1,skb,len); 
	mbuf_prepend(&skb,len,0);
	IWI_DUMP_MBUF(2,skb,len);
	return  (UInt8 *)mbuf_data(skb);
}

inline void *skb_pull(mbuf_t skb, unsigned int len)
{
	/*skb->len -= len;
	BUG_ON(skb->len < skb->data_len);
	return skb->data += len;*/
	IWI_DUMP_MBUF(1,skb,len);  
	mbuf_adj(skb,len);
	void *data=(UInt8*)mbuf_data(skb);
	IWI_DUMP_MBUF(2,skb,len);		
	return data;
}

#define kTransmitQueueCapacity 1024
//64

u8 P802_1H_OUI[P80211_OUI_LEN] = { 0x00, 0x00, 0xf8 };
u8 RFC1042_OUI[P80211_OUI_LEN] = { 0x00, 0x00, 0x00 };

typedef __u16 __be16;
struct ethhdr {
	unsigned char	h_dest[ETH_ALEN];	/* destination eth addr	*/
	unsigned char	h_source[ETH_ALEN];	/* source ether addr	*/
	__be16		h_proto;		/* packet type ID field	*/
} __attribute__((packed));

struct ipw_frame {
	int len;
	union {
		struct ieee80211_hdr frame;
		u8 raw[IEEE80211_FRAME_LEN];
		u8 cmd[360];
	} u;
	struct list_head list;
};

static int from_priority_to_tx_queue[] = {
	IPW_TX_QUEUE_1, IPW_TX_QUEUE_2, IPW_TX_QUEUE_2, IPW_TX_QUEUE_1,
	IPW_TX_QUEUE_3, IPW_TX_QUEUE_3, IPW_TX_QUEUE_4, IPW_TX_QUEUE_4
};

 static unsigned char rfc1042_header[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00 };

/* Bridge-Tunnel header (for EtherTypes ETH_P_AARP and ETH_P_IPX) */
static unsigned char bridge_tunnel_header[] =   { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8 };

#define IPW_CMD(x) case IPW_CMD_ ## x : return #x
static char *get_cmd_string(u8 cmd)
{
	switch (cmd) {
		IPW_CMD(HOST_COMPLETE);
		IPW_CMD(POWER_DOWN);
		IPW_CMD(SYSTEM_CONFIG);
		IPW_CMD(MULTICAST_ADDRESS);
		IPW_CMD(SSID);
		IPW_CMD(ADAPTER_ADDRESS);
		IPW_CMD(PORT_TYPE);
		IPW_CMD(RTS_THRESHOLD);
		IPW_CMD(FRAG_THRESHOLD);
		IPW_CMD(POWER_MODE);
		IPW_CMD(WEP_KEY);
		IPW_CMD(TGI_TX_KEY);
		IPW_CMD(SCAN_REQUEST);
		IPW_CMD(SCAN_REQUEST_EXT);
		IPW_CMD(ASSOCIATE);
		IPW_CMD(SUPPORTED_RATES);
		IPW_CMD(SCAN_ABORT);
		IPW_CMD(TX_FLUSH);
		IPW_CMD(QOS_PARAMETERS);
		IPW_CMD(DINO_CONFIG);
		IPW_CMD(RSN_CAPABILITIES);
		IPW_CMD(RX_KEY);
		IPW_CMD(CARD_DISABLE);
		IPW_CMD(SEED_NUMBER);
		IPW_CMD(TX_POWER);
		IPW_CMD(COUNTRY_INFO);
		IPW_CMD(AIRONET_INFO);
		IPW_CMD(AP_TX_POWER);
		IPW_CMD(CCKM_INFO);
		IPW_CMD(CCX_VER_INFO);
		IPW_CMD(SET_CALIBRATION);
		IPW_CMD(SENSITIVITY_CALIB);
		IPW_CMD(RETRY_LIMIT);
		IPW_CMD(IPW_PRE_POWER_DOWN);
		IPW_CMD(VAP_BEACON_TEMPLATE);
		IPW_CMD(VAP_DTIM_PERIOD);
		IPW_CMD(EXT_SUPPORTED_RATES);
		IPW_CMD(VAP_LOCAL_TX_PWR_CONSTRAINT);
		IPW_CMD(VAP_QUIET_INTERVALS);
		IPW_CMD(VAP_CHANNEL_SWITCH);
		IPW_CMD(VAP_MANDATORY_CHANNELS);
		IPW_CMD(VAP_CELL_PWR_LIMIT);
		IPW_CMD(VAP_CF_PARAM_SET);
		IPW_CMD(VAP_SET_BEACONING_STATE);
		IPW_CMD(MEASUREMENT);
		IPW_CMD(POWER_CAPABILITY);
		IPW_CMD(SUPPORTED_CHANNELS);
		IPW_CMD(TPC_REPORT);
		IPW_CMD(WME_INFO);
		IPW_CMD(PRODUCTION_COMMAND);
	default:
		return "UNKNOWN";
	}
}

static u8 qos_oui[QOS_OUI_LEN] = { 0x00, 0x50, 0xF2 };

static struct ieee80211_qos_parameters def_qos_parameters_CCK = {
	{QOS_TX0_CW_MIN_CCK, QOS_TX1_CW_MIN_CCK, QOS_TX2_CW_MIN_CCK,
	 QOS_TX3_CW_MIN_CCK},
	{QOS_TX0_CW_MAX_CCK, QOS_TX1_CW_MAX_CCK, QOS_TX2_CW_MAX_CCK,
	 QOS_TX3_CW_MAX_CCK},
	{QOS_TX0_AIFS, QOS_TX1_AIFS, QOS_TX2_AIFS, QOS_TX3_AIFS},
	{QOS_TX0_ACM, QOS_TX1_ACM, QOS_TX2_ACM, QOS_TX3_ACM},
	{QOS_TX0_TXOP_LIMIT_CCK, QOS_TX1_TXOP_LIMIT_CCK, QOS_TX2_TXOP_LIMIT_CCK,
	 QOS_TX3_TXOP_LIMIT_CCK}
};

static struct ieee80211_qos_parameters def_qos_parameters_OFDM = {
	{QOS_TX0_CW_MIN_OFDM, QOS_TX1_CW_MIN_OFDM, QOS_TX2_CW_MIN_OFDM,
	 QOS_TX3_CW_MIN_OFDM},
	{QOS_TX0_CW_MAX_OFDM, QOS_TX1_CW_MAX_OFDM, QOS_TX2_CW_MAX_OFDM,
	 QOS_TX3_CW_MAX_OFDM},
	{QOS_TX0_AIFS, QOS_TX1_AIFS, QOS_TX2_AIFS, QOS_TX3_AIFS},
	{QOS_TX0_ACM, QOS_TX1_ACM, QOS_TX2_ACM, QOS_TX3_ACM},
	{QOS_TX0_TXOP_LIMIT_OFDM, QOS_TX1_TXOP_LIMIT_OFDM,
	 QOS_TX2_TXOP_LIMIT_OFDM, QOS_TX3_TXOP_LIMIT_OFDM}
};

static struct ieee80211_qos_parameters def_parameters_OFDM = {
	{DEF_TX0_CW_MIN_OFDM, DEF_TX1_CW_MIN_OFDM, DEF_TX2_CW_MIN_OFDM,
	 DEF_TX3_CW_MIN_OFDM},
	{DEF_TX0_CW_MAX_OFDM, DEF_TX1_CW_MAX_OFDM, DEF_TX2_CW_MAX_OFDM,
	 DEF_TX3_CW_MAX_OFDM},
	{DEF_TX0_AIFS, DEF_TX1_AIFS, DEF_TX2_AIFS, DEF_TX3_AIFS},
	{DEF_TX0_ACM, DEF_TX1_ACM, DEF_TX2_ACM, DEF_TX3_ACM},
	{DEF_TX0_TXOP_LIMIT_OFDM, DEF_TX1_TXOP_LIMIT_OFDM,
	 DEF_TX2_TXOP_LIMIT_OFDM, DEF_TX3_TXOP_LIMIT_OFDM}
};

static struct ieee80211_qos_parameters def_parameters_CCK = {
	{DEF_TX0_CW_MIN_CCK, DEF_TX1_CW_MIN_CCK, DEF_TX2_CW_MIN_CCK,
	 DEF_TX3_CW_MIN_CCK},
	{DEF_TX0_CW_MAX_CCK, DEF_TX1_CW_MAX_CCK, DEF_TX2_CW_MAX_CCK,
	 DEF_TX3_CW_MAX_CCK},
	{DEF_TX0_AIFS, DEF_TX1_AIFS, DEF_TX2_AIFS, DEF_TX3_AIFS},
	{DEF_TX0_ACM, DEF_TX1_ACM, DEF_TX2_ACM, DEF_TX3_ACM},
	{DEF_TX0_TXOP_LIMIT_CCK, DEF_TX1_TXOP_LIMIT_CCK, DEF_TX2_TXOP_LIMIT_CCK,
	 DEF_TX3_TXOP_LIMIT_CCK}
};

static const char ipw_modes[] = {
	'a', 'b', 'g', '?'
};

struct ipw_status_code {
	u16 status;
	const char *reason;
};

#define MFIE_STRING(x) case MFIE_TYPE_ ##x: return #x

static const char *get_info_element_string(u16 id)
{
	switch (id) {
		MFIE_STRING(SSID);
		MFIE_STRING(RATES);
		MFIE_STRING(FH_SET);
		MFIE_STRING(DS_SET);
		MFIE_STRING(CF_SET);
		MFIE_STRING(TIM);
		MFIE_STRING(IBSS_SET);
		MFIE_STRING(COUNTRY);
		MFIE_STRING(HOP_PARAMS);
		MFIE_STRING(HOP_TABLE);
		MFIE_STRING(REQUEST);
		MFIE_STRING(CHALLENGE);
		MFIE_STRING(POWER_CONSTRAINT);
		MFIE_STRING(POWER_CAPABILITY);
		MFIE_STRING(TPC_REQUEST);
		MFIE_STRING(TPC_REPORT);
		MFIE_STRING(SUPP_CHANNELS);
		MFIE_STRING(CSA);
		MFIE_STRING(MEASURE_REQUEST);
		MFIE_STRING(MEASURE_REPORT);
		MFIE_STRING(QUIET);
		MFIE_STRING(IBSS_DFS);
		MFIE_STRING(ERP_INFO);
		MFIE_STRING(RSN);
		MFIE_STRING(RATES_EX);
		MFIE_STRING(GENERIC);
		MFIE_STRING(QOS_PARAMETER);
	default:
		return "UNKNOWN";
	}
}			

inline int is_multicast_ether_addr(const u8 *addr)
{
       return addr[0] & 0x01;
}

inline int is_broadcast_ether_addr(const u8 *addr)
{
        return (addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5]) == 0xff;
}

inline unsigned int
__div(unsigned long long n, unsigned int base)
{
	return n / base;
}
//FIXME
#define jiffies		\
({		\
	uint64_t m;		\
	clock_get_uptime(&m);		\
	__div(m , 10000000)*5;		\
})

inline unsigned int jiffies_to_msecs(const unsigned long j)
{
#if HZ <= MSEC_PER_SEC && !(MSEC_PER_SEC % HZ)
	return (MSEC_PER_SEC / HZ) * j;
#elif HZ > MSEC_PER_SEC && !(HZ % MSEC_PER_SEC)
	return (j + (HZ / MSEC_PER_SEC) - 1)/(HZ / MSEC_PER_SEC);
#else
	return (j * MSEC_PER_SEC) / HZ;
#endif
}

#define time_after(a,b)	((long)(b) - (long)(a) < 0)
	 
#define container_of(ptr, type, member)					\
({									\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);		\
	(type *)( (char *)__mptr - offsetof(type,member) );		\
})

#define __list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)
	
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

static inline void prefetch(const void *x) {;}

#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     prefetch(pos->member.next), &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))


#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)

struct hlist_head {
	struct hlist_node *first;
};

inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

inline void __hlist_del(struct hlist_node *n)
{
	struct hlist_node *next = n->next;
	struct hlist_node **pprev = n->pprev;
	*pprev = next;
	if (next)
		next->pprev = pprev;
}

inline void __list_add(struct list_head *new2,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new2;
	new2->next = next;
	new2->prev = prev;
	prev->next = new2;
}

inline void list_add_tail(struct list_head *new2, struct list_head *head)
{
	__list_add(new2, head->prev, head);
}

inline void list_add(struct list_head *new2, struct list_head *head)
{
	__list_add(new2, head, head->next);
}

inline void hlist_del(struct hlist_node *n)
{
	__hlist_del(n);
	(void*)n->next = LIST_POISON1;
	(void*)n->pprev = LIST_POISON2;
}

inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	(void*)entry->next = LIST_POISON1;
	(void*)entry->prev = LIST_POISON2;
}

struct ipw_network_match {
	struct ieee80211_network *network;
	struct ipw_supported_rates rates;
};

struct fw_header {
	u32 version;
	u32 mode;
};

struct fw_chunk {
	u32 address;
	u32 length;
};

struct firmware {
	size_t size;
	u8 *data;
};

struct iwi_hdr {
	UInt8	type;
#define IWI_HDR_TYPE_DATA	0
#define IWI_HDR_TYPE_COMMAND	1
#define IWI_HDR_TYPE_NOTIF	3
#define IWI_HDR_TYPE_FRAME	9

	UInt8	seq;
	UInt8	flags;
#define IWI_HDR_FLAG_IRQ	0x04

	UInt8	reserved;
}  __attribute__ ((packed));


/* command */
struct iwi_cmd_desc {
	struct iwi_hdr	hdr;
	UInt8		type;
#define IWI_CMD_ENABLE				2
#define IWI_CMD_SET_CONFIG			6
#define IWI_CMD_SET_ESSID			8
#define IWI_CMD_SET_MAC_ADDRESS			11
#define IWI_CMD_SET_RTS_THRESHOLD		15
#define IWI_CMD_SET_FRAG_THRESHOLD		16
#define IWI_CMD_SET_POWER_MODE			17
#define IWI_CMD_SET_WEP_KEY			18
#define IWI_CMD_SCAN				20
#define IWI_CMD_ASSOCIATE			21
#define IWI_CMD_SET_RATES			22
#define IWI_CMD_ABORT_SCAN			23
#define IWI_CMD_SET_WME_PARAMS			25
#define IWI_CMD_SET_OPTIE			31
#define IWI_CMD_DISABLE				33
#define IWI_CMD_SET_IV				34
#define IWI_CMD_SET_TX_POWER			35
#define IWI_CMD_SET_SENSITIVITY			42
#define IWI_CMD_SET_WMEIE			84

	UInt8		len;
	UInt16	reserved;
	UInt8		data[120];
} __attribute__ ((packed));


struct iwi_cmd_ring {
//	bus_dma_tag_t		desc_dmat;
//	bus_dmamap_t		desc_map;
//	bus_addr_t		physaddr;
	IOBufferMemoryDescriptor *memD;
	dma_addr_t	physaddr;
	struct iwi_cmd_desc	*desc;
	int			count;
	int			queued;
	int			cur;
	int			next;
};

struct iwi_rx_radiotap_header {
//	struct ieee80211_radiotap_header wr_ihdr;
	UInt8		wr_flags;
	UInt8		wr_rate;
	UInt16	wr_chan_freq;
	UInt16	wr_chan_flags;
	UInt8		wr_antsignal;
	UInt8		wr_antenna;
};

#define IWI_RX_RADIOTAP_PRESENT						\
	((1 << IEEE80211_RADIOTAP_FLAGS) |				\
	 (1 << IEEE80211_RADIOTAP_RATE) |				\
	 (1 << IEEE80211_RADIOTAP_CHANNEL) |				\
	 (1 << IEEE80211_RADIOTAP_DB_ANTSIGNAL) |			\
	 (1 << IEEE80211_RADIOTAP_ANTENNA))

struct iwi_tx_radiotap_header {
//	struct ieee80211_radiotap_header wt_ihdr;
	UInt8		wt_flags;
	UInt16	wt_chan_freq;
	UInt16	wt_chan_flags;
};

struct iwi_tx_data {
	IOMemoryMap*	map;
	mbuf_t			m;
//	struct ieee80211_node	*ni;
};

struct iwi_tx_ring {
//	bus_dma_tag_t		desc_dmat;
//	bus_dma_tag_t		data_dmat;
//	bus_dmamap_t		desc_map;
	dma_addr_t		physaddr;
//	bus_addr_t		csr_ridx;
//	bus_addr_t		csr_widx;
	struct iwi_tx_desc	*desc;
	struct iwi_tx_data	*data;
	int			count;
	int			queued;
	int			cur;
	IOBufferMemoryDescriptor *memD;
	int			next;
};

struct iwi_rx_data {
	IOBufferMemoryDescriptor *memD;
//	bus_dmamap_t	map;
	dma_addr_t	physaddr;
	UInt32	reg;
	mbuf_t		m;
	void *			m_data;
};

struct iwi_rx_ring {
//	bus_dma_tag_t		data_dmat;
	struct iwi_rx_data	*data;
	int			count;
	int			cur;
};

/* header for transmission */
struct iwi_tx_desc {
	struct iwi_hdr	hdr;
	UInt32	reserved1;
	UInt8		station;
	UInt8		reserved2[3];
	UInt8		cmd;
#define IWI_DATA_CMD_TX	0x0b

	UInt8		seq;
	UInt16	len;
	UInt8		priority;
	UInt8		flags;
#define IWI_DATA_FLAG_SHPREAMBLE	0x04
#define IWI_DATA_FLAG_NO_WEP		0x20
#define IWI_DATA_FLAG_NEED_ACK		0x80

	UInt8		xflags;
#define IWI_DATA_XFLAG_QOS	0x10

	UInt8		wep_txkey;
//	UInt8		wepkey[IEEE80211_KEYBUF_SIZE];
	UInt8		rate;
	UInt8		antenna;
	UInt8		reserved3[10];
//	struct ieee80211_qosframe_addr4	wh;
	UInt32	iv;
	UInt32	eiv;
	UInt32	nseg;
#define IWI_MAX_NSEG	6

	UInt32	seg_addr[IWI_MAX_NSEG];
	UInt16	seg_len[IWI_MAX_NSEG];
};

struct iwi_configuration {
	UInt8	bluetooth_coexistence;
	UInt8	reserved1;
	UInt8	answer_pbreq;
	UInt8	allow_invalid_frames;
	UInt8	multicast_enabled;
	UInt8	drop_unicast_unencrypted;
	UInt8	disable_unicast_decryption;
	UInt8	drop_multicast_unencrypted;
	UInt8	disable_multicast_decryption;
	UInt8	antenna;
	UInt8	reserved2;
	UInt8	use_protection;
	UInt8	protection_ctsonly;
	UInt8	enable_multicast_filtering;
	UInt8	bluetooth_threshold;
	UInt8	reserved4;
	UInt8	allow_beacon_and_probe_resp;
	UInt8	allow_mgt;
	UInt8	noise_reported;
	UInt8	reserved5;
} __attribute__ ((packed));

/* structure for command IWI_CMD_SET_RATES */
struct iwi_rateset {
	UInt8	mode;
	UInt8	nrates;
	UInt8	type;
#define IWI_RATESET_TYPE_NEGOCIATED	0
#define IWI_RATESET_TYPE_SUPPORTED	1

	UInt8	reserved;
	UInt8	rates[12];
} __attribute__ ((packed));



struct iwi_scan {
	UInt8		type;
#define IWI_SCAN_TYPE_PASSIVE	1
#define IWI_SCAN_TYPE_BROADCAST	3

	UInt16		dwelltime;
	UInt8		channels[54];
#define IWI_CHAN_5GHZ	(0 << 6)
#define IWI_CHAN_2GHZ	(1 << 6)

	UInt8		reserved[3];
} __attribute__ ((packed));


typedef enum {
    MEDIUM_TYPE_NONE = 0,
    MEDIUM_TYPE_AUTO,
    MEDIUM_TYPE_1MBIT,
    MEDIUM_TYPE_2MBIT,
    MEDIUM_TYPE_5MBIT,
    MEDIUM_TYPE_11MBIT,
    MEDIUM_TYPE_54MBIT,
	MEDIUM_TYPE_ADHOC,
    MEDIUM_TYPE_INVALID
} mediumType_t;


class darwin_iwi2200 : public IO80211Controller
{
	OSDeclareDefaultStructors(darwin_iwi2200)

public:
//virtual const char * getNamePrefix() const;
	virtual bool		init(OSDictionary *dictionary = 0);
	virtual void		free(void);
//	virtual IOService *	probe(IOService *provider, SInt32 *score);
	virtual bool		start(IOService *provider);
    virtual void		stop(IOService *provider);
	virtual IOReturn	getHardwareAddress(IOEthernetAddress *addr);
	virtual IOReturn	enable(IONetworkInterface * netif);
	virtual IOReturn	disable(IONetworkInterface * netif);
	virtual bool		uploadFirmware2(UInt16 *base, const unsigned char *fw, UInt32 size, int offset);
	virtual bool		uploadFirmware(u8 * data, size_t len);
	virtual bool		uploadUCode(const unsigned char * data, UInt16 len);
	virtual bool		uploadUCode2(UInt16 *base, const unsigned char *uc, UInt16 size, int offset);
	virtual void		stopMaster(UInt16 *base);
	virtual void		stopDevice(UInt16 *base);
	virtual bool		resetDevice(UInt16 *);
	virtual UInt16		readPromWord(UInt16 *base, UInt8 addr);
	static void			interruptOccurred(OSObject * owner, void * src, IOService *nub, int count);
	virtual UInt32		handleInterrupt(void);
	virtual IOBufferMemoryDescriptor * MemoryDmaAlloc(UInt32 buf_size, dma_addr_t *phys_add, void *virt_add);
//	virtual bool configureInterface( IONetworkInterface * interface );
	virtual bool		createWorkLoop( void );
	virtual IOWorkLoop * getWorkLoop( void ) const;
	virtual IOOutputQueue * createOutputQueue( void );
	virtual const OSString * newModelString( void ) const;
	virtual const OSString * newVendorString( void ) const;
	virtual bool		addMediumType(UInt32 type, UInt32 speed, UInt32 code, char* name = 0);
	virtual IOReturn	selectMedium(const IONetworkMedium * medium);

			
	virtual int			sendCommand(UInt8 type,void *data,UInt8 len,bool async);
	virtual int			ipw_scan(struct ipw_priv *priv, int type);
	virtual int			initCmdQueue();
	virtual int			resetCmdQueue();
	virtual int			initRxQueue();
	virtual int			resetRxQueue();
	virtual int			initTxQueue();
	virtual int			resetTxQueue();
	virtual void		ipw_rx(struct ipw_priv *priv);
	virtual int			configu(struct ipw_priv *priv);
	
	virtual IOReturn setPowerState ( unsigned long powerStateOrdinal, IOService* whatDevice );
virtual IOOptionBits getState( void ) const;

	virtual void notifIntr(struct ipw_priv *priv,
				struct ipw_rx_notification *notif);
	
	/* Memory operation functions */
	virtual void inline ipw_write32(UInt32 offset, UInt32 data);
	virtual UInt32 inline ipw_read32(UInt32 offset);
	virtual void inline ipw_set_bit(UInt32 reg, UInt32 mask);
	virtual void inline ipw_clear_bit(UInt32 reg, UInt32 mask);
	virtual int ipw_poll_bit(UInt32 reg, UInt32 mask, int timeout);
	
	/* EEPROM functions */
	virtual void cacheEEPROM(struct ipw_priv *priv);
	virtual void inline eeprom_write_reg(UInt32 data);
	virtual void inline eeprom_cs(bool sel);
	virtual void inline eeprom_write_bit(UInt8 bit);
	virtual void eeprom_op(UInt8 op, UInt8 addr);
	virtual UInt16 eeprom_read_UInt16(UInt8 addr);
	virtual UInt32 read_reg_UInt32(UInt32 reg);
	
	virtual void ipw_zero_memory(UInt32 start, UInt32 count);
	virtual inline void ipw_fw_dma_reset_command_blocks();
	virtual void ipw_write_reg32( UInt32 reg, UInt32 value);
	virtual int ipw_fw_dma_enable();
	virtual int ipw_fw_dma_add_buffer(UInt32 src_phys, UInt32 dest_address, UInt32 length);
	virtual int ipw_fw_dma_write_command_block(int index,
					  struct command_block *cb);
	virtual int ipw_fw_dma_kick();
	virtual int ipw_fw_dma_add_command_block(
					UInt32 src_address,
					UInt32 dest_address,
					UInt32 length,
					int interrupt_enabled, int is_last);
	virtual void ipw_write_indirect(UInt32 addr, UInt8 * buf,
				int num);
	virtual int ipw_fw_dma_wait();			
	virtual int ipw_fw_dma_command_block_index();
	virtual void ipw_fw_dma_dump_command_block();
	virtual void ipw_fw_dma_abort();
	virtual UInt32 ipw_read_reg32( UInt32 reg);
	virtual void ipw_write_reg8(UInt32 reg, UInt8 value);		
	virtual UInt8 ipw_read_reg8(UInt32 reg);
	virtual void ipw_write_reg16(UInt32 reg, UInt16 value);
	virtual int ipw_stop_nic();
	virtual int ipw_reset_nic(struct ipw_priv *priv);
	virtual int ipw_init_nic();									
	virtual void ipw_start_nic();
	virtual inline void ipw_enable_interrupts(struct ipw_priv *priv);
	virtual int ipw_set_geo(struct ieee80211_device *ieee,
		       const struct ieee80211_geo *geo);
	virtual int rf_kill_active(struct ipw_priv *priv);
	virtual void ipw_down(struct ipw_priv *priv);
	virtual int ipw_up(struct ipw_priv *priv);
	virtual inline int ipw_is_init(struct ipw_priv *priv);
	virtual void ipw_deinit(struct ipw_priv *priv);
	virtual void ipw_led_shutdown(struct ipw_priv *priv);
	virtual u32 ipw_register_toggle(u32 reg);
	virtual void ipw_led_activity_off(struct ipw_priv *priv);
	virtual void ipw_led_link_off(struct ipw_priv *priv);
	virtual void ipw_led_band_off(struct ipw_priv *priv);
	virtual inline void ipw_disable_interrupts(struct ipw_priv *priv);
	virtual void ipw_led_radio_off(struct ipw_priv *priv);
	virtual void ipw_led_init(struct ipw_priv *priv);
	virtual void ipw_led_link_on(struct ipw_priv *priv);
	virtual void ipw_led_band_on(struct ipw_priv *priv);
	virtual int ipw_sw_reset(int option);
	virtual int ipw_get_fw(const struct firmware **fw, const char *name);
	virtual void ipw_led_link_down(struct ipw_priv *priv);
	virtual void ipw_rf_kill(ipw_priv *priv);
	virtual int ipw_best_network(struct ipw_priv *priv,
			    struct ipw_network_match *match,
			    struct ieee80211_network *network, int roaming);
	virtual int ipw_compatible_rates(struct ipw_priv *priv,
				const struct ieee80211_network *network,
				struct ipw_supported_rates *rates);
	virtual void ipw_copy_rates(struct ipw_supported_rates *dest,
			   const struct ipw_supported_rates *src);
	virtual int ipw_is_rate_in_mask(struct ipw_priv *priv, int ieee_mode, u8 rate);
	virtual void ipw_adhoc_create(struct ipw_priv *priv,
			     struct ieee80211_network *network);		   
	virtual int ipw_is_valid_channel(struct ieee80211_device *ieee, u8 channel);
	virtual int ipw_channel_to_index(struct ieee80211_device *ieee, u8 channel);
	virtual void ipw_create_bssid(struct ipw_priv *priv, u8 * bssid);
	virtual void ipw_set_fixed_rate(struct ipw_priv *priv, int mode);
	virtual void ipw_qos_init(struct ipw_priv *priv, int enable,
			 int burst_enable, u32 burst_duration_CCK,
			 u32 burst_duration_OFDM);
	virtual struct ipw_rx_queue *darwin_iwi2200::ipw_rx_queue_alloc(struct ipw_priv *priv);
	virtual int ipw_queue_tx_hcmd(struct ipw_priv *priv, int hcmd, void *buf,
			     int len, int sync);
	virtual int ipw_queue_space(const struct clx2_queue *q);
	virtual int ipw_queue_tx_reclaim(struct ipw_priv *priv,
				struct clx2_tx_queue *txq, int qindex);
	virtual int is_duplicate_packet(struct ipw_priv *priv,
			       struct ieee80211_hdr_4addr *header);
	virtual int ipw_get_tx_queue_number(struct ipw_priv *priv, u16 priority);
	virtual int ipw_net_is_queue_full(struct net_device *dev, int pri);
	virtual int ipw_qos_set_tx_queue_command(struct ipw_priv *priv,
					u16 priority, struct tfd_data *tfd);
	virtual int ipw_net_hard_start_xmit(struct ieee80211_txb *txb,
				   struct net_device *dev, int pri);
          virtual mbuf_t ieee80211_frag_cache_get(struct ieee80211_device *ieee,
							struct ieee80211_hdr_4addr *hdr);
	virtual struct ieee80211_frag_entry *ieee80211_frag_cache_find(struct
					ieee80211_device
					*ieee,
					unsigned int seq,
					unsigned int frag,
					u8 * src,
					u8 * dst);
	virtual int ieee80211_frag_cache_invalidate(struct ieee80211_device *ieee,
					struct ieee80211_hdr_4addr *hdr);
	virtual int ieee80211_is_eapol_frame(struct ieee80211_device *ieee,
						mbuf_t skb);	
	
	
	
	
	
			   

	virtual SInt32	getSSID(IO80211Interface *interface,
							struct apple80211_ssid_data *sd);
	
	virtual SInt32 getCHANNEL(IO80211Interface *interface,
							  struct apple80211_channel_data *cd);
	
	virtual SInt32 getBSSID(IO80211Interface *interface,
							struct apple80211_bssid_data *bd);
	
	virtual SInt32 getCARD_CAPABILITIES(IO80211Interface *interface,
										struct apple80211_capability_data *cd);
	
	virtual SInt32 getSTATE(IO80211Interface *interface,
							struct apple80211_state_data *sd);
	
	virtual SInt32 getRSSI(IO80211Interface *interface,
						   struct apple80211_rssi_data *rd);
	
	virtual SInt32 getPOWER(IO80211Interface *interface,
							struct apple80211_power_data *pd);
	
	virtual SInt32 getSCAN_RESULT(IO80211Interface *interface,
								  struct apple80211_scan_result **scan_result);
	
	//virtual SInt32 getASSOCIATE_RESULT(IO80211Interface *interface,									   struct apple80211_assoc_result_data *ard);
	
	virtual SInt32 getRATE(IO80211Interface *interface,
						   struct apple80211_rate_data *rd);
	
	virtual SInt32 getSTATUS_DEV(IO80211Interface *interface,
								 struct apple80211_status_dev_data *dd);
	
	virtual SInt32 getRATE_SET(IO80211Interface	*interface,
							   struct apple80211_rate_set_data *rd);
							   
	virtual SInt32	getASSOCIATION_STATUS( IO80211Interface * interface, struct apple80211_assoc_status_data * asd );

	virtual SInt32 setSCAN_REQ(IO80211Interface *interface,
							   struct apple80211_scan_data *sd);
	
	virtual SInt32 setASSOCIATE(IO80211Interface *interface,
								struct apple80211_assoc_data *ad);
	
	virtual SInt32 setPOWER(IO80211Interface *interface,
							struct apple80211_power_data *pd);
	
	virtual SInt32 setCIPHER_KEY(IO80211Interface *interface,
								 struct apple80211_key *key);
	
	virtual SInt32 setAUTH_TYPE(IO80211Interface *interface,
								struct apple80211_authtype_data *ad);
	
	virtual SInt32 setDISASSOCIATE(IO80211Interface	*interface);
	
	virtual SInt32 setSSID(IO80211Interface *interface,
						   struct apple80211_ssid_data *sd);
	
	virtual SInt32 setAP_MODE(IO80211Interface *interface,
							  struct apple80211_apmode_data *ad);

	virtual bool attachInterfaceWithMacAddress( void * macAddr, 
												UInt32 macLen, 
												IONetworkInterface ** interface, 
												bool doRegister ,
												UInt32 timeout  );

virtual void	dataLinkLayerAttachComplete( IO80211Interface * interface );

    virtual int ipw_load(struct ipw_priv *priv);
	virtual void ipw_add_scan_channels(struct ipw_priv *priv,
				  struct ipw_scan_request_ext *scan,
				  int scan_type);
	virtual int ipw_associate(ipw_priv *data);
	virtual void ipw_adapter_restart(ipw_priv *adapter);
	virtual void ipw_arc_release();
	virtual int ipw_stop_master();
	virtual void queue_te(int num, thread_call_func_t func, thread_call_param_t par, UInt32 timei, bool start);
	virtual void queue_td(int num , thread_call_func_t func);
	virtual void ipw_scan_check(ipw_priv *priv);
	virtual IOReturn message( UInt32 type, IOService * provider,
                              void * argument);
	virtual int ipw_associate_network(struct ipw_priv *priv,
				 struct ieee80211_network *network,
				 struct ipw_supported_rates *rates, int roaming);
	virtual void ipw_remove_current_network(struct ipw_priv *priv);
	virtual void ipw_abort_scan(struct ipw_priv *priv);
	virtual int ipw_disassociate(struct ipw_priv *data);
	virtual int ipw_set_tx_power(struct ipw_priv *priv);
	virtual void init_sys_config(struct ipw_sys_config *sys_config);
	virtual int init_supported_rates(struct ipw_priv *priv,
				struct ipw_supported_rates *rates);
	virtual void ipw_set_hwcrypto_keys(struct ipw_priv *priv);
	virtual void ipw_add_cck_scan_rates(struct ipw_supported_rates *rates,
				   u8 modulation, u32 rate_mask);
	virtual void ipw_add_ofdm_scan_rates(struct ipw_supported_rates *rates,
				    u8 modulation, u32 rate_mask);
	virtual void ipw_send_tgi_tx_key(struct ipw_priv *priv, int type, int index);
	virtual void ipw_send_wep_keys(struct ipw_priv *priv, int type);
	virtual void ipw_set_hw_decrypt_unicast(struct ipw_priv *priv, int level);
	virtual void ipw_set_hw_decrypt_multicast(struct ipw_priv *priv, int level);
	virtual const struct ieee80211_geo* ipw_get_geo(struct ieee80211_device *ieee);
	virtual void ipw_send_disassociate(struct ipw_priv *priv, int quiet);
	virtual int ipw_send_associate(struct ipw_priv *priv,
			      struct ipw_associate *associate);
	virtual void ipw_link_up(struct ipw_priv *priv);
	virtual void ipw_link_down(struct ipw_priv *priv);
	virtual const char* ipw_get_status_code(u16 status);
	virtual bool configureInterface(IONetworkInterface * netif);
	virtual int ipw_qos_activate(struct ipw_priv *priv,
			    struct ieee80211_qos_data *qos_network_data);
	virtual u8 ipw_qos_current_mode(struct ipw_priv *priv);
	virtual u32 ipw_qos_get_burst_duration(struct ipw_priv *priv);
	virtual void ipw_init_ordinals(struct ipw_priv *priv);
	virtual void ipw_reset_stats(struct ipw_priv *priv);
	virtual int ipw_get_ordinal(struct ipw_priv *priv, u32 ord, void *val, u32 * len);
	virtual void ipw_read_indirect(struct ipw_priv *priv, u32 addr, u8 * buf,
			       int num);
	virtual u32 ipw_get_current_rate(struct ipw_priv *priv);
	virtual u32 ipw_get_max_rate(struct ipw_priv *priv);
	virtual void ipw_gather_stats(struct ipw_priv *priv);
	virtual void average_add(struct average *avg, s16 val);
	virtual void ipw_rx_queue_reset(struct ipw_priv *priv, struct ipw_rx_queue *rxq);
	virtual int ipw_queue_reset(struct ipw_priv *priv);
	virtual void ipw_tx_queue_free(struct ipw_priv *priv);
	virtual void ipw_queue_tx_free(struct ipw_priv *priv, struct clx2_tx_queue *txq);
	virtual int ipw_queue_inc_wrap(int index, int n_bd);
	virtual void ipw_queue_tx_free_tfd(struct ipw_priv *priv,
				  struct clx2_tx_queue *txq);
	virtual int ipw_queue_tx_init(struct ipw_priv *priv,
			     struct clx2_tx_queue *q,
			     int count, u32 read, u32 write, u32 base, u32 size);
	virtual void ipw_queue_init(struct ipw_priv *priv, struct clx2_queue *q,
			   int count, u32 read, u32 write, u32 base, u32 size);
	virtual void ipw_rx_queue_replenish(void *data);		   
	virtual void ipw_rx_queue_restock(struct ipw_priv *priv);		   
	virtual int is_network_packet(struct ipw_priv *priv,
			     struct ieee80211_hdr_4addr *header);
	virtual int ipw_is_multicast_ether_addr(const u8 * addr);
	virtual s16 exponential_average(s16 prev_avg, s16 val, u8 depth);
	virtual int __ipw_send_cmd(struct ipw_priv *priv, struct host_cmd *cmd);
	virtual int ipw_send_cmd_simple(struct ipw_priv *priv, u8 command);
	virtual int ipw_send_cmd_pdu(struct ipw_priv *priv, u8 command, u8 len,
			    void *data);
	// add kazu
    virtual void ieee80211_rx_mgt(struct ieee80211_device *ieee, 
									  struct ieee80211_hdr_4addr *header,
									   struct ieee80211_rx_stats *stats);
	virtual void ieee80211_process_probe_response(struct ieee80211_device *ieee,
        struct ieee80211_probe_response *new_beacon,
        struct ieee80211_rx_stats *stats); // copy from isr_process_probe_response in ipw-0.2
	virtual int ieee80211_network_init(struct ieee80211_device *ieee,
	                 struct ieee80211_probe_response *beacon,
					 struct ieee80211_network *network,
					 struct ieee80211_rx_stats *stats); 
    virtual int ieee80211_parse_info_param(struct ieee80211_info_element
				      *info_element, u16 length,
				      struct ieee80211_network *network);
	virtual int ieee80211_handle_assoc_resp(struct ieee80211_device *ieee, struct ieee80211_assoc_response
				       *frame, struct ieee80211_rx_stats *stats);
	virtual int is_beacon(__le16 fc)
	{
		return (WLAN_FC_GET_STYPE(le16_to_cpu(fc)) == IEEE80211_STYPE_BEACON);
	}
	virtual int is_same_network(struct ieee80211_network *src,
				  struct ieee80211_network *dst)
	{
		return ((src->ssid_len == dst->ssid_len) &&
		(src->channel == dst->channel) &&
		!compare_ether_addr(src->bssid, dst->bssid) &&
		!memcmp(src->ssid, dst->ssid, src->ssid_len));
	}
	virtual unsigned compare_ether_addr(const u8 *_a, const u8 *_b)
	{
		const u16 *a = (const u16 *) _a;
		const u16 *b = (const u16 *) _b;

		if(ETH_ALEN != 6) return -1;
		return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2])) != 0;
	}
	virtual void update_network(struct ieee80211_network *dst,
				  struct ieee80211_network *src);
	virtual void ipw_handle_mgmt_packet(struct ipw_priv *priv,
				   struct ipw_rx_mem_buffer *rxb,
				   struct ieee80211_rx_stats *stats);			  
	virtual void ieee80211_network_reset(struct ieee80211_network *network)
	{
		if (!network)
		return;

		if (network->ibss_dfs) {
		kfree(network->ibss_dfs);
		network->ibss_dfs = NULL;
		}
	}			  
	virtual u8 ipw_add_station(struct ipw_priv *priv, u8 * bssid);
	virtual void ipw_write_direct(struct ipw_priv *priv, u32 addr, void *buf,
			     int num);
	virtual void ipw_adhoc_check(void *data);
	virtual bool ipw_handle_data_packet(struct ipw_priv *priv,
				   struct ipw_rx_mem_buffer *rxb,
				   struct ieee80211_rx_stats *stats);
	virtual void __ipw_led_activity_on(struct ipw_priv *priv);
	virtual int ipw_qos_association(struct ipw_priv *priv,
			       struct ieee80211_network *network);
	virtual int ipw_qos_set_info_element(struct ipw_priv *priv);
	virtual int ipw_send_qos_info_command(struct ipw_priv *priv, struct ieee80211_qos_information_element
				     *qos_param);
	virtual int ipw_handle_assoc_response(struct net_device *dev,
				     struct ieee80211_assoc_response *resp,
				     struct ieee80211_network *network);		
	virtual int ipw_qos_association_resp(struct ipw_priv *priv,
				    struct ieee80211_network *network);
	virtual int ipw_handle_beacon(struct net_device *dev,
			     struct ieee80211_beacon *resp,
			     struct ieee80211_network *network);
	virtual int ipw_handle_probe_response(struct net_device *dev,
				     struct ieee80211_probe_response *resp,
				     struct ieee80211_network *network);
	virtual int ipw_qos_handle_probe_response(struct ipw_priv *priv,
					 int active_network,
					 struct ieee80211_network *network);
	virtual void ipw_bg_qos_activate(void *data);
	virtual int ieee80211_parse_qos_info_param_IE(struct ieee80211_info_element
					     *info_element,
					     struct ieee80211_network *network);
	virtual int ieee80211_read_qos_info_element(struct
					   ieee80211_qos_information_element
					   *element_info, struct ieee80211_info_element
					   *info_element);
	virtual int ieee80211_verify_qos_info(struct ieee80211_qos_information_element
				     *info_element, int sub_type);
	virtual int ieee80211_read_qos_param_element(struct ieee80211_qos_parameter_info
					    *element_param, struct ieee80211_info_element
					    *info_element);
	virtual int ieee80211_qos_convert_ac_to_parameters(struct
						  ieee80211_qos_parameter_info
						  *param_elm, struct
						  ieee80211_qos_parameters
						  *qos_param);
	virtual void ipw_handle_missed_beacon(struct ipw_priv *priv, int missed_count);
	virtual void ipw_merge_adhoc_network(void *data);
	virtual int ipw_find_adhoc_network(struct ipw_priv *priv,
				  struct ipw_network_match *match,
				  struct ieee80211_network *network,
				  int roaming);
	virtual void ipw_rebuild_decrypted_skb(struct ipw_priv *priv,
				      mbuf_t skb);
	virtual int ieee80211_rx(struct ieee80211_device *ieee, mbuf_t skb,
		 struct ieee80211_rx_stats *rx_stats);
	virtual mbuf_t mergePacket(mbuf_t m);
	virtual UInt32 outputPacket(mbuf_t m, void * param);
	virtual void ieee80211_txb_free(struct ieee80211_txb *txb);
	virtual u8 ipw_find_station(struct ipw_priv *priv, u8 * bssid);
	virtual int ipw_handle_probe_request(struct net_device *dev, struct ieee80211_probe_request
				    *frame, struct ieee80211_rx_stats *stats);
	virtual struct ipw_frame *ipw_get_free_frame(struct ipw_priv *priv);
	virtual int ipw_fill_beacon_frame(struct ipw_priv *priv,
				 struct ieee80211_hdr *hdr, u8 * dest, int left);
	virtual void *ieee80211_next_info_element(struct ieee80211_info_element
					 *info_element);
	virtual int ieee80211_xmit(mbuf_t skb, struct net_device *dev);
	virtual int ipw_tx_skb(struct ipw_priv *priv, struct ieee80211_txb *txb, int pri);
	virtual struct ieee80211_txb *ieee80211_alloc_txb(int nr_frags, int txb_size,
						 int headroom, int gfp_mask);
	virtual int ipw_is_qos_active(struct net_device *dev, mbuf_t skb);
	virtual void freePacket(mbuf_t  m,  IOOptionBits options = 0); 
	
	







int ieee80211_copy_snap(u8 * data, u16 h_proto)
{
	struct ieee80211_snap_hdr *snap;
	u8 *oui;

	snap = (struct ieee80211_snap_hdr *)data;
	snap->dsap = 0xaa;
	snap->ssap = 0xaa;
	snap->ctrl = 0x03;

	if (h_proto == 0x8137 || h_proto == 0x80f3)
		oui = P802_1H_OUI;
	else
		oui = RFC1042_OUI;
	snap->oui[0] = oui[0];
	snap->oui[1] = oui[1];
	snap->oui[2] = oui[2];

	*(u16 *) (data + SNAP_SIZE) = htons(h_proto);

	return SNAP_SIZE + sizeof(u16);
}

inline void *kzalloc(size_t size, unsigned flags)
{
	void *ret = kmalloc(size, flags);
	if (ret)
		memset(ret, 0, size);
	return ret;
}

inline UInt32 MEM_READ_4(UInt16 *base, UInt32 addr)
{
	CSR_WRITE_4(base, IWI_CSR_INDIRECT_ADDR, addr & IWI_INDIRECT_ADDR_MASK);
	return CSR_READ_4(base, IWI_CSR_INDIRECT_DATA);
}

inline UInt8 MEM_READ_1(UInt16 *base, UInt32 addr)
{
	CSR_WRITE_4(base, IWI_CSR_INDIRECT_ADDR, addr);
	return CSR_READ_1(base, IWI_CSR_INDIRECT_DATA);
}


#define CB_NUMBER_OF_ELEMENTS_SMALL 64

	IOPCIDevice *				fPCIDevice;		// PCI nub
	IOEthernetAddress			fEnetAddr;		// holds the mac address currently hardcoded
	IOWorkLoop *				fWorkLoop;		// the workloop
    IO80211Interface*			fNetif;			// ???
	//IONetworkInterface*			fNetif;
	IOInterruptEventSource *	fInterruptSrc;	// ???
//	IOTimerEventSource *		fWatchdogTimer;	// ???
	IOOutputQueue *				fTransmitQueue;	// ???
	UInt16 *					memBase;
	UInt32						event;
	u8 eeprom[0x100];
		// statistics
    IONetworkStats		*netStats;
    IOEthernetStats		*etherStats;
    
    // packet buffer variables
    //IOOutputQueue                               *transmitQueue;
    IOMbufNaturalMemoryCursor                   *rxMbufCursor;
    IOMbufNaturalMemoryCursor                   *txMbufCursor;
	
	IOMemoryMap	*				map;			// io memory map
	UInt8						irqNumber;		// irq number
	UInt16						vendorID;		// vendor ID shld be 8086 (intel)
	UInt16						deviceID;		// device ID
	UInt16						pciReg;			// revision
	IOPhysicalAddress			ioBase;			// map->getPhysicalAddress();
	IOMemoryDescriptor *		memDes;			// map->getMemoryDescriptor();
	IODeviceMemory *			mem;			// fPCIDevice->getDeviceMemoryWithIndex(index);
	OSDictionary *				mediumDict;
	IONetworkMedium	*			mediumTable[MEDIUM_TYPE_INVALID];
	//IO80211Interface2			ieee80211;
	iwi_cmd_ring				cmdq;
	iwi_rx_ring					rxq;
	iwi_tx_ring					txq;
	

	struct fw_image_desc sram_desc;
	struct alive_command_responce dino_alive;
	u8 nic_type;
	u32 led_activity_on;
	u32 led_activity_off;
	u32 led_association_on;
	u32 led_association_off;
	u32 led_ofdm_on;
	u32 led_ofdm_off;
	u32 status;
	u32 config;
	u32 iw_mode;
	u32 assoc_networkmode;
	struct ipw_sys_config sys_config;
	int pl;
	
	
	 int cmdlog;
	 int debug;
	 int channel;
	 int mode;
	 int disable2;
	 int early_up;
	 u32 ipw_debug_level;
	 int associate;
	 int auto_create;
	 int led;
	 int bt_coexist;
	 int hwcrypto;
	 int roaming;
	int antenna;
	struct ipw_supported_rates rates;
	u32 power;
	IOLock *mutex;
	IOSimpleLock *spin;
	u32 freq_band;
	u32 band;
	u32 modulation;
	u8 channel2;
	u16 rates_mask;
	u8 essid[IW_ESSID_MAX_SIZE];
	u8 essid_len;
	u8 speed_scan[MAX_SPEED_SCAN];
	u8 speed_scan_pos;
	//struct ipw_rx_queue *rxq;
	//struct clx2_tx_queue txq_cmd;
	//struct clx2_tx_queue txq[4];
	u16 rts_threshold;
	struct list_head network_list;
	struct list_head network_free_list;
	thread_call_t tlink[20];
	ipw_priv *priv;
	ieee80211_device ieee2;
	ipw_priv priv2;
	net_device net_dev2;
	int qos_enable;
	int qos_burst_enable;
	int qos_no_ack_mask;
	int burst_duration_CCK;
	int burst_duration_OFDM;
	ifnet_t fifnet;
	int countnonet;
	struct ieee80211_network nonets[20];
	UInt32 countb;
	int test_lock;
	int showip;

	
};





#endif

