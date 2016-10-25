#pragma once
#include "steam/steamtypes.h"
#include "network.h"
#include "CClientFrameManager.h"
#include "event.h"
#include "CUtlLinkedList.h"
#include "proto.h"

#define STEAM_KEYSIZE 2048

#define MAX_AREA_STATE_BYTES 32
#define MAX_AREA_PORTAL_STATE_BYTES 24
#define MAX_PLAYER_NAME_LENGTH 32
#define MAX_CUSTOM_FILES 4 // max 4 files
#define MAX_CUSTOM_FILE_SIZE 131072
#define MAX_DEMOS 32
#define MAX_DEMONAME 32

// Resource counts;
#define MAX_MODEL_INDEX_BITS 10 // sent as a short
#define MAX_MODELS (1 << MAX_MODEL_INDEX_BITS)

#define MAX_GENERIC_INDEX_BITS 9
#define MAX_GENERIC (1 << MAX_GENERIC_INDEX_BITS)
#define MAX_DECAL_INDEX_BITS 9
#define MAX_BASE_DECALS (1 << MAX_DECAL_INDEX_BITS)

#define MAX_SOUND_INDEX_BITS 14
#define MAX_SOUNDS (1 << MAX_SOUND_INDEX_BITS)

struct model_t;
class ClientClass;

// dummy pointers for classes we dont want or need
// TODO impl if nesercary
typedef int INetworkStringTable, PackedEntity, C_ServerClassInfo, CNetworkStringTableContainer, CSfxTable, WaitForResourcesHandle_t;

class CEntityInfo
{
public:
	CEntityInfo()
	{
		m_nOldEntity  = -1;
		m_nNewEntity  = -1;
		m_nHeaderBase = -1;
	}
	virtual ~CEntityInfo(){};

	bool m_bAsDelta;
	CClientFrame *m_pFrom;
	CClientFrame *m_pTo;

	UpdateType m_UpdateType;

	int m_nOldEntity; // current entity index in m_pFrom
	int m_nNewEntity; // current entity index in m_pTo

	int m_nHeaderBase;
	int m_nHeaderCount;

	// unnecessary functions that can be impled later if necessercary

	//inline void	NextOldEntity(void)
	//{
	//	if(m_pFrom)
	//	{
	//		m_nOldEntity = m_pFrom->transmit_entity.FindNextSetBit(m_nOldEntity + 1);

	//		if(m_nOldEntity < 0)
	//		{
	//			// Sentinel/end of list....
	//			m_nOldEntity = ENTITY_SENTINEL;
	//		}
	//	}
	//	else
	//	{
	//		m_nOldEntity = ENTITY_SENTINEL;
	//	}
	//}

	//inline void	NextNewEntity(void)
	//{
	//	m_nNewEntity = m_pTo->transmit_entity.FindNextSetBit(m_nNewEntity + 1);

	//	if(m_nNewEntity < 0)
	//	{
	//		// Sentinel/end of list....
	//		m_nNewEntity = ENTITY_SENTINEL;
	//	}
	//}
};

// PostDataUpdate calls are stored in a list until all ents have been updated.
class CPostDataUpdateCall
{
public:
	int m_iEnt;
	DataUpdateType_t m_UpdateType;
};

class CEntityReadInfo : public CEntityInfo
{

public:
	CEntityReadInfo()
	{
		m_nPostDataUpdateCalls = 0;
		m_nLocalPlayerBits	 = 0;
		m_nOtherPlayerBits	 = 0;
		m_UpdateType			 = PreserveEnt;
	}

	bf_read *m_pBuf;
	int m_UpdateFlags; // from the subheader
	bool m_bIsEntity;

	int m_nBaseline;		 // what baseline index do we use (0/1)
	bool m_bUpdateBaselines; // update baseline while parsing snaphsot

	int m_nLocalPlayerBits; // profiling data
	int m_nOtherPlayerBits; // profiling data

	CPostDataUpdateCall m_PostDataUpdateCalls[MAX_EDICTS];
	int m_nPostDataUpdateCalls;
};

class CClockDriftMgr
{
	friend class CBaseClientState;

public:
	CClockDriftMgr();

	// Is clock correction even enabled right now?
	static bool IsClockCorrectionEnabled();

	// Clear our state.
	void Clear();

	// This is called each time a server packet comes in. It is used to correlate
	// where the server is in time compared to us.
	void SetServerTick(int iServerTick);

	// Pass in the frametime you would use, and it will drift it towards the server clock.
	float AdjustFrameTime(float inputFrameTime);

	// Returns how many ticks ahead of the server the client is.
	float GetCurrentClockDifference() const;

private:
	void ShowDebugInfo(float flAdjustment);

	// This scales the offsets so the average produced is equal to the
	// current average + flAmount. This way, as we add corrections,
	// we lower the average accordingly so we don't keep responding
	// as much as we need to after we'd adjusted it a couple times.
	void AdjustAverageDifferenceBy(float flAmountInSeconds);

private:
	enum
	{
		// This controls how much it smoothes out the samples from the server.
		NUM_CLOCKDRIFT_SAMPLES = 16
	};

	// This holds how many ticks the client is ahead each time we get a server tick.
	// We average these together to get our estimate of how far ahead we are.
	float m_ClockOffsets[NUM_CLOCKDRIFT_SAMPLES];
	int m_iCurClockOffset;

	int m_nServerTick; // Last-received tick from the server.
	int m_nClientTick; // The client's own tick counter (specifically, for interpolation during rendering).
					   // The server may be on a slightly different tick and the client will drift towards it.
};

class CPrecacheItem
{
public:
	CPrecacheItem()
	{
	}

public:
	/*tstring*/ int m_sClass;
	CHandle<CBaseEntity> m_hEntity;
};

struct AddAngle
{
	float total;
	float starttime;
};

// =========================================================================
// CBaseClientState
// This can only really be used as a place holder as the compiler optimizes many of the internals as static variables
// It is also possible that this has changed since 2007
// =========================================================================
class CBaseClientState : public INetChannelHandler, public IConnectionlessPacketHandler, public IServerMessageHandler
{

public:
	CBaseClientState() = delete;
	virtual ~CBaseClientState();

public: // IConnectionlessPacketHandler interface:
	virtual bool ProcessConnectionlessPacket(struct netpacket_s *packet);

public: // INetMsgHandler interface:
	virtual void ConnectionStart(INetChannel *chan);
	virtual void ConnectionClosing(const char *reason);
	virtual void ConnectionCrashed(const char *reason);

	virtual void PacketStart(int incoming_sequence, int outgoing_acknowledged){};
	virtual void PacketEnd(void){};

	virtual void FileReceived(const char *fileName, unsigned int transferID);
	virtual void FileRequested(const char *fileName, unsigned int transferID);
	virtual void FileDenied(const char *fileName, unsigned int transferID);

public: // IServerMessageHandlers
	PROCESS_NET_MESSAGE(Tick);
	PROCESS_NET_MESSAGE(StringCmd);
	PROCESS_NET_MESSAGE(SetConVar);
	PROCESS_NET_MESSAGE(SignonState);

	PROCESS_SVC_MESSAGE(Print);
	PROCESS_SVC_MESSAGE(ServerInfo);
	PROCESS_SVC_MESSAGE(SendTable);
	PROCESS_SVC_MESSAGE(ClassInfo);
	PROCESS_SVC_MESSAGE(SetPause);
	PROCESS_SVC_MESSAGE(CreateStringTable);
	PROCESS_SVC_MESSAGE(UpdateStringTable);
	PROCESS_SVC_MESSAGE(SetView);
	PROCESS_SVC_MESSAGE(PacketEntities);
	PROCESS_SVC_MESSAGE(Menu);
	PROCESS_SVC_MESSAGE(GameEventList);
	PROCESS_SVC_MESSAGE(GetCvarValue);

public:
	inline bool IsActive(void) const { return m_nSignonState == SIGNONSTATE_FULL; };
	inline bool IsConnected(void) const { return m_nSignonState >= SIGNONSTATE_CONNECTED; };
	virtual void Clear(void);
	virtual void FullConnect(netadr_t &adr); // a connection was established
	virtual void Connect(const char *adr);   // start a connection challenge
	virtual bool SetSignonState(int state, int count);
	virtual void Disconnect(bool bShowMainMenu = true);
	virtual void SendConnectPacket(int challengeNr, int authProtocol, int keySize, const char *encryptionKey, uint64 unGSSteamID, bool bGSSecure);
	virtual const char *GetCDKeyHash() { return "123"; }
	virtual void RunFrame(void);
	virtual void CheckForResend(void);
	virtual void InstallStringTableCallback(char const *tableName) {}
	virtual bool HookClientStringTable(char const *tableName) { return false; }
	virtual bool LinkClasses(void);
	virtual int GetConnectionRetryNumber() const = 0;
	virtual const char *GetClientName() = 0;

	static ClientClass *FindClientClass(const char *pClassName);

	// just a normal function, comment out
	//CClockDriftMgr& GetClockDriftMgr();

	int GetClientTickCount() const; // Get the client tick count.
	void SetClientTickCount(int tick);

	int GetServerTickCount() const;
	void SetServerTickCount(int tick);

	void SetClientAndServerTickCount(int tick);

	INetworkStringTable *GetStringTable(const char *name) const;

	PackedEntity *GetEntityBaseline(int iBaseline, int nEntityIndex);
	void SetEntityBaseline(int iBaseline, ClientClass *pClientClass, int index, char *packedData, int length);
	void CopyEntityBaseline(int iFrom, int iTo);
	void FreeEntityBaselines();
	bool GetClassBaseline(int iClass, void const **pData, int *pDatalen);
	ClientClass *GetClientClass(int i);

	void ForceFullUpdate(void);
	void SendStringCmd(const char *command);

	void ReadPacketEntities(CEntityReadInfo &u);

	virtual void ReadEnterPVS(CEntityReadInfo &u) = 0;
	virtual void ReadLeavePVS(CEntityReadInfo &u) = 0;
	virtual void ReadDeltaEnt(CEntityReadInfo &u) = 0;
	virtual void ReadPreserveEnt(CEntityReadInfo &u) = 0;
	virtual void ReadDeletions(CEntityReadInfo &u) = 0;

protected:
	bool InternalProcessStringCmd(NET_StringCmd *msg, bool bIsHLTV);

private:
	bool PrepareSteamConnectResponse(int keySize, const char *encryptionKey, uint64 unGSSteamID, bool bGSSecure, const netadr_t &adr, bf_write &msg);

public:
	// Connection to server.
	int m_Socket;				 // network socket
	INetChannel *m_NetChannel;   // Our sequenced channel to the remote server.
	unsigned int m_nChallengeNr; // connection challenge number
	double m_flConnectTime;		 // If gap of connect_time to net_time > 3000, then resend connect packet
	int m_nRetryNumber;			 // number of retry connection attemps
	char m_szRetryAddress[MAX_OSPATH];
	int m_nSignonState;		// see SIGNONSTATE_* definitions
	double m_flNextCmdTime; // When can we send the next command packet?
	int m_nServerCount;		// server identification for prespawns, must match the svs.spawncount which
							// is incremented on server spawning.  This supercedes svs.spawn_issued, in that
							// we can now spend a fair amount of time sitting connected to the server
							// but downloading models, sounds, etc.  So much time that it is possible that the
							// server might change levels again and, if so, we need to know that.
	int m_nCurrentSequence; // this is the sequence number of the current incoming packet

	CClockDriftMgr m_ClockDriftMgr;

	int m_nDeltaTick;  //	last valid received snapshot (server) tick
	bool m_bPaused;	// send over by server
	int m_nViewEntity; // cl_entitites[cl.viewentity] == player point of view

	int m_nPlayerSlot; // own player entity index-1. skips world. Add 1 to get cl_entitites index;

	char m_szLevelName[40];		 // for display on solo scoreboard
	char m_szLevelNameShort[40]; // removes maps/ and .bsp extension

	int m_nMaxClients; // max clients on server

	PackedEntity *m_pEntityBaselines[2][MAX_EDICTS]; // storing entity baselines

	// This stuff manages the receiving of data tables and instantiating of client versions
	// of server-side classes.
	C_ServerClassInfo *m_pServerClasses;
	int m_nServerClasses;
	int m_nServerClassBits;
	char m_szEncrytionKey[STEAM_KEYSIZE];
	unsigned int m_iEncryptionKeySize;

	CNetworkStringTableContainer *m_StringTableContainer;

	bool m_bRestrictServerCommands; // If true, then the server is only allowed to execute commands marked with FCVAR_SERVER_CAN_EXECUTE on the client.
	bool m_bRestrictClientCommands; // If true, then IVEngineClient::ClientCmd is only allowed to execute commands marked with FCVAR_CLIENTCMD_CAN_EXECUTE on the client.
};

class ClientClass;
class CPureServerWhitelist;

//-----------------------------------------------------------------------------
// Purpose: CClientState should hold all pieces of the client state
//   The client_state_t structure is wiped completely at every server signon
//-----------------------------------------------------------------------------
class CClientState : public CBaseClientState, public CClientFrameManager
{
	typedef struct CustomFile_s
	{
		CRC32_t crc;		//file CRC
		unsigned int reqID; // download request ID
	} CustomFile_t;

public:
	CClientState() = delete;
	~CClientState();

public: // IConnectionlessPacketHandler interface:
	bool ProcessConnectionlessPacket(struct netpacket_s *packet);

public: // CBaseClientState overrides:
	void Disconnect(bool bShowMainMenu);
	void FullConnect(netadr_t &adr);
	bool SetSignonState(int state, int count);
	void PacketStart(int incoming_sequence, int outgoing_acknowledged);
	void PacketEnd(void);
	void FileReceived(const char *fileName, unsigned int transferID);
	void FileRequested(const char *fileName, unsigned int transferID);
	void FileDenied(const char *fileName, unsigned int transferID);
	void ConnectionCrashed(const char *reason);
	void ConnectionClosing(const char *reason);
	const char *GetCDKeyHash(void);
	void SetFriendsID(unsigned int friendsID, const char *friendsName);
	void SendClientInfo(void);
	void InstallStringTableCallback(char const *tableName);
	bool HookClientStringTable(char const *tableName);
	bool InstallEngineStringTableCallback(char const *tableName);

	void StartUpdatingSteamResources();
	void CheckUpdatingSteamResources();
	void CheckFileCRCsWithServer();
	void FinishSignonState_New();
	void ConsistencyCheck(bool bForce);
	void RunFrame();

	void ReadEnterPVS(CEntityReadInfo &u);
	void ReadLeavePVS(CEntityReadInfo &u);
	void ReadDeltaEnt(CEntityReadInfo &u);
	void ReadPreserveEnt(CEntityReadInfo &u);
	void ReadDeletions(CEntityReadInfo &u);

	// In case the client DLL is using the old interface to set area bits,
	// copy what they've passed to us into the m_chAreaBits array (and 0xFF-out the m_chAreaPortalBits array).
	void UpdateAreaBits_BackwardsCompatible();

	// Used to be pAreaBits.
	unsigned char **GetAreaBits_BackwardCompatibility();

public: // IServerMessageHandlers
	PROCESS_NET_MESSAGE(Tick);

	PROCESS_NET_MESSAGE(StringCmd);
	PROCESS_SVC_MESSAGE(ServerInfo);
	PROCESS_SVC_MESSAGE(ClassInfo);
	PROCESS_SVC_MESSAGE(SetPause);
	PROCESS_SVC_MESSAGE(VoiceInit);
	PROCESS_SVC_MESSAGE(VoiceData);
	PROCESS_SVC_MESSAGE(Sounds);
	PROCESS_SVC_MESSAGE(FixAngle);
	PROCESS_SVC_MESSAGE(CrosshairAngle);
	PROCESS_SVC_MESSAGE(BSPDecal);
	PROCESS_SVC_MESSAGE(GameEvent);
	PROCESS_SVC_MESSAGE(UserMessage);
	PROCESS_SVC_MESSAGE(EntityMessage);
	PROCESS_SVC_MESSAGE(PacketEntities);
	PROCESS_SVC_MESSAGE(TempEntities);
	PROCESS_SVC_MESSAGE(Prefetch);

public:
	float m_flLastServerTickTime; // the timestamp of last message
	bool insimulation;

	int oldtickcount;	  // previous tick
	float m_tickRemainder; // client copy of tick remainder
	float m_frameTime;	 // dt of the current frame

	int lastoutgoingcommand; // Sequence number of last outgoing command
	int chokedcommands;		 // number of choked commands
	int last_command_ack;	// last command sequence number acknowledged by server
	int command_ack;		 // current command sequence acknowledged by server
	int m_nSoundSequence;	// current processed reliable sound sequence number

	//
	// information that is static for the entire time connected to a server
	//
	bool ishltv; // true if HLTV server/demo

	CRC32_t serverCRC;				// To determine if client is playing hacked .map. (entities lump is skipped)
	CRC32_t serverClientSideDllCRC; // To determine if client is playing on a hacked client dll.

	unsigned char m_chAreaBits[MAX_AREA_STATE_BYTES];
	unsigned char m_chAreaPortalBits[MAX_AREA_PORTAL_STATE_BYTES];
	bool m_bAreaBitsValid; // Have the area bits been set for this level yet?

	// refresh related state
	Vector viewangles;
	CUtlVector<AddAngle> addangle;
	float addangletotal;
	float prevaddangletotal;
	int cdtrack; // cd audio

	CustomFile_t m_nCustomFiles[MAX_CUSTOM_FILES]; // own custom files CRCs

	unsigned int m_nFriendsID;
	char m_FriendsName[MAX_PLAYER_NAME_LENGTH];

	CUtlFixedLinkedList<CEventInfo> events; // list of received events

	// demo loop control
	int demonum;						 // -1 = don't play demos
	char demos[MAX_DEMOS][MAX_DEMONAME]; // when not playing

public:
	// If 'insimulation', returns the time (in seconds) at the client's current tick.
	// Otherwise, returns the exact client clock.
	float GetTime() const;

	bool IsPaused() const;

	float GetFrameTime(void) const;
	void SetFrameTime(float dt) { m_frameTime = dt; }

	float GetClientInterpAmount(); // Formerly cl_interp, now based on cl_interp_ratio and cl_updaterate.

	void Clear(void);

	void DumpPrecacheStats(const char *name);

	// Public API to models
	model_t *GetModel(int index);
	void SetModel(int tableIndex);
	int LookupModelIndex(char const *name);

	// Public API to generic
	char const *GetGeneric(int index);
	void SetGeneric(int tableIndex);
	int LookupGenericIndex(char const *name);

	// Public API to sounds
	CSfxTable *GetSound(int index);
	char const *GetSoundName(int index);
	void SetSound(int tableIndex);
	int LookupSoundIndex(char const *name);
	void ClearSounds();

	// Public API to decals
	char const *GetDecalName(int index);
	void SetDecal(int tableIndex);

	// customization files code
	void CheckOwnCustomFiles();				 // load own custom file
	void CheckOthersCustomFile(CRC32_t crc); // check if we have to download custom files from server
	void AddCustomFile(int slot, const char *resourceFile);

public:
	INetworkStringTable *m_pModelPrecacheTable;
	INetworkStringTable *m_pGenericPrecacheTable;
	INetworkStringTable *m_pSoundPrecacheTable;
	INetworkStringTable *m_pDecalPrecacheTable;
	INetworkStringTable *m_pInstanceBaselineTable;
	INetworkStringTable *m_pLightStyleTable;
	INetworkStringTable *m_pUserInfoTable;
	INetworkStringTable *m_pServerStartupTable;
	INetworkStringTable *m_pDownloadableFileTable;

	CPrecacheItem model_precache[MAX_MODELS];
	CPrecacheItem generic_precache[MAX_GENERIC];
	CPrecacheItem sound_precache[MAX_SOUNDS];
	CPrecacheItem decal_precache[MAX_BASE_DECALS];

	WaitForResourcesHandle_t m_hWaitForResourcesHandle;
	bool m_bUpdateSteamResources;
	bool m_bShownSteamResourceUpdateProgress;
	bool m_bDownloadResources;
	bool m_bCheckCRCsWithServer;
	float m_flLastCRCBatchTime;

	// This is only kept around to print out the whitelist info if sv_pure is used.
	CPureServerWhitelist *m_pPureServerWhitelist;

private:
	// Note: This is only here for backwards compatibility. If it is set to something other than NULL,
	// then we'll copy its contents into m_chAreaBits in UpdateAreaBits_BackwardsCompatible.
	BYTE *m_pAreaBits;

	// Set to false when we first connect to a server and true later on before we
	// respond to a new whitelist.
	bool m_bMarkedCRCsUnverified;
}; //CClientState
