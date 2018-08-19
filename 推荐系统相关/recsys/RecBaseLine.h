/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file RecBaseLine.h
 * @author lihaibo(com@baidu.com)
 * @date 2013/12/17 10:56:46
 * @brief CF基线对比基线
 *  
 **/


#ifndef  __RECBASELINE_H_
#define  __RECBASELINE_H_

#include "nr3.h"
#include "recdef.h"
#include "recfunc.h"



class RecBaseLine : public RecTask/*{{{*/
{
    private:
        int _lambdaU;
        int _lambdaI;
        int _mat_train_row;
        int _mat_train_col;
        double _u_avg;

        MatDoub _mat_train;
        VecDoub _vu;
        VecDoub _vi;

        int _calc_baseline(MatDoub & mat, double U_AVG
            , int lambdaU, int lambdaI)
        /*{{{*/
        {
            U_AVG = calc_uavg(mat);
            VecUint vic(_vi.size()); //存储 item i 当前被投票的次数
            int uc = 0;
            double rating_val = 0.0f;
            double total_rating_val = 0.0f;
            int total_rating_num = 0;

            for(int i = 0; i < mat.nrows(); i++)
            {
                uc = 0;
            rating_val = 0.0f;
            for(int j = 0; j < mat.ncols(); j++)
            {
                double r = mat[i][j];
                if(r > REC_MAX_RATE_VAL)
                    continue;
                uc++;
                rating_val += r;
                _vi[j] += r;
                vic[j]++;
            }

            // 更新用户i的bias
            if(uc <= 0)
            {
                _vu[i] = REC_NO_RATE_VAL; 
            }
            else
            {
                _vu[i] = (rating_val - U_AVG * uc)/(uc + lambdaU);
            }
        }

        // 计算每个item的bias
        for(int i = 0; i < _vi.size(); i++)
        {
            if(vic[i] <= 0)
            {
                _vi[i] = REC_NO_RATE_VAL;
            }
            else
            {
                _vi[i] = (_vi[i] - U_AVG * vic[i])/(vic[i] + lambdaI);
            }
        }
        return NR_SUCC;
    }/*}}}*/
    public:
        RecBaseLine():_lambdaU(0), _lambdaI(0), _mat_train_row(0), _mat_train_col(0){}

        RecBaseLine(int laU, int laI)/*{{{*/
        {
            _lambdaU = laU;
            _lambdaI = laI;
            _mat_train_row = 0;
            _mat_train_col = 0;
            _u_avg = 0.0f;
        }/*}}}*/

        virtual int load_data(const char *file_name, int row, int col)/*{{{*/
        {
            // 载入数据后马上计算 average rating
            _mat_train_row = row;
            _mat_train_col = col;
            _mat_train.assign(_mat_train_row, _mat_train_col, REC_NO_RATE_VAL);
            _vu.assign(_mat_train_row, REC_NO_RATE_VAL);
            _vi.assign(_mat_train_col, REC_NO_RATE_VAL);
            load_item_line(file_name, 4, (void*)&_mat_train
                    , parse_useritem_func, "\t");
            _u_avg = calc_uavg(_mat_train);
            return NR_SUCC;
        }/*}}}*/

        virtual int train()/*{{{*/
        {
            int ret = _calc_baseline(_mat_train
                    , _u_avg, _lambdaU, _lambdaI);
            {
                if(ret != NR_SUCC)
                {
                    printf("_calc_baseline failed:%d\n", ret);
                    return NR_FAIL;
                }
            }
            return NR_SUCC;
        }/*}}}*/

        virtual double predict(int user, int item)/*{{{*/
        {
            double rate = 0.0f;
            if(_vu[user] < REC_MAX_RATE_VAL && _vi[item] < REC_MAX_RATE_VAL)
            {
                return _u_avg + _vu[user] + _vi[item];
            }
            else
            {
                return _u_avg;
            }
        }/*}}}*/

        virtual void dump_param() const/*{{{*/
        {
            printf("mu:%f,lambdau:%d,lambdai:%d\n"
                    , _u_avg, _lambdaU, _lambdaI);
        }/*}}}*/
};/*}}}*/

class RecBaseLineAdv : public RecTask/*{{{*/
{
    private:
        int _lambdaU;
        int _lambdaI;
        int _mat_train_row;
        int _mat_train_col;
        double _u_avg;

        MatDoub _mat_train;
        VecDoub _vu;
        VecDoub _vi;

        int _calc_baseline(MatDoub & mat, double U_AVG
            , int lambdaU, int lambdaI)
        /*{{{*/
        {
            U_AVG = calc_uavg(mat);
            int cnt = 0;
            double rating_val = 0.0f;
            double total_rating_val = 0.0f;
            int total_rating_num = 0;

            // 计算item bias
            for(int j = 0; j < mat.ncols(); j++)
            {
                cnt = 0;
                rating_val = 0.0f;
                for(int i = 0; i < mat.nrows(); i++)
                {
                    double r = mat[i][j];
                    if(r > REC_MAX_RATE_VAL)
                        continue;
                    cnt++;
                    rating_val += r;
                }

                //  更新item j的bias
                if(cnt <= 0)
                {
                    _vi[j] = REC_NO_RATE_VAL; 
                }
                else
                {
                    _vi[j] = (rating_val - U_AVG * cnt)/(cnt + lambdaI);
                }
            }

            // 计算 user bias
            for(int i = 0; i < mat.nrows(); i++)
            {
                cnt = 0;
                rating_val = 0.0f;
                for(int j = 0; j < mat.ncols(); j++)
                {
                    double r = mat[i][j];
                    if(r > REC_MAX_RATE_VAL)
                        continue;
                    cnt++;
                    if(_vi[j] > REC_MAX_RATE_VAL)
                    {
                        rating_val += r;
                    }
                    else
                    {
                        rating_val += (r + _vi[j]);
                    }
                }
                if(cnt <= 0)
                {
                    _vu[i] = REC_NO_RATE_VAL;
                }
                else
                {
                    _vu[i] = (rating_val - U_AVG * cnt)/(cnt  + lambdaU);
                }
            }

        return NR_SUCC;
    }/*}}}*/
    public:
        RecBaseLineAdv():_lambdaU(0), _lambdaI(0), _mat_train_row(0), _mat_train_col(0){}

        RecBaseLineAdv(int laU, int laI)/*{{{*/
        {
            _lambdaU = laU;
            _lambdaI = laI;
            _mat_train_row = 0;
            _mat_train_col = 0;
            _u_avg = 0.0f;
        }/*}}}*/

        virtual int load_data(const char *file_name, int row, int col)/*{{{*/
        {
            // 载入数据后马上计算 average rating
            _mat_train_row = row;
            _mat_train_col = col;
            _mat_train.assign(_mat_train_row, _mat_train_col, REC_NO_RATE_VAL);
            _vu.assign(_mat_train_row, REC_NO_RATE_VAL);
            _vi.assign(_mat_train_col, REC_NO_RATE_VAL);
            load_item_line(file_name, 4, (void*)&_mat_train
                    , parse_useritem_func, "\t");
            _u_avg = calc_uavg(_mat_train);
            return NR_SUCC;
        }/*}}}*/

        virtual int train()/*{{{*/
        {
            int ret = _calc_baseline(_mat_train
                    , _u_avg, _lambdaU, _lambdaI);
            {
                if(ret != NR_SUCC)
                {
                    printf("_calc_baseline failed:%d\n", ret);
                    return NR_FAIL;
                }
            }
            return NR_SUCC;
        }/*}}}*/

        virtual double predict(int user, int item)/*{{{*/
        {
            double rate = 0.0f;
            if(_vu[user] < REC_MAX_RATE_VAL && _vi[item] < REC_MAX_RATE_VAL)
            {
                return _u_avg + _vu[user] + _vi[item];
            }
            else
            {
                return _u_avg;
            }
        }/*}}}*/

        virtual void dump_param() const/*{{{*/
        {
            printf("mu:%f,lambdau:%d,lambdai:%d\n"
                    , _u_avg, _lambdaU, _lambdaI);
        }/*}}}*/
};/*}}}*/

class BaseLineCreator: public TaskCreator/*{{{*/
{
    public:
        BaseLineCreator(const char * name
                , int user_num, int item_num
                , const char * file_train
                , const char * file_test
                , int lambdau, int lambdai):TaskCreator(name)
        {

            _prec_task = new RecBaseLine(lambdau, lambdai);
            _prec_task->load_data(file_train, user_num, item_num);

            _ptest_task = new TestTask(_prec_task);
            _ptest_task->load_data(file_test, user_num , item_num);
        }
        ~BaseLineCreator()
        {
            delete _ptest_task;
            delete _prec_task;
        }
};/*}}}*/

class BaseLineAdvCreator: public TaskCreator/*{{{*/
{
    public:
        BaseLineAdvCreator(const char * name
                , int user_num, int item_num
                , const char * file_train
                , const char * file_test
                , int lambdau, int lambdai):TaskCreator(name)
        {

            _prec_task = new RecBaseLineAdv(lambdau, lambdai);
            _prec_task->load_data(file_train, user_num, item_num);

            _ptest_task = new TestTask(_prec_task);
            _ptest_task->load_data(file_test, user_num , item_num);
        }
        ~BaseLineAdvCreator()
        {
            delete _ptest_task;
            delete _prec_task;
        }
};/*}}}*/

#endif  //__RECBASELINE_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
