// tests/datablock/tst_datablock.cpp
#include "test_datablock.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QSignalSpy>
#include <QtTest>

void TestableDataBlock::setupValuesDesc()
{
    // Setup some test values
    addNewValue("test_float", "Test float value", &m_testFloat, 3);
    addNewValue("test_uint", "Test uint value", &m_testUint, 0);

    ValueGroupStr group;
    group.groupDesc = "Test Group";
    group.values.append({ "group_float1", "First group float", "value[0]", &m_groupFloat1, 2 });
    group.values.append({ "group_float2", "Second group float", "value[1]", &m_groupFloat2, 2 });
    m_valuesDesc.append(group);
}

void TestableDataBlock::setDefBlock()
{
    m_testFloat = 10.5f;
    m_testUint = 42u;
    m_groupFloat1 = 1.23f;
    m_groupFloat2 = 4.56f;
}

TestDataBlock::TestDataBlock()
{
    m_dataBlock = nullptr;
    m_mockConnection = nullptr;
    m_testUID = "TEST_DEVICE_001";
}

TestDataBlock::~TestDataBlock()
{
    delete m_dataBlock;
    delete m_mockConnection;
}

void TestDataBlock::initTestCase()
{
    m_mockConnection = new MockSyncConnection();
    m_dataBlock = new TestableDataBlock();
}

void TestDataBlock::cleanupTestCase()
{
    // Cleanup if needed
}

void TestDataBlock::testConstructor()
{
    QVERIFY(m_dataBlock != nullptr);
    QCOMPARE(m_dataBlock->block().blocknum, 0);
    QCOMPARE(m_dataBlock->block().caption, QString(""));
    QCOMPARE(m_dataBlock->block().blocktype, DataTypes::DataBlockTypes::BacBlock);
    QCOMPARE(m_dataBlock->block().block, nullptr);
    QCOMPARE(m_dataBlock->block().blocksize, 0);
    QCOMPARE(m_dataBlock->block().bottomButtonsVisible, true);
}

void TestDataBlock::testSetBlock()
{
    DataBlock::BlockStruct blockStruct;
    blockStruct.blocknum = 5;
    blockStruct.caption = "Test Block";
    blockStruct.blocktype = DataTypes::DataBlockTypes::BciBlock;
    blockStruct.block = nullptr;
    blockStruct.blocksize = 1024;
    blockStruct.bottomButtonsVisible = false;

    m_dataBlock->setBlock(blockStruct);

    auto returnedBlock = m_dataBlock->block();
    QCOMPARE(returnedBlock.blocknum, 5);
    QCOMPARE(returnedBlock.caption, QString("Test Block"));
    QCOMPARE(returnedBlock.blocktype, DataTypes::DataBlockTypes::BciBlock);
    QCOMPARE(returnedBlock.blocksize, 1024);
    QCOMPARE(returnedBlock.bottomButtonsVisible, false);
}

void TestDataBlock::testSetup()
{
    m_dataBlock->setup(m_testUID, m_mockConnection);

    QCOMPARE(m_dataBlock->block().blocknum, 0);
    QVERIFY(!m_dataBlock->block().caption.isEmpty());
    QCOMPARE(m_dataBlock->block().blocktype, DataTypes::DataBlockTypes::BacBlock);
    QCOMPARE(m_dataBlock->m_deviceUID, m_testUID);
    QVERIFY(m_dataBlock->m_conn == m_mockConnection);
}

void TestDataBlock::testWidgetCreation()
{
    // Test widget creation
    QWidget *widget = m_dataBlock->widget();
    QVERIFY(widget != nullptr);

    // Test that widget is properly set
    QVERIFY(m_dataBlock->m_widgetIsSet);

    // Test widget with buttons hidden
    QWidget *widgetNoButtons = m_dataBlock->widget(false);
    QVERIFY(widgetNoButtons != nullptr);
}

void TestDataBlock::testUpdateWidget()
{
    // Set up test values
    float testFloat = 15.7f;
    quint32 testUint = 100u;

    // Update the block values directly
    m_dataBlock->updateGeneralWidget();

    // Verify that widget is created and can be updated
    QVERIFY(m_dataBlock->m_widget != nullptr);
}

void TestDataBlock::testUpdateFromWidget()
{
    // This would require a real widget with QLineEdit fields
    // For now, just test that it doesn't crash
    m_dataBlock->updateFromWidget();
}

void TestDataBlock::testReadBlockFromModule()
{
    QSignalSpy spy(m_dataBlock, &DataBlock::destroyed);

    // Test reading from module (should not crash)
    m_dataBlock->readBlockFromModule();

    // Verify no signals were emitted
    QCOMPARE(spy.count(), 0);
}

void TestDataBlock::testWriteBlockToModule()
{
    // Test writing to module (should not crash)
    Error::Msg result = m_dataBlock->writeBlockToModule(false);
    QCOMPARE(result, Error::Msg::NoError);
}

void TestDataBlock::testButtonsUI()
{
    QWidget *buttonsWidget = m_dataBlock->blockButtonsUI();
    QVERIFY(buttonsWidget != nullptr);

    // Test that the buttons widget is created only once
    QWidget *buttonsWidget2 = m_dataBlock->blockButtonsUI();
    QCOMPARE(buttonsWidget, buttonsWidget2);
}

void TestDataBlock::testFileOperations()
{
    // Test save to file (should not crash)
    Error::Msg result = m_dataBlock->saveToFile();
    QCOMPARE(result, Error::Msg::NoError);

    // Test read from file (should not crash)
    Error::Msg readResult = m_dataBlock->readFromFile();
    QCOMPARE(readResult, Error::Msg::NoError);
}

void TestDataBlock::testSignalsAndSlots()
{
    // Test that slots work correctly
    QSignalSpy spy1(m_dataBlock, &DataBlock::destroyed);

    // Call slot functions
    m_dataBlock->setDefBlockAndUpdate();
    m_dataBlock->readAndUpdate();

    // Verify no signals were emitted (since we're not destroying the object)
    QCOMPARE(spy1.count(), 0);
}

void TestDataBlock::testEdgeCases()
{
    // Test with null connection
    DataBlock::BlockStruct blockStruct;
    blockStruct.blocknum = 1;
    blockStruct.caption = "Edge Case Test";
    blockStruct.blocktype = DataTypes::DataBlockTypes::BacBlock;
    blockStruct.block = nullptr;
    blockStruct.blocksize = 0;
    blockStruct.bottomButtonsVisible = true;

    m_dataBlock->setBlock(blockStruct);

    // Test update with no widget set
    m_dataBlock->updateGeneralWidget();

    // Test read from module with null connection (should not crash)
    m_dataBlock->readBlockFromModule();

    // Test update from widget with no widget set
    m_dataBlock->updateFromWidget();

    // Test that the copy widget is properly handled
    QWidget *copyWidget = m_dataBlock->createCopy(nullptr);
    QVERIFY(copyWidget != nullptr);

    m_dataBlock->deleteWidgetCopy();

    // Test with empty UID
    m_dataBlock->setup("", m_mockConnection);
    QString filename = m_dataBlock->cpuIDFilenameStr();
    QVERIFY(!filename.isEmpty());
}

QTEST_APPLESS_MAIN(TestDataBlock)
