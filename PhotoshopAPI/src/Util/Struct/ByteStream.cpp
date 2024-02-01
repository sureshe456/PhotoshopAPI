#include "ByteStream.h"

#include "Profiling/Perf/Instrumentor.h"

#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>

PSAPI_NAMESPACE_BEGIN


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void ByteStream::setOffset(const uint64_t offset)
{
	if (offset > m_Size)
	{
		PSAPI_LOG_ERROR("ByteStream", "Trying to access illegal offset, maximum is %" PRIu64 " but got %" PRIu64 " instead", m_Size, offset);
	}
	m_Offset = offset;
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void ByteStream::read(char* buffer, uint64_t size)
{
	PROFILE_FUNCTION();
	if (m_Offset + size > m_Size)
	{
		PSAPI_LOG_ERROR("ByteStream", "Trying to read too much data, maximum is %" PRIu64 " but got %" PRIu64 " instead", m_Size, m_Offset + size);			
	}

	// Use memcpy to copy data from m_Buffer to the provided buffer
	std::memcpy(buffer, m_Buffer.data() + m_Offset, size);

	m_Offset += size;
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
void ByteStream::setOffsetAndRead(char* buffer, uint64_t offset, uint64_t size)
{
	PROFILE_FUNCTION();
	std::lock_guard<std::mutex> guard(m_Mutex);
	if (offset > m_Size)
	{
		PSAPI_LOG_ERROR("ByteStream", "Trying to access illegal offset, maximum is %" PRIu64 " but got %" PRIu64 " instead", m_Size, offset);
	}
	m_Offset = offset;

	if (offset + size > m_Size)
	{
		PSAPI_LOG_ERROR("ByteStream", "Trying to read too much data, maximum is %" PRIu64 " but got %" PRIu64 " instead", m_Size, m_Offset + size);			
	}

	// Use memcpy to copy data from m_Buffer to the provided buffer
	std::memcpy(buffer, m_Buffer.data() + m_Offset, size);

	m_Offset += size;
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
ByteStream::ByteStream(File& document, const uint64_t offset, const uint64_t size)
{
	PROFILE_FUNCTION();
	{
		PROFILE_SCOPE("ByteStream MemAlloc");
		m_Buffer = std::vector<uint8_t>(size);
	}
	m_Size = size;
	document.setOffsetAndRead(reinterpret_cast<char*>(m_Buffer.data()), offset, size);
	m_FileOffset = offset;
}


PSAPI_NAMESPACE_END