/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file matop.cpp
 * @author lihaibo(com@baidu.com)
 * @date 2013/12/06 17:01:00
 * @brief 
 *  
 **/

#ifndef __MAT_OP_H__
#define __MAT_OP_H__

#include "nr3.h"


int MatMul(MatDoub & rslt, MatDoub & a, MatDoub & b);

int MatDiag(MatDoub & rslt, VecDoub & v);

int MatReverse(MatDoub & rslt, MatDoub & a);

void MatDump(MatDoub & rslt, const char * mat_name );

void MatZero(MatDoub & mat);

int MatInner(MatDoub & mata, MatDoub & matb
        , int row, int col, double & rslt);

#endif

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
