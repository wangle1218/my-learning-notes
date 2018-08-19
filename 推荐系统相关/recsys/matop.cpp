/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file matop.cpp
 * @author lihaibo(com@baidu.com)
 * @date 2013/12/10 16:14:02
 * @brief 
 *  
 **/

#include "matop.h"

int MatMul(MatDoub & rslt, MatDoub & a, MatDoub & b)/*{{{*/
{
    if(a.ncols() != b.nrows())
    {
        return 1;
    }
    rslt.resize(a.nrows(), b.ncols());
    for(int i = 0; i < a.nrows(); i++)
    {
        for(int j = 0; j < b.ncols(); j++)
        {
            double s = 0.0f;
            for(int k = 0; k < a.ncols(); k++)
                s += a[i][k] * b[k][j];
            rslt[i][j] = s;
        }
    }
    return 0;
}/*}}}*/

int MatDiag(MatDoub & rslt, VecDoub & v)/*{{{*/
{
    rslt.resize(v.size(), v.size());
    for(int i = 0; i < rslt.nrows(); i++)
    {
        for(int j = 0; j < rslt.ncols(); j++)
        {
            if(i == j)
            {
                rslt[i][i] = v[i];
            }
            else
            {
                rslt[i][j] = 0.0f;
            }
        }
    }
    return 0;
}/*}}}*/

int MatReverse(MatDoub & rslt, MatDoub & a)/*{{{*/
{
    rslt.resize(a.ncols(), a.nrows());
    for(int i = 0; i < a.nrows(); i++)
    {
        for(int j = 0; j < a.ncols(); j++)
            rslt[j][i] = a[i][j];
    }
    return 0;
}/*}}}*/

void MatDump(MatDoub & rslt, const char * mat_name = "matrix") /*{{{*/
{

    printf("%s\n", mat_name);
    for(int i = 0; i < rslt.nrows(); i++)
    {
        for(int j = 0; j < rslt.ncols(); j++)
        {
            printf(" %f", rslt[i][j]);
        }
        printf("\n");
    }
}
/*}}}*/

void MatZero(MatDoub & mat)/*{{{*/
{
    for(int i = 0; i < mat.nrows(); i++)
        for(int j = 0; j < mat.ncols(); j++)
            mat[i][j] = 0;
}/*}}}*/

int MatInner(MatDoub & mata, MatDoub & matb/*{{{*/
        , int row, int col, double & rslt)
{
    // 计算矩阵中两个向量的内积
    if(mata.ncols() != matb.nrows())
    {
        printf("matb row:%d, mata col:%d not equal");
        return 1;
    }

    rslt = 0.0f;
    for(int i = 0; i < mata.ncols(); ++i)
    {
        rslt += mata[row][i] * matb[i][col];
    }
    
    return 0;
}/*}}}*/

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
