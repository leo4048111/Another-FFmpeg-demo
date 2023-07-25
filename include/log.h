//
// Created by 李茹玥 on 2023/7/12.
//

#ifndef DEMO2_LOG_H
#define DEMO2_LOG_H
/*
 * 打印日志信息
 */

#include <string>
using namespace std;

void LogInit();
void Serialize(const char* fmt,...);

#define __FILENAME__ (strchr(__FILE__,'/'))?(strchr(__FILE__,'/')+1):__FILE__
#define makePrefix(fmt) std::string(__FUNCTION__).append('() - ').append(fmt).c_str()
#define LogInfo(fmt,...) Serialize(makePrefix(fmt),##__VA_ARGS__)
#define LogDebug(fmt,...) Serialize(makePrefix(fmt), ##__VA_ARGS__)
#define LogNotice(fmt, ...) Serialize(makePrefix(fmt),##__VA_ARGS__)
#define LogError(fmt, ...) Serialize(makePrefix(fmt),##__VA_ARGS__)
#define FunEntry(...) LogDebug("Entry" ##__VA_ARGS__)
#define FunExit(...) LogDebug("Exit" ##_VA_ARGS__)

#endif //DEMO2_LOG_H
