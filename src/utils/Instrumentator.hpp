//
// Basic instrumentation profiler by Cherno
// https://gist.github.com/TheCherno/31f135eea6ee729ab5f26a6908eb3a5e

// Usage: include this header file somewhere in your code (eg. precompiled
// header), and then use like:
//
// Instrumentor::Get().BeginSession("Session Name");        // Begin session
// {
//     InstrumentationTimer timer("Profiled Scope Name");   // Place code like
//     this in scopes you'd like to include in profiling
//     // Code
// }
// Instrumentor::Get().EndSession();                        // End Session

#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <memory>
#include <string>
#include <thread>

struct ProfileResult {
    std::string Name;
    long long   Start, End;
    uint32_t    ThreadID;
};

struct InstrumentationSession {
    std::string Name;
};

class Instrumentor {
  private:
    std::unique_ptr<InstrumentationSession> m_CurrentSession;
    std::ofstream           m_OutputStream;
    int                     m_ProfileCount;

    Instrumentor() : m_CurrentSession(), m_ProfileCount(0) {}

  public:
    void BeginSession(const std::string &name,
                      const std::string &filepath = "profile.json") {
        m_OutputStream.open(filepath);
        WriteHeader();
        m_CurrentSession = std::make_unique<InstrumentationSession>(
            InstrumentationSession{name});
    }

    void EndSession() {
        WriteFooter();
        m_OutputStream.close();
        m_CurrentSession.reset();
        m_ProfileCount   = 0;
    }

    void WriteProfile(const ProfileResult &result) {
        if (m_ProfileCount++ > 0)
            m_OutputStream << ",";

        std::string name = result.Name;
        std::replace(name.begin(), name.end(), '"', '\'');

        m_OutputStream << "{";
        m_OutputStream << "\"cat\":\"function\",";
        m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
        m_OutputStream << "\"name\":\"" << name << "\",";
        m_OutputStream << "\"ph\":\"X\",";
        m_OutputStream << "\"pid\":0,";
        m_OutputStream << "\"tid\":" << result.ThreadID << ",";
        m_OutputStream << "\"ts\":" << result.Start;
        m_OutputStream << "}";

        m_OutputStream.flush();
    }

    void WriteHeader() {
        m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
        m_OutputStream.flush();
    }

    void WriteFooter() {
        m_OutputStream << "]}";
        m_OutputStream.flush();
    }

    static Instrumentor &Get() {
        static Instrumentor instance;
        return instance;
    }
};

class InstrumentationTimer {
  public:
    InstrumentationTimer(const char *name) : m_Name(name), m_Stopped(false) {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~InstrumentationTimer() {
        if (!m_Stopped)
            Stop();
    }

    void Stop() {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        long long start =
            std::chrono::time_point_cast<std::chrono::microseconds>(
                m_StartTimepoint)
                .time_since_epoch()
                .count();
        long long end = std::chrono::time_point_cast<std::chrono::microseconds>(
                            endTimepoint)
                            .time_since_epoch()
                            .count();

        uint32_t threadID =
            std::hash<std::thread::id>{}(std::this_thread::get_id());
        Instrumentor::Get().WriteProfile({m_Name, start, end, threadID});

        m_Stopped = true;
    }

  private:
    const char *m_Name;
    std::chrono::time_point<std::chrono::high_resolution_clock>
         m_StartTimepoint;
    bool m_Stopped;
};
