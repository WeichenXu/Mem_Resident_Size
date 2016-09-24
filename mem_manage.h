// Author: Weichen Xu
// Date: 09/24/2016
// Transplant memory management from .c to .cpp 

#include <iostream>
#include <std.h>

namespace Memory_System{
    class mem_analysis_sys{
    public:
        // use Singleton design pattern
        static mem_analysis_sys& getMemAnalysisSys()
        {
            if (sys_instance_ == nullptr){
                sys_instance_ = new mem_analysis_sys;
            }
            return *sys_instance_;
        }

        // getter and setter for the parameters
        size_t getPageNum() const;
        size_t getMemAccTimes() const;
        size_t getPageFaultTimes() const;

        //----------------Utility function----------------//
        void shrinkResidentSize();

        //----------------Perform memory resident algorithm----------//
        // PFF algorithm
        // 

    private:
        static mem_analysis_sys* sys_instance_;
        // use pimpl pattern
        paraters *params_(nullptr);
        // nested class, parameters for mem_analysis_sys
        struct parameters{
            size_t page_num(0);
            size_t mem_acc_times(0);
            size_t page_fault_times(0);
        }
    };
}
