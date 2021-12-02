#include "pch.h"
#include "iocp_core.h"
#include "iocp_event.h"

/*--------------
	IocpCore
---------------*/

IocpCore::IocpCore()
{
	_iocp_handle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocp_handle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocp_handle);
}

HANDLE IocpCore::GetHandle() const
{
	return _iocp_handle;
}

auto IocpCore::Register(const IocpObjectRef iocpObject) -> bool
{
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocp_handle, /*key*/0, 0);
}

auto IocpCore::Dispatch(const uint32_t timeoutMs) -> bool
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;	
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocp_handle, OUT &numOfBytes, OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		const IocpObjectRef iocpObject = iocpEvent->owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);

	}
	else
	{
		switch (int32_t errCode = ::WSAGetLastError())
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO : ·Î±× Âï±â
			const IocpObjectRef iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
