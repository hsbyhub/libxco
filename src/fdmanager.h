/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	�ļ����ƣ�fdmanager.h
      	�� �� �ߣ�hsby
      	�������ڣ�2022/2/10
 *================================================================*/
#pragma once
#include <memory>
#include "iomanager.h"

XCO_NAMESPAVE_START
/**
 * @brief �׽���������
 */
class FdCtx : public std::enable_shared_from_this<FdCtx>{
public:
    typedef std::shared_ptr<FdCtx> Ptr;

public:
    FdCtx(int fd);
    ~FdCtx();

    /**
     * @brief ��ʼ���׽���������
     */
    bool Init();

    bool IsInit() const;

    void SetIsInit(bool mIsInit);

    bool IsSocket() const;

    void SetIsSocket(bool mIsSocket);

    bool IsSysNonblock() const;

    void SetIsSysNonblock(bool mIsSysNonblock);

    bool IsUserNonblock() const;

    void SetIsUserNonblock(bool mIsUserNonblock);

    bool IsClosed() const;

    void SetIsClosed(bool mIsClosed);

    int SetFs() const;

    void SetFs(int mFs);

    void SetTimeout(int type, uint64_t val);

    uint64_t GetTimeout(int type);

    IoManager *GetIomanager() const;

    void SetIomanager(IoManager *mIomanager);

private:
    bool m_isInit : 1;
    bool m_isSocket : 1;
    bool m_isSysNonblock : 1;
    bool m_isUserNonblock : 1;
    bool m_isClosed : 1;
    int m_fd;

    uint64_t m_recvTimeout;
    uint64_t m_sendTimeout;

    xco::IoManager* m_iomanager;
};

/**
 * @brief �׽��������Ĺ�����
 */
class FdManager{
public:
    FdManager();

    FdCtx::Ptr Get(int fd, bool auto_create = true);
    void Del(int fd);

private:
    std::vector<FdCtx::Ptr> m_fdctxs;
};
using FdManagerSgt = Singleton<FdManager>;

XCO_NAMESPAVE_END