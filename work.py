import os
import cv2

#读取图片库中的所有图片，保存其文件名
def read_data():
    for f in os.listdir("archive/seg_pred/"):
        X_train.append("archive/seg_pred/" + str(f))

#计算RGB直方图
def calchist_RGB(data, data_hist):
    for i, filename in enumerate(data):
        image = cv2.imread(filename)
        img = cv2.resize(image, (256, 256), interpolation=cv2.INTER_CUBIC)
        hist = cv2.calcHist([img], [0, 1, 2], None, [8, 8, 8], [0, 256, 0, 256, 0, 256])
        hist = cv2.normalize(hist, hist).flatten()
        data_hist[filename] = hist

#比较
def hist_compare(src, index):
    result = {}
    compareFileName = src
    for (k, hist) in index.items():
        d = cv2.compareHist(index[compareFileName], hist, cv2.HISTCMP_BHATTACHARYYA)
        result[k] = d
    # 排序巴氏距离结果
    result = sorted(result.items(), key=lambda x: x[1])
    result = result[:50]
    print(result)

if __name__ == "__main__":
    X_train = []  # 图像名数组
    read_data()
    train_hist = {}
    calchist_RGB(X_train, train_hist)
    hist_compare("archive/seg_pred/103.jpg", train_hist)
    print("***************")
