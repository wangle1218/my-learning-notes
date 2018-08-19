# -*- encoding:utf-8 -*-
# author: wangle 

import numpy as np
import random
import time

class SVDPP(object):
    """
    implementation of SVD++ for CF
    """
    def __init__(self, epoch, eta, userNums, itemNums, ku=0.001, km=0.001, \
                f=30,save_model=False):
        super(SVDPP, self).__init__()
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
        # 构造每个用户有过评分行为字典
        self.Udict = {}
        for i in range(train.shape[0]):
            uid = train[i,0]
            iid = train[i,1]
            self.Udict.setdefault(uid,[])
            self.Udict[uid].append(iid)

        rateNums = train.shape[0]
        self.meanV = np.sum(train[:,2]) / rateNums
        initv = np.sqrt((self.meanV - 1) / self.f)
        self.U = initv + np.random.uniform(-0.01,0.01,(self.userNums+1,self.f))
        self.M = initv + np.random.uniform(-0.01,0.01,(self.itemNums+1,self.f))
        self.bu = np.zeros(self.userNums + 1)
        self.bi = np.zeros(self.itemNums + 1)
        self.y = np.zeros((self.itemNums+1, self.f)) + 0.1
        
        start = time.time()
        for i in range(self.epoch):
            sumRmse = 0.0
            for sample in train:
                uid = sample[0]
                iid = sample[1]
                vij = float(sample[2])

                sumYj, sqrt_Ni = self.get_Yi(uid)
                # p(U_i,M_j) = \mu + b_i + b_u + U_i^T (M_j + \frac{1}{\sqrt{|N_i|^2}}*\sum_{j\in N_i}y_j)
                p = self.meanV + self.bu[uid] + self.bi[iid] + \
                    np.sum(self.M[iid] * (self.U[uid] + sumYj))

                error = vij - p
                sumRmse += error**2
                # 计算Ui,Mj的梯度
                deltaU = error * self.M[iid] - self.ku * self.U[uid]
                deltaM = error * (self.U[uid] + sumYj) - self.km * self.M[iid]
                # 更新参数
                self.U[uid] += self.eta *  deltaU
                self.M[iid] += self.eta *  deltaM

                self.bu[uid] += self.eta * (error - self.ku * self.bu[uid])
                self.bi[iid] += self.eta * (error - self.km * self.bi[iid])

                # for j in self.Udict[uid]:
                #     self.y[j] += self.eta * (error * self.M[j]/sqrt_Ni - self.ku * self.y[j])

                rating_list = self.Udict[uid]
                self.y[rating_list] += self.eta * (error * self.M[rating_list]/sqrt_Ni - \
                    self.ku * self.y[rating_list])

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
            sumYj, _ = self.get_Yi(uid)
            predi = self.meanV + self.bu[uid] + self.bi[iid] \
                    + np.sum(self.M[iid] * (self.U[uid] + sumYj))
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

    # 计算 sqrt_Ni 和 ∑yj
    def get_Yi(self,uid):
        Ni = self.Udict[uid]
        numNi = len(Ni)
        sqrt_Ni = np.sqrt(numNi)
        yj = np.zeros((1, self.f))
        if numNi == 0:
            sumYj = yj + 0.1
        else:
            yj = np.mean(self.y[Ni],axis=0)
            sumYj = yj / sqrt_Ni
        return sumYj, sqrt_Ni

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
    svd = SVDPP(35, 0.001, userNums, itemNums, f=50)
    svd.fit(train,val=val)

if __name__ == '__main__':
    test()







