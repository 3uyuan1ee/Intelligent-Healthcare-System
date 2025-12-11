-- 智能医疗系统数据库初始化脚本
-- 创建数据库
CREATE DATABASE IF NOT EXISTS SmartMedical CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE SmartMedical;

-- 用户账户表
CREATE TABLE IF NOT EXISTS `account` (
  `username` VARCHAR(50) PRIMARY KEY,
  `type` ENUM('patient', 'doctor', 'admin') NOT NULL,
  `reverse` VARCHAR(255) NOT NULL COMMENT '密码'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 患者信息表
CREATE TABLE IF NOT EXISTS `patientInfo` (
  `username` VARCHAR(50) PRIMARY KEY,
  `name` VARCHAR(100) NOT NULL,
  `gender` ENUM('male', 'female') NOT NULL,
  `birthday` DATE NOT NULL,
  `id` VARCHAR(18) UNIQUE NOT NULL COMMENT '身份证号',
  `phoneNumber` VARCHAR(20) NOT NULL,
  `email` VARCHAR(100),
  FOREIGN KEY (`username`) REFERENCES `account`(`username`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 医生信息表
CREATE TABLE IF NOT EXISTS `doctorInfo` (
  `username` VARCHAR(50) PRIMARY KEY,
  `name` VARCHAR(100) NOT NULL,
  `id` VARCHAR(18) UNIQUE NOT NULL COMMENT '身份证号',
  `department` VARCHAR(100) NOT NULL COMMENT '科室',
  `cost` DECIMAL(10,2) NOT NULL DEFAULT 0.00 COMMENT '挂号费',
  `begin` TIME NOT NULL COMMENT '上班时间',
  `end` TIME NOT NULL COMMENT '下班时间',
  `limit` INT NOT NULL DEFAULT 20 COMMENT '每日预约上限',
  FOREIGN KEY (`username`) REFERENCES `account`(`username`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 预约记录表
CREATE TABLE IF NOT EXISTS `appointment` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `patientUsername` VARCHAR(50) NOT NULL,
  `doctorUsername` VARCHAR(50) NOT NULL,
  `date` DATE NOT NULL,
  `time` TIME NOT NULL,
  `cost` DECIMAL(10,2) NOT NULL,
  `status` ENUM('pending', 'confirmed', 'cancelled', 'completed') DEFAULT 'pending',
  FOREIGN KEY (`patientUsername`) REFERENCES `account`(`username`) ON DELETE CASCADE,
  FOREIGN KEY (`doctorUsername`) REFERENCES `account`(`username`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 病历表
CREATE TABLE IF NOT EXISTS `case` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `patientUsername` VARCHAR(50) NOT NULL,
  `doctorUsername` VARCHAR(50) NOT NULL,
  `date` DATE NOT NULL,
  `time` TIME NOT NULL,
  `main` TEXT COMMENT '主诉',
  `now` TEXT COMMENT '现病史',
  `past` TEXT COMMENT '既往史',
  `check` TEXT COMMENT '检查结果',
  `diagnose` TEXT COMMENT '诊断',
  FOREIGN KEY (`patientUsername`) REFERENCES `account`(`username`) ON DELETE CASCADE,
  FOREIGN KEY (`doctorUsername`) REFERENCES `account`(`username`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 医嘱表
CREATE TABLE IF NOT EXISTS `advice` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `patientUsername` VARCHAR(50) NOT NULL,
  `doctorUsername` VARCHAR(50) NOT NULL,
  `date` DATE NOT NULL,
  `time` TIME NOT NULL,
  `medicine` TEXT COMMENT '用药建议',
  `check` TEXT COMMENT '检查建议',
  `therapy` TEXT COMMENT '治疗建议',
  `care` TEXT COMMENT '护理建议',
  FOREIGN KEY (`patientUsername`) REFERENCES `account`(`username`) ON DELETE CASCADE,
  FOREIGN KEY (`doctorUsername`) REFERENCES `account`(`username`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 通知表
CREATE TABLE IF NOT EXISTS `notice` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `username` VARCHAR(50) NOT NULL,
  `type` ENUM('appointment', 'case', 'system', 'reminder') NOT NULL,
  `message` TEXT NOT NULL,
  `time` DATETIME DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (`username`) REFERENCES `account`(`username`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 医生工作安排表
CREATE TABLE IF NOT EXISTS `work` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `username` VARCHAR(50) NOT NULL,
  `date` DATE NOT NULL,
  `status` ENUM('available', 'busy', 'off') DEFAULT 'available',
  FOREIGN KEY (`username`) REFERENCES `account`(`username`) ON DELETE CASCADE,
  UNIQUE KEY unique_work (`username`, `date`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 健康问卷表
CREATE TABLE IF NOT EXISTS `question` (
  `id` INT AUTO_INCREMENT PRIMARY KEY,
  `name` VARCHAR(100) NOT NULL,
  `gender` ENUM('male', 'female') NOT NULL,
  `age` INT NOT NULL,
  `height` DECIMAL(5,2) COMMENT '身高(cm)',
  `weight` DECIMAL(5,2) COMMENT '体重(kg)',
  `heart` INT COMMENT '心率',
  `pressure` VARCHAR(20) COMMENT '血压',
  `lung` INT COMMENT '肺活量',
  `createTime` DATETIME DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 插入测试数据
-- 管理员账户
INSERT IGNORE INTO `account` (`username`, `type`, `reverse`) VALUES ('admin', 'admin', 'admin123');

-- 测试患者账户
INSERT IGNORE INTO `account` (`username`, `type`, `reverse`) VALUES
('patient1', 'patient', '123456'),
('patient2', 'patient', '123456');

INSERT IGNORE INTO `patientInfo` (`username`, `name`, `gender`, `birthday`, `id`, `phoneNumber`, `email`) VALUES
('patient1', '张三', 'male', '1990-01-01', '110101199001011234', '13800138000', 'zhangsan@email.com'),
('patient2', '李四', 'female', '1995-05-15', '110101199505154321', '13800138001', 'lisi@email.com');

-- 测试医生账户
INSERT IGNORE INTO `account` (`username`, `type`, `reverse`) VALUES
('doctor1', 'doctor', '123456'),
('doctor2', 'doctor', '123456');

INSERT IGNORE INTO `doctorInfo` (`username`, `name`, `id`, `department`, `cost`, `begin`, `end`, `limit`) VALUES
('doctor1', '王医生', '110101198001011234', '内科', 50.00, '08:00:00', '17:00:00', 20),
('doctor2', '李医生', '110101198002022345', '外科', 80.00, '09:00:00', '18:00:00', 15);

-- 插入一些示例预约记录
INSERT IGNORE INTO `appointment` (`patientUsername`, `doctorUsername`, `date`, `time`, `cost`, `status`) VALUES
('patient1', 'doctor1', CURDATE(), '09:00:00', 50.00, 'confirmed'),
('patient2', 'doctor2', CURDATE() + INTERVAL 1 DAY, '10:00:00', 80.00, 'pending');

-- 创建索引提高查询性能
CREATE INDEX idx_appointment_doctor_date ON `appointment`(`doctorUsername`, `date`);
CREATE INDEX idx_appointment_patient_date ON `appointment`(`patientUsername`, `date`);
CREATE INDEX idx_case_patient_doctor ON `case`(`patientUsername`, `doctorUsername`);
CREATE INDEX idx_advice_patient_doctor ON `advice`(`patientUsername`, `doctorUsername`);
CREATE INDEX idx_notice_username_time ON `notice`(`username`, `time`);
CREATE INDEX idx_work_username_date ON `work`(`username`, `date`);