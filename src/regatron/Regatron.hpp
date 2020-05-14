#pragma once
#include "utils/Instrumentator.hpp"
#include <stdexcept>
#include <string>

namespace Regatron {
enum class CommStatus { Ok, CommFail, DLLFail, DLLCommFail, Disconncted };

class CommException : public std::runtime_error {
  protected:
    std::string      m_Message;
    const CommStatus m_CommStatus;

  public:
    CommException(std::string message, CommStatus commstatus)
        : std::runtime_error(""), m_Message(std::move(message)),
          m_CommStatus(commstatus) {}

    CommException(std::string message)
        : std::runtime_error(""), m_Message(std::move(message)),
          m_CommStatus(CommStatus::CommFail) {}

    CommException(CommStatus commstatus)
        : std::runtime_error(""), m_CommStatus(commstatus) {
        if (commstatus == CommStatus::Disconncted) {
            m_Message = "comm is disconnected";
        } else if (commstatus == CommStatus::CommFail) {
            m_Message = "comm operation failed";
        } else if (commstatus == CommStatus::DLLFail) {
            m_Message = "dll status report: communication error";
        } else if (commstatus == CommStatus::DLLCommFail) {
            m_Message =
                "dll status report: device reported command execution error";
        } else {
            m_Message = "comm ok?";
        }
    }
    CommStatus getCommStatus() const noexcept { return this->m_CommStatus; }
    virtual const char *what() const noexcept {
        return this->m_Message.c_str();
    }
};
} // namespace Regatron
