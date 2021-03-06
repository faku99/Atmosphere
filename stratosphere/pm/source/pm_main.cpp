#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <malloc.h>

#include <switch.h>
#include <stratosphere.hpp>

extern "C" {
    extern u32 __start__;

    u32 __nx_applet_type = AppletType_None;

    #define INNER_HEAP_SIZE 0x200000
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];
    
    void __libnx_initheap(void);
    void __appInit(void);
    void __appExit(void);
}


void __libnx_initheap(void) {
	void*  addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	/* Newlib */
	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char*)addr;
	fake_heap_end   = (char*)addr + size;
}

void __appInit(void) {
    Result rc;

    rc = smInitialize();
    if (R_FAILED(rc)) {
        fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));
    }
    
    rc = fsInitialize();
    if (R_FAILED(rc)) {
        fatalSimple(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));
    }
        
    rc = lrInitialize();
    if (R_FAILED(rc))  {
        fatalSimple(0xCAFE << 4 | 1);
    }
    
    rc = fsprInitialize();
    if (R_FAILED(rc))  {
        fatalSimple(0xCAFE << 4 | 2);
    }
    
    rc = splInitialize();
    if (R_FAILED(rc))  {
        fatalSimple(0xCAFE << 4 | 3);
    }
}

void __appExit(void) {
    /* Cleanup services. */
    fsdevUnmountAll();
    splExit();
    fsprExit();
    lrExit();
    fsExit();
    smExit();
}

int main(int argc, char **argv)
{
    consoleDebugInit(debugDevice_SVC);
    
    /* TODO: What's a good timeout value to use here? */
    WaitableManager *server_manager = new WaitableManager(U64_MAX);
        
    /* TODO: Create services. */
    
    /* Loop forever, servicing our services. */
    server_manager->process();
    
    /* Cleanup. */
    delete server_manager;
	return 0;
}

