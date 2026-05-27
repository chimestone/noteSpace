%% 手机 IMU 离线姿态估计完整脚本（适配 timetable 版）
clc; clear; close all

%% 1. 填写你的文件
matfile = "D:\HuaweiMoveData\Users\钟磬\Documents\MATLAB\Examples\R2025b\shared_positioning\EstimateIPhoneOrientationUsingSensorFusionExample\MobileSensorData_20251012_205728.mat";  % <-- 换自己的
targetRate = 100;                       % 想要统一到的采样率（Hz）

%% 2. 读原始 timetable
raw       = load(matfile);
ttAcc     = raw.Acceleration;
ttGyro    = raw.AngularVelocity;
ttMag     = raw.MagneticField;
ttOrient  = raw.Orientation;            % 欧拉角或四元数 timetable

%% 3. 时间对齐 & 重采样
tMin   = max([ttAcc.Timestamp(1); ttGyro.Timestamp(1); ttMag.Timestamp(1); ttOrient.Timestamp(1)]);
tMax   = min([ttAcc.Timestamp(end); ttGyro.Timestamp(end); ttMag.Timestamp(end); ttOrient.Timestamp(end)]);
tVec   = (tMin : seconds(1/targetRate) : tMax).';
ttAcc  = retime(ttAcc ,tVec,'linear');
ttGyro = retime(ttGyro,tVec,'linear');
ttMag  = retime(ttMag ,tVec,'linear');
ttOrient=retime(ttOrient,tVec,'linear');

%% 4. 抽出 double
acc   = ttAcc.Variables;        % N×3
gyro  = ttGyro.Variables;
mag   = ttMag.Variables;
orient= ttOrient.Variables;     % N×3(°) 或 N×4(四元数)

%% 5. 坐标系对齐（与原 demo 完全一致）
Accelerometer = -[acc(:,2), acc(:,1), -acc(:,3)];
Gyroscope     = [gyro(:,2), gyro(:,1), -gyro(:,3)];
Magnetometer  = [mag(:,2),  mag(:,1), -mag(:,3)];

%% 6. 真值 → 四元数
if size(orient,2) == 3                                    % 欧拉角
    qTrue = quaternion([orient(:,3), -orient(:,2), orient(:,1)], ...
                       'eulerd', 'ZYX', 'frame');
else                                                      % 四元数 wxyz
    qTrue = quaternion(orient,'wxyz');
end

%% 7. 安装角修正（同 demo）
Navg = 4;
q = ecompass(Accelerometer, Magnetometer);
qfix = meanrot(q(1:Navg)) ./ meanrot(qTrue(1:Navg));
Orientation = qfix * qTrue;          % 后面当地面真值用

%% 8. 滤波器自动调参
orientFilt = ahrsfilter('SampleRate', targetRate);
groundTruth = table(Orientation);
sensorData  = table(Accelerometer, Gyroscope, Magnetometer);
tc = tunerconfig('ahrsfilter','MaxIterations',30, ...
                 'ObjectiveLimit',1e-3,'Display','none');
tune(orientFilt, sensorData, groundTruth, tc);
reset(orientFilt);

%% 9. 正式滤波
qEst = orientFilt(Accelerometer, Gyroscope, Magnetometer);

%% 10. 误差计算
numSamples = numel(Orientation);
t = (0:numSamples-1).'/targetRate;
d = rad2deg(dist(qEst, Orientation));   % 角度误差（°）

%% 11. 画图
% 11.1 滤波结果欧拉角
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

% 11.3 误差曲线 + RMS
figure
plot(t, d)
title('Orientation Error')
ylabel('Degrees'); xlabel('Time (s)')
rmsval = sqrt(mean(d.^2));
line(t, repmat(rmsval,size(t)),'LineStyle','-.','Color','red');
text(t(1), rmsval+0.7, sprintf('RMS Error = %.2f°',rmsval),'Color','red')

%% 12. 3D 动画（需要 phoneMesh.stl 同目录）
if exist('phoneMesh.stl','file')
    figure
    pp = poseplot("MeshFileName","phoneMesh.stl");
    for i = 1:numel(qEst)
        set(pp,"Orientation",qEst(i));
        drawnow limitrate
    end
end