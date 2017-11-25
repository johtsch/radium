
//if_ether.h
#ifndef ether_header
#define ether_header
    struct	ether_header {
        u_char	ether_dhost[6];
        u_char	ether_shost[6];
        u_short	ether_type;
    };
#endif

// wird verwendet um auszuwerten welche Felder in einem Paketfilter gecheckt werden sollen
struct ether_filter{
    bool _dhost;
    bool _shost;
    bool _type;         
};

//if_arp.h
struct arp_header {
	unsigned short		ar_hrd;		/* format of hardware address	*/
	unsigned short		ar_pro;		/* format of protocol address	*/
	unsigned char	    ar_hln;		/* length of hardware address	*/
	unsigned char	    ar_pln;		/* length of protocol address	*/
	unsigned short		ar_op;		/* ARP opcode (command)		*/
	 /*
	  *	 Ethernet looks like this : This bit is variable sized however...
	  */
	unsigned char		ar_sha[ETH_ALEN];	/* sender hardware address	*/
	unsigned char		ar_sip[4];		    /* sender IP address		*/
	unsigned char		ar_tha[ETH_ALEN];	/* target hardware address	*/
	unsigned char		ar_tip[4];		    /* target IP address		*/
};

struct arp_filter{
    bool _hrd;
    bool _pro;
    bool _hln;
    bool _pln;
    bool _op;
    bool _sha;
    bool _sip;
    bool _tha;
    bool _tip;
};

/*
in packetfilter() :

bool packetfilter(){
    ...
    if(_dhost){
        if(ether_dhost == incommingpacket.ethernet.dhost){
            return true;
        }
    }
    ...
    return false;
}

*/