%% 手机端实时采集 + 离线级姿态估计（一体化脚本）
clc; clear; close all

%% ========= 1. 手机端采集（代码一功能） =========
disp('👉 正在创建 mobiledev 对象...');
m = mobiledev;

if ~m.Connected
    error(['⚠️ 手机未连接！\n',...
           '请在 MATLAB Mobile → Sensors 页面中点击右上角 ⚙️ 设置\n',...
           '选择 "Connect to MATLAB"，输入此电脑的 IP 地址。']);
else
    disp(['✅ 手机已连接：' m.Device]);
end

% 启用传感器
m.AccelerationSensorEnabled  = true;
m.AngularVelocitySensorEnabled = true;
m.MagneticSensorEnabled      = true;
m.OrientationSensorEnabled   = true;
disp('✅ 加速度计、陀螺仪、磁场、方向传感器已启用');

% 清空旧数据并开始记录
discardlogs(m);
m.Logging = 1;
recordTime = 10;                % 可改
disp('🎬 开始记录，请移动手机...');
pause(recordTime);
m.Logging = 0;
disp('🛑 停止记录');

% 读取数据
[Acceleration, t_acc] = accellog(m);
[AngularVelocity, t_gyro] = angvellog(m);
[Orientation, t_ori] = orientlog(m);
[MagneticField, t_mag] = magfieldlog(m);

% 保存一份副本
filename = ['MobileSensorData_' datestr(now,'yyyymmdd_HHMMSS') '.mat'];
save(filename,'Acceleration','AngularVelocity','Orientation','MagneticField',...
             't_acc','t_gyro','t_ori','t_mag');
disp(['💾 数据已保存为: ' filename]);

%% ========= 2. 直接拿变量做"离线级"姿态估计（代码二功能） =========
targetRate = 100;                       % 目标统一采样率 (Hz)

% 自动转 timetable（统一用 duration 做时间轴）
if isnumeric(t_acc),  t_acc  = seconds(t_acc);  end
if isnumeric(t_gyro), t_gyro = seconds(t_gyro); end
if isnumeric(t_mag),  t_mag  = seconds(t_mag);  end
if isnumeric(t_ori),  t_ori  = seconds(t_ori);  end

ttAcc   = timetable(t_acc ,Acceleration , 'VariableNames',{'acc'});
ttGyro  = timetable(t_gyro,AngularVelocity,'VariableNames',{'gyro'});
ttMag   = timetable(t_mag ,MagneticField , 'VariableNames',{'mag'});
ttOrient= timetable(t_ori ,Orientation   ,'VariableNames',{'orient'});

% 时间对齐 & 重采样
tMin = max([ttAcc.Properties.RowTimes(1); ttGyro.Properties.RowTimes(1); ttMag.Properties.RowTimes(1); ttOrient.Properties.RowTimes(1)]);
tMax = min([ttAcc.Properties.RowTimes(end); ttGyro.Properties.RowTimes(end); ttMag.Properties.RowTimes(end); ttOrient.Properties.RowTimes(end)]);
tVec = (tMin : seconds(1/targetRate) : tMax).';
ttAcc   = retime(ttAcc  ,tVec,'linear');
ttGyro  = retime(ttGyro ,tVec,'linear');
ttMag   = retime(ttMag  ,tVec,'linear');
ttOrient= retime(ttOrient,tVec,'linear');

% 抽出 double
acc   = ttAcc.acc;
gyro  = ttGyro.gyro;
mag   = ttMag.mag;
orient= ttOrient.orient;

%% 坐标系对齐（与原 demo 一致）
Accelerometer = -[acc(:,2), acc(:,1), -acc(:,3)];
Gyroscope     = [gyro(:,2), gyro(:,1), -gyro(:,3)];
Magnetometer  = [mag(:,2),  mag(:,1), -mag(:,3)];

%% 真值 → 四元数
if size(orient,2)==3
    qTrue = quaternion([orient(:,3), -orient(:,2), orient(:,1)], ...
                       'eulerd', 'ZYX', 'frame');
else
    qTrue = quaternion(orient,'wxyz');
end

%% 安装角修正
Navg = 4;
q = ecompass(Accelerometer, Magnetometer);
qfix = meanrot(q(1:Navg)) ./ meanrot(qTrue(1:Navg));
Orientation = qfix * qTrue;

%% 滤波器自动调参
orientFilt = ahrsfilter('SampleRate', targetRate);
groundTruth = table(Orientation);
sensorData  = table(Accelerometer, Gyroscope, Magnetometer);
tc = tunerconfig('ahrsfilter','MaxIterations',30, ...
                 'ObjectiveLimit',1e-3,'Display','none');
tune(orientFilt, sensorData, groundTruth, tc);
reset(orientFilt);

%% 正式滤波
qEst = orientFilt(Accelerometer, Gyroscope, Magnetometer);

%% 误差 & 画图
numSamples = numel(Orientation);
t = (0:numSamples-1).' / targetRate;
d = rad2deg(dist(qEst, Orientation));

% 滤波欧拉角
figure
plot(t, eulerd(qEst, 'ZYX', 'frame'))
legend('yaw','pitch','roll','Location','bestoutside')
title('ahrsfilter Euler Angles')
ylabel('Degrees'); xlabel('Time (s)')

% 手机原始欧拉角（真值）
figure
plot(t, eulerd(Orientation, 'ZYX', 'frame'))
legend('yaw','pitch','roll','Location','bestoutside')
title('Phone Euler Angles (ground truth)')
ylabel('Degrees'); xlabel('Time (s)')

% 误差
figure
plot(t, d); hold on
rmsval = sqrt(mean(d.^2));
plot(t, repmat(rmsval,size(t)),'LineStyle','-.','Color','red');
text(t(1), rmsval+0.7, sprintf('RMS Error = %.2f°',rmsval),'Color','red')
title('Orientation Error'); ylabel('Degrees'); xlabel('Time (s)')

%% 3-D 动画（可选）
if exist('phoneMesh.stl','file')
    figure
    pp = poseplot("MeshFileName","phoneMesh.stl");
    for i = 1:numel(qEst)
        set(pp,"Orientation",qEst(i));
        drawnow limitrate
    end
end

disp('✅ 全部完成！');