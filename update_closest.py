import os
import cv2
import numpy as np
from sklearn.metrics import classification_report
from sklearn.ensemble import RandomForestClassifier
from sklearn import metrics
import pickle


#读取训练集
def read_train(X, Y, classifications):
    for i in classifications:
        # 遍历文件夹，读取图片
        for f in os.listdir("archive/seg_train/%s" % i):
            # 获取图像名称
            X.append("archive/seg_train/" + str(i) + "/" + str(f))
            # 获取图像类标即为文件夹名称
            Y.append(i)
    X = np.array(X)
    Y = np.array(Y)
#读取测试集
def read_test(X, Y, classifications):
    for i in classifications:
        # 遍历文件夹，读取图片
        for f in os.listdir("archive/seg_test/%s" % i):
            # 获取图像名称
            X.append("archive/seg_test/" + str(i) + "/" + str(f))
            # 获取图像类标即为文件夹名称
            Y.append(i)
    X = np.array(X)
    Y = np.array(Y)

#计算颜色直方图
def data_hist(X, X_hist):
    for i in X:
        # 读取图像
        image = cv2.imread(i)

        # 图像像素大小一致
        img = cv2.resize(image, (256, 256), interpolation=cv2.INTER_CUBIC)
        # 计算图像直方图并存储至X数组
        hist = cv2.calcHist([img], [0, 1, 2], None,
                            [8, 8, 8], [0.0, 255.0, 0.0, 255.0, 0.0, 255.0])

        X_hist.append(((hist / 255).flatten()))

#计算单个图像的颜色直方图
def data_hist1(X):
        # 读取图像
        image = cv2.imread(X)
        # 图像像素大小一致
        img = cv2.resize(image, (256, 256), interpolation=cv2.INTER_CUBIC)
        # 计算颜色直方图
        hist = cv2.calcHist([img], [0, 1, 2], None,
                            [8, 8, 8], [0.0, 255.0, 0.0, 255.0, 0.0, 255.0])
        return (((hist / 255).flatten()))

#计算HSV直方图
def calchist_HSV(data, data_hist):
    for i, filename in enumerate(data):
        image = cv2.imread(filename)
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        hist1 = cv2.calcHist([hsv], [0, 1], None, [90, 256], [0, 180, 0, 256])
        hist1 = cv2.normalize(hist1, hist1).flatten()
        data_hist[filename] = hist1

#比较颜色直方图
def hist_compare(src, index):
    result = {}  # 保存相似度计算结果
    compareFileName = src # 需要进行相似比较的图像
    for (k, hist) in index.items():
        # 使用巴氏距离方法，该值越小，则两幅图像越相似
        d = cv2.compareHist(index[compareFileName], hist, cv2.HISTCMP_BHATTACHARYYA)
        result[k] = str(d)
    # 排序相似度结果，从小到大
    result = sorted(result.items(), key=lambda x: x[1])
    # 选择最相似的前五幅
    result = result[:5]
    print("与上述图片最接近的五幅图片分别是：")
    for i in result:
        print(i)

if __name__ == '__main__':
    # ******************************
    # 第一步 训练随机森林分类器
    # ******************************

    X_train = []  # 训练集图像名称
    y_train = []  # 训练集图像分类类标
    X_test = []   # 测试集图像名称
    y_test = []   # 测试集图像分类类标
    classifications = ['buildings', 'forest', 'glacier', 'mountain', 'sea', 'street'] # 图片的所有分类
    read_train(X_train, y_train, classifications) # 读取训练集
    read_test(X_test, y_test, classifications)  # 读取测试集
    print(len(X_train), len(X_test), len(y_train), len(y_test))  # 查看训练集和测试集的大小
    X_train_hist = []   # 保存训练集的颜色直方图
    X_test_hist = []    # 存测试集的颜色直方图
    # 分别计算训练集和测试集的颜色直方图
    data_hist(X_train, X_train_hist)
    data_hist(X_test, X_test_hist)
    # 使用随机森林分类器
    try:
        with open("model.pkl", "rb") as f:  # 若模型已经存在，加载已保存的模型
            model = pickle.load(f)
    except Exception:   # 若模型不存在，重新训练
        model = RandomForestClassifier(n_jobs=-1, n_estimators=350, max_depth=11, oob_score=1, random_state=1).fit(X_train_hist, y_train)
        with open("model.pkl", "wb") as f:  # 保存模型
            pickle.dump(model, f)
    predictions_labels = model.predict(X_test_hist)  # 保存预测结果
    # 输出算法评价指标
    print('准确率accuracy：{:.4f}'.format(metrics.accuracy_score(y_test, predictions_labels), '.4f'))
    print('查准率Precision：{:.4f}'.format(metrics.precision_score(y_test, predictions_labels, average='macro'), '.4f'))
    print('查全率reall：{:.4f}'.format(metrics.recall_score(y_test, predictions_labels, average='macro'), '.4f'))
    print(classification_report(y_test, predictions_labels))

    # ******************************************************************
    # 第二步 利用分类器找到某幅图片的分类，在该分类下的所有图片中寻找与之最接近的五幅图片
    # ******************************************************************

    number = 2372   # 该副图片img的编号，从0~2999
    src_img = X_test[number]    # img的名称/路径
    # 找出源图片的真实分类
    for i in classifications:
        if i in src_img:
            src_label = i
    print('图片{}的预测分类是:'.format(src_img)+str(predictions_labels[number])+'；真实分类是:'+str(src_label))
    # 找到与img同一分类下的所有图片
    families = []
    for i in range(0, len(X_test)):
        if predictions_labels[i] == src_label:
            families.append(i)
    # 保存同一分类所有图片的名称/路径
    families_names = []
    for i in families:
        families_names.append(X_test[i])
    families_hist = {}  # 保存同一分类所有图片的颜色直方图
    calchist_HSV(families_names, families_hist)  # 计算同一分类所有图片的颜色直方图
    hist_compare(src_img, families_hist)  # 与img的直方图进行比较，找出最接近的五幅图片

