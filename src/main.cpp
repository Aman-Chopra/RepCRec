/**
    main.cpp
    Purpose: The main function
    @author Aman Chopra (ac8511)
    @version 1.1 20/11/21
*/

#include <iostream>
#include <vector>
#include <string>
#include "TransactionManager/TransactionManager.h"

using namespace std;

int main(int argc, char **argv)
{
    //string fileName = argv[1];
    TransactionManager *manager = new TransactionManager();
    string file_name = "";
    cout << "Please enter the absolute path of the input file" << endl;
    cin >> file_name;
    manager->start(file_name);
    return 0;
}
