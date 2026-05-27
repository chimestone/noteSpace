%% 优化版手机端实时采集 + 离线级姿态估计（一体化脚本）
clc; clear; close all

%% 主函数
function main()
    try
        % 参数设置
        recordTime = 10;
        targetRate = 100;
        
        % 1. 手机连接与数据采集
        m = connectMobile(30);
        [sensorData, timeData] = collectSensorData(m, recordTime, true);
        
        % 2. 数据保存
        saveData(sensorData, timeData);
        
        % 3. 数据处理与姿态估计
        results = processIMUData(sensorData, timeData, targetRate);
        
        % 4. 结果可视化（优化前后对比）
        visualizeResults(results.qEst, results.Orientation, targetRate, results.Orientation_raw);
        
        disp('✅ 全部完成！');
        
    catch ME
        fprintf('❌ 错误: %s\n', ME.message);
        rethrow(ME);
    end
end

%% 1. 手机连接函数
function m = connectMobile(timeout)
    if nargin < 1, timeout = 30; end
    
    disp('👉 正在创建 mobiledev 对象...');
    m = mobiledev;
    
    % 等待连接
    tic;
    while ~m.Connected && toc < timeout
        pause(1);
        fprintf('.');
    end
    fprintf('\n');
    
    if ~m.Connected
        error('连接超时！请检查网络设置');
    else
        disp(['✅ 手机已连接：' m.Device]);
    end
    
    % 批量启用传感器
    sensors = {'AccelerationSensorEnabled', 'AngularVelocitySensorEnabled', ...
               'MagneticSensorEnabled', 'OrientationSensorEnabled'};
    sensorNames = {'加速度计', '陀螺仪', '磁场', '方向'};
    
    for i = 1:length(sensors)
        try
            m.(sensors{i}) = true;
            if m.(sensors{i})
                fprintf('✅ %s已启用\n', sensorNames{i});
            else
                warning('⚠️ %s启用失败', sensorNames{i});
            end
        catch ME
            warning('传感器启用异常: %s', ME.message);
        end
    end
end

%% 2. 数据采集函数
function [sensorData, timeData] = collectSensorData(m, recordTime, showProgress)
    if nargin < 3, showProgress = true; end
    
    discardlogs(m); % 清除数据
    m.Logging = 1;
    
    if showProgress
        fprintf('🎬 开始记录 %d 秒，请移动手机...\n', recordTime);
        for i = 1:recordTime
            fprintf('进度: %d/%d 秒\r', i, recordTime);
            pause(1);
        end
        fprintf('\n');
    else
        pause(recordTime);
    end
    
    m.Logging = 0;
    disp('🛑 停止记录');
    
    % 批量读取数据
    try
        [sensorData.Acceleration, timeData.t_acc] = accellog(m);
        [sensorData.AngularVelocity, timeData.t_gyro] = angvellog(m);
        [sensorData.Orientation, timeData.t_ori] = orientlog(m);
        [sensorData.MagneticField, timeData.t_mag] = magfieldlog(m);
    catch ME
        error('数据读取失败: %s', ME.message);
    end
end

%% 3. 数据保存函数
function saveData(sensorData, timeData, customName)
    if nargin < 3
        timestamp = datestr(now,'yyyymmdd_HHMMSS');
        filename = sprintf('MobileSensorData_%s.mat', timestamp);
    else
        filename = [customName '.mat'];
    end
    
    % 添加元数据
    metadata.timestamp = now;
    metadata.matlabVersion = version;
    
    save(filename, 'sensorData', 'timeData', 'metadata', '-v7.3');
    fileInfo = dir(filename);
    fprintf('💾 数据已保存为: %s (%.2f MB)\n', filename, fileInfo.bytes/1024/1024);
end

%% 4. 时间格式转换函数
function timeData = convertTimeFormat(timeData)
    timeFields = fieldnames(timeData);
    for i = 1:length(timeFields)
        field = timeFields{i};
        if isnumeric(timeData.(field))
            timeData.(field) = seconds(timeData.(field));
        end
    end
end

%% 5. Timetable创建函数
function timetables = createTimetables(sensorData, timeData)
    sensors = {'acc', 'gyro', 'mag', 'orient'};
    dataNames = {'Acceleration', 'AngularVelocity', 'MagneticField', 'Orientation'};
    timeNames = {'t_acc', 't_gyro', 't_mag', 't_ori'};
    
    timetables = struct();
    for i = 1:length(sensors)
        timetables.(sensors{i}) = timetable(timeData.(timeNames{i}), ...
                                          sensorData.(dataNames{i}), ...
                                          'VariableNames', {sensors{i}});
    end
end

%% 6. 时间对齐与重采样函数
function [timetables, tVec] = alignAndResample(timetables, targetRate)
    ttNames = fieldnames(timetables);
    
    % 计算公共时间窗口
    startTimes = cellfun(@(name) timetables.(name).Properties.RowTimes(1), ttNames);
    endTimes = cellfun(@(name) timetables.(name).Properties.RowTimes(end), ttNames);
    
    tMin = max(startTimes);
    tMax = min(endTimes);
    tVec = transpose(tMin : seconds(1/targetRate) : tMax);
    
    fprintf('时间对齐: %.2f秒 → %.2f秒 (共%.1f秒)\n', ...
            seconds(tMin), seconds(tMax), seconds(tMax-tMin));
    
    % 批量重采样
    for i = 1:length(ttNames)
        timetables.(ttNames{i}) = retime(timetables.(ttNames{i}), tVec, 'linear');
    end
end

%% 7. 坐标系对齐函数
function [Accelerometer, Gyroscope, Magnetometer] = alignCoordinates(acc, gyro, mag)
    % 检查重力方向来验证坐标系
    meanAcc = mean(acc, 1);
    [~, gravityAxis] = max(abs(meanAcc));
    
    % 根据重力方向调整坐标变换
    if gravityAxis == 3 && meanAcc(3) < -8  % Z轴向下
        % 标准变换
        Accelerometer = [acc(:,1), acc(:,2), acc(:,3)];
        Gyroscope = [gyro(:,1), gyro(:,2), gyro(:,3)];
        Magnetometer = [mag(:,1), mag(:,2), mag(:,3)];
    else
        % 手机坐标系 → 标准IMU坐标系变换（原方案）
        Accelerometer = -[acc(:,2), acc(:,1), -acc(:,3)];
        Gyroscope = [gyro(:,2), gyro(:,1), -gyro(:,3)];
        Magnetometer = [mag(:,2), mag(:,1), -mag(:,3)];
    end
    
    % 验证变换结果
    fprintf('坐标变换完成 - 数据范围检查:\n');
    fprintf('加速度: [%.2f, %.2f] m/s² (重力轴: %d)\n', ...
            min(Accelerometer(:)), max(Accelerometer(:)), gravityAxis);
    fprintf('角速度: [%.2f, %.2f] rad/s\n', min(Gyroscope(:)), max(Gyroscope(:)));
    fprintf('磁场: [%.2f, %.2f] μT\n', min(Magnetometer(:)), max(Magnetometer(:)));
end

%% 8. 四元数转换函数
function qTrue = convertToQuaternion(orient)
    % 输入验证
    if isempty(orient) || any(isnan(orient(:)))
        error('方向数据包含NaN或为空');
    end
    
    switch size(orient, 2)
        case 3  % 欧拉角格式
            qTrue = quaternion([orient(:,3), -orient(:,2), orient(:,1)], ...
                              'eulerd', 'ZYX', 'frame');
        case 4  % 四元数格式
            qTrue = quaternion(orient, 'wxyz');
        otherwise
            error('不支持的方向数据格式，需要3列(欧拉角)或4列(四元数)');
    end
    
    % 归一化检查
    norms = norm(qTrue);
    if any(abs(norms - 1) > 0.1)
        warning('四元数归一化异常，自动修正');
        qTrue = normalize(qTrue);
    end
end

%% 9. 安装角校正函数
function [Orientation, qfix] = correctInstallationAngle(Accelerometer, Magnetometer, qTrue)
    Navg = min(20, floor(length(qTrue) * 0.1));  % 增加样本数提高稳定性
    
    % 检查静止段
    accVar = var(Accelerometer(1:min(50, end), :));
    if max(accVar) > 1.0
        warning('初始段运动较大，校正可能不准确');
        % 如果运动太大，减少校正强度
        Navg = max(5, floor(Navg/2));
    end
    
    % 使用ecompass计算参考姿态
    q_ref = ecompass(Accelerometer, Magnetometer);
    
    % 计算安装角校正四元数
    qfix = meanrot(q_ref(1:Navg)) ./ meanrot(qTrue(1:Navg));
    
    % 应用校正
    Orientation = qfix * qTrue;
    
    % 校正效果评估（修正逻辑）
    initialError = rad2deg(dist(q_ref(1:Navg), qTrue(1:Navg)));
    correctedError = rad2deg(dist(q_ref(1:Navg), Orientation(1:Navg)));
    
    fprintf('安装角校正: 初始误差 %.2f° → 校正后 %.2f°\n', ...
            mean(initialError), mean(correctedError));
    
    % 如果校正后误差更大，则不应用校正
    if mean(correctedError) > mean(initialError)
        warning('校正后误差增大，使用原始数据');
        Orientation = qTrue;
        qfix = quaternion([1 0 0 0]);
    end
end

%% 10. AHRS滤波器设置函数
function orientFilt = setupAHRSFilter(Accelerometer, Gyroscope, Magnetometer, Orientation, targetRate)
    orientFilt = ahrsfilter('SampleRate', targetRate);
    
    % 设置保守的初始参数，避免过拟合
    orientFilt.AccelerometerNoise = 0.1;
    orientFilt.GyroscopeNoise = 1e-3;
    orientFilt.MagnetometerNoise = 0.1;
    orientFilt.GyroscopeDriftNoise = 1e-6;
    orientFilt.LinearAccelerationNoise = 0.01;
    orientFilt.MagneticDisturbanceNoise = 1e-3;
    
    % 可选：进行参数优化（但使用更保守的设置）
    doTuning = false;  % 设为true启用调参
    
    if doTuning
        fprintf('正在优化AHRS滤波器参数...\n');
        
        % 只使用部分数据进行调参，避免过拟合
        tuneLength = min(500, floor(length(Orientation) * 0.3));
        groundTruth = table(Orientation(1:tuneLength));
        sensorData = table(Accelerometer(1:tuneLength,:), ...
                          Gyroscope(1:tuneLength,:), ...
                          Magnetometer(1:tuneLength,:));
        
        tc = tunerconfig('ahrsfilter', 'MaxIterations', 15, ...
                         'ObjectiveLimit', 5e-3, 'Display', 'none');
        
        tic;
        tune(orientFilt, sensorData, groundTruth, tc);
        tuneTime = toc;
        
        fprintf('参数优化完成，耗时 %.1f 秒\n', tuneTime);
    else
        fprintf('使用默认AHRS参数（推荐）\n');
    end
    
    reset(orientFilt);
end

%% 11. 诊断函数
function diagnoseOptimization(qEst, Orientation, Orientation_raw)
    if isempty(Orientation_raw)
        fprintf('⚠️ 缺少优化前数据，无法进行对比\n');
        return;
    end
    
    % 计算各种误差指标
    error_raw = rad2deg(dist(qEst, Orientation_raw));
    error_opt = rad2deg(dist(qEst, Orientation));
    
    fprintf('\n=== 优化诊断报告 ===\n');
    fprintf('优化前误差: RMS=%.3f°, 最大=%.3f°, 中位数=%.3f°\n', ...
            sqrt(mean(error_raw.^2)), max(error_raw), median(error_raw));
    fprintf('优化后误差: RMS=%.3f°, 最大=%.3f°, 中位数=%.3f°\n', ...
            sqrt(mean(error_opt.^2)), max(error_opt), median(error_opt));
    
    % 分析问题
    if mean(error_opt) > mean(error_raw)
        fprintf('❌ 问题: 优化后误差增大了 %.1f%%\n', ...
                (mean(error_opt) - mean(error_raw))/mean(error_raw)*100);
        
        % 可能原因分析
        fprintf('\n可能原因:\n');
        if std(error_opt) > std(error_raw) * 1.5
            fprintf('- 安装角校正引入了额外噪声\n');
        end
        if max(error_opt) > max(error_raw) * 1.2
            fprintf('- 校正算法在某些姿态下不稳定\n');
        end
        
        fprintf('\n建议:\n');
        fprintf('- 检查初始静止段是否真的静止\n');
        fprintf('- 尝试关闭安装角校正\n');
        fprintf('- 使用更保守的AHRS参数\n');
    else
        fprintf('✅ 优化成功: 误差减少了 %.1f%%\n', ...
                (mean(error_raw) - mean(error_opt))/mean(error_raw)*100);
    end
end

%% 12. 主数据处理函数
function results = processIMUData(sensorData, timeData, targetRate)
    % 时间格式转换
    timeData = convertTimeFormat(timeData);
    
    % 创建timetable
    timetables = createTimetables(sensorData, timeData);
    
    % 时间对齐与重采样
    [timetables, ~] = alignAndResample(timetables, targetRate);
    
    % 提取数据
    acc = timetables.acc.acc;
    gyro = timetables.gyro.gyro;
    mag = timetables.mag.mag;
    orient = timetables.orient.orient;
    
    % 坐标系对齐
    [Accelerometer, Gyroscope, Magnetometer] = alignCoordinates(acc, gyro, mag);
    
    % 四元数转换（优化前）
    qTrue_raw = convertToQuaternion(orient);
    
    % 安装角校正（优化后）
    [Orientation, ~] = correctInstallationAngle(Accelerometer, Magnetometer, qTrue_raw);
    
    % AHRS滤波
    orientFilt = setupAHRSFilter(Accelerometer, Gyroscope, Magnetometer, Orientation, targetRate);
    qEst = orientFilt(Accelerometer, Gyroscope, Magnetometer);
    
    % 诊断优化效果
    diagnoseOptimization(qEst, Orientation, qTrue_raw);
    
    % 返回结果（包含优化前后数据）
    results.qEst = qEst;
    results.Orientation = Orientation;  % 优化后
    results.Orientation_raw = qTrue_raw;  % 优化前
    results.Accelerometer = Accelerometer;
    results.Gyroscope = Gyroscope;
    results.Magnetometer = Magnetometer;
end

%% 13. 结果可视化函数（优化前后对比）
function visualizeResults(qEst, Orientation, targetRate, Orientation_raw)
    if nargin < 4
        Orientation_raw = [];
    end
    
    numSamples = numel(Orientation);
    t = transpose(0:numSamples-1) / targetRate;
    
    % 计算误差
    d_optimized = rad2deg(dist(qEst, Orientation));
    if ~isempty(Orientation_raw)
        d_raw = rad2deg(dist(qEst, Orientation_raw));
    end
    
    % 创建主对比图表
    figure('Position', [50 50 1600 1000], 'Name', '优化前后对比分析');
    
    % 子图1: 欧拉角对比
    subplot(3,3,1);
    euler_est = eulerd(qEst, 'ZYX', 'frame');
    euler_opt = eulerd(Orientation, 'ZYX', 'frame');
    plot(t, euler_est(:,1), 'b-', 'LineWidth', 1.5); hold on;
    plot(t, euler_opt(:,1), 'r--', 'LineWidth', 1.5);
    if ~isempty(Orientation_raw)
        euler_raw = eulerd(Orientation_raw, 'ZYX', 'frame');
        plot(t, euler_raw(:,1), 'g:', 'LineWidth', 1.2);
        legend('AHRS估计', '优化后真值', '优化前真值');
    else
        legend('AHRS估计', '真值');
    end
    title('Yaw角对比'); ylabel('角度 (°)'); xlabel('时间 (s)'); grid on;
    
    % 子图2: Pitch角对比
    subplot(3,3,2);
    plot(t, euler_est(:,2), 'b-', 'LineWidth', 1.5); hold on;
    plot(t, euler_opt(:,2), 'r--', 'LineWidth', 1.5);
    if ~isempty(Orientation_raw)
        plot(t, euler_raw(:,2), 'g:', 'LineWidth', 1.2);
    end
    title('Pitch角对比'); ylabel('角度 (°)'); xlabel('时间 (s)'); grid on;
    
    % 子图3: Roll角对比
    subplot(3,3,3);
    plot(t, euler_est(:,3), 'b-', 'LineWidth', 1.5); hold on;
    plot(t, euler_opt(:,3), 'r--', 'LineWidth', 1.5);
    if ~isempty(Orientation_raw)
        plot(t, euler_raw(:,3), 'g:', 'LineWidth', 1.2);
    end
    title('Roll角对比'); ylabel('角度 (°)'); xlabel('时间 (s)'); grid on;
    
    % 子图4: 误差对比
    subplot(3,3,4);
    plot(t, d_optimized, 'r-', 'LineWidth', 1.5); hold on;
    if ~isempty(Orientation_raw)
        plot(t, d_raw, 'g--', 'LineWidth', 1.5);
        legend('优化后误差', '优化前误差');
    end
    title('误差对比'); ylabel('误差 (°)'); xlabel('时间 (s)'); grid on;
    
    % 子图5: 误差统计对比
    subplot(3,3,5);
    if ~isempty(Orientation_raw)
        histogram(d_optimized, 20, 'Normalization', 'probability', 'FaceAlpha', 0.7, 'FaceColor', 'r'); hold on;
        histogram(d_raw, 20, 'Normalization', 'probability', 'FaceAlpha', 0.7, 'FaceColor', 'g');
        legend('优化后', '优化前');
    else
        histogram(d_optimized, 30, 'Normalization', 'probability');
    end
    title('误差分布对比'); xlabel('误差 (°)'); ylabel('概率'); grid on;
    
    % 子图6: 3D轨迹对比
    subplot(3,3,6);
    plot3(euler_est(:,1), euler_est(:,2), euler_est(:,3), 'b-', 'LineWidth', 1.5); hold on;
    plot3(euler_opt(:,1), euler_opt(:,2), euler_opt(:,3), 'r--', 'LineWidth', 1.5);
    if ~isempty(Orientation_raw)
        plot3(euler_raw(:,1), euler_raw(:,2), euler_raw(:,3), 'g:', 'LineWidth', 1.2);
        legend('AHRS估计', '优化后', '优化前');
    else
        legend('AHRS估计', '真值');
    end
    xlabel('Yaw (°)'); ylabel('Pitch (°)'); zlabel('Roll (°)');
    title('3D姿态轨迹对比'); grid on;
    
    % 子图7: 性能指标对比
    subplot(3,3,7);
    rms_opt = sqrt(mean(d_optimized.^2));
    max_opt = max(d_optimized);
    std_opt = std(d_optimized);
    mean_opt = mean(abs(d_optimized));
    
    if ~isempty(Orientation_raw)
        rms_raw = sqrt(mean(d_raw.^2));
        max_raw = max(d_raw);
        std_raw = std(d_raw);
        mean_raw = mean(abs(d_raw));
        
        metrics = [rms_opt rms_raw; max_opt max_raw; std_opt std_raw; mean_opt mean_raw];
        bar(metrics);
        legend('优化后', '优化前');
        set(gca, 'XTickLabel', {'RMS', '最大值', '标准差', '平均值'});
    else
        metrics = [rms_opt; max_opt; std_opt; mean_opt];
        bar(metrics);
        set(gca, 'XTickLabel', {'RMS', '最大值', '标准差', '平均值'});
    end
    title('性能指标对比'); ylabel('误差 (°)'); grid on;
    
    % 子图8: 改善程度
    if ~isempty(Orientation_raw)
        subplot(3,3,8);
        improvement = [(rms_raw-rms_opt)/rms_raw*100; (max_raw-max_opt)/max_raw*100; ...
                      (std_raw-std_opt)/std_raw*100; (mean_raw-mean_opt)/mean_raw*100];
        bar(improvement, 'FaceColor', [0.2 0.8 0.2]);
        set(gca, 'XTickLabel', {'RMS', '最大值', '标准差', '平均值'});
        title('性能改善程度'); ylabel('改善百分比 (%)'); grid on;
        
        % 添加数值标签
        for i = 1:length(improvement)
            text(i, improvement(i)+0.5, sprintf('%.1f%%', improvement(i)), ...
                 'HorizontalAlignment', 'center', 'FontWeight', 'bold');
        end
    end
    
    % 子图9: AHRS估计的三个欧拉角时间序列
    subplot(3,3,9);
    plot(t, euler_est(:,1), 'r-', 'LineWidth', 1.5); hold on;
    plot(t, euler_est(:,2), 'g-', 'LineWidth', 1.5);
    plot(t, euler_est(:,3), 'b-', 'LineWidth', 1.5);
    legend('Yaw', 'Pitch', 'Roll', 'Location', 'best');
    title('AHRS估计的欧拉角'); ylabel('角度 (°)'); xlabel('时间 (s)'); grid on;
    
    % 输出统计信息
    fprintf('\n=== 性能评估对比 ===\n');
    if ~isempty(Orientation_raw)
        fprintf('优化前: RMS=%.3f°, 最大=%.3f°, 平均=%.3f°\n', rms_raw, max_raw, mean_raw);
        fprintf('优化后: RMS=%.3f°, 最大=%.3f°, 平均=%.3f°\n', rms_opt, max_opt, mean_opt);
        fprintf('改善程度: RMS减少%.1f%%, 最大误差减少%.1f%%\n', ...
                (rms_raw-rms_opt)/rms_raw*100, (max_raw-max_opt)/max_raw*100);
    else
        fprintf('RMS误差: %.3f°, 最大误差: %.3f°\n', rms_opt, max_opt);
    end
    
    % 统计信息表（独立图窗）
    if ~isempty(Orientation_raw)
        figure('Position', [100 100 600 400], 'Name', '性能统计对比');
        axis off;
        info_text = sprintf(['性能对比统计\n\n' ...
                           '优化前:\n' ...
                           'RMS: %.3f°\n' ...
                           '最大: %.3f°\n' ...
                           '平均: %.3f°\n' ...
                           '标准差: %.3f°\n\n' ...
                           '优化后:\n' ...
                           'RMS: %.3f°\n' ...
                           '最大: %.3f°\n' ...
                           '平均: %.3f°\n' ...
                           '标准差: %.3f°\n\n' ...
                           '数据长度: %.1f秒'], ...
                          rms_raw, max_raw, mean_raw, std_raw, ...
                          rms_opt, max_opt, mean_opt, std_opt, t(end));
        text(0.1, 0.9, info_text, 'FontSize', 12, 'VerticalAlignment', 'top', ...
             'FontName', 'FixedWidth', 'BackgroundColor', [0.95 0.95 0.95]);
    end
    
    % 3D动画与视频保存
    if exist('phoneMesh.stl','file')
        % 创建动画窗口
        fig = figure('Name', '3D姿态动画', 'Position', [200 200 800 600]);
        pp = poseplot("MeshFileName","phoneMesh.stl");
        
        % 设置视频参数
        timestamp = datestr(now,'yyyymmdd_HHMMSS');
        videoFile = sprintf('3D_Animation_%s.mp4', timestamp);
        
        % 创建视频写入器
        v = VideoWriter(videoFile, 'MPEG-4');
        v.FrameRate = 20;  % 20 FPS
        v.Quality = 90;
        open(v);
       
        
        % 生成动画帧
        for i = 1:3:numel(qEst)
            set(pp,"Orientation",qEst(i));
            
            % 添加时间标签
            title(sprintf('时间: %.2f秒', t(i)), 'FontSize', 14);
            
            drawnow;
            
            % 捕获帧并写入视频
            frame = getframe(fig);
            writeVideo(v, frame);
            
            % 显示进度
            if mod(i, 30) == 1
                fprintf('进度: %.1f%%\r', i/numel(qEst)*100);
            end
        end
        
        % 关闭视频写入器
        close(v);
        
        % 显示结果
        fileInfo = dir(videoFile);
        fprintf('\n🎥 3D动画视频已保存: %s (%.2f MB)\n', ...
                videoFile, fileInfo.bytes/1024/1024);
        fprintf('📹 视频参数: %d FPS, 质量 %d%%\n', v.FrameRate, v.Quality);
    end
end

%% 运行主函数
main();