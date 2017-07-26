#include <iostream>
#include <ESBManager.h>

int main()
{
    _ESBManager::ESBManager esbManager = _ESBManager::ESBManager();
    esbManager.InitializeAndStart();
    return (0);
}
