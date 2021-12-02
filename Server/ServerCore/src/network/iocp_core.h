#pragma once

/*----------------
	IocpObject
-----------------*/

class IocpObject : public std::enable_shared_from_this<IocpObject>
{
public:
	virtual auto GetHandle()-> HANDLE abstract;
	virtual auto Dispatch(class IocpEvent* iocpEvent, int32_t numOfBytes = 0) -> void abstract;
};

/*--------------
	IocpCore
---------------*/

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() const;

	bool		Register(IocpObjectRef iocpObject);
	bool		Dispatch(uint32_t timeoutMs = INFINITE);

private:
	HANDLE		_iocp_handle;
};