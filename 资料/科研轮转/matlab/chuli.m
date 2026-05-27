%% ========== 主程序 ==========
function main()
    % 加载配置
    config = load_config();
    
    % 读取数据
    data = readtable(config.input_filename);
    
    % 时段统计
    [stats, people_data] = process_time_slots(data, config);
    
    % 数据可视化
    create_chart(stats, config);
    
    % 提取参与者姓名
    names = extract_participant_names(data, people_data, config);
    
    % 特定人群筛选
    filtered = filter_target_students(data, config);
    
    % 数据整合与导出
    merge_and_export_data(data, config);
end

%% ========== 配置模块 ==========
function config = load_config()
    % 文件路径
    config.input_filename = "D:\HuaweiMoveData\Users\钟磬\Desktop\资料整理\整合资料\百团大战\2025学生社团招新活动（收集结果）.xlsx";
    config.output_filename1 = "D:\HuaweiMoveData\Users\钟磬\Desktop\新建 XLSX 工作表.xlsx";
    config.output_filename2 = "D:\HuaweiMoveData\Users\钟磬\Desktop\新建 XLSX 工作表 (2).xlsx";
    
    % 数据列索引
    config.name_col = 3;
    config.age_col = 4;
    config.tel_col = 5;
    config.qq_col = 6;
    config.from_col = 7;
    config.inten_col = 8;
    config.friday_col = 9;
    config.saturday_col = 10;
    config.sunday_col = 11;
    
    % 时段标识符
    config.sat_slots = {"8:00-9:00［中操］布场", "9:00-10:30［中操］", "10:30-12:00［中操］", ...
                       "14:30-16:30［中操］", "16:30-18:00［中操］", "18:00-19:30［中操］收场", "20:30-22:30［西操］布场"};
    config.sun_slots = {"8:00-9:00［西操］布场", "9:00-10:30［西操］", "10:30-12:00［西操］", ...
                       "14:30-16:30［西操］", "16:30-18:00［西操］", "18:00-19:30［西操］收场"};
    
    % 筛选条件
    config.intention_keyword = "是";
    config.target_grades = {"24级", "23级"};
    config.target_department = "文艺拓展部";
    
    % 图表设置
    config.chart_title = "各时段参与人数统计";
    config.x_label = "时段";
    config.y_label = "人数";
    config.rotation_angle = 45;
end

%% ========== 时段统计模块 ==========
function [stats, people_data] = process_time_slots(data, config)
    % 周五参与统计
    stats.Fd = sum(~ismissing(data(:, config.friday_col)));
    people_data.FdPeople = ~ismissing(data(:, config.friday_col));
    
    % 周六时段统计
    for i = 1:length(config.sat_slots)
        people_mask = contains(string(data{:, config.saturday_col}), config.sat_slots{i});
        stats.(sprintf('Sd%d', i)) = sum(people_mask);
        people_data.(sprintf('Sd%dPeople', i)) = people_mask;
    end
    
    % 周日时段统计
    for i = 1:length(config.sun_slots)
        people_mask = contains(string(data{:, config.sunday_col}), config.sun_slots{i});
        stats.(sprintf('Td%d', i)) = sum(people_mask);
        people_data.(sprintf('Td%dPeople', i)) = people_mask;
    end
end

%% ========== 数据可视化模块 ==========
function create_chart(stats, config)
    % 准备图表数据
    values = [stats.Fd, stats.Sd1, stats.Sd2, stats.Sd3, stats.Sd4, stats.Sd5, stats.Sd6, stats.Sd7, ...
              stats.Td1, stats.Td2, stats.Td3, stats.Td4, stats.Td5, stats.Td6];
    labels = {'Fd', 'Sd1', 'Sd2', 'Sd3', 'Sd4', 'Sd5', 'Sd6', 'Sd7', 'Td1', 'Td2', 'Td3', 'Td4', 'Td5', 'Td6'};
    
    % 绘制柱状图
    figure;
    bar(values);
    set(gca, 'XTickLabel', labels);
    xlabel(config.x_label);
    ylabel(config.y_label);
    title(config.chart_title);
    xtickangle(config.rotation_angle);
end

%% ========== 参与者姓名提取模块 ==========
function names = extract_participant_names(data, people_data, config)
    name = data(:, config.name_col);
    
    % 各时段参与者姓名
    names.FdName = name{people_data.FdPeople, 1};
    
    for i = 1:length(config.sat_slots)
        names.(sprintf('Sd%dName', i)) = name{people_data.(sprintf('Sd%dPeople', i)), 1};
    end
    
    for i = 1:length(config.sun_slots)
        names.(sprintf('Td%dName', i)) = name{people_data.(sprintf('Td%dPeople', i)), 1};
    end
end

%% ========== 特定人群筛选模块 ==========
function filtered = filter_target_students(data, config)
    name = data(:, config.name_col);
    
    % 有意向的目标年级学生
    inten_mask = contains(string(data{:, config.inten_col}), config.intention_keyword);
    grade_mask = false(size(data, 1), 1);
    for i = 1:length(config.target_grades)
        grade_mask = grade_mask | contains(string(data{:, config.age_col}), config.target_grades{i});
    end
    filtered.intention_students = name{inten_mask & grade_mask, 1};
    
    % 特定部门且有周日安排的学生
    dept_mask = contains(string(data{:, config.from_col}), config.target_department);
    sunday_mask = ~ismissing(data(:, config.sunday_col));
    filtered.department_students = name{dept_mask & sunday_mask, 1};
end

%% ========== 数据整合导出模块 ==========
function merge_and_export_data(data, config)
    % 提取关键信息
    key_info = data(:, [config.name_col, config.tel_col, config.qq_col, config.from_col]);
    
    % 读取待整合文件
    base_data = readtable(config.output_filename1);
    
    % 数据匹配与合并
    [~, idx] = ismember(base_data{:, config.name_col}, key_info{:, 1});
    valid_idx = idx > 0;
    base_data{valid_idx, end+1:end+3} = key_info{idx(valid_idx), 2:4};
    
    % 导出结果
    writetable(base_data, config.output_filename2);
end

% 运行主程序
main();
