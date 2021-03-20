// ZThreadPool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "ThreadPool.h"
#include <iostream>

#define NUMTHREADS 8
#define NUMTASKS 10000000

void PrintTestResults(TestTask* tests)
{
    for (unsigned int i = 0; i < NUMTASKS; i++)
    {
        std::cout << tests[i]._result << std::endl;
    }
}

int main()
{
    ThreadPool* pool = new ThreadPool(NUMTHREADS);

    TestTask* tests = new TestTask[NUMTASKS];

    // init tests
    for (unsigned int i = 0; i < NUMTASKS; i++)
    {
        tests[i]._a = i;
        tests[i]._b = i;
    }

    std::cout << "Starting test with: " << NUMTASKS << " Tasks" << std::endl;
    for (int i = 0 ; i < NUMTASKS; i++)
    {
        pool->AddTask(&tests[i]);
    }
    
    pool->WaitForAllThreads();
    std::cout << "ALL THREADS FINISHED!!!!" << std::endl;

    // print values
    PrintTestResults(tests);

    delete pool;
    
    std::cin.get();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
