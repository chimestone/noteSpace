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
m.MagneticSensorEnabled = true;
m.OrientationSensorEnabled = true;

disp('✅ 加速度计、陀螺仪、磁场、方向传感器已启用');

%% Step 3: 清空旧数据并开始记录
discardlogs(m);
m.Logging = 1;
disp('🎬 开始记录，请移动手机...');
recordTime = 10; % 记录时间（秒），可修改
pause(recordTime);
m.Logging = 0;
disp('🛑 停止记录');

%% Step 4: 读取数据
[Acceleration, t_acc] = accellog(m);
[AngularVelocity, t_gyro] = angvellog(m);
[Orientation, t_ori] = orientlog(m);
[MagneticField, t_mag] = magfieldlog(m);



%% Step 6: 保存数据
filename = ['MobileSensorData_' datestr(now,'yyyymmdd_HHMMSS') '.mat'];
save(filename,'Acceleration','AngularVelocity','Orientation','MagneticField','t_acc','t_gyro','t_ori','t_mag');
disp(['💾 数据已保存为: ' filename]);

disp('✅ 完成！');