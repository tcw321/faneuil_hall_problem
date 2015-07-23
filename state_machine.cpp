#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>

std::atomic<bool> judgeIn;
std::atomic<bool> allCheckedIn;

std::atomic<int> immigrantsEntered;
std::atomic<int> immigrantsChecked;

std::mutex writeMessage;

class Immigrant
{
public:
    typedef enum {ENTER, CHECKIN, CONFIRMED, LEAVE} State;

    Immigrant() : m_state(ENTER), m_immigrantDone(false)
    {}

    void run()
    {
        while (m_immigrantDone == false) {
            switch (m_state) {
                case ENTER:
                    if (judgeIn == false)
                    {
                        immigrantsEntered++;
                        m_state = CHECKIN;
                        {
                            std::lock_guard<std::mutex> lock(writeMessage);
                            std::cout << "IEnter\n";
                        }
                    }
                    break;
                case CHECKIN:
                    immigrantsChecked++;
                    if ((judgeIn == true) && (immigrantsChecked == immigrantsEntered))
                    {
                        allCheckedIn = true;
                    }
                    {
                        std::lock_guard<std::mutex> lock(writeMessage);
                        std::cout << "ICheckedin\n";
                    }
                    m_state = CONFIRMED;
                    break;
                case CONFIRMED:
                    if (judgeIn == false)
                        m_state = LEAVE;
                    {
                        std::lock_guard<std::mutex> lock(writeMessage);
                        std::cout << "IConfirmed\n";
                    }

                    break;
                case LEAVE:
                    m_immigrantDone = true;
                    immigrantsChecked--;
                    immigrantsEntered--;
                    {
                        std::lock_guard<std::mutex> lock(writeMessage);
                        std::cout << "ILeave\n";
                    }

                    break;
            }
        }
    }

private:
    State m_state;
    bool m_immigrantDone;
};

class Judge
{
public:
    typedef enum { ENTER, CONFIRM, LEAVE} State;

    Judge() : m_state(ENTER), m_judgeIn(true)
    {}

    void run()
    {
        while (m_judgeIn)
        {
            switch(m_state)
            {
                case ENTER:
                    judgeIn = true;
                    m_state = CONFIRM;
                    {
                        std::lock_guard<std::mutex> lock(writeMessage);
                        std::cout << "JEnter\n";
                    }
                    break;
                case CONFIRM:
                    if (allCheckedIn == true) {
                        m_state = LEAVE;
                        {
                            std::lock_guard<std::mutex> lock(writeMessage);
                            std::cout << "JConfirm\n";
                        }
                    }
                    break;
                case LEAVE:
                    {
                        std::lock_guard<std::mutex> lock(writeMessage);
                        std::cout << "JLeave\n";
                    }
                    m_judgeIn = false;
                    judgeIn = false;
                    break;

            }
        }
    }
private:
    State m_state;
    bool m_judgeIn;
};

int main()
{
    immigrantsEntered = 0;
    immigrantsChecked = 0;
    Immigrant immigrant1;
    std::thread t1(&Immigrant::run, immigrant1 );

    Immigrant immigrant2;
    std::thread t2(&Immigrant::run, immigrant2 );

    Immigrant immigrant3;
    std::thread t3(&Immigrant::run, immigrant3 );

    Immigrant immigrant4;
    std::thread t4(&Immigrant::run, immigrant4 );

    Immigrant immigrant5;
    std::thread t5(&Immigrant::run, immigrant5 );

    Immigrant immigrant6;
    std::thread t6(&Immigrant::run, immigrant6 );

    Immigrant immigrant7;
    std::thread t7(&Immigrant::run, immigrant7 );

    Judge judge;
    std::thread j1(&Judge::run, judge);

    std::this_thread::sleep_for(std::chrono::nanoseconds(3000000));

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    j1.join();

}