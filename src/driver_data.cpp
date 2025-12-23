
#include <cstddef> //size_t
#include <linux/nbd.h>
#include <vector>
#include <cstring>

#include <arpa/inet.h>

#include "driver_data.hpp"


#ifdef WORDS_BIGENDIAN
u_int64_t ntohll(u_int64_t a)
{
  return a;
}
#else
u_int64_t ntohll(u_int64_t a)
{
  u_int32_t lo = a & 0xffffffff;
  u_int32_t hi = a >> 32U;
  lo = ntohl(lo);
  hi = ntohl(hi);
  return ((u_int64_t)lo) << 32U | hi;
}
#endif
#define htonll ntohll

namespace abc
{
    DriverData::DriverData(nbd_request &request_) : 
        m_type((ntohl(request_.type) == NBD_CMD_READ ? READ : WRITE)),
        m_handle(*(size_t *)(request_.handle)),
        m_offset(size_t(ntohll(request_.from))),
        m_len(size_t(ntohl(request_.len))),
        m_status(SUCCESS),
        m_data(m_len)
    {}
    // may throw bad_alloc, bad_read
} // namespace abc
