#pragma once

#include <vector>
#include <memory>
#include <any>

class Timer;

typedef std::vector<std::any> TimerParam_t;
typedef void (*TimerCallback_t)(Timer*);

class Timer
{
public:
    Timer(float delay, int id, TimerCallback_t callback, const TimerParam_t &params, bool repeated);
    Timer(float delay, int id, TimerCallback_t callback, bool repeated);

    bool SholudExecuteNow() const;
    float GetStartedTime() const { return m_startTime; }
    float GetDelay() const { return m_delay; }
    bool IsRepeated() const { return m_repeated; }
    bool IsDeleted() const { return m_deleted; }
    bool IsExecuting() const { return m_executing; }
    void MarkAsDeleted(bool value) { m_deleted = value; }
    TimerCallback_t GetFunction() const { return m_pFunction; }
    TimerParam_t GetParams() const { return m_params; }
    TimerParam_t& GetParams() { return m_params; }
    void Execute();
    int GetId() const { return m_id; }

    void SetStartTime(float startTime) { m_startTime = startTime; }

private:
    int m_id;
    bool m_repeated;
    bool m_deleted;
    bool m_executing;
    float m_startTime;
    float m_delay;
    TimerCallback_t m_pFunction;
    TimerParam_t m_params;
};

class TimerManager
{
public:
    TimerManager() : m_nextThink(0) {}
    ~TimerManager() { this->Clean(); }

    Timer *Create(float delay, int id, TimerCallback_t callback, const TimerParam_t &params, bool repeated=false);
    Timer* Create(float delay, int id, TimerCallback_t callback, const TimerParam_t& params);
    Timer* Create(float delay, int id, TimerCallback_t callback, bool repeated=false);
    Timer* Create(float delay, int id, TimerCallback_t callback);
    void Remove(Timer* pTimer);
    void Remove(std::vector<Timer*>::const_iterator iter);
    void Remove(int id);
    void Clean();
    void Executes();

    unsigned int GetIndex(Timer* pTimer) const;
    Timer* At(unsigned int index) const;

    std::vector<Timer*>::const_iterator FindById(int id, std::vector<Timer*>::const_iterator begin) const;

private:
    std::vector<Timer*> m_timers;
    float m_nextThink;
};

extern TimerManager g_TimerManager;