#include "mem_manage.h"
using namespace Memory_System;
int main(){
    mem_analysis_sys& mSys = mem_analysis_sys::getMemAnalysisSys();
    std::cout << mSys.getResidentSize() << std::endl;
    return 0;
}
