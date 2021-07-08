#include "Debug.h"

// WIKI: namespace is for 
namespace Monte {

void Debug::SendDebug(){
    #ifdef BUILD_FOR_LADDER
    // don't send debug
    #else
    debug->SendDebug();

    #endif
}

} // end namespace Monte