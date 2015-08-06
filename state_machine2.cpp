
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

#include <iostream>

class Log
{
private:
    static std::mutex m_mutex;

public:
    static void write(const std::string &name, int id = 0)
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        std::cout << name;
        if (id != 0)
            std::cout << " " << id;
        std::cout << std::endl;
    }
};

std::mutex Log::m_mutex;

class ThreadFlag
{
private:
    std::mutex m_mutex;
    std::condition_variable m_setCondition;
    std::condition_variable m_unsetCondition;
    bool m_flag;

public:
    void set()
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_flag = true;
        m_setCondition.notify_all();
    }

    void waitForSet()
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        m_setCondition.wait(lk, [this]{return m_flag;});
    }

    void clear()
    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_flag = false;
        m_unsetCondition.notify_all();
    }

    void waitForUnset()
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        m_unsetCondition.wait(lk, [this]{return !m_flag;});
    }

};


class Immigrant
{
private:
    typedef enum {ENTER,CHECKIN, SITDOWN, CONFIRM, GETCERT, LEAVE } State;
    int m_identifier;
    State m_state;
    bool m_running;
    ThreadFlag &m_judgeStatus;
    ThreadFlag &m_allConfirmStatus;
    ThreadFlag &m_allIssuedCertStatus;
    static std::atomic<int> m_entered;
    static std::atomic<int> m_confirmed;

public:

   Immigrant(int id, ThreadFlag &judgeStatus, ThreadFlag &allConfirmStatus, ThreadFlag &allIssuedCertStatus)
           : m_identifier(id)
           , m_judgeStatus(judgeStatus)
           , m_allConfirmStatus(allConfirmStatus)
           , m_allIssuedCertStatus(allIssuedCertStatus)
           , m_running(true)
   {}

   void run()
   {
        while (m_running)
        {
          switch(m_state)
          {
              case ENTER:
                  m_judgeStatus.waitForUnset();
                  m_entered++;
                  Log::write("Immigrant Entered", m_identifier);
                  m_state = CHECKIN;
                  break;
              case CHECKIN:
                  Log::write("Immigrant Checkin", m_identifier);
                  m_state = SITDOWN;
                  std::this_thread::sleep_for(std::chrono::milliseconds(10));
                  break;
              case SITDOWN:
                  Log::write("Immigrant Sitdown", m_identifier);
                  m_state = CONFIRM;
                  std::this_thread::sleep_for(std::chrono::milliseconds(10));
                  break;
              case CONFIRM:
                  Log::write("Immigrant Confirm", m_identifier);
                  m_confirmed++;
                  m_judgeStatus.waitForSet();
                  if (m_entered == m_confirmed) {
                      m_allConfirmStatus.set();
                  }
                  m_state = GETCERT;
                  std::this_thread::sleep_for(std::chrono::milliseconds(10));
                  break;
              case GETCERT:
                  Log::write("Immigrant Get Certification", m_identifier);
                  m_allIssuedCertStatus.waitForSet();
                  m_allConfirmStatus.clear();
                  m_state = LEAVE;
                  std::this_thread::sleep_for(std::chrono::milliseconds(10));
                  break;
              case LEAVE:
                  m_judgeStatus.waitForUnset();
                  m_entered--;
                  m_confirmed--;
                  if (m_entered == 0)
                      m_allIssuedCertStatus.clear();
                  Log::write("Immigrant Leave", m_identifier);
                  m_running = false;
                  break;

          }

        };
   }
};

std::atomic<int> Immigrant::m_entered(0);
std::atomic<int> Immigrant::m_confirmed(0);

class Judge
{
private:
    typedef enum {ENTER, CONFIRM, ISSUE, LEAVE } State;
    State m_state;
    ThreadFlag &m_judgeStatus;
    ThreadFlag &m_allConfirmStatus;
    ThreadFlag &m_allIssuedCertStatus;
    bool m_running;

public:

    Judge(ThreadFlag &judgeStatus, ThreadFlag &allConfirmStatus, ThreadFlag &allIssuedCertStatus)
    : m_judgeStatus(judgeStatus)
    , m_allConfirmStatus(allConfirmStatus)
    , m_allIssuedCertStatus(allIssuedCertStatus)
    , m_running(true)
    {}

    void run()
    {
        while (m_running) {
            switch(m_state) {
                case ENTER:
                    Log::write("Judge Entered");
                    m_judgeStatus.set();
                    m_state = CONFIRM;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    break;
                case CONFIRM:
                    Log::write("Judge Confirm");
                    m_allConfirmStatus.waitForSet();
                    m_state = ISSUE;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    break;
                case ISSUE:
                    Log::write("Judge Issue Certificates");
                    m_allIssuedCertStatus.set();
                    m_state = LEAVE;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    break;
                case LEAVE:
                    Log::write("Judge Leave");
                    m_running = false;
                    break;
            }
        }
    }
};

class Building
{
private:
    ThreadFlag m_judgeStatus;
    ThreadFlag m_allConfirmStatus;
    ThreadFlag m_allIssuedCertStatus;

public:
    Building()
    {}

    void run()
    {
        Immigrant immigrant1(1, m_judgeStatus, m_allConfirmStatus, m_allIssuedCertStatus);
        std::thread t1(&Immigrant::run, immigrant1 );

        Immigrant immigrant2(2, m_judgeStatus, m_allConfirmStatus, m_allIssuedCertStatus);
        std::thread t2(&Immigrant::run, immigrant2 );

        Immigrant immigrant3(3, m_judgeStatus, m_allConfirmStatus, m_allIssuedCertStatus);
        std::thread t3(&Immigrant::run, immigrant3 );

        Immigrant immigrant4(4, m_judgeStatus, m_allConfirmStatus, m_allIssuedCertStatus);
        std::thread t4(&Immigrant::run, immigrant4 );

        Immigrant immigrant5(5, m_judgeStatus, m_allConfirmStatus, m_allIssuedCertStatus);
        std::thread t5(&Immigrant::run, immigrant5 );

        Immigrant immigrant6(6, m_judgeStatus, m_allConfirmStatus, m_allIssuedCertStatus);
        std::thread t6(&Immigrant::run, immigrant6 );

        Immigrant immigrant7(7, m_judgeStatus, m_allConfirmStatus, m_allIssuedCertStatus);
        std::thread t7(&Immigrant::run, immigrant7 );

        Judge judge(m_judgeStatus, m_allIssuedCertStatus, m_allIssuedCertStatus);
        std::thread j1(&Judge::run, judge);

        std::this_thread::sleep_for(std::chrono::seconds(1));
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();
        t6.join();
        t7.join();
        j1.join();

    }
};


int main()
{
   Building building;
   building.run();
   return 0;
}