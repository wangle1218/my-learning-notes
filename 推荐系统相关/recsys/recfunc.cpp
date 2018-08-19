/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file recfunc.cpp
 * @author lihaibo(com@baidu.com)
 * @date 2013/12/15 21:09:35
 * @brief 
 *  
 **/

#include <stdio.h>
#include "nr3.h"
#include "recfunc.h"
#include "matop.h"


double calc_uavg(MatDoub & mat)/*{{{*/
{
    double total_rating_val = 0.0f;
    int total_rating_num = 0;
    for(int i = 0; i < mat.nrows(); i++)
    {
        for(int j = 0; j < mat.ncols(); j++)
        {
            double r = mat[i][j];
            if(r > REC_MAX_RATE_VAL)
                continue;
            total_rating_val += r;
            total_rating_num++;
        }
    }
   
    // 计算所有用户的  bias    
    if(total_rating_num == 0)
    {
        printf("rating_num:0, can no process");
        return -1.0f;
    }

    return total_rating_val/total_rating_num;

}/*}}}*/

void parse_useritem_func(void * param/*{{{*/
		, uint32_t item_num,char *item_ptrs[],const char * cont)
{
    MatDoub &mat = (*(MatDoub*)param);
    int row_idx = atoi(item_ptrs[0]);
    int col_idx = atoi(item_ptrs[1]);
    double rating = atof(item_ptrs[2]);

    if(row_idx > mat.nrows() || col_idx > mat.ncols()
            || row_idx < 1 || col_idx < 1)
    {
        printf("error row:%d or col:%d, range:%d*%d",
                row_idx, col_idx, mat.nrows(), mat.ncols());
        exit(1);
    }
    mat[row_idx-1][col_idx-1] = rating;

}/*}}}*/
















/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
