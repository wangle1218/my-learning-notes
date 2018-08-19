/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file recsys.cpp
 * @author lihaibo(com@baidu.com)
 * @date 2013/12/12 11:37:52
 * @brief   主函数，直接调用Creator实验CF Baseline及 因式分解推荐方法
 *          没有配置文件，如要实验其他参数，可稍微修改main函数及参数实现外部调用
 *          程序为之前调研时所写，目前加上注释供大家参考， hadoop版本的CF实现可以直接
 *          联系作者
 **/
#include <stdio.h>
#include <stdlib.h>
#include "nr3.h"
#include "str_util.h"
#include "matop.h"
#include "recdef.h"
#include "recfunc.h"
#include "RecBaseLine.h"
#include "RecSGD.h"

int main(int argc, char *argv)
{
    const int USER_NUM = 943;
    const int ITEM_NUM = 1682;
    char * file_name_train = "data/ml-100k/u1.base";
    char * file_name_test = "data/ml-100k/u1.test";
    BaseLineCreator rec_creator("baseline"
            , USER_NUM, ITEM_NUM
            , file_name_train, file_name_test
            , 0, 0);
    rec_creator.create();
    rec_creator.run();

    BaseLineCreator rec_creator1("baseline1"
            , USER_NUM, ITEM_NUM
            , file_name_train, file_name_test
            , 10, 10);
    rec_creator1.create();
    rec_creator1.run();

    BaseLineCreator rec_creator2("baseline1"
            , USER_NUM, ITEM_NUM
            , file_name_train, file_name_test
            , 20, 20);
    rec_creator2.create();
    rec_creator2.run();

    BaseLineCreator rec_creator3("baseline1"
            , USER_NUM, ITEM_NUM
            , file_name_train, file_name_test
            , 30, 30);
    rec_creator3.create();
    rec_creator3.run();

    BaseLineAdvCreator rec_creatoradv("baselineadv"
            , USER_NUM, ITEM_NUM
            , file_name_train, file_name_test
            , 0, 0);
    rec_creatoradv.create();
    rec_creatoradv.run();

    BaseLineAdvCreator rec_creatoradv1("baselineadv"
            , USER_NUM, ITEM_NUM
            , file_name_train, file_name_test
            , 10, 10);
    rec_creatoradv1.create();
    rec_creatoradv1.run();

    BaseLineAdvCreator rec_creatoradv2("baselineadv"
            , USER_NUM, ITEM_NUM
            , file_name_train, file_name_test
            , 20, 20);
    rec_creatoradv2.create();
    rec_creatoradv2.run();

    BaseLineAdvCreator rec_creatoradv3("baselineadv"
            , USER_NUM, ITEM_NUM
            , file_name_train, file_name_test
            , 30, 30);
    rec_creatoradv3.create();
    rec_creatoradv3.run();

    double gama = 0.02; // 下降梯度
    double lamda = 0.2; // 防止过拟合
    double min_res_err = 0.01;
    int max_iter_num = 10000;
    int FEA_DIM = 1;
    SGDCreator rec_creatorSGD30("SGD30"
            , USER_NUM, ITEM_NUM
            , file_name_train, file_name_test
            , gama, lamda
            , max_iter_num, min_res_err
            , FEA_DIM);
    rec_creatorSGD30.create();
    rec_creatorSGD30.run();


    FEA_DIM = 20;
    SGDCreator rec_creatorSGD1("SGD10"
            , USER_NUM, ITEM_NUM
            , file_name_train, file_name_test
            , gama, lamda
            , max_iter_num, min_res_err
            , FEA_DIM);
    rec_creatorSGD1.create();
    rec_creatorSGD1.run();
    return NR_SUCC;
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
