// tests/datablock/test_datablock.h
#pragma once

#include "datablocks/datablock.h"
#include "interfaces/conn/async_connection.h"
#include "interfaces/conn/sync_connection.h"

#include <QObject>

// Mock connection class for testing
class MockAsyncConnection : public Interface::AsyncConnection
{
public:
    MockAsyncConnection(QObject *parent = nullptr) : Interface::AsyncConnection(parent) { }
};

// Mock connection class for testing
class MockSyncConnection : public Interface::SyncConnection
{
public:
    MockSyncConnection(QObject *parent = nullptr) : Interface::SyncConnection(new MockAsyncConnection) { }

    Error::Msg reqBlockSync(int blockNum, DataTypes::DataBlockTypes type, void *block, int size)
    {
        // Simulate successful read
        return Error::Msg::NoError;
    }

    Error::Msg writeBlockSync(int blockNum, DataTypes::DataBlockTypes type, const void *block, int size)
    {
        // Simulate successful write
        return Error::Msg::NoError;
    }

    Error::Msg readS2FileSync(S2::FilesEnum file)
    {
        // Simulate successful read
        return Error::Msg::NoError;
    }
};

// Mock DataBlock subclass for testing
class TestableDataBlock : public DataBlock
{
public:
    TestableDataBlock(QObject *parent = nullptr) : DataBlock(parent) { }

    void setupValuesDesc() override;

    void setDefBlock() override;

    void specificUpdateWidget() override { }
    void specificUpdateFromWidget() override { }

private:
    float m_testFloat = 0.0f;
    quint32 m_testUint = 0u;
    float m_groupFloat1 = 0.0f;
    float m_groupFloat2 = 0.0f;
};

class TestDataBlock : public QObject
{
    Q_OBJECT

public:
    TestDataBlock();
    ~TestDataBlock();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testConstructor();
    void testSetBlock();
    void testSetup();
    void testWidgetCreation();
    void testUpdateWidget();
    void testUpdateFromWidget();
    void testReadBlockFromModule();
    void testWriteBlockToModule();
    void testButtonsUI();
    void testFileOperations();
    void testSignalsAndSlots();
    void testEdgeCases();

private:
    TestableDataBlock *m_dataBlock;
    MockSyncConnection *m_mockConnection;
    QString m_testUID;
};
