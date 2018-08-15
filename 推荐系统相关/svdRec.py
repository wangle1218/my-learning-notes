# -*- encoding:utf-8 -*-
# author: wangle 

import numpy as np
import random
import time

class SVD(object):
    """
    implementation of SVD for CF
    Reference:
    A Guide to Singular Value Decomposition for Collaborative Filtering
    """
    def __init__(self, epoch, eta, userNums, itemNums, ku=0.001, km=0.001, \
                f=30,save_model=False):
        super(SVD, self).__init__()
        self.epoch = epoch
        self.userNums = userNums
        self.itemNums = itemNums
        self.eta = eta
        self.ku = ku
        self.km = km
        self.f = f
        self.save_model = save_model

        self.U = None
        self.M = None


    def fit(self, train, val=None):
        rateNums = train.shape[0]
        self.meanV = np.sum(train[:,2]) / rateNums
        initv = np.sqrt((self.meanV - 1) / self.f)
        self.U = initv + np.random.uniform(-0.01,0.01,(self.userNums+1,self.f))
        self.M = initv + np.random.uniform(-0.01,0.01,(self.itemNums+1,self.f))
        self.bu = np.zeros(self.userNums + 1)
        self.bi = np.zeros(self.itemNums + 1)
        
        start = time.time()
        for i in range(self.epoch):
            sumRmse = 0.0
            for sample in train:
                uid = sample[0]
                iid = sample[1]
                vij = float(sample[2])
                # p(U_i,M_j) = mu + b_i + b_u + U_i^TM_j
                p = self.meanV + self.bu[uid] + self.bi[iid] + \
                    np.sum(self.U[uid] * self.M[iid])
                error = vij - p
                sumRmse += error**2
                # 计算Ui,Mj的梯度
                deltaU = error * self.M[iid] - self.ku * self.U[uid]
                deltaM = error * self.U[uid] - self.km * self.M[iid]
                # 更新参数
                self.U[uid] += self.eta *  deltaU
                self.M[iid] += self.eta *  deltaM

                self.bu[uid] += self.eta * (error - self.ku * self.bu[uid])
                self.bi[iid] += self.eta * (error - self.km * self.bi[iid])

            trainRmse = np.sqrt(sumRmse/rateNums)

            if val.any():
                _ , valRmse = self.evaluate(val)
                print("Epoch %d cost time %.4f, train RMSE: %.4f, validation RMSE: %.4f" % \
                    (i, time.time()-start, trainRmse, valRmse))
            else:
                print("Epoch %d cost time %.4f, train RMSE: %.4f" % \
                    (i, time.time()-start, trainRmse))

        if self.save_model:
            model = (self.meanV, self.bu, self.bi, self.U, self.M)
            pickle.dump(model, open(save_model + '/svcRecModel.pkl', 'wb'))


    def evaluate(self, val):
        loss = 0
        pred = []
        for sample in val:
            uid = sample[0]
            iid = sample[1]
            if uid > self.userNums or iid > self.itemNums:
                continue
            
            predi = self.meanV + self.bu[uid] + self.bi[iid] \
                    + np.sum(self.U[uid] * self.M[iid])
            if predi < 1:
                predi = 1
            elif predi > 5:
                predi = 5
            pred.append(predi)

            if val.shape[1] == 3:
                vij = sample[2]
                loss += (predi - vij)**2

        if val.shape[1] == 3:
            rmse = np.sqrt(loss/val.shape[0])
            return pred, rmse

        return pred

    def predict(self,test):

        return self.evaluate(test)

def test():
    import pandas as pd
    data_path = 'ratings.dat'
    data = pd.read_csv(data_path, sep='::', header=None, \
            names = ['user','item','rate','time'],engine='python')
    
    data = data.sample(frac=1)
    print(data.head())

    del data['time']
    trainNum = int(data.shape[0] * 0.8)
    train = data[:trainNum].values
    val = data[trainNum:].values

    userNums = data['user'].max()
    itemNums = data['item'].max()
    svd = SVD(25, 0.001, userNums, itemNums, f=50)
    svd.fit(train,val=val)

if __name__ == '__main__':
    test()







