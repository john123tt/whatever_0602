#ifndef GPR_GLOBAL_H
#define GPR_GLOBAL_H

#if _MSC_VER >=1600
#pragma execution_character_set("utf-8")
#endif

namespace GPR {
const auto KHZ = 1000.0;
const auto MHZ = 1000.0 * KHZ;
const auto GHZ = 1000.0 * MHZ;

const auto LAUNCH_CHANNEL_COUNT = 8;
const auto RECEIVE_CHANNEL_COUNT = 7;
}

#endif // GLOBAL_H
