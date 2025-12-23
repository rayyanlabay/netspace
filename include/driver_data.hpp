/*
Description:
Author:
Reviewer:
Version:
    v0.1 - For mentor approval
*/
#ifndef __HRD31_DRIVER_DATA_HPP__
#define __HRD31_DRIVER_DATA_HPP__

#include <cstddef> //size_t

#include <vector>

#include <linux/nbd.h>

#include "ISerializable.hpp"


namespace hrd31
{
typedef enum {READ, WRITE} req_type_t;
typedef enum {SUCCESS, FAIL} status_t;

struct DriverData : public ISerializable
{
    explicit DriverData(nbd_request& request_); //may throw bad_alloc, bad_read
    ~DriverData() = default;
    DriverData(const DriverData& other_) = delete;
    DriverData& operator=(const DriverData& other_) = delete;

    req_type_t m_type;
	size_t m_handle;
	size_t m_offset;
	size_t m_len;
    status_t m_status;

    std::vector<char> m_data;
};

}//namespace hrd31

#endif //__HRD31_DRIVER_DATA_HPP__