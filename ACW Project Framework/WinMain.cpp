#include <Windows.h>
#include "GraphicsEngine.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,     
    _In_opt_ HINSTANCE hPrevInstance, 
    _In_ LPSTR lpCmdLine,        
    _In_ int nCmdShow              
) {
    GraphicsEngine* graphicsEngine = new GraphicsEngine();
    if (!graphicsEngine->Initialize()) {
        delete graphicsEngine;  
        return 0;
    }

    graphicsEngine->Execute();

    delete graphicsEngine;
    graphicsEngine = nullptr;

    return 0;
}
