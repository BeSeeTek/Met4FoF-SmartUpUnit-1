#ifndef PTPD_DEP_H_
#define PTPD_DEP_H_
#ifdef __cplusplus
extern "C" {
#endif
/** \name Debug messages */
/**\{*/
#ifdef PTPD_DBGVV
#define PTPD_DBGV
#define PTPD_DBG
#define PTPD_ERR
#define DBGVV(...) printf("(V) " __VA_ARGS__)
#else
#define DBGVV(...)
#endif

#ifdef PTPD_DBGV
#define PTPD_DBG
#define PTPD_ERR
#define DBGV(...)  { TimeInternal tmpTime; getTime(&tmpTime); printf("(d %d.%09d) ", tmpTime.seconds, tmpTime.nanoseconds); printf(__VA_ARGS__); }
#else
#define DBGV(...)
#endif

#ifdef PTPD_DBG
#define PTPD_ERR
#define DBG(...)  { TimeInternal tmpTime; getTime(&tmpTime); printf("(D %d.%09d) ", tmpTime.seconds, tmpTime.nanoseconds); printf(__VA_ARGS__); }
#else
#define DBG(...)
#endif
/** \}*/

/** \name System messages */
/**\{*/
#ifdef PTPD_ERR
#define ERROR(...)  { TimeInternal tmpTime; getTime(&tmpTime); printf("(E %d.%09d) ", tmpTime.seconds, tmpTime.nanoseconds); printf(__VA_ARGS__); }
/* #define ERROR(...)  { printf("(E) "); printf(__VA_ARGS__); } */
#else
#define ERROR(...)
#endif
/** \}*/

/** \name Endian corrections */
/**\{*/

#if defined(PTPD_MSBF)
#define shift8(x,y)   ( (x) << ((3-y)<<3) )
#define shift16(x,y)  ( (x) << ((1-y)<<4) )
#elif defined(PTPD_LSBF)
#define shift8(x,y)   ( (x) << ((y)<<3) )
#define shift16(x,y)  ( (x) << ((y)<<4) )
#endif

#define flip16(x) htons(x)
#define flip32(x) htonl(x)

/* i don't know any target platforms that do not have htons and htonl,
	 but here are generic funtions just in case */
/*
#if defined(PTPD_MSBF)
#define flip16(x) (x)
#define flip32(x) (x)
#elif defined(PTPD_LSBF)
static inline int16_t flip16(int16_t x)
{
	 return (((x) >> 8) & 0x00ff) | (((x) << 8) & 0xff00);
}

static inline int32_t flip32(x)
{
	return (((x) >> 24) & 0x000000ff) | (((x) >> 8 ) & 0x0000ff00) |
				 (((x) << 8 ) & 0x00ff0000) | (((x) << 24) & 0xff000000);
}
#endif
*/

/** \}*/


/** \name Bit array manipulations */
/**\{*/
#define getFlag(flagField, mask) (bool)(((flagField)  & (mask)) == (mask))
#define setFlag(flagField, mask) (flagField) |= (mask)
#define clearFlag(flagField, mask) (flagField) &= ~(mask)
/* #define getFlag(x,y)  (bool)!!(  *(uint8_t*)((x)+((y)<8?1:0)) &   (1<<((y)<8?(y):(y)-8)) ) */
/* #define setFlag(x,y)    ( *(uint8_t*)((x)+((y)<8?1:0)) |=   1<<((y)<8?(y):(y)-8)  ) */
/* #define clearFlag(x,y)  ( *(uint8_t*)((x)+((y)<8?1:0)) &= ~(1<<((y)<8?(y):(y)-8)) ) */
/** \}*/

/** \name msg.c
 *-Pack and unpack PTP messages */
/**\{*/

void msgUnpackHeader(const char*, MsgHeader*);
void msgUnpackAnnounce(const char*, MsgAnnounce*);
void msgUnpackSync(const char*, MsgSync*);
void msgUnpackFollowUp(const char*, MsgFollowUp*);
void msgUnpackDelayReq(const char*, MsgDelayReq*);
void msgUnpackDelayResp(const char*, MsgDelayResp*);
void msgUnpackPDelayReq(const char*, MsgPDelayReq*);
void msgUnpackPDelayResp(const char*, MsgPDelayResp*);
void msgUnpackPDelayRespFollowUp(const char*, MsgPDelayRespFollowUp*);
void msgUnpackManagement(const char*, MsgManagement*);
void msgUnpackManagementPayload(const char *buf, MsgManagement *manage);
void msgPackHeader(const PtpClock*, char*);
void msgPackAnnounce(const PtpClock*, char*);
void msgPackSync(const PtpClock*, char*, const Timestamp*);
void msgPackFollowUp(const PtpClock*, char*, const Timestamp*);
void msgPackDelayReq(const PtpClock*, char*, const Timestamp*);
void msgPackDelayResp(const PtpClock*, char*, const MsgHeader*, const Timestamp*);
void msgPackPDelayReq(const PtpClock*, char*, const Timestamp*);
void msgPackPDelayResp(char*, const MsgHeader*, const Timestamp*);
void msgPackPDelayRespFollowUp(char*, const MsgHeader*, const Timestamp*);
int16_t msgPackManagement(const PtpClock*,  char*, const MsgManagement*);
int16_t msgPackManagementResponse(const PtpClock*,  char*, MsgHeader*, const MsgManagement*);
/** \}*/

/** \name net.c (Linux API dependent)
 * -Init network stuff, send and receive datas */
/**\{*/

bool  netInit(NetPath*, PtpClock*);
bool  netShutdown(NetPath*);
int32_t netSelect(NetPath*, const TimeInternal*);
ssize_t netRecvEvent(NetPath*, char*, TimeInternal*);
ssize_t netRecvGeneral(NetPath*, char*, TimeInternal*);
ssize_t netSendEvent(NetPath*, const char*, int16_t, TimeInternal*);
ssize_t netSendGeneral(NetPath*, const char*, int16_t);
ssize_t netSendPeerGeneral(NetPath*, const char*, int16_t);
ssize_t netSendPeerEvent(NetPath*, const char*, int16_t, TimeInternal*);
void netEmptyEventQ(NetPath *netPath);
/** \}*/

/** \name servo.c
 * -Clock servo */
/**\{*/

void initClock(PtpClock*);
void updatePeerDelay(PtpClock*, const TimeInternal*, bool);
void updateDelay(PtpClock*, const TimeInternal*, const TimeInternal*, const TimeInternal*);
void updateOffset(PtpClock *, const TimeInternal*, const TimeInternal*, const TimeInternal*);
void updateClock(PtpClock*);
/** \}*/

/** \name startup.c (Linux API dependent)
 * -Handle with runtime options */
/**\{*/
int16_t ptpdStartup(PtpClock*, RunTimeOpts*, ForeignMasterRecord*);
void ptpdShutdown(PtpClock *);
/** \}*/

/** \name sys.c (Linux API dependent)
 * -Manage timing system API */
/**\{*/
void displayStats(const PtpClock *ptpClock);
bool  nanoSleep(const TimeInternal*);
void getTime(TimeInternal*);
void setTime(const TimeInternal*);
void updateTime(const TimeInternal*);
bool  adjFreq(int32_t);
uint32_t getRand(uint32_t);
/** \}*/

/** \name timer.c (Linux API dependent)
 * -Handle with timers */
/**\{*/
void initTimer(void);
void timerStop(int32_t);
void timerStart(int32_t,  uint32_t);
bool timerExpired(int32_t);
/** \}*/


/* Test functions */

#ifdef __cplusplus
}
#endif
#endif /* PTPD_DEP_H_*/
