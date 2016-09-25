#ifndef _mem_analysis_sys_
#define _mem_analysis_sys_
// Author: Weichen Xu
// Date: 09/24/2016
// Transplant memory management from .c to .cpp 

#include <iostream>
#include <std.h>
#include <vector>

namespace Memory_System{
    // enum class for 

    class mem_analysis_sys{
    public:
        // use Singleton design pattern
        static mem_analysis_sys& getMemAnalysisSys();

        // getter and setter for the parameters
        size_t getPageNum() const { returnn this->params_->page_num; }
        size_t getMemAccTimes() const { return this->params_->mem_acc_times; }
        size_t getPageFaultTimes() const { return this->params_->page_fault_times; }
        size_t getResidentSize() const { return this->res_mem_size_; }

        //----------------Utility function----------------//
        void shrinkResidentSize();
        void memAccess();    

        //----------------Perform memory resident algorithm----------//
        // PFF algorithm
        void PFF();
        // VSWS algorithm
        void VSWS();

    private:
        // constructor and destructor
        mem_analysis_sys(int mem_access_times = 0);
        ~mem_analysis_sys();

        static mem_analysis_sys* sys_instance_;
        // use pimpl pattern

        parameters* params_(nullptr);
        // nested class, parameters for mem_analysis_sys
        struct parameters{
            size_t page_num(0);
            size_t mem_acc_times(0);
            size_t page_fault_times(0);
        }
        
        // nested class, pages in memory
        struct page{
            size_t address(0);
            bool in_memory(false);
            int use_bit(0);
        }

        // resident memory
        std::vector<page> pages_;
        size_t res_mem_size_(0);
    };
}
#endif
