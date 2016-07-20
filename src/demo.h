#pragma once

#include "netpacket.h"

class bf_read;
struct netpacket_s;
typedef netpacket_s netpacket_t;

typedef int ServerClass, CDemoFile;

#define FDEMO_NORMAL 0
#define FDEMO_USE_ORIGIN2 (1 << 0)
#define FDEMO_USE_ANGLES2 (1 << 1)
#define FDEMO_NOINTERP (1 << 2) // don't interpolate between this an last view

class IDemoRecorder
{
public:
	~IDemoRecorder() {}

	virtual CDemoFile *GetDemoFile() = 0;
	virtual int GetRecordingTick(void) = 0;

	virtual void StartRecording(const char *filename, bool bContinuously) = 0;
	virtual void SetSignonState(int state) = 0;
	virtual bool IsRecording(void) = 0;
	virtual void PauseRecording(void) = 0;
	virtual void ResumeRecording(void) = 0;
	virtual void StopRecording(void) = 0;

	virtual void RecordCommand(const char *cmdstring) = 0;		 // record a console command
	virtual void RecordUserInput(int cmdnumber) = 0;			 // record a user input command
	virtual void RecordMessages(bf_read &data, int bits) = 0;	// add messages to current packet
	virtual void RecordPacket(void) = 0;						 // packet finished, write all recorded stuff to file
	virtual void RecordServerClasses(ServerClass *pClasses) = 0; // packet finished, write all recorded stuff to file

	virtual void ResetDemoInterpolation() = 0;
};

class IDemoPlayer
{
public:
	virtual ~IDemoPlayer(){};

	virtual CDemoFile *GetDemoFile() = 0;
	virtual int GetPlaybackTick(void) = 0;
	virtual int GetTotalTicks(void) = 0;

	virtual bool StartPlayback(const char *filename, bool bAsTimeDemo) = 0;

	virtual bool IsPlayingBack(void) = 0;	 // true if demo loaded and playing back
	virtual bool IsPlaybackPaused(void) = 0;  // true if playback paused
	virtual bool IsPlayingTimeDemo(void) = 0; // true if playing back in timedemo mode
	virtual bool IsSkipping(void) = 0;		  // true, if demo player skiiping trough packets
	virtual bool CanSkipBackwards(void) = 0;  // true if demoplayer can skip backwards

	virtual void SetPlaybackTimeScale(float timescale) = 0; // sets playback timescale
	virtual float GetPlaybackTimeScale(void) = 0;			// get playback timescale

	virtual void PausePlayback(float seconds) = 0;						// pause playback n seconds, -1 = forever
	virtual void SkipToTick(int tick, bool bRelative, bool bPause) = 0; // goto a specific tick, 0 = start, -1 = end
	virtual void ResumePlayback(void) = 0;								// resume playback
	virtual void StopPlayback(void) = 0;								// stop playback, close file
	virtual void InterpolateViewpoint() = 0;							// override viewpoint
	virtual netpacket_t *ReadPacket(void) = 0;							// read packet from demo file

	virtual void ResetDemoInterpolation() = 0;
};

struct democmdinfo_t
{
	// Default constructor
	democmdinfo_t()
	{
		flags = FDEMO_NORMAL;
		viewOrigin.Init();
		viewAngles.Init();
		localViewAngles.Init();

		// Resampled origin/angles
		viewOrigin2.Init();
		viewAngles2.Init();
		localViewAngles2.Init();
	}

	// Copy constructor
	// Assignment
	democmdinfo_t &operator=(const democmdinfo_t &src)
	{
		if(this == &src)
			return *this;

		flags			= src.flags;
		viewOrigin	   = src.viewOrigin;
		viewAngles	   = src.viewAngles;
		localViewAngles  = src.localViewAngles;
		viewOrigin2	  = src.viewOrigin2;
		viewAngles2	  = src.viewAngles2;
		localViewAngles2 = src.localViewAngles2;

		return *this;
	}

	const Vector &GetViewOrigin()
	{
		if(flags & FDEMO_USE_ORIGIN2)
		{
			return viewOrigin2;
		}
		return viewOrigin;
	}

	const Vector &GetViewAngles()
	{
		if(flags & FDEMO_USE_ANGLES2)
		{
			return viewAngles2;
		}
		return viewAngles;
	}
	const Vector &GetLocalViewAngles()
	{
		if(flags & FDEMO_USE_ANGLES2)
		{
			return localViewAngles2;
		}
		return localViewAngles;
	}

	void Reset(void)
	{
		flags			= 0;
		viewOrigin2	  = viewOrigin;
		viewAngles2	  = viewAngles;
		localViewAngles2 = localViewAngles;
	}

	int flags;

	// original origin/viewangles
	Vector viewOrigin;
	Vector viewAngles;
	Vector localViewAngles;

	// Resampled origin/viewangles
	Vector viewOrigin2;
	Vector viewAngles2;
	Vector localViewAngles2;
};

struct DemoCommandQueue
{
	DemoCommandQueue()
	{
		tick = 0;
	}
	int tick;
	democmdinfo_t info;
	int filepos;
};

class CDemoPlayer : public IDemoPlayer
{

public: // IDemoPlayer interface implementation:
	CDemoPlayer();
	~CDemoPlayer();

	CDemoFile *GetDemoFile();

	bool StartPlayback(const char *filename, bool bAsTimeDemo);
	void PausePlayback(float seconds);
	void SkipToTick(int tick, bool bRelative, bool bPause);
	void ResumePlayback(void);
	void StopPlayback(void);

	int GetPlaybackTick(void);
	float GetPlaybackTimeScale(void);
	int GetTotalTicks(void);

	bool IsPlayingBack(void);
	bool IsPlaybackPaused(void);
	bool IsPlayingTimeDemo(void);
	bool IsSkipping(void);
	bool CanSkipBackwards(void) { return false; }

	void SetPlaybackTimeScale(float timescale);
	void InterpolateViewpoint(); // override viewpoint
	netpacket_t *ReadPacket(void);
	void ResetDemoInterpolation(void);

public: // other public functions
	void MarkFrame(float flFPSVariability);
	void SetBenchframe(int tick, const char *filename);
	void ResyncDemoClock(void);
	bool CheckPausedPlayback(void);
	void WriteTimeDemoResults(void);
	bool ParseAheadForInterval(int curtick, int intervalticks);
	void InterpolateDemoCommand(int targettick, DemoCommandQueue &prev, DemoCommandQueue &next);

protected:
	bool OverrideView(democmdinfo_t &info);

public:
	CDemoFile m_DemoFile;
	int m_nStartTick; // For synchronizing playback during timedemo.
	int m_nPreviousTick;
	netpacket_t m_DemoPacket; // last read demo packet
	bool m_bPlayingBack;	  // true if demo playback
	bool m_bPlaybackPaused;   // true if demo is paused right now
	float m_flAutoResumeTime; // how long do we pause demo playback
	float m_flPlaybackRateModifier;
	int m_nSkipToTick; // skip to tick ASAP, -1 = off

	// view origin/angle interpolation:
	CUtlVector<DemoCommandQueue> m_DestCmdInfo;
	democmdinfo_t m_LastCmdInfo;
	bool m_bInterpolateView;
	bool m_bResetInterpolation;

	// timedemo stuff:
	bool m_bTimeDemo;			   // ture if in timedemo mode
	int m_nTimeDemoStartFrame;	 // host_tickcount at start
	double m_flTimeDemoStartTime;  // Sys_FloatTime() at second frame of timedemo
	float m_flTotalFPSVariability; // Frame rate variability
	int m_nTimeDemoCurrentFrame;   // last frame we read a packet

	// benchframe stuff
	int m_nSnapshotTick;
	char m_SnapshotFilename[MAX_OSPATH];
};

class CDemoRecorder : public IDemoRecorder
{
public:
	~CDemoRecorder();
	CDemoRecorder();

	CDemoFile *GetDemoFile(void);
	int GetRecordingTick(void);

	void StartRecording(const char *filename, bool bContinuously);
	void SetSignonState(int state);
	bool IsRecording(void);
	void PauseRecording(void);
	void ResumeRecording(void);
	void StopRecording(void);

	void RecordCommand(const char *cmdstring);		 // record a console command
	void RecordUserInput(int cmdnumber);			 // record a user input command
	void RecordMessages(bf_read &data, int bits);	// add messages to current packet
	void RecordPacket(void);						 // packet finished, write all recorded stuff to file
	void RecordServerClasses(ServerClass *pClasses); // packet finished, write all recorded stuff to file

	void ResetDemoInterpolation(void);

protected:
	void ResyncDemoClock(void);
	void StartupDemoFile(void);
	void StartupDemoHeader(void);
	void CloseDemoFile(void);
	void GetClientCmdInfo(democmdinfo_t &info);
	void WriteDemoCvars(void);
	void WriteBSPDecals(void);
	void WriteMessages(bf_write &message);
	bool ComputeNextIncrementalDemoFilename(char *name, int namesize);

public:
	CDemoFile m_DemoFile;

	// For synchronizing playback during timedemo.
	int m_nStartTick; // host_tickcount when starting recoring

	// Name of demo file we are appending onto.
	char m_szDemoBaseName[MAX_OSPATH];

	// For demo file handle
	bool m_bIsDemoHeader;  // true, if m_hDemoFile is the header file
	bool m_bCloseDemoFile; // if true, demo file will be closed ASAP

	bool m_bRecording;	// true if recording
	bool m_bContinuously; // start new record after each
	int m_nDemoNumber;	// demo count, increases each changelevel
	int m_nFrameCount;	// # of demo frames in this segment.

	bf_write m_MessageData; // temp buffer for all network messages

	bool m_bResetInterpolation;
};
