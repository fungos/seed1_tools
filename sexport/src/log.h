#ifndef __LOG_H__
#define __LOG_H__

extern void Log(const char *message, ...);
extern void Error(int returnCode, const char *message, ...);
extern void DebugInfo(const char *message, ...);

#endif // __LOG_H__
