#pragma once

#include "network.h"
#include "proto.h"

class KeyValues;

class INetMessage
{
public:
	virtual ~INetMessage(){};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void SetNetChannel(INetChannel *netchan) = 0; // netchannel this message is from/for
	virtual void SetReliable(bool state) = 0;			  // set to true if it's a reliable message

	virtual bool Process(void) = 0; // calles the recently set handler to process this message

	virtual bool ReadFromBuffer(bf_read &buffer) = 0; // returns true if parsing was OK
	virtual bool WriteToBuffer(bf_write &buffer) = 0; // returns true if writing was OK

	virtual bool IsReliable(void) const = 0; // true, if message needs reliable handling

	virtual int GetType(void) const = 0;		 // returns module specific header tag eg svc_serverinfo
	virtual int GetGroup(void) const = 0;		 // returns net message group of this message
	virtual const char *GetName(void) const = 0; // returns network message name, eg "svc_serverinfo"
	virtual INetChannel *GetNetChannel(void) const = 0;
	virtual const char *ToString(void) const = 0; // returns a human readable string about message content
};

class CNetMessage : public INetMessage
{
public:
	CNetMessage()
	{
		m_bReliable  = true;
		m_NetChannel = NULL;
	}

	virtual ~CNetMessage(){};

	virtual int GetGroup() const { return INetChannelInfo::GENERIC; }
	INetChannel *GetNetChannel() const { return m_NetChannel; }

	virtual void SetReliable(bool state) { m_bReliable = state; };
	virtual bool IsReliable() const { return m_bReliable; };
	virtual void SetNetChannel(INetChannel *netchan) { m_NetChannel = netchan; }
	virtual bool Process()
	{
		Assert(0);
		return false;
	}; // no handler set

protected:
	bool m_bReliable;		   // true if message should be send reliable
	INetChannel *m_NetChannel; // netchannel this message is from/for
};

#define PROCESS_NET_MESSAGE(name) \
	virtual bool Process##name(NET_##name *msg)

// server to client
#define PROCESS_SVC_MESSAGE(name) \
	virtual bool Process##name(SVC_##name *msg)

// client to server
#define PROCESS_CLC_MESSAGE(name) \
	virtual bool Process##name(CLC_##name *msg)

#define PROCESS_MM_MESSAGE(name) \
	virtual bool Process##name(MM_##name *msg)

// these arent important but the compiler doesnt like us if we dont have them

#define DECLARE_BASE_MESSAGE(msgtype)           \
public:                                         \
	bool ReadFromBuffer(bf_read &buffer);       \
	bool WriteToBuffer(bf_write &buffer);       \
	const char *ToString() const { return ""; } \
	int GetType() const { return msgtype; }     \
	const char *GetName() const { return #msgtype; }

#define DECLARE_NET_MESSAGE(name)     \
	DECLARE_BASE_MESSAGE(net_##name); \
	void *m_pMessageHandler;          \
	bool Process() { return true; }

#define DECLARE_SVC_MESSAGE(name)     \
	DECLARE_BASE_MESSAGE(svc_##name); \
	void *m_pMessageHandler;          \
	bool Process() { return true; }

#define DECLARE_CLC_MESSAGE(name)     \
	DECLARE_BASE_MESSAGE(clc_##name); \
	void *m_pMessageHandler;          \
	bool Process() { return true; }

#define DECLARE_MM_MESSAGE(name)     \
	DECLARE_BASE_MESSAGE(mm_##name); \
	void *m_pMessageHandler;         \
	bool Process() { return true; }

class NET_Tick;
class NET_StringCmd;
class NET_SetConVar;
class NET_SignonState;

class INetMessageHandler
{
public:
	virtual ~INetMessageHandler(void){};

	PROCESS_NET_MESSAGE(Tick) = 0;
	//virtual bool ProcessTick(NET_Tick *msg) = 0;
	PROCESS_NET_MESSAGE(StringCmd) = 0;
	PROCESS_NET_MESSAGE(SetConVar) = 0;
	PROCESS_NET_MESSAGE(SignonState) = 0;
};

class SVC_Print;
class SVC_ServerInfo;
class SVC_SendTable;
class SVC_ClassInfo;
class SVC_SetPause;
class SVC_CreateStringTable;
class SVC_UpdateStringTable;
class SVC_VoiceInit;
class SVC_VoiceData;
class SVC_Sounds;
class SVC_SetView;
class SVC_FixAngle;
class SVC_CrosshairAngle;
class SVC_BSPDecal;
class SVC_GameEvent;
class SVC_UserMessage;
class SVC_EntityMessage;
class SVC_PacketEntities;
class SVC_TempEntities;
class SVC_Prefetch;
class SVC_Menu;
class SVC_GameEventList;
class SVC_GetCvarValue;

class IServerMessageHandler : public INetMessageHandler
{
public:
	virtual ~IServerMessageHandler(void){};

	PROCESS_SVC_MESSAGE(Print) = 0;
	PROCESS_SVC_MESSAGE(ServerInfo) = 0;
	PROCESS_SVC_MESSAGE(SendTable) = 0;
	PROCESS_SVC_MESSAGE(ClassInfo) = 0;
	PROCESS_SVC_MESSAGE(SetPause) = 0;
	PROCESS_SVC_MESSAGE(CreateStringTable) = 0;
	PROCESS_SVC_MESSAGE(UpdateStringTable) = 0;
	PROCESS_SVC_MESSAGE(VoiceInit) = 0;
	PROCESS_SVC_MESSAGE(VoiceData) = 0;
	PROCESS_SVC_MESSAGE(Sounds) = 0;
	PROCESS_SVC_MESSAGE(SetView) = 0;
	PROCESS_SVC_MESSAGE(FixAngle) = 0;
	PROCESS_SVC_MESSAGE(CrosshairAngle) = 0;
	PROCESS_SVC_MESSAGE(BSPDecal) = 0;
	PROCESS_SVC_MESSAGE(GameEvent) = 0;
	PROCESS_SVC_MESSAGE(UserMessage) = 0;
	PROCESS_SVC_MESSAGE(EntityMessage) = 0;
	PROCESS_SVC_MESSAGE(PacketEntities) = 0;
	PROCESS_SVC_MESSAGE(TempEntities) = 0;
	PROCESS_SVC_MESSAGE(Prefetch) = 0;
	PROCESS_SVC_MESSAGE(Menu) = 0;
	PROCESS_SVC_MESSAGE(GameEventList) = 0;
	PROCESS_SVC_MESSAGE(GetCvarValue) = 0;
};

// NET_

class NET_StringCmd : public CNetMessage
{
	DECLARE_NET_MESSAGE(StringCmd);

	int GetGroup() const { return INetChannelInfo::STRINGCMD; }

	NET_StringCmd() { m_szCommand = nullptr; };
	NET_StringCmd(const char *cmd) { m_szCommand = cmd; };

public:
	const char *m_szCommand; // execute this command

private:
	char m_szCommandBuffer[1024]; // buffer for received messages
};

#if _MSC_VER == 1900
class NET_SetConVar : public CNetMessage
{
	DECLARE_NET_MESSAGE(SetConVar);

	int GetGroup() const { return INetChannelInfo::STRINGCMD; }

	NET_SetConVar() { m_ConVars = CUtlVector<cvar_t>(); };
	NET_SetConVar(const char *name, const char *value)
	{
		m_ConVars = CUtlVector<cvar_t>();
		cvar_t cvar;
		strncpy(cvar.name, name, MAX_OSPATH);
		strncpy(cvar.value, value, MAX_OSPATH);
		m_ConVars.AddToTail(cvar);
	}

public:
	CUtlVector<cvar_t> m_ConVars;
};
#else
#pragma message("comiling under vs2013")
class NET_SetConVar
{
public:
	NET_SetConVar( const char *name, const char *value );

private:
	int32_t pad0[ 9 ];
};
#endif

// SVC_

class SVC_GetCvarValue : public CNetMessage
{
public:
	DECLARE_SVC_MESSAGE(GetCvarValue);

	QueryCvarCookie_t m_iCookie;
	const char *m_szCvarName; // The sender sets this, and it automatically points it at m_szCvarNameBuffer when receiving.

private:
	char m_szCvarNameBuffer[256];
};

// CLC_

class CLC_RespondCvarValue : public CNetMessage
{
public:
	DECLARE_CLC_MESSAGE(RespondCvarValue)

	QueryCvarCookie_t m_iCookie;

	const char *m_szCvarName;
	const char *m_szCvarValue; // The sender sets this, and it automatically points it at m_szCvarNameBuffer when receiving.

	EQueryCvarValueStatus m_eStatusCode;

private:
	char m_szCvarNameBuffer[256];
	char m_szCvarValueBuffer[256];
};

class CLC_CmdKeyValues : public CNetMessage
{

	// this = vftptr
	// this + 4 = ?
	// this + 8 = ?
	// this + 12 = KeyValues *

public:
	DECLARE_CLC_MESSAGE(CmdKeyValues)

	CLC_CmdKeyValues(KeyValues *kv);

private:
	DWORD unk;
	DWORD unk2;
	KeyValues *m_keyValues;
};
