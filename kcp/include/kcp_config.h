#ifndef __KCP_CONFIG_H__
#define __KCP_CONFIG_H__

#include <stdint.h>

// kcp 交流号标志, 即支持65536个会话
#define KCP_CONV_FLAG  0xFFFF0000

enum ConfigKey {
    CONFIG_KEY_NODELAY  = 0b0001,
    CONFIG_KEY_INTERVAL = 0b0010,
    CONFIG_KEY_RESEND   = 0b0100,
    CONFIG_KEY_NC       = 0b1000,
    CONFIG_KEY_ALL      = 0b1111,
};
typedef uint32_t config_key_t;

typedef struct KcpConfig {
    int32_t nodelay;    // 是否启用 nodelay模式, 0不启用; 1启用
    int32_t interval;   // 协议内部工作的 interval, 单位毫秒, 比如 10ms 或者 20ms
    int32_t resend;     // 快速重传模式, 默认0关闭, 可以设置2 (2次ACK跨越将会直接重传)
    int32_t nc;         // 是否关闭流控, 默认是0代表不关闭, 1代表关闭
} kcp_config_t;

#define KCP_CONFIG_NORMAL   (kcp_config_t){0, 40, 0, 0}
#define KCP_CONFIG_FAST     (kcp_config_t){0, 30, 2, 1}
#define KCP_CONFIG_FAST_2   (kcp_config_t){1, 20, 2, 1}
#define KCP_CONFIG_FAST_3   (kcp_config_t){1, 10, 2, 1}

static const uint32_t IKCP_RTO_NDL = 30;    // no delay min rto
static const uint32_t IKCP_RTO_MIN = 100;   // normal min rto
static const uint32_t IKCP_RTO_DEF = 200;
static const uint32_t IKCP_RTO_MAX = 60000;

static const uint32_t IKCP_ASK_SEND = 1;            // need to send IKCP_CMD_WASK
static const uint32_t IKCP_ASK_TELL = 2;            // need to send IKCP_CMD_WINS
static const uint32_t IKCP_WND_SND = 32;
static const uint32_t IKCP_WND_RCV = 128;           // must >= max fragment size
static const uint32_t IKCP_MTU_DEF = 1400;
static const uint32_t IKCP_ACK_FAST = 3;
static const uint32_t IKCP_INTERVAL = 100;
static const uint32_t IKCP_OVERHEAD = 24;
static const uint32_t IKCP_DEADLINK = 20;
static const uint32_t IKCP_THRESH_INIT = 2;
static const uint32_t IKCP_THRESH_MIN = 2;
static const uint32_t IKCP_PROBE_INIT = 7000;       // 7 secs to probe window size
static const uint32_t IKCP_PROBE_LIMIT = 120000;    // up to 120 secs to probe window
static const uint32_t IKCP_FASTACK_LIMIT = 5;       // max times to trigger fastack

static const uint32_t   KCP_INTERVAL_MAX    = 100;
static const uint32_t   KCP_INTERVAL_MIN    = 10;
static const int32_t    KCP_RESEND_MAX      = 5;
static const int32_t    KCP_RESEND_MIN      = 0;

#endif // __KCP_CONFIG_H__
