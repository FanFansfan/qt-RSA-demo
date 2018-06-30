#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QDebug>
#include <Rsa.h>
#include <gmpxx.h>

int main(int argc, char *argv[])
{
    unsigned int t;
    time((time_t*)&t);
    srand(t);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QTranslator translator;
    translator.load(":/zh_CN.qm");
    app.installTranslator(&translator);

    qRegisterMetaType<mpz_class>("mpz_class");
    //qmlRegisterType<QByteArray>("QByteArray");
    //qmlRegisterType<QByteArray>("QByteArray", 1, 0, "QByteArray");

    Rsa rsa;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("rsa", &rsa);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
