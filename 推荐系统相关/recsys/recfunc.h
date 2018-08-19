/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file recfunc.h
 * @author lihaibo(com@baidu.com)
 * @date 2013/12/15 21:07:58
 * @brief 
 *  
 **/




#ifndef  __RECFUNC_H_
#define  __RECFUNC_H_

#include "nr3.h"
#include "matop.h"
#include "recdef.h"
#include "str_util.h"

/**
 * @brief 计算矩阵中投票的均值
 *
 * @param [in/out] mat   : MatDoub& rating matrix
 * @return  double  average rating
 * @retval   
 * @see 
 * @note mat中未投票的元素需设置为 REC_NO_RATE_VAL 
 * @author lihaibo
 * @date 2013/12/17 11:36:48
**/
double calc_uavg(MatDoub & mat);


/**
 * @brief 解析投票文件函数
 *
 * @param [in/out] param   : void*
 * @param [in/out] item_num   : uint32_t
 * @param [in/out] item_ptrs   : char*[]
 * @param [in/out] cont   : const char*
 * @return  void 
 * @retval   
 * @see 
 * @note 
 * @author lihaibo
 * @date 2013/12/17 12:43:34
**/
void parse_useritem_func(void * param
		, uint32_t item_num,char *item_ptrs[]
        , const char * cont);


class RecTask/*{{{*/
{
    public:
        virtual int load_data(const char * file_name, int r, int c) = 0;
        virtual int train() = 0;
        virtual double predict(int user, int item) = 0;
        virtual void dump_param()const = 0;
};/*}}}*/

class TestTask/*{{{*/
{
    private:
        int _mat_row;
        int _mat_col;
        MatDoub _mat_test;
        RecTask * _prec_task;
        char _test_file_name[NR_MAX_STR_LEN];
    public:
        TestTask(RecTask * ptask)
        {
            _prec_task = ptask;
            _mat_row = 0;
            _mat_col = 0;
        }

        virtual int load_data(const char *file_name,int row, int col)/*{{{*/
        {
            _mat_row = row;
            _mat_col = col;
            _mat_test.assign(_mat_row, _mat_col, REC_NO_RATE_VAL);
            load_item_line(file_name, 4, (void*)&_mat_test
                    , parse_useritem_func, "\t");
            return NR_SUCC;
        }/*}}}*/

        double MAE()/*{{{*/
        {
            double total_bias = 0.0f;
            int num = 0;
            for(int i = 0; i < _mat_test.nrows(); i++)
            {
                for(int j = 0; j < _mat_test.ncols(); j++)
                {
                    if(_mat_test[i][j] >= REC_MAX_RATE_VAL)
                        continue;
                    total_bias += abs(_prec_task->predict(i, j) - _mat_test[i][j]);
                    num++;
                }
            }
            if(num > 0)
            {
                return total_bias/num;
            }
            else
            {
                return 0.0f;
            }
        }/*}}}*/

        double RMSE()/*{{{*/
        {
            double total_bias = 0.0f;
            int num = 0;
            for(int i = 0; i < _mat_test.nrows(); i++)
            {
                for(int j = 0; j < _mat_test.ncols(); j++)
                {
                    if(_mat_test[i][j] >= REC_MAX_RATE_VAL)
                        continue;
                    total_bias += pow(abs(_prec_task->predict(i, j) - _mat_test[i][j]), 2);
                    num++;
                }
            }
            if(num > 0)
            {
                return pow(total_bias/num, 0.5);
            }
            else
            {
                return 0.0f;
            }
        }/*}}}*/

};/*}}}*/

class TaskCreator/*{{{*/
{
    protected:
        RecTask * _prec_task;
        TestTask * _ptest_task;
        char _task_name[NR_MAX_STR_LEN];
    public:
        TaskCreator(const char * name)
        {
            strncpy(_task_name, name, sizeof(_task_name)-1);
        }
        virtual ~TaskCreator()
        {
            //delete _prec_task;
            //delete _ptest_task;
        }
        virtual int create(){} ;
        virtual int run()
        {
            double mae = 0.0f;
            double rmse = 0.0f;
            _prec_task->train();
            mae = _ptest_task->MAE();
            rmse = _ptest_task->RMSE();
            printf("\ntask:%s,mae:%f,rmse:%f\n",_task_name, mae, rmse);
            _prec_task->dump_param();
            return NR_SUCC;
        }
};/*}}}*/


#endif  //__RECFUNC_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
