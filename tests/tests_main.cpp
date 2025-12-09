#include <QtTest/QtTest>
#include <QApplication>
#include <QProcess>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "开始运行智能医疗管理系统所有测试...";

    int result = 0;

    // 定义要运行的测试列表
    QStringList tests = {
        "TcpClient_test",
        "UserSession_test",
        "JsonMessageBuilder_test",
        "StateManager_test",
        "DataManager_test"
    };

    for (const QString& test : tests) {
        qDebug() << "======================================";
        qDebug() << "运行测试:" << test;
        qDebug() << "======================================";

        QProcess process;
        process.setProgram("./" + test);
        process.setProcessChannelMode(QProcess::MergedChannels);

        process.start();
        process.waitForFinished();

        int exitCode = process.exitCode();
        QString output = process.readAll();

        if (!output.isEmpty()) {
            qDebug().noquote() << output;
        }

        if (exitCode != 0) {
            qDebug() << "测试" << test << "失败，退出码:" << exitCode;
            result |= exitCode;
        } else {
            qDebug() << "测试" << test << "通过";
        }

        qDebug() << "";
    }

    if (result == 0) {
        qDebug() << "所有测试都通过了！";
    } else {
        qDebug() << "有测试失败，总退出码:" << result;
    }

    return result;
}