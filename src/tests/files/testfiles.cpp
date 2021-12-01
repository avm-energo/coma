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
                           QCOMPARE(model->mainPoints().size(), 498);
                           QCOMPARE(model->length(), 498);
                       } //
                   },
            item);
    }
}

void TestFiles::testOsc85()
{
    OscManager oscManager;
    auto fileVector = oscManager.loadFromFile(":/osc/37353239303451123f0033-8585-2005-6730917119207093248.osc");
    QCOMPARE(fileVector.size(), 2);
    for (auto &item : fileVector)
    {
        std::visit(overloaded {
                       [](S2DataTypes::OscHeader &record) {
                           QCOMPARE(record.len, 1947);
                           QCOMPARE(record.step, 0.0792380943894);
                           QCOMPARE(record.time, 6730917119207093248);
                       },                                //
                       [](auto &&arg) { Q_UNUSED(arg) }, //
                       [](std::unique_ptr<TrendViewModel> &model) {
                           QCOMPARE(model->mainPoints().size(), 1947);
                           QCOMPARE(model->length(), 1947);
                       } //
                   },
            item);
    }
}

void TestFiles::testSwj()
{
    SwjManager swjManager;
    OscManager oscManager;

    constexpr char path[] = ":/swj/37353239303451123f0033-8585-1110420049-6730912496292894630.swj";

    auto fileVector = oscManager.loadFromFile(path);
    {
        auto oscVector = swjManager.loadFromFile(path);
        std::move(oscVector.begin(), oscVector.end(), std::back_inserter(fileVector));
    }
    QCOMPARE(fileVector.size(), 3);

    auto checkModel = [](SwjModel &model) {
        auto commonModel = model.commonModel.get();
        QVERIFY(commonModel);
        QCOMPARE(commonModel->columnCount(), 2);
        QCOMPARE(commonModel->rowCount(), 9);
        auto detailModel = model.detailModel.get();
        QVERIFY(detailModel);
        QCOMPARE(detailModel->columnCount(), 4);
        QCOMPARE(detailModel->rowCount(), 11);
    };

    for (auto &item : fileVector)
    {
        std::visit(overloaded {
                       [](S2DataTypes::OscHeader &record) {
                           QCOMPARE(record.len, 4096);
                           QCOMPARE(record.step, 0.0792380943894);
                           QCOMPARE(record.time, 6730912496293968896);
                       },          //
                       checkModel, //
                       [](std::unique_ptr<TrendViewModel> &model) {
                           QCOMPARE(model->mainPoints().size(), 4096);
                           QCOMPARE(model->length(), 4096);
                       } //
                   },
            item);
    }
}

QTEST_GUILESS_MAIN(TestFiles)
