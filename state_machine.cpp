#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>

std::atomic<bool> judgeIn;
std::atomic<bool> allCheckedIn;

std::atomic<int> immigrantsEntered;
std::atomic<int> immigrantsChecked;

std::mutex writeMessage;

class Immigrant
{
public:
    typedef enum {ENTER, CHECKIN, CONFIRMED, LEAVE} State;

    Immigrant(int id) : m_state(ENTER), m_immigrantDone(false), m_id(id)
    {}

    void run()
    {
        using namespace std::chrono;
        while (m_immigrantDone == false) {
            switch (m_state) {
                case ENTER:
                    if (judgeIn == false)
                    {
                        if (immigrantsEntered == 0)
                            allCheckedIn = false;
                        immigrantsEntered++;
                        m_state = CHECKIN;
                        {
                            std::lock_guard<std::mutex> lock(writeMessage);
                            std::cout << "IEnter " << m_id << "\n";
                            std::cout << "entered " << immigrantsEntered << " checked " << immigrantsChecked << std::endl;
                            std::cout << "judge in " << judgeIn << std::endl;
                            std::cout << "all checked in " << allCheckedIn << std::endl;
                        }
                    }
                    std::this_thread::sleep_for(milliseconds(20));
                    break;
                case CHECKIN:
                    immigrantsChecked++;
                    if ((judgeIn == true) && (immigrantsChecked == immigrantsEntered)) {
                        allCheckedIn = true;
                    }
                    m_state = CONFIRMED;
                    {
                        std::lock_guard<std::mutex> lock(writeMessage);
                        std::cout << "ICheckedin " << m_id << "\n";
                        std::cout << "entered " << immigrantsEntered << " checked " << immigrantsChecked << std::endl;
                        std::cout << "judge in " << judgeIn << std::endl;
                        std::cout << "all checked in " << allCheckedIn << std::endl;
                    }
                    std::this_thread::sleep_for(milliseconds(3));
                    break;
                case CONFIRMED:
                    if (allCheckedIn == true) {
                        m_state = LEAVE;
                        {
                            std::lock_guard<std::mutex> lock(writeMessage);
                            std::cout << "IConfirmed " << m_id << "\n";
                            std::cout << "judge in " << judgeIn << std::endl;
                            std::cout << "all checked in " << allCheckedIn << std::endl;
                        }
                    }
                    std::this_thread::sleep_for(milliseconds(3));
                    break;
                case LEAVE:
                    m_immigrantDone = true;
                    {
                        std::lock_guard<std::mutex> lock(writeMessage);
                        std::cout << "ILeave " << m_id << "\n";
                        std::cout << "judge in " << judgeIn << std::endl;
                        std::cout << "all checked in " << allCheckedIn << std::endl;
                    }
                    std::this_thread::sleep_for(milliseconds(3));
                    break;
            }
        }
    }

private:
    State m_state;
    bool m_immigrantDone;
    int m_id;
};

class Judge
{
public:
    typedef enum { ENTER, CONFIRM, LEAVE} State;

    Judge() : m_state(ENTER), m_judgeIn(true)
    {}

    void run()
    {
        using namespace std::chrono;
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
                        std::cout << "judge in " << judgeIn << std::endl;
                        std::cout << "all checked in " << allCheckedIn << std::endl;

                    }
                    std::this_thread::sleep_for(milliseconds(3));
                    break;
                case CONFIRM:
                    if (allCheckedIn == true) {
                        allCheckedIn = false;
                        immigrantsChecked = 0;
                        immigrantsEntered = 0;
                        judgeIn = false;
                        m_state = LEAVE;
                        {
                            std::lock_guard<std::mutex> lock(writeMessage);
                            std::cout << "JConfirm\n";
                            std::cout << "judge in " << judgeIn << std::endl;
                            std::cout << "all checked in " << allCheckedIn << std::endl;
                        }
                    }
                    std::this_thread::sleep_for(milliseconds(3));
                    break;
                case LEAVE:
                    m_judgeIn = false;
                    {
                        std::lock_guard<std::mutex> lock(writeMessage);
                        std::cout << "JLeave\n";
                        std::cout << "judge in " << judgeIn << std::endl;
                        std::cout << "all checked in " << allCheckedIn << std::endl;
                    }
                    std::this_thread::sleep_for(milliseconds(3));
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
    using namespace std::chrono;

    immigrantsEntered = 0;
    immigrantsChecked = 0;
    allCheckedIn = true;  // in case judge enters before any immigrants
    Immigrant immigrant1(1);
    std::thread t1(&Immigrant::run, immigrant1 );

    Immigrant immigrant2(2);
    std::thread t2(&Immigrant::run, immigrant2 );

    Immigrant immigrant3(3);
    std::thread t3(&Immigrant::run, immigrant3 );

    Immigrant immigrant4(4);
    std::thread t4(&Immigrant::run, immigrant4 );

    Immigrant immigrant5(5);
    std::thread t5(&Immigrant::run, immigrant5 );

    Immigrant immigrant6(6);
    std::thread t6(&Immigrant::run, immigrant6 );

    Immigrant immigrant7(7);
    std::thread t7(&Immigrant::run, immigrant7 );

    Judge judge;
    std::thread j1(&Judge::run, judge);

    std::this_thread::sleep_for(seconds(1));

    //std::thread j2(&Judge::run, judge);
    //std::this_thread::sleep_for(seconds(1));

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    j1.join();
    //j2.join();

}