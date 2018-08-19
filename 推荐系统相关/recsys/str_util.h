#ifndef __STR_UTIL_H__
#define __STR_UTIL_H__

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef unsigned int uint32_t;
typedef int int32_t;

	/**
	 * @brief 字符串拆分，会修改str的内容，将原串分割为各个字串arr中记录各个字串首地址
	 * @copy from apache src by linxiaobin
	 * @param [in/out] str	 : char* 原串
	 * @param [in] dem	 : char* 分隔符
	 * @param [out] arr   : char*[] 生成的数组，注意只有指针，指向的是str中的空间
	 * @param [in/out] len	 : int* 限制生成的arr的元素上限，函数完成时，保存实际的arr元素个数
	**/
	void split_string(char *str, char *dem, char *arr[], int *len);
	
	typedef void (*parse_line_func)(void * param
		, uint32_t item_num,char *item_ptrs[],const char * cont);
	
    int load_item_line(const char * rule_path
		, const int32_t ITEM_NUM
		, void * param
		, parse_line_func func
		, const char * dem)	;
		
    /**
     * @brief 
     *
     * @param [in/out] rule_path   : const char* 载入文件路径
     * @param [in/out] ITEM_NUM   : const int32_t item总数
     * @param [in/out] param   : void* 传入参数
     * @param [in/out] func   : parse_line_func 回调函数
     * @param [in/out] dem   : const char* 分隔符，可以指定多个
     * @return  int 
     * @retval   
     * @see 
     * @note 
     * @author lihaibo
     * @date 2013/12/12 11:58:58
    **/
    int load_item_line(const char * rule_path
		, const int32_t ITEM_NUM
		, void * param
		, parse_line_func func
		, const char * dem);
        		
/*		
void QRuleFilter::parse_no_eff(void* param, uint32_t item_num, char* item_ptrs[], const char* cont)
{
    QRuleFilter* filter = (QRuleFilter*)param;
    uint32_t userid = 0;
    if (kr::get_item_uint(item_ptrs[0], userid) != kr::SUCC) {
        UB_LOG_WARNING("illegal no eff rule.item_num:[%u],line:[%s]", item_num, cont);
        return;
    }
    filter->set_no_eff_userid->set(userid);
}	
*/	

#endif    
