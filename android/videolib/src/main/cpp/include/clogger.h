#ifndef CLOGGER_H
#define CLOGGER_H


typedef enum
{
	LEVEL_INFO = 0,
	LEVEL_DEBUG,
	LEVEL_WARN,
	LEVEL_ERROR
} LOG_LEVEL;

class Logger;

class CLogger
{
public:
	CLogger(const char* module);
	~CLogger();

	// global control: send log out to server.
	// Please call it at beginning of your program.
	static void setLogServer(const char* ip);

	// global control: set process name, it can be used to filter log.
	static void setProgName(const char* progName);

	// global control: set log level
	static void setLogLevel(bool info, bool debug, bool warn, bool error);

	static void blockLog( bool bBlock );

	// every module/class can set it's own module name, it can be used to filter log.
	void setModule(const char* name);

	void info(const char * format, ...);
	void warn(const char * format, ...);
	void error(const char * format, ...);
	void debug(const char * format, ...);

	void clearLog();

private:
	void sendLog(LOG_LEVEL level, const char* log);
	void writeBuf(char* buf, int len);

	char module_name[128];
	char log_buf[10*1024];

	int sktUdp;
};

#endif // CLOGGER_H
