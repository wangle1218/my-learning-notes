/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file str_util.cpp
 * @author lihaibo(com@baidu.com)
 * @date 2013/12/12 11:57:35
 * @brief 
 *  
 **/

#include "str_util.h"

	void split_string(char *str, char *dem, char *arr[], int *len)/*{{{*/
	{
		char *p = NULL;
		char *pd = NULL;
		char *pend = str + strlen(str);
		int count = 0;
		p = str;
		while (*p) {

			pd = dem;
			while (*pd) {
				if (*p == *pd) {
					*p = 0;
					break;
				}
				pd++;
			}
			p++;
		}

		memset(arr, 0, sizeof(char *) * (*len));
		p = str;
		while (p < pend && count < *len) {
			if (*p) {
				//not null
				arr[count] = p;
				count++;
				p += strlen(p);
			}
			p++;
		}
		*len = count;
		return;
	}	/*}}}*/

    int load_item_line(const char * rule_path/*{{{*/
		, const int32_t ITEM_NUM
		, void * param
		, parse_line_func func
		, const char * dem)
    {
        FILE* fp;
        fp = fopen(rule_path, "r");
        if (!fp)
        {
            printf("failed to open file. [fn='%s']", rule_path);
            return 1;
        }

        int LINE_LEN = 2048;
        int line_count = 0;
        char line[LINE_LEN+1],line_bk[LINE_LEN+1];
        char * item_ptrs[ITEM_NUM];

        while (fgets(line, LINE_LEN, fp))
        {
			if (line[strlen(line)-1] == '\n')
			{
				line[strlen(line)-1] = '\0';
			}
			if(line[strlen(line)-1] == '\r')
			{
				line[strlen(line)-1] = '\0';
			}
            memcpy(line_bk, line, sizeof(line));
           	int32_t item_num = ITEM_NUM;
            // 每行不是两个字段,或是转换失败,则跳过
            split_string(line, (char*)dem, item_ptrs, &item_num);
            if (item_num != ITEM_NUM) 
                {
                    printf("illegal buser rule.item_num:[%u],line:[%s]", item_num,line_bk);
                    continue;
                }
                func(param, item_num,item_ptrs,line_bk);

                ++line_count;
            }
            fclose(fp);
            return 0;
    }	/*}}}*/



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
