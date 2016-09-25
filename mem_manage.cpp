#include <mem_manage.h>

// Implementation for the mem_manage
// Author: Weichen
// Date:09/24/2016

namespace Memory_System{
    // initialize static member for mem_analysis_sys
    mem_analysis_sys::sys_instance_ = nullptr;
    
    // constructors and destructor sfor mem_analysis_sys
    static mem_analysis_sys& mem_analysis_sys::getMemAnalysisSys()
    {
        if (sys_instance_ == nullptr){
            sys_instance_ = new mem_analysis_sys();
        }
        return *sys_instance_;
    }
    mem_analysis_sys::mem_analysis_sys(size_t mem_access_times):
        mem_access_times_(mem_access_times)
    {
        params_ = new parameters;
        params_->mem_access_times_ = mem_access_times;
        pages_.resize(mem_access_times);
    }
    ~mem_analysis_sys::mem_analysis_sys(){
        pages_.clear();
        if (sys_instance_ != nullptr){
            delete sys_instance_˜;
        }
        if (params_ != nullptr){
            delete params_;
        }
    }
    
    // shrink resident set
    // 1. discard the pages whose use-bit == 0
    // 2. set remaining pages use-bit == 0
    void shrinkResidentSet(){
        for (auto &page in pages_){
            if (page.use_bit){
                page.use_bit = 0;
            }
            else{
                if (page.in_memory){
                    --res_mem_size_;
                    page.in_memory = false;
                }
            }
        }
    }


}
