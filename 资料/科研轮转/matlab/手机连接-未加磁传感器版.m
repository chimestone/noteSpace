clear; clc; close all;

%% Step 1: 创建 mobiledev 对象
disp('👉 正在创建 mobiledev 对象...');
m = mobiledev;


% 检查手机是否已连接
if ~m.Connected
    error(['⚠️ 手机未连接！\n',...
           '请在 MATLAB Mobile → Sensors 页面中点击右上角 ⚙️ 设置\n',...
           '选择 "Connect to MATLAB"，输入此电脑的 IP 地址。']);
else
    disp(['✅ 手机已连接：' m.Device]);
end

%% Step 2: 启用传感器
m.AccelerationSensorEnabled = true;
m.AngularVelocitySensorEnabled = true;
m.OrientationSensorEnabled = true;

disp('✅ 加速度计、陀螺仪、方向传感器已启用');

%% Step 3: 清空旧数据并开始记录
discardlogs(m);
m.Logging = 1;
disp('🎬 开始记录，请移动手机...');
recordTime = 10; % 记录时间（秒），可修改
pause(recordTime);
m.Logging = 0;
disp('🛑 停止记录');

%% Step 4: 读取数据
[acc, t_acc] = accellog(m);
[gyro, t_gyro] = angvellog(m);
[ori, t_ori] = orientlog(m);

%% Step 5: 绘制传感器数据
figure('Name','MATLAB Mobile Sensor Data','Position',[100 100 900 700]);

subplot(3,1,1);
plot(t_acc, acc);
title('Acceleration (m/s^2)'); xlabel('Time'); ylabel('Accel');
legend('X','Y','Z'); grid on;

subplot(3,1,2);
plot(t_gyro, gyro);
title('Angular Velocity (rad/s)'); xlabel('Time'); ylabel('Angular Vel');
legend('X','Y','Z'); grid on;

subplot(3,1,3);
plot(t_ori, ori);
title('Orientation (quaternion)'); xlabel('Time'); ylabel('Quaternion');
legend('q1','q2','q3','q4'); grid on;

%% Step 6: 保存数据
filename = ['MobileSensorData_' datestr(now,'yyyymmdd_HHMMSS') '.mat'];
save(filename,'acc','gyro','ori','t_acc','t_gyro','t_ori');
disp(['💾 数据已保存为: ' filename]);

disp('✅ 完成！');