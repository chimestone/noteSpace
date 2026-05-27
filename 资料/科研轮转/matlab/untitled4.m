%% 手机 IMU 离线姿态估计（直接加载 .mat 自动转 timetable 版）
clc; clear; close all

%% 1. 选文件
[matfile,pathn] = uigetfile("D:\HuaweiMoveData\Users\钟磬\Documents\MATLAB\Examples\R2025b\shared_positioning\EstimateIPhoneOrientationUsingSensorFusionExample\MobileSensorData_20251012_204408.mat");
if isequal(matfile,0), return, end
matfile = fullfile(pathn,matfile);
targetRate = 100;                       % 目标统一采样率 (Hz)

%% 2. 读原始变量（自动匹配字段名）
fn = fieldnames(raw);
acc   = raw.(fn(contains(fn,'ccel')));
gyro  = raw.(fn(contains(fn,'yro')));
mag   = raw.(fn(contains(fn,'ag')));
orient= raw.(fn(contains(fn,'rient')));
tAcc  = raw.(fn(contains(fn,'t_acc')));
tGyro = raw.(fn(contains(fn,'t_gyro')));
tMag  = raw.(fn(contains(fn,'t_mag')));
tOri  = raw.(fn(contains(fn,'t_ori')));

%% 3. 自动转 timetable（统一用 duration 做时间轴）
if isnumeric(tAcc),  tAcc  = seconds(tAcc);  end
if isnumeric(tGyro), tGyro = seconds(tGyro); end
if isnumeric(tMag),  tMag  = seconds(tMag);  end
if isnumeric(tOri),  tOri  = seconds(tOri);  end

ttAcc  = timetable(tAcc ,acc , 'VariableNames',{'acc'});
ttGyro = timetable(tGyro,gyro, 'VariableNames',{'gyro'});
ttMag  = timetable(tMag ,mag , 'VariableNames',{'mag'});
ttOrient=timetable(tOri ,orient,'VariableNames',{'orient'});

%% 4. 时间对齐 & 重采样
tMin = max([ttAcc.Properties.RowTimes(1);
            ttGyro.Properties.RowTimes(1);
            ttMag.Properties.RowTimes(1);
            ttOrient.Properties.RowTimes(1)]);
tMax = min([ttAcc.Properties.RowTimes(end);
            ttGyro.Properties.RowTimes(end);
            ttMag.Properties.RowTimes(end);
            ttOrient.Properties.RowTimes(end)]);
tVec = (tMin : seconds(1/targetRate) : tMax).';
ttAcc  = retime(ttAcc ,tVec,'linear');
ttGyro = retime(ttGyro,tVec,'linear');
ttMag  = retime(ttMag ,tVec,'linear');
ttOrient=retime(ttOrient,tVec,'linear');
ttAcc  = retime(ttAcc ,tVec,'linear');
ttGyro = retime(ttGyro,tVec,'linear');
ttMag  = retime(ttMag ,tVec,'linear');
ttOrient=retime(ttOrient,tVec,'linear');

%% 5. 抽出 double
acc   = ttAcc.acc;
gyro  = ttGyro.gyro;
mag   = ttMag.mag;
orient= ttOrient.orient;

%% 6. 坐标系对齐（与原 demo 一致）
Accelerometer = -[acc(:,2), acc(:,1), -acc(:,3)];
Gyroscope     = [gyro(:,2), gyro(:,1), -gyro(:,3)];
Magnetometer  = [mag(:,2),  mag(:,1), -mag(:,3)];

%% 7. 真值 → 四元数
if size(orient,2)==3
    qTrue = quaternion([orient(:,3), -orient(:,2), orient(:,1)], ...
                       'eulerd', 'ZYX', 'frame');
else
    qTrue = quaternion(orient,'wxyz');
end

%% 8. 安装角修正
Navg = 4;
q = ecompass(Accelerometer, Magnetometer);
qfix = meanrot(q(1:Navg)) ./ meanrot(qTrue(1:Navg));
Orientation = qfix * qTrue;

%% 9. 滤波器自动调参
orientFilt = ahrsfilter('SampleRate', targetRate);
groundTruth = table(Orientation);
sensorData  = table(Accelerometer, Gyroscope, Magnetometer);
tc = tunerconfig('ahrsfilter','MaxIterations',30, ...
                 'ObjectiveLimit',1e-3,'Display','none');
tune(orientFilt, sensorData, groundTruth, tc);
reset(orientFilt);

%% 10. 正式滤波
qEst = orientFilt(Accelerometer, Gyroscope, Magnetometer);

%% 11. 误差 & 画图（完全保留原脚本）
numSamples = numel(Orientation);
t = (0:numSamples-1).' / targetRate;
d = rad2deg(dist(qEst, Orientation));

% 11.1 滤波欧拉角
figure
plot(t, eulerd(qEst, 'ZYX', 'frame'))
legend('yaw','pitch','roll','Location','bestoutside')
title('ahrsfilter Euler Angles')
ylabel('Degrees'); xlabel('Time (s)')

% 11.2 手机原始欧拉角
figure
plot(t, eulerd(Orientation, 'ZYX', 'frame'))
legend('yaw','pitch','roll','Location','bestoutside')
title('Phone Euler Angles (ground truth)')
ylabel('Degrees'); xlabel('Time (s)')

% 11.3 误差
figure
plot(t, d); hold on
rmsval = sqrt(mean(d.^2));
plot(t, repmat(rmsval,size(t)),'LineStyle','-.','Color','red');
text(t(1), rmsval+0.7, sprintf('RMS Error = %.2f°',rmsval),'Color','red')
title('Orientation Error'); ylabel('Degrees'); xlabel('Time (s)')

%% 12. 3D 动画（可选）
if exist('phoneMesh.stl','file')
    figure
    pp = poseplot("MeshFileName","phoneMesh.stl");
    for i = 1:numel(qEst)
        set(pp,"Orientation",qEst(i));
        drawnow limitrate
    end
end