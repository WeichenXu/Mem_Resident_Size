#include "mem_manage.h"
using namespace Memory_System;
int main(){
    mem_analysis_sys& mSys = mem_analysis_sys::getMemAnalysisSys();
    
    // generate the test mem acc sequence
    using mem_acc_seq_t = std::vector<size_t>;
    mem_acc_seq_t seq = {0, 0, 1, 1, 2, 2};
//    size_t testSize = 1000;
//    seq.resize(testSize);
//    for (int i=0; i < testSize; ++i){
//        seq[i] = (size_t) i%10;
//        //seq[i] = 1;
//    }

    // use the PFF for resident set size test
    mSys.setPFFParams(3);
    mSys.memAccess(seq, resident_set_manage_algo::PFF);
    std::cout << mSys.getPageFaultTimes() << std::endl;
    std::cout << mSys.getResidentSize() << std::endl;
    // use the VSWS for resident set size test
    mSys.setVSWSParams(50, 1, 2);
    mSys.memAccess(seq, resident_set_manage_algo::VSWS);
    std::cout << mSys.getPageFaultTimes() << std::endl;
    std::cout << mSys.getResidentSize() << std::endl;
    

    return 0;
}
