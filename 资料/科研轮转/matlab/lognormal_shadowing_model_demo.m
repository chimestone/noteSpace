function lognormal_shadowing_model_demo()
    % 对数正态阴影模型完整演示（不含蒙特卡洛分析）
    close all; clc;
    
    %% 模型参数设置
    params.d0 = 1;          % 参考距离 (米)
    params.P0 = -40;        % 参考距离处的RSSI (dBm)
    params.n = 3.2;         % 路径损耗指数
    params.sigma = 4.5;     % 阴影衰落标准差 (dB)
    
    fprintf('=== 对数正态阴影模型参数 ===\n');
    fprintf('参考距离 d0: %.1f 米\n', params.d0);
    fprintf('参考功率 P0: %.1f dBm\n', params.P0);
    fprintf('路径损耗指数 n: %.2f\n', params.n);
    fprintf('阴影衰落标准差 σ: %.2f dB\n', params.sigma);
    
    %% 距离范围设置
    distances = 0.5:0.5:25;  % 分析距离范围
    
    %% 1. 理论模型分析
    analyze_theoretical_model(distances, params);
    
    %% 2. 参数估计实验
    parameter_estimation_experiment(params);
    
    %% 3. 定位性能评估
    positioning_performance_evaluation(params);
end

function analyze_theoretical_model(distances, params)
    % 分析理论模型特性
    
    fprintf('\n=== 理论模型分析 ===\n');
    
    % 计算理论平均RSSI
    mean_rssi = params.P0 - 10 * params.n * log10(distances/params.d0);
    
    % 计算不同距离处的RSSI分布上下界 (±2σ)
    upper_bound = mean_rssi + 2 * params.sigma;
    lower_bound = mean_rssi - 2 * params.sigma;
    
    figure('Position', [100, 100, 1200, 400]);
    
    % 子图1: RSSI随距离变化
    subplot(1,3,1);
    plot(distances, mean_rssi, 'b-', 'LineWidth', 3);
    hold on;
    fill([distances, fliplr(distances)], [upper_bound, fliplr(lower_bound)], ...
         [0.8, 0.8, 1], 'EdgeColor', 'none', 'FaceAlpha', 0.3);
    plot(distances, upper_bound, 'r--', 'LineWidth', 1);
    plot(distances, lower_bound, 'r--', 'LineWidth', 1);
    
    xlabel('距离 (米)');
    ylabel('RSSI (dBm)');
    title('对数正态阴影模型: RSSI vs 距离');
    legend('平均RSSI', '±2σ范围', 'Location', 'southwest');
    grid on;
    ylim([-100, -30]);
    
    % 子图2: 特定距离处的概率密度函数
    subplot(1,3,2);
    selected_distances = [2, 5, 10, 15];
    colors = ['r', 'g', 'b', 'm'];
    
    hold on;
    for i = 1:length(selected_distances)
        d = selected_distances(i);
        mu = params.P0 - 10 * params.n * log10(d/params.d0);
        
        % 生成PDF - 使用手动实现的normpdf
        rssi_range = mu - 3*params.sigma : 0.1 : mu + 3*params.sigma;
        pdf_values = my_normpdf(rssi_range, mu, params.sigma);
        
        plot(rssi_range, pdf_values, [colors(i) '-'], 'LineWidth', 2, ...
             'DisplayName', sprintf('d=%.0fm', d));
    end
    xlabel('RSSI (dBm)');
    ylabel('概率密度');
    title('不同距离处的RSSI分布');
    legend('show');
    grid on;
    
    % 子图3: 累积分布函数
    subplot(1,3,3);
    hold on;
    for i = 1:length(selected_distances)
        d = selected_distances(i);
        mu = params.P0 - 10 * params.n * log10(d/params.d0);
        
        rssi_range = mu - 3*params.sigma : 0.1 : mu + 3*params.sigma;
        cdf_values = my_normcdf(rssi_range, mu, params.sigma);
        
        plot(rssi_range, cdf_values, [colors(i) '-'], 'LineWidth', 2, ...
             'DisplayName', sprintf('d=%.0fm', d));
    end
    xlabel('RSSI (dBm)');
    ylabel('累积概率');
    title('不同距离处的CDF');
    legend('show');
    grid on;
end

function pdf_values = my_normpdf(x, mu, sigma)
    % 手动实现正态分布概率密度函数
    % 公式: f(x) = (1/(σ√(2π))) * exp(-(x-μ)²/(2σ²))
    
    coefficient = 1 / (sigma * sqrt(2 * pi));
    exponent = -((x - mu).^2) / (2 * sigma^2);
    pdf_values = coefficient * exp(exponent);
end

function cdf_values = my_normcdf(x, mu, sigma)
    % 手动实现正态分布累积分布函数
    % 使用误差函数erf计算
    
    z = (x - mu) / (sigma * sqrt(2));
    cdf_values = 0.5 * (1 + erf(z));
end

function parameter_estimation_experiment(params)
    % 参数估计实验
    
    fprintf('\n=== 参数估计实验 ===\n');
    
    % 生成模拟测量数据
    measured_distances = [1, 2, 3, 5, 7, 10, 12, 15, 18, 20];
    num_measurements_per_distance = 50;
    
    % 预分配内存
    total_measurements = length(measured_distances) * num_measurements_per_distance;
    measured_rssi = zeros(total_measurements, 1);
    true_distances = zeros(total_measurements, 1);
    
    current_index = 1;
    for i = 1:length(measured_distances)
        d = measured_distances(i);
        mean_rssi = params.P0 - 10 * params.n * log10(d/params.d0);
        
        % 计算当前块的索引范围
        start_idx = current_index;
        end_idx = current_index + num_measurements_per_distance - 1;
        
        % 生成RSSI数据 - 使用手动实现的随机数生成
        rssi_at_d = mean_rssi + params.sigma * randn(num_measurements_per_distance, 1);
        
        % 填充预分配的数组
        measured_rssi(start_idx:end_idx) = rssi_at_d;
        true_distances(start_idx:end_idx) = d;
        
        current_index = end_idx + 1;
    end
    
    % 参数估计
    estimated_params = estimate_lognormal_parameters(true_distances, measured_rssi, params.d0);
    
    fprintf('参数估计结果:\n');
    fprintf('真实值: P0=%.1f, n=%.2f, σ=%.2f\n', params.P0, params.n, params.sigma);
    fprintf('估计值: P0=%.1f, n=%.2f, σ=%.2f\n', ...
            estimated_params.P0, estimated_params.n, estimated_params.sigma);
    fprintf('拟合优度 R² = %.4f\n', estimated_params.R2);
    
    % 可视化拟合效果
    figure;
    
    % 散点图
    scatter(true_distances, measured_rssi, 20, 'filled', 'MarkerFaceAlpha', 0.3);
    hold on;
    
    % 理论曲线
    dist_range = 0.5:0.1:25;
    theoretical_curve = params.P0 - 10 * params.n * log10(dist_range/params.d0);
    plot(dist_range, theoretical_curve, 'r-', 'LineWidth', 3, ...
         'DisplayName', '真实模型');
    
    % 估计曲线
    estimated_curve = estimated_params.P0 - 10 * estimated_params.n * log10(dist_range/params.d0);
    plot(dist_range, estimated_curve, 'b--', 'LineWidth', 2, ...
         'DisplayName', '估计模型');
    
    xlabel('距离 (米)');
    ylabel('RSSI (dBm)');
    title('对数正态模型参数估计');
    legend('show', 'Location', 'southwest');
    grid on;
    ylim([-90, -30]);
end

function estimated_params = estimate_lognormal_parameters(distances, rssi_values, d0)
    % 估计对数正态模型的参数
    
    % 转换为对数距离
    log_distances = log10(distances/d0);
    
    % 线性回归估计 P0 和 n
    X = [ones(size(log_distances)), log_distances];
    coefficients = X \ rssi_values;
    
    estimated_params.P0 = coefficients(1);
    estimated_params.n = -coefficients(2) / 10;
    
    % 估计标准差
    predicted_rssi = coefficients(1) - 10 * estimated_params.n * log_distances;
    residuals = rssi_values - predicted_rssi;
    estimated_params.sigma = std(residuals);
    
    % 计算置信区间 - 使用基础MATLAB函数替代regress
    [~, ~, ~, ~, stats] = my_regress(rssi_values, X);
    estimated_params.R2 = stats(1);
    estimated_params.p_value = stats(3);
end

function [b, bint, r, rint, stats] = my_regress(y, X)
    % 简化版的线性回归实现，替代Statistics Toolbox中的regress函数
    
    % 计算系数
    b = X \ y;
    
    % 预测值
    y_hat = X * b;
    
    % 残差
    r = y - y_hat;
    
    % 残差区间 (简化处理)
    n = length(y);
    p = size(X, 2);
    %% 227 行附近改成
XtX = X' * X;
if rcond(XtX) < 1e-12          % 数值奇异
    XtX_inv = pinv(XtX);
else
    XtX_inv = XtX \ eye(size(XtX));
end
h = diag(X * XtX_inv * X');
    s2 = sum(r.^2) / (n - p); % 均方误差
    se = sqrt(s2 * (1 - h)); % 标准误差
    
    % 95%置信区间
    t_val = tinv(0.975, n - p);
    rint = [r - t_val * se, r + t_val * se];
    
    % 系数置信区间
    covb = s2 * inv(X' * X);
    seb = sqrt(diag(covb));
    bint = [b - t_val * seb, b + t_val * seb];
    
    % 统计量
    SST = sum((y - mean(y)).^2);
    SSE = sum(r.^2);
    R2 = 1 - SSE / SST;
    F = (SST - SSE) / (p - 1) / (SSE / (n - p));
    p_value = 1 - fcdf(F, p - 1, n - p);
    
    stats = [R2, F, p_value, s2];
end

function positioning_performance_evaluation(params)
    % 定位性能评估
    
    fprintf('\n=== 定位性能评估 ===\n');
    
    % 定义测试场景
    beacon_positions = [
        0, 0;
        20, 0;
        0, 15;
        20, 15;
        10, 7.5
    ];
    
    test_positions = [
        5, 3;
        12, 8;
        8, 12;
        15, 4
    ];
    
    num_trials = 100;
    num_test_points = length(test_positions);
    positioning_errors = zeros(num_test_points, num_trials);
    
    figure('Position', [100, 100, 1000, 600]);
    
    for test_idx = 1:num_test_points
        true_position = test_positions(test_idx, :);
        
        for trial = 1:num_trials
            % 生成带噪声的RSSI测量
            num_beacons = size(beacon_positions, 1);
            rssi_measurements = zeros(num_beacons, 1);
            
            for i = 1:num_beacons
                distance = norm(true_position - beacon_positions(i, :));
                mean_rssi = params.P0 - 10 * params.n * log10(max(distance, params.d0)/params.d0);
                rssi_measurements(i) = mean_rssi + params.sigma * randn();
            end
            
            % 多边定位
            estimated_position = multilateration_with_lognormal(beacon_positions, rssi_measurements, params);
            positioning_errors(test_idx, trial) = norm(estimated_position - true_position);
        end
        
        % 绘制误差分布 - 使用手动实现的PDF
        subplot(2, 2, test_idx);
        histogram(positioning_errors(test_idx, :), 20, 'Normalization', 'pdf');
        hold on;
        
        % 拟合正态分布
        mu_error = mean(positioning_errors(test_idx, :));
        sigma_error = std(positioning_errors(test_idx, :));
        x_range = 0:0.1:max(positioning_errors(test_idx, :)) * 1.2;
        pdf_fit = my_normpdf(x_range, mu_error, sigma_error);
        plot(x_range, pdf_fit, 'r-', 'LineWidth', 2);
        
        title(sprintf('测试点 %d: μ=%.2fm, σ=%.2fm', test_idx, mu_error, sigma_error));
        xlabel('定位误差 (米)');
        ylabel('概率密度');
        grid on;
    end
    
    % 总体性能统计
    all_errors = positioning_errors(:);
    fprintf('定位性能统计:\n');
    fprintf('平均误差: %.3f 米\n', mean(all_errors));
    fprintf('误差标准差: %.3f 米\n', std(all_errors));
    fprintf('中位数误差: %.3f 米\n', median(all_errors));
    fprintf('75%%误差: %.3f 米\n', prctile(all_errors, 75));
    fprintf('95%%误差: %.3f 米\n', prctile(all_errors, 95));
    
    % 可视化信标和测试点布局
    figure;
    plot(beacon_positions(:,1), beacon_positions(:,2), 'ro', 'MarkerSize', 10, ...
         'MarkerFaceColor', 'red', 'DisplayName', '蓝牙信标');
    hold on;
    
    for i = 1:num_test_points
        plot(test_positions(i,1), test_positions(i,2), 'b*', 'MarkerSize', 12, ...
             'LineWidth', 2, 'DisplayName', sprintf('测试点%d', i));
    end
    
    xlabel('X坐标 (米)');
    ylabel('Y坐标 (米)');
    title('测试场景布局');
    legend('show', 'Location', 'best');
    grid on;
    axis equal;
end

function estimated_pos = multilateration_with_lognormal(beacon_pos, rssi_values, params)
    % 基于对数正态模型的多边定位
    
    num_beacons = size(beacon_pos, 1);
    
    % 预分配距离估计数组
    estimated_distances = zeros(num_beacons, 1);
    
    % 将RSSI转换为距离估计（考虑阴影衰落）
    for i = 1:num_beacons
        % 使用最大似然估计距离
        estimated_distances(i) = rssi_to_distance_ml(rssi_values(i), params);
    end
    
    % 多边定位（使用加权最小二乘）
    if num_beacons < 3
        error('至少需要3个信标进行定位');
    end
    
    % 预分配矩阵和向量
    num_equations = num_beacons - 1;
    A = zeros(num_equations, 2);
    B = zeros(num_equations, 1);
    weights = zeros(num_equations, 1);
    
    % 构建线性方程组
    for i = 1:num_equations
        beacon_idx = i + 1; % 从第二个信标开始
        
        % 构建系数矩阵A
        A(i, 1) = -2 * (beacon_pos(beacon_idx, 1) - beacon_pos(1, 1));
        A(i, 2) = -2 * (beacon_pos(beacon_idx, 2) - beacon_pos(1, 2));
        
        % 构建常数项B
        B(i) = estimated_distances(beacon_idx)^2 - estimated_distances(1)^2 ...
              - beacon_pos(beacon_idx, 1)^2 + beacon_pos(1, 1)^2 ...
              - beacon_pos(beacon_idx, 2)^2 + beacon_pos(1, 2)^2;
        
        % 基于距离不确定性设置权重（距离越远，不确定性越大）
        weights(i) = 1 / (estimated_distances(beacon_idx)^2 + eps);
    end
    
    % 加权最小二乘解
    W = diag(weights);
    estimated_pos = (A' * W * A) \ (A' * W * B);
end

function distance = rssi_to_distance_ml(rssi, params)
    % 基于最大似然的RSSI到距离转换
    
    % 对于对数正态模型，距离的ML估计为：
    % d = d0 * 10^((P0 - RSSI) / (10 * n))
    
    distance = params.d0 * 10^((params.P0 - rssi) / (10 * params.n));
    
    % 限制距离范围
    distance = max(distance, 0.1);
    distance = min(distance, 50);
end