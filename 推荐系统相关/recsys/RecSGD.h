/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file RecSGD.h
 * @author lihaibo(com@baidu.com)
 * @date 2013/12/17 18:04:09
 * @brief 因式分解方式进行CF，使用梯度下降法获取最优参数
 *  
 **/


#ifndef  __RECSGD_H_
#define  __RECSGD_H_

class RecSGD : public RecTask
{
    private:
        double _u_avg;
        double _lambda;
        double _gama;
        double _min_res_err;
        int _max_iter_num;
        int _fea_dim;

        MatDoub _mat_train;
        MatDoub _matpu;
        MatDoub _matpi;
        VecDoub _vecbu;
        VecDoub _vecbi;

        int _calc_sgd(MatDoub &mat, double &u_avg
                , double gama, double lamda
                , int max_iter_num, double min_res_err
                , VecDoub & vbi, VecDoub & vbu
                , MatDoub & matpi, MatDoub & matpu)
        /*{{{*/
        {

            // user, item feature空间必须一致
            if(matpu.ncols() != matpi.nrows())
            {
                printf("feature space dimension not equal.pu:%d,pi:%d"
                        , matpu.ncols(), matpi.nrows());
                return NR_FAIL;
            }
            // 使用随机梯度下降方式计算
            int iter_num = 0;
            double total_res_err = INIT_RES_ERROR;
        
            while((iter_num < max_iter_num) && (total_res_err > min_res_err))
            {
                total_res_err = 0.0f;
                for(int i = 0; i < mat.nrows(); i++)
                {
                    for(int j = 0; j < mat.ncols(); j++)
                    {
                        // 仅计算有效投票的 rating 
                        if(mat[i][j] > REC_MAX_RATE_VAL)
                            continue;

                        double inner = 0.0f;
                        MatInner(matpu, matpi, i, j, inner); //内积
                        double rate_predict = u_avg + vbu[i] + vbi[j] + inner;
                        double err = mat[i][j] - rate_predict;
                        total_res_err += err;
                
                        // 更新参数 bu, bi
                        vbu[i] = vbu[i] + gama * (err - lamda * vbu[i]);
                        vbi[j] = vbi[j] + gama * (err - lamda * vbi[j]);

                        // 更新参数pu, pi
                        for(int fea_idx = 0; fea_idx < matpu.ncols(); fea_idx++)
                        {
                            matpu[i][fea_idx] = matpu[i][fea_idx] + gama \
                                * (err * matpi[fea_idx][j] - lamda * matpu[i][fea_idx]);
                            matpi[fea_idx][j] = matpi[fea_idx][j] + gama \
                                * (err * matpu[i][fea_idx] - lamda * matpi[fea_idx][j]);               
                        }
                    }
                }
                iter_num++;
            }
            return NR_SUCC;
        }/*}}}*/

    public:
        RecSGD(double ld, double gm, double res_err, int inum, int dm)/*{{{*/
        {
            _lambda = ld;
            _gama = gm;
            _min_res_err = res_err;
            _max_iter_num = inum;
            _fea_dim = dm;
        }/*}}}*/

        virtual int load_data(const char *file_name, int row, int col)/*{{{*/
        {
            // 载入数据后马上计算 average rating
            _mat_train.assign(row, col, REC_NO_RATE_VAL);
            _matpu.assign(_mat_train.nrows(), _fea_dim, 0.01f);
            _matpi.assign(_fea_dim, _mat_train.ncols(), 0.01f);
            _vecbu.assign(_mat_train.nrows(), 0.01f);
            _vecbi.assign(_mat_train.ncols(), 0.01f);
            load_item_line(file_name, 4, (void*)&_mat_train
                    , parse_useritem_func, "\t");
            _u_avg = calc_uavg(_mat_train);

            return NR_SUCC;
        }/*}}}*/

        virtual int train()/*{{{*/
        {
            int ret = _calc_sgd(_mat_train
                    , _u_avg, _gama, _lambda
                    , _max_iter_num, _min_res_err
                    , _vecbi, _vecbu
                    , _matpi, _matpu);
            {
                if(ret != NR_SUCC)
                {
                    printf("_calc_sgd failed:%d\n", ret);
                    return NR_FAIL;
                }
            }
            return NR_SUCC;
        }/*}}}*/

        virtual double predict(int user, int item)/*{{{*/
        {
            double rate = 0.0f;
            double inner = 0.0f;
            MatInner(_matpu, _matpi, user, item, inner);
            return _u_avg + _vecbu[user] + _vecbi[item] + inner;
         }/*}}}*/

        virtual void dump_param()const/*{{{*/
        {
            printf("mu:%f,lambda:%f,gama:%f,"
                    ,_u_avg, _lambda, _gama);
            printf("min_res_err:%f,max_iter_num:%d,fea_dim:%d\n"
                    , _min_res_err, _max_iter_num, _fea_dim);
        }/*}}}*/

};

class SGDCreator: public TaskCreator/*{{{*/
{
    public:
        SGDCreator(const char * name
                , int user_num, int item_num
                , const char * file_train
                , const char * file_test
                , double gama, double lambda
                , int iter_num, double res_err
                , int fea_dim):TaskCreator(name)
        {

            _prec_task = new RecSGD(lambda, gama
                    , res_err, iter_num, fea_dim);
            _prec_task->load_data(file_train, user_num, item_num);

            _ptest_task = new TestTask(_prec_task);
            _ptest_task->load_data(file_test, user_num , item_num);
        }
};/*}}}*/


#endif  //__RECSGD_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
