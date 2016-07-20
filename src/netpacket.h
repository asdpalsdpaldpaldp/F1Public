#pragma once

typedef struct netadr_s
{
public:
	netadr_s()
	{
		SetIP(0);
		SetPort(0);
		SetType(NA_IP);
	}
	netadr_s(unsigned int unIP, uint16 usPort)
	{
		SetIP(unIP);
		SetPort(usPort);
		SetType(NA_IP);
	}
	netadr_s(const char *pch) { SetFromString(pch); }
	void Clear(); // invalids Address

	void SetType(netadrtype_t type);
	void SetPort(unsigned short port);
	bool SetFromSockadr(const struct sockaddr *s);
	void SetIP(uint8 b1, uint8 b2, uint8 b3, uint8 b4);
	void SetIP(unsigned int unIP); // Sets IP.  unIP is in host order (little-endian)
	void SetIPAndPort(unsigned int unIP, unsigned short usPort)
	{
		SetIP(unIP);
		SetPort(usPort);
	}
	void SetFromString(const char *pch, bool bUseDNS = false); // if bUseDNS is true then do a DNS lookup if needed

	bool CompareAdr(const netadr_s &a, bool onlyBase = false) const;
	bool CompareClassBAdr(const netadr_s &a) const;
	bool CompareClassCAdr(const netadr_s &a) const;

	netadrtype_t GetType() const;
	unsigned short GetPort() const;
	const char *ToString(bool onlyBase = false) const; // returns xxx.xxx.xxx.xxx:ppppp
	void ToSockadr(struct sockaddr *s) const;
	unsigned int GetIP() const;

	bool IsLocalhost() const;   // true, if this is the localhost IP
	bool IsLoopback() const;	// true if engine loopback buffers are used
	bool IsReservedAdr() const; // true, if this is a private LAN IP
	bool IsValid() const;		// ip & port != 0
	void SetFromSocket(int hSocket);
	// These function names are decorated because the Xbox360 defines macros for ntohl and htonl
	unsigned long addr_ntohl() const;
	unsigned long addr_htonl() const;
	bool operator==(const netadr_s &netadr) const { return (CompareAdr(netadr)); }
	bool operator<(const netadr_s &netadr) const;

public: // members are public to avoid to much changes
	netadrtype_t type;
	unsigned char ip[4];
	unsigned short port;
} netadr_t;

typedef struct netpacket_s
{
	netadr_t from;			   // sender IP
	int source;				   // received source
	double received;		   // received time
	unsigned char *data;	   // pointer to raw packet data
	bf_read message;		   // easy bitbuf data access
	int size;				   // size in BYTEs
	int wiresize;			   // size in BYTEs before decompression
	bool stream;			   // was send as stream
	struct netpacket_s *pNext; // for internal use, should be NULL in public
} netpacket_t;
