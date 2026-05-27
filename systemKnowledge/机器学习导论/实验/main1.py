import numpy as np
import pandas as pd
import sys
import os

# ---------------------------- 辅助函数 ----------------------------
def standardize(X, mean=None, std=None):
    if mean is None:
        mean = np.mean(X, axis=0)
        std = np.std(X, axis=0)
        std[std == 0] = 1
    return (X - mean) / std, mean, std

# ---------------------------- 加权逻辑回归 ----------------------------
class WeightedLogisticRegression:
    def __init__(self, learning_rate=0.01, max_iter=1000):
        self.lr = learning_rate
        self.max_iter = max_iter
        self.w = None

    def fit(self, X, y, sample_weight):
        y_bin = (y == 1).astype(int)
        m, n = X.shape
        X_bias = np.c_[np.ones(m), X]
        self.w = np.zeros(n + 1)
        for _ in range(self.max_iter):
            linear = X_bias @ self.w
            linear = np.clip(linear, -500, 500)
            pred = 1 / (1 + np.exp(-linear))
            error = pred - y_bin
            gradient = X_bias.T @ (sample_weight * error) / m
            self.w -= self.lr * gradient

    def predict(self, X):
        m = X.shape[0]
        X_bias = np.c_[np.ones(m), X]
        linear = X_bias @ self.w
        prob = 1 / (1 + np.exp(-linear))
        return np.where(prob >= 0.5, 1, -1)

# ---------------------------- 决策树桩 (正确的简化版本) ----------------------------
class DecisionStump:
    def __init__(self):
        self.feature_idx = None
        self.threshold = None
        self.polarity = 1

    def fit(self, X, y, sample_weight):
        m, n = X.shape
        best_error = float('inf')
        best_stump = None
        for f in range(n):
            values = X[:, f]
            sorted_idx = np.argsort(values)
            sorted_vals = values[sorted_idx]
            for i in range(m-1):
                if sorted_vals[i] == sorted_vals[i+1]:
                    continue
                thresh = (sorted_vals[i] + sorted_vals[i+1]) / 2.0
                # 左 +1，右 -1
                pred = np.where(values <= thresh, 1, -1)
                error = np.sum(sample_weight * (pred != y))
                if error < best_error:
                    best_error = error
                    best_stump = (f, thresh, 1)
                # 翻转：左 -1，右 +1
                pred_flip = -pred
                error_flip = np.sum(sample_weight * (pred_flip != y))
                if error_flip < best_error:
                    best_error = error_flip
                    best_stump = (f, thresh, -1)
        # 防止best_stump为None（理论上总能找到）
        if best_stump is None:
            self.feature_idx = 0
            self.threshold = 0.0
            self.polarity = 1
        else:
            self.feature_idx, self.threshold, self.polarity = best_stump

    def predict(self, X):
        pred = np.where(X[:, self.feature_idx] <= self.threshold, 1, -1)
        if self.polarity == -1:
            pred = -pred
        return pred

# ---------------------------- AdaBoost 通用 ----------------------------
class AdaBoost:
    def __init__(self, base_type='stump', T=100):
        self.T = T
        self.base_type = base_type
        self.alphas = []
        self.models = []

    def fit(self, X, y):
        m = X.shape[0]
        D = np.ones(m) / m
        for _ in range(self.T):
            if self.base_type == 'stump':
                model = DecisionStump()
            else:
                model = WeightedLogisticRegression()
            model.fit(X, y, D)
            pred = model.predict(X)
            error = np.sum(D * (pred != y))
            if error == 0:
                alpha = 1e10
                self.alphas.append(alpha)
                self.models.append(model)
                break
            if error >= 0.5:
                continue
            alpha = 0.5 * np.log((1 - error) / error)
            D = D * np.exp(-alpha * y * pred)
            D = D / np.sum(D)
            self.alphas.append(alpha)
            self.models.append(model)

    def predict(self, X):
        if not self.models:
            return np.ones(X.shape[0], dtype=int)
        pred_sum = np.zeros(X.shape[0])
        for alpha, model in zip(self.alphas, self.models):
            pred_sum += alpha * model.predict(X)
        pred = np.sign(pred_sum).astype(int)
        pred[pred == 0] = 1
        return pred

# ---------------------------- 10 折交叉验证 ----------------------------
def manual_kfold_indices(n_samples, n_splits=10, shuffle=True, random_state=42):
    indices = np.arange(n_samples)
    if shuffle:
        np.random.seed(random_state)
        np.random.shuffle(indices)
    fold_sizes = np.full(n_splits, n_samples // n_splits, dtype=int)
    fold_sizes[:n_samples % n_splits] += 1
    current = 0
    folds = []
    for fold_size in fold_sizes:
        folds.append(indices[current:current+fold_size])
        current += fold_size
    for i in range(n_splits):
        test_idx = folds[i]
        train_idx = np.concatenate([folds[j] for j in range(n_splits) if j != i])
        yield train_idx, test_idx

def run_cross_validation(data_path, target_path, base_type):
    # 读取指定的训练数据
    X = pd.read_csv(data_path, header=None).values
    y_orig = pd.read_csv(target_path, header=None).values.flatten()
    if set(np.unique(y_orig)) == {0, 1}:
        y = 2 * y_orig - 1
        need_transform = True
    else:
        y = y_orig
        need_transform = False

    X, _, _ = standardize(X)
    T_list = [1, 5, 10, 100]
    n = X.shape[0]
    os.makedirs('experiments', exist_ok=True)

    base_str = 'logreg' if base_type == 0 else 'stump'

    for T in T_list:
        fold = 1
        for train_idx, test_idx in manual_kfold_indices(n, n_splits=10, shuffle=True, random_state=42):
            X_train, X_test = X[train_idx], X[test_idx]
            y_train = y[train_idx]
            model = AdaBoost(base_type=base_str, T=T)
            model.fit(X_train, y_train)
            pred = model.predict(X_test)
            if need_transform:
                pred = (pred + 1) // 2
            ids = test_idx + 1
            filename = f'experiments/base{T}_fold{fold}.csv'
            np.savetxt(filename, np.column_stack((ids, pred)), delimiter=',', fmt='%d')
            print(f'Generated {filename}')
            fold += 1

# ---------------------------- 主入口 ----------------------------
if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: python main.py <data.csv> <targets.csv> <base_type>")
        print("  base_type: 0 for logistic regression, 1 for decision stump")
        sys.exit(1)
    data_path = sys.argv[1]
    target_path = sys.argv[2]
    base_type = int(sys.argv[3])
    run_cross_validation(data_path, target_path, base_type)