#pragma once

/* macOS provides no <endian.h>. libcsp includes it unconditionally in ~18
 * source files for the glibc/BSD htobeNN/beNNtoh-style byte-swap helpers.
 * This shim maps those onto Darwin's <libkern/OSByteOrder.h> equivalents so
 * those files build unmodified. Only added to the include path for Apple
 * builds -- see the `if(APPLE)` block in ../CMakeLists.txt. */

#include <libkern/OSByteOrder.h>

#define htobe16(x) OSSwapHostToBigInt16(x)
#define htole16(x) OSSwapHostToLittleInt16(x)
#define be16toh(x) OSSwapBigToHostInt16(x)
#define le16toh(x) OSSwapLittleToHostInt16(x)

#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)

#define htobe64(x) OSSwapHostToBigInt64(x)
#define htole64(x) OSSwapHostToLittleInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)
