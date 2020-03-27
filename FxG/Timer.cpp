#include "Timer.h"

#include "amxxmodule.h"

#include <algorithm>

TimerManager g_TimerManager;

Timer::Timer(float delay, int id, TimerCallback_t callback, const TimerParam_t &params, bool repeated)
{
    m_startTime = gpGlobals->time;
    m_delay = delay;
    m_pFunction = callback;
    m_params = params;
    m_id = id;
    m_repeated = repeated;
    m_deleted = false;
    m_executing = false;
}

Timer::Timer(float delay, int id, TimerCallback_t callback, bool repeated)
{
    m_startTime = gpGlobals->time;
    m_delay = delay;
    m_pFunction = callback;
    m_id = id;
    m_repeated = repeated;
    m_deleted = false;
    m_executing = false;
}

bool Timer::SholudExecuteNow() const
{
    return gpGlobals->time >= m_startTime + m_delay;
}

void Timer::Execute()
{
    m_executing = true;
    m_pFunction(this);
    m_executing = false;
}

Timer* TimerManager::Create(float delay, int id, TimerCallback_t callback, const TimerParam_t &params, bool repeated)
{
    if (delay < 0.1) // performance
        delay = 0.1;

    if (this->FindById(id, m_timers.begin()) != m_timers.end())
    {
        MF_Log("Cannot create timer, timer id(%d) exists.", id);
        return nullptr;
    }

    m_timers.push_back(new Timer(delay, id, callback, params, repeated));
    return m_timers.back();
}

Timer* TimerManager::Create(float delay, int id, TimerCallback_t callback, bool repeated)
{
    if (delay < 0.1) // performance
        delay = 0.1;

    if (this->FindById(id, m_timers.begin()) != m_timers.end())
    {
        MF_Log("Cannot create timer, timer id(%d) exists.", id);
        return nullptr;
    }

    m_timers.push_back(new Timer(delay, id, callback, repeated));
    return m_timers.back();
}

Timer* TimerManager::Create(float delay, int id, TimerCallback_t callback)
{
    return this->Create(delay, id, callback, false);
}

Timer* TimerManager::Create(float delay, int id, TimerCallback_t callback, const TimerParam_t& params)
{
    return this->Create(delay, id, callback, params, false);
}

void TimerManager::Remove(Timer *pTimer)
{
    if (pTimer->IsExecuting())
    {
        pTimer->MarkAsDeleted(true);
        return;
    }

    m_timers.erase(std::remove(m_timers.begin(), m_timers.end(), pTimer), m_timers.end());
    delete pTimer;
}

void TimerManager::Remove(std::vector<Timer*>::const_iterator iter)
{
    if ((*iter)->IsExecuting())
    {
        (*iter)->MarkAsDeleted(true);
        return;
    }

    delete (*iter);
    m_timers.erase(iter);
}

void TimerManager::Remove(int id)
{
    auto iter = this->FindById(id, m_timers.begin());
    if (iter == m_timers.end())
        return;

    this->Remove(iter);
}

void TimerManager::Clean()
{
    while (m_timers.size())
    {
        delete m_timers.back();
        m_timers.pop_back();
    }
}

void TimerManager::Executes()
{
    if (gpGlobals->time >= m_nextThink)
    {
        for (auto it = m_timers.begin(); it != m_timers.end(); )
        {
            if ((*it)->SholudExecuteNow())
            {
                if (!(*it)->IsDeleted())
                {
                    (*it)->Execute();

                    if ((*it)->IsRepeated())
                    {
                        (*it)->SetStartTime(gpGlobals->time);
                        continue;
                    }
                }

                SERVER_PRINT(UTIL_VarArgs("Timer (id:%d) deleted.\n", (*it)->GetId()));
                delete (*it);
                it = m_timers.erase(it);
            }
            else
            {
                it++;
            }
        }

        m_nextThink = gpGlobals->time + 0.1;
    }
}

unsigned int TimerManager::GetIndex(Timer* pTimer) const
{
    auto iter = std::find(m_timers.begin(), m_timers.end(), pTimer);
    if (iter == m_timers.end())
        return -1;

    return std::distance(m_timers.begin(), iter);
}

Timer* TimerManager::At(unsigned int index) const
{
    if (index >= m_timers.size())
        return nullptr;

    return m_timers.at(index);
}

std::vector<Timer*>::const_iterator TimerManager::FindById(int id, std::vector<Timer*>::const_iterator begin) const
{
    auto iter = std::find_if(
        begin, m_timers.end(),
        [id](const Timer* ptr) { return ptr->GetId() == id; });

    return iter;
}
