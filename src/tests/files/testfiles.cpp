#include "testfiles.h"

#include "../../avtuk/oscmanager.h"
#include "../../avtuk/swjmanager.h"
#include "../../widgets/wd_func.h"

TestFiles::TestFiles(QObject *parent) : QObject(parent)
{
}

void TestFiles::testOsc()
{
    OscManager oscManager;
    auto fileVector = oscManager.loadFromFile(":/osc/1021.osc");
    QCOMPARE(fileVector.size(), 2);
    for (auto &item : fileVector)
    {
        std::visit(overloaded {
                       [](S2DataTypes::OscHeader &record) {
                           QCOMPARE(record.len, 498);
                           QCOMPARE(record.step, 20.0);
                           QCOMPARE(record.time, 5832358505620324140);
                       },                                //
                       [](auto &&arg) { Q_UNUSED(arg) }, //
                       [](std::unique_ptr<TrendViewModel> &model) {
                           QCOMPARE(model->MainPoints.size(), 498);
                           QCOMPARE(model->Len, 498);
                       } //
                   },
            item);
    }
}

void TestFiles::testSwj()
{
    SwjManager swjManager;
    OscManager oscManager;

    auto fileVector = oscManager.loadFromFile(":/swj/356_2.swj");
    {
        auto oscVector = swjManager.loadFromFile(":/swj/356_2.swj");
        std::move(oscVector.begin(), oscVector.end(), std::back_inserter(fileVector));
    }
    QCOMPARE(fileVector.size(), 3);
    auto fileVector2 = oscManager.loadFromFile(":/swj/1337.swj");
    {
        auto oscVector = swjManager.loadFromFile(":/swj/1337.swj");
        std::move(oscVector.begin(), oscVector.end(), std::back_inserter(fileVector2));
    }
    QCOMPARE(fileVector2.size(), 3);

    auto checkModel = [](SwjModel &model) {
        auto commonModel = model.commonModel.get();
        QVERIFY(commonModel);
        QCOMPARE(commonModel->columnCount(), 2);
        QCOMPARE(commonModel->rowCount(), 10);
        auto detailModel = model.detailModel.get();
        QVERIFY(detailModel);
        QCOMPARE(detailModel->columnCount(), 4);
        QCOMPARE(detailModel->rowCount(), 11);
    };

    for (auto &item : fileVector)
    {
        std::visit(overloaded {
                       [](S2DataTypes::OscHeader &record) {
                           QCOMPARE(record.len, 3840);
                           QCOMPARE(record.step, 0.0792380943894);
                           QCOMPARE(record.time, 6665603355934695424);
                       },          //
                       checkModel, //
                       [](std::unique_ptr<TrendViewModel> &model) {
                           QCOMPARE(model->MainPoints.size(), 3840);
                           QCOMPARE(model->Len, 3840);
                       } //
                   },
            item);
    }
    for (auto &item : fileVector2)
    {
        std::visit(overloaded {
                       [](S2DataTypes::OscHeader &record) {
                           QCOMPARE(record.len, 2098);
                           QCOMPARE(record.step, 0.0792380943894);
                           QCOMPARE(record.time, 6904198229938224128);
                       },          //
                       checkModel, //
                       [](std::unique_ptr<TrendViewModel> &model) {
                           QCOMPARE(model->MainPoints.size(), 2098);
                           QCOMPARE(model->Len, 2098);
                       } //
                   },
            item);
    }
}

QTEST_GUILESS_MAIN(TestFiles)
