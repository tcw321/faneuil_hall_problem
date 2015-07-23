#include <mutex>
#include <thread>
#include <chrono>
#include <iostream>

std::mutex noJudge;
std::mutex confirmed;
std::mutex checked;
std::mutex allSignedIn;
std::mutex writeMessage;

int immigrantsEntered=0;
int immigrantsChecked=0;

bool judge = 0;

void enter()
{

}

void checkIn()
{

}

void sitDown()
{

}

void swear()
{

}

void getCertificate()
{}

void leave()
{}

void addImmigrant()
{
    noJudge.lock();
    enter();
    immigrantsEntered++;
    {
        std::lock_guard<std::mutex> lock(writeMessage);
        std::cout << "immigrant entered\n";
        std::cout << "entered " << immigrantsEntered << " checked " << immigrantsChecked << std::endl;
    }
    noJudge.unlock();

    checked.lock();
    checkIn();
    immigrantsChecked++;
    {
        std::lock_guard<std::mutex> lock(writeMessage);
        std::cout << "immigrant checked in\n";
        std::cout << "entered " << immigrantsEntered << " checked " << immigrantsChecked << std::endl;
    }
    if ((judge == 1) && (immigrantsEntered == immigrantsChecked))
        allSignedIn.unlock();
    else
        checked.unlock();

    sitDown();
    confirmed.lock();

    swear();
    getCertificate();

    noJudge.lock();
    leave();
    {
        std::lock_guard<std::mutex> lock(writeMessage);
        std::cout << "immigrant leave\n";
        std::cout << "entered " << immigrantsEntered << " checked " << immigrantsChecked << std::endl;
    }
    noJudge.unlock();
}

void confirm()
{

}

void addJudge()
{
    noJudge.lock();
    checked.lock();

    enter();
    {
        std::lock_guard<std::mutex> lock(writeMessage);
        std::cout << "Judge entered\n";
        std::cout << "entered " << immigrantsEntered << " checked " << immigrantsChecked << std::endl;
    }
    judge = 1;

    if (immigrantsEntered > immigrantsChecked)
    {
        checked.unlock();
        allSignedIn.lock();
    }
    confirm();
    while (immigrantsChecked !=0) {
        confirmed.unlock();
        immigrantsChecked--;
    }
    confirmed.unlock();
    immigrantsEntered = 0;
    immigrantsChecked = 0;

    leave();
    {
        std::lock_guard<std::mutex> lock(writeMessage);
        std::cout << "Judge left\n";
        std::cout << "entered " << immigrantsEntered << "checked " << immigrantsChecked << std::endl;
    }
    judge = 0;

    checked.unlock();
    noJudge.unlock();

}

void spectate()
{}

void addSpectator()
{
    noJudge.lock();
    enter();
    noJudge.unlock();
    spectate();
    leave();
}

int main()
{
    std::thread i1(addImmigrant);
    std::thread i2(addImmigrant);
    std::thread i3(addImmigrant);
    std::thread i4(addImmigrant);
    std::thread i5(addImmigrant);
    std::thread i6(addImmigrant);
    std::thread i7(addImmigrant);
    std::thread j(addJudge);
    //std::thread s1(addSpectator);
    //std::thread s2(addSpectator);
    //std::thread s3(addSpectator);
    //std::thread s4(addSpectator);

    std::this_thread::sleep_for(std::chrono::nanoseconds(3000000));
    std::thread j2(addJudge);
    std::this_thread::sleep_for(std::chrono::nanoseconds(3000000));

    i1.join();
    i2.join();
    i3.join();
    i4.join();
    i5.join();
    i6.join();
    i7.join();
    j.join();
    //s1.join();
    //s2.join();
    //s3.join();
    //s4.join();
    j2.join();
    {
        std::lock_guard<std::mutex> lock(writeMessage);
        std::cout << "Done\n";
    }

}

