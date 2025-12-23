#include <iostream>
#include <cassert>
#include <stdio.h>
#include <dlfcn.h>
#include "singleton.hpp"
#include "shared_test.hpp"

int *g_i;

using namespace abc;

int main()
{
    g_i = Singleton<int>::GetInstance();

    // assert(FromSharedGetInstance() == g_i);

    void *hdl;
    void *sym;
    char *err;
    int *(*call)(void);

    printf("Main entry point\n");

    // Check parameters
    printf("Loading shared lib...\n");
    hdl = dlopen("./libtest.so", RTLD_LAZY | RTLD_GLOBAL);
    if (NULL == hdl)
    {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    // Clear any pending error message
    (void)dlerror();

    // Look for symbol in the shared lib
    sym = dlsym(hdl, "_Z21FromSharedGetInstancev");
    if (NULL == sym)
    {
        err = dlerror();
        if (err)
        {
            fprintf(stderr, "%s\n", err);
            return 1;
        }
        else
        {
            // The symbol has been found but it is NULL
            fprintf(stderr, "The symbol is NULL\n");
            return 1;
        }
    }

    // Call a function in shared lib
    call = (int *(*)(void))sym;
    std::cout << g_i << " " << *call << " " << (*call)() << " " << std::endl;
    assert((call)() == g_i);

    return 0;
}
