#pragma once

#include "PacketData.hpp"
#include "OtherRingBuffer.hpp"
#include <queue>
#include <mutex>
#include <windows.h>

#define GET_ACCEPTEX_SOCKADDRS( lpOutputBuffer, dwBytesReceived, lpLocalAddress, lpRemoteAddress ) \
    do { \
        sockaddr* pLocal = (sockaddr*)lpLocalAddress; \
        sockaddr* pRemote = (sockaddr*)lpRemoteAddress; \
        int localSize = sizeof(sockaddr_in); \
        int remoteSize = sizeof(sockaddr_in); \
        memcpy(pLocal, lpOutputBuffer, localSize); \
        memcpy(pRemote, lpOutputBuffer + localSize, remoteSize); \
    } while (0)

const int MAX_RINGBUFFER_SIZE = 8192;

class Connection
{
public:
	Connection(const SOCKET listenSocket_, const int index) : m_ListenSocket(listenSocket_), m_ClientIndex(index)
	{
		Init();
		BindAcceptEx();
		m_SendBuffer.Init(MAX_RINGBUFFER_SIZE, true);
		m_RecvBuffer.Init(MAX_RINGBUFFER_SIZE);
	}

	virtual ~Connection()
	{

	}

	void Init()
	{
		ZeroMemory(mAcceptBuf, 64);
		ZeroMemory(mRecvBuf, MAX_SOCKBUF);
		ZeroMemory(&m_RecvOverlapped, sizeof(stOverlappedEx));

		m_RecvOverlapped.m_userIndex = m_ClientIndex;

		m_IsConnected.store(false);
		m_ClientSocket = INVALID_SOCKET;
	}

	void ResetConnection()
	{
		Init();
		BindAcceptEx();

		return;
	}

	bool BindIOCP(const HANDLE hWorkIOCP_)
	{
		auto hIOCP = CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_ClientSocket),
			hWorkIOCP_,
			(ULONG_PTR)(this),
			0);

		if (hIOCP == INVALID_HANDLE_VALUE || hIOCP != hWorkIOCP_)
		{
			return false;
		}

		m_IsConnected.store(true);

		return true;
	}

	bool BindRecv()
	{
		if (m_IsConnected.load() == false)
		{
			return false;
		}

		m_RecvOverlapped.m_eOperation = eIOOperation::RECV;
		m_RecvOverlapped.m_wsaBuf.len = MAX_SOCKBUF;
		m_RecvOverlapped.m_wsaBuf.buf = mRecvBuf;

		ZeroMemory(&m_RecvOverlapped.m_overlapped, sizeof(WSAOVERLAPPED));

		DWORD flag = 0;
		DWORD bytes = 0;

		auto result = WSARecv(
			m_ClientSocket,
			&m_RecvOverlapped.m_wsaBuf,
			1,
			&bytes,
			&flag,
			&m_RecvOverlapped.m_overlapped,
			NULL
		);

		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			return false;
		}

		return true;
	}

	void SendMsg(PacketData* pData_)
	{
		if (pData_ == nullptr)
		{
			return;
		}

		if (m_IsConnected.load() == false)
		{
			delete pData_;
			return;
		}

		std::lock_guard<std::mutex> guard(m_mutex);

		if (!m_SendBuffer.enqueue(pData_->pData, pData_->dataSize))
		{
			std::cout << "full\n";
		}

		//if (m_SendBuffer.Size() == pData_->GetBlock()->dataSize) // 송신중인 데이터를 빼다보니 문제가 생길것 같다.
		unsigned int buffer4bytes = 0;
		CopyMemory(&buffer4bytes, m_SendingBuffer, sizeof(unsigned int));
		if (buffer4bytes == 0)
		{
			SendIO();
		}

		return;
	}

	bool SendIO()
	{
		int datasize = m_SendBuffer.dequeue(m_SendingBuffer, MAX_SOCKBUF);

		//std::cout << datasize << "bytes dequeued\n";
		//
		//for (int i = 0; i < datasize; i++)
		//{
		//	std::cout << (int)m_SendingBuffer[i] << ", ";
		//}
		//std::cout << '\n';

		ZeroMemory(&m_SendOverlapped, sizeof(stOverlappedEx));

		m_SendOverlapped.m_wsaBuf.len = datasize;
		m_SendOverlapped.m_wsaBuf.buf = m_SendingBuffer;
		m_SendOverlapped.m_eOperation = eIOOperation::SEND;
		m_SendOverlapped.m_userIndex = m_ClientIndex;

		DWORD dwRecvNumBytes = 0;

		auto result = WSASend(m_ClientSocket,
			&(m_SendOverlapped.m_wsaBuf),
			1,
			&dwRecvNumBytes,
			0,
			(LPWSAOVERLAPPED) & (m_SendOverlapped),
			NULL);

		if (result == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
		{
			return false;
		}

		//std::cout << "Connection::SendIO : SendMsg : " << m_SendingBuffer << '\n';

		return true;
	}

	void SendCompleted()
	{
		std::lock_guard<std::mutex> guard(m_mutex);

		ZeroMemory(m_SendingBuffer, MAX_SOCKBUF); // 송신완료되었으니 버퍼를 비운다.

		if (!m_SendBuffer.IsEmpty()) // 송신할 데이터가 남아있으니
		{
			SendIO(); // 이어서 송신한다.
		}

		return;
	}

	void Close(bool bIsForce_ = false)
	{
		m_IsConnected = false;

		struct linger stLinger = { 0,0 };

		if (bIsForce_)
		{
			stLinger.l_onoff = 1;
		}

		shutdown(m_ClientSocket, SD_BOTH);
		setsockopt(m_ClientSocket, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
		closesocket(m_ClientSocket);
		m_ClientSocket = INVALID_SOCKET;

		return;
	}

	bool GetIP(uint32_t& out_)
	{
		setsockopt(m_ClientSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&m_ListenSocket, (int)sizeof(SOCKET));

		SOCKADDR* l_addr = nullptr; SOCKADDR* r_addr = nullptr; int l_size = 0, r_size = 0;

		GetAcceptExSockaddrs(mAcceptBuf,
			0,
			sizeof(SOCKADDR_STORAGE) + 16,
			sizeof(SOCKADDR_STORAGE) + 16,
			&l_addr,
			&l_size,
			&r_addr,
			&r_size
		);

		SOCKADDR_IN address = { 0 };
		int addressSize = sizeof(address);

		int nRet = getpeername(m_ClientSocket, (struct sockaddr*)&address, &addressSize);

		if (nRet)
		{
			std::cerr << "Connection::GetIP : getpeername Failed\n";

			return false;
		}

		out_ = address.sin_addr.S_un.S_addr;

		return true;
	}

	char* RecvBuffer() { return mRecvBuf; }
	unsigned short GetIndex() { return m_ClientIndex; }

	bool StorePartialMessage(char* str_, int size_)
	{
		std::lock_guard<std::mutex> guard(m_mutex);

		//std::cout << "StorePartialMessage : size :" << size_ << '\n';

		return m_RecvBuffer.enqueue(str_, size_);
	}

	int GetReqMessage(char* out_)
	{
		std::lock_guard<std::mutex> guard(m_mutex);

		char cstr[MAX_RINGBUFFER_SIZE] = { 0 };

		int len = m_RecvBuffer.dequeue(cstr, MAX_RINGBUFFER_SIZE);

		if (len == 0)
		{
			return 0;
		}

		if (len < 5)
		{
			m_RecvBuffer.enqueue(cstr, len);
			return 0;
		}

		int remainlen = CheckStringSize(cstr, len, out_);

		if (remainlen == len)
		{
			return 0;
		}

		len = len - remainlen - sizeof(unsigned int);

		// 남은 데이터는 다시 저장한다.
		m_RecvBuffer.enqueue(cstr, remainlen);

		//std::cout << "GetReqMsg : size :" << len << '\n';

		return len;
	}

private:
	bool BindAcceptEx()
	{
		ZeroMemory(&m_RecvOverlapped.m_overlapped, sizeof(WSAOVERLAPPED));

		m_ClientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);

		if (m_ClientSocket == INVALID_SOCKET)
		{
			return false;
		}

		DWORD bytes = 0;
		DWORD flags = 0;
		m_RecvOverlapped.m_wsaBuf.len = 0;
		m_RecvOverlapped.m_wsaBuf.buf = nullptr;
		m_RecvOverlapped.m_eOperation = eIOOperation::ACCEPT;

		auto result = AcceptEx(
			m_ListenSocket,
			m_ClientSocket,
			mAcceptBuf,
			0,
			sizeof(SOCKADDR_IN) + 16,
			sizeof(SOCKADDR_IN) + 16,
			&bytes,
			reinterpret_cast<LPOVERLAPPED>(&m_RecvOverlapped)
		);

		if (result == FALSE && WSAGetLastError() != WSA_IO_PENDING)
		{
			return false;
		}

		return true;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="str_"></param>
	/// <param name="size_"></param>
	/// <param name="out_"></param>
	/// <returns>remain string's size</returns>
	unsigned int CheckStringSize(char* str_, unsigned int size_, char* out_)
	{
		unsigned int header;
		CopyMemory(&header, str_, sizeof(unsigned int));

		//std::cout << header << "bytes.\n";
		if (size_ < sizeof(unsigned int) + header)
		{
			//std::cout << size_ << "bytes. Not Enough.\n";
			return size_;
		}
		else
		{
			//std::cout << size_ << "bytes. Enough.\n";
			CopyMemory(out_, str_ + sizeof(unsigned int), header);
			CopyMemory(str_, str_ + header + sizeof(unsigned int), size_ - header - sizeof(unsigned int));
			return size_ - sizeof(unsigned int) - header;
		}
	}

	std::wstring utf8_to_wstring(const std::string& str) {
		std::wstring result;
		int length = str.length();
		int i = 0;

		while (i < length) {
			unsigned char c = str[i];

			// 1바이트 문자 (ASCII 범위)
			if (c <= 0x7F) {
				result.push_back(c);
				i++;
			}
			// 2바이트 문자
			else if ((c & 0xE0) == 0xC0) {
				unsigned char c2 = str[i + 1];
				result.push_back(((c & 0x1F) << 6) | (c2 & 0x3F));
				i += 2;
			}
			// 3바이트 문자
			else if ((c & 0xF0) == 0xE0) {
				unsigned char c2 = str[i + 1];
				unsigned char c3 = str[i + 2];
				result.push_back(((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F));
				i += 3;
			}
			// 4바이트 문자
			else if ((c & 0xF8) == 0xF0) {
				unsigned char c2 = str[i + 1];
				unsigned char c3 = str[i + 2];
				unsigned char c4 = str[i + 3];
				result.push_back(((c & 0x07) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F));
				i += 4;
			}
		}

		return result;
	}

	SOCKET m_ListenSocket;
	SOCKET m_ClientSocket;

	std::atomic<bool> m_IsConnected;
	unsigned short m_ClientIndex;

	std::mutex m_mutex;

	char mAcceptBuf[64];

	stOverlappedEx m_RecvOverlapped;
	char mRecvBuf[MAX_SOCKBUF];

	RingBuffer m_SendBuffer;
	RingBuffer m_RecvBuffer;
	char m_SendingBuffer[MAX_SOCKBUF];
	stOverlappedEx m_SendOverlapped;
};